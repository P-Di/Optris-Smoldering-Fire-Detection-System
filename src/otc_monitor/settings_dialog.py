"""Settings dialog — edit key fields in monitor.json from within the GUI."""
from __future__ import annotations

import json
from pathlib import Path
from typing import Any

from PyQt5.QtCore import Qt
from PyQt5.QtWidgets import (
    QDialog,
    QDialogButtonBox,
    QDoubleSpinBox,
    QFormLayout,
    QGroupBox,
    QLabel,
    QLineEdit,
    QMessageBox,
    QSpinBox,
    QVBoxLayout,
    QWidget,
)

_DARK = """
QDialog            { background: #1e1e1e; color: #ddd; }
QGroupBox {
    color: #aaa; font-weight: bold; font-size: 12px;
    border: 1px solid #444; border-radius: 4px;
    margin-top: 10px; padding-top: 8px;
}
QGroupBox::title   { subcontrol-origin: margin; left: 8px; padding: 0 4px; }
QLabel             { color: #ccc; font-size: 11px; }
QLineEdit, QSpinBox, QDoubleSpinBox {
    background: #2a2a2a; color: #eee; border: 1px solid #555;
    border-radius: 3px; padding: 3px 6px; font-size: 12px; min-width: 180px;
}
QLineEdit:disabled, QSpinBox:disabled, QDoubleSpinBox:disabled {
    background: #222; color: #555; border-color: #383838;
}
QPushButton {
    background: #2a2a2a; color: #ccc; border: 1px solid #555;
    border-radius: 4px; padding: 5px 14px; font-size: 12px;
}
QPushButton:hover  { background: #383838; }
"""


def _row_label(name: str, description: str, warn: bool = False) -> QWidget:
    """Two-line label: bold name + small description."""
    w = QWidget()
    v = QVBoxLayout(w)
    v.setContentsMargins(0, 2, 8, 2)
    v.setSpacing(2)
    n = QLabel(name)
    n.setStyleSheet("color: #e0e0e0; font-size: 12px; font-weight: bold;")
    d = QLabel(description)
    d.setStyleSheet(f"color: {'#e67e22' if warn else '#777'}; font-size: 10px;")
    d.setWordWrap(True)
    v.addWidget(n)
    v.addWidget(d)
    return w


class SettingsDialog(QDialog):
    def __init__(self, config_path: str, parent: QWidget | None = None) -> None:
        super().__init__(parent)
        self._path = Path(config_path)
        self._data: dict[str, Any] = {}

        self.setWindowTitle("Monitor Settings")
        self.setMinimumWidth(560)
        self.setModal(True)
        self.setStyleSheet(_DARK)

        self._load()
        self._build_ui()

    # ── I/O ───────────────────────────────────────────────────────────────────

    def _load(self) -> None:
        with open(self._path, encoding="utf-8") as f:
            self._data = json.load(f)

    def _save(self) -> None:
        fm = self._data.setdefault("default_camera", {}).setdefault("fire_monitor", {})
        rt = self._data.setdefault("runtime", {})
        em = self._data.setdefault("email", {})

        fm["absolute_threshold_c"] = self._abs_thresh.value()
        fm["relative_threshold_c"] = self._rel_thresh.value()
        fm["alarm_window_s"]        = self._alarm_window.value()

        rt["ethernet_subnets"] = [
            s.strip() for s in self._subnets.text().split(",") if s.strip()
        ]
        em["to_addresses"] = [
            s.strip() for s in self._to_addresses.text().split(",") if s.strip()
        ]

        with open(self._path, "w", encoding="utf-8") as f:
            json.dump(self._data, f, indent=2)
            f.write("\n")

        QMessageBox.information(
            self, "Saved",
            "Settings saved to monitor.json.\nRestart the app to apply all changes.",
        )
        self.accept()

    # ── UI ────────────────────────────────────────────────────────────────────

    def _build_ui(self) -> None:
        fm = self._data.get("default_camera", {}).get("fire_monitor", {})
        em = self._data.get("email", {})
        rt = self._data.get("runtime", {})

        outer = QVBoxLayout(self)
        outer.setSpacing(10)
        outer.setContentsMargins(14, 14, 14, 14)

        # ── Fire Detection ────────────────────────────────────────────────
        fire_grp = QGroupBox("Fire Detection")
        fire_form = QFormLayout(fire_grp)
        fire_form.setSpacing(10)
        fire_form.setHorizontalSpacing(12)
        fire_form.setLabelAlignment(Qt.AlignLeft | Qt.AlignVCenter)

        self._abs_thresh = QDoubleSpinBox()
        self._abs_thresh.setRange(0, 1200)
        self._abs_thresh.setDecimals(1)
        self._abs_thresh.setSuffix(" °C")
        self._abs_thresh.setValue(float(fm.get("absolute_threshold_c", 40)))
        fire_form.addRow(
            _row_label("Absolute threshold",
                       "Create a candidate when any pixel exceeds this temperature."),
            self._abs_thresh,
        )

        self._rel_thresh = QDoubleSpinBox()
        self._rel_thresh.setRange(0, 200)
        self._rel_thresh.setDecimals(1)
        self._rel_thresh.setSuffix(" °C")
        self._rel_thresh.setValue(float(fm.get("relative_threshold_c", 10)))
        fire_form.addRow(
            _row_label("Relative threshold",
                       "Create a candidate when any pixel is this many °C above the scene average."),
            self._rel_thresh,
        )

        self._alarm_window = QSpinBox()
        self._alarm_window.setRange(1, 7200)
        self._alarm_window.setSuffix(" s")
        self._alarm_window.setValue(int(fm.get("alarm_window_s", 300)))
        fire_form.addRow(
            _row_label("Alarm window",
                       "How long a hotspot must persist continuously before an ALARM is sent."),
            self._alarm_window,
        )

        outer.addWidget(fire_grp)

        # ── Network ───────────────────────────────────────────────────────
        net_grp = QGroupBox("Network")
        net_form = QFormLayout(net_grp)
        net_form.setSpacing(10)
        net_form.setHorizontalSpacing(12)
        net_form.setLabelAlignment(Qt.AlignLeft | Qt.AlignVCenter)

        self._subnets = QLineEdit(", ".join(rt.get("ethernet_subnets", [])))
        net_form.addRow(
            _row_label("Ethernet subnets",
                       "Comma-separated CIDR subnets to scan for cameras (e.g. 192.168.0.0/24)."),
            self._subnets,
        )

        outer.addWidget(net_grp)

        # ── Email ─────────────────────────────────────────────────────────
        email_grp = QGroupBox("Email Alerts")
        email_form = QFormLayout(email_grp)
        email_form.setSpacing(10)
        email_form.setHorizontalSpacing(12)
        email_form.setLabelAlignment(Qt.AlignLeft | Qt.AlignVCenter)

        self._to_addresses = QLineEdit(", ".join(em.get("to_addresses", [])))
        email_form.addRow(
            _row_label("To addresses",
                       "Comma-separated list of recipient emails (or SMS gateways) for alerts."),
            self._to_addresses,
        )

        self._from_address = QLineEdit(em.get("from_address", ""))
        self._from_address.setEnabled(False)
        email_form.addRow(
            _row_label("From address",
                       "Do not change — must match the Gmail account set up on the server.",
                       warn=True),
            self._from_address,
        )

        self._username = QLineEdit(em.get("username", ""))
        self._username.setEnabled(False)
        email_form.addRow(
            _row_label("Username",
                       "Do not change — SMTP login credential tied to the server app password.",
                       warn=True),
            self._username,
        )

        outer.addWidget(email_grp)

        # ── Footer note ───────────────────────────────────────────────────
        note = QLabel("Restart the app after saving for all changes to take effect.")
        note.setAlignment(Qt.AlignCenter)
        note.setStyleSheet("font-size: 10px; color: #666; font-style: italic;")
        outer.addWidget(note)

        # ── Buttons ───────────────────────────────────────────────────────
        btns = QDialogButtonBox()
        btns.addButton("Cancel", QDialogButtonBox.RejectRole)
        save = btns.addButton("Save", QDialogButtonBox.AcceptRole)
        save.setStyleSheet(
            "background: #1a5276; border-color: #2980b9; color: #fff; font-weight: bold;"
        )
        btns.accepted.connect(self._save)
        btns.rejected.connect(self.reject)
        outer.addWidget(btns)
