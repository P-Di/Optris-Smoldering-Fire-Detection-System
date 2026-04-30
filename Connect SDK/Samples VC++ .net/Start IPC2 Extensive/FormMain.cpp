#include "stdafx.h"
#include "time.h"
#include "FormMain.h"

//#define POLLING 

#ifdef _WIN64
#define OS 64
#else
#define OS 32
#endif

namespace IPCSample {

unsigned char clip(int val) { return (val <= 255) ? ((val > 0) ? val : 0) : 255; };

FormMain::FormMain(HWND hostHandle)
{
	InitializeComponent();
	FrameWidth = 0;
	FrameHeight = 0;
	FrameDepth = 0;
    MetadataSize = 0;
	bmp = gcnew Bitmap( 100 , 100 , System::Drawing::Imaging::PixelFormat::Format24bppRgb );
	ipc = nullptr;
	ipcInitialized = false;
	frameInitialized = false;
	Connected = false;
	Painted = false;
	radioButtonRange3Sigma->Checked = true;
	MeasureAreaCount = 0;
	Margin = groupBoxVideo->Size - pictureBox->Size;
	FC0 = FC1 = 0;
	AppendLog = false;
	ResetPending = false;
	List = gcnew System::Collections::ArrayList(100);
	char ch[256];
	::GetSystemDirectoryA(ch, 256);
    HotPen = gcnew Pen(Color::Red);
    ColdPen = gcnew Pen(Color::Blue);
    ContrastPen = gcnew Pen(Color::Yellow);
}

FormMain::~FormMain()
{
	if (components)
		delete components;
}

void FormMain::AppInit(void)
{
	Text += String::Format(" (Rel. {0} (x{1}))", Version, OS);
	Init(160, 120, 2);
	ipc = gcnew IPC(1);
	//ipc = gcnew IPC(1, L".\\IPC Sample.log", 2, false);
	//ipc = gcnew IPC(1, ~(LOGGRP_FRAME | LOGGRP_ALIVE | LOGGRP_STATE));

#ifdef POLLING 
	Application::Idle +=  gcnew EventHandler(this, &FormMain::Application_Idle);
#endif
}

System::Void FormMain::textBoxInstanceName_TextChanged(System::Object^  sender, System::EventArgs^  e)
{
	ReleaseIPC();
}

void FormMain::InitIPC() 
{
	HRESULT hr;
	 if(ipc && !ipcInitialized)
	 {
		hr = ipc->Init(0, textBoxInstanceName->Text);
		
		if(FAILED(hr))
		{
			ipcInitialized = frameInitialized = false;
		}
		else
		{
#ifndef POLLING 
			ipc->OnServerStopped = gcnew IPC::delOnServerStopped(this, &FormMain::OnServerStopped );
			ipc->SetCallback_OnServerStopped(0, ipc->OnServerStopped);

			ipc->OnFrameIRInit = gcnew IPC::delOnFrameInit(this, &FormMain::OnFrameInit );
			ipc->SetCallback_OnFrameInit(0, ipc->OnFrameIRInit);

			ipc->OnNewFrameIREx2 = gcnew IPC::delOnNewFrameEx2(this, &FormMain::OnNewFrameEx );
			ipc->SetCallback_OnNewFrameEx2(0, ipc->OnNewFrameIREx2);

			ipc->OnConfigChanged = gcnew IPC::delOnConfigChanged(this, &FormMain::OnConfigChanged );
			ipc->SetCallback_OnConfigChanged(0, ipc->OnConfigChanged);

			ipc->OnInitCompleted = gcnew IPC::delOnInitCompleted(this, &FormMain::OnInitCompleted );
			ipc->SetCallback_OnInitCompleted(0, ipc->OnInitCompleted);

			ipc->OnFileCommandReady = gcnew IPC::delOnStringSend(this, &FormMain::OnFileCommandReady);
			ipc->SetCallback_OnFileCommandReady(0, ipc->OnFileCommandReady);

			ipc->OnNewNMEAString = gcnew IPC::delOnStringSend(this, &FormMain::OnNewNMEAString);
			ipc->SetCallback_OnNewNMEAString(0, ipc->OnNewNMEAString);

#endif
			hr = ipc->Run(0);
			ipcInitialized = SUCCEEDED(hr);
			LastFrameTime = DateTime::Now;
			Tmax = 0;
			ResetPending = true;
			FC0 = FC1 = 0;
		}
		label1->Text = hr ? "NOT CONNECTED" : "OK";

	 }
}

void FormMain::ReleaseIPC(void) 
{
	Connected = false;
	if(ipc && ipcInitialized)
	{
		if(OldApplRect)
		{
			ipc->SetMainWindowEmbedded(0, OldEmbeddedState);
			ipc->SetMainWindowRect(0, OldApplRect);
		}
		ipc->Release(0);
		ipcInitialized = false;
	}
}

void FormMain::AppExit(void)
{
#ifdef POLLING 
	Application::Idle -=  gcnew EventHandler(this, &FormMain::Application_Idle);
#endif
	if (startedPix && ipc){
		ipc->CloseApplication(0);
	}
	ReleaseIPC();
	delete ipc;
}

void FormMain::GetBitmap(Bitmap^ Bmp, array<short>^values)
{
	int stride_diff;
	// Lock the bitmap's bits.  
	System::Drawing::Rectangle rect = System::Drawing::Rectangle(0, 0, Bmp->Width, Bmp->Height);
	Imaging::BitmapData^ bmpData = Bmp->LockBits( rect, Imaging::ImageLockMode::ReadWrite, Bmp->PixelFormat );
	stride_diff = bmpData->Stride - FrameWidth*3;

	// Get the address of the first line.
	IntPtr ptr = bmpData->Scan0;

	if(checkBoxColors->Checked)
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


void FormMain::GetBitmap_Limits(array<short>^Values, short *min, short *max)
{
	int y, Count;
	double Sum, Mean, Variance;
	if(!Values) return;

	if(radioButtonRangeMinMax->Checked)
	{
		*min = SHRT_MAX;
		*max = SHRT_MIN;
		for (y=0; y < FrameSize; y++ ) 
		{
			if(Values[y] != INVALIDPIXEL)
			{
				*min = min(Values[y], *min);
				*max = max(Values[y], *max);
			}
		}
		return;
	}

	Sum = 0;
	Count = 0;
	for (y=0; y < FrameSize; y++ ) 
		if(Values[y] != INVALIDPIXEL)
		{
			Sum += Values[y];
			Count++;
		}
	Mean = (double)Sum / Count;
	Sum = 0;
	for (y=0; y < FrameSize; y++ ) 
		if(Values[y] != INVALIDPIXEL)
			Sum += (Mean - Values[y]) * (Mean - Values[y]);
	Variance = Sum / Count;
	Variance = Math::Sqrt(Variance);
	Variance *= radioButtonRange1Sigma->Checked ? 1: 3;  // 1 or 3 Sigma
	*min = short(Mean - Variance);
	*max = short(Mean + Variance);
}


void FormMain::Application_Idle(Object ^sender, EventArgs ^e)
{
#ifdef POLLING 
	if(Connected && frameInitialized)
	{
		int Size = FrameWidth * FrameHeight * FrameDepth;
		void *Buffer = new char[Size];
		FrameMetadata2 *Metadata = (FrameMetadata2*)(new char[MetadataSize]);
		if(ipc->GetFrameQueue(0))
			if(SUCCEEDED(ipc->GetFrame2(0, 0, Buffer, Size, Metadata, MetadataSize)))
				OnNewFrameEx(Buffer, Metadata);
		delete [] Buffer;
        delete [] Metadata;
	}
#endif
}

HRESULT FormMain::MainTimer100ms(void)
{
	Painted = false;
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
		WORD State = ipc->GetIPCState(0, true);
		if(State & IPC_EVENT_SERVER_STOPPED)
			OnServerStopped(0);
		if(State & IPC_EVENT_CONFIG_CHANGED)
			OnConfigChanged(0);
		if(!Connected && (State & IPC_EVENT_INIT_COMPLETED))
			OnInitCompleted();
		if(State & IPC_EVENT_FRAME_INIT)
		{
            int frameWidth, frameHeight, frameDepth, frameMetadataSize;
            if (SUCCEEDED(ipc->GetFrameConfig(0, &frameWidth, &frameHeight, &frameDepth)))
            {
                Init(frameWidth, frameHeight, frameDepth);
                if (SUCCEEDED(ipc->GetFrameMetadataSize(0, &frameMetadataSize)))
                {
                    MetadataSize = frameMetadataSize;
                }
                
            }
		}
		if(Connected && (State & IPC_EVENT_FILE_CMD_READY))
		{
			wchar_t Filename[1024];
			ipc->GetPathOfStoredFile(0, Filename, 1024);
			OnFileCommandReady(Filename);
		}
		if (Connected && (State & IPC_EVENT_NEW_NMEA_STRING))
		{
			wchar_t NMEAString[1024];
			ipc->GetNewNMEAString(0, NMEAString, 1024);
			OnNewNMEAString(NMEAString);
		}
	}
#endif
    return S_OK;
}

HRESULT FormMain::MainTimer1000ms(void)
{
	double Mean = 0, Sigma = 0, x, last=0;
	IEnumerator^ Enum = List->GetEnumerator();
	while ( Enum->MoveNext() )
	{
		x = safe_cast<double>(Enum->Current);
		Tmax = max(x - last, Tmax);
		last = x;
		Mean += x;
	}
	Mean /= List->Count;
	Enum->Reset();
	while ( Enum->MoveNext() )
		x = Mean - safe_cast<double>(Enum->Current), 
		Sigma += x*x;
	Sigma = Math::Sqrt(Sigma / List->Count);
	List->Clear();

	labelFramerate->Text = String::Format(L"{0:0.0}Hz,   \u0394t={1:0.0}ms, \u0394tmax={2:0}ms, \u03C3={3:0.0}ms",  
		1000.0 / Mean, Mean, Tmax, Sigma);

	if(Connected)
	{
		labelTempTarget->Text	= String::Format("Target-Temp: {0:##0.0}°C", ipc->GetTempTarget(0));
		labelTempChip->Text		= String::Format("Chip-Temp: {0:##0.0}°C", ipc->GetTempChip(0));
		labelTempFlag->Text		= String::Format("Flag-Temp: {0:##0.0}°C", ipc->GetTempFlag(0));
		labelTempBox->Text		= String::Format("Box-Temp: {0:##0.0}°C", ipc->GetTempBox(0));
		labelTempOptics->Text	= String::Format("Optics-Temp: {0:##0.0}°C", ipc->GetTempOptics(0));

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
	GetOptics();
	GetVideoFormats();
	Dev.Info.HW_Model			= ipc->GetHardware_Model(0);
	Dev.Info.HW_Spec			= ipc->GetHardware_Spec(0);
	Dev.Info.SerialNumber		= ipc->GetSerialNumber(0);
	Dev.Info.SerialNumberULIS	= ipc->GetSerialNumber(0);
	Dev.Info.FW_MSP				= ipc->GetFirmware_MSP(0);
	Dev.Info.FW_Cypress			= ipc->GetFirmware_Cypress(0);
	Dev.Info.PID				= ipc->GetPID(0);
	Dev.Info.VID				= ipc->GetVID(0);
	Dev.Info.OpticsFOV			= ipc->GetOpticsFOV(0, ipc->GetOpticsIndex(0));
	Dev.Info.OpticsSerialNumber	= ipc->GetOpticsSerialNumber(0, ipc->GetOpticsIndex(0));
	Dev.Info.TempMinRange		= ipc->GetTempMinRange(0, ipc->GetTempRangeIndex(0));
	Dev.Info.TempMaxRange		= ipc->GetTempMaxRange(0, ipc->GetTempRangeIndex(0));
	Dev.Info.TempRangeDecimal_CaliValue	= ipc->GetTempRangeDecimal(0, false);
	Dev.Info.TempRangeDecimal_EffValue	= ipc->GetTempRangeDecimal(0, true);
}
void FormMain::GetDeviceSetup(void)
{
	int cnt = ipc->GetOpticsCount(0);
	if(cnt > 0)
		comboBoxOptics->SelectedIndex	= Dev.Setup.OpticsIndex		= ipc->GetOpticsIndex(0);
	else
		comboBoxOptics->SelectedIndex	= -1;
	if(ipc->GetTempRangeCount(0) > 0)
		comboBoxTempRange->SelectedIndex= Dev.Setup.TempRangeIndex	= ipc->GetTempRangeIndex(0);
	else
		comboBoxTempRange->SelectedIndex= -1;
	if(ipc->GetVideoFormatCount(0) > 0)
		comboBoxVideoFormats->SelectedIndex= Dev.Setup.VideoFormatIndex	= ipc->GetVideoFormatIndex(0);
	else
		comboBoxVideoFormats->SelectedIndex= -1;
	
	checkBoxColors->Checked =  TIPCMode(Dev.Setup.IPCMode = ipc->GetIPCMode(0)) == ipcColors;
	Dev.Setup.FixedEmissivity = ipc->GetFixedEmissivity(0);
	Dev.Setup.FixedTransmissivity = ipc->GetFixedTransmissivity(0);
	Dev.Setup.FixedTempAmbient = ipc->GetFixedTempAmbient(0);
	Dev.Setup.FixedTempReference = ipc->GetFixedTempReference(0);
	numericUpDownEmissivity->Value = Decimal(Dev.Setup.FixedEmissivity);
	numericUpDownTempAmbient->Value = Decimal(Dev.Setup.FixedTempAmbient);
	MeasureAreaCount = ipc->GetMeasureAreaCount(0);
}

void FormMain::GetOptics(void)
{
	int i, Cnt = ipc->GetOpticsCount(0);

	comboBoxOptics->Items->Clear();
	comboBoxOptics->Text = nullptr;
	for (i = 0; i < Cnt; i++)
	{
		String ^s = String::Format("{0}°", ipc->GetOpticsFOV(0, i));
		ULONG o_serno = ipc->GetOpticsSerialNumber(0, i);
		if (o_serno)
			s += String::Format(" (#{0})", o_serno);
		comboBoxOptics->Items->Add(s);
	}

	i = ipc->GetOpticsIndex(0);
	if(i < Cnt)	comboBoxOptics->SelectedIndex = i;
	GetTempRanges();
}

void FormMain::GetTempRanges(void)
{
	int i, Cnt = ipc->GetTempRangeCount(0);

	comboBoxTempRange->Items->Clear();
	comboBoxTempRange->Text = nullptr;
	for(i=0; i<Cnt; i++)
		comboBoxTempRange->Items->Add(String::Format("{0:##0.0}°C .. {1:##0.0}°C", ipc->GetTempMinRange(0, i), ipc->GetTempMaxRange(0, i)));
	i = ipc->GetTempRangeIndex(0);
	if(i < Cnt)	comboBoxTempRange->SelectedIndex = i;
}

void FormMain::GetVideoFormats(void)
{
	int i, Cnt = ipc->GetVideoFormatCount(0);

	comboBoxVideoFormats->Items->Clear();
	comboBoxVideoFormats->Text = nullptr;
	String ^sFormat = "{0}x{1}@{2:0}Hz";
	for(i=0; i<Cnt; i++)
	{
		VideoFormat vf;
		ipc->GetVideoFormat(0, i, &vf);
		String ^s;
		if(vf.WidthIR && vf.HeightIR && vf.FramerateIR)
		{
			s = String::Format(sFormat, vf.WidthIR, vf.HeightIR, vf.FramerateIR);
			if(vf.WidthVisible && vf.HeightVisible && vf.FramerateVisible)
				s += " / Vis:" + String::Format(sFormat, vf.WidthVisible, vf.HeightVisible, vf.FramerateVisible);
			comboBoxVideoFormats->Items->Add(s);
		}
		else
			comboBoxVideoFormats->Items->Add("wrong format");
	}

	i = ipc->GetVideoFormatIndex(0);
	if(i < Cnt)	comboBoxVideoFormats->SelectedIndex = i;
}

void FormMain::SetOpticsIndex(void)
{
	Dev.Setup.OpticsIndex = ipc->SetOpticsIndex(0, Decimal::ToUInt16(comboBoxOptics->SelectedIndex));
	GetTempRanges();
}

void FormMain::SetTempRangeIndex(void)
{
	Dev.Setup.TempRangeIndex = ipc->SetTempRangeIndex(0, Decimal::ToUInt16(comboBoxTempRange->SelectedIndex));
}

void FormMain::SetVideoFormatIndex(void)
{
	Dev.Setup.VideoFormatIndex = ipc->SetVideoFormatIndex(0, Decimal::ToUInt16(comboBoxVideoFormats->SelectedIndex));
}

void FormMain::SetFlag(bool flag)
{
	ipc->SetFlag(0, flag);
}

void FormMain::RenewFlag(void)
{
	buttonFlagRenew->Text = String::Format("Renew ({0})", ipc->RenewFlag(0) ? "Success" : "Failed");
}

void FormMain::SetEmissivity(void)
{
	Dev.Setup.FixedEmissivity = ipc->SetFixedEmissivity(0, Decimal::ToSingle(numericUpDownEmissivity->Value));
}

void FormMain::SetTempAmbient(void)
{
	Dev.Setup.FixedTempAmbient = ipc->SetFixedTempAmbient(0, Decimal::ToSingle(numericUpDownTempAmbient->Value));
}

void FormMain::Init(int frameWidth, int frameHeight, int frameDepth)
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
	groupBoxRangeMode->Top = groupBoxVideo->Bottom + 5;
	groupBoxFlag->Top = groupBoxRangeMode->Bottom + 5;
	groupBoxOptics->Top = groupBoxFlag->Bottom + 5;
	groupBoxFile->Top = groupBoxOptics->Bottom + 5;
	checkBoxDock->Top = groupBoxFile->Bottom + 5;
	UpdateSize();
	frameInitialized = true;
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
	int right = max(
		groupBoxVideo->Location.X + groupBoxVideo->Size.Width,
		groupBoxRangeMode->Location.X + groupBoxRangeMode->Size.Width);
	int bottom = max(
		groupBoxMeasureAreas->Location.Y + groupBoxMeasureAreas->Size.Height,
		checkBoxDock->Location.Y + checkBoxDock->Size.Height);

	groupBoxHostAppl->Visible = checkBoxDock->Checked && Connected;
	if(groupBoxHostAppl->Visible)
	{
		groupBoxHostAppl->Location = Drawing::Point(right + 10, groupBoxVideo->Location.Y);
		groupBoxHostAppl->Size = Drawing::Size(FrameWidth*2 + 20, FrameHeight*2 + 20);
		right = groupBoxHostAppl->Right;
		if(!OldApplRect)
		{
			Drawing::Rectangle r = ipc->GetMainWindowRect(0);
			OldApplRect = gcnew Drawing::Rectangle(r.Location, r.Size);
			OldEmbeddedState = ipc->GetMainWindowEmbedded(0);
			ipc->SetMainWindowEmbedded(0, true);
		}
	}
	else
	{
		if(OldApplRect)
		{
			ipc->SetMainWindowEmbedded(0, OldEmbeddedState);
			ipc->SetMainWindowRect(0, OldApplRect);
			OldApplRect = nullptr;
		}
	}

	this->Size = Drawing::Size(right + 20, bottom + 50);
}

void FormMain::ResizeHostAppl(void)
{
	if(groupBoxHostAppl->Visible && Connected)
	{
		double NewWidth = this->ClientSize.Width - groupBoxHostAppl->Left - 20;
		double NewHeight = (NewWidth + 20) / FrameRatio;
		if((groupBoxHostAppl->Top + NewHeight + 20) > this->ClientSize.Height)
		{
			NewHeight = this->ClientSize.Height - groupBoxHostAppl->Top - 20;
			NewWidth = (NewHeight - 20) * FrameRatio;
		}
		groupBoxHostAppl->Size = Drawing::Size((int)NewWidth, (int)NewHeight);
		Drawing::Rectangle r = groupBoxHostAppl->RectangleToScreen(groupBoxHostAppl->ClientRectangle);
		r.X += 10;
		r.Y += 20;
		r.Width -= 20;
		r.Height -= 30;
		ipc->SetMainWindowRect(0, r);
	}
}


HRESULT FormMain::OnServerStopped(int reason)
{
	ReleaseIPC();
	Graphics ^g = Graphics::FromImage(bmp);
	g->FillRectangle(gcnew SolidBrush(Color::Black), 0, 0, bmp->Width, bmp->Height);
	delete g;
	pictureBox->Invalidate();
	return 0;
}

HRESULT FormMain::OnFrameInit(int frameWidth, int frameHeight, int frameDepth)
{
	Init(frameWidth, frameHeight, frameDepth);
	return 0;
} 

// will work with Imager.exe release > 2.0 only:
HRESULT FormMain::OnNewFrameEx(void * pBuffer, FrameMetadata2 *pMetadata)
{
    int sz = sizeof(FrameMetadata2);
    unsigned char bf[88];
    memcpy(bf, pMetadata, 88);
	labelFrameCounter->Text = "Frame counter HW/SW: " + pMetadata->CounterHW.ToString() + "/" + pMetadata->Counter.ToString();
    String ^s = "PIF  ";
    for (int i = 0; i < pMetadata->PIFnDI; i++)
        s += String::Format("  DI{0}:{1}", i + 1, (pMetadata->PIFDI >> i) & 1);
    for (int i = 0; i < pMetadata->PIFnAI; i++)
        s += String::Format("  AI{0}:{1}", i + 1, pMetadata->PIFAI[i]);
    labelPIF->Text = s;

	switch(pMetadata->FlagState)
	{
	case fsFlagOpen: labelFlag->Text = "open"; labelFlag->ForeColor = Color::Green; labelFlag->BackColor = labelFlag1->BackColor; break;
	case fsFlagClose: labelFlag->Text = "closed"; labelFlag->ForeColor = Color::White; labelFlag->BackColor = Color::Red; break;
	case fsFlagOpening: labelFlag->Text = "opening"; labelFlag->ForeColor = SystemColors::WindowText; labelFlag->BackColor = Color::Yellow; break;
	case fsFlagClosing: labelFlag->Text = "closing"; labelFlag->ForeColor = SystemColors::WindowText; labelFlag->BackColor = Color::Yellow; break;
	default: labelFlag->Text = ""; labelFlag->ForeColor = labelFlag1->ForeColor; labelFlag->BackColor = labelFlag1->BackColor;
	}
    HotSpot = Drawing::Point((int)pMetadata->HotSpot.x, (int)pMetadata->HotSpot.y);
    ColdSpot = Drawing::Point((int)pMetadata->ColdSpot.x, (int)pMetadata->ColdSpot.y);

	return NewFrame((short*)pBuffer, pMetadata->Counter);
}

HRESULT FormMain::OnNewFrame(char * pBuffer, int frameCounter)
{
	return NewFrame((short*)pBuffer, frameCounter);
}

HRESULT FormMain::NewFrame(short *ImgBuf, int frameCounter)
{
	DateTime time = DateTime::Now;
	TimeSpan ts = time - LastFrameTime;
	List->Add(ts.TotalMilliseconds);
	LastFrameTime = time;

	FrameCounter0 = frameCounter;
	FrameCounter1++;
	if(ResetPending)
	{
		Tmax = 0; 
		FC0 = FrameCounter0;
		FC1 = FrameCounter1;
		ResetPending = false;
	}
	int fc0 = FrameCounter0 - FC0;
	int fc1 = FrameCounter1 - FC1;

	label2->Text = String::Format("Frames from Imager Application: {0}\nReceived frames: {1}\nNot recveived frames: {2}", 
		           fc0, fc1, fc0 - fc1);

	for ( int x = 0; x < FrameSize; x++ ) Values[x] = ImgBuf[x];

	if(!Painted)
	{
		GetBitmap(bmp, Values);
		pictureBox->Invalidate();
		Painted = true;
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
	labelVersionAppl->Text = String::Format("Host-Appl.: {0}.{1}.{2}.{3}", 
		Dev.SoftInfo.Application->Major,
		Dev.SoftInfo.Application->Minor,
		Dev.SoftInfo.Application->Build,
		Dev.SoftInfo.Application->Revision);
	labelVersionIPC->Text = String::Format("IPC-DLL: {0}.{1}.{2}.{3}", 
		Dev.SoftInfo.IPC_DLL->Major,
		Dev.SoftInfo.IPC_DLL->Minor,
		Dev.SoftInfo.IPC_DLL->Build,
		Dev.SoftInfo.IPC_DLL->Revision);
	labelHW->Text = String::Format("HW: {0}", Dev.Info.HW_Model);
	labelFW->Text = String::Format("FW: {0}", Dev.Info.FW_Cypress);
	label1->Text = "Connected with #" + Dev.Info.SerialNumber.ToString();
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

HRESULT FormMain::OnNewNMEAString(wchar_t *path)
{
	Text = gcnew String(path);
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

System::Void FormMain::pictureBox_Paint(System::Object^  sender, System::Windows::Forms::PaintEventArgs^  e) 
{ 
    e->Graphics->DrawImage(bmp, 0, 0); 
    if (checkBoxSpots->Checked)
    {
        DrawReticle(e->Graphics, HotSpot, HotPen, 4);
        DrawReticle(e->Graphics, ColdSpot, ColdPen, 4);
    }
}

void FormMain::DrawReticle(Drawing::Graphics ^g, Drawing::Point point, Drawing::Pen ^pen, int l)
{
    g->DrawLine(ContrastPen, point.X - l - 1, point.Y - 1, point.X + l - 1, point.Y - 1);
    g->DrawLine(ContrastPen, point.X - 1, point.Y - l - 1, point.X - 1, point.Y + l - 1);
    g->DrawLine(pen, point.X - l, point.Y, point.X + l, point.Y);
    g->DrawLine(pen, point.X, point.Y - l, point.X, point.Y + l);
}

void FormMain::StartPix(){
	String^ command = this->startPixHiddenTextBox->Text;
	array<String^>^ split = command->Split('"');
	String^ path = split[1];
	String^	flags = "";
	for (int i = 2; i < split->Length; i++){
		flags += split[i] + ' ';
	}
	
	if (!System::IO::File::Exists(path)){
		MessageBox::Show("Invalid Path");
	}

	else{
		startedPix = System::Diagnostics::Process::Start(path, flags);
	}
}


}
