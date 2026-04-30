"""
Pixel-level fire / heat anomaly monitor — adapted from pixel_fire_monitor.py.

Frame-size agnostic: works with any 2-D float32 numpy array (rows × cols)
representing per-pixel temperatures in °C.  All hardcoded MLX90640 dimensions
have been replaced with dynamic frame.shape lookups.

The entry point for the OTC SDK integration is AbsoluteThresholdFrameAnalyzer:
  analyzer = AbsoluteThresholdFrameAnalyzer(config)
  for alert in analyzer.process(temp_frame_2d, time.monotonic()):
      ...

See FireMonitorConfig for tunable parameters.
"""
from __future__ import annotations

import logging
from collections import deque
from dataclasses import dataclass
from enum import Enum

import numpy as np

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
    # --- Candidate creation ---
    # Create a candidate when any pixel exceeds this temperature (°C).
    # Maps to high_alarm_c / high_pre_alarm_c from the old threshold config.
    absolute_threshold_c: float = 30.0

    # --- Rising-trend thresholds (vs. stored baseline at candidate_threshold) ---
    # WARNING clock starts when neighbourhood max >= threshold + warning_rise_c
    warning_rise_c: float = 1.5
    # ALARM clock starts when neighbourhood max >= threshold + alarm_rise_c
    alarm_rise_c:   float = 3.0
    # Seconds the neighbourhood must stay above each level (continuously) before alert fires.
    warning_window_s: float = 120.0   # 2 minutes
    alarm_window_s:   float = 300.0   # 5 minutes
    # Dismiss tracker if neighbourhood drops this far below baseline max
    drop_tolerance_c: float = 0.5
    # Expire a CANDIDATE that never crossed warning_rise_c after this many seconds.
    max_candidate_age_s: float = 3600.0

    # --- Relative threshold (scene-adaptive) ---
    # Also create a candidate when (max_pixel - rolling_scene_avg) exceeds this value.
    # Set to None to disable.  Useful when ambient temperature varies — catches anomalies
    # that are hot *relative to the scene* rather than in absolute terms.
    # The rolling average is built from the first `ambient_window` frames and then frozen,
    # so it represents the scene's baseline state at startup.
    relative_threshold_c: float | None = None

    # --- Spatial coherence (spike-based detector only) ---
    spike_factor: float = 1.6
    min_temp_diff_c: float = 3.0
    baseline_window: int = 30
    neighbour_threshold: float = 1.5
    min_hot_neighbours: int = 2

    # --- Ambient drift check ---
    ambient_window: int = 60
    ambient_spike_c: float = 4.0
    ambient_median_fraction: float = 0.4
    send_ambient_alerts: bool = False

    # --- Rate limiting ---
    min_repeat_interval_s: float = 10.0


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
        baseline_max_override: float | None = None,
    ) -> None:
        self.row = row
        self.col = col
        self.baseline_neigh = baseline_neigh.copy()
        self.baseline_diff = baseline_diff
        self.baseline_max = baseline_max_override if baseline_max_override is not None else float(baseline_neigh.max())
        self.created_at = created_at

        self.state = _Phase.CANDIDATE
        self.is_active = True

        self._first_above_warning_at: float | None = None
        self._first_above_alarm_at:   float | None = None
        self._last_alert_at: float = 0.0

    def _neighbourhood(self, frame: np.ndarray) -> np.ndarray:
        rows, cols = frame.shape
        r0, r1 = max(0, self.row - 1), min(rows, self.row + 2)
        c0, c1 = max(0, self.col - 1), min(cols, self.col + 2)
        return frame[r0:r1, c0:c1]

    def update(self, frame: np.ndarray, config: FireMonitorConfig, now: float) -> FireAlert | None:
        neigh = self._neighbourhood(frame)
        cur_max = float(neigh.max())
        rise = cur_max - self.baseline_max

        if rise < -config.drop_tolerance_c:
            self.is_active = False
            return None

        if (self.state is _Phase.CANDIDATE
                and self._first_above_warning_at is None
                and config.max_candidate_age_s > 0
                and now - self.created_at > config.max_candidate_age_s):
            self.is_active = False
            return None

        # Warning clock
        if rise >= config.warning_rise_c:
            if self._first_above_warning_at is None:
                self._first_above_warning_at = now
        else:
            self._first_above_warning_at = None

        # Alarm clock
        if rise >= config.alarm_rise_c:
            if self._first_above_alarm_at is None:
                self._first_above_alarm_at = now
        else:
            self._first_above_alarm_at = None

        # Phase transitions (time-based, continuous above threshold required)
        if self.state is _Phase.CANDIDATE or self.state is _Phase.WARNING:
            if (self._first_above_alarm_at is not None
                    and now - self._first_above_alarm_at >= config.alarm_window_s):
                self.state = _Phase.ALARM
            elif (self.state is _Phase.CANDIDATE
                    and self._first_above_warning_at is not None
                    and now - self._first_above_warning_at >= config.warning_window_s):
                self.state = _Phase.WARNING

        if self.state in (_Phase.WARNING, _Phase.ALARM):
            if now - self._last_alert_at >= config.min_repeat_interval_s:
                self._last_alert_at = now
                level = AlertLevel.WARNING if self.state is _Phase.WARNING else AlertLevel.ALARM
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
                        f"{cur_max:.1f}°C (+{rise:.1f}°C above baseline {self.baseline_max:.1f}°C)"
                    ),
                )

        return None


# ---------------------------------------------------------------------------
# AbsoluteThresholdFrameAnalyzer — primary algorithm for OTC SDK integration
# ---------------------------------------------------------------------------

class AbsoluteThresholdFrameAnalyzer:
    """
    Creates a candidate tracker when any pixel exceeds `absolute_threshold_c`.
    WARNING/ALARM are only issued if the hotspot sustains a rising trend above
    the threshold for the configured time windows without cooling back down.

    Call process(frame_2d, now) once per incoming frame.
    frame_2d must be a float32 numpy array shaped (rows, cols) with temps in °C.
    """

    def __init__(self, config: FireMonitorConfig) -> None:
        self.config = config
        self._trackers: list[_HotspotTracker] = []
        self._avg_history: deque[float] = deque(maxlen=config.ambient_window)
        self._last_ambient_alert_at: float = 0.0

    def process(self, frame: np.ndarray, now: float) -> list[FireAlert]:
        alerts: list[FireAlert] = []
        cfg = self.config

        frame_avg = float(np.mean(frame))
        frame_max = float(np.max(frame))

        # Ambient drift check
        if cfg.send_ambient_alerts:
            half = cfg.ambient_window // 2
            if len(self._avg_history) >= half:
                expected_avg = float(np.mean(self._avg_history))
                if frame_avg > expected_avg + cfg.ambient_spike_c:
                    frame_median = float(np.median(frame))
                    median_high = frame_median > expected_avg + cfg.ambient_spike_c * cfg.ambient_median_fraction
                    rate_ok = now - self._last_ambient_alert_at >= cfg.min_repeat_interval_s * 3
                    if median_high and rate_ok:
                        self._last_ambient_alert_at = now
                        alerts.append(FireAlert(
                            level=AlertLevel.AMBIENT_HIGH,
                            timestamp=now,
                            pixel_row=None, pixel_col=None,
                            baseline_temp_c=expected_avg,
                            current_temp_c=frame_avg,
                            rise_c=frame_avg - expected_avg,
                            message=(
                                f"AMBIENT_HIGH: scene avg {frame_avg:.1f}°C "
                                f"(+{frame_avg - expected_avg:.1f}°C), "
                                f"median={frame_median:.1f}°C, max={frame_max:.1f}°C"
                            ),
                        ))

        # --- Step 1: Update existing trackers FIRST so cooling ones are dismissed ---
        # This ensures a new hotspot that appears near a just-dismissed tracker can
        # immediately create its own candidate in the same frame.
        surviving: list[_HotspotTracker] = []
        for tracker in self._trackers:
            alert = tracker.update(frame, cfg, now)
            if alert:
                alerts.append(alert)
            if tracker.is_active:
                surviving.append(tracker)
        self._trackers = surviving

        # --- Step 2: Candidate creation against the now-pruned tracker list ---
        # Fires when any pixel exceeds the absolute threshold OR rises more than
        # relative_threshold_c above the frozen scene average.
        scene_avg = float(np.mean(self._avg_history)) if self._avg_history else frame_avg
        above_absolute = frame_max > cfg.absolute_threshold_c
        above_relative = (
            cfg.relative_threshold_c is not None
            and len(self._avg_history) > 0
            and (frame_max - scene_avg) > cfg.relative_threshold_c
        )

        if above_absolute or above_relative:
            rows, cols = frame.shape
            flat_idx = int(np.argmax(frame))
            r_max, c_max = divmod(flat_idx, cols)
            if not self._near_tracker(r_max, c_max):
                r0, r1 = max(0, r_max - 1), min(rows, r_max + 2)
                c0, c1 = max(0, c_max - 1), min(cols, c_max + 2)
                neigh = frame[r0:r1, c0:c1]
                # Baseline is whichever trigger set the higher bar, so WARNING/ALARM
                # require a genuine rise above the triggering level.
                baseline_override = max(
                    cfg.absolute_threshold_c if above_absolute else 0.0,
                    (scene_avg + cfg.relative_threshold_c) if above_relative and cfg.relative_threshold_c else 0.0,
                )
                tracker = _HotspotTracker(
                    row=r_max, col=c_max,
                    baseline_neigh=neigh,
                    baseline_diff=0.0,
                    created_at=now,
                    baseline_max_override=baseline_override,
                )
                self._trackers.append(tracker)
                reason = []
                if above_absolute:
                    reason.append(f"abs {frame_max:.1f}°C > {cfg.absolute_threshold_c}°C")
                if above_relative:
                    reason.append(f"rel {frame_max - scene_avg:.1f}°C above avg {scene_avg:.1f}°C")
                LOG.debug("Candidate at (%d,%d): %s", r_max, c_max, ", ".join(reason))

        # Ambient history (frozen once full — baseline stays static)
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
