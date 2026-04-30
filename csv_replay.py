"""
MLX90640 CSV thermal replay viewer
------------------------------------
Reads a pixel CSV logged with run_viewer_with_monitor.py --save-pixels and
replays the frames as a matplotlib heatmap with playback controls.

Controls
--------
  Space           play / pause
  Left / Right    step one frame back / forward
  + / -           speed up / slow down playback
  Slider          scrub to any frame

Usage
-----
    python csv_replay.py thermal_log.csv
    python csv_replay.py thermal_log.csv --cmap plasma --fps 4
    python csv_replay.py thermal_log.csv --vmin 20 --vmax 40
"""

from __future__ import annotations

import argparse
import csv
import sys
import time
from pathlib import Path

import matplotlib.pyplot as plt
import matplotlib.widgets as mwidgets
import numpy as np

_SENSOR_ROWS = 24


def load_csv(path: Path):
    """Return (timestamps, ta_arr, frames ndarray shape (N, ROWS, COLS))."""
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
                f"No pixel columns (p0, p1, …) found in {path}.\n"
                "Re-log with --save-pixels."
            )
        n_pixels = len(pixel_indices)
        if n_pixels % _SENSOR_ROWS != 0:
            sys.exit(
                f"Pixel count {n_pixels} is not divisible by {_SENSOR_ROWS} rows."
            )

        timestamps: list[str] = []
        ta_vals: list[float] = []
        frame_rows: list[list[float]] = []

        for row in reader:
            timestamps.append(row.get("timestamp", ""))
            try:
                ta_vals.append(float(row.get("ta", "nan") or "nan"))
            except ValueError:
                ta_vals.append(float("nan"))
            try:
                frame_rows.append([float(row[f"p{i}"]) for i in pixel_indices])
            except (KeyError, ValueError):
                frame_rows.append([float("nan")] * n_pixels)

    n_frames = len(frame_rows)
    if n_frames == 0:
        sys.exit(f"No data rows found in {path}.")

    cols = n_pixels // _SENSOR_ROWS
    frames = np.array(frame_rows, dtype=np.float32).reshape(n_frames, _SENSOR_ROWS, cols)
    return timestamps, np.array(ta_vals, dtype=np.float32), frames


def main() -> None:
    ap = argparse.ArgumentParser(
        description="Replay MLX90640 pixel CSV as a matplotlib heatmap"
    )
    ap.add_argument("csv", metavar="FILE", help="CSV logged with --save-pixels")
    ap.add_argument("--cmap",   default="inferno",
                    help="Colormap  (default: inferno)")
    ap.add_argument("--interp", default="bicubic",
                    help="Interpolation  (default: bicubic)")
    ap.add_argument("--fps",    type=float, default=8.0,
                    help="Playback speed in frames/sec  (default: 8)")
    ap.add_argument("--vmin",   type=float, default=None,
                    help="Fixed colour-scale min °C  (default: per-frame auto)")
    ap.add_argument("--vmax",   type=float, default=None,
                    help="Fixed colour-scale max °C  (default: per-frame auto)")
    args = ap.parse_args()

    path = Path(args.csv)
    if not path.exists():
        sys.exit(f"File not found: {path}")

    print(f"Loading {path} …", end=" ", flush=True)
    timestamps, ta_arr, frames = load_csv(path)
    n_frames, rows, cols = frames.shape
    print(f"{n_frames} frames  •  {rows}×{cols} pixels each  •  +/- to adjust speed")

    fixed_range = args.vmin is not None and args.vmax is not None

    # --- state ----------------------------------------------------------
    state = {"idx": 0, "playing": False, "fps": args.fps}

    # --- figure layout --------------------------------------------------
    fig = plt.figure(figsize=(9, 7))
    fig.canvas.manager.set_window_title("MLX90640 CSV Replay")

    ax_img  = fig.add_axes([0.05, 0.22, 0.88, 0.70])
    ax_sldr = fig.add_axes([0.10, 0.10, 0.80, 0.04])
    ax_btn  = fig.add_axes([0.43, 0.02, 0.14, 0.06])

    frame0 = frames[0]
    vmin0 = args.vmin if fixed_range else float(np.nanmin(frame0))
    vmax0 = args.vmax if fixed_range else float(np.nanmax(frame0))

    img = ax_img.imshow(
        frame0,
        cmap=args.cmap,
        interpolation=args.interp,
        vmin=vmin0, vmax=vmax0,
        origin="upper",
        aspect="auto",
    )
    ax_img.set_xticks(np.arange(cols))
    ax_img.set_yticks(np.arange(rows))
    ax_img.set_xticklabels(np.arange(cols), fontsize=7)
    ax_img.set_yticklabels(np.arange(rows), fontsize=7)
    ax_img.set_xticks(np.arange(cols) - 0.5, minor=True)
    ax_img.set_yticks(np.arange(rows) - 0.5, minor=True)
    ax_img.grid(which="minor", color="white", linewidth=0.4, alpha=0.4)
    ax_img.tick_params(which="minor", length=0)
    fig.colorbar(img, ax=ax_img, label="Temperature (°C)", fraction=0.03, pad=0.02)
    title_obj = ax_img.set_title("")

    hover_text = ax_img.text(
        0.01, 0.99, "", transform=ax_img.transAxes,
        va="top", ha="left", fontsize=8, color="white",
        bbox=dict(boxstyle="round,pad=0.2", facecolor="black", alpha=0.6),
    )

    slider = mwidgets.Slider(
        ax_sldr, "Frame", 0, n_frames - 1,
        valinit=0, valstep=1, color="steelblue",
    )
    btn = mwidgets.Button(ax_btn, "▶  Play")

    # --- render ---------------------------------------------------------
    def render(idx: int) -> None:
        frame = frames[idx]
        if fixed_range:
            img.set_clim(args.vmin, args.vmax)
        else:
            img.set_clim(float(np.nanmin(frame)), float(np.nanmax(frame)))
        img.set_data(frame)

        ts  = timestamps[idx] if idx < len(timestamps) else ""
        ta  = float(ta_arr[idx]) if idx < len(ta_arr) else float("nan")
        fmn = float(np.nanmin(frame))
        fmx = float(np.nanmax(frame))
        title_obj.set_text(
            f"{ts}   Ta={ta:.1f}°C   min={fmn:.1f}   max={fmx:.1f}°C   "
            f"frame {idx + 1}/{n_frames}   {state['fps']:.1f} fps"
        )
        # Suppress recursive on_slider callback
        slider.eventson = False
        slider.set_val(idx)
        slider.eventson = True

        fig.canvas.draw_idle()

    # --- controls -------------------------------------------------------
    def on_slider(val: float) -> None:
        state["idx"] = int(val)
        render(state["idx"])

    slider.on_changed(on_slider)

    def toggle_play(event=None) -> None:
        state["playing"] = not state["playing"]
        # Restart from beginning if we reached the end
        if state["playing"] and state["idx"] >= n_frames - 1:
            state["idx"] = 0
        btn.label.set_text("⏸  Pause" if state["playing"] else "▶  Play")
        fig.canvas.draw_idle()

    btn.on_clicked(toggle_play)

    def on_key(event) -> None:
        if event.key == " ":
            toggle_play()
        elif event.key == "right":
            state["playing"] = False
            btn.label.set_text("▶  Play")
            state["idx"] = min(state["idx"] + 1, n_frames - 1)
            render(state["idx"])
        elif event.key == "left":
            state["playing"] = False
            btn.label.set_text("▶  Play")
            state["idx"] = max(state["idx"] - 1, 0)
            render(state["idx"])
        elif event.key in ("+", "="):
            state["fps"] = min(state["fps"] * 1.5, 60.0)
            render(state["idx"])
        elif event.key == "-":
            state["fps"] = max(state["fps"] / 1.5, 0.5)
            render(state["idx"])

    fig.canvas.mpl_connect("key_press_event", on_key)

    def on_mouse_move(event) -> None:
        if event.inaxes is not ax_img:
            hover_text.set_text("")
            fig.canvas.draw_idle()
            return
        col = int(round(event.xdata))
        row = int(round(event.ydata))
        if 0 <= col < cols and 0 <= row < rows:
            pixel_idx = row * cols + col
            temp = float(frames[state["idx"]][row, col])
            hover_text.set_text(f"p{pixel_idx}  col={col} row={row}  {temp:.2f}°C")
        else:
            hover_text.set_text("")
        fig.canvas.draw_idle()

    fig.canvas.mpl_connect("motion_notify_event", on_mouse_move)

    # --- playback timer (30 ms poll, rate-limited by fps) ---------------
    last_tick: list[float] = [time.monotonic()]

    def tick(_event=None) -> None:
        if not state["playing"]:
            return
        now = time.monotonic()
        if now - last_tick[0] < 1.0 / state["fps"]:
            return
        last_tick[0] = now
        next_idx = state["idx"] + 1
        if next_idx >= n_frames:
            state["playing"] = False
            btn.label.set_text("▶  Play")
            fig.canvas.draw_idle()
            return
        state["idx"] = next_idx
        render(next_idx)

    timer = fig.canvas.new_timer(interval=30)
    timer.add_callback(tick)
    timer.start()

    render(0)
    plt.show()


if __name__ == "__main__":
    main()
