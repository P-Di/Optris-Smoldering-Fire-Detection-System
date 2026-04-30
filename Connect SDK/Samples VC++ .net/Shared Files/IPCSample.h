#pragma once

namespace IPCSample {

public ref struct DeviceData
{
	float TempChip;
	float TempFlag;
	float TempBox;
	float TempTarget;
};

public ref struct DeviceSetup
{
	USHORT IPCMode;
	USHORT OpticsIndex;
	USHORT TempRangeIndex;
	USHORT VideoFormatIndex;
	float FixedEmissivity;
	float FixedTransmissivity;
	float FixedTempAmbient;
	float FixedTempReference;
};

public ref struct DeviceInfo
{
	USHORT HW_Model; //UCHAR HW_Model;
	UCHAR HW_Spec;
	ULONG SerialNumber;
	ULONG SerialNumberULIS;
	USHORT FW_MSP;
	USHORT FW_Cypress;
	USHORT PID;
	USHORT VID;
	float TempMinRange;
	float TempMaxRange;
	USHORT TempRangeDecimal_EffValue;
	USHORT TempRangeDecimal_CaliValue;
	USHORT OpticsFOV;
	ULONG  OpticsSerialNumber;
};

public ref struct Version
{
	Version(long long vers)
	{
		Major = (USHORT)((vers >> 48) & 0xFFFF);
		Minor = (USHORT)((vers >> 32) & 0xFFFF);
		Build = (USHORT)((vers >> 16) & 0xFFFF);
		Revision = (USHORT)(vers & 0xFFFF);
	};
	Version()
	{
		Major = Minor = Build = Revision = 0;
	};
	USHORT Major;
	USHORT Minor;
	USHORT Build;
	USHORT Revision;
};


public ref struct SoftwareInfo
{
	SoftwareInfo()
	{
		HID_DLL = gcnew Version();
		CD_DLL = gcnew Version();
		IPC_DLL = gcnew Version();
		Application = gcnew Version();
	};
	Version^ HID_DLL;
	Version^ CD_DLL;
	Version^ IPC_DLL;
	Version^ Application;
};



public ref struct Device
{
	SoftwareInfo SoftInfo;
	DeviceInfo Info;
	DeviceSetup Setup;
	DeviceData Data;
};


}