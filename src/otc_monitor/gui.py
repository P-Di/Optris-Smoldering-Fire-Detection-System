"""PyQt5 GUI — main window and per-camera tile widget."""
from __future__ import annotations

import numpy as np
from PyQt5.QtCore import Qt, pyqtSlot
from PyQt5.QtGui import QImage, QPixmap
from PyQt5.QtWidgets import (
    QFrame,
    QGridLayout,
    QLabel,
    QMainWindow,
    QScrollArea,
    QSizePolicy,
    QStatusBar,
    QVBoxLayout,
    QWidget,
)

# level string → border colour
_BORDER = {
    "normal":    "#27ae60",   # green
    "candidate": "#f1c40f",   # yellow — being watched
    "warning":   "#f39c12",   # orange — sustained hotspot
    "alarm":     "#e74c3c",   # red    — confirmed rising alarm
}

# level string → status label text template
_STATUS = {
    "normal":    "Normal",
    "candidate": "Candidate detected",
    "warning":   "WARNING — sustained hotspot",
    "alarm":     "ALARM — rising heat anomaly",
}

_LABEL_STYLE = {
    "normal":    "font-size: 11px; color: #2ecc71; border: none;",
    "candidate": "font-size: 11px; color: #f1c40f; font-weight: bold; border: none;",
    "warning":   "font-size: 11px; color: #f39c12; font-weight: bold; border: none;",
    "alarm":     "font-size: 11px; color: #e74c3c; font-weight: bold; border: none;",
}


class CameraWidget(QFrame):
    """Single camera display tile: thermal image + max temp + alarm state."""

    def __init__(self, camera_name: str, serial_number: int, parent: QWidget | None = None) -> None:
        super().__init__(parent)
        self.setObjectName("cameraFrame")
        self._current_level = "normal"
        self._apply_border("normal")
        self.setMinimumSize(260, 210)   # compact enough for 12-up grid

        layout = QVBoxLayout(self)
        layout.setContentsMargins(6, 6, 6, 6)
        layout.setSpacing(3)

        title = QLabel(f"{camera_name}  ·  S/N {serial_number}")
        title.setAlignment(Qt.AlignCenter)
        title.setStyleSheet("font-weight: bold; font-size: 13px; color: #ddd; border: none;")

        self._image_label = QLabel("Connecting…")
        self._image_label.setAlignment(Qt.AlignCenter)
        self._image_label.setSizePolicy(QSizePolicy.Expanding, QSizePolicy.Expanding)
        self._image_label.setStyleSheet("background: #111; color: #555; font-size: 14px; border: none;")

        self._temp_label = QLabel("Max: -- °C")
        self._temp_label.setAlignment(Qt.AlignCenter)
        self._temp_label.setStyleSheet("font-size: 15px; font-weight: bold; color: #eee; border: none;")

        self._alarm_label = QLabel("Initializing")
        self._alarm_label.setAlignment(Qt.AlignCenter)
        self._alarm_label.setStyleSheet(_LABEL_STYLE["normal"])

        self._candidate_label = QLabel("")
        self._candidate_label.setAlignment(Qt.AlignCenter)
        self._candidate_label.setStyleSheet("font-size: 10px; color: #888; border: none;")

        layout.addWidget(title)
        layout.addWidget(self._image_label, stretch=1)
        layout.addWidget(self._temp_label)
        layout.addWidget(self._alarm_label)
        layout.addWidget(self._candidate_label)

    @pyqtSlot(object, float, str)
    def update_frame(self, image_bgr: np.ndarray, max_temp: float, flag_state: str) -> None:
        h, w, _ = image_bgr.shape
        rgb = image_bgr[:, :, ::-1].copy()
        qt_img = QImage(rgb.data, w, h, w * 3, QImage.Format_RGB888)
        lw, lh = self._image_label.width(), self._image_label.height()
        if lw > 1 and lh > 1:
            pix = QPixmap.fromImage(qt_img).scaled(lw, lh, Qt.KeepAspectRatio, Qt.SmoothTransformation)
        else:
            pix = QPixmap.fromImage(qt_img)
        self._image_label.setPixmap(pix)
        self._temp_label.setText(f"Max: {max_temp:.1f} °C")

    @pyqtSlot(str, str, float, int)
    def update_alarm_state(self, _camera_name: str, level: str, temp: float, candidates: int) -> None:
        self._current_level = level
        self._apply_border(level)

        status = _STATUS.get(level, level.upper())
        if level in ("warning", "alarm"):
            self._alarm_label.setText(f"{status}  {temp:.1f} °C")
        else:
            self._alarm_label.setText(status)
        self._alarm_label.setStyleSheet(_LABEL_STYLE.get(level, _LABEL_STYLE["normal"]))

        if candidates > 0:
            self._candidate_label.setText(f"{candidates} tracked hotspot{'s' if candidates != 1 else ''}")
        else:
            self._candidate_label.setText("")

    @pyqtSlot(str)
    def set_disconnected(self, _camera_name: str = "") -> None:
        self._image_label.clear()
        self._image_label.setText("Connection Lost")
        self._apply_border(None)
        self._alarm_label.setText("Disconnected")
        self._alarm_label.setStyleSheet("font-size: 11px; color: #e74c3c; border: none;")
        self._candidate_label.setText("")

    def _apply_border(self, level: str | None) -> None:
        color = _BORDER.get(level, "#555555")
        self.setStyleSheet(
            f"QFrame#cameraFrame {{ border: 4px solid {color}; border-radius: 6px; background: #1c1c1c; }}"
        )


_COLS = 3   # cameras per row — change to 4 for very wide monitors


class MainWindow(QMainWindow):
    def __init__(self) -> None:
        super().__init__()
        self.setWindowTitle("Optris Thermal Monitor")
        self.setMinimumSize(820, 600)
        self.setStyleSheet("QMainWindow { background: #141414; } QStatusBar { color: #888; }")

        # Scroll area so all cameras are reachable without resizing the window
        self._scroll = QScrollArea()
        self._scroll.setWidgetResizable(True)
        self._scroll.setStyleSheet("QScrollArea { border: none; background: #141414; }")

        self._grid_widget = QWidget()
        self._grid_widget.setStyleSheet("background: #141414;")
        self._grid = QGridLayout(self._grid_widget)
        self._grid.setSpacing(8)
        self._grid.setContentsMargins(8, 8, 8, 8)

        self._scroll.setWidget(self._grid_widget)
        self.setCentralWidget(self._scroll)

        self._widgets: dict[int, CameraWidget] = {}

        self.setStatusBar(QStatusBar())
        self.statusBar().showMessage("Scanning for cameras…")

    def add_camera(self, serial: int, name: str) -> CameraWidget:
        widget = CameraWidget(name, serial)
        n = len(self._widgets)
        self._grid.addWidget(widget, n // _COLS, n % _COLS)
        self._widgets[serial] = widget
        count = len(self._widgets)
        self.statusBar().showMessage(f"{count} camera{'s' if count != 1 else ''} connected")
        return widget

    def no_cameras_found(self) -> None:
        msg = QLabel("No cameras detected.\nCheck USB/Ethernet connections and restart.")
        msg.setAlignment(Qt.AlignCenter)
        msg.setStyleSheet("font-size: 16px; color: #666;")
        self._grid.addWidget(msg, 0, 0)
        self.statusBar().showMessage("No cameras found.")
