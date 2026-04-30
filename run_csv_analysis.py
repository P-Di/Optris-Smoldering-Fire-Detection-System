"""
Run the fire-detection algorithm against a saved pixel CSV.

Replays every frame in order (using the logged monotonic timestamps so the
time-based warning/alarm clocks behave identically to a live run), then
prints every alert with its wall-clock timestamp.

Usage
-----
    python run_csv_analysis.py                          # defaults to thermal_log-5.csv
    python run_csv_analysis.py thermal_log.csv
    python run_csv_analysis.py thermal_log.csv --spike-factor 1.4
"""

from __future__ import annotations

import argparse
import csv
import sys
from pathlib import Path

import numpy as np

ROOT = Path(__file__).resolve().parent
SRC  = ROOT / "src"
if str(SRC) not in sys.path:
    sys.path.insert(0, str(SRC))

import optris_alert_daemon.pixel_fire_monitor as _pfm
from optris_alert_daemon.pixel_fire_monitor import (
    AbsoluteThresholdFrameAnalyzer,
    FireMonitorConfig,
    FrameAnalyzer,
)

_SENSOR_ROWS = 24


# ---------------------------------------------------------------------------
# CSV loader
# ---------------------------------------------------------------------------

def load_csv(path: Path):
    """Return (timestamps, monotonic_s, frames ndarray (N, ROWS, COLS))."""
    with open(path, newline="", encoding="utf-8") as fh:
        reader = csv.DictReader(fh)
        if reader.fieldnames is None:
            sys.exit(f"Empty or headerless CSV: {path}")

        pixel_indices = sorted(
            int(f[1:])
            for f in reader.fieldnames
            if f.startswith("p") and f[1:].isdigit()
        )
        if not pixel_indices:
            sys.exit(
                f"No pixel columns found in {path}.\n"
                "Re-log with --save-pixels."
            )
        n_pixels = len(pixel_indices)
        if n_pixels % _SENSOR_ROWS != 0:
            sys.exit(f"Pixel count {n_pixels} not divisible by {_SENSOR_ROWS}.")

        timestamps:  list[str]   = []
        monotonics:  list[float] = []
        frame_rows:  list[list[float]] = []

        for row in reader:
            timestamps.append(row.get("timestamp", ""))
            try:
                monotonics.append(float(row.get("monotonic_s", "nan") or "nan"))
            except ValueError:
                monotonics.append(float("nan"))
            try:
                frame_rows.append([float(row[f"p{i}"]) for i in pixel_indices])
            except (KeyError, ValueError):
                frame_rows.append([float("nan")] * n_pixels)

    if not frame_rows:
        sys.exit(f"No data rows in {path}.")

    cols   = n_pixels // _SENSOR_ROWS
    frames = np.array(frame_rows, dtype=np.float32).reshape(len(frame_rows), _SENSOR_ROWS, cols)
    return timestamps, np.array(monotonics, dtype=np.float64), frames


# ---------------------------------------------------------------------------
# Main
# ---------------------------------------------------------------------------

def main() -> None:
    ap = argparse.ArgumentParser(
        description="Replay a pixel CSV through the fire-detection algorithm"
    )
    ap.add_argument(
        "csv", metavar="FILE", nargs="?", default="thermal_log-5.csv",
        help="CSV logged with --save-pixels  (default: thermal_log-5.csv)"
    )
    # Expose the same tuning knobs as run_viewer_with_monitor.py
    ap.add_argument(
        "--algo", choices=["spike", "absolute"], default="spike",
        help="Detection algorithm: 'spike' (default) uses temp_diff baseline; "
             "'absolute' raises a candidate when any pixel exceeds --abs-threshold",
    )
    ap.add_argument("--abs-threshold", type=float, default=30.0,
                    help="Absolute temperature threshold °C for --algo absolute  (default: 30.0)")
    ap.add_argument("--spike-factor",  type=float, default=1.6)
    ap.add_argument("--warning-rise",  type=float, default=1.5)
    ap.add_argument("--alarm-rise",    type=float, default=3.0)
    ap.add_argument("--warning-time",  type=float, default=120.0)
    ap.add_argument("--alarm-time",    type=float, default=300.0)
    ap.add_argument("--ambient-spike", type=float, default=4.0)
    args = ap.parse_args()

    path = Path(args.csv)
    if not path.exists():
        sys.exit(f"File not found: {path}")

    print(f"Loading {path} …", end=" ", flush=True)
    timestamps, monotonics, frames = load_csv(path)
    n_frames, rows, cols = frames.shape
    print(f"{n_frames} frames  •  {rows}×{cols} pixels")

    # Patch the module-level COLS so argmax unpacking and neighbourhood
    # bounds use the actual (possibly cropped) frame width, not the default 32.
    _pfm.COLS = cols
    _pfm.ROWS = rows

    config = FireMonitorConfig(
        spike_factor=args.spike_factor,
        warning_rise_c=args.warning_rise,
        alarm_rise_c=args.alarm_rise,
        warning_window_s=args.warning_time,
        alarm_window_s=args.alarm_time,
        ambient_spike_c=args.ambient_spike,
        absolute_threshold_c=args.abs_threshold,
    )

    if args.algo == "absolute":
        analyzer = AbsoluteThresholdFrameAnalyzer(config)
        algo_desc = (
            f"absolute threshold >{config.absolute_threshold_c}°C  "
            f"warn +{config.warning_rise_c}°C/{config.warning_window_s}s  "
            f"alarm +{config.alarm_rise_c}°C/{config.alarm_window_s}s"
        )
    else:
        analyzer = FrameAnalyzer(config)
        algo_desc = (
            f"spike×{config.spike_factor}  "
            f"warn +{config.warning_rise_c}°C/{config.warning_window_s}s  "
            f"alarm +{config.alarm_rise_c}°C/{config.alarm_window_s}s"
        )

    all_alerts: list[tuple[str, object]] = []

    print(f"\nRunning algorithm [{args.algo}]  ({algo_desc}) …\n")

    for i, (frame, mono) in enumerate(zip(frames, monotonics)):
        now = float(mono) if not np.isnan(mono) else float(i) / 8.0
        alerts = analyzer.process(frame, now)
        for alert in alerts:
            ts = timestamps[i] if i < len(timestamps) else f"frame {i}"
            all_alerts.append((ts, alert))
            print(f"  [{ts}]  {alert.message}")

    # --- Summary ---
    print()
    print("=" * 70)
    if not all_alerts:
        print("No alerts triggered across all frames.")
    else:
        print(f"{len(all_alerts)} alert(s) triggered:\n")
        from optris_alert_daemon.pixel_fire_monitor import AlertLevel
        for level in (AlertLevel.ALARM, AlertLevel.WARNING, AlertLevel.AMBIENT_HIGH):
            subset = [(ts, a) for ts, a in all_alerts if a.level == level]
            if subset:
                print(f"  {level.value.upper()}  ({len(subset)} occurrence(s))")
                print(f"    First : {subset[0][0]}")
                print(f"    Last  : {subset[-1][0]}")
                print()
    print("=" * 70)


if __name__ == "__main__":
    main()
