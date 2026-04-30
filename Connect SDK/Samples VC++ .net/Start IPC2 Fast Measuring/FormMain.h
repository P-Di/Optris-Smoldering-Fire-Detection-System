#pragma once

#include "IPCSample.h"
#include "IPC2.h"


namespace IPCSample 
{
	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	public ref class FormMain : public System::Windows::Forms::Form
	{
	public:	FormMain(HWND hostHandle);


	private: System::Windows::Forms::TextBox^  textBoxInstanceName;
	private: System::Windows::Forms::Label^  labelInstanceName;
	private: System::Windows::Forms::Button^  buttonFlagRenew;
	private: System::Windows::Forms::Label^  labelFrameCounter;
	private: System::Windows::Forms::Label^  labelFramerate;
	private: System::Windows::Forms::Label^  labelFlag1;
	private: System::Windows::Forms::Label^  labelFlag;
	private: System::Windows::Forms::Label^  label1;
	private: System::Windows::Forms::Timer^  timer1;
	private: System::Windows::Forms::PictureBox^  pictureBox;
	private: System::Windows::Forms::Timer^  timer2;
	private: System::Windows::Forms::Label^  labelFrames;
	private: System::Windows::Forms::GroupBox^  groupBoxAreas;
	private: System::Windows::Forms::Label^  labelAreas;
	private: System::Windows::Forms::Label^  label2;
	private: System::Windows::Forms::Label^  labelMode;
	private: System::Windows::Forms::RadioButton^  radioButtonModeMA;
	private: System::Windows::Forms::RadioButton^  radioButtonModeFR;
	private: System::Windows::Forms::RadioButton^  radioButtonModeMAFR;
	private: System::Windows::Forms::Label^  labelFrameCounterFR;
	private: System::Windows::Forms::Label^  labelFramerateFR;
	private: System::Windows::Forms::Label^  labelFrameCounterMA;
	private: System::Windows::Forms::Label^  labelFramerateMA;

	private: System::ComponentModel::IContainer^  components;

	property System::Version^ Version
	{
		System::Version^ get()
		{
			Reflection::Assembly^ assembly = Reflection::Assembly::GetExecutingAssembly();
			return assembly->GetName()->Version;
		}
	}

	protected:
		~FormMain();
		void GetBitmap(Bitmap^ Bmp, array<short>^values);
		void GetBitmap_Limits(array<short>^Values, short *min, short *max);

        short FrameWidth, FrameHeight, FrameDepth, MetadataSize;
		int FrameSize;
		IPC^ ipc;
		bool ipcInitialized, frameInitialized;
		Bitmap^ bmp;
		bool Connected;
		bool Colors;
		int AreaCount;
		int AreaBufferCounter;
		int FrameCounter;
		DateTime LastTime;

		array<Byte>^rgbValues;
		array<short>^Values;

		int MainTimerDivider;
		bool Painted, Updated;

	private:

#pragma region Windows Form Designer generated code
		void InitializeComponent(void)
		{
			this->components = (gcnew System::ComponentModel::Container());
			System::ComponentModel::ComponentResourceManager^  resources = (gcnew System::ComponentModel::ComponentResourceManager(FormMain::typeid));
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->timer1 = (gcnew System::Windows::Forms::Timer(this->components));
			this->pictureBox = (gcnew System::Windows::Forms::PictureBox());
			this->timer2 = (gcnew System::Windows::Forms::Timer(this->components));
			this->labelFrames = (gcnew System::Windows::Forms::Label());
			this->buttonFlagRenew = (gcnew System::Windows::Forms::Button());
			this->textBoxInstanceName = (gcnew System::Windows::Forms::TextBox());
			this->labelInstanceName = (gcnew System::Windows::Forms::Label());
			this->labelFrameCounter = (gcnew System::Windows::Forms::Label());
			this->labelFramerate = (gcnew System::Windows::Forms::Label());
			this->labelFlag1 = (gcnew System::Windows::Forms::Label());
			this->labelFlag = (gcnew System::Windows::Forms::Label());
			this->groupBoxAreas = (gcnew System::Windows::Forms::GroupBox());
			this->labelAreas = (gcnew System::Windows::Forms::Label());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->labelMode = (gcnew System::Windows::Forms::Label());
			this->radioButtonModeMA = (gcnew System::Windows::Forms::RadioButton());
			this->radioButtonModeFR = (gcnew System::Windows::Forms::RadioButton());
			this->radioButtonModeMAFR = (gcnew System::Windows::Forms::RadioButton());
			this->labelFrameCounterFR = (gcnew System::Windows::Forms::Label());
			this->labelFramerateFR = (gcnew System::Windows::Forms::Label());
			this->labelFrameCounterMA = (gcnew System::Windows::Forms::Label());
			this->labelFramerateMA = (gcnew System::Windows::Forms::Label());
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox))->BeginInit();
			this->SuspendLayout();
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Location = System::Drawing::Point(12, 91);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(16, 13);
			this->label1->TabIndex = 0;
			this->label1->Text = L"---";
			// 
			// timer1
			// 
			this->timer1->Tick += gcnew System::EventHandler(this, &FormMain::timer1_Tick);
			// 
			// pictureBox
			// 
			this->pictureBox->Location = System::Drawing::Point(281, 20);
			this->pictureBox->Name = L"pictureBox";
			this->pictureBox->Size = System::Drawing::Size(194, 121);
			this->pictureBox->TabIndex = 8;
			this->pictureBox->TabStop = false;
			this->pictureBox->Paint += gcnew System::Windows::Forms::PaintEventHandler(this, &FormMain::pictureBox_Paint);
			// 
			// timer2
			// 
			this->timer2->Enabled = true;
			this->timer2->Interval = 200;
			this->timer2->Tick += gcnew System::EventHandler(this, &FormMain::timer2_Tick);
			// 
			// labelFrames
			// 
			this->labelFrames->AutoSize = true;
			this->labelFrames->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->labelFrames->Location = System::Drawing::Point(117, 109);
			this->labelFrames->Name = L"labelFrames";
			this->labelFrames->Size = System::Drawing::Size(41, 13);
			this->labelFrames->TabIndex = 32;
			this->labelFrames->Text = L"Frames";
			// 
			// buttonFlagRenew
			// 
			this->buttonFlagRenew->Location = System::Drawing::Point(120, 167);
			this->buttonFlagRenew->Name = L"buttonFlagRenew";
			this->buttonFlagRenew->Size = System::Drawing::Size(60, 23);
			this->buttonFlagRenew->TabIndex = 35;
			this->buttonFlagRenew->Text = L"Flag";
			this->buttonFlagRenew->UseVisualStyleBackColor = true;
			this->buttonFlagRenew->Click += gcnew System::EventHandler(this, &FormMain::buttonFlagRenew_Click);
			// 
			// textBoxInstanceName
			// 
			this->textBoxInstanceName->Location = System::Drawing::Point(98, 20);
			this->textBoxInstanceName->Name = L"textBoxInstanceName";
			this->textBoxInstanceName->Size = System::Drawing::Size(177, 20);
			this->textBoxInstanceName->TabIndex = 71;
			this->textBoxInstanceName->TextChanged += gcnew System::EventHandler(this, &FormMain::textBoxInstanceName_TextChanged);
			// 
			// labelInstanceName
			// 
			this->labelInstanceName->AutoSize = true;
			this->labelInstanceName->Location = System::Drawing::Point(12, 23);
			this->labelInstanceName->Name = L"labelInstanceName";
			this->labelInstanceName->Size = System::Drawing::Size(80, 13);
			this->labelInstanceName->TabIndex = 70;
			this->labelInstanceName->Text = L"Instance name:";
			// 
			// labelFrameCounter
			// 
			this->labelFrameCounter->AutoSize = true;
			this->labelFrameCounter->Location = System::Drawing::Point(12, 128);
			this->labelFrameCounter->Name = L"labelFrameCounter";
			this->labelFrameCounter->Size = System::Drawing::Size(100, 13);
			this->labelFrameCounter->TabIndex = 81;
			this->labelFrameCounter->Text = L"Frame counter HW:";
			// 
			// labelFramerate
			// 
			this->labelFramerate->AutoSize = true;
			this->labelFramerate->Location = System::Drawing::Point(12, 147);
			this->labelFramerate->Name = L"labelFramerate";
			this->labelFramerate->Size = System::Drawing::Size(57, 13);
			this->labelFramerate->TabIndex = 82;
			this->labelFramerate->Text = L"Framerate:";
			// 
			// labelFlag1
			// 
			this->labelFlag1->AutoSize = true;
			this->labelFlag1->Location = System::Drawing::Point(12, 172);
			this->labelFlag1->Name = L"labelFlag1";
			this->labelFlag1->Size = System::Drawing::Size(30, 13);
			this->labelFlag1->TabIndex = 82;
			this->labelFlag1->Text = L"Flag:";
			// 
			// labelFlag
			// 
			this->labelFlag->AutoSize = true;
			this->labelFlag->Location = System::Drawing::Point(52, 172);
			this->labelFlag->Name = L"labelFlag";
			this->labelFlag->Size = System::Drawing::Size(31, 13);
			this->labelFlag->TabIndex = 82;
			this->labelFlag->Text = L"open";
			// 
			// groupBoxAreas
			// 
			this->groupBoxAreas->Location = System::Drawing::Point(15, 218);
			this->groupBoxAreas->Name = L"groupBoxAreas";
			this->groupBoxAreas->Size = System::Drawing::Size(174, 185);
			this->groupBoxAreas->TabIndex = 83;
			this->groupBoxAreas->TabStop = false;
			this->groupBoxAreas->Text = L"Areas:";
			// 
			// labelAreas
			// 
			this->labelAreas->AutoSize = true;
			this->labelAreas->Location = System::Drawing::Point(184, 109);
			this->labelAreas->Name = L"labelAreas";
			this->labelAreas->Size = System::Drawing::Size(34, 13);
			this->labelAreas->TabIndex = 81;
			this->labelAreas->Text = L"Areas";
			// 
			// label2
			// 
			this->label2->AutoSize = true;
			this->label2->Location = System::Drawing::Point(12, 199);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(18, 13);
			this->label2->TabIndex = 82;
			this->label2->Text = L"str";
			// 
			// labelMode
			// 
			this->labelMode->AutoSize = true;
			this->labelMode->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->labelMode->Location = System::Drawing::Point(12, 50);
			this->labelMode->Name = L"labelMode";
			this->labelMode->Size = System::Drawing::Size(37, 13);
			this->labelMode->TabIndex = 32;
			this->labelMode->Text = L"Mode:";
			// 
			// radioButtonModeMA
			// 
			this->radioButtonModeMA->AutoSize = true;
			this->radioButtonModeMA->Checked = true;
			this->radioButtonModeMA->Location = System::Drawing::Point(55, 48);
			this->radioButtonModeMA->Name = L"radioButtonModeMA";
			this->radioButtonModeMA->Size = System::Drawing::Size(116, 17);
			this->radioButtonModeMA->TabIndex = 84;
			this->radioButtonModeMA->TabStop = true;
			this->radioButtonModeMA->Text = L"measure areas only";
			this->radioButtonModeMA->UseVisualStyleBackColor = true;
			this->radioButtonModeMA->CheckedChanged += gcnew System::EventHandler(this, &FormMain::radioButtonMode_CheckedChanged);
			// 
			// radioButtonModeFR
			// 
			this->radioButtonModeFR->AutoSize = true;
			this->radioButtonModeFR->Location = System::Drawing::Point(177, 48);
			this->radioButtonModeFR->Name = L"radioButtonModeFR";
			this->radioButtonModeFR->Size = System::Drawing::Size(81, 17);
			this->radioButtonModeFR->TabIndex = 84;
			this->radioButtonModeFR->Text = L"frames only ";
			this->radioButtonModeFR->UseVisualStyleBackColor = true;
			this->radioButtonModeFR->CheckedChanged += gcnew System::EventHandler(this, &FormMain::radioButtonMode_CheckedChanged);
			// 
			// radioButtonModeMAFR
			// 
			this->radioButtonModeMAFR->AutoSize = true;
			this->radioButtonModeMAFR->Location = System::Drawing::Point(55, 67);
			this->radioButtonModeMAFR->Name = L"radioButtonModeMAFR";
			this->radioButtonModeMAFR->Size = System::Drawing::Size(179, 17);
			this->radioButtonModeMAFR->TabIndex = 84;
			this->radioButtonModeMAFR->Text = L"both (measure areas and frames)";
			this->radioButtonModeMAFR->UseVisualStyleBackColor = true;
			this->radioButtonModeMAFR->CheckedChanged += gcnew System::EventHandler(this, &FormMain::radioButtonMode_CheckedChanged);
			// 
			// labelFrameCounterFR
			// 
			this->labelFrameCounterFR->AutoSize = true;
			this->labelFrameCounterFR->Location = System::Drawing::Point(118, 128);
			this->labelFrameCounterFR->Name = L"labelFrameCounterFR";
			this->labelFrameCounterFR->Size = System::Drawing::Size(16, 13);
			this->labelFrameCounterFR->TabIndex = 81;
			this->labelFrameCounterFR->Text = L"---";
			// 
			// labelFramerateFR
			// 
			this->labelFramerateFR->AutoSize = true;
			this->labelFramerateFR->Location = System::Drawing::Point(118, 147);
			this->labelFramerateFR->Name = L"labelFramerateFR";
			this->labelFramerateFR->Size = System::Drawing::Size(16, 13);
			this->labelFramerateFR->TabIndex = 82;
			this->labelFramerateFR->Text = L"---";
			// 
			// labelFrameCounterMA
			// 
			this->labelFrameCounterMA->AutoSize = true;
			this->labelFrameCounterMA->Location = System::Drawing::Point(184, 128);
			this->labelFrameCounterMA->Name = L"labelFrameCounterMA";
			this->labelFrameCounterMA->Size = System::Drawing::Size(16, 13);
			this->labelFrameCounterMA->TabIndex = 81;
			this->labelFrameCounterMA->Text = L"---";
			// 
			// labelFramerateMA
			// 
			this->labelFramerateMA->AutoSize = true;
			this->labelFramerateMA->Location = System::Drawing::Point(184, 147);
			this->labelFramerateMA->Name = L"labelFramerateMA";
			this->labelFramerateMA->Size = System::Drawing::Size(16, 13);
			this->labelFramerateMA->TabIndex = 82;
			this->labelFramerateMA->Text = L"---";
			// 
			// FormMain
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(487, 415);
			this->Controls->Add(this->radioButtonModeMAFR);
			this->Controls->Add(this->radioButtonModeFR);
			this->Controls->Add(this->radioButtonModeMA);
			this->Controls->Add(this->groupBoxAreas);
			this->Controls->Add(this->pictureBox);
			this->Controls->Add(this->buttonFlagRenew);
			this->Controls->Add(this->labelFlag);
			this->Controls->Add(this->label2);
			this->Controls->Add(this->labelFlag1);
			this->Controls->Add(this->labelFramerateMA);
			this->Controls->Add(this->labelFramerateFR);
			this->Controls->Add(this->labelFramerate);
			this->Controls->Add(this->labelAreas);
			this->Controls->Add(this->labelFrameCounterMA);
			this->Controls->Add(this->labelFrameCounterFR);
			this->Controls->Add(this->labelFrameCounter);
			this->Controls->Add(this->textBoxInstanceName);
			this->Controls->Add(this->labelInstanceName);
			this->Controls->Add(this->labelMode);
			this->Controls->Add(this->labelFrames);
			this->Controls->Add(this->label1);
			this->Icon = (cli::safe_cast<System::Drawing::Icon^>(resources->GetObject(L"$this.Icon")));
			this->Name = L"FormMain";
			this->Text = L"Imager IPC Fast Measuring Sample Application";
			this->FormClosing += gcnew System::Windows::Forms::FormClosingEventHandler(this, &FormMain::FormMain_FormClosing);
			this->Load += gcnew System::EventHandler(this, &FormMain::FormMain_Load);
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox))->EndInit();
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
protected: 
	void AppInit(void);
	void AppExit(void);
	void InitIPC(void);
	void ReleaseIPC(void) ;
	void Application_Idle(Object ^sender, EventArgs ^e);
	HRESULT MainTimer100ms(void);
	HRESULT MainTimer1000ms(void);
	void RenewFlag(void);
	void Init(int frameWidth, int frameHeight, int frameDepth);
	void UpdateSize(void);
	void InitMeasureAreas(int areaCount);
	HRESULT NewFrame(short *ImgBuf, int frameCounter);

	HRESULT OnServerStopped(int reason);
	HRESULT OnFrameInit(int frameWidth, int frameHeight, int frameDepth);
	HRESULT OnAreasInit(int areaCount, int areasBufferSize);
	HRESULT OnNewFrameEx(void * pBuffer, FrameMetadata2 *pMetadata);
	HRESULT OnNewAreasBuffer(void * pBuffer, FrameMetadata2 *pMetadata);
	HRESULT OnInitCompleted(void);

private: System::Void FormMain_Load(System::Object^  sender, System::EventArgs^  e) {AppInit(); } 
private: System::Void timer1_Tick(System::Object^  sender, System::EventArgs^  e)
		 {
			MainTimerDivider++;
			MainTimer100ms();
			if(!(MainTimerDivider%20)) MainTimer1000ms();
		 }
private: System::Void FormMain_FormClosing(System::Object^  sender, System::Windows::Forms::FormClosingEventArgs^  e) { AppExit(); }
private: System::Void timer2_Tick(System::Object^  sender, System::EventArgs^  e) {	if (!ipcInitialized || !Connected) InitIPC(); }
private: System::Void pictureBox_Paint(System::Object^  sender, System::Windows::Forms::PaintEventArgs^  e) {  e->Graphics->DrawImage( bmp, 0, 0 );  }
private: System::Void buttonFlagRenew_Click(System::Object^  sender, System::EventArgs^  e) { RenewFlag();	 }
private: System::Void textBoxInstanceName_TextChanged(System::Object^  sender, System::EventArgs^  e) ;
private: System::Void checkBoxBuffered_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {	ReleaseIPC(); };
private: System::Void radioButtonMode_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
};

}

