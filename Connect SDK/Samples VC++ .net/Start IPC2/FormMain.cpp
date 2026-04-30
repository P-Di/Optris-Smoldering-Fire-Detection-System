#include "stdafx.h"
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
    bmp = gcnew Bitmap(100, 100, System::Drawing::Imaging::PixelFormat::Format24bppRgb);
	ipc = nullptr;
	ipcInitialized = false;
	frameInitialized = false;
	Connected = false;
	Painted = false;
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

			ipc->OnInitCompleted = gcnew IPC::delOnInitCompleted(this, &FormMain::OnInitCompleted );
			ipc->SetCallback_OnInitCompleted(0, ipc->OnInitCompleted);
#endif
			hr = ipc->Run(0);
			ipcInitialized = SUCCEEDED(hr);
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

void FormMain::GetBitmap(Bitmap^ Bmp, array<short>^values)
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


void FormMain::GetBitmap_Limits(array<short>^Values, short *min, short *max)
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


void FormMain::Application_Idle(Object ^sender, EventArgs ^e)
{
#ifdef POLLING 
	if(Connected && frameInitialized)
	{
        int Size = FrameWidth * FrameHeight * FrameDepth;
        void *Buffer = new char[Size];
        FrameMetadata2 *Metadata = (FrameMetadata2*)(new char[MetadataSize]);
        if (ipc->GetFrameQueue(0))
        if (SUCCEEDED(ipc->GetFrame2(0, 0, Buffer, Size, Metadata, MetadataSize)))
            OnNewFrameEx(Buffer, Metadata);
        delete[] Buffer;
        delete[] Metadata;
    }
#endif
}

HRESULT FormMain::MainTimer100ms(void)
{
	Painted = false;
#ifdef POLLING
	if(ipcInitialized)
	{
		WORD State = ipc->GetIPCState(0, true);
		if(State & IPC_EVENT_SERVER_STOPPED)
			OnServerStopped(0);
		if(!Connected && (State & IPC_EVENT_INIT_COMPLETED))
			OnInitCompleted();
		if(State & IPC_EVENT_FRAME_INIT)
		{
            int frameWidth, frameHeight, frameDepth, frameMetadataSize;
            if (SUCCEEDED(ipc->GetFrameConfig(0, &frameWidth, &frameHeight, &frameDepth)))
            {
                Init(frameWidth, frameHeight, frameDepth);
                if (SUCCEEDED(ipc->GetFrameMetadataSize(0, &frameMetadataSize)))
                    MetadataSize = frameMetadataSize;
            }
        }
	}
#endif
    return S_OK;
}

HRESULT FormMain::MainTimer1000ms(void)
{
	if(Connected)
		labelTempTarget->Text	= String::Format("Target-Temp: {0:##0.0}°C", ipc->GetTempTarget(0));
	return S_OK;
}

void FormMain::RenewFlag(void)
{
	buttonFlagRenew->Text = String::Format("Renew ({0})", ipc->RenewFlag(0) ? "Success" : "Failed");
}

void FormMain::Init(int frameWidth, int frameHeight, int frameDepth)
{
	FrameWidth = frameWidth;
	FrameHeight = frameHeight;
	FrameSize = FrameWidth * FrameHeight;
	FrameDepth = frameDepth;
	timer1->Enabled = true;
	bmp = gcnew Bitmap( FrameWidth , FrameHeight , System::Drawing::Imaging::PixelFormat::Format24bppRgb );
	System::Drawing::Rectangle rect = System::Drawing::Rectangle(0, 0, bmp->Width, bmp->Height);
	Imaging::BitmapData^ bmpData = bmp->LockBits( rect, Imaging::ImageLockMode::ReadWrite, bmp->PixelFormat );
	int stride = bmpData->Stride;
	bmp->UnlockBits( bmpData );
	rgbValues = gcnew array<Byte>(stride * FrameHeight);
	Values = gcnew array<short>(FrameSize);
	pictureBox->Size = Drawing::Size(FrameWidth, FrameHeight);
	UpdateSize();
	frameInitialized = true;
}

void FormMain::UpdateSize(void)
{
	this->Size = Drawing::Size(pictureBox->Right + 20, max(buttonFlagRenew->Bottom, pictureBox->Bottom) + 50);
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
	return NewFrame((short*)pBuffer, pMetadata->Counter);
}

HRESULT FormMain::OnNewFrame(char * pBuffer, int frameCounter)
{
	return NewFrame((short*)pBuffer, frameCounter);
}

HRESULT FormMain::NewFrame(short *ImgBuf, int frameCounter)
{
	for ( int x = 0; x < FrameSize; x++ ) 
		Values[x] = ImgBuf[x];
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
	label1->Text = "Connected with #" + ipc->GetSerialNumber(0);
	Colors = (TIPCMode(ipc->GetIPCMode(0)) == ipcColors);
	Connected = true;
	UpdateSize();
	return S_OK;
}

}
