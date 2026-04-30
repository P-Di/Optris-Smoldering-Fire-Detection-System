"""
Unified MLX90640 viewer + fire monitor + CSV logger.

Reads the serial stream once and simultaneously:
  - Displays the live thermal image in matplotlib (same as mlx90640_viewer.py)
  - Runs the fire detection algorithm inline (negligible latency on 24x32)
  - Optionally logs every frame to a CSV file

Overlay markers on the image:
  Yellow +  = CANDIDATE  (spike detected, observation window running)
  Orange +  = WARNING    (elevated for warning_window_s seconds)
  Red    +  = ALARM      (elevated for alarm_window_s seconds)

Usage
-----
    python run_viewer_with_monitor.py --port COM5
    python run_viewer_with_monitor.py --port COM5 --csv thermal_log.csv
    python run_viewer_with_monitor.py --port COM5 --csv thermal_log.csv --save-pixels
    python run_viewer_with_monitor.py --port COM5 --warning-time 600 --alarm-time 600
"""

from __future__ import annotations

import argparse
import csv
import datetime
import sys
import time
from collections import deque
from pathlib import Path

import matplotlib.pyplot as plt
import numpy as np

ROOT = Path(__file__).resolve().parent
SRC = ROOT / "src"
if str(SRC) not in sys.path:
    sys.path.insert(0, str(SRC))

from optris_alert_daemon.frame_source import COLS, ROWS, MLX90640FrameSource

_MLX_H_CROP = int(COLS * 0.25)  # columns trimmed from each horizontal edge (≈25 %)
from optris_alert_daemon.pixel_fire_monitor import (
    AlertLevel,
    FireMonitorConfig,
    FrameAnalyzer,
)

_OVERLAY_COLOR = {
    "candidate": "yellow",
    "warning":   "orange",
    "alarm":     "red",
}
_ALERT_STYLE = {
    AlertLevel.ALARM:        ("red",    "white"),
    AlertLevel.WARNING:      ("orange", "black"),
    AlertLevel.AMBIENT_HIGH: ("yellow", "black"),
}
_ALERT_PRIORITY = {
    AlertLevel.ALARM: 3, AlertLevel.WARNING: 2, AlertLevel.AMBIENT_HIGH: 1,
}


# ---------------------------------------------------------------------------
# CSV helpers
# ---------------------------------------------------------------------------

_SUMMARY_FIELDS = [
    "timestamp", "monotonic_s", "ta",
    "frame_min", "frame_avg", "frame_median", "frame_max", "temp_diff",
    "alert_level", "alert_pixel_row", "alert_pixel_col", "alert_rise_c",
    "alert_message",
]


def _open_csv(
    path: Path, save_pixels: bool
) -> tuple[object, csv.DictWriter]:
    fields = _SUMMARY_FIELDS.copy()
    if save_pixels:
        fields += [f"p{i}" for i in range(ROWS * (COLS - 2 * _MLX_H_CROP))]
    new_file = not path.exists() or path.stat().st_size == 0
    fh = open(path, "a", newline="", encoding="utf-8")
    writer = csv.DictWriter(fh, fieldnames=fields)
    if new_file:
        writer.writeheader()
    return fh, writer


def _csv_row(
    ta: float,
    frame: np.ndarray,
    alerts: list,
    monotonic: float,
    save_pixels: bool,
) -> dict:
    top = (max(alerts, key=lambda a: _ALERT_PRIORITY.get(a.level, 0))
           if alerts else None)
    row: dict = {
        "timestamp":      datetime.datetime.now().isoformat(timespec="milliseconds"),
        "monotonic_s":    f"{monotonic:.3f}",
        "ta":             f"{ta:.2f}",
        "frame_min":      f"{float(frame.min()):.2f}",
        "frame_avg":      f"{float(frame.mean()):.2f}",
        "frame_median":   f"{float(np.median(frame)):.2f}",
        "frame_max":      f"{float(frame.max()):.2f}",
        "temp_diff":      f"{float(frame.max() - frame.mean()):.2f}",
        "alert_level":    top.level.value if top else "",
        "alert_pixel_row": top.pixel_row  if top else "",
        "alert_pixel_col": top.pixel_col  if top else "",
        "alert_rise_c":   f"{top.rise_c:.2f}" if top else "",
        # replace commas so the message doesn't break CSV parsing
        "alert_message":  top.message.replace(",", ";") if top else "",
    }
    if save_pixels:
        for i, v in enumerate(frame.ravel()):
            row[f"p{i}"] = f"{v:.2f}"
    return row


# ---------------------------------------------------------------------------
# Main
# ---------------------------------------------------------------------------

def main() -> None:
    ap = argparse.ArgumentParser(
        description="MLX90640 live viewer + fire monitor + CSV logger"
    )
    # Serial
    ap.add_argument("--port",  required=True, help="Serial port e.g. COM5")
    ap.add_argument("--baud",  type=int, default=921600)

    # Display (same options as mlx90640_viewer.py)
    ap.add_argument("--cmap",       default="inferno",
                    help="Colormap: inferno, magma, plasma, turbo, jet, hot, viridis")
    ap.add_argument("--interp",     default="bicubic",
                    help="Interpolation: nearest, bilinear, bicubic, lanczos")
    ap.add_argument("--auto-range", action="store_true",
                    help="Auto-scale color range per frame")
    ap.add_argument("--vmin",       type=float, default=None)
    ap.add_argument("--vmax",       type=float, default=None)
    ap.add_argument("--mirror",     action="store_true",
                    help="Flip image horizontally")

    # CSV logging
    ap.add_argument("--csv",         metavar="FILE", default=None,
                    help="Path to CSV log file (appends if file already exists)")
    ap.add_argument("--save-pixels", action="store_true",
                    help="Include all 768 pixel temps in every CSV row (~800 columns)")

    # Fire monitor tuning
    ap.add_argument("--spike-factor",  type=float, default=1.6,
                    help="temp_diff multiplier to flag a spike (default 1.6)")
    ap.add_argument("--warning-rise",  type=float, default=1.5,
                    help="°C above baseline to start warning clock (default 1.5)")
    ap.add_argument("--alarm-rise",    type=float, default=3.0,
                    help="°C above baseline to start alarm clock (default 3.0)")
    ap.add_argument("--warning-time",  type=float, default=120.0,
                    help="Seconds of sustained rise before WARNING (default 120 = 2 min)")
    ap.add_argument("--alarm-time",    type=float, default=300.0,
                    help="Seconds of sustained rise before ALARM (default 300 = 5 min)")
    ap.add_argument("--ambient-spike", type=float, default=4.0,
                    help="°C scene-avg rise that triggers ambient check (default 4.0)")
    args = ap.parse_args()

    # Fire monitor
    config = FireMonitorConfig(
        spike_factor=args.spike_factor,
        warning_rise_c=args.warning_rise,
        alarm_rise_c=args.alarm_rise,
        warning_window_s=args.warning_time,
        alarm_window_s=args.alarm_time,
        ambient_spike_c=args.ambient_spike,
    )
    analyzer = FrameAnalyzer(config)

    # CSV
    csv_fh = csv_writer = None
    if args.csv:
        csv_path = Path(args.csv)
        csv_fh, csv_writer = _open_csv(csv_path, args.save_pixels)
        pixel_note = " (+pixels)" if args.save_pixels else ""
        print(f"CSV logging{pixel_note} → {csv_path}")

    # Serial source
    source = MLX90640FrameSource(port=args.port, baud=args.baud)
    source.open()

    # Matplotlib setup
    plt.ion()
    fig, ax = plt.subplots(figsize=(8, 6))
    fig.canvas.manager.set_window_title("MLX90640 — Viewer + Fire Monitor")

    vmin = args.vmin if args.vmin is not None else 20.0
    vmax = args.vmax if args.vmax is not None else 35.0
    img = ax.imshow(
        np.zeros((ROWS, COLS), dtype=np.float32),
        cmap=args.cmap,
        interpolation=args.interp,
        vmin=vmin, vmax=vmax,
        origin="upper",
    )
    ax.set_xticks([])
    ax.set_yticks([])
    fig.colorbar(img, ax=ax, label="Temperature (°C)")
    title_obj = ax.set_title("waiting for first frame…")

    # Alert banner — sits at the bottom of the axes, invisible until needed
    alert_text = ax.text(
        0.5, 0.02, "",
        transform=ax.transAxes,
        ha="center", va="bottom",
        fontsize=12, fontweight="bold",
        color="white",
        bbox=dict(boxstyle="round,pad=0.3", facecolor="black", alpha=0.0),
    )

    frame_times: deque[float] = deque(maxlen=20)
    recent_mins: deque[float] = deque(maxlen=16)
    recent_maxs: deque[float] = deque(maxlen=16)
    fixed_range = args.vmin is not None and args.vmax is not None
    overlay_artists: list = []

    print("Running. Close the window or Ctrl+C to stop.")
    try:
        while plt.fignum_exists(fig.number):
            result = source.read_frame()
            if result is None:
                plt.pause(0.01)
                continue

            ta, frame = result
            if args.mirror:
                frame = np.fliplr(frame)
            frame = frame[:, _MLX_H_CROP : COLS - _MLX_H_CROP]
            now = time.monotonic()

            # ── Fire monitor (inline, ~microseconds for 24×32) ────────────
            alerts = analyzer.process(frame, now)
            for alert in alerts:
                ts = time.strftime("%H:%M:%S")
                print(f"[{ts}] {alert.message}")

            # ── CSV row ───────────────────────────────────────────────────
            if csv_writer is not None:
                csv_writer.writerow(_csv_row(ta, frame, alerts, now, args.save_pixels))
                csv_fh.flush()  # type: ignore[union-attr]

            # ── Image update ──────────────────────────────────────────────
            fmin, fmax = float(frame.min()), float(frame.max())
            if fixed_range:
                pass
            elif args.auto_range:
                img.set_clim(fmin, fmax)
            else:
                recent_mins.append(fmin)
                recent_maxs.append(fmax)
                img.set_clim(min(recent_mins), max(recent_maxs))
            img.set_data(frame)

            # FPS counter in title
            frame_times.append(now)
            fps = (
                (len(frame_times) - 1) / (frame_times[-1] - frame_times[0])
                if len(frame_times) >= 2 else 0.0
            )
            title_obj.set_text(
                f"Ta={ta:5.1f}°C   min={fmin:5.1f}   max={fmax:5.1f}   {fps:4.1f} fps"
            )

            # ── Hotspot overlays ──────────────────────────────────────────
            for art in overlay_artists:
                art.remove()
            overlay_artists.clear()

            for row, col, state in analyzer.active_trackers:
                color = _OVERLAY_COLOR.get(state, "white")
                (art,) = ax.plot(
                    col, row, "+",
                    color=color, markersize=24, markeredgewidth=2.5,
                )
                overlay_artists.append(art)

            # ── Alert banner ──────────────────────────────────────────────
            if alerts:
                worst = max(alerts, key=lambda a: _ALERT_PRIORITY.get(a.level, 0))
                bg, fg = _ALERT_STYLE[worst.level]
                alert_text.set_text(f"  {worst.level.value.upper()}  ")
                alert_text.set_color(fg)
                patch = alert_text.get_bbox_patch()
                patch.set_facecolor(bg)
                patch.set_alpha(0.88)
            else:
                alert_text.set_text("")
                alert_text.get_bbox_patch().set_alpha(0.0)

            fig.canvas.draw_idle()
            fig.canvas.flush_events()

    except KeyboardInterrupt:
        pass
    finally:
        source.close()
        if csv_fh:
            csv_fh.close()
            print(f"\nCSV closed → {args.csv}")
        plt.ioff()
        plt.close("all")


if __name__ == "__main__":
    main()
