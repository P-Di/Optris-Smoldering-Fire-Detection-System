from __future__ import annotations

import json
from dataclasses import dataclass, field
from pathlib import Path


@dataclass(frozen=True)
class EmailConfig:
    smtp_host: str
    smtp_port: int
    username: str
    password_env: str
    to_addresses: list[str]
    from_address: str | None = None
    subject_prefix: str = "Optris Alert"
    use_ssl: bool = True


@dataclass(frozen=True)
class RuntimeConfig:
    consecutive_samples: int = 2              # unused by fire algorithm, kept for compat
    repeat_alert_interval_seconds: float = 1800.0
    send_pre_alarm: bool = False
    clear_alerts: bool = True
    enumeration_wait_seconds: float = 2.5
    # Ethernet camera detection — add one entry per subnet your cameras are on.
    # CIDR notation, e.g. "192.168.0.0/24". Leave empty to skip Ethernet scanning.
    ethernet_subnets: list[str] = field(default_factory=list)
    # Set to False to skip USB scanning (useful for Ethernet-only deployments).
    use_usb: bool = True


@dataclass(frozen=True)
class FireConfig:
    """Parameters forwarded directly to AbsoluteThresholdFrameAnalyzer."""
    # Candidate creation: a pixel must exceed this temperature (°C) to start tracking.
    absolute_threshold_c: float = 30.0
    # Also create a candidate when (max_pixel - rolling_scene_avg) exceeds this (°C).
    # Set to null/None to disable.  Catches relative anomalies regardless of ambient temp.
    relative_threshold_c: float | None = None
    # WARNING fires when hotspot has been >= threshold + warning_rise_c for warning_window_s seconds.
    warning_rise_c: float = 1.5
    warning_window_s: float = 120.0
    # ALARM fires when hotspot has been >= threshold + alarm_rise_c for alarm_window_s seconds.
    alarm_rise_c: float = 3.0
    alarm_window_s: float = 300.0
    # Dismiss tracker if hotspot drops this far below its baseline
    drop_tolerance_c: float = 0.5
    # Expire a candidate that never crossed warning_rise_c after this many seconds
    max_candidate_age_s: float = 3600.0
    # Minimum time between repeated alerts for the same tracker
    min_repeat_interval_s: float = 10.0
    # Whether to email AMBIENT_HIGH alerts (whole-scene average rise)
    send_ambient_alerts: bool = False


@dataclass(frozen=True)
class CameraConfig:
    name: str
    serial_number: int = 0
    fire_config: FireConfig = field(default_factory=FireConfig)
    send_pre_alarm: bool = False   # if False, suppress WARNING emails (ALARM still sends)


@dataclass(frozen=True)
class AppConfig:
    email: EmailConfig
    runtime: RuntimeConfig
    default_camera: CameraConfig
    cameras: list[CameraConfig]

    def get_camera_config(self, serial_number: int, index: int) -> CameraConfig:
        for cam in self.cameras:
            if cam.serial_number == serial_number:
                return cam
        return CameraConfig(
            name=self.default_camera.name.format(index=index + 1, serial=serial_number),
            serial_number=serial_number,
            fire_config=self.default_camera.fire_config,
            send_pre_alarm=self.default_camera.send_pre_alarm,
        )


def _parse_fire_config(d: dict, runtime_send_pre_alarm: bool) -> tuple[FireConfig, bool]:
    """Returns (FireConfig, send_pre_alarm)."""
    fm = d.get("fire_monitor", {})
    # absolute_threshold_c: prefer fire_monitor block, fall back to legacy thresholds.high_alarm_c
    legacy_thresh = d.get("thresholds", {}).get("high_alarm_c")
    threshold = fm.get("absolute_threshold_c", legacy_thresh if legacy_thresh is not None else 30.0)

    fire_cfg = FireConfig(
        absolute_threshold_c=threshold,
        relative_threshold_c=fm.get("relative_threshold_c", None),
        warning_rise_c=fm.get("warning_rise_c", 1.5),
        warning_window_s=fm.get("warning_window_s", 120.0),
        alarm_rise_c=fm.get("alarm_rise_c", 3.0),
        alarm_window_s=fm.get("alarm_window_s", 300.0),
        drop_tolerance_c=fm.get("drop_tolerance_c", 0.5),
        max_candidate_age_s=fm.get("max_candidate_age_s", 3600.0),
        min_repeat_interval_s=fm.get("min_repeat_interval_s", 10.0),
        send_ambient_alerts=fm.get("send_ambient_alerts", False),
    )
    send_pre_alarm = d.get("send_pre_alarm", runtime_send_pre_alarm)
    return fire_cfg, send_pre_alarm


def _parse_camera(d: dict, runtime_send_pre_alarm: bool) -> CameraConfig:
    fire_cfg, send_pre_alarm = _parse_fire_config(d, runtime_send_pre_alarm)
    return CameraConfig(
        name=d.get("name", "Camera {index}"),
        serial_number=d.get("serial_number", 0),
        fire_config=fire_cfg,
        send_pre_alarm=send_pre_alarm,
    )


def load_config(path: str | Path) -> AppConfig:
    with open(path) as f:
        data = json.load(f)

    e = data["email"]
    email = EmailConfig(
        smtp_host=e["smtp_host"],
        smtp_port=e["smtp_port"],
        username=e["username"],
        password_env=e.get("password_env", "GMAIL_APP_PASSWORD"),
        to_addresses=e["to_addresses"],
        from_address=e.get("from_address"),
        subject_prefix=e.get("subject_prefix", "Optris Alert"),
        use_ssl=e.get("use_ssl", True),
    )

    rt = data.get("runtime", {})
    runtime = RuntimeConfig(
        consecutive_samples=rt.get("consecutive_samples", 2),
        repeat_alert_interval_seconds=rt.get("repeat_alert_interval_seconds", 1800.0),
        send_pre_alarm=rt.get("send_pre_alarm", False),
        clear_alerts=rt.get("clear_alerts", True),
        enumeration_wait_seconds=rt.get("enumeration_wait_seconds", 2.5),
        ethernet_subnets=rt.get("ethernet_subnets", []),
        use_usb=rt.get("use_usb", True),
    )

    default_camera = _parse_camera(data.get("default_camera", {}), runtime.send_pre_alarm)
    cameras = [_parse_camera(c, runtime.send_pre_alarm) for c in data.get("cameras", [])]

    return AppConfig(email=email, runtime=runtime, default_camera=default_camera, cameras=cameras)
