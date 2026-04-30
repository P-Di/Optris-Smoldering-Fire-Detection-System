#!/usr/bin/env python3
"""
Process watchdog for the Optris Thermal Monitor.

Runs run_monitor.py in a subprocess and restarts it automatically if it
crashes (non-zero exit code).  A clean exit (code 0, user closed the window)
stops the watchdog — the monitor is not restarted against the user's intent.

Usage:  python watchdog.py [config/monitor.json]
Point the desktop shortcut at this file, not run_monitor.py directly.
"""
from __future__ import annotations

import logging
import subprocess
import sys
import time
from logging.handlers import RotatingFileHandler
from pathlib import Path

RESTART_DELAY_S   = 5     # seconds to wait before restarting after a crash
MAX_RESTART_DELAY = 60    # cap for exponential backoff (seconds)
MAX_RESTARTS      = 0     # 0 = unlimited restarts

logging.basicConfig(
    level=logging.INFO,
    format="%(asctime)s  %(levelname)-8s  watchdog  %(message)s",
    handlers=[
        logging.StreamHandler(sys.stdout),
        RotatingFileHandler(
            Path(__file__).parent / "watchdog.log",
            maxBytes=2 * 1024 * 1024, backupCount=3, encoding="utf-8"
        ),
    ],
)
LOG = logging.getLogger("watchdog")

_HERE   = Path(__file__).parent
_PYTHON = _HERE / ".venv" / "Scripts" / "python.exe"
_SCRIPT = _HERE / "run_monitor.py"


def _python_exe() -> Path:
    if _PYTHON.exists():
        return _PYTHON
    return Path(sys.executable)  # fall back to whichever Python launched this


def main() -> None:
    config = sys.argv[1] if len(sys.argv) > 1 else str(_HERE / "config" / "monitor.json")
    cmd = [str(_python_exe()), str(_SCRIPT), config]

    restart_count = 0
    delay = RESTART_DELAY_S

    while True:
        LOG.info("Launching monitor (attempt %d): %s", restart_count + 1, " ".join(cmd))
        start = time.monotonic()

        try:
            result = subprocess.run(cmd)
            exit_code = result.returncode
        except Exception as exc:
            LOG.error("Failed to launch monitor: %s", exc)
            exit_code = -1

        elapsed = time.monotonic() - start

        if exit_code == 0:
            LOG.info("Monitor exited cleanly (code 0) after %.0fs — watchdog stopping.", elapsed)
            break

        restart_count += 1
        LOG.warning(
            "Monitor exited with code %d after %.0fs (crash #%d). "
            "Restarting in %ds…",
            exit_code, elapsed, restart_count, delay,
        )

        if MAX_RESTARTS and restart_count >= MAX_RESTARTS:
            LOG.error("Reached MAX_RESTARTS (%d). Giving up.", MAX_RESTARTS)
            break

        time.sleep(delay)
        delay = min(delay * 1.5, MAX_RESTART_DELAY)  # exponential backoff, capped


if __name__ == "__main__":
    main()
