from __future__ import annotations

import ctypes
import logging
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


class OptrisSdk:
    def __init__(self, dll_path: Path):
        self.dll_path = Path(dll_path)
        if not self.dll_path.exists():
            raise OptrisSdkError(f"Connect SDK DLL not found: {self.dll_path}")
        self._dll = ctypes.WinDLL(str(self.dll_path))
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
