"""
Camera frame source abstraction.

Swap MLX90640FrameSource for OptrisFrameSource when raw-frame SDK
bindings are available in optris_sdk.py.
"""

from __future__ import annotations

import time
from abc import ABC, abstractmethod

import numpy as np
import serial

ROWS, COLS = 24, 32
_PIXELS = ROWS * COLS  # 768


class FrameSource(ABC):
    """Produce successive (ambient_temp_c, 24×32 float32 ndarray) pairs."""

    @abstractmethod
    def open(self) -> None: ...

    @abstractmethod
    def close(self) -> None: ...

    @abstractmethod
    def read_frame(self) -> tuple[float, np.ndarray] | None:
        """Return (Ta, frame) or None if no data is ready this tick."""
        ...

    def __enter__(self):
        self.open()
        return self

    def __exit__(self, *_):
        self.close()


# ---------------------------------------------------------------------------
# MLX90640 via ESP32 serial stream
# ---------------------------------------------------------------------------

class MLX90640FrameSource(FrameSource):
    """
    Reads frames from an ESP32 running mlx90640_stream.ino over USB serial.

    Wire format (one UTF-8 line, newline-terminated):
        FRAME,<Ta>,<t0>,<t1>,...,<t767>

    Array layout: 24 rows × 32 cols, row-major, top-left origin.
    """

    def __init__(self, port: str, baud: int = 921600, timeout: float = 1.0):
        self.port = port
        self.baud = baud
        self.timeout = timeout
        self._ser: serial.Serial | None = None

    def open(self) -> None:
        self._ser = serial.Serial(self.port, self.baud, timeout=self.timeout)
        time.sleep(2.0)               # wait for ESP32 reboot after DTR reset
        self._ser.reset_input_buffer()

    def close(self) -> None:
        if self._ser and self._ser.is_open:
            self._ser.close()

    def read_frame(self) -> tuple[float, np.ndarray] | None:
        if not self._ser:
            return None
        raw = self._ser.readline()
        if not raw:
            return None
        try:
            line = raw.decode("utf-8", errors="replace")
        except Exception:
            return None
        return self._parse(line)

    @classmethod
    def _parse(cls, line: str) -> tuple[float, np.ndarray] | None:
        if not line.startswith("FRAME,"):
            return None
        parts = line.strip().split(",")
        if len(parts) != 2 + _PIXELS:
            return None
        try:
            ta = float(parts[1])
            pixels = np.fromiter(
                (float(p) for p in parts[2:]),
                dtype=np.float32,
                count=_PIXELS,
            )
        except ValueError:
            return None
        return ta, pixels.reshape(ROWS, COLS)


# ---------------------------------------------------------------------------
# Optris PIX Connect (stub — raw frame bindings not yet implemented)
# ---------------------------------------------------------------------------

class OptrisFrameSource(FrameSource):
    """
    Swap-in stub for Optris PIX Connect pixel-level integration.

    WHY IT IS A STUB
    ----------------
    The current optris_sdk.py only binds the *measure-area* scalar API
    (GetTempMeasureArea / GetAlarmThreshold).  That API returns one
    temperature per pre-defined region — individual pixels are not
    accessible through it.

    Getting surrounding-pixel data from the Optris requires binding the
    raw radiometric frame functions from the PIX Connect SDK DLL, for
    example GetTempImage (or equivalent).  Those functions are not yet
    bound.

    HOW TO ACTIVATE
    ---------------
    1. In optris_sdk.py, bind the raw-frame acquisition function and add
       a helper method (e.g. OptrisSession.read_frame() -> np.ndarray).
    2. Implement open(), close(), and read_frame() below using that helper.
    3. In run_fire_monitor.py (or wherever you build the source), replace
       MLX90640FrameSource with OptrisFrameSource and pass appropriate
       kwargs (instance_name, dll_path, etc.).
    """

    def __init__(self, **kwargs):
        self._kwargs = kwargs

    def open(self) -> None:
        raise NotImplementedError(
            "OptrisFrameSource requires raw-frame SDK bindings not yet implemented. "
            "Use MLX90640FrameSource for testing. "
            "See the class docstring for activation instructions."
        )

    def close(self) -> None:
        pass

    def read_frame(self) -> tuple[float, np.ndarray] | None:
        raise NotImplementedError
