#include "stdafx.h"
#include "time.h"
#include "FormMain.h"

//#define POLLING

namespace IPCSample {

unsigned char clip(int val) { return (val <= 255) ? ((val > 0) ? val : 0) : 255; };

FormMain::FormMain(HWND hostHandle)
{
	InitializeComponent();
	FrameIRWidth = FrameIRHeight = FrameIRDepth = 0;
	FrameVisWidth = FrameVisHeight = FrameVisDepth = 0;
    MetadataSize = 0;
    bmpIR = gcnew Bitmap(100, 100, System::Drawing::Imaging::PixelFormat::Format24bppRgb);
	bmpVis = gcnew Bitmap( 100 , 100 , System::Drawing::Imaging::PixelFormat::Format24bppRgb );
	ipc = nullptr;
	ipcInitialized = false;
	frameInitialized = false;
	visframeInitialized = false;
	Connected = false;
	PaintedIR = PaintedVis = false;
	MeasureAreaCount = 0;
	Margin = groupBoxVideoIR->Size - pictureBoxIR->Size;
	ListIR = gcnew System::Collections::ArrayList(100);
	ListVis = gcnew System::Collections::ArrayList(100);
}

FormMain::~FormMain()
{
	if (components)
		delete components;
}

void FormMain::AppInit(void)
{
	this->Text = this->Text + " (Rel. " + this->Version->ToString() + ")";
	InitIR(160, 120, 2);
	InitVis(640, 480, 2);
	ipc = gcnew IPC(1);
#ifdef POLLING 
	Application::Idle +=  gcnew EventHandler(this, &FormMain::Application_Idle);
#endif
}

System::Void FormMain::textBoxInstanceName_TextChanged(System::Object^  sender, System::EventArgs^  e)
{
	ReleaseIPC();
}

void FormMain::InitIPC(void) 
{
	 if(ipc && !ipcInitialized)
	 {
		HRESULT hr = ipc->Init(0, textBoxInstanceName->Text);
		
		if(FAILED(hr))
			ipcInitialized = frameInitialized = visframeInitialized = false;
		else
		{
#ifndef POLLING 
			ipc->OnServerStopped = gcnew IPC::delOnServerStopped(this, &FormMain::OnServerStopped );
			ipc->SetCallback_OnServerStopped(0, ipc->OnServerStopped);

			ipc->OnFrameIRInit = gcnew IPC::delOnFrameInit(this, &FormMain::OnFrameIRInit );
			ipc->SetCallback_OnFrameInit(0, ipc->OnFrameIRInit);

			ipc->OnNewFrameIREx2 = gcnew IPC::delOnNewFrameEx2(this, &FormMain::OnNewFrameIR );
			ipc->SetCallback_OnNewFrameEx2(0, ipc->OnNewFrameIREx2);

			ipc->OnFrameVisInit = gcnew IPC::delOnFrameInit(this, &FormMain::OnFrameVisInit );
			ipc->SetCallback_OnVisibleFrameInit(0, ipc->OnFrameVisInit);

			ipc->OnNewFrameVisEx2 = gcnew IPC::delOnNewFrameEx2(this, &FormMain::OnNewFrameVis );
			ipc->SetCallback_OnNewVisibleFrameEx2(0, ipc->OnNewFrameVisEx2);

			ipc->OnConfigChanged = gcnew IPC::delOnConfigChanged(this, &FormMain::OnConfigChanged );
			ipc->SetCallback_OnConfigChanged(0, ipc->OnConfigChanged);

			ipc->OnInitCompleted = gcnew IPC::delOnInitCompleted(this, &FormMain::OnInitCompleted );
			ipc->SetCallback_OnInitCompleted(0, ipc->OnInitCompleted);

            ipc->OnFileCommandReady = gcnew IPC::delOnStringSend(this, &FormMain::OnFileCommandReady);
            ipc->SetCallback_OnFileCommandReady(0, ipc->OnFileCommandReady);
#endif
			hr = ipc->Run(0);
			ipcInitialized = SUCCEEDED(hr);

			LastFrameIRTime = LastFrameVisTime = DateTime::Now;
		}
		label1->Text = hr ? "NOT CONNECTED" : "OK";

	 }
}

void FormMain::ReleaseIPC(void) 
{
	Connected = false;
	if(ipc && ipcInitialized)
	{
		ipc->Release(0);
		ipcInitialized = false;
	}
}

void FormMain::AppExit(void)
{
#ifdef POLLING 
	Application::Idle -=  gcnew EventHandler(this, &FormMain::Application_Idle);
#endif
	ReleaseIPC();
	delete ipc;
}

void FormMain::GetBitmap(Bitmap^ Bmp, array<short>^values, array<Byte> ^rgbValues, bool color)
{
	int stride_diff;
	// Lock the bitmap's bits.  
	System::Drawing::Rectangle rect = System::Drawing::Rectangle(0, 0, Bmp->Width, Bmp->Height);
	Imaging::BitmapData^ bmpData = Bmp->LockBits( rect, Imaging::ImageLockMode::ReadWrite, Bmp->PixelFormat );
	stride_diff = bmpData->Stride - Bmp->Width*3;

	// Get the address of the first line.
	IntPtr ptr = bmpData->Scan0;

	int w = Bmp->Width, h = Bmp->Height;
	if(color)
	{
		for ( int dst=0, src=0, y=0; y < h; y++, dst += stride_diff) 
			for ( int x=0; x < w; x++, src++, dst+=3 ) 
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

		for ( int dst=0, src=0, y=0; y < h; y++, dst += stride_diff) 
			for ( int x=0; x < w; x++, src++, dst+=3 ) 
				rgbValues[dst] = rgbValues[dst+1] = rgbValues[dst+2] = min(max((int)(Fact * (values[src] - mn)), 0), 255);
	}



	// Copy the RGB values back to the bitmap
	System::Runtime::InteropServices::Marshal::Copy( rgbValues, 0, ptr, rgbValues->Length );

	// Unlock the bits.
	Bmp->UnlockBits( bmpData );
}


void FormMain::GetBitmap_Limits(array<short>^Values, short *min, short *max)
{
	int y;
	double Sum, Mean, Variance;
	if(!Values) return;

	Sum = 0;
	int size = Values->Length;
	for (y=0; y < size; y++ ) 
		Sum += Values[y];
	Mean = (double)Sum / size;
	Sum = 0;
	for (y=0; y < size; y++ ) 
		Sum += (Mean - Values[y]) * (Mean - Values[y]);
	Variance = Sum / size;
	Variance = Math::Sqrt(Variance);
	Variance *= 3;  // 3 Sigma
	*min = short(Mean - Variance);
	*max = short(Mean + Variance);
}


double FormMain::GetFramerate(System::Collections::ArrayList ^List)
{
	double Mean = 0, x;
	IEnumerator^ Enum = List->GetEnumerator();
	while ( Enum->MoveNext() )
	{
		x = safe_cast<double>(Enum->Current);
		Mean += x;
	}
	Mean /= List->Count;
	Enum->Reset();
	while ( Enum->MoveNext() )
		x = Mean - safe_cast<double>(Enum->Current);
	List->Clear();
	return 1000.0 / Mean;
}

void FormMain::Application_Idle(Object ^sender, EventArgs ^e)
{
#ifdef POLLING 
	if(Connected)
	{
		int Size;
		void *Buffer;
        if (frameInitialized || visframeInitialized)
        {
            FrameMetadata2 *Metadata = (FrameMetadata2*)(new char[MetadataSize]);
            if (frameInitialized)
            {
                Size = FrameIRWidth * FrameIRHeight * FrameIRDepth;
                Buffer = new char[Size];
                if (ipc->GetFrameQueue(0))
                if (SUCCEEDED(ipc->GetFrame2(0, 0, Buffer, Size, Metadata, MetadataSize)))
                    OnNewFrameIR(Buffer, Metadata);
                delete[] Buffer;
            }
            if (visframeInitialized)
            {
                Size = FrameVisWidth * FrameVisHeight * FrameVisDepth;
                void *Buffer = new char[Size];
                if (ipc->GetVisibleFrameQueue(0))
                if (SUCCEEDED(ipc->GetVisibleFrame2(0, 0, Buffer, Size, Metadata, MetadataSize)))
                    OnNewFrameVis(Buffer, Metadata);
                delete[] Buffer;
            }
            delete[] Metadata;
        }
	}
#endif
}

HRESULT FormMain::MainTimer100ms(void)
{
	PaintedIR = PaintedVis = false;
	if(Connected)
	{
		if(progressBarInit->Visible)
		{
			int i = ipc->GetInitCounter(0);
			progressBarInit->Value = progressBarInit->Maximum - i;
			progressBarInit->Visible = (i != 0);
		}
	}
#ifdef POLLING
	if(ipcInitialized)
	{
		int frameWidth, frameHeight, frameDepth, frameMetadataSize;
		WORD State = ipc->GetIPCState(0, true);
		if(State & IPC_EVENT_SERVER_STOPPED)
			OnServerStopped(0);
		if(State & IPC_EVENT_CONFIG_CHANGED)
			OnConfigChanged(0);
		if(!Connected && (State & IPC_EVENT_INIT_COMPLETED))
			OnInitCompleted();
		if(State & IPC_EVENT_FRAME_INIT)
			if(SUCCEEDED(ipc->GetFrameConfig(0, &frameWidth, &frameHeight, &frameDepth)))
            {
                InitIR(frameWidth, frameHeight, frameDepth);
                if (SUCCEEDED(ipc->GetFrameMetadataSize(0, &frameMetadataSize)))
                    MetadataSize = frameMetadataSize;
            }
            if (State & IPC_EVENT_FRAME_INIT)
			if(SUCCEEDED(ipc->GetVisibleFrameConfig(0, &frameWidth, &frameHeight, &frameDepth)))
				InitVis(frameWidth, frameHeight, frameDepth);
	}
#endif
    return S_OK;
}

HRESULT FormMain::MainTimer1000ms(void)
{
	FramerateIR = GetFramerate(ListIR);
	FramerateVis = GetFramerate(ListVis);

	if(Connected)
	{
		for(int i=0; i < groupBoxMeasureAreas->Controls->Count; i++)
			((Label^)groupBoxMeasureAreas->Controls->default[i])->Text
				= String::Format("Temp[{0}]: {1:##0.0}°C", i, ipc->GetTempMeasureArea(0, i));
	}
	return S_OK;
}


void FormMain::GetSoftwareInfo(void)
{
	Dev.SoftInfo.Application	= gcnew IPCSample::Version(ipc->GetVersionApplication(0));
	Dev.SoftInfo.CD_DLL			= gcnew IPCSample::Version(ipc->GetVersionCD_DLL(0));
	Dev.SoftInfo.HID_DLL		= gcnew IPCSample::Version(ipc->GetVersionHID_DLL(0)); 
	Dev.SoftInfo.IPC_DLL		= gcnew IPCSample::Version(ipc->GetVersionIPC_DLL(0));
}

void FormMain::GetDeviceInfo(void)
{
	Dev.Info.HW_Model			= ipc->GetHardware_Model(0);
	Dev.Info.HW_Spec			= ipc->GetHardware_Spec(0);
	Dev.Info.SerialNumber		= ipc->GetSerialNumber(0);
	Dev.Info.SerialNumberULIS	= ipc->GetSerialNumber(0);
	Dev.Info.FW_MSP				= ipc->GetFirmware_MSP(0);
	Dev.Info.FW_Cypress			= ipc->GetFirmware_Cypress(0);
	Dev.Info.PID				= ipc->GetPID(0);
	Dev.Info.VID				= ipc->GetVID(0);
	Dev.Info.OpticsFOV			= ipc->GetOpticsFOV(0, ipc->GetOpticsIndex(0));
	Dev.Info.TempMinRange		= ipc->GetTempMinRange(0, ipc->GetTempRangeIndex(0));
	Dev.Info.TempMaxRange		= ipc->GetTempMaxRange(0, ipc->GetTempRangeIndex(0));
	Dev.Info.TempRangeDecimal_CaliValue	= ipc->GetTempRangeDecimal(0, false);
	Dev.Info.TempRangeDecimal_EffValue	= ipc->GetTempRangeDecimal(0, true);
}
void FormMain::GetDeviceSetup(void)
{
	int cnt = ipc->GetOpticsCount(0);
	MeasureAreaCount = ipc->GetMeasureAreaCount(0);
}

void FormMain::SetFlag(bool flag)
{
	ipc->SetFlag(0, flag);
}

void FormMain::RenewFlag(void)
{
	buttonFlag->Text = String::Format("Renew ({0})", ipc->RenewFlag(0) ? "Success" : "Failed");
}

void FormMain::InitIR(int frameWidth, int frameHeight, int frameDepth)
{
	FrameIRWidth = frameWidth;
	FrameIRHeight = frameHeight;
	FrameIRSize = FrameIRWidth * FrameIRHeight;
	FrameIRRatio = (double)FrameIRWidth /  (double)FrameIRHeight;
	FrameIRDepth = frameDepth;
	timer1->Enabled = true;
	bmpIR = gcnew Bitmap( FrameIRWidth , FrameIRHeight , System::Drawing::Imaging::PixelFormat::Format24bppRgb );
	System::Drawing::Rectangle rect = System::Drawing::Rectangle(0, 0, bmpIR->Width, bmpIR->Height);
	Imaging::BitmapData^ bmpData = bmpIR->LockBits( rect, Imaging::ImageLockMode::ReadWrite, bmpIR->PixelFormat );
	int stride = bmpData->Stride;
	bmpIR->UnlockBits( bmpData );
	rgbValuesIR = gcnew array<Byte>(stride * FrameIRHeight);
	ValuesIR = gcnew array<short>(FrameIRSize);

	groupBoxVideoIR->Size = Drawing::Size(FrameIRWidth, FrameIRHeight) + Margin;
	UpdateSize();
	frameInitialized = true;
}

void FormMain::InitVis(int frameWidth, int frameHeight, int frameDepth)
{
	FrameVisWidth = frameWidth;
	FrameVisHeight = frameHeight;
	FrameVisSize = FrameVisWidth * FrameVisHeight;
	FrameVisRatio = (double)FrameVisWidth /  (double)FrameVisHeight;
	FrameVisDepth = frameDepth;
	timer1->Enabled = true;
	bmpVis = gcnew Bitmap( FrameVisWidth , FrameVisHeight , System::Drawing::Imaging::PixelFormat::Format24bppRgb );
	System::Drawing::Rectangle rect = System::Drawing::Rectangle(0, 0, bmpVis->Width, bmpVis->Height);
	Imaging::BitmapData^ bmpData = bmpVis->LockBits( rect, Imaging::ImageLockMode::ReadWrite, bmpVis->PixelFormat );
	int stride = bmpData->Stride;
	bmpVis->UnlockBits( bmpData );
	rgbValuesVis = gcnew array<Byte>(stride * FrameVisHeight);
	ValuesVis = gcnew array<short>(FrameVisSize);

	groupBoxVideoVis->Size = Drawing::Size(FrameVisWidth, FrameVisHeight) + Margin;
	UpdateSize();
	visframeInitialized = true;
}

void FormMain::InitMeasureAreas(void)
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

void FormMain::UpdateSize(void)
{
	groupBoxVideoVis->Left = groupBoxVideoIR->Right + 10;
	int right = max(max(groupBoxVideoVis->Right, groupBoxMeasureAreas->Right), buttonFlag->Right);
	int bottom = max(groupBoxVideoVis->Bottom, groupBoxMeasureAreas->Bottom);

	ClientSize = Drawing::Size(right + 10, bottom + 10);
	labelFrameIR->Left = groupBoxVideoIR->Left;
	labelFrameVis->Left = groupBoxVideoVis->Left;
}

HRESULT FormMain::OnServerStopped(int reason)
{
	ReleaseIPC();
	Graphics::FromImage(bmpIR)->FillRectangle(gcnew SolidBrush(Color::Black), 0, 0, bmpIR->Width, bmpIR->Height);
	Graphics::FromImage(bmpVis)->FillRectangle(gcnew SolidBrush(Color::Black), 0, 0, bmpVis->Width, bmpVis->Height);
	pictureBoxIR->Invalidate();
	pictureBoxVis->Invalidate();
	labelFrameIR->Text = "0 / 0Hz";
	labelFrameVis->Text = "0 / 0Hz";
	return 0;
}

HRESULT FormMain::OnFrameIRInit(int frameWidth, int frameHeight, int frameDepth)
{
	InitIR(frameWidth, frameHeight, frameDepth);
	return 0;
} 

HRESULT FormMain::OnNewFrameIR(void * pBuffer, FrameMetadata2 *pMetadata)
{
	labelFrameIR->Text = String::Format("{0} / {1}Hz", pMetadata->CounterHW, Math::Round(FramerateIR));

	short *ImgBuf = (short*)pBuffer;
	DateTime time = DateTime::Now;
	TimeSpan ts = time - LastFrameIRTime;
	ListIR->Add(ts.TotalMilliseconds);
	LastFrameIRTime = time;

	for ( int x = 0; x < FrameIRSize; x++ ) ValuesIR[x] = ImgBuf[x];

	if(!PaintedIR)
	{
		GetBitmap(bmpIR, ValuesIR, rgbValuesIR, Colors);
		pictureBoxIR->Invalidate();
		PaintedIR = true;
	}

	return 0;
}

HRESULT FormMain::OnFrameVisInit(int frameWidth, int frameHeight, int frameDepth)
{
	InitVis(frameWidth, frameHeight, frameDepth);
	return 0;
} 

HRESULT FormMain::OnNewFrameVis(void * pBuffer, FrameMetadata2 *pMetadata)
{
	labelFrameVis->Text = String::Format("{0} / {1}Hz", pMetadata->CounterHW, Math::Round(FramerateVis));

	short *ImgBuf = (short*)pBuffer;
	DateTime time = DateTime::Now;
	TimeSpan ts = time - LastFrameVisTime;
	ListVis->Add(ts.TotalMilliseconds);
	LastFrameVisTime = time;

	for ( int x = 0; x < FrameVisSize; x++ ) ValuesVis[x] = ImgBuf[x];

	if(!PaintedVis)
	{
		GetBitmap(bmpVis, ValuesVis, rgbValuesVis, true);
		pictureBoxVis->Invalidate();
		PaintedVis = true;
	}

	return 0;
}

HRESULT FormMain::OnInitCompleted(void)
{
	progressBarInit->Maximum = ipc->GetInitCounter(0);
	progressBarInit->Value = 0;
	progressBarInit->Visible = true;
	GetSoftwareInfo();
	GetDeviceInfo(); 
	GetDeviceSetup(); 
	InitMeasureAreas();
	label1->Text = "Connected with #" + Dev.Info.SerialNumber.ToString();
	Colors = (TIPCMode(ipc->GetIPCMode(0)) == ipcColors);
	Connected = true;
	UpdateSize();
	return S_OK;
}

HRESULT FormMain::OnConfigChanged(long reserved)
{
	GetDeviceInfo(); 
	GetDeviceSetup(); 
	InitMeasureAreas();
	return S_OK;
}

HRESULT FormMain::OnFileCommandReady(wchar_t *path)
{
	System::Windows::Forms::MessageBox::Show( gcnew String(path), "File was saved to path:", MessageBoxButtons::OK, MessageBoxIcon::Exclamation );
	return S_OK;
}

System::Void FormMain::checkBoxDock_CheckedChanged(System::Object^  sender, System::EventArgs^  e) 
{
	UpdateSize();
}

void FormMain::FileOpen(void)
{
	if(ipc) 
	{
		System::Windows::Forms::OpenFileDialog^ Dlg = gcnew System::Windows::Forms::OpenFileDialog;
		String^ s = "All files|*.*|Radiom. files|*.ravi;*.jpg;*.tiff|Ravi|*.ravi|Avi|*.avi|Jpeg|*.jpg|Tiff|*.tiff|";
		Dlg->Filter = s->Remove(s->Length-1);
		Dlg->FilterIndex  = 2;
		Dlg->CheckFileExists = true;
		if (Dlg->ShowDialog() == System::Windows::Forms::DialogResult::OK) 
		{
			wchar_t Filename[1024];
			pin_ptr<const wchar_t> wch = PtrToStringChars(Dlg->FileName); 
			wcscpy_s(Filename, 1024, wch); 			

			USHORT retVal = ipc->FileOpen(0, Filename);
			if(retVal != 0)
				System::Windows::Forms::MessageBox::Show( gcnew String(Filename), "Could not open file:", MessageBoxButtons::OK, MessageBoxIcon::Error );
		}

	}
}

}
