"""
Pixel-level fire / heat anomaly monitor.

Algorithm
---------
Each frame:

1.  Ambient drift check — if the rolling scene *average* climbs well
    above its historical baseline, confirm with the *median* (robust to
    isolated hot pixels), then compare max vs. median.  Emit
    AMBIENT_HIGH if both checks pass.

2.  Hotspot spike detection — if (max − avg) exceeds its rolling
    baseline by `spike_factor`, the scene has a new localised anomaly.
    Find the hottest pixel and run a *spatial coherence* test: are
    enough of its 3×3 neighbours also anomalously hot?  If yes, create
    a CANDIDATE tracker anchored to that pixel.

3.  Tracker updates — for each active CANDIDATE:
      • Compare the neighbourhood's current max to the stored baseline.
      • Require `warning_frames` consecutive frames of continued rise
        above `warning_rise_c` to transition to WARNING.
      • Require a further `alarm_frames` consecutive frames above
        `alarm_rise_c` to escalate to ALARM.
      • If temperatures drop back within `drop_tolerance_c` of the
        baseline, dismiss the tracker immediately.
      • Expire stale CANDIDATEs that never escalated after
        `max_candidate_age_s` seconds.

False-positive resistance
-------------------------
- Person walking through: the tracker is spatially anchored.  As the
  person moves on, the tracked pixels cool → dismissed before WARNING.
- Bullet / muzzle flash: very brief (< frame period at 8 Hz) and cools
  rapidly.  The drop check and consecutive-frame requirement both filter
  these before WARNING is reached.
- Brief ambient spikes: the ambient check compares against a rolling
  window, so a single hot frame does not trigger.

Threading
---------
`start_monitor_process()` runs the loop in a separate OS process via
`multiprocessing`, giving true parallelism that cannot block the UI or
other camera threads.  `start_monitor_thread()` is available as a
lighter fallback (sufficient on most platforms since numpy and serial
I/O both release the GIL).
"""

from __future__ import annotations

import logging
import multiprocessing
import queue
import threading
import time
from collections import deque
from dataclasses import dataclass, field
from enum import Enum
from typing import Any

import numpy as np

from .frame_source import COLS, ROWS, FrameSource, MLX90640FrameSource, OptrisFrameSource

LOG = logging.getLogger("fire_monitor")


# ---------------------------------------------------------------------------
# Public alert types
# ---------------------------------------------------------------------------

class AlertLevel(str, Enum):
    AMBIENT_HIGH = "ambient_high"   # whole-scene average anomaly
    WARNING      = "warning"        # localised hotspot — watch closely
    ALARM        = "alarm"          # confirmed, still rising — act now


@dataclass
class FireAlert:
    level: AlertLevel
    timestamp: float                # time.monotonic()
    pixel_row: int | None           # None for AMBIENT_HIGH
    pixel_col: int | None
    baseline_temp_c: float
    current_temp_c: float
    rise_c: float
    message: str


# ---------------------------------------------------------------------------
# Configuration
# ---------------------------------------------------------------------------

@dataclass
class FireMonitorConfig:
    # --- Spike detection ---
    # Raise a candidate when temp_diff > rolling_baseline * spike_factor
    spike_factor: float = 1.6
    # Ignore spikes smaller than this (°C); filters very uniform scenes
    min_temp_diff_c: float = 3.0
    # Frames to average for the "normal" temp_diff baseline
    baseline_window: int = 30

    # --- Spatial coherence ---
    # A neighbour pixel is "hot" if its temp > frame_avg + threshold * std
    neighbour_threshold: float = 1.5
    # Minimum hot neighbours (of up to 8) to accept a candidate
    min_hot_neighbours: int = 2

    # --- Rising-trend thresholds (vs. stored baseline) ---
    warning_rise_c: float = 1.5     # °C above baseline to start warning clock
    alarm_rise_c:   float = 3.0     # °C above baseline to start alarm clock
    # How long (seconds) the neighbourhood must stay above each threshold
    # before an alert is issued.  Set these to your desired observation window.
    # e.g. warning_window_s=300, alarm_window_s=600 → 5 min warn, 10 min alarm.
    warning_window_s: float = 120.0   # default: 2 minutes
    alarm_window_s:   float = 300.0   # default: 5 minutes
    # Dismiss if neighbourhood max drops this far below baseline max
    drop_tolerance_c: float = 0.5
    # Expire a CANDIDATE that has never crossed warning_rise_c after this long.
    # Set to 0.0 to disable expiry (not recommended for very long windows).
    max_candidate_age_s: float = 3600.0

    # --- Ambient drift check ---
    ambient_window: int = 60            # frames for rolling ambient baseline
    ambient_spike_c: float = 4.0        # scene-avg rise that triggers the check
    # Fraction of ambient_spike_c that the median must also exceed
    ambient_median_fraction: float = 0.4

    # --- Algorithm 3: absolute temperature threshold ---
    absolute_threshold_c: float = 30.0   # raise candidate when any pixel exceeds this

    # --- Rate limiting ---
    min_repeat_interval_s: float = 10.0  # minimum gap between repeated alerts


# ---------------------------------------------------------------------------
# Internal hotspot tracker
# ---------------------------------------------------------------------------

class _Phase(str, Enum):
    CANDIDATE = "candidate"
    WARNING   = "warning"
    ALARM     = "alarm"


class _HotspotTracker:
    """Tracks one spatially-anchored region of interest across frames."""

    def __init__(
        self,
        row: int,
        col: int,
        baseline_neigh: np.ndarray,
        baseline_diff: float,
        created_at: float,
    ) -> None:
        self.row = row
        self.col = col
        self.baseline_neigh = baseline_neigh.copy()
        self.baseline_diff = baseline_diff
        self.baseline_max = float(baseline_neigh.max())
        self.created_at = created_at

        self.state = _Phase.CANDIDATE
        self.is_active = True

        # Time-based clocks: record when temp first exceeded each threshold.
        # Reset to None if temp drops back below the threshold.
        self._first_above_warning_at: float | None = None
        self._first_above_alarm_at:   float | None = None
        self._last_alert_at: float = 0.0

    def _neighbourhood(self, frame: np.ndarray) -> np.ndarray:
        r0, r1 = max(0, self.row - 1), min(ROWS, self.row + 2)
        c0, c1 = max(0, self.col - 1), min(COLS, self.col + 2)
        return frame[r0:r1, c0:c1]

    def update(
        self,
        frame: np.ndarray,
        temp_diff: float,
        config: FireMonitorConfig,
        now: float,
    ) -> FireAlert | None:
        neigh = self._neighbourhood(frame)
        cur_max = float(neigh.max())
        rise = cur_max - self.baseline_max

        # Dismiss: cooled back toward baseline
        if rise < -config.drop_tolerance_c:
            self.is_active = False
            return None

        # Dismiss: stale candidate that never crossed the warning threshold.
        # Once the warning clock has started, keep tracking indefinitely.
        if (self.state is _Phase.CANDIDATE
                and self._first_above_warning_at is None
                and config.max_candidate_age_s > 0
                and now - self.created_at > config.max_candidate_age_s):
            self.is_active = False
            return None

        # --- Update time-based clocks ---
        # Warning clock: runs while rise >= warning_rise_c; resets on cooling.
        if rise >= config.warning_rise_c:
            if self._first_above_warning_at is None:
                self._first_above_warning_at = now
        else:
            self._first_above_warning_at = None

        # Alarm clock: runs while rise >= alarm_rise_c; resets if it drops below.
        if rise >= config.alarm_rise_c:
            if self._first_above_alarm_at is None:
                self._first_above_alarm_at = now
        else:
            self._first_above_alarm_at = None

        # --- Phase transitions (time-based) ---
        # Escalate only when temp has been continuously above the threshold
        # for the full configured window without cooling back down.
        if self.state is _Phase.CANDIDATE or self.state is _Phase.WARNING:
            if (self._first_above_alarm_at is not None
                    and now - self._first_above_alarm_at >= config.alarm_window_s):
                self.state = _Phase.ALARM
            elif (self.state is _Phase.CANDIDATE
                    and self._first_above_warning_at is not None
                    and now - self._first_above_warning_at >= config.warning_window_s):
                self.state = _Phase.WARNING

        # Emit if alertable and rate limit allows
        if self.state in (_Phase.WARNING, _Phase.ALARM):
            if now - self._last_alert_at >= config.min_repeat_interval_s:
                self._last_alert_at = now
                level = (AlertLevel.WARNING
                         if self.state is _Phase.WARNING
                         else AlertLevel.ALARM)
                return FireAlert(
                    level=level,
                    timestamp=now,
                    pixel_row=self.row,
                    pixel_col=self.col,
                    baseline_temp_c=self.baseline_max,
                    current_temp_c=cur_max,
                    rise_c=rise,
                    message=(
                        f"{level.value.upper()} pixel ({self.row},{self.col}): "
                        f"{cur_max:.1f}°C (+{rise:.1f}°C from "
                        f"baseline {self.baseline_max:.1f}°C)"
                    ),
                )

        return None


# ---------------------------------------------------------------------------
# Algorithm 3 — absolute temperature threshold
# ---------------------------------------------------------------------------

class AbsoluteThresholdFrameAnalyzer:
    """
    Raises a CANDIDATE when any pixel exceeds `config.absolute_threshold_c`
    (default 30 °C).  From that point the tracker behaves identically to the
    spike-based FrameAnalyzer — WARNING/ALARM are issued only when the
    hotspot remains above the threshold by warning_rise_c / alarm_rise_c
    continuously for the configured time windows.
    """

    def __init__(self, config: FireMonitorConfig) -> None:
        self.config = config
        self._trackers: list[_HotspotTracker] = []
        self._avg_history: deque[float] = deque(maxlen=config.ambient_window)
        self._last_ambient_alert_at: float = 0.0

    def process(self, frame: np.ndarray, now: float) -> list[FireAlert]:
        """Analyse one frame; return any new/repeated alerts (may be empty)."""
        alerts: list[FireAlert] = []
        cfg = self.config

        frame_avg = float(np.mean(frame))
        frame_max = float(np.max(frame))
        threshold = cfg.absolute_threshold_c

        # --- Ambient drift check (same as FrameAnalyzer) ---
        half_ambient = cfg.ambient_window // 2
        if len(self._avg_history) >= half_ambient:
            expected_avg = float(np.mean(self._avg_history))
            if frame_avg > expected_avg + cfg.ambient_spike_c:
                frame_median = float(np.median(frame))
                median_also_high = (
                    frame_median > expected_avg + cfg.ambient_spike_c * cfg.ambient_median_fraction
                )
                rate_ok = now - self._last_ambient_alert_at >= cfg.min_repeat_interval_s * 3
                if median_also_high and rate_ok:
                    self._last_ambient_alert_at = now
                    alerts.append(FireAlert(
                        level=AlertLevel.AMBIENT_HIGH,
                        timestamp=now,
                        pixel_row=None,
                        pixel_col=None,
                        baseline_temp_c=expected_avg,
                        current_temp_c=frame_avg,
                        rise_c=frame_avg - expected_avg,
                        message=(
                            f"AMBIENT_HIGH: scene avg {frame_avg:.1f}°C "
                            f"(+{frame_avg - expected_avg:.1f}°C above expected "
                            f"{expected_avg:.1f}°C), median={frame_median:.1f}°C, "
                            f"max={frame_max:.1f}°C"
                        ),
                    ))

        # --- Candidate creation: any pixel above absolute threshold ---
        if frame_max > threshold:
            r_max, c_max = divmod(int(np.argmax(frame)), COLS)
            if not self._near_tracker(r_max, c_max):
                r0, r1 = max(0, r_max - 1), min(ROWS, r_max + 2)
                c0, c1 = max(0, c_max - 1), min(COLS, c_max + 2)
                neigh = frame[r0:r1, c0:c1]
                tracker = _HotspotTracker(
                    row=r_max, col=c_max,
                    baseline_neigh=neigh,
                    baseline_diff=0.0,
                    created_at=now,
                )
                # Rise is measured from the threshold, not the neighbourhood max,
                # so WARNING/ALARM require sustained temp above threshold + rise_c.
                tracker.baseline_max = threshold
                self._trackers.append(tracker)
                LOG.debug(
                    "Absolute candidate at (%d,%d): %.1f°C > threshold %.1f°C",
                    r_max, c_max, frame_max, threshold,
                )

        # --- Update trackers ---
        surviving: list[_HotspotTracker] = []
        for tracker in self._trackers:
            alert = tracker.update(frame, 0.0, cfg, now)
            if alert:
                alerts.append(alert)
            if tracker.is_active:
                surviving.append(tracker)
        self._trackers = surviving

        # --- Update ambient history ---
        if len(self._avg_history) < self._avg_history.maxlen:
            self._avg_history.append(frame_avg)

        return alerts

    def _near_tracker(self, row: int, col: int, radius: int = 2) -> bool:
        return any(
            abs(t.row - row) <= radius and abs(t.col - col) <= radius
            for t in self._trackers
        )

    @property
    def active_tracker_count(self) -> int:
        return len(self._trackers)

    @property
    def active_trackers(self) -> list[tuple[int, int, str]]:
        return [(t.row, t.col, t.state.value) for t in self._trackers]


# ---------------------------------------------------------------------------
# Frame analyser — pure algorithm, no I/O
# ---------------------------------------------------------------------------

class FrameAnalyzer:
    """Stateful frame processor.  Call process() once per incoming frame."""

    def __init__(self, config: FireMonitorConfig) -> None:
        self.config = config
        self._diff_history: deque[float] = deque(maxlen=config.baseline_window)
        self._avg_history:  deque[float] = deque(maxlen=config.ambient_window)
        self._trackers: list[_HotspotTracker] = []
        self._last_ambient_alert_at: float = 0.0

    def process(self, frame: np.ndarray, now: float) -> list[FireAlert]:
        """Analyse one frame; return any new/repeated alerts (may be empty)."""
        alerts: list[FireAlert] = []
        cfg = self.config

        frame_avg = float(np.mean(frame))
        frame_max = float(np.max(frame))
        frame_std = float(np.std(frame)) + 1e-6   # avoid division by zero
        temp_diff = frame_max - frame_avg

        # ------------------------------------------------------------------
        # 1. Ambient drift check
        # ------------------------------------------------------------------
        half_ambient = cfg.ambient_window // 2
        if len(self._avg_history) >= half_ambient:
            expected_avg = float(np.mean(self._avg_history))
            if frame_avg > expected_avg + cfg.ambient_spike_c:
                frame_median = float(np.median(frame))
                median_also_high = (
                    frame_median > expected_avg + cfg.ambient_spike_c * cfg.ambient_median_fraction
                )
                rate_ok = now - self._last_ambient_alert_at >= cfg.min_repeat_interval_s * 3
                if median_also_high and rate_ok:
                    self._last_ambient_alert_at = now
                    alerts.append(FireAlert(
                        level=AlertLevel.AMBIENT_HIGH,
                        timestamp=now,
                        pixel_row=None,
                        pixel_col=None,
                        baseline_temp_c=expected_avg,
                        current_temp_c=frame_avg,
                        rise_c=frame_avg - expected_avg,
                        message=(
                            f"AMBIENT_HIGH: scene avg {frame_avg:.1f}°C "
                            f"(+{frame_avg - expected_avg:.1f}°C above expected "
                            f"{expected_avg:.1f}°C), median={frame_median:.1f}°C, "
                            f"max={frame_max:.1f}°C"
                        ),
                    ))

        # ------------------------------------------------------------------
        # 2. Hotspot spike detection → new candidate
        # ------------------------------------------------------------------
        half_baseline = cfg.baseline_window // 2
        if len(self._diff_history) >= half_baseline and temp_diff >= cfg.min_temp_diff_c:
            baseline_diff = float(np.mean(self._diff_history))
            if temp_diff > baseline_diff * cfg.spike_factor:
                r_max, c_max = divmod(int(np.argmax(frame)), COLS)
                if not self._near_tracker(r_max, c_max):
                    r0, r1 = max(0, r_max - 1), min(ROWS, r_max + 2)
                    c0, c1 = max(0, c_max - 1), min(COLS, c_max + 2)
                    neigh = frame[r0:r1, c0:c1]
                    hot_thresh = frame_avg + cfg.neighbour_threshold * frame_std
                    # hot_count includes center pixel; subtract it
                    hot_neighbours = int(np.sum(neigh > hot_thresh)) - 1
                    if hot_neighbours >= cfg.min_hot_neighbours:
                        tracker = _HotspotTracker(
                            row=r_max, col=c_max,
                            baseline_neigh=neigh,
                            baseline_diff=temp_diff,
                            created_at=now,
                        )
                        self._trackers.append(tracker)
                        LOG.debug(
                            "Candidate at (%d,%d): temp_diff=%.1f°C "
                            "(baseline=%.1f°C), hot_neighbours=%d",
                            r_max, c_max, temp_diff, baseline_diff, hot_neighbours,
                        )

        # ------------------------------------------------------------------
        # 3. Update existing trackers
        # ------------------------------------------------------------------
        surviving: list[_HotspotTracker] = []
        for tracker in self._trackers:
            alert = tracker.update(frame, temp_diff, cfg, now)
            if alert:
                alerts.append(alert)
            if tracker.is_active:
                surviving.append(tracker)
        self._trackers = surviving

        # ------------------------------------------------------------------
        # 4. Update histories — frozen once full so baselines are static
        # ------------------------------------------------------------------
        if len(self._diff_history) < self._diff_history.maxlen:
            self._diff_history.append(temp_diff)
        if len(self._avg_history) < self._avg_history.maxlen:
            self._avg_history.append(frame_avg)

        return alerts

    def _near_tracker(self, row: int, col: int, radius: int = 2) -> bool:
        return any(
            abs(t.row - row) <= radius and abs(t.col - col) <= radius
            for t in self._trackers
        )

    @property
    def active_tracker_count(self) -> int:
        return len(self._trackers)

    @property
    def active_trackers(self) -> list[tuple[int, int, str]]:
        """Return (row, col, state_name) for each currently tracked hotspot."""
        return [(t.row, t.col, t.state.value) for t in self._trackers]


# ---------------------------------------------------------------------------
# Monitor runner
# ---------------------------------------------------------------------------

class FireMonitor:
    """
    Wraps a FrameSource and FrameAnalyzer in a loop.

    Alerts are placed on `alert_queue`.  Supports both
    multiprocessing.Queue (for process mode) and queue.Queue (for thread
    mode) — the interface is identical.
    """

    def __init__(
        self,
        source: FrameSource,
        config: FireMonitorConfig,
        alert_queue: Any,
        stop_event: Any,
    ) -> None:
        self.source = source
        self.analyzer = FrameAnalyzer(config)
        self.alert_queue = alert_queue
        self.stop_event = stop_event

    def run(self) -> None:
        LOG.info("FireMonitor starting.")
        try:
            self.source.open()
            while not self.stop_event.is_set():
                result = self.source.read_frame()
                if result is None:
                    continue
                _ta, frame = result
                now = time.monotonic()
                for alert in self.analyzer.process(frame, now):
                    try:
                        self.alert_queue.put_nowait(alert)
                    except Exception:
                        pass  # queue full — drop oldest would require extra logic
        except Exception as exc:
            LOG.exception("FireMonitor error: %s", exc)
        finally:
            self.source.close()
            LOG.info("FireMonitor stopped.")


# ---------------------------------------------------------------------------
# Process-worker function (must be importable at module level for Windows spawn)
# ---------------------------------------------------------------------------

def _process_worker(
    source_type: str,
    source_kwargs: dict,
    config: FireMonitorConfig,
    alert_queue: "multiprocessing.Queue[FireAlert]",
    stop_event: "multiprocessing.Event",
) -> None:
    logging.basicConfig(
        level=logging.INFO,
        format="%(asctime)s %(levelname)s fire_monitor: %(message)s",
    )
    if source_type == "mlx90640":
        source: FrameSource = MLX90640FrameSource(**source_kwargs)
    elif source_type == "optris":
        source = OptrisFrameSource(**source_kwargs)
    else:
        raise ValueError(f"Unknown source_type: {source_type!r}")

    FireMonitor(source, config, alert_queue, stop_event).run()


# ---------------------------------------------------------------------------
# Public launchers
# ---------------------------------------------------------------------------

def start_monitor_process(
    source_type: str,
    source_kwargs: dict,
    config: FireMonitorConfig | None = None,
) -> "tuple[multiprocessing.Process, multiprocessing.Queue, multiprocessing.Event]":
    """
    Start the monitor in a separate OS process (true parallelism).

    Parameters
    ----------
    source_type  : "mlx90640" or "optris"
    source_kwargs: keyword arguments forwarded to the FrameSource constructor
                   e.g. {"port": "COM5", "baud": 921600}
    config       : optional FireMonitorConfig (defaults used if None)

    Returns
    -------
    (process, alert_queue, stop_event)
    Call stop_event.set() to request a clean shutdown, then process.join().
    """
    if config is None:
        config = FireMonitorConfig()
    alert_queue: multiprocessing.Queue = multiprocessing.Queue(maxsize=200)
    stop_event = multiprocessing.Event()
    proc = multiprocessing.Process(
        target=_process_worker,
        args=(source_type, source_kwargs, config, alert_queue, stop_event),
        daemon=True,
        name="fire-monitor",
    )
    proc.start()
    return proc, alert_queue, stop_event


def start_monitor_thread(
    source: FrameSource,
    config: FireMonitorConfig | None = None,
) -> "tuple[threading.Thread, queue.Queue, threading.Event]":
    """
    Start the monitor in a daemon thread (lighter-weight fallback).

    The source must not be opened before calling this — FireMonitor.run()
    will call source.open() inside the thread.
    """
    if config is None:
        config = FireMonitorConfig()
    alert_queue: queue.Queue = queue.Queue(maxsize=200)
    stop_event = threading.Event()
    monitor = FireMonitor(source, config, alert_queue, stop_event)
    thread = threading.Thread(target=monitor.run, daemon=True, name="fire-monitor")
    thread.start()
    return thread, alert_queue, stop_event
