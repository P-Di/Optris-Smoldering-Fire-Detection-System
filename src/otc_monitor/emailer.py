from __future__ import annotations

import logging
import os
import smtplib
from datetime import datetime, timezone
from email.message import EmailMessage

from .config import EmailConfig
from .fire_algorithm import AlertLevel, FireAlert

LOG = logging.getLogger(__name__)

# Known SMS/MMS email gateway domains — get a short body, no image attachment.
# Most gateways support MMS but keeping it short improves reliability.
_SMS_DOMAINS = frozenset({
    "vtext.com", "vzwpix.com",           # Verizon
    "tmomail.net", "tmmms.com",          # T-Mobile
    "txt.att.net", "mms.att.net",        # AT&T
    "messaging.sprintpcs.com",           # Sprint
    "pm.sprint.com",
    "myboostmobile.com",
    "sms.myboostmobile.com",
    "email.uscc.net",
    "vmobl.com",
})


def _is_sms_gateway(address: str) -> bool:
    domain = address.split("@")[-1].lower() if "@" in address else ""
    return domain in _SMS_DOMAINS


class SmtpAlerter:
    def __init__(self, config: EmailConfig) -> None:
        self.config = config

    def send_fire_alert(
        self,
        alert: FireAlert,
        camera_name: str,
        serial_number: int,
        snapshot_png: bytes | None = None,
    ) -> None:
        password = os.environ.get(self.config.password_env)
        if not password:
            LOG.error("Env var %r not set — skipping email", self.config.password_env)
            return

        subject = self._fire_subject(alert, camera_name)

        try:
            if self.config.use_ssl:
                conn = smtplib.SMTP_SSL(self.config.smtp_host, self.config.smtp_port, timeout=30)
            else:
                conn = smtplib.SMTP(self.config.smtp_host, self.config.smtp_port, timeout=30)
                conn.ehlo()
                conn.starttls()
                conn.ehlo()

            with conn:
                conn.login(self.config.username, password)
                for recipient in self.config.to_addresses:
                    msg = self._build_message(
                        alert, camera_name, serial_number,
                        subject, recipient, snapshot_png,
                    )
                    conn.send_message(msg)
                    LOG.info("Alert sent to %s: %s / %s", recipient, camera_name, alert.level.value)

        except Exception as exc:
            LOG.error("Failed to send alert email: %s", exc)

    # ── message builders ──────────────────────────────────────────────────────

    def _build_message(
        self,
        alert: FireAlert,
        camera_name: str,
        serial_number: int,
        subject: str,
        recipient: str,
        snapshot_png: bytes | None,
    ) -> EmailMessage:
        msg = EmailMessage()
        msg["From"]    = self.config.from_address or self.config.username
        msg["To"]      = recipient
        msg["Subject"] = subject

        if _is_sms_gateway(recipient):
            msg.set_content(self._sms_body(alert, camera_name))
        else:
            msg.set_content(self._fire_body(alert, camera_name, serial_number))
            if snapshot_png:
                ts = datetime.now(timezone.utc).strftime("%Y%m%d_%H%M%S")
                filename = f"snapshot_{camera_name.replace(' ', '_')}_{ts}.png"
                msg.add_attachment(
                    snapshot_png,
                    maintype="image",
                    subtype="png",
                    filename=filename,
                )

        return msg

    def _fire_subject(self, alert: FireAlert, camera_name: str) -> str:
        level = alert.level.value.upper()
        location = (
            f"pixel ({alert.pixel_row},{alert.pixel_col})"
            if alert.pixel_row is not None else "scene"
        )
        return f"{self.config.subject_prefix}: {camera_name} / {level} / {location}"

    def _sms_body(self, alert: FireAlert, camera_name: str) -> str:
        return (
            f"{alert.level.value.upper()} — {camera_name}\n"
            f"Temp: {alert.current_temp_c:.1f}°C (+{alert.rise_c:+.1f}°C)\n"
            f"{alert.message}"
        )

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
