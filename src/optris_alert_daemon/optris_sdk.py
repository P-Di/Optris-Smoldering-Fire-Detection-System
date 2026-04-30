from __future__ import annotations

import ctypes
import logging
import os
import time
from ctypes import wintypes
from dataclasses import dataclass
from pathlib import Path

LOG = logging.getLogger("optris_alert_daemon")

from .models import Thresholds


IPC_EVENT_INIT_COMPLETED = 1 << 0
IPC_EVENT_SERVER_STOPPED = 1 << 1
IPC_EVENT_CONFIG_CHANGED = 1 << 2
IPC_EVENT_FILE_CMD_READY = 1 << 3
IPC_EVENT_AREAS_INIT = 1 << 11

MEASURE_AREA_TYPE_MEASURE_AREA = 1

# MeasureAreaShape enum values (from C# IPC2 sample — authoritative for the DLL)
MAS_OFF = 0
MAS_MP1X1 = 1
MAS_MP3X3 = 2
MAS_MP5X5 = 3
MAS_USER_DEF_RECT = 4  # Full user-defined rectangle
MAS_ELLIPSE = 5
MAS_POLYGON = 6
MAS_CURVE = 7

# MeasureAreaMode enum values
MAM_MIN = 0
MAM_MAX = 1
MAM_AVG = 2
MAM_DIST = 3

HRESULT = ctypes.c_long


class OptrisSdkError(RuntimeError):
    pass


@dataclass(frozen=True)
class MeasureAreaInfo:
    index: int
    name: str
    thresholds: Thresholds


class FRANGE(ctypes.Structure):
    _fields_ = [("Min", ctypes.c_float), ("Max", ctypes.c_float)]


class AlarmSetting(ctypes.Structure):
    _fields_ = [
        ("Index", ctypes.c_ushort),
        ("Type", ctypes.c_int),
        ("AlarmRange", FRANGE),
        ("PreAlarmRange", FRANGE),
        ("DispRange", FRANGE),
        ("DisplayWarning", wintypes.BOOL),
    ]


# Layout matches C# IPC2 sample explicit field offsets (total 56 bytes):
# Shape@0, Mode@4, BindToTempProfile@8, UseEmissivity@12, Emissivity@16,
# ShowInDigDispGroup@20, distMin@24, distMax@28, Location@32, Size@40,
# IsHotSpot@48, IsColdSpot@52
class MeasureAreaData(ctypes.Structure):
    _fields_ = [
        ("Shape", ctypes.c_int),
        ("Mode", ctypes.c_int),
        ("BindToTempProfile", ctypes.c_uint),
        ("UseEmissivity", ctypes.c_uint),
        ("Emissivity", ctypes.c_float),
        ("ShowInDigDispGroup", ctypes.c_uint),
        ("distMin", ctypes.c_float),
        ("distMax", ctypes.c_float),
        ("Location", wintypes.POINT),
        ("Size", wintypes.SIZE),
        ("IsHotSpot", ctypes.c_uint),
        ("IsColdSpot", ctypes.c_uint),
    ]


class OptrisSdk:
    def __init__(self, dll_path: Path):
        self.dll_path = Path(dll_path)
        if not self.dll_path.exists():
            raise OptrisSdkError(f"Connect SDK DLL not found: {self.dll_path}")
        os.add_dll_directory(str(self.dll_path.parent))
        try:
            # winmode=0 uses the classic Windows search order so the DLL can
            # find its own VC++ runtime dependencies (needed on Python 3.8+).
            self._dll = ctypes.WinDLL(str(self.dll_path), winmode=0)
        except OSError as exc:
            raise OptrisSdkError(
                f"Failed to load Connect SDK DLL ({self.dll_path}): {exc}\n"
                "This usually means a Visual C++ redistributable is missing.\n"
                "  v90  -> VC++ 2008  v100 -> VC++ 2010  v120 -> VC++ 2013\n"
                "Try switching to the v100 build in your config's sdk_dll_path."
            ) from exc
        self._bind()

    def _bind(self) -> None:
        self._dll.SetImagerIPCCount.argtypes = [ctypes.c_ushort]
        self._dll.SetImagerIPCCount.restype = HRESULT

        self._dll.InitImagerIPC.argtypes = [ctypes.c_ushort]
        self._dll.InitImagerIPC.restype = HRESULT

        self._dll.InitNamedImagerIPC.argtypes = [ctypes.c_ushort, ctypes.c_wchar_p]
        self._dll.InitNamedImagerIPC.restype = HRESULT

        self._dll.RunImagerIPC.argtypes = [ctypes.c_ushort]
        self._dll.RunImagerIPC.restype = HRESULT

        self._dll.ReleaseImagerIPC.argtypes = [ctypes.c_ushort]
        self._dll.ReleaseImagerIPC.restype = HRESULT

        self._dll.GetIPCState.argtypes = [ctypes.c_ushort, ctypes.c_bool]
        self._dll.GetIPCState.restype = ctypes.c_ushort

        self._dll.GetMeasureAreaCount.argtypes = [ctypes.c_ushort]
        self._dll.GetMeasureAreaCount.restype = ctypes.c_ushort

        self._dll.GetTempMeasureArea.argtypes = [ctypes.c_ushort, ctypes.c_ulong]
        self._dll.GetTempMeasureArea.restype = ctypes.c_float

        self._dll.GetMeasureAreaName.argtypes = [
            ctypes.c_ushort,
            ctypes.c_ulong,
            ctypes.c_wchar_p,
            ctypes.POINTER(ctypes.c_int),
            ctypes.c_int,
        ]
        self._dll.GetMeasureAreaName.restype = HRESULT

        self._dll.GetAlarmThreshold.argtypes = [
            ctypes.c_ushort,
            ctypes.c_int,
            ctypes.c_ulong,
            ctypes.POINTER(AlarmSetting),
        ]
        self._dll.GetAlarmThreshold.restype = HRESULT

        self._dll.ImagerIPCProcessMessages.argtypes = [ctypes.c_ushort]
        self._dll.ImagerIPCProcessMessages.restype = HRESULT

        self._dll.FileSnapshot.argtypes = [ctypes.c_ushort]
        self._dll.FileSnapshot.restype = HRESULT

        self._dll.GetPathOfStoredFile.argtypes = [ctypes.c_ushort, ctypes.c_wchar_p, ctypes.c_int]
        self._dll.GetPathOfStoredFile.restype = HRESULT

        self._dll.GetSourceResolutionIR.argtypes = [ctypes.c_ushort, ctypes.POINTER(wintypes.SIZE)]
        self._dll.GetSourceResolutionIR.restype = HRESULT

        self._dll.GetMeasureArea.argtypes = [
            ctypes.c_ushort,
            ctypes.c_ulong,
            ctypes.POINTER(MeasureAreaData),
        ]
        self._dll.GetMeasureArea.restype = HRESULT

        self._dll.SetMeasureArea.argtypes = [
            ctypes.c_ushort,
            ctypes.c_ulong,
            ctypes.POINTER(MeasureAreaData),
            ctypes.c_bool,
        ]
        self._dll.SetMeasureArea.restype = HRESULT

        self._dll.SetMeasureAreaName.argtypes = [
            ctypes.c_ushort,
            ctypes.c_ulong,
            ctypes.c_wchar_p,
        ]
        self._dll.SetMeasureAreaName.restype = HRESULT

        self._dll.RemoveMeasureArea.argtypes = [ctypes.c_ushort, ctypes.c_ulong]
        self._dll.RemoveMeasureArea.restype = HRESULT

        self._dll.SetAlarmThreshold.argtypes = [ctypes.c_ushort, AlarmSetting]
        self._dll.SetAlarmThreshold.restype = HRESULT

    def set_imager_count(self, count: int) -> None:
        self._check_hresult(self._dll.SetImagerIPCCount(count), "SetImagerIPCCount")

    def create_session(self, index: int, instance_name: str | None) -> "OptrisSession":
        return OptrisSession(self, index=index, instance_name=instance_name)

    @staticmethod
    def _check_hresult(value: int, function_name: str) -> None:
        if int(value) < 0:
            raise OptrisSdkError(f"{function_name} failed with HRESULT {int(value)}")


class OptrisSession:
    def __init__(self, sdk: OptrisSdk, index: int, instance_name: str | None):
        self.sdk = sdk
        self.index = index
        self.instance_name = instance_name

    def connect(self, timeout_seconds: float, retry_interval_seconds: float) -> None:
        deadline = time.monotonic() + timeout_seconds
        while True:
            try:
                if self.instance_name:
                    self.sdk._check_hresult(
                        self.sdk._dll.InitNamedImagerIPC(self.index, self.instance_name),
                        "InitNamedImagerIPC",
                    )
                else:
                    self.sdk._check_hresult(self.sdk._dll.InitImagerIPC(self.index), "InitImagerIPC")
                break
            except OptrisSdkError:
                if time.monotonic() >= deadline:
                    raise
                time.sleep(retry_interval_seconds)

        self.sdk._check_hresult(self.sdk._dll.RunImagerIPC(self.index), "RunImagerIPC")
        self._wait_for_init(deadline)

    def _wait_for_init(self, deadline: float) -> None:
        while time.monotonic() < deadline:
            events = self.poll_events(reset=True)
            LOG.debug("Camera index %d IPC state: 0x%04x", self.index, events)
            if events & IPC_EVENT_SERVER_STOPPED:
                raise OptrisSdkError(f"PIX Connect stopped IPC for camera index {self.index}")
            if events & IPC_EVENT_INIT_COMPLETED:
                LOG.debug("Camera index %d: IPC_EVENT_INIT_COMPLETED received.", self.index)
                return
            if events & IPC_EVENT_AREAS_INIT:
                LOG.debug("Camera index %d: IPC_EVENT_AREAS_INIT received — waiting for areas to populate.", self.index)
            # PIX Connect only fires IPC_EVENT_INIT_COMPLETED once, during its own startup.
            # If the daemon connects to an already-running PIX Connect, that event has
            # already been consumed.  Probe the session directly instead.
            count = self.get_measure_area_count()
            LOG.debug("Camera index %d: GetMeasureAreaCount returned %d", self.index, count)
            if count > 0:
                LOG.debug(
                    "Camera index %d: no init event but %d measure area(s) visible — treating as ready.",
                    self.index,
                    count,
                )
                return
            self.process_messages()
            time.sleep(0.5)
        raise OptrisSdkError(f"Timed out waiting for IPC init completion on camera index {self.index}")

    def release(self) -> None:
        try:
            self.sdk._check_hresult(self.sdk._dll.ReleaseImagerIPC(self.index), "ReleaseImagerIPC")
        except OptrisSdkError:
            pass

    def process_messages(self) -> None:
        self.sdk._dll.ImagerIPCProcessMessages(self.index)

    def poll_events(self, reset: bool = True) -> int:
        return int(self.sdk._dll.GetIPCState(self.index, reset))

    def get_measure_area_count(self) -> int:
        return int(self.sdk._dll.GetMeasureAreaCount(self.index))

    def get_measure_area_temperature(self, measure_area_index: int) -> float:
        return float(self.sdk._dll.GetTempMeasureArea(self.index, measure_area_index))

    def get_measure_area_name(self, measure_area_index: int, buffer_length: int = 256) -> str:
        buffer = ctypes.create_unicode_buffer(buffer_length)
        name_length = ctypes.c_int()
        self.sdk._check_hresult(
            self.sdk._dll.GetMeasureAreaName(
                self.index,
                measure_area_index,
                buffer,
                ctypes.byref(name_length),
                buffer_length,
            ),
            "GetMeasureAreaName",
        )
        return buffer.value

    def get_alarm_threshold(self, measure_area_index: int) -> Thresholds:
        setting = AlarmSetting()
        self.sdk._check_hresult(
            self.sdk._dll.GetAlarmThreshold(
                self.index,
                MEASURE_AREA_TYPE_MEASURE_AREA,
                measure_area_index,
                ctypes.byref(setting),
            ),
            "GetAlarmThreshold",
        )
        return Thresholds(
            low_alarm_c=float(setting.AlarmRange.Min),
            low_pre_alarm_c=float(setting.PreAlarmRange.Min),
            high_pre_alarm_c=float(setting.PreAlarmRange.Max),
            high_alarm_c=float(setting.AlarmRange.Max),
            source="pix_connect",
        )

    def capture_snapshot(self, timeout_seconds: float) -> Path | None:
        self.sdk._check_hresult(self.sdk._dll.FileSnapshot(self.index), "FileSnapshot")
        deadline = time.monotonic() + timeout_seconds
        while time.monotonic() < deadline:
            events = self.poll_events(reset=True)
            if events & IPC_EVENT_FILE_CMD_READY:
                return self.get_last_stored_file()
            if events & IPC_EVENT_SERVER_STOPPED:
                raise OptrisSdkError(f"PIX Connect stopped IPC during snapshot for camera index {self.index}")
            time.sleep(0.1)
        return None

    def get_last_stored_file(self, buffer_length: int = 1024) -> Path | None:
        buffer = ctypes.create_unicode_buffer(buffer_length)
        self.sdk._check_hresult(
            self.sdk._dll.GetPathOfStoredFile(self.index, buffer, buffer_length),
            "GetPathOfStoredFile",
        )
        value = buffer.value.strip()
        return Path(value) if value else None

    def get_source_resolution_ir(self) -> tuple[int, int]:
        size = wintypes.SIZE()
        self.sdk._check_hresult(
            self.sdk._dll.GetSourceResolutionIR(self.index, ctypes.byref(size)),
            "GetSourceResolutionIR",
        )
        return int(size.cx), int(size.cy)

    def get_measure_area(self, measure_area_index: int) -> MeasureAreaData:
        data = MeasureAreaData()
        self.sdk._check_hresult(
            self.sdk._dll.GetMeasureArea(self.index, measure_area_index, ctypes.byref(data)),
            "GetMeasureArea",
        )
        return data

    def set_measure_area(self, area_index: int, data: MeasureAreaData, *, add_new: bool = False) -> None:
        self.sdk._check_hresult(
            self.sdk._dll.SetMeasureArea(self.index, area_index, ctypes.byref(data), add_new),
            "SetMeasureArea",
        )

    def set_measure_area_name(self, area_index: int, name: str) -> None:
        self.sdk._check_hresult(
            self.sdk._dll.SetMeasureAreaName(self.index, area_index, name),
            "SetMeasureAreaName",
        )

    def set_alarm_threshold(self, setting: AlarmSetting) -> None:
        self.sdk._check_hresult(
            self.sdk._dll.SetAlarmThreshold(self.index, setting),
            "SetAlarmThreshold",
        )

    def ensure_fullscreen_alarm_area(self, name: str = "Full Screen Alarm") -> int:
        """Create a max-mode rectangle covering the full camera frame if one doesn't exist.

        Returns the index of the full-screen area (existing or newly created).
        """
        width, height = self.get_source_resolution_ir()
        if width <= 0 or height <= 0:
            raise OptrisSdkError(
                f"Camera index {self.index}: GetSourceResolutionIR returned invalid size {width}x{height}"
            )

        count = self.get_measure_area_count()
        for i in range(count):
            try:
                area = self.get_measure_area(i)
                if (
                    area.Shape == MAS_USER_DEF_RECT
                    and area.Location.x == 0
                    and area.Location.y == 0
                    and area.Size.cx == width
                    and area.Size.cy == height
                ):
                    LOG.debug(
                        "Camera index %d: full-screen alarm area already exists at index %d.",
                        self.index,
                        i,
                    )
                    return i
            except OptrisSdkError:
                continue

        # Build a new full-screen rectangle area using maximum-temperature mode
        data = MeasureAreaData()
        data.Shape = MAS_USER_DEF_RECT
        data.Mode = MAM_MAX
        data.BindToTempProfile = 0
        data.UseEmissivity = 0
        data.Emissivity = 1.0
        data.ShowInDigDispGroup = 1
        data.distMin = 0.0
        data.distMax = 0.0
        data.Location.x = 0
        data.Location.y = 0
        data.Size.cx = width
        data.Size.cy = height
        data.IsHotSpot = 0
        data.IsColdSpot = 0

        new_index = count  # SetMeasureArea with addNew=True appends at count
        self.set_measure_area(new_index, data, add_new=True)
        self.set_measure_area_name(new_index, name)
        LOG.info(
            "Camera index %d: created full-screen alarm area '%s' (%dx%d) at index %d.",
            self.index,
            name,
            width,
            height,
            new_index,
        )
        return new_index
