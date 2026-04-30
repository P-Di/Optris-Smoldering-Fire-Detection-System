#pragma once

#include < vcclr.h >
#include "ImagerIPC.h"

using namespace System::Runtime::InteropServices; 

public ref class IPC
{
	public:
		IPC(void){};

		delegate HRESULT delOnServerStopped(int);
		delegate HRESULT delOnFrameInit(int, int, int);
		delegate HRESULT delOnNewFrame(char*, int);
		delegate HRESULT delOnNewFrameEx(void*, FrameMetadata*);
		delegate HRESULT delOnInitCompleted(void );
		delegate HRESULT delOnConfigChanged(long);
		delegate HRESULT delOnFileCommandReady(wchar_t*);

		delOnServerStopped^		OnServerStopped;
		delOnFrameInit^			OnFrameInit;
		delOnNewFrame^			OnNewFrame;
		delOnNewFrameEx^		OnNewFrameEx;
		delOnInitCompleted^		OnInitCompleted;
		delOnConfigChanged^		OnConfigChanged;
		delOnFileCommandReady^	OnFileCommandReady;

		literal System::String^ strDLL = "ImagerIPC.dll"; 
 
		// General functions:
		[ DllImport(strDLL, EntryPoint = "_InitImagerIPC@0") ]  		static HRESULT Init(void); 
		[ DllImport(strDLL, EntryPoint = "_InitNamedImagerIPC@4") ]  	static HRESULT Init(wchar_t *InstanceName); 
		[ DllImport(strDLL, EntryPoint = "_RunImagerIPC@0") ]  			static HRESULT Run(void); 
		[ DllImport(strDLL, EntryPoint = "_ReleaseImagerIPC@0") ]		static HRESULT Release(void);  
		[ DllImport(strDLL, EntryPoint = "_AcknowledgeFrame@0") ]		static HRESULT AcknowledgeFrame(void);
		[ DllImport(strDLL, EntryPoint = "_SetLogFile@12") ]  			static HRESULT SetLogFile(wchar_t *Filename, int LogLevel, bool Append); 

		// Callback functions:
		[ DllImport(strDLL, EntryPoint = "_SetCallback_OnServerStopped@4")]	static HRESULT SetCallback_OnServerStopped(delOnServerStopped^ ptr);  
		[ DllImport(strDLL, EntryPoint = "_SetCallback_OnFrameInit@4")]		static HRESULT SetCallback_OnFrameInit(delOnFrameInit^ ptr);  
		[ DllImport(strDLL, EntryPoint = "_SetCallback_OnNewFrame@4")] 		static HRESULT SetCallback_OnNewFrame(delOnNewFrame^ ptr);  
		// OnNewFrameEx will work with Imager.exe release > 2.0 only:
		[ DllImport(strDLL, EntryPoint = "_SetCallback_OnNewFrameEx@4")] 	static HRESULT SetCallback_OnNewFrameEx(delOnNewFrameEx^ ptr);  
		[ DllImport(strDLL, EntryPoint = "_SetCallback_OnInitCompleted@4")] static HRESULT SetCallback_OnInitCompleted(delOnInitCompleted^ ptr);  
		[ DllImport(strDLL, EntryPoint = "_SetCallback_OnConfigChanged@4")] static HRESULT SetCallback_OnConfigChanged(delOnConfigChanged^ ptr);  
		[ DllImport(strDLL, EntryPoint = "_SetCallback_OnFileCommandReady@4")] static HRESULT SetCallback_OnFileCommandReady(delOnFileCommandReady^ ptr);  

		// Get & Set Functions
		[ DllImport(strDLL, EntryPoint = "_GetVersionApplication@0") ] 	static long long GetVersionApplication(void);  
		[ DllImport(strDLL, EntryPoint = "_GetVersionHID_DLL@0") ] 		static long long GetVersionHID_DLL(void);  
		[ DllImport(strDLL, EntryPoint = "_GetVersionCD_DLL@0") ] 		static long long GetVersionCD_DLL(void);  
		[ DllImport(strDLL, EntryPoint = "_GetVersionIPC_DLL@0") ]		static long long GetVersionIPC_DLL(void);  

		[ DllImport(strDLL, EntryPoint = "_GetTempTec@0") ] 			static float GetTempTec(void);  
		[ DllImport(strDLL, EntryPoint = "_SetTempTec@4") ] 			static float SetTempTec(float Value);  
		[ DllImport(strDLL, EntryPoint = "_GetFlag@0") ] 				static bool GetFlag(void);  
		[ DllImport(strDLL, EntryPoint = "_SetFlag@4") ] 				static bool SetFlag(bool Value);  
		[ DllImport(strDLL, EntryPoint = "_GetOpticsIndex@0") ]			static USHORT GetOpticsIndex(void);  
		[ DllImport(strDLL, EntryPoint = "_SetOpticsIndex@4") ]			static USHORT SetOpticsIndex(USHORT Value);  
		[ DllImport(strDLL, EntryPoint = "_GetTempRangeIndex@0") ]		static USHORT GetTempRangeIndex(void);  
		[ DllImport(strDLL, EntryPoint = "_SetTempRangeIndex@4") ]		static USHORT SetTempRangeIndex(USHORT Value);  
		[ DllImport(strDLL, EntryPoint = "_GetMainWindowEmbedded@0") ] 	static bool   GetMainWindowEmbedded(void);  
		[ DllImport(strDLL, EntryPoint = "_SetMainWindowEmbedded@4") ] 	static bool   SetMainWindowEmbedded(bool Value);  
		[ DllImport(strDLL, EntryPoint = "_GetMainWindowLocX@0") ]		static USHORT GetMainWindowLocX(void);  
		[ DllImport(strDLL, EntryPoint = "_SetMainWindowLocX@4") ]		static USHORT SetMainWindowLocX(USHORT Value);  
		[ DllImport(strDLL, EntryPoint = "_GetMainWindowLocY@0") ]		static USHORT GetMainWindowLocY(void);  
		[ DllImport(strDLL, EntryPoint = "_SetMainWindowLocY@4") ]		static USHORT SetMainWindowLocY(USHORT Value);  
		[ DllImport(strDLL, EntryPoint = "_GetMainWindowWidth@0") ]		static USHORT GetMainWindowWidth(void);  
		[ DllImport(strDLL, EntryPoint = "_SetMainWindowWidth@4") ]		static USHORT SetMainWindowWidth(USHORT Value);  
		[ DllImport(strDLL, EntryPoint = "_GetMainWindowHeight@0") ]	static USHORT GetMainWindowHeight(void);  
		[ DllImport(strDLL, EntryPoint = "_SetMainWindowHeight@4") ]	static USHORT SetMainWindowHeight(USHORT Value);  
		System::Drawing::Rectangle GetMainWindowRect(void)
		{
			return System::Drawing::Rectangle(
				GetMainWindowLocX(), GetMainWindowLocY(), GetMainWindowWidth(),	GetMainWindowHeight());
		}
		System::Drawing::Rectangle SetMainWindowRect(System::Drawing::Rectangle rect)
		{
			return System::Drawing::Rectangle(
				SetMainWindowLocX(rect.X), SetMainWindowLocY(rect.Y), SetMainWindowWidth(rect.Width), SetMainWindowHeight(rect.Height));
		}
		System::Drawing::Rectangle SetMainWindowRect(System::Drawing::Rectangle ^rect)
		{
			return System::Drawing::Rectangle(
				SetMainWindowLocX(rect->X), SetMainWindowLocY(rect->Y), SetMainWindowWidth(rect->Width), SetMainWindowHeight(rect->Height));
		}


		[ DllImport(strDLL, EntryPoint = "_GetHardware_Model@0") ]		static UCHAR GetHardware_Model(void);  
		[ DllImport(strDLL, EntryPoint = "_GetHardware_Spec@0") ]		static UCHAR GetHardware_Spec(void);  
		[ DllImport(strDLL, EntryPoint = "_GetSerialNumber@0") ] 		static ULONG GetSerialNumber(void);  
		[ DllImport(strDLL, EntryPoint = "_GetSerialNumberULIS@0")]		static ULONG GetSerialNumberULIS(void);  
		[ DllImport(strDLL, EntryPoint = "_GetFirmware_MSP@0") ] 		static USHORT GetFirmware_MSP(void);  
		[ DllImport(strDLL, EntryPoint = "_GetFirmware_Cypress@0")]		static USHORT GetFirmware_Cypress(void);  
		[ DllImport(strDLL, EntryPoint = "_GetPID@0") ]					static USHORT GetPID(void);  
		[ DllImport(strDLL, EntryPoint = "_GetVID@0") ]					static USHORT GetVID(void);  

		[ DllImport(strDLL, EntryPoint = "_GetTempChip@0") ] 			static float GetTempChip(void);  
		[ DllImport(strDLL, EntryPoint = "_GetTempFlag@0") ] 			static float GetTempFlag(void);  
		[ DllImport(strDLL, EntryPoint = "_GetTempProc@0") ] 			static float GetTempProc(void);  
		[ DllImport(strDLL, EntryPoint = "_GetTempBox@0") ] 			static float GetTempBox(void);  
		[ DllImport(strDLL, EntryPoint = "_GetTempHousing@0") ] 		static float GetTempHousing(void);  
		[ DllImport(strDLL, EntryPoint = "_GetTempTarget@0") ] 			static float GetTempTarget(void);  
		[ DllImport(strDLL, EntryPoint = "_GetHumidity@0") ] 			static float GetHousing(void);  
		[ DllImport(strDLL, EntryPoint = "_GetTempMinRange@4") ] 		static float GetTempMinRange(ULONG index);  
		[ DllImport(strDLL, EntryPoint = "_GetTempMaxRange@4") ] 		static float GetTempMaxRange(ULONG index);  
		[ DllImport(strDLL, EntryPoint = "_GetTempRangeCount@0") ]		static USHORT GetTempRangeCount(void);  
		[ DllImport(strDLL, EntryPoint = "_GetOpticsFOV@4") ] 			static USHORT GetOpticsFOV(ULONG index);  
		[ DllImport(strDLL, EntryPoint = "_GetOpticsCount@0") ]			static USHORT GetOpticsCount(void);  
		[ DllImport(strDLL, EntryPoint = "_GetMeasureAreaCount@0")]		static USHORT GetMeasureAreaCount(void);  
		[ DllImport(strDLL, EntryPoint = "_GetTempMeasureArea@4") ]		static float GetTempMeasureArea(ULONG index);  
		[ DllImport(strDLL, EntryPoint = "_GetInitCounter@0") ]			static USHORT GetInitCounter(void);  

		// Control commands:
		[ DllImport(strDLL, EntryPoint = "_CloseApplication@0") ]		static void CloseApplication(void);  
		[ DllImport(strDLL, EntryPoint = "_ReinitDevice@0") ]			static void ReinitDevice(void);  
		[ DllImport(strDLL, EntryPoint = "_RenewFlag@0") ]				static bool RenewFlag(void);  
		[ DllImport(strDLL, EntryPoint = "_FileSnapshot@0") ]			static bool FileSnapshot(void);  
		[ DllImport(strDLL, EntryPoint = "_FileRecord@0") ]				static bool FileRecord(void);  
		[ DllImport(strDLL, EntryPoint = "_FileStop@0") ]				static bool FileStop(void);  
		[ DllImport(strDLL, EntryPoint = "_FilePlay@0") ]				static bool FilePlay(void);  
		[ DllImport(strDLL, EntryPoint = "_FilePause@0") ]				static bool FilePause(void);  
		[ DllImport(strDLL, EntryPoint = "_FileOpen@4") ]				static USHORT FileOpen(wchar_t *Filename);  

		static HRESULT Init(System::String^ InstanceName) 
		{ 
			wchar_t szInstanceName[MAX_PATH];
			pin_ptr<const wchar_t> ch = PtrToStringChars(InstanceName); 
			wcscpy_s(szInstanceName, MAX_PATH, ch); 
			return Init(szInstanceName);
		};

};
