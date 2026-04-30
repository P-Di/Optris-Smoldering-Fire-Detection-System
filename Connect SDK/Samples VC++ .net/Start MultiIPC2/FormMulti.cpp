#include "stdafx.h"
#include "FormMulti.h"

#define IMAGERCOUNT 2

namespace IPCSample {

FormMulti::FormMulti(HWND hostHandle)
{
	InitializeComponent();
	formsImager = gcnew array<FormImager^>(IMAGERCOUNT);
	ipc = gcnew IPC(IMAGERCOUNT);

	this->SuspendLayout();

	for(int i=0; i<IMAGERCOUNT; i++)
	{
		//formsImager[i] = gcnew FormImager(hostHandle, ipc, i, "Instance " + (i+1).ToString());
		formsImager[i] = gcnew FormImager(hostHandle, ipc, i, "");
		formsImager[i]->Location = Drawing::Point(i*300+5, 5);
		formsImager[i]->Name = "FormImager" + (i+1).ToString();
		formsImager[i]->Visible = true;
		formsImager[i]->Resize += gcnew System::EventHandler(this, &FormMulti::FormImager_OnResize);
		Controls->Add(formsImager[i]);
	}
	this->ResumeLayout();
	this->PerformLayout();
}


System::Void FormMulti::FormMulti_Load(System::Object^  sender, System::EventArgs^  e) 
{
	this->Text = this->Text + " (Rel. " + this->Version->ToString() + ")";
}

System::Void FormMulti::FormMulti_FormClosing(System::Object^  sender, System::Windows::Forms::FormClosingEventArgs^  e) 
{
	// we explicitely delete to release resources:
	for(int i=0; i<formsImager->Length; i++)
		delete formsImager[i];
	delete ipc;
}

System::Void FormMulti::FormImager_OnResize(System::Object^  sender, System::EventArgs^  e)
{
	this->SuspendLayout();

	int maxb = 0;
	for(int i=0; i<formsImager->Length; i++)
	{
		formsImager[i]->Location = Drawing::Point((!i) ? 5 : formsImager[i-1]->Right + 5, 5);
		maxb = max(maxb, formsImager[i]->Bottom);
	}
	ClientSize = Drawing::Size(formsImager[formsImager->Length-1]->Right + 5, maxb + 5);
	this->ResumeLayout();
	this->PerformLayout();
}


}
