from __future__ import annotations

import json
import os
from dataclasses import dataclass, field
from pathlib import Path
from typing import Any

from .models import Thresholds


VALID_DIRECTIONS = {"high", "low", "both"}
VALID_THRESHOLD_SOURCES = {"pix_connect", "custom"}


class ConfigError(ValueError):
    pass


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
    poll_interval_seconds: float = 1.0
    connect_timeout_seconds: float = 60.0
    connect_retry_interval_seconds: float = 1.0
    startup_grace_seconds: float = 5.0
    consecutive_samples: int = 2
    repeat_alert_interval_seconds: float = 1800.0
    clear_alerts: bool = True
    snapshot_timeout_seconds: float = 10.0
    reconnect_backoff_seconds: float = 5.0
    log_level: str = "INFO"
    log_file: Path | None = None
    # Startup configuration steps
    auto_configure_pix_connect: bool = True   # pywinauto: enable Connect SDK in External Communication
    ensure_fullscreen_alarm_area: bool = True  # SDK: create full-screen measure area if absent
    fullscreen_area_name: str = "Full Screen Alarm"  # name for the auto-created area


@dataclass(frozen=True)
class MeasureAreaConfig:
    label: str | None = None
    index: int | None = None
    name: str | None = None
    direction: str = "high"
    threshold_source: str = "pix_connect"
    thresholds: Thresholds = field(default_factory=Thresholds)
    send_pre_alarm: bool = False
    repeat_alert_interval_seconds: float | None = None
    snapshot_on_alarm: bool = False
    attach_snapshot: bool = False

    @property
    def key(self) -> tuple[str | None, int | None, str | None]:
        return (self.label, self.index, self.name)


@dataclass(frozen=True)
class CameraConfig:
    name: str
    instance_name: str | None = None
    launch_command: list[str] | None = None
    measure_areas: list[MeasureAreaConfig] = field(default_factory=list)
    snapshot_dir: Path | None = None


@dataclass(frozen=True)
class AppConfig:
    config_path: Path
    sdk_dll_path: Path
    email: EmailConfig
    runtime: RuntimeConfig
    cameras: list[CameraConfig]


def load_config(path: str | Path) -> AppConfig:
    config_path = Path(path).expanduser().resolve()
    if not config_path.exists():
        raise ConfigError(f"Config file does not exist: {config_path}")

    with config_path.open("r", encoding="utf-8") as handle:
        data = json.load(handle)

    base_dir = config_path.parent
    runtime = _parse_runtime(data.get("runtime", {}), base_dir)
    email = _parse_email(data.get("email", {}))
    sdk_dll_path = _resolve_path(_require_str(data, "sdk_dll_path"), base_dir)
    cameras = _parse_cameras(data.get("cameras"), base_dir)

    if not cameras:
        raise ConfigError("At least one camera must be configured.")
    if len(cameras) > 1:
        unnamed = [camera.name for camera in cameras if not camera.instance_name]
        if unnamed:
            joined = ", ".join(unnamed)
            raise ConfigError(
                "When configuring multiple cameras, each camera needs an instance_name. "
                f"Missing for: {joined}"
            )

    return AppConfig(
        config_path=config_path,
        sdk_dll_path=sdk_dll_path,
        email=email,
        runtime=runtime,
        cameras=cameras,
    )


def _parse_email(raw: dict[str, Any]) -> EmailConfig:
    to_addresses = raw.get("to_addresses") or []
    if not isinstance(to_addresses, list) or not all(isinstance(item, str) and item.strip() for item in to_addresses):
        raise ConfigError("email.to_addresses must be a non-empty list of email addresses.")
    return EmailConfig(
        smtp_host=str(raw.get("smtp_host", "smtp.gmail.com")),
        smtp_port=int(raw.get("smtp_port", 465)),
        username=_require_str(raw, "username", scope="email"),
        password_env=str(raw.get("password_env", "GMAIL_APP_PASSWORD")),
        to_addresses=[item.strip() for item in to_addresses],
        from_address=_optional_str(raw.get("from_address")),
        subject_prefix=str(raw.get("subject_prefix", "Optris Alert")),
        use_ssl=bool(raw.get("use_ssl", True)),
    )


def _parse_runtime(raw: dict[str, Any], base_dir: Path) -> RuntimeConfig:
    log_file_raw = raw.get("log_file")
    log_file = _resolve_path(log_file_raw, base_dir) if log_file_raw else None
    consecutive_samples = int(raw.get("consecutive_samples", 2))
    if consecutive_samples < 1:
        raise ConfigError("runtime.consecutive_samples must be >= 1.")
    return RuntimeConfig(
        poll_interval_seconds=float(raw.get("poll_interval_seconds", 1.0)),
        connect_timeout_seconds=float(raw.get("connect_timeout_seconds", 60.0)),
        connect_retry_interval_seconds=float(raw.get("connect_retry_interval_seconds", 1.0)),
        startup_grace_seconds=float(raw.get("startup_grace_seconds", 5.0)),
        consecutive_samples=consecutive_samples,
        repeat_alert_interval_seconds=float(raw.get("repeat_alert_interval_seconds", 1800.0)),
        clear_alerts=bool(raw.get("clear_alerts", True)),
        snapshot_timeout_seconds=float(raw.get("snapshot_timeout_seconds", 10.0)),
        reconnect_backoff_seconds=float(raw.get("reconnect_backoff_seconds", 5.0)),
        log_level=str(raw.get("log_level", "INFO")).upper(),
        log_file=log_file,
        auto_configure_pix_connect=bool(raw.get("auto_configure_pix_connect", True)),
        ensure_fullscreen_alarm_area=bool(raw.get("ensure_fullscreen_alarm_area", True)),
        fullscreen_area_name=str(raw.get("fullscreen_area_name", "Full Screen Alarm")),
    )


def _parse_cameras(raw: Any, base_dir: Path) -> list[CameraConfig]:
    if not isinstance(raw, list):
        raise ConfigError("cameras must be a list.")
    cameras: list[CameraConfig] = []
    for item in raw:
        if not isinstance(item, dict):
            raise ConfigError("Each camera entry must be an object.")
        launch_command = item.get("launch_command")
        if launch_command is not None:
            if not isinstance(launch_command, list) or not all(isinstance(part, str) and part for part in launch_command):
                raise ConfigError(f"camera {item.get('name', '<unnamed>')}: launch_command must be a list of strings.")
            launch_command = [_expand_env(part) for part in launch_command]
        measure_areas_raw = item.get("measure_areas") or []
        if not measure_areas_raw:
            raise ConfigError(f"camera {item.get('name', '<unnamed>')}: at least one measure_area is required.")
        measure_areas = [_parse_measure_area(area, item.get("name", "<unnamed>")) for area in measure_areas_raw]
        snapshot_dir_raw = item.get("snapshot_dir")
        snapshot_dir = _resolve_path(snapshot_dir_raw, base_dir) if snapshot_dir_raw else None
        cameras.append(
            CameraConfig(
                name=_require_str(item, "name", scope="camera"),
                instance_name=_optional_str(item.get("instance_name")),
                launch_command=launch_command,
                measure_areas=measure_areas,
                snapshot_dir=snapshot_dir,
            )
        )
    return cameras


def _parse_measure_area(raw: Any, camera_name: str) -> MeasureAreaConfig:
    if not isinstance(raw, dict):
        raise ConfigError(f"camera {camera_name}: each measure_area entry must be an object.")
    index = raw.get("index")
    name = _optional_str(raw.get("name"))
    if index is None and not name:
        raise ConfigError(f"camera {camera_name}: each measure_area needs either index or name.")
    if index is not None:
        index = int(index)
        if index < 0:
            raise ConfigError(f"camera {camera_name}: measure_area index must be >= 0.")
    direction = str(raw.get("direction", "high")).lower()
    if direction not in VALID_DIRECTIONS:
        raise ConfigError(f"camera {camera_name}: invalid direction {direction!r}. Use one of {sorted(VALID_DIRECTIONS)}.")
    threshold_source = str(raw.get("threshold_source", "pix_connect")).lower()
    if threshold_source not in VALID_THRESHOLD_SOURCES:
        raise ConfigError(
            f"camera {camera_name}: invalid threshold_source {threshold_source!r}. "
            f"Use one of {sorted(VALID_THRESHOLD_SOURCES)}."
        )
    thresholds = _parse_thresholds(raw.get("thresholds", {}), camera_name)
    if threshold_source == "custom" and not any(
        value is not None for value in (
            thresholds.low_alarm_c,
            thresholds.low_pre_alarm_c,
            thresholds.high_pre_alarm_c,
            thresholds.high_alarm_c,
        )
    ):
        raise ConfigError(f"camera {camera_name}: custom threshold_source requires at least one threshold value.")
    return MeasureAreaConfig(
        label=_optional_str(raw.get("label")),
        index=index,
        name=name,
        direction=direction,
        threshold_source=threshold_source,
        thresholds=thresholds,
        send_pre_alarm=bool(raw.get("send_pre_alarm", False)),
        repeat_alert_interval_seconds=float(raw["repeat_alert_interval_seconds"]) if "repeat_alert_interval_seconds" in raw else None,
        snapshot_on_alarm=bool(raw.get("snapshot_on_alarm", False)),
        attach_snapshot=bool(raw.get("attach_snapshot", False)),
    )


def _parse_thresholds(raw: Any, camera_name: str) -> Thresholds:
    if not isinstance(raw, dict):
        raise ConfigError(f"camera {camera_name}: thresholds must be an object.")
    return Thresholds(
        low_alarm_c=_optional_float(raw.get("low_alarm_c")),
        low_pre_alarm_c=_optional_float(raw.get("low_pre_alarm_c")),
        high_pre_alarm_c=_optional_float(raw.get("high_pre_alarm_c")),
        high_alarm_c=_optional_float(raw.get("high_alarm_c")),
    )


def _resolve_path(value: str | Path, base_dir: Path) -> Path:
    path = Path(_expand_env(str(value))).expanduser()
    if not path.is_absolute():
        path = (base_dir / path).resolve()
    else:
        path = path.resolve()
    return path


def _expand_env(value: str) -> str:
    return os.path.expandvars(value)


def _optional_str(value: Any) -> str | None:
    if value is None:
        return None
    text = str(value).strip()
    return text or None


def _optional_float(value: Any) -> float | None:
    if value is None:
        return None
    return float(value)


def _require_str(mapping: dict[str, Any], key: str, scope: str | None = None) -> str:
    value = mapping.get(key)
    if not isinstance(value, str) or not value.strip():
        prefix = f"{scope}." if scope else ""
        raise ConfigError(f"{prefix}{key} must be a non-empty string.")
    return value.strip()
