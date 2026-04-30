#include "stdafx.h"
#include <stdio.h>
#include <unknwn.h>
#include <windows.h>
#include <math.h>
#include <gdiplus.h>
#include "mainWin32.h"
#include "ImagerIPC2.h"

using namespace Gdiplus;

//#define POLLING
#define MAXLEN 1024
#define SAFE_DELETE(x) { if (x) delete x; x = NULL; }				

// Global Vars:
HINSTANCE hInst;							
HWND ghWnd = NULL;
const TCHAR szWindowClass[] = TEXT("StartIPCWin32");
TCHAR labelConnected[MAXLEN];
TCHAR labelFrameCounter[MAXLEN];
TCHAR labelPIF[MAXLEN];
TCHAR labelFlag[MAXLEN];
TCHAR labelTarget[MAXLEN];

bool ipcInitialized = false;
bool frameInitialized = false;
bool Connected = false;
bool Colors = false;
bool Painted = false;
short FrameWidth, FrameHeight, FrameDepth;
int FrameSize, mdSize = 0;

ULONG_PTR gdiplusToken;
Font *font;
SolidBrush *WhiteBrush, *BlackBrush;
Bitmap *Bmp = NULL;
BitmapData *bmpData = NULL; 
void *FrameBuffer = NULL;

// function prototypes:
ATOM MyRegisterClass(HINSTANCE hInstance);
BOOL	InitInstance(HINSTANCE, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void AppInit(HWND hWnd);
void AppExit(HWND hWnd);
void InitIPC(void);
void ReleaseIPC(void);
void Idle(void);
void HandleEvents(void);
void Init(int frameWidth, int frameHeight, int frameDepth);
void ResizeWindow(int w, int h);

HRESULT WINAPI OnServerStopped(int reason);
HRESULT WINAPI OnInitCompleted(void);
HRESULT WINAPI OnFrameInit(int frameWidth, int frameHeight, int frameDepth);
HRESULT WINAPI OnNewFrame(void * pBuffer, FrameMetadata2 *pMetadata);

void GetBitmap_Limits(short* buf, int FrameSize, short *min, short *max, bool Sigma);
BYTE clip(int val);

int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	MSG msg;
	MyRegisterClass(hInstance);

	if (!InitInstance (hInstance, nCmdShow))
		return FALSE;

	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);

		while (!PeekMessage( &msg, NULL, 0, 0, PM_NOREMOVE))
			Idle();
	}
	return (int) msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize			= sizeof(WNDCLASSEX);
	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_STARTIPC2));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; 

   ghWnd = CreateWindow(szWindowClass, szWindowClass, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, 320, 300, NULL, NULL, hInstance, NULL);

   if (!ghWnd)
      return FALSE;

   ShowWindow(ghWnd, nCmdShow);
   UpdateWindow(ghWnd);

   return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_CREATE:
		AppInit(hWnd);
		break;
	case WM_TIMER:
		switch(wParam)
		{
		case 1: 
			Painted = false;
			HandleEvents();
			break;
		case 2: 
			InitIPC();
			break;
		case 3: 
			if(Connected)
			{
				_stprintf_s(labelTarget, MAXLEN, TEXT("Target-Temp: %3.1f°C"), (float)GetTempTarget(0));
				InvalidateRect(hWnd, NULL, FALSE); 
			}
			break;
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		if(Bmp)
		{
			Graphics graphics(hWnd, false);
			float y = 0;
			RECT rc;
			GetClientRect(hWnd, &rc);
			graphics.FillRectangle(WhiteBrush, rc.left, rc.top, 200, rc.bottom-rc.top);
			graphics.DrawString(labelConnected, -1, font, PointF(0, y+=20), BlackBrush);
			graphics.DrawString(labelTarget, -1, font, PointF(0, y+=20), BlackBrush);
			graphics.DrawString(labelFrameCounter, -1, font, PointF(0, y+=20), BlackBrush);
			graphics.DrawString(labelPIF, -1, font, PointF(0, y+=20), BlackBrush);
			graphics.DrawString(labelFlag, -1, font, PointF(0, y+=20), BlackBrush);
			graphics.DrawImage(Bmp, 200, 0);
		}
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		AppExit(hWnd);
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

void AppInit(HWND hWnd)
{
	SetTimer(hWnd, 1, 100, NULL); // Timer for frame processing
	SetTimer(hWnd, 2, 200, NULL); // timer for connection
	SetTimer(hWnd, 3, 500, NULL); // timer for temp target

	::SetWindowText(hWnd, TEXT("Start IPC Win32 (Rel. 2.1.2005.0)"));

	GdiplusStartupInput gdiplusStartupInput;
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

	FontFamily fontFamily(TEXT("Times New Roman"));
	font = new Font(&fontFamily, 12, FontStyleRegular, UnitPixel);
	BlackBrush = new SolidBrush (Color(255, 0, 0, 0));
	WhiteBrush = new SolidBrush (Color(255, 255, 255, 255));

	Init(160, 120, 2);
}

void AppExit(HWND hWnd)
{
	KillTimer(hWnd, 1);
	KillTimer(hWnd, 2);
	KillTimer(hWnd, 3);
	ReleaseIPC();
	SAFE_DELETE(bmpData);
	SAFE_DELETE(Bmp);
	SAFE_DELETE(FrameBuffer);
	GdiplusShutdown(gdiplusToken);
}

void InitIPC(void) 
{
	HRESULT hr;
	 if(!ipcInitialized)
	 {
		hr = InitImagerIPC(0);
		
		if(FAILED(hr))
		{
			ipcInitialized = frameInitialized = false;
		}
		else
		{
#ifndef POLLING 
			SetCallback_OnServerStopped(0, OnServerStopped);
			SetCallback_OnFrameInit(0, OnFrameInit);
			SetCallback_OnNewFrameEx2(0, OnNewFrame);
			SetCallback_OnInitCompleted(0, OnInitCompleted);
#endif
			hr = RunImagerIPC(0);
			ipcInitialized = SUCCEEDED(hr);			
		}
		_stprintf_s(labelConnected, MAXLEN, hr ? TEXT("NOT CONNECTED") : TEXT("OK"));
		InvalidateRect(ghWnd, NULL, FALSE); 
	 }
}

void ReleaseIPC(void) 
{
	Connected = false;
	if(ipcInitialized)
	{
		ReleaseImagerIPC(0);
		ipcInitialized = false;
	}
}

void Idle(void) 
{
#ifdef POLLING 
	if(Connected && frameInitialized && (FrameBuffer != NULL))
	{
		FrameMetadata2 Metadata;		
		if(GetFrameQueue(0))			
			if(SUCCEEDED(GetFrame2(0, 0, FrameBuffer, FrameSize * FrameDepth, &Metadata, mdSize)))
				OnNewFrame(FrameBuffer, &Metadata);
	}
#endif
}

void HandleEvents(void) 
{
#ifdef POLLING 
	if(ipcInitialized)
	{
		WORD State = GetIPCState(0, true);
		if(State & IPC_EVENT_SERVER_STOPPED)
			OnServerStopped(0);
		if(!Connected && (State & IPC_EVENT_INIT_COMPLETED))
			OnInitCompleted();
		if(State & IPC_EVENT_FRAME_INIT)
		{
			int frameWidth, frameHeight, frameDepth;
			if(SUCCEEDED(GetFrameConfig(0, &frameWidth, &frameHeight, &frameDepth)))
				Init(frameWidth, frameHeight, frameDepth);
		}
	}
#endif
}

void ResizeWindow(int w, int h)
{
    RECT rcW, rcC;
    int xExtra, yExtra;
    int cyBorder = GetSystemMetrics(SM_CYBORDER);

    GetWindowRect(ghWnd, &rcW);
    GetClientRect(ghWnd, &rcC);
    xExtra = rcW.right - rcW.left - rcC.right;
    yExtra = rcW.bottom - rcW.top - rcC.bottom + cyBorder;

    rcC.right = w+200;
    rcC.bottom = h;
    SetWindowPos(ghWnd, NULL, 0, 0, rcC.right + xExtra, rcC.bottom + yExtra, SWP_NOZORDER | SWP_NOMOVE);
}

void Init(int frameWidth, int frameHeight, int frameDepth)
{
	FrameWidth = frameWidth;
	FrameHeight = frameHeight;
	FrameSize = FrameWidth * FrameHeight;
	FrameDepth = frameDepth;
	SAFE_DELETE(Bmp);
	Bmp = new Bitmap(FrameWidth, FrameHeight, PixelFormat32bppRGB);
	SAFE_DELETE(bmpData);
	bmpData = new BitmapData; 
	ResizeWindow(FrameWidth, FrameHeight);
	frameInitialized = true;
#ifdef POLLING 
	SAFE_DELETE(FrameBuffer);
	int Size = FrameWidth * FrameHeight * FrameDepth;
	FrameBuffer = new char[FrameSize * FrameDepth];
#endif
}

HRESULT WINAPI OnServerStopped(int reason)
{
	ReleaseIPC();
	return 0;
}

HRESULT WINAPI OnInitCompleted(void)
{
	_stprintf_s(labelConnected, MAXLEN, TEXT("Connected with #%d"), GetSerialNumber(0));
	InvalidateRect(ghWnd, NULL, FALSE); 
	Colors = (TIPCMode(GetIPCMode(0)) == ipcColors);
	GetFrameMetadataSize(0, &mdSize);
	Connected = true;
	return S_OK;
}

HRESULT WINAPI OnFrameInit(int frameWidth, int frameHeight, int frameDepth)
{
	Init(frameWidth, frameHeight, frameDepth);
	return 0;
} 

HRESULT WINAPI OnNewFrame(void * pBuffer, FrameMetadata2 *pMetadata)
{
	_stprintf_s(labelFrameCounter, MAXLEN, TEXT("Frame counter HW/SW: %d/%d"), pMetadata->CounterHW, pMetadata->Counter);
	_stprintf_s(labelPIF, MAXLEN, TEXT("PIF   DI:%d     AI1:%d     AI2:%d"), (pMetadata->PIFDI & 0x01) == 0, pMetadata->PIFnAI > 0 ? pMetadata->PIFAI[0] : 0, pMetadata->PIFnAI > 1 ? pMetadata->PIFAI[1] : 0);

	_tcscpy_s(labelFlag, MAXLEN, TEXT("Flag: "));
	switch(pMetadata->FlagState)
	{
	case fsFlagOpen: _tcscat_s(labelFlag, MAXLEN, TEXT("open")); break;
	case fsFlagClose: _tcscat_s(labelFlag, MAXLEN, TEXT("closed")); break;
	case fsFlagOpening: _tcscat_s(labelFlag, MAXLEN, TEXT("opening")); break;
	case fsFlagClosing: _tcscat_s(labelFlag, MAXLEN, TEXT("closing")); break;
	}

	if(!Painted)
	{
		int W = Bmp->GetWidth();
		int H = Bmp->GetHeight();

		int stride_diff;
		Rect rect(0, 0, FrameWidth, FrameHeight);
		short* buf = (short*)pBuffer;
		
		try // Lock the bitmap's bits: 
		{
			if(Ok == Bmp->LockBits(&rect, ImageLockModeWrite | ImageLockModeRead, PixelFormat32bppRGB, bmpData))
			{
				stride_diff = bmpData->Stride - FrameWidth*4;

				// Get the address of the first line.
				char *ptr = (char*)bmpData->Scan0;
				if(ptr)
				{
					if(Colors)
					{
						for ( int dst=0, src=0, y=0; y < FrameHeight; y++, dst += stride_diff) 
							for ( int x=0; x < FrameWidth; x++, src++, dst+=4 ) 
							{
								int C = LOBYTE(buf[src              ]) - 16;
								int D = HIBYTE(buf[src - (src%2)    ]) - 128;
								int E = HIBYTE(buf[src - (src%2) + 1]) - 128;
								ptr[ dst   ] = clip(( 298 * C + 516 * D           + 128) >> 8);
								ptr[ dst+1 ] = clip(( 298 * C - 100 * D - 208 * E + 128) >> 8);
								ptr[ dst+2 ] = clip(( 298 * C           + 409 * E + 128) >> 8);
							}
					}
					else
					{
						short mn, mx;
						GetBitmap_Limits(buf, FrameWidth*FrameHeight, &mn, &mx, true);
						double Fact = 255.0 / (mx - mn);

						for ( int dst=0, src=0, y=0; y < FrameHeight; y++, dst += stride_diff) 
							for ( int x=0; x < FrameWidth; x++, src++, dst+=4 ) 
								ptr[dst] = 
								ptr[dst+1] = 
								ptr[dst+2] = (char)min(max((int)(Fact * (buf[src] - mn)), 0), 255);
					}
				}
			}
		}
		catch(char*)
		{
		}
		// Unlock the bits.
		Bmp->UnlockBits( bmpData );

		InvalidateRect(ghWnd, NULL, FALSE); 
		Painted = true;
	}
	return 0;
}

void GetBitmap_Limits(short* buf, int FrameSize, short *min, short *max, bool Sigma)
{
	int y;
	double Sum, Mean, Variance;
	if(!buf) return;

	if(!Sigma)
	{
		*min = SHRT_MAX;
		*max = SHRT_MIN;
		for (y=0; y < FrameSize; y++ ) 
		{
			*min = min(buf[y], *min);
			*max = max(buf[y], *max);
		}
		return;
	}

	Sum = 0;
	for (y=0; y < FrameSize; y++ ) 
		Sum += buf[y];
	Mean = (double)Sum / FrameSize;
	Sum = 0;
	for (y=0; y < FrameSize; y++ ) 
		Sum += (Mean - buf[y]) * (Mean - buf[y]);
	Variance = Sum / FrameSize;
	Variance = sqrt(Variance);
	Variance *= 3;  // 3 Sigma
	*min = short(Mean - Variance);
	*max = short(Mean + Variance);
	unsigned short d = *max - *min;
	if(d < 40)
	{
		d >>= 1;
		*min = *min - d;
		*max = *max + d;
	}
}

BYTE clip(int val) 
{ 
	return (val <= 255) ? ((val > 0) ? val : 0) : 255; 
};

