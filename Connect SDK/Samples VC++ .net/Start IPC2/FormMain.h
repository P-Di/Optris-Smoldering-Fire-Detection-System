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
	private: System::Windows::Forms::Label^  labelPIF;
	private: System::Windows::Forms::Label^  labelFlag1;
	private: System::Windows::Forms::Label^  labelFlag;
	private: System::Windows::Forms::Label^  label1;

	private: System::Windows::Forms::Timer^  timer1;
	private: System::Windows::Forms::PictureBox^  pictureBox;
	private: System::Windows::Forms::Timer^  timer2;
	private: System::Windows::Forms::Label^  labelTempTarget;
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

		array<Byte>^rgbValues;
		array<short>^Values;

		int MainTimerDivider;
		bool Painted;

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
			this->labelTempTarget = (gcnew System::Windows::Forms::Label());
			this->buttonFlagRenew = (gcnew System::Windows::Forms::Button());
			this->textBoxInstanceName = (gcnew System::Windows::Forms::TextBox());
			this->labelInstanceName = (gcnew System::Windows::Forms::Label());
			this->labelFrameCounter = (gcnew System::Windows::Forms::Label());
			this->labelPIF = (gcnew System::Windows::Forms::Label());
			this->labelFlag1 = (gcnew System::Windows::Forms::Label());
			this->labelFlag = (gcnew System::Windows::Forms::Label());
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->pictureBox))->BeginInit();
			this->SuspendLayout();
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Location = System::Drawing::Point(12, 50);
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
			// labelTempTarget
			// 
			this->labelTempTarget->AutoSize = true;
			this->labelTempTarget->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->labelTempTarget->Location = System::Drawing::Point(12, 72);
			this->labelTempTarget->Name = L"labelTempTarget";
			this->labelTempTarget->Size = System::Drawing::Size(71, 13);
			this->labelTempTarget->TabIndex = 32;
			this->labelTempTarget->Text = L"Target-Temp:";
			// 
			// buttonFlagRenew
			// 
			this->buttonFlagRenew->Location = System::Drawing::Point(120, 133);
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
			this->labelFrameCounter->Location = System::Drawing::Point(12, 94);
			this->labelFrameCounter->Name = L"labelFrameCounter";
			this->labelFrameCounter->Size = System::Drawing::Size(123, 13);
			this->labelFrameCounter->TabIndex = 81;
			this->labelFrameCounter->Text = L"Frame counter HW/SW:";
			// 
			// labelPIF
			// 
			this->labelPIF->AutoSize = true;
			this->labelPIF->Location = System::Drawing::Point(12, 116);
			this->labelPIF->Name = L"labelPIF";
			this->labelPIF->Size = System::Drawing::Size(26, 13);
			this->labelPIF->TabIndex = 82;
			this->labelPIF->Text = L"PIF:";
			// 
			// labelFlag1
			// 
			this->labelFlag1->AutoSize = true;
			this->labelFlag1->Location = System::Drawing::Point(12, 138);
			this->labelFlag1->Name = L"labelFlag1";
			this->labelFlag1->Size = System::Drawing::Size(30, 13);
			this->labelFlag1->TabIndex = 82;
			this->labelFlag1->Text = L"Flag:";
			// 
			// labelFlag
			// 
			this->labelFlag->AutoSize = true;
			this->labelFlag->Location = System::Drawing::Point(52, 138);
			this->labelFlag->Name = L"labelFlag";
			this->labelFlag->Size = System::Drawing::Size(31, 13);
			this->labelFlag->TabIndex = 82;
			this->labelFlag->Text = L"open";
			// 
			// FormMain
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(487, 186);
			this->Controls->Add(this->pictureBox);
			this->Controls->Add(this->buttonFlagRenew);
			this->Controls->Add(this->labelFlag);
			this->Controls->Add(this->labelFlag1);
			this->Controls->Add(this->labelPIF);
			this->Controls->Add(this->labelFrameCounter);
			this->Controls->Add(this->textBoxInstanceName);
			this->Controls->Add(this->labelInstanceName);
			this->Controls->Add(this->labelTempTarget);
			this->Controls->Add(this->label1);
			this->Icon = (cli::safe_cast<System::Drawing::Icon^  >(resources->GetObject(L"$this.Icon")));
			this->Name = L"FormMain";
			this->Text = L"Imager IPC Sample Application";
			this->Load += gcnew System::EventHandler(this, &FormMain::FormMain_Load);
			this->FormClosing += gcnew System::Windows::Forms::FormClosingEventHandler(this, &FormMain::FormMain_FormClosing);
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->pictureBox))->EndInit();
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
	HRESULT NewFrame(short *ImgBuf, int frameCounter);

	HRESULT OnServerStopped(int reason);
	HRESULT OnFrameInit(int frameWidth, int frameHeight, int frameDepth);
	HRESULT OnNewFrameEx(void * pBuffer, FrameMetadata2 *pMetadata);
	HRESULT OnNewFrame(char * pBuffer, int frameCounter);
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
private: System::Void buttonInit_Click(System::Object^  sender, System::EventArgs^  e){};
private: System::Void textBoxInstanceName_TextChanged(System::Object^  sender, System::EventArgs^  e) ;
private: System::Void checkBoxBuffered_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {	ReleaseIPC(); };
};

}

