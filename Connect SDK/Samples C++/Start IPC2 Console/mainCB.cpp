#include "stdafx.h"
#include <windows.h>
#include "ImagerIPC2.h"

HANDLE hStdout;
COORD CursorPosition = {0, 0};
bool frameInitialized = false, Connected = false, Stopped = false;
short FrameWidth, FrameHeight, FrameDepth;
int FrameSize;
const char s[] = {"open         \0       closed\0 <-opening   \0   closing-> \0             \0"};

HRESULT WINAPI OnServerStopped(int reason) { Stopped = true; return 0; }
HRESULT WINAPI OnInitCompleted(void) { Connected = true; return 0; }
HRESULT WINAPI OnFrameInit(int frameWidth, int frameHeight, int frameDepth) 
{
	FrameWidth = frameWidth;
	FrameHeight = frameHeight;
	FrameDepth = frameDepth;
	FrameSize = FrameWidth * FrameHeight;
	frameInitialized = true;
	printf("------------------------------------------------------\nWidth,Height: (%d,%d)\n\n\n\n\n", FrameWidth, FrameHeight);
	printf("------------------------------------------------------\nHit ESC to exit...\n");
	CursorPosition.Y += 2;
	return 0;
}

HRESULT WINAPI OnNewFrame(void* pBuffer, FrameMetadata2 *pMetadata) 
{
	if(frameInitialized && Connected)
	{
		SetConsoleCursorPosition(hStdout, CursorPosition);
		printf("Frame counter HW/SW: %d/%d\n", pMetadata->CounterHW, pMetadata->Counter);
        printf("PIF");
        for (int i = 0; i < pMetadata->PIFnDI; i++)
            printf("  DI%d:%d", i + 1, (pMetadata->PIFDI >> i) & 1);
        for (int i = 0; i < pMetadata->PIFnAI; i++)
            printf("  AI%d:%d", i + 1, pMetadata->PIFAI[i]);
        printf("\n");
		printf("Target-Temp: %3.1f\370C\n", (float)GetTempTarget(0));
		printf("Flag: |%s|      <-- Hit SPACE to renew flag\n", &s[min(int(pMetadata->FlagState),4)*14]);
	}
	return 0;
}

int main(int argc, char* argv[]) 
{
	if (InitImagerIPC(0) < 0) 
	{
		printf("\nInit failed! Press Enter to exit...");
		getchar();
		return -1;
	}
	SetCallback_OnServerStopped(0, &OnServerStopped); 
	SetCallback_OnFrameInit(0, &OnFrameInit);
	SetCallback_OnNewFrameEx2(0, &OnNewFrame);
	SetCallback_OnInitCompleted(0, &OnInitCompleted);

	if (RunImagerIPC(0) < 0)
	{
		ReleaseImagerIPC(0);
		printf("\nRun failed! Press Enter to exit...");
		getchar();
		return -1;
	}

    hStdout = GetStdHandle(STD_OUTPUT_HANDLE); 
	CONSOLE_SCREEN_BUFFER_INFO CSBI;
	GetConsoleScreenBufferInfo(hStdout, &CSBI); // memorize last cursor pos to repeat output 
	CursorPosition = CSBI.dwCursorPosition;   

	while(!GetAsyncKeyState(VK_ESCAPE) && !Stopped) // loop until ESC is pressed or stopped by server
	{
		ImagerIPCProcessMessages(0);
		if(GetAsyncKeyState(VK_SPACE)) // renew flag if SPACE is pressed
 			RenewFlag(0);
	}
	printf("\n                    "); // clear last line
	if(Stopped)
	{
		printf("\nIPC stopped by server! Press Enter to exit...");
		getchar();
	}

	ReleaseImagerIPC(0);
	return 0;
}