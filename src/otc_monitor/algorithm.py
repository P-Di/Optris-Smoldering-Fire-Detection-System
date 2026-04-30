"""Algorithm 4 — hysteresis-based alarm state machine."""
from __future__ import annotations

from .models import AlarmPhase, AlertKind, Thresholds, TrackerNotification


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


class AlarmTracker:
    def __init__(
        self,
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
                        kind=AlertKind.CLEARED, phase=phase,
                        previous_phase=previous, temperature_c=temperature_c,
                    )
                return None

            if phase.is_pre_alarm and not self.send_pre_alarm:
                return None

            self._last_notification_at = now
            return TrackerNotification(
                kind=AlertKind.ENTERED, phase=phase,
                previous_phase=previous, temperature_c=temperature_c,
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
                kind=AlertKind.REPEATED, phase=phase,
                previous_phase=self._stable_phase, temperature_c=temperature_c,
            )

        return None
