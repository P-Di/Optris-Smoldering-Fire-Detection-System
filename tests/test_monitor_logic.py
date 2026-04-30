from __future__ import annotations

import sys
import time
import unittest
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
SRC = ROOT / "src"
if str(SRC) not in sys.path:
    sys.path.insert(0, str(SRC))

from optris_alert_daemon.models import AlarmPhase, AlertKind, Thresholds
from optris_alert_daemon.monitor import AlarmTracker, evaluate_alarm_phase


class EvaluateAlarmPhaseTests(unittest.TestCase):
    def test_high_alarm_priority(self) -> None:
        thresholds = Thresholds(high_pre_alarm_c=80.0, high_alarm_c=100.0)
        self.assertEqual(evaluate_alarm_phase(101.0, thresholds, "high"), AlarmPhase.HIGH_ALARM)
        self.assertEqual(evaluate_alarm_phase(85.0, thresholds, "high"), AlarmPhase.HIGH_PREALARM)
        self.assertEqual(evaluate_alarm_phase(60.0, thresholds, "high"), AlarmPhase.NORMAL)

    def test_low_alarm_priority(self) -> None:
        thresholds = Thresholds(low_alarm_c=10.0, low_pre_alarm_c=15.0)
        self.assertEqual(evaluate_alarm_phase(9.0, thresholds, "low"), AlarmPhase.LOW_ALARM)
        self.assertEqual(evaluate_alarm_phase(14.0, thresholds, "low"), AlarmPhase.LOW_PREALARM)
        self.assertEqual(evaluate_alarm_phase(25.0, thresholds, "low"), AlarmPhase.NORMAL)


class AlarmTrackerTests(unittest.TestCase):
    def test_requires_consecutive_samples(self) -> None:
        tracker = AlarmTracker(
            consecutive_samples=2,
            repeat_interval_seconds=60.0,
            send_pre_alarm=False,
            clear_alerts=True,
        )
        now = time.monotonic()
        self.assertIsNone(tracker.observe(AlarmPhase.HIGH_ALARM, 101.0, now))
        notification = tracker.observe(AlarmPhase.HIGH_ALARM, 102.0, now + 1.0)
        self.assertIsNotNone(notification)
        assert notification is not None
        self.assertEqual(notification.kind, AlertKind.ENTERED)
        self.assertEqual(notification.phase, AlarmPhase.HIGH_ALARM)

    def test_clear_notification(self) -> None:
        tracker = AlarmTracker(
            consecutive_samples=1,
            repeat_interval_seconds=60.0,
            send_pre_alarm=False,
            clear_alerts=True,
        )
        now = time.monotonic()
        tracker.observe(AlarmPhase.HIGH_ALARM, 101.0, now)
        notification = tracker.observe(AlarmPhase.NORMAL, 70.0, now + 1.0)
        self.assertIsNotNone(notification)
        assert notification is not None
        self.assertEqual(notification.kind, AlertKind.CLEARED)
        self.assertEqual(notification.previous_phase, AlarmPhase.HIGH_ALARM)

    def test_repeat_notification(self) -> None:
        tracker = AlarmTracker(
            consecutive_samples=1,
            repeat_interval_seconds=5.0,
            send_pre_alarm=True,
            clear_alerts=True,
        )
        now = time.monotonic()
        tracker.observe(AlarmPhase.HIGH_PREALARM, 90.0, now)
        notification = tracker.observe(AlarmPhase.HIGH_PREALARM, 91.0, now + 6.0)
        self.assertIsNotNone(notification)
        assert notification is not None
        self.assertEqual(notification.kind, AlertKind.REPEATED)
        self.assertEqual(notification.phase, AlarmPhase.HIGH_PREALARM)


    def test_no_clear_when_pre_alarm_suppressed(self) -> None:
        # If send_pre_alarm=False, no ENTERED was sent for pre-alarm, so
        # no CLEARED should be sent when temperature returns to normal.
        tracker = AlarmTracker(
            consecutive_samples=1,
            repeat_interval_seconds=60.0,
            send_pre_alarm=False,
            clear_alerts=True,
        )
        now = time.monotonic()
        tracker.observe(AlarmPhase.HIGH_PREALARM, 85.0, now)
        notification = tracker.observe(AlarmPhase.NORMAL, 70.0, now + 1.0)
        self.assertIsNone(notification)


if __name__ == "__main__":
    unittest.main()
