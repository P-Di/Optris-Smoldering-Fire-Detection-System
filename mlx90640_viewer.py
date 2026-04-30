"""
MLX90640 live thermal viewer
----------------------------
Reads frames streamed by the ESP32 companion sketch (mlx90640_stream.ino)
over USB serial and displays them in real time.

Frame format from the ESP32 (one line, '\n' terminated):
    FRAME,<Ta>,<t0>,<t1>,...,<t767>

Array layout: 24 rows x 32 cols, row-major, top-left origin.

Usage:
    python mlx90640_viewer.py --port COM5              # Windows
    python mlx90640_viewer.py --port /dev/ttyUSB0      # Linux
    python mlx90640_viewer.py --port /dev/cu.usbserial-XXXX  # macOS

Requirements:
    pip install pyserial numpy matplotlib
"""

import argparse
import sys
import time
from collections import deque

import numpy as np
import serial
import matplotlib.pyplot as plt

ROWS, COLS = 24, 32
PIXELS = ROWS * COLS  # 768
_MLX_H_CROP = int(COLS * 0.25)  # columns trimmed from each horizontal edge (≈25 %)


def parse_frame(line: str):
    """Parse one 'FRAME,Ta,t0,...,t767' line into (Ta, 24x32 ndarray).

    Returns None on malformed input.
    """
    if not line.startswith("FRAME,"):
        return None
    parts = line.strip().split(",")
    # Expect 1 tag + 1 Ta + 768 pixels = 770 tokens
    if len(parts) != 2 + PIXELS:
        return None
    try:
        ta = float(parts[1])
        pixels = np.fromiter(
            (float(p) for p in parts[2:]),
            dtype=np.float32,
            count=PIXELS,
        )
    except ValueError:
        return None
    return ta, pixels.reshape(ROWS, COLS)


def main():
    ap = argparse.ArgumentParser(description="MLX90640 live viewer")
    ap.add_argument("--port", required=True, help="Serial port (e.g. COM5, /dev/ttyUSB0)")
    ap.add_argument("--baud", type=int, default=921600, help="Baud rate (match the sketch)")
    ap.add_argument("--cmap", default="inferno",
                    help="Matplotlib colormap: inferno, magma, plasma, turbo, jet, hot, viridis")
    ap.add_argument("--interp", default="bicubic",
                    help="Display interpolation: nearest, bilinear, bicubic, lanczos")
    ap.add_argument("--auto-range", action="store_true",
                    help="Auto-scale color range per frame (default: rolling range over last ~2s)")
    ap.add_argument("--vmin", type=float, default=None, help="Fixed min temperature for color scale")
    ap.add_argument("--vmax", type=float, default=None, help="Fixed max temperature for color scale")
    ap.add_argument("--mirror", action="store_true",
                    help="Flip horizontally (many breakouts read mirrored)")
    args = ap.parse_args()

    # Open serial
    try:
        ser = serial.Serial(args.port, args.baud, timeout=1)
    except serial.SerialException as e:
        print(f"Could not open {args.port}: {e}", file=sys.stderr)
        sys.exit(1)

    # ESP32 resets when the port opens. Give it a moment, then flush.
    time.sleep(2.0)
    ser.reset_input_buffer()

    # Matplotlib setup (interactive mode, no blocking)
    plt.ion()
    fig, ax = plt.subplots(figsize=(8, 6))
    fig.canvas.manager.set_window_title("MLX90640 Thermal Viewer")

    # Seed with zeros so the image handle exists before first real frame.
    initial = np.zeros((ROWS, COLS), dtype=np.float32)
    vmin = args.vmin if args.vmin is not None else 20.0
    vmax = args.vmax if args.vmax is not None else 35.0
    img = ax.imshow(
        initial,
        cmap=args.cmap,
        interpolation=args.interp,
        vmin=vmin,
        vmax=vmax,
        origin="upper",
    )
    ax.set_xticks([])
    ax.set_yticks([])
    cbar = fig.colorbar(img, ax=ax, label="Temperature (°C)")
    title = ax.set_title("waiting for first frame...")

    # For FPS display and rolling auto-range
    frame_times = deque(maxlen=20)
    recent_mins = deque(maxlen=16)  # ~2s at 8 Hz
    recent_maxs = deque(maxlen=16)

    fixed_range = args.vmin is not None and args.vmax is not None

    print("Connected. Close the window or Ctrl+C to quit.")
    try:
        while plt.fignum_exists(fig.number):
            raw = ser.readline()
            if not raw:
                # No data this tick; let GUI breathe
                plt.pause(0.01)
                continue

            try:
                line = raw.decode("utf-8", errors="replace")
            except Exception:
                continue

            # Surface sketch-side status messages but don't treat them as frames.
            if line.startswith("READY") or line.startswith("ERROR"):
                print(line.rstrip())
                continue

            parsed = parse_frame(line)
            if parsed is None:
                continue
            ta, frame = parsed

            if args.mirror:
                frame = np.fliplr(frame)
            frame = frame[:, _MLX_H_CROP : COLS - _MLX_H_CROP]

            # Color scaling
            fmin = float(frame.min())
            fmax = float(frame.max())
            if fixed_range:
                pass  # leave vmin/vmax as supplied
            elif args.auto_range:
                img.set_clim(fmin, fmax)
            else:
                # Rolling window — smoother than per-frame autoscale, still adapts.
                recent_mins.append(fmin)
                recent_maxs.append(fmax)
                img.set_clim(min(recent_mins), max(recent_maxs))

            img.set_data(frame)

            # FPS
            now = time.time()
            frame_times.append(now)
            if len(frame_times) >= 2:
                fps = (len(frame_times) - 1) / (frame_times[-1] - frame_times[0])
            else:
                fps = 0.0

            title.set_text(
                f"Ta={ta:5.1f}°C   min={fmin:5.1f}   max={fmax:5.1f}   {fps:4.1f} fps"
            )

            fig.canvas.draw_idle()
            fig.canvas.flush_events()

    except KeyboardInterrupt:
        pass
    finally:
        ser.close()
        plt.ioff()
        plt.close("all")


if __name__ == "__main__":
    main()
