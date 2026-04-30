from __future__ import annotations

import mimetypes
import os
import smtplib
from email.message import EmailMessage

from .config import EmailConfig
from .models import AlertEvent


class SmtpAlerter:
    def __init__(self, config: EmailConfig):
        self.config = config

    def send(self, event: AlertEvent, attach_snapshot: bool = False) -> None:
        password = os.environ.get(self.config.password_env)
        if not password:
            raise RuntimeError(
                f"Environment variable {self.config.password_env!r} is not set. "
                "For Gmail, this should usually be an app password."
            )

        message = EmailMessage()
        from_address = self.config.from_address or self.config.username
        message["From"] = from_address
        message["To"] = ", ".join(self.config.to_addresses)
        message["Subject"] = self._subject(event)
        message.set_content(self._body(event))

        if attach_snapshot and event.snapshot_path and event.snapshot_path.exists():
            mime_type, _ = mimetypes.guess_type(event.snapshot_path.name)
            maintype, subtype = (mime_type or "application/octet-stream").split("/", 1)
            with event.snapshot_path.open("rb") as handle:
                message.add_attachment(
                    handle.read(),
                    maintype=maintype,
                    subtype=subtype,
                    filename=event.snapshot_path.name,
                )

        if self.config.use_ssl:
            with smtplib.SMTP_SSL(self.config.smtp_host, self.config.smtp_port, timeout=30) as client:
                client.login(self.config.username, password)
                client.send_message(message)
        else:
            with smtplib.SMTP(self.config.smtp_host, self.config.smtp_port, timeout=30) as client:
                client.ehlo()
                client.starttls()
                client.ehlo()
                client.login(self.config.username, password)
                client.send_message(message)

    def _subject(self, event: AlertEvent) -> str:
        state_text = event.phase.value.replace("_", " ")
        return f"{self.config.subject_prefix}: {event.camera_name} / {event.measure_area_label} / {state_text}"

    def _body(self, event: AlertEvent) -> str:
        threshold_lines = [
            f"High alarm: {self._fmt_threshold(event.thresholds.high_alarm_c)}",
            f"High pre-alarm: {self._fmt_threshold(event.thresholds.high_pre_alarm_c)}",
            f"Low pre-alarm: {self._fmt_threshold(event.thresholds.low_pre_alarm_c)}",
            f"Low alarm: {self._fmt_threshold(event.thresholds.low_alarm_c)}",
        ]
        snapshot_text = str(event.snapshot_path) if event.snapshot_path else "not captured"
        return "\n".join(
            [
                f"Alert type: {event.kind.value}",
                f"State: {event.phase.value}",
                f"Previous state: {event.previous_phase.value}",
                f"Observed at: {event.observed_at.isoformat()}",
                f"Camera: {event.camera_name}",
                f"PIX Connect instance: {event.instance_name or '<default>'}",
                f"Measure area label: {event.measure_area_label}",
                f"Measure area name: {event.measure_area_name}",
                f"Measure area index: {event.measure_area_index}",
                f"Temperature: {event.temperature_c:.2f} C",
                f"Threshold source: {event.thresholds.source}",
                *threshold_lines,
                f"Snapshot: {snapshot_text}",
            ]
        )

    @staticmethod
    def _fmt_threshold(value: float | None) -> str:
        return f"{value:.2f} C" if value is not None else "disabled"
