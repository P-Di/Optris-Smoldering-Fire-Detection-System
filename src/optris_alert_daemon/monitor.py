from __future__ import annotations

import logging
import subprocess
import time
from dataclasses import dataclass, field
from datetime import datetime
from logging.handlers import RotatingFileHandler
from pathlib import Path

from .config import AppConfig, CameraConfig, MeasureAreaConfig
from .emailer import SmtpAlerter
from .models import AlertEvent, AlertKind, AlarmPhase, Thresholds
from .optris_sdk import (
    IPC_EVENT_AREAS_INIT,
    IPC_EVENT_CONFIG_CHANGED,
    IPC_EVENT_SERVER_STOPPED,
    MeasureAreaInfo,
    OptrisSdk,
    OptrisSdkError,
    OptrisSession,
)


LOG = logging.getLogger("optris_alert_daemon")


@dataclass
class TrackerNotification:
    kind: AlertKind
    phase: AlarmPhase
    previous_phase: AlarmPhase
    temperature_c: float


class AlarmTracker:
    def __init__(
        self,
        *,
        consecutive_samples: int,
        repeat_interval_seconds: float,
        send_pre_alarm: bool,
        clear_alerts: bool,
    ) -> None:
        self.consecutive_samples = consecutive_samples
        self.repeat_interval_seconds = repeat_interval_seconds
        self.send_pre_alarm = send_pre_alarm
        self.clear_alerts = clear_alerts
        self._stable_phase = AlarmPhase.NORMAL
        self._candidate_phase = AlarmPhase.NORMAL
        self._candidate_count = 0
        self._last_notification_at: float | None = None

    def observe(self, phase: AlarmPhase, temperature_c: float, now: float) -> TrackerNotification | None:
        if phase == self._candidate_phase:
            self._candidate_count += 1
        else:
            self._candidate_phase = phase
            self._candidate_count = 1

        if self._candidate_count >= self.consecutive_samples and phase != self._stable_phase:
            previous = self._stable_phase
            self._stable_phase = phase
            if phase is AlarmPhase.NORMAL:
                if previous.is_active and self.clear_alerts and (not previous.is_pre_alarm or self.send_pre_alarm):
                    self._last_notification_at = now
                    return TrackerNotification(
                        kind=AlertKind.CLEARED,
                        phase=phase,
                        previous_phase=previous,
                        temperature_c=temperature_c,
                    )
                return None
            if phase.is_pre_alarm and not self.send_pre_alarm:
                return None
            self._last_notification_at = now
            return TrackerNotification(
                kind=AlertKind.ENTERED,
                phase=phase,
                previous_phase=previous,
                temperature_c=temperature_c,
            )

        if (
            phase == self._stable_phase
            and phase.is_active
            and (not phase.is_pre_alarm or self.send_pre_alarm)
            and self._last_notification_at is not None
            and now - self._last_notification_at >= self.repeat_interval_seconds
        ):
            self._last_notification_at = now
            return TrackerNotification(
                kind=AlertKind.REPEATED,
                phase=phase,
                previous_phase=self._stable_phase,
                temperature_c=temperature_c,
            )

        return None


@dataclass
class AreaRuntime:
    config: MeasureAreaConfig
    label: str
    index: int
    name: str
    thresholds: Thresholds
    tracker: AlarmTracker


@dataclass
class CameraRuntime:
    config: CameraConfig
    session: OptrisSession
    areas: list[AreaRuntime] = field(default_factory=list)
    connected: bool = False
    next_connect_attempt_at: float = 0.0


class MonitorService:
    def __init__(self, config: AppConfig):
        self.config = config
        self.alerter = SmtpAlerter(config.email)
        self.sdk: OptrisSdk | None = None
        self.cameras: list[CameraRuntime] = []

    def validate(self) -> None:
        LOG.info("Config loaded successfully from %s", self.config.config_path)
        LOG.info("SDK DLL: %s", self.config.sdk_dll_path)
        for camera in self.config.cameras:
            LOG.info("Camera configured: %s (instance=%s)", camera.name, camera.instance_name or "<default>")

    def run(self) -> None:
        self._setup_logging()
        self.sdk = OptrisSdk(self.config.sdk_dll_path)
        self.sdk.set_imager_count(len(self.config.cameras))
        self._initialize_cameras()
        LOG.info("Monitoring started for %d camera(s).", len(self.cameras))
        try:
            while True:
                self._poll_once()
                time.sleep(self.config.runtime.poll_interval_seconds)
        finally:
            for camera in self.cameras:
                camera.session.release()

    def _initialize_cameras(self) -> None:
        assert self.sdk is not None
        for index, camera_config in enumerate(self.config.cameras):
            if camera_config.launch_command:
                self._launch_pix_connect(camera_config)
            session = self.sdk.create_session(index=index, instance_name=camera_config.instance_name)
            camera = CameraRuntime(config=camera_config, session=session)
            self._connect_camera(camera, startup=True)
            self.cameras.append(camera)

    def _poll_once(self) -> None:
        now = time.monotonic()
        for camera in self.cameras:
            if not camera.connected:
                if now >= camera.next_connect_attempt_at:
                    self._connect_camera(camera, startup=False)
                continue

            try:
                camera.session.process_messages()
                events = camera.session.poll_events(reset=True)
                if events & IPC_EVENT_SERVER_STOPPED:
                    raise OptrisSdkError(f"PIX Connect stopped IPC for {camera.config.name}")
                if events & (IPC_EVENT_CONFIG_CHANGED | IPC_EVENT_AREAS_INIT):
                    LOG.info("Reloading area configuration for %s after PIX Connect change.", camera.config.name)
                    self._reload_camera_areas(camera)

                for area in camera.areas:
                    temperature_c = camera.session.get_measure_area_temperature(area.index)
                    phase = evaluate_alarm_phase(temperature_c, area.thresholds, area.config.direction)
                    notification = area.tracker.observe(phase, temperature_c, now)
                    if notification:
                        try:
                            self._handle_notification(camera, area, notification)
                        except Exception as exc:  # pragma: no cover - depends on SMTP/camera runtime
                            LOG.exception(
                                "Failed to send notification for %s / %s: %s",
                                camera.config.name,
                                area.label,
                                exc,
                            )
            except OptrisSdkError as exc:
                LOG.warning("Camera %s disconnected: %s", camera.config.name, exc)
                camera.session.release()
                camera.connected = False
                camera.next_connect_attempt_at = now + self.config.runtime.reconnect_backoff_seconds

    def _connect_camera(self, camera: CameraRuntime, *, startup: bool) -> None:
        try:
            camera.session.connect(
                timeout_seconds=self.config.runtime.connect_timeout_seconds,
                retry_interval_seconds=self.config.runtime.connect_retry_interval_seconds,
            )
            self._reload_camera_areas(camera)
            camera.connected = True
            camera.next_connect_attempt_at = 0.0
            LOG.info("Connected to %s (instance=%s).", camera.config.name, camera.config.instance_name or "<default>")
            if startup and self.config.runtime.startup_grace_seconds > 0:
                time.sleep(self.config.runtime.startup_grace_seconds)
        except OptrisSdkError as exc:
            camera.connected = False
            camera.next_connect_attempt_at = time.monotonic() + self.config.runtime.reconnect_backoff_seconds
            message = f"Could not connect to {camera.config.name}: {exc}"
            if startup:
                raise OptrisSdkError(message) from exc
            LOG.warning(message)

    def _reload_camera_areas(self, camera: CameraRuntime) -> None:
        area_count = camera.session.get_measure_area_count()
        discovered = {
            index: MeasureAreaInfo(
                index=index,
                name=camera.session.get_measure_area_name(index) or f"Area {index}",
                thresholds=camera.session.get_alarm_threshold(index),
            )
            for index in range(area_count)
        }
        LOG.debug(
            "Camera %s: discovered %d area(s): %s",
            camera.config.name,
            area_count,
            {i: info.name for i, info in discovered.items()},
        )
        previous = {area.config.key: area for area in camera.areas}
        new_areas: list[AreaRuntime] = []
        for area_config in camera.config.measure_areas:
            info = resolve_measure_area(area_config, discovered, camera.config.name)
            base_thresholds = info.thresholds if area_config.threshold_source == "pix_connect" else Thresholds(source="custom")
            thresholds = base_thresholds.overlay(area_config.thresholds)
            previous_area = previous.get(area_config.key)
            tracker = previous_area.tracker if previous_area else AlarmTracker(
                consecutive_samples=self.config.runtime.consecutive_samples,
                repeat_interval_seconds=area_config.repeat_alert_interval_seconds
                or self.config.runtime.repeat_alert_interval_seconds,
                send_pre_alarm=area_config.send_pre_alarm,
                clear_alerts=self.config.runtime.clear_alerts,
            )
            new_areas.append(
                AreaRuntime(
                    config=area_config,
                    label=area_config.label or info.name or f"Area {info.index}",
                    index=info.index,
                    name=info.name,
                    thresholds=thresholds,
                    tracker=tracker,
                )
            )
        camera.areas = new_areas

    def _handle_notification(
        self,
        camera: CameraRuntime,
        area: AreaRuntime,
        notification: TrackerNotification,
    ) -> None:
        snapshot_path: Path | None = None
        if area.config.snapshot_on_alarm and notification.phase.is_alarm and notification.kind is not AlertKind.CLEARED:
            try:
                triggered_at = time.time()
                snapshot_path = camera.session.capture_snapshot(self.config.runtime.snapshot_timeout_seconds)
                if snapshot_path is None and camera.config.snapshot_dir:
                    snapshot_path = _find_file_after(camera.config.snapshot_dir, triggered_at)
                    if snapshot_path:
                        LOG.debug("Snapshot found via fallback dir: %s", snapshot_path)
                    else:
                        LOG.warning("No snapshot found in fallback dir %s after trigger.", camera.config.snapshot_dir)
            except OptrisSdkError as exc:
                LOG.warning("Snapshot capture failed for %s / %s: %s", camera.config.name, area.label, exc)

        event = AlertEvent(
            kind=notification.kind,
            phase=notification.phase,
            previous_phase=notification.previous_phase,
            observed_at=datetime.now().astimezone(),
            camera_name=camera.config.name,
            instance_name=camera.config.instance_name,
            measure_area_label=area.label,
            measure_area_name=area.name,
            measure_area_index=area.index,
            temperature_c=notification.temperature_c,
            thresholds=area.thresholds,
            snapshot_path=snapshot_path,
        )
        self.alerter.send(event, attach_snapshot=area.config.attach_snapshot)
        LOG.info(
            "Alert sent: %s / %s / %s / %.2f C",
            event.camera_name,
            event.measure_area_label,
            event.phase.value,
            event.temperature_c,
        )

    def _launch_pix_connect(self, camera_config: CameraConfig) -> None:
        LOG.info("Launching PIX Connect for %s: %s", camera_config.name, camera_config.launch_command)
        subprocess.Popen(camera_config.launch_command)

    def _setup_logging(self) -> None:
        level = getattr(logging, self.config.runtime.log_level.upper(), logging.INFO)
        handlers: list[logging.Handler] = [logging.StreamHandler()]
        if self.config.runtime.log_file:
            self.config.runtime.log_file.parent.mkdir(parents=True, exist_ok=True)
            handlers.append(
                RotatingFileHandler(
                    self.config.runtime.log_file,
                    maxBytes=1_000_000,
                    backupCount=3,
                    encoding="utf-8",
                )
            )
        logging.basicConfig(
            level=level,
            format="%(asctime)s %(levelname)s %(name)s: %(message)s",
            handlers=handlers,
        )


def resolve_measure_area(
    area_config: MeasureAreaConfig,
    discovered: dict[int, MeasureAreaInfo],
    camera_name: str,
) -> MeasureAreaInfo:
    if area_config.index is not None:
        if area_config.index not in discovered:
            raise OptrisSdkError(
                f"camera {camera_name}: configured measure area index {area_config.index} was not found in PIX Connect"
            )
        return discovered[area_config.index]
    assert area_config.name is not None
    for info in discovered.values():
        if info.name == area_config.name:
            return info
    raise OptrisSdkError(
        f"camera {camera_name}: configured measure area name {area_config.name!r} was not found in PIX Connect"
    )


def _find_file_after(directory: Path, after_timestamp: float) -> Path | None:
    try:
        candidates = [
            f for f in directory.iterdir()
            if f.is_file() and f.stat().st_mtime >= after_timestamp
        ]
        return max(candidates, key=lambda f: f.stat().st_mtime) if candidates else None
    except OSError:
        return None


def evaluate_alarm_phase(temperature_c: float, thresholds: Thresholds, direction: str) -> AlarmPhase:
    if direction in {"low", "both"}:
        if thresholds.low_alarm_c is not None and temperature_c <= thresholds.low_alarm_c:
            return AlarmPhase.LOW_ALARM
        if thresholds.low_pre_alarm_c is not None and temperature_c <= thresholds.low_pre_alarm_c:
            return AlarmPhase.LOW_PREALARM
    if direction in {"high", "both"}:
        if thresholds.high_alarm_c is not None and temperature_c >= thresholds.high_alarm_c:
            return AlarmPhase.HIGH_ALARM
        if thresholds.high_pre_alarm_c is not None and temperature_c >= thresholds.high_pre_alarm_c:
            return AlarmPhase.HIGH_PREALARM
    return AlarmPhase.NORMAL
