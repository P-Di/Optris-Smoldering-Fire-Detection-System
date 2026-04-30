#include "Form1.h"

namespace StartListAreas
{
	Form1::Form1(void)
	{
		InitializeComponent();
		this->ipc = nullptr;
		this->ipc = gcnew IPC(1);			
		this->area_points = gcnew List<Point>();
		this->comboBox1->SelectedIndex = 3;

		if(FAILED(ipc->Init(0)))
		{
			MessageBox::Show("Could not connect. IPC service is running?", "failed", MessageBoxButtons::OK, MessageBoxIcon::Error);
			System::Environment::Exit(1);
		}	

		ipc->OnInitCompleted = gcnew IPC::delOnInitCompleted(this, &Form1::OnInitCompleted);
		ipc->SetCallback_OnInitCompleted(0, ipc->OnInitCompleted);

		if(FAILED(ipc->Run(0)))
		{
			MessageBox::Show("Could not connect. IPC service is running?","failed",MessageBoxButtons::OK,MessageBoxIcon::Error);
			System::Environment::Exit(1);
		}			
	}

	Form1::~Form1()
	{
		if (components)
		{
			delete components;
		}
	}


	HRESULT Form1::OnServerStopped(int reason)
	{
		if(ipc) ipc->Release(0);
		return 0;
	}

	HRESULT Form1::OnInitCompleted(void)
	{
		this->refreshList();
		return S_OK;
	}

	bool Form1::refreshList()
	{
		if(this->ipc == nullptr) return false;

		this->listBox1->Items->Clear();
		wchar_t buffer[512]; int len = 0;
		String ^mString;		
		USHORT cnt = this->ipc->GetMeasureAreaCount(0);
		for(USHORT i = 0; i < cnt;i++)
		{
			this->ipc->GetMeasureAreaName(0,ULONG(i),buffer, &len, 512);
			mString = gcnew String(buffer);
			this->listBox1->Items->Add(mString->Length != 0 ? mString : "<not named>");
		}
		return true;
	}

	System::Void Form1::button_add_Click(System::Object^  sender, System::EventArgs^  e)
	{
		MeasureArea ma;
		ma.BindToTempProfile = false;		
		ma.IsColdSpot = this->checkBox_cs->Checked;
		ma.IsHotSpot = this->checkBox_hs->Checked;
		ma.distMin= 20.0;
		ma.distMax= 40.0;
		ma.Emissivity = 1.0f;
		ma.UseEmissivity = false;
		ma.ShowInDigDispGroup = false;		
		ma.Location.x = LONG(Decimal::ToInt32(this->numericUpDown1->Value));
		ma.Location.y = LONG(Decimal::ToInt32(this->numericUpDown2->Value));
		ma.Mode = mamAvg;
		ma.Shape = this->comboBox1->SelectedIndex > -1 ? (MeasureAreaShape)this->comboBox1->SelectedIndex : masOff;
		ma.Size.cx = LONG(Decimal::ToInt32(this->numericUpDown3->Value));
		ma.Size.cy = LONG(Decimal::ToInt32(this->numericUpDown4->Value));


		USHORT mCount = ipc->GetMeasureAreaCount(0);

		//add new measure area ... 
		this->ipc->SetMeasureArea(0,0,&ma,true);
		if(this->textBox1->Text->Length != 0)
		{	wchar_t name[256];
			pin_ptr<const wchar_t> wch = PtrToStringChars(this->textBox1->Text); 
			wcscpy_s(name, 256, wch); 
			this->ipc->SetMeasureAreaName(0,mCount,name);
		}

		POINT p;

		if(ma.Shape > 5)
			for(int i = 0; i < this->area_points->Count;i++)
			{
				p.x = LONG(this->area_points[i].X);
				p.y = LONG(this->area_points[i].Y);
				this->ipc->AddMeasureAreaPoint(0, mCount, p);
			}

		this->refreshList();
	}

	System::Void Form1::button_remove_Click(System::Object^  sender, System::EventArgs^  e)
	{
		if(this->ipc == nullptr) return;
		if(!this->ipc->RemoveMeasureArea(0,this->listBox1->SelectedIndex))
			this->refreshList();
	}

	System::Void Form1::button_addp_Click(System::Object^  sender, System::EventArgs^  e)
	{
		if(this->ipc == nullptr) return;
		this->area_points->Add(Point(Decimal::ToInt32(this->numericUpDown1->Value),Decimal::ToInt32(this->numericUpDown2->Value)));
		this->listBox_points->Items->Add(String::Format("P({0},{1})",this->numericUpDown1->Value,this->numericUpDown2->Value));
		this->button_add->Enabled = this->area_points->Count >= 3;
	}

	System::Void Form1::button_remp_Click(System::Object^  sender, System::EventArgs^  e)
	{
		if(this->ipc == nullptr || 
			this->area_points->Count <= this->listBox_points->SelectedIndex ||
			this->listBox_points->SelectedIndex < 0) return;				

		this->area_points->RemoveAt(this->listBox_points->SelectedIndex);
		this->listBox_points->Items->RemoveAt(this->listBox_points->SelectedIndex);
		this->button_add->Enabled = this->area_points->Count >= 3;
	}

	System::Void Form1::checkBox_cs_CheckedChanged(System::Object^  sender, System::EventArgs^  e)
	{
		if(this->checkBox_hs->Checked && this->checkBox_cs->Checked)		
			this->checkBox_hs->Checked = false;
		
	}

	System::Void Form1::checkBox_hs_CheckedChanged(System::Object^  sender, System::EventArgs^  e)
	{
		if(this->checkBox_hs->Checked && this->checkBox_cs->Checked)		
			this->checkBox_cs->Checked = false;
	}

	System::Void Form1::comboBox1_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e)
	{
		switch(this->comboBox1->SelectedIndex)
		{
			case 0:
				this->button_add->Enabled = false;
				this->button_addp->Enabled = false;
				this->button_remp->Enabled = false;
				this->button_add->Enabled = false;
				this->numericUpDown1->Enabled = false;
				this->numericUpDown2->Enabled = false;
				this->numericUpDown3->Enabled = false;
				this->numericUpDown4->Enabled = false;				
				this->checkBox_cs->Enabled = true;
				this->checkBox_hs->Enabled = true;
				break;
			case 1:
			case 2:				
			case 3:
				this->button_addp->Enabled = false;
				this->button_remp->Enabled = false;
				this->button_add->Enabled = true;
				this->numericUpDown1->Enabled = true;
				this->numericUpDown2->Enabled = true;
				this->numericUpDown3->Enabled = false;
				this->numericUpDown4->Enabled = false;				
				this->checkBox_cs->Enabled = true;
				this->checkBox_hs->Enabled = true;
				break;
			case 4:
			case 5:
				this->button_addp->Enabled = false;
				this->button_remp->Enabled = false;
				this->numericUpDown1->Enabled = true;
				this->numericUpDown2->Enabled = true;
				this->numericUpDown3->Enabled = true;
				this->numericUpDown4->Enabled = true;				
				this->checkBox_cs->Enabled = true;
				this->checkBox_hs->Enabled = true;
				this->button_add->Enabled = true;
				break;
			case 6:				
			case 7:
				this->button_addp->Enabled = true;
				this->button_remp->Enabled = true;
				this->checkBox_cs->Enabled = false;
				this->checkBox_hs->Enabled = false;
				this->numericUpDown3->Enabled = false;
				this->numericUpDown4->Enabled = false;				
				this->button_add->Enabled = this->listBox_points->Items->Count >= 3;				
				break;
		}
	}

	System::Void Form1::button_log_Click(System::Object^  sender, System::EventArgs^  e)
	{
		//set logging for groups: init, device setup, device data, devince info
		this->ipc->SetLogging(LOGGRP_INIT | LOGGRP_DEVSETUP | LOGGRP_DEVDATA | LOGGRP_DEVINFO);
		this->button_log->Enabled = false;
	}


}