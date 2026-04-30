#pragma once
// Imager IPC.h
// Definitions for Imager Interprocess Communication

#ifdef __cplusplus 
extern "C"
{
#endif

#ifdef IMAGERIPC_EXPORTS
#define IMAGERIPC_API __declspec(dllexport)
#else
#define IMAGERIPC_API __declspec(dllimport)
#endif

#ifndef WINAPI
#define WINAPI __stdcall 
#endif

#ifndef HRESULT
#define HRESULT long
#endif

enum TFlagState {fsFlagOpen, fsFlagClose, fsFlagOpening, fsFlagClosing, fsError};
struct FrameMetadata
{
	unsigned short Size;	// size of this structure
	unsigned int Counter;	// frame counter
	unsigned int CounterHW;	// frame counter hardware
	long long Timestamp;	// time stamp in UNITS (10000000 per second)
	long long TimestampMedia;
	TFlagState FlagState;
	float TempChip;
	float TempFlag;
	float TempBox;
	WORD PIFin[2];			
	// DI = PIFin[0] & 0x8000
	// AI1 = PIFin[0] & 0x03FF
	// AI2 = PIFin[1] & 0x03FF
};


// type definitions for function pointer
typedef HRESULT (WINAPI *fpOnServerStopped)(int reason);
typedef HRESULT (WINAPI *fpOnFrameInit)(int, int, int);
typedef HRESULT (WINAPI *fpOnNewFrame)(char*, int);
typedef HRESULT (WINAPI *fpOnNewFrameEx)(void*, FrameMetadata*);
typedef HRESULT (WINAPI *fpOnInitCompleted)(void );
typedef HRESULT (WINAPI *fpOnConfigChanged)(long reserved);
typedef HRESULT (WINAPI *fpOnFileCommandReady)(wchar_t *Path);


IMAGERIPC_API HRESULT WINAPI InitImagerIPC(void);
IMAGERIPC_API HRESULT WINAPI InitNamedImagerIPC(wchar_t *InstanceName);
IMAGERIPC_API HRESULT WINAPI RunImagerIPC(void);
IMAGERIPC_API HRESULT WINAPI ReleaseImagerIPC(void);
IMAGERIPC_API HRESULT WINAPI AcknowledgeFrame(void);
IMAGERIPC_API HRESULT WINAPI SetLogFile(wchar_t *LogFilename, int LogLevel, bool Append);

//--------------------------------------------------------------------------------------
// Callback procedures:
IMAGERIPC_API HRESULT WINAPI SetCallback_OnServerStopped(fpOnServerStopped OnServerStopped);
IMAGERIPC_API HRESULT WINAPI SetCallback_OnFrameInit(fpOnFrameInit OnFrameInit);
IMAGERIPC_API HRESULT WINAPI SetCallback_OnNewFrame(fpOnNewFrame OnNewFrame);
IMAGERIPC_API HRESULT WINAPI SetCallback_OnNewFrameEx(fpOnNewFrameEx OnNewFrameEx);
IMAGERIPC_API HRESULT WINAPI SetCallback_OnInitCompleted(fpOnInitCompleted OnInitCompleted);
IMAGERIPC_API HRESULT WINAPI SetCallback_OnConfigChanged(fpOnConfigChanged OnConfigChanged);
IMAGERIPC_API HRESULT WINAPI SetCallback_OnFileCommandReady(fpOnFileCommandReady OnFileCommandReady);

//--------------------------------------------------------------------------------------
// Get & Set procedures: 
IMAGERIPC_API __int64 WINAPI GetVersionApplication(void);
IMAGERIPC_API __int64 WINAPI GetVersionHID_DLL(void);
IMAGERIPC_API __int64 WINAPI GetVersionCD_DLL(void);
IMAGERIPC_API __int64 WINAPI GetVersionIPC_DLL(void);

IMAGERIPC_API float WINAPI GetTempChip(void);
IMAGERIPC_API float WINAPI GetTempFlag(void);
IMAGERIPC_API float WINAPI GetTempProc(void); 
IMAGERIPC_API float WINAPI GetTempBox(void);
IMAGERIPC_API float WINAPI GetTempHousing(void);
IMAGERIPC_API float WINAPI GetTempTarget(void);
IMAGERIPC_API float WINAPI GetHumidity(void);
IMAGERIPC_API USHORT WINAPI GetTempRangeCount(void);
IMAGERIPC_API USHORT WINAPI GetOpticsCount(void);
IMAGERIPC_API USHORT WINAPI GetMeasureAreaCount(void);
IMAGERIPC_API float WINAPI GetTempMinRange(ULONG Index);
IMAGERIPC_API float WINAPI GetTempMaxRange(ULONG Index);
IMAGERIPC_API USHORT WINAPI GetOpticsFOV(ULONG Index);
IMAGERIPC_API float WINAPI GetTempMeasureArea(ULONG Index);
IMAGERIPC_API USHORT WINAPI GetInitCounter(void);

IMAGERIPC_API bool WINAPI GetFlag(void);
IMAGERIPC_API bool WINAPI SetFlag(bool Value);
IMAGERIPC_API USHORT WINAPI GetOpticsIndex(void);
IMAGERIPC_API USHORT WINAPI SetOpticsIndex(USHORT Value);
IMAGERIPC_API USHORT WINAPI GetTempRangeIndex(void);
IMAGERIPC_API USHORT WINAPI SetTempRangeIndex(USHORT Value);
IMAGERIPC_API bool   WINAPI GetMainWindowEmbedded(void);
IMAGERIPC_API bool   WINAPI SetMainWindowEmbedded(bool Value);
IMAGERIPC_API USHORT WINAPI GetMainWindowLocX(void);
IMAGERIPC_API USHORT WINAPI SetMainWindowLocX(USHORT Value);
IMAGERIPC_API USHORT WINAPI GetMainWindowLocY(void);
IMAGERIPC_API USHORT WINAPI SetMainWindowLocY(USHORT Value);
IMAGERIPC_API USHORT WINAPI GetMainWindowWidth(void);
IMAGERIPC_API USHORT WINAPI SetMainWindowWidth(USHORT Value);
IMAGERIPC_API USHORT WINAPI GetMainWindowHeight(void);
IMAGERIPC_API USHORT WINAPI SetMainWindowHeight(USHORT Value);

IMAGERIPC_API UCHAR  WINAPI GetHardware_Model(void);
IMAGERIPC_API UCHAR  WINAPI GetHardware_Spec(void);
IMAGERIPC_API ULONG  WINAPI GetSerialNumber(void);
IMAGERIPC_API ULONG  WINAPI GetSerialNumberULIS(void);
IMAGERIPC_API USHORT WINAPI GetFirmware_MSP(void);
IMAGERIPC_API USHORT WINAPI GetFirmware_Cypress(void);
IMAGERIPC_API USHORT WINAPI GetPID(void);
IMAGERIPC_API USHORT WINAPI GetVID(void);

//--------------------------------------------------------------------------------------
// Control commands:
IMAGERIPC_API void WINAPI ResetFlag(void);
IMAGERIPC_API bool WINAPI RenewFlag(void);
IMAGERIPC_API void WINAPI CloseApplication(void);
IMAGERIPC_API void WINAPI ReinitDevice(void);
IMAGERIPC_API void WINAPI FileSnapshot(void);
IMAGERIPC_API void WINAPI FileRecord(void);
IMAGERIPC_API void WINAPI FileStop(void); 
IMAGERIPC_API void WINAPI FilePlay(void);
IMAGERIPC_API void WINAPI FilePause(void); 
IMAGERIPC_API USHORT WINAPI FileOpen(wchar_t * FileName);

#ifdef __cplusplus 
}
#endif