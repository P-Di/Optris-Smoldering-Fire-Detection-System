#include "stdafx.h"
#include "time.h"
#include "FormImager.h"


namespace IPCSample {

unsigned char clip(int val) { return (val <= 255) ? ((val > 0) ? val : 0) : 255; };

FormImager::FormImager(HWND hostHandle, IPC^ Ipc, int imagerIndex, String ^instanceName)
{
	InitializeComponent();
	ImagerIndex = imagerIndex;
	ipc = Ipc;
	TopLevel = false;
	FrameWidth = 0;
	FrameHeight = 0;
	FrameDepth = 0;
	bmp = gcnew Bitmap( 100 , 100 , System::Drawing::Imaging::PixelFormat::Format24bppRgb );
	ipcInitialized = false;
	Connected = false;
	Painted = false;
	MeasureAreaCount = 0;
	Margin = groupBoxVideo->Size - pictureBox->Size;
	FC0 = FC1 = 0;
	List = gcnew System::Collections::ArrayList(100);
	textBoxInstanceName->Text = instanceName;
}

FormImager::~FormImager()
{
	if (components)
		delete components;
}

void FormImager::AppInit(void)
{
	Init(160, 120, 2);
}

System::Void FormImager::textBoxInstanceName_TextChanged(System::Object^  sender, System::EventArgs^  e)
{
	ReleaseIPC();
}

void FormImager::InitIPC(void) 
{
	 if(ipc && !ipcInitialized)
	 {
		HRESULT hr = ipc->Init(ImagerIndex, textBoxInstanceName->Text);
		
		if(FAILED(hr))
			ipcInitialized = false;
		else
		{
			onServerStopped = gcnew IPC::delOnServerStopped(this, &FormImager::OnServerStopped );
			ipc->SetCallback_OnServerStopped(ImagerIndex, onServerStopped);

			onFrameInit = gcnew IPC::delOnFrameInit(this, &FormImager::OnFrameInit );
			ipc->SetCallback_OnFrameInit(ImagerIndex, onFrameInit);

			onNewFrameEx2 = gcnew IPC::delOnNewFrameEx2(this, &FormImager::OnNewFrameEx );
			ipc->SetCallback_OnNewFrameEx2(ImagerIndex, onNewFrameEx2);

			onConfigChanged = gcnew IPC::delOnConfigChanged(this, &FormImager::OnConfigChanged );
			ipc->SetCallback_OnConfigChanged(ImagerIndex, onConfigChanged);

			onInitCompleted = gcnew IPC::delOnInitCompleted(this, &FormImager::OnInitCompleted );
			ipc->SetCallback_OnInitCompleted(ImagerIndex, onInitCompleted);

			hr = ipc->Run(ImagerIndex);
			ipcInitialized = SUCCEEDED(hr);
			LastFrameTime = DateTime::Now;
			FC0 = FC1 = 0; 
		}
		label1->Text = hr ? "NOT CONNECTED" : "OK";

	 }
}

void FormImager::ReleaseIPC(void) 
{
	Connected = false;
	if(ipc && ipcInitialized)
	{
		ipc->Release(ImagerIndex);
		ipcInitialized = false;
	}
}

void FormImager::AppExit(void)
{
	ReleaseIPC();
}

void FormImager::GetBitmap(Bitmap^ Bmp, array<short>^values)
{
	int stride_diff;
	// Lock the bitmap's bits.  
	System::Drawing::Rectangle rect = System::Drawing::Rectangle(0, 0, Bmp->Width, Bmp->Height);
	Imaging::BitmapData^ bmpData = Bmp->LockBits( rect, Imaging::ImageLockMode::ReadWrite, Bmp->PixelFormat );
	stride_diff = bmpData->Stride - FrameWidth*3;

	// Get the address of the first line.
	IntPtr ptr = bmpData->Scan0;

	if(Colors)
	{
		for ( int dst=0, src=0, y=0; y < FrameHeight; y++, dst += stride_diff) 
			for ( int x=0; x < FrameWidth; x++, src++, dst+=3 ) 
			{
				int C = LOBYTE(values[src              ]) - 16;
				int D = HIBYTE(values[src - (src%2)    ]) - 128;
				int E = HIBYTE(values[src - (src%2) + 1]) - 128;
				rgbValues[ dst   ] = clip(( 298 * C + 516 * D           + 128) >> 8);
				rgbValues[ dst+1 ] = clip(( 298 * C - 100 * D - 208 * E + 128) >> 8);
				rgbValues[ dst+2 ] = clip(( 298 * C           + 409 * E + 128) >> 8);
			}
	}
	else
	{
		short mn, mx;
		GetBitmap_Limits(values, &mn, &mx);
		double Fact = 255.0 / (mx - mn);

		for ( int dst=0, src=0, y=0; y < FrameHeight; y++, dst += stride_diff) 
			for ( int x=0; x < FrameWidth; x++, src++, dst+=3 ) 
				rgbValues[dst] = rgbValues[dst+1] = rgbValues[dst+2] = min(max((int)(Fact * (values[src] - mn)), 0), 255);
	}

	// Copy the RGB values back to the bitmap
	System::Runtime::InteropServices::Marshal::Copy( rgbValues, 0, ptr, rgbValues->Length );

	// Unlock the bits.
	Bmp->UnlockBits( bmpData );
}


void FormImager::GetBitmap_Limits(array<short>^Values, short *min, short *max)
{
	int y;
	double Sum, Mean, Variance;
	if(!Values) return;

	Sum = 0;
	for (y=0; y < FrameSize; y++ ) 
		Sum += Values[y];
	Mean = (double)Sum / FrameSize;
	Sum = 0;
	for (y=0; y < FrameSize; y++ ) 
		Sum += (Mean - Values[y]) * (Mean - Values[y]);
	Variance = Sum / FrameSize;
	Variance = Math::Sqrt(Variance);
	Variance *= 3;  // 3 Sigma
	*min = short(Mean - Variance);
	*max = short(Mean + Variance);
}


HRESULT FormImager::MainTimer100ms(void)
{
	Painted = false;
	if(Connected && progressBarInit->Visible)
	{
		int i = ipc->GetInitCounter(ImagerIndex);
		progressBarInit->Value = progressBarInit->Maximum - i;
		progressBarInit->Visible = (i != 0);
	}
    return S_OK;
}

HRESULT FormImager::MainTimer1000ms(void)
{
	double Mean = 0, x, last=0;
	IEnumerator^ Enum = List->GetEnumerator();
	while ( Enum->MoveNext() )
	{
		x = safe_cast<double>(Enum->Current);
		last = x;
		Mean += x;
	}
	Mean /= List->Count;
	Enum->Reset();
	while ( Enum->MoveNext() )
		x = Mean - safe_cast<double>(Enum->Current);
	List->Clear();

	labelFramerate->Text = String::Format(L"{0:0.0}Hz", 1000.0 / Mean);

	if(Connected)
	{
		for(int i=0; i < groupBoxMeasureAreas->Controls->Count; i++)
			((Label^)groupBoxMeasureAreas->Controls->default[i])->Text
				= String::Format("Temp[{0}]: {1:##0.0}°C", i, ipc->GetTempMeasureArea(ImagerIndex, i));
	}
	return S_OK;
}


void FormImager::GetSoftwareInfo(void)
{
	Dev.SoftInfo.Application	= gcnew IPCSample::Version(ipc->GetVersionApplication(ImagerIndex));
	Dev.SoftInfo.CD_DLL			= gcnew IPCSample::Version(ipc->GetVersionCD_DLL(ImagerIndex));
	Dev.SoftInfo.HID_DLL		= gcnew IPCSample::Version(ipc->GetVersionHID_DLL(ImagerIndex)); 
	Dev.SoftInfo.IPC_DLL		= gcnew IPCSample::Version(ipc->GetVersionIPC_DLL(ImagerIndex));
}

void FormImager::GetDeviceInfo(void)
{
	Dev.Info.HW_Model			= ipc->GetHardware_Model(ImagerIndex);
	Dev.Info.HW_Spec			= ipc->GetHardware_Spec(ImagerIndex);
	Dev.Info.SerialNumber		= ipc->GetSerialNumber(ImagerIndex);
	Dev.Info.SerialNumberULIS	= ipc->GetSerialNumber(ImagerIndex);
	Dev.Info.FW_MSP				= ipc->GetFirmware_MSP(ImagerIndex);
	Dev.Info.FW_Cypress			= ipc->GetFirmware_Cypress(ImagerIndex);
	Dev.Info.PID				= ipc->GetPID(ImagerIndex);
	Dev.Info.VID				= ipc->GetVID(ImagerIndex);
	Dev.Info.OpticsFOV			= ipc->GetOpticsFOV(ImagerIndex, ipc->GetOpticsIndex(ImagerIndex));
	Dev.Info.TempMinRange		= ipc->GetTempMinRange(ImagerIndex, ipc->GetTempRangeIndex(ImagerIndex));
	Dev.Info.TempMaxRange		= ipc->GetTempMaxRange(ImagerIndex, ipc->GetTempRangeIndex(ImagerIndex));
}
void FormImager::GetDeviceSetup(void)
{
	MeasureAreaCount = ipc->GetMeasureAreaCount(ImagerIndex);
	Dev.Setup.TempRangeIndex = ipc->GetTempRangeCount(ImagerIndex);
	Dev.Setup.TempRangeIndex = ipc->GetTempRangeIndex(0);
}
void FormImager::RenewFlag(void)
{
	ipc->RenewFlag(ImagerIndex);
}

void FormImager::Init(int frameWidth, int frameHeight, int frameDepth)
{
	FrameWidth = frameWidth;
	FrameHeight = frameHeight;
	FrameSize = FrameWidth * FrameHeight;
	FrameRatio = (double)FrameWidth /  (double)FrameHeight;
	FrameDepth = frameDepth;
	FrameCounter1 = LastFrameCounter = FC0 = FC1 =0;
	timer1->Enabled = true;
	bmp = gcnew Bitmap( FrameWidth , FrameHeight , System::Drawing::Imaging::PixelFormat::Format24bppRgb );
	System::Drawing::Rectangle rect = System::Drawing::Rectangle(0, 0, bmp->Width, bmp->Height);
	Imaging::BitmapData^ bmpData = bmp->LockBits( rect, Imaging::ImageLockMode::ReadWrite, bmp->PixelFormat );
	int stride = bmpData->Stride;
	bmp->UnlockBits( bmpData );
	rgbValues = gcnew array<Byte>(stride * FrameHeight);
	Values = gcnew array<short>(FrameSize);

	groupBoxVideo->Size = Drawing::Size(FrameWidth, FrameHeight) + Margin;
	groupBoxMeasureAreas->Location = Drawing::Point(groupBoxMeasureAreas->Left, groupBoxVideo->Bottom + 10);
	UpdateSize();
}

void FormImager::InitMeasureAreas(void)
{
	if(MeasureAreaCount != groupBoxMeasureAreas->Controls->Count)
	{
		groupBoxMeasureAreas->SuspendLayout();
		groupBoxMeasureAreas->Controls->Clear();

		for(int i=0; i < MeasureAreaCount; i++)
		{
			Label^ lbl = gcnew Label();
			lbl->AutoSize = true;
			lbl->Location = System::Drawing::Point(10, i*15 + 16);
			lbl->Name = L"labelMeasureArea" + i.ToString();
			lbl->Text = "";
			groupBoxMeasureAreas->Controls->Add(lbl);
		}

		groupBoxMeasureAreas->Size = Drawing::Size(groupBoxMeasureAreas->Size.Width, MeasureAreaCount * 15 + 20);
		groupBoxMeasureAreas->ResumeLayout();
		UpdateSize();
	}
}

void FormImager::UpdateSize(void)
{
	int right = max(max(groupBoxVideo->Right, groupBoxMeasureAreas->Right), buttonFlagRenew->Right);
	int bottom = groupBoxMeasureAreas->Bottom;

	ClientSize = Drawing::Size(right + 10, bottom + 10);
}

HRESULT FormImager::OnServerStopped(int reason)
{
	ReleaseIPC();
	return 0;
}

HRESULT FormImager::OnFrameInit(int frameWidth, int frameHeight, int frameDepth)
{
	Init(frameWidth, frameHeight, frameDepth);
	return 0;
} 

// will work with Imager.exe release > 2.0 only:
HRESULT FormImager::OnNewFrameEx(void * pBuffer, FrameMetadata2 *pMetadata)
{
	labelFrameCounter->Text = "Frame counter HW/SW: " + pMetadata->CounterHW.ToString() + "/" + pMetadata->Counter.ToString();
    String ^s = "PIF  ";
    for (int i = 0; i < pMetadata->PIFnDI; i++)
        s += String::Format("  DI{0}:{1}", i + 1, (pMetadata->PIFDI >> i) & 1);
    for (int i = 0; i < pMetadata->PIFnAI; i++)
        s += String::Format("  AI{0}:{1}", i + 1, pMetadata->PIFAI[i]);
    labelPIF->Text = s;
    return NewFrame((short*)pBuffer, pMetadata->Counter);
}

HRESULT FormImager::OnNewFrame(char * pBuffer, int frameCounter)
{
	return NewFrame((short*)pBuffer, frameCounter);
}

HRESULT FormImager::NewFrame(short *ImgBuf, int frameCounter)
{
	DateTime time = DateTime::Now;
	TimeSpan ts = time - LastFrameTime;
	List->Add(ts.TotalMilliseconds);
	LastFrameTime = time;

	FrameCounter0 = frameCounter;
	FrameCounter1++;

	for ( int x = 0; x < FrameSize; x++ ) Values[x] = ImgBuf[x];

	if(!Painted)
	{
		GetBitmap(bmp, Values);
		pictureBox->Invalidate();
		Painted = true;
	}

	return 0;
}

HRESULT FormImager::OnInitCompleted(void)
{
	progressBarInit->Maximum = ipc->GetInitCounter(ImagerIndex);
	progressBarInit->Value = 0;
	progressBarInit->Visible = true;
	GetSoftwareInfo();
	GetDeviceInfo(); 
	GetDeviceSetup(); 
	InitMeasureAreas();
	labelFW->Text = String::Format("FW: {0}", Dev.Info.FW_Cypress);
	label1->Text = "Connected with #" + Dev.Info.SerialNumber.ToString();
	Colors = (TIPCMode(ipc->GetIPCMode(ImagerIndex)) == ipcColors);
	Connected = true;
	return S_OK;
}

HRESULT FormImager::OnConfigChanged(long reserved)
{
	GetDeviceInfo(); 
	GetDeviceSetup(); 
	InitMeasureAreas();
	return S_OK;
}



}
