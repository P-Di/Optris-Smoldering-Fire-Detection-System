from __future__ import annotations

from dataclasses import dataclass
from datetime import datetime
from enum import Enum
from pathlib import Path


class AlarmPhase(str, Enum):
    NORMAL = "normal"
    LOW_PREALARM = "low_pre_alarm"
    HIGH_PREALARM = "high_pre_alarm"
    LOW_ALARM = "low_alarm"
    HIGH_ALARM = "high_alarm"

    @property
    def is_active(self) -> bool:
        return self is not AlarmPhase.NORMAL

    @property
    def is_alarm(self) -> bool:
        return self in {AlarmPhase.LOW_ALARM, AlarmPhase.HIGH_ALARM}

    @property
    def is_pre_alarm(self) -> bool:
        return self in {AlarmPhase.LOW_PREALARM, AlarmPhase.HIGH_PREALARM}


class AlertKind(str, Enum):
    ENTERED = "entered"
    REPEATED = "repeated"
    CLEARED = "cleared"


@dataclass(frozen=True)
class Thresholds:
    low_alarm_c: float | None = None
    low_pre_alarm_c: float | None = None
    high_pre_alarm_c: float | None = None
    high_alarm_c: float | None = None
    source: str = "custom"

    def overlay(self, other: "Thresholds") -> "Thresholds":
        return Thresholds(
            low_alarm_c=other.low_alarm_c if other.low_alarm_c is not None else self.low_alarm_c,
            low_pre_alarm_c=other.low_pre_alarm_c if other.low_pre_alarm_c is not None else self.low_pre_alarm_c,
            high_pre_alarm_c=other.high_pre_alarm_c if other.high_pre_alarm_c is not None else self.high_pre_alarm_c,
            high_alarm_c=other.high_alarm_c if other.high_alarm_c is not None else self.high_alarm_c,
            source=self.source,
        )


@dataclass(frozen=True)
class AlertEvent:
    kind: AlertKind
    phase: AlarmPhase
    previous_phase: AlarmPhase
    observed_at: datetime
    camera_name: str
    instance_name: str | None
    measure_area_label: str
    measure_area_name: str
    measure_area_index: int
    temperature_c: float
    thresholds: Thresholds
    snapshot_path: Path | None = None
