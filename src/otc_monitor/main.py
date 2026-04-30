"""Application entry point — SDK init, camera enumeration, GUI startup."""
from __future__ import annotations

import logging
import sys
from logging.handlers import RotatingFileHandler
from pathlib import Path

import optris.otcsdk as otc
from PyQt5.QtCore import QTimer
from PyQt5.QtWidgets import QApplication

from .camera_worker import CameraClient, CameraSignals
from .config import load_config
from .emailer import SmtpAlerter
from .gui import MainWindow

LOG = logging.getLogger(__name__)


def run(config_path: str = "config/monitor.json") -> None:
    log_file = Path(config_path).resolve().parent.parent / "monitor.log"
    logging.basicConfig(
        level=logging.INFO,
        format="%(asctime)s  %(levelname)-8s  %(name)s  %(message)s",
        handlers=[
            logging.StreamHandler(sys.stdout),
            RotatingFileHandler(
                log_file, maxBytes=5 * 1024 * 1024, backupCount=5, encoding="utf-8"
            ),
        ],
    )

    config = load_config(config_path)
    alerter = SmtpAlerter(config.email)

    app = QApplication(sys.argv)
    app.setStyle("Fusion")

    window = MainWindow(config_path)
    window.show()

    # Init SDK — Sdk.init() always adds a USB detector automatically.
    otc.Sdk.init(otc.Verbosity_Info, otc.Verbosity_Off, sys.argv[0] if sys.argv else "otc_monitor")

    enum_mgr = otc.EnumerationManager.getInstance()

    # Remove the auto-added USB detector if this is an Ethernet-only deployment.
    if not config.runtime.use_usb:
        enum_mgr.clearDetectors()
        LOG.info("USB scanning disabled (use_usb=false)")

    # Add one Ethernet detector per configured subnet.
    for subnet in config.runtime.ethernet_subnets:
        enum_mgr.addEthernetDetector(subnet)
        LOG.info("Ethernet detector added for subnet %s", subnet)

    if not config.runtime.use_usb and not config.runtime.ethernet_subnets:
        LOG.warning("Both USB and Ethernet scanning are disabled — no cameras will be found")

    detected: list[otc.DeviceInfo] = []

    class _StartupEnumClient(otc.EnumerationClient):
        def __init__(self):
            super().__init__()
            otc.EnumerationManager.getInstance().addClient(self)

        def onDeviceDetected(self, info):
            LOG.info("Detected camera S/N %s via %s", info.getSerialNumber(), info.getConnectionInterface())
            detected.append(info)

    enum_client = _StartupEnumClient()  # must stay alive for duration of wait

    workers: list[CameraClient] = []

    def _start_cameras() -> None:
        if not detected:
            LOG.warning("No cameras found after enumeration wait")
            window.no_cameras_found()
            return

        for i, info in enumerate(detected):
            serial = info.getSerialNumber()
            cam_config = config.get_camera_config(serial, i)
            signals = CameraSignals()
            widget = window.add_camera(serial, cam_config.name)

            signals.frame_ready.connect(widget.update_frame)
            signals.alarm_changed.connect(widget.update_alarm_state)
            signals.connection_lost.connect(widget.set_disconnected)

            try:
                client = CameraClient(info, cam_config, config.runtime, alerter, signals)
                client.start()
                workers.append(client)
                LOG.info("Started worker for %s (S/N %s)", cam_config.name, serial)
            except otc.SDKException as exc:
                LOG.error("Cannot connect to S/N %s: %s", serial, exc)
                widget.set_disconnected()

    wait_ms = int(config.runtime.enumeration_wait_seconds * 1000)
    QTimer.singleShot(wait_ms, _start_cameras)

    ret = app.exec_()

    for w in workers:
        w.stop()

    sys.exit(ret)
