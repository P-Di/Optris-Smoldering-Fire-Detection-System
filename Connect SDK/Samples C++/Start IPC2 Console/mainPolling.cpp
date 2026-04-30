#include "stdafx.h"
#include <windows.h>
#include "ImagerIPC2.h"

int main(int argc, char* argv[]) 
{
	HANDLE hStdout;
	bool frameInitialized = false, Connected = false, Stopped = false;
    int FrameWidth, FrameHeight, FrameDepth, FrameSize, MetadataSize;
	void *FrameBuffer = NULL;
	FrameMetadata2 *Metadata = NULL;
	const char s[] = {"open         \0       closed\0 <-opening   \0   closing-> \0             \0"};

	if (InitImagerIPC(0) < 0) {	printf("\nInit failed! Press Enter to exit..."); getchar();	return -1; }
	if (RunImagerIPC(0) < 0)  {	printf("\nRun failed! Press Enter to exit...");	getchar(); ReleaseImagerIPC(0);	return -1; }

    hStdout = GetStdHandle(STD_OUTPUT_HANDLE); 
	CONSOLE_SCREEN_BUFFER_INFO CSBI;
	GetConsoleScreenBufferInfo(hStdout, &CSBI); // memorize last cursor pos to repeat output 

	while(!GetAsyncKeyState(VK_ESCAPE) && !Stopped) // loop until ESC is pressed or stopped by server
	{
		WORD State = GetIPCState(0, true);
		if(State & IPC_EVENT_SERVER_STOPPED) Stopped = true;
		if(!Connected && (State & IPC_EVENT_INIT_COMPLETED)) Connected = true;
		if((State & IPC_EVENT_FRAME_INIT) && (SUCCEEDED(GetFrameConfig(0, &FrameWidth, &FrameHeight, &FrameDepth))))
		{
			frameInitialized = true;
			printf("------------------------------------------------------\nWidth,Height: (%d,%d)\n\n\n\n\n", FrameWidth, FrameHeight);
			printf("------------------------------------------------------\nHit ESC to exit...\n");
			CSBI.dwCursorPosition.Y += 2;
			FrameSize = FrameWidth * FrameHeight * FrameDepth;
			FrameBuffer = new char[FrameSize];
            if (SUCCEEDED(GetFrameMetadataSize(0, &MetadataSize)))
                Metadata = (FrameMetadata2 *)(new char[MetadataSize]);
		}
        if (Connected && frameInitialized && (FrameBuffer != NULL) && (Metadata != NULL) && GetFrameQueue(0))
			if(SUCCEEDED(GetFrame2(0, 0, FrameBuffer, FrameSize, Metadata, MetadataSize)))
			{
				SetConsoleCursorPosition(hStdout, CSBI.dwCursorPosition);
				printf("Frame counter HW/SW: %d/%d\n", Metadata->CounterHW, Metadata->Counter);
                printf("PIF");
                for (int i = 0; i < Metadata->PIFnDI; i++)
                    printf("  DI%d:%d", i + 1, (Metadata->PIFDI >> i) & 1);
                for (int i = 0; i < Metadata->PIFnAI; i++)
                    printf("  AI%d:%d", i + 1, Metadata->PIFAI[i]);
                printf("\n");
                printf("Target-Temp: %3.1f\370C\n", (float)GetTempTarget(0));
				printf("Flag: |%s|      <-- Hit SPACE to renew flag\n", &s[min(int(Metadata->FlagState),4)*14]);
			}
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

	if(FrameBuffer) delete [] FrameBuffer;
    if (Metadata) delete[] Metadata;
	ReleaseImagerIPC(0);
	return 0;
}
