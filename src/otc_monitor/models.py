from __future__ import annotations

from dataclasses import dataclass
from datetime import datetime
from enum import Enum


class AlarmPhase(Enum):
    NORMAL = "normal"
    LOW_PREALARM = "low_pre_alarm"
    HIGH_PREALARM = "high_pre_alarm"
    LOW_ALARM = "low_alarm"
    HIGH_ALARM = "high_alarm"

    @property
    def is_active(self) -> bool:
        return self != AlarmPhase.NORMAL

    @property
    def is_pre_alarm(self) -> bool:
        return self in (AlarmPhase.LOW_PREALARM, AlarmPhase.HIGH_PREALARM)


class AlertKind(Enum):
    ENTERED = "entered"
    REPEATED = "repeated"
    CLEARED = "cleared"


@dataclass(frozen=True)
class Thresholds:
    low_alarm_c: float | None = None
    low_pre_alarm_c: float | None = None
    high_pre_alarm_c: float | None = None
    high_alarm_c: float | None = None


@dataclass(frozen=True)
class TrackerNotification:
    kind: AlertKind
    phase: AlarmPhase
    previous_phase: AlarmPhase
    temperature_c: float


@dataclass(frozen=True)
class AlertEvent:
    kind: AlertKind
    phase: AlarmPhase
    previous_phase: AlarmPhase
    observed_at: datetime
    camera_name: str
    serial_number: int
    temperature_c: float
    thresholds: Thresholds
