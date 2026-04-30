"""Per-camera SDK worker: bridges IRImagerClient callbacks to Qt signals."""
from __future__ import annotations

import logging
import threading
import time
from datetime import datetime, timezone

import numpy as np
import optris.otcsdk as otc
from PyQt5.QtCore import QObject, pyqtSignal

from .fire_algorithm import AbsoluteThresholdFrameAnalyzer, AlertLevel, FireAlert, FireMonitorConfig
from .config import CameraConfig, RuntimeConfig
from .emailer import SmtpAlerter

LOG = logging.getLogger(__name__)


class CameraSignals(QObject):
    """Qt signal bus — kept separate from IRImagerClient to avoid C++/Qt MRO conflicts."""
    # (numpy image BGR, max temp °C, flag state string)
    frame_ready = pyqtSignal(object, float, str)
    # (camera_name, AlertLevel str, current_temp °C, active_candidate_count)
    alarm_changed = pyqtSignal(str, str, float, int)
    # camera_name
    connection_lost = pyqtSignal(str)


class CameraClient(otc.IRImagerClient):
    """
    Connects to one camera, processes frames with the pixel-level fire algorithm,
    and fires Qt signals for the GUI.  One background thread per camera runs
    imager.run(); all heavy work is done there.
    """

    def __init__(
        self,
        device_info: otc.DeviceInfo,
        cam_config: CameraConfig,
        runtime: RuntimeConfig,
        alerter: SmtpAlerter,
        signals: CameraSignals,
    ) -> None:
        super().__init__()

        self._name = cam_config.name
        self._serial = device_info.getSerialNumber()
        self._config = cam_config
        self._alerter = alerter
        self._signals = signals

        self._imager = otc.IRImagerFactory.getInstance().create("native")
        self._imager.addClient(self)
        self._imager.connect(device_info)

        # False-color image builder (BGR for numpy → Qt RGB flip)
        self._builder = otc.ImageBuilder(
            colorFormat=otc.ColorFormat_BGR,
            widthAlignment=otc.WidthAlignment_OneByte,
        )

        self._flag_lock = threading.Lock()
        self._flag_state = otc.FlagState_Initializing
        self._thread: threading.Thread | None = None

        # Fire / heat anomaly algorithm
        fire_cfg = FireMonitorConfig(
            absolute_threshold_c=cam_config.fire_config.absolute_threshold_c,
            relative_threshold_c=cam_config.fire_config.relative_threshold_c,
            warning_rise_c=cam_config.fire_config.warning_rise_c,
            alarm_rise_c=cam_config.fire_config.alarm_rise_c,
            warning_window_s=cam_config.fire_config.warning_window_s,
            alarm_window_s=cam_config.fire_config.alarm_window_s,
            drop_tolerance_c=cam_config.fire_config.drop_tolerance_c,
            max_candidate_age_s=cam_config.fire_config.max_candidate_age_s,
            min_repeat_interval_s=cam_config.fire_config.min_repeat_interval_s,
            send_ambient_alerts=cam_config.fire_config.send_ambient_alerts,
        )
        self._analyzer = AbsoluteThresholdFrameAnalyzer(fire_cfg)
        self._send_warning = cam_config.send_pre_alarm

        # Pre-allocate temperature buffer (resized on first frame)
        self._temp_buf: np.ndarray | None = None

        # GUI frame-rate limiter: emit frame_ready at most this often (seconds).
        # Algorithm runs every frame regardless.
        self._gui_interval: float = 0.5   # ~2 Hz display update
        self._last_gui_emit: float = 0.0

        # Reconnection state
        self._reconnect_lock = threading.Lock()
        self._reconnecting = False
        self._reconnect_backoff = 5.0      # initial retry delay (seconds)
        self._reconnect_max_backoff = 60.0 # cap

    def start(self) -> None:
        self._thread = threading.Thread(
            target=self._imager.run,
            daemon=True,
            name=f"cam-{self._serial}",
        )
        self._thread.start()

    def stop(self) -> None:
        self._imager.stopRunning()
        if self._thread:
            self._thread.join(timeout=5.0)

    # ── IRImagerClient callbacks (called from imager thread) ──────────────────

    def onThermalFrame(self, thermal, meta) -> None:
        with self._flag_lock:
            flag = self._flag_state

        if flag == otc.FlagState_Initializing:
            return

        w, h = thermal.getWidth(), thermal.getHeight()

        # Get raw temperature array (°C, float32, row-major)
        n = w * h
        if self._temp_buf is None or self._temp_buf.size != n:
            self._temp_buf = np.empty(n, dtype=np.float32)
        thermal.copyTemperaturesTo(self._temp_buf)
        temp_frame = self._temp_buf.reshape(h, w)

        max_temp = float(np.max(temp_frame))

        # Run fire algorithm
        now = time.monotonic()
        alerts = self._analyzer.process(temp_frame, now)
        for alert in alerts:
            if alert.level == AlertLevel.AMBIENT_HIGH:
                LOG.info("Camera %s: %s", self._name, alert.message)
                continue  # ambient alerts are logged only unless send_ambient_alerts=True
            if alert.level == AlertLevel.WARNING and not self._send_warning:
                LOG.info("Camera %s: %s", self._name, alert.message)
                continue  # warning suppressed (send_pre_alarm=false)
            self._dispatch_fire_alert(alert)

        # Emit GUI alarm state whenever anything changes
        candidate_count = self._analyzer.active_tracker_count
        worst = _worst_level(self._analyzer.active_trackers)
        self._signals.alarm_changed.emit(self._name, worst, max_temp, candidate_count)

        # False-color display image — throttled to _gui_interval to keep Qt
        # main thread responsive with many cameras.  Algorithm runs every frame.
        if now - self._last_gui_emit >= self._gui_interval:
            self._last_gui_emit = now
            self._builder.setThermalFrame(thermal)
            self._builder.convertTemperatureToPaletteImage()
            img_h = self._builder.getHeight()
            img_w = self._builder.getWidth()
            image = np.empty((img_h, img_w, 3), dtype=np.uint8)
            self._builder.copyImageDataTo(image)
            flag_str = otc.flagStateToString(flag)
            self._signals.frame_ready.emit(image, max_temp, flag_str)

    def onFlagStateChange(self, flagState) -> None:
        with self._flag_lock:
            self._flag_state = flagState

    def onConnectionLost(self) -> None:
        LOG.warning("Camera %s (S/N %s): connection lost — starting reconnect loop", self._name, self._serial)
        self._signals.connection_lost.emit(self._name)
        self._start_reconnect()

    def onConnectionTimeout(self) -> None:
        LOG.warning("Camera %s (S/N %s): connection timeout — starting reconnect loop", self._name, self._serial)
        self._signals.connection_lost.emit(self._name)
        self._start_reconnect()

    def _start_reconnect(self) -> None:
        with self._reconnect_lock:
            if self._reconnecting:
                return   # already retrying
            self._reconnecting = True
        threading.Thread(
            target=self._reconnect_loop,
            daemon=True,
            name=f"reconnect-{self._serial}",
        ).start()

    def _reconnect_loop(self) -> None:
        delay = self._reconnect_backoff
        attempt = 0
        while True:
            time.sleep(delay)
            attempt += 1
            LOG.info("Camera %s: reconnect attempt %d (delay was %.0fs)…", self._name, attempt, delay)
            try:
                new_imager = otc.IRImagerFactory.getInstance().create("native")
                new_imager.addClient(self)
                new_imager.connect(self._serial)

                # Reset flag state before the new run() thread starts delivering frames
                with self._flag_lock:
                    self._flag_state = otc.FlagState_Initializing

                old_imager = self._imager
                self._imager = new_imager

                self._thread = threading.Thread(
                    target=self._imager.run,
                    daemon=True,
                    name=f"cam-{self._serial}",
                )
                self._thread.start()

                # Stop the dead old imager (no-op if already stopped)
                try:
                    old_imager.stopRunning()
                except Exception:
                    pass

                LOG.info("Camera %s (S/N %s): reconnected on attempt %d", self._name, self._serial, attempt)
                with self._reconnect_lock:
                    self._reconnecting = False
                return

            except otc.SDKException as exc:
                LOG.warning(
                    "Camera %s: reconnect attempt %d failed: %s — retrying in %.0fs",
                    self._name, attempt, exc, delay,
                )
                delay = min(delay * 1.5, self._reconnect_max_backoff)

    # ── internal ──────────────────────────────────────────────────────────────

    def _dispatch_fire_alert(self, alert: FireAlert) -> None:
        self._alerter.send_fire_alert(
            alert=alert,
            camera_name=self._name,
            serial_number=self._serial,
        )


def _worst_level(trackers: list[tuple[int, int, str]]) -> str:
    """Return the worst alert level string across all active trackers."""
    states = {s for _, _, s in trackers}
    if "alarm" in states:
        return AlertLevel.ALARM.value
    if "warning" in states:
        return AlertLevel.WARNING.value
    if "candidate" in states:
        return "candidate"
    return "normal"
