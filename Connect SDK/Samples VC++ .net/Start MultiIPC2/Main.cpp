// HookAppl.cpp: Hauptprojektdatei.

#include "stdafx.h"
#include "FormMulti.h"

using namespace IPCSample;

[STAThreadAttribute]
int main(array<System::String ^> ^args)
{
	// Aktivieren visueller Effekte von Windows XP, bevor Steuerelemente erstellt werden
	Application::EnableVisualStyles();
	Application::SetCompatibleTextRenderingDefault(false); 

	// Hauptfenster erstellen und ausführen
	HWND hwnd = (args->Length) ? (HWND)System::Convert::ToUInt32(args[0]) : 0;
	Application::Run(gcnew FormMulti(hwnd));
	
	return 0;
}
 