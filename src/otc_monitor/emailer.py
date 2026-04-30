from __future__ import annotations

import logging
import os
import smtplib
from datetime import datetime, timezone
from email.message import EmailMessage

from .config import EmailConfig
from .fire_algorithm import AlertLevel, FireAlert

LOG = logging.getLogger(__name__)


class SmtpAlerter:
    def __init__(self, config: EmailConfig) -> None:
        self.config = config

    def send_fire_alert(self, alert: FireAlert, camera_name: str, serial_number: int) -> None:
        password = os.environ.get(self.config.password_env)
        if not password:
            LOG.error("Env var %r not set — skipping email", self.config.password_env)
            return

        subject = self._fire_subject(alert, camera_name)
        body = self._fire_body(alert, camera_name, serial_number)

        msg = EmailMessage()
        msg["From"] = self.config.from_address or self.config.username
        msg["To"] = ", ".join(self.config.to_addresses)
        msg["Subject"] = subject
        msg.set_content(body)

        try:
            if self.config.use_ssl:
                with smtplib.SMTP_SSL(self.config.smtp_host, self.config.smtp_port, timeout=30) as s:
                    s.login(self.config.username, password)
                    s.send_message(msg)
            else:
                with smtplib.SMTP(self.config.smtp_host, self.config.smtp_port, timeout=30) as s:
                    s.ehlo()
                    s.starttls()
                    s.ehlo()
                    s.login(self.config.username, password)
                    s.send_message(msg)
            LOG.info("Alert sent: %s / %s", camera_name, alert.level.value)
        except Exception as exc:
            LOG.error("Failed to send alert email: %s", exc)

    def _fire_subject(self, alert: FireAlert, camera_name: str) -> str:
        level = alert.level.value.upper()
        location = (
            f"pixel ({alert.pixel_row},{alert.pixel_col})"
            if alert.pixel_row is not None else "scene"
        )
        return f"{self.config.subject_prefix}: {camera_name} / {level} / {location}"

    def _fire_body(self, alert: FireAlert, camera_name: str, serial_number: int) -> str:
        observed = datetime.now(timezone.utc).astimezone().isoformat()
        location = (
            f"pixel ({alert.pixel_row}, {alert.pixel_col})"
            if alert.pixel_row is not None else "whole scene"
        )
        lines = [
            f"Alert level:    {alert.level.value.upper()}",
            f"Observed at:    {observed}",
            f"Camera:         {camera_name}",
            f"Serial number:  {serial_number}",
            f"Location:       {location}",
            f"Current temp:   {alert.current_temp_c:.2f} °C",
            f"Baseline temp:  {alert.baseline_temp_c:.2f} °C",
            f"Rise:           {alert.rise_c:+.2f} °C",
            "",
            alert.message,
        ]
        return "\n".join(lines)
