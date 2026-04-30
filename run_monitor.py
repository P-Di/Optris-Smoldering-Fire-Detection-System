#!/usr/bin/env python3
"""
Optris Thermal Monitor — entry point.

Sets up the OTC SDK DLL and Python binding paths, then launches the GUI.
Usage: python run_monitor.py [path/to/monitor.json]
"""
from __future__ import annotations

import os
import sys
from pathlib import Path

# ── SDK paths — must be configured before importing otc_monitor ──────────────
_SDK_ROOT = Path(r"C:\Program Files\Optris\otcsdk")
_SDK_BIN  = _SDK_ROOT / "bin"
_SDK_PY   = _SDK_ROOT / "bindings" / "python3"

if not _SDK_BIN.exists():
    sys.exit(f"OTC SDK bin not found at {_SDK_BIN}. Check SDK installation.")

os.add_dll_directory(str(_SDK_BIN))   # makes otcsdk.dll / SoSCorrectionLib.dll visible
sys.path.insert(0, str(_SDK_PY))       # makes `import optris.otcsdk` work

# ── Project src on path ──────────────────────────────────────────────────────
sys.path.insert(0, str(Path(__file__).parent / "src"))

from otc_monitor.main import run

if __name__ == "__main__":
    config = sys.argv[1] if len(sys.argv) > 1 else "config/monitor.json"
    run(config)
