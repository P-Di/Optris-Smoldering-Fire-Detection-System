"""
Fire monitor CLI — MLX90640 test mode.

Runs the pixel-level fire detection algorithm against an MLX90640 connected
via an ESP32 over USB serial.  Prints alerts to the console.  The monitor
runs in a separate process so it cannot block this terminal loop.

Usage
-----
    python run_fire_monitor.py --port COM5
    python run_fire_monitor.py --port COM5 --use-thread       # thread instead of process
    python run_fire_monitor.py --port COM5 --warning-rise 2.0 --alarm-rise 4.0

Swapping to Optris
------------------
When OptrisFrameSource raw-frame bindings are ready (see frame_source.py),
change the start_monitor_process call to:

    proc, alert_q, stop_ev = start_monitor_process(
        source_type="optris",
        source_kwargs={...},     # instance_name, dll_path, etc.
        config=config,
    )

The algorithm is identical — only the source changes.
"""

from __future__ import annotations

import argparse
import multiprocessing
import queue
import sys
import time
from pathlib import Path

ROOT = Path(__file__).resolve().parent
SRC = ROOT / "src"
if str(SRC) not in sys.path:
    sys.path.insert(0, str(SRC))

from optris_alert_daemon.frame_source import MLX90640FrameSource
from optris_alert_daemon.pixel_fire_monitor import (
    AlertLevel,
    FireMonitorConfig,
    start_monitor_process,
    start_monitor_thread,
)

_LEVEL_PREFIX = {
    AlertLevel.AMBIENT_HIGH: "[AMBIENT ]",
    AlertLevel.WARNING:      "[ WARNING ]",
    AlertLevel.ALARM:        "[** ALARM **]",
}


def main() -> None:
    ap = argparse.ArgumentParser(description="Thermal fire monitor (MLX90640 mode)")
    ap.add_argument("--port", required=True, help="Serial port e.g. COM5")
    ap.add_argument("--baud", type=int, default=921600)
    ap.add_argument(
        "--use-thread", action="store_true",
        help="Use a thread instead of a subprocess (lower overhead, shares GIL)",
    )

    # Expose the most useful tuning knobs as CLI flags
    ap.add_argument("--spike-factor",    type=float, default=1.6,
                    help="temp_diff multiplier to trigger candidate detection (default 1.6)")
    ap.add_argument("--min-diff",        type=float, default=3.0,
                    help="Minimum temp_diff °C to care about (default 3.0)")
    ap.add_argument("--warning-rise",    type=float, default=1.5,
                    help="°C rise from baseline required to count toward WARNING (default 1.5)")
    ap.add_argument("--alarm-rise",      type=float, default=3.0,
                    help="°C rise from baseline required to count toward ALARM (default 3.0)")
    ap.add_argument("--warning-time",    type=float, default=120.0,
                    help="Seconds temp must stay above warning-rise before WARNING alert (default 120 = 2 min)")
    ap.add_argument("--alarm-time",      type=float, default=300.0,
                    help="Seconds temp must stay above alarm-rise before ALARM alert (default 300 = 5 min)")
    ap.add_argument("--ambient-spike",   type=float, default=4.0,
                    help="°C above rolling avg to trigger ambient check (default 4.0)")
    args = ap.parse_args()

    config = FireMonitorConfig(
        spike_factor=args.spike_factor,
        min_temp_diff_c=args.min_diff,
        warning_rise_c=args.warning_rise,
        alarm_rise_c=args.alarm_rise,
        warning_window_s=args.warning_time,
        alarm_window_s=args.alarm_time,
        ambient_spike_c=args.ambient_spike,
    )

    mode = "thread" if args.use_thread else "process"
    print(f"Starting fire monitor on {args.port} ({mode} mode). Ctrl+C to stop.\n")

    if args.use_thread:
        source = MLX90640FrameSource(port=args.port, baud=args.baud)
        worker, alert_q, stop_ev = start_monitor_thread(source, config)
    else:
        worker, alert_q, stop_ev = start_monitor_process(
            source_type="mlx90640",
            source_kwargs={"port": args.port, "baud": args.baud},
            config=config,
        )

    try:
        while True:
            try:
                alert = alert_q.get(timeout=1.0)
            except (queue.Empty, Exception):
                continue

            ts = time.strftime("%H:%M:%S")
            prefix = _LEVEL_PREFIX.get(alert.level, f"[{alert.level.value}]")
            print(f"{ts} {prefix} {alert.message}")

            if alert.level is AlertLevel.ALARM:
                # Additional detail for alarms
                print(
                    f"         baseline={alert.baseline_temp_c:.1f}°C  "
                    f"current={alert.current_temp_c:.1f}°C  "
                    f"rise={alert.rise_c:.1f}°C"
                )

    except KeyboardInterrupt:
        print("\nShutting down…")
    finally:
        stop_ev.set()
        worker.join(timeout=5.0)
        print("Stopped.")


if __name__ == "__main__":
    # Required on Windows when using multiprocessing spawn method
    multiprocessing.freeze_support()
    main()
