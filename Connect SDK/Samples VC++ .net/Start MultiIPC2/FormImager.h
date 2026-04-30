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

	/// <summary>
	/// Zusammenfassung für FormImager
	///
	/// Warnung: Wenn Sie den Namen dieser Klasse ändern, müssen Sie auch
	///          die Ressourcendateiname-Eigenschaft für das Tool zur Kompilierung verwalteter Ressourcen ändern,
	///          das allen RESX-Dateien zugewiesen ist, von denen diese Klasse abhängt.
	///          Anderenfalls können die Designer nicht korrekt mit den lokalisierten Ressourcen
	///          arbeiten, die diesem Formular zugewiesen sind.
	/// </summary>
	public ref class FormImager : public System::Windows::Forms::Form
	{
	public:	FormImager(HWND hostHandle, IPC^ Ipc, int imagerIndex, String ^instanceName);

	private: System::Windows::Forms::GroupBox^  groupBoxVideo;
	private: System::Windows::Forms::GroupBox^  groupBoxMeasureAreas;
	private: System::Windows::Forms::TextBox^  textBoxInstanceName;
	private: System::Windows::Forms::Label^  labelInstanceName;
	private: System::Windows::Forms::Button^  buttonKill;
	private: System::Windows::Forms::Button^  buttonFlagRenew;
	private: System::Windows::Forms::ProgressBar^  progressBarInit;
	private: System::Windows::Forms::Label^  labelFW;
	private: System::Windows::Forms::Label^  labelFrameCounter;
	private: System::Windows::Forms::Panel^  panelImager;
	private: System::Windows::Forms::Label^  labelPIF;
	protected:
		/// <summary>
		/// Verwendete Ressourcen bereinigen.
		/// </summary>
		~FormImager();
		void GetBitmap(Bitmap^ Bmp, array<short>^values);
		void GetBitmap_Limits(array<short>^Values, short *min, short *max);

		short FrameWidth, FrameHeight, FrameDepth;
		double FrameRatio;
		int FrameSize;
		IPC^ ipc;
		bool ipcInitialized;
		int FrameCounter0, FrameCounter1, LastFrameCounter, FC0, FC1;
		System::DateTime LastFrameTime;
		System::Collections::ArrayList^ List;
		Bitmap^ bmp;
		Device Dev;
		bool Connected;
		bool Colors;
		int ImagerIndex;

		array<Byte>^rgbValues;
		array<short>^Values;

		int MainTimerDivider;
		bool Painted;
		USHORT MeasureAreaCount;
		Drawing::Size Margin;

		IPC::delOnServerStopped^		onServerStopped;
		IPC::delOnFrameInit^			onFrameInit;
		IPC::delOnNewFrameEx2^			onNewFrameEx2;
		IPC::delOnInitCompleted^		onInitCompleted;
		IPC::delOnConfigChanged^		onConfigChanged;

	private: System::Windows::Forms::Label^  label1;
	private: System::Windows::Forms::Label^  labelFramerate;
	private: System::Windows::Forms::Timer^  timer1;
	private: System::Windows::Forms::PictureBox^  pictureBox;
	private: System::Windows::Forms::Timer^  timer2;
	private: System::ComponentModel::IContainer^  components;
	private:
		/// <summary>
		/// Erforderliche Designervariable.
		/// </summary>
#pragma region Windows Form Designer generated code
		/// <summary>
		/// Erforderliche Methode für die Designerunterstützung.
		/// Der Inhalt der Methode darf nicht mit dem Code-Editor geändert werden.
		/// </summary>
		void InitializeComponent(void)
		{
			this->components = (gcnew System::ComponentModel::Container());
			System::ComponentModel::ComponentResourceManager^  resources = (gcnew System::ComponentModel::ComponentResourceManager(FormImager::typeid));
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->labelFramerate = (gcnew System::Windows::Forms::Label());
			this->timer1 = (gcnew System::Windows::Forms::Timer(this->components));
			this->pictureBox = (gcnew System::Windows::Forms::PictureBox());
			this->timer2 = (gcnew System::Windows::Forms::Timer(this->components));
			this->buttonFlagRenew = (gcnew System::Windows::Forms::Button());
			this->groupBoxVideo = (gcnew System::Windows::Forms::GroupBox());
			this->groupBoxMeasureAreas = (gcnew System::Windows::Forms::GroupBox());
			this->textBoxInstanceName = (gcnew System::Windows::Forms::TextBox());
			this->labelInstanceName = (gcnew System::Windows::Forms::Label());
			this->buttonKill = (gcnew System::Windows::Forms::Button());
			this->progressBarInit = (gcnew System::Windows::Forms::ProgressBar());
			this->labelFW = (gcnew System::Windows::Forms::Label());
			this->labelFrameCounter = (gcnew System::Windows::Forms::Label());
			this->labelPIF = (gcnew System::Windows::Forms::Label());
			this->panelImager = (gcnew System::Windows::Forms::Panel());
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->pictureBox))->BeginInit();
			this->groupBoxVideo->SuspendLayout();
			this->panelImager->SuspendLayout();
			this->SuspendLayout();
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Location = System::Drawing::Point(12, 48);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(16, 13);
			this->label1->TabIndex = 0;
			this->label1->Text = L"---";
			// 
			// labelFramerate
			// 
			this->labelFramerate->AutoSize = true;
			this->labelFramerate->Location = System::Drawing::Point(12, 125);
			this->labelFramerate->Name = L"labelFramerate";
			this->labelFramerate->Size = System::Drawing::Size(16, 13);
			this->labelFramerate->TabIndex = 4;
			this->labelFramerate->Text = L"---";
			// 
			// timer1
			// 
			this->timer1->Tick += gcnew System::EventHandler(this, &FormImager::timer1_Tick);
			// 
			// pictureBox
			// 
			this->pictureBox->Dock = System::Windows::Forms::DockStyle::Fill;
			this->pictureBox->Location = System::Drawing::Point(3, 16);
			this->pictureBox->Name = L"pictureBox";
			this->pictureBox->Size = System::Drawing::Size(194, 121);
			this->pictureBox->TabIndex = 8;
			this->pictureBox->TabStop = false;
			this->pictureBox->Paint += gcnew System::Windows::Forms::PaintEventHandler(this, &FormImager::pictureBox_Paint);
			// 
			// timer2
			// 
			this->timer2->Enabled = true;
			this->timer2->Interval = 500;
			this->timer2->Tick += gcnew System::EventHandler(this, &FormImager::timer2_Tick);
			// 
			// buttonFlagRenew
			// 
			this->buttonFlagRenew->Location = System::Drawing::Point(193, 153);
			this->buttonFlagRenew->Name = L"buttonFlagRenew";
			this->buttonFlagRenew->Size = System::Drawing::Size(73, 23);
			this->buttonFlagRenew->TabIndex = 35;
			this->buttonFlagRenew->Text = L"Flag";
			this->buttonFlagRenew->UseVisualStyleBackColor = true;
			this->buttonFlagRenew->Click += gcnew System::EventHandler(this, &FormImager::buttonFlagRenew_Click);
			// 
			// groupBoxVideo
			// 
			this->groupBoxVideo->Controls->Add(this->pictureBox);
			this->groupBoxVideo->Location = System::Drawing::Point(15, 182);
			this->groupBoxVideo->Name = L"groupBoxVideo";
			this->groupBoxVideo->Size = System::Drawing::Size(200, 140);
			this->groupBoxVideo->TabIndex = 66;
			this->groupBoxVideo->TabStop = false;
			// 
			// groupBoxMeasureAreas
			// 
			this->groupBoxMeasureAreas->Location = System::Drawing::Point(15, 328);
			this->groupBoxMeasureAreas->Name = L"groupBoxMeasureAreas";
			this->groupBoxMeasureAreas->Size = System::Drawing::Size(130, 30);
			this->groupBoxMeasureAreas->TabIndex = 67;
			this->groupBoxMeasureAreas->TabStop = false;
			this->groupBoxMeasureAreas->Text = L"Measure areas:";
			// 
			// textBoxInstanceName
			// 
			this->textBoxInstanceName->Location = System::Drawing::Point(98, 10);
			this->textBoxInstanceName->Name = L"textBoxInstanceName";
			this->textBoxInstanceName->Size = System::Drawing::Size(168, 20);
			this->textBoxInstanceName->TabIndex = 71;
			this->textBoxInstanceName->TextChanged += gcnew System::EventHandler(this, &FormImager::textBoxInstanceName_TextChanged);
			// 
			// labelInstanceName
			// 
			this->labelInstanceName->AutoSize = true;
			this->labelInstanceName->Location = System::Drawing::Point(12, 13);
			this->labelInstanceName->Name = L"labelInstanceName";
			this->labelInstanceName->Size = System::Drawing::Size(80, 13);
			this->labelInstanceName->TabIndex = 70;
			this->labelInstanceName->Text = L"Instance name:";
			// 
			// buttonKill
			// 
			this->buttonKill->Location = System::Drawing::Point(173, 36);
			this->buttonKill->Name = L"buttonKill";
			this->buttonKill->Size = System::Drawing::Size(93, 27);
			this->buttonKill->TabIndex = 72;
			this->buttonKill->Text = L"Close Host-Appl.";
			this->buttonKill->UseVisualStyleBackColor = true;
			this->buttonKill->Click += gcnew System::EventHandler(this, &FormImager::buttonKill_Click);
			// 
			// progressBarInit
			// 
			this->progressBarInit->Location = System::Drawing::Point(15, 153);
			this->progressBarInit->Name = L"progressBarInit";
			this->progressBarInit->Size = System::Drawing::Size(172, 23);
			this->progressBarInit->TabIndex = 74;
			this->progressBarInit->Visible = false;
			// 
			// labelFW
			// 
			this->labelFW->AutoSize = true;
			this->labelFW->Location = System::Drawing::Point(12, 100);
			this->labelFW->Name = L"labelFW";
			this->labelFW->Size = System::Drawing::Size(27, 13);
			this->labelFW->TabIndex = 29;
			this->labelFW->Text = L"FW:";
			// 
			// labelFrameCounter
			// 
			this->labelFrameCounter->AutoSize = true;
			this->labelFrameCounter->Location = System::Drawing::Point(12, 72);
			this->labelFrameCounter->Name = L"labelFrameCounter";
			this->labelFrameCounter->Size = System::Drawing::Size(123, 13);
			this->labelFrameCounter->TabIndex = 81;
			this->labelFrameCounter->Text = L"Frame counter HW/SW:";
			// 
			// labelPIF
			// 
			this->labelPIF->AutoSize = true;
			this->labelPIF->Location = System::Drawing::Point(12, 87);
			this->labelPIF->Name = L"labelPIF";
			this->labelPIF->Size = System::Drawing::Size(93, 13);
			this->labelPIF->TabIndex = 82;
			this->labelPIF->Text = L"PIF digital/analog:";
			// 
			// panelImager
			// 
			this->panelImager->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->panelImager->Controls->Add(this->labelInstanceName);
			this->panelImager->Controls->Add(this->buttonFlagRenew);
			this->panelImager->Controls->Add(this->label1);
			this->panelImager->Controls->Add(this->labelPIF);
			this->panelImager->Controls->Add(this->labelFramerate);
			this->panelImager->Controls->Add(this->labelFrameCounter);
			this->panelImager->Controls->Add(this->labelFW);
			this->panelImager->Controls->Add(this->progressBarInit);
			this->panelImager->Controls->Add(this->groupBoxVideo);
			this->panelImager->Controls->Add(this->buttonKill);
			this->panelImager->Controls->Add(this->groupBoxMeasureAreas);
			this->panelImager->Controls->Add(this->textBoxInstanceName);
			this->panelImager->Dock = System::Windows::Forms::DockStyle::Fill;
			this->panelImager->Location = System::Drawing::Point(0, 0);
			this->panelImager->Name = L"panelImager";
			this->panelImager->Size = System::Drawing::Size(295, 400);
			this->panelImager->TabIndex = 83;
			// 
			// FormImager
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(295, 400);
			this->Controls->Add(this->panelImager);
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::None;
			this->Icon = (cli::safe_cast<System::Drawing::Icon^  >(resources->GetObject(L"$this.Icon")));
			this->Name = L"FormImager";
			this->Text = L"FormImager";
			this->Load += gcnew System::EventHandler(this, &FormImager::FormImager_Load);
			this->FormClosing += gcnew System::Windows::Forms::FormClosingEventHandler(this, &FormImager::FormImager_FormClosing);
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->pictureBox))->EndInit();
			this->groupBoxVideo->ResumeLayout(false);
			this->panelImager->ResumeLayout(false);
			this->panelImager->PerformLayout();
			this->ResumeLayout(false);

		} 
#pragma endregion
protected: 
	void AppInit(void);
	void AppExit(void);
	void InitIPC(void);
	void ReleaseIPC(void) ;
	HRESULT MainTimer100ms(void);
	HRESULT MainTimer1000ms(void);
	void GetSoftwareInfo(void);
	void GetDeviceInfo(void);
	void GetDeviceSetup(void);
	void RenewFlag(void);
	void Init(int frameWidth, int frameHeight, int frameDepth); 
	void InitMeasureAreas(void);
	void UpdateSize(void);
	HRESULT NewFrame(short *ImgBuf, int frameCounter); 

	HRESULT OnServerStopped(int reason);
	HRESULT OnFrameInit(int frameWidth, int frameHeight, int frameDepth);
	HRESULT OnNewFrameEx(void * pBuffer, FrameMetadata2 *pMetadata);
	HRESULT OnNewFrame(char * pBuffer, int frameCounter);
	HRESULT OnInitCompleted(void);
	HRESULT OnConfigChanged(long reserved);

private: System::Void FormImager_Load(System::Object^  sender, System::EventArgs^  e) {AppInit(); } 
private: System::Void timer1_Tick(System::Object^  sender, System::EventArgs^  e)
		 {
			MainTimerDivider++;
			MainTimer100ms();
			if(!(MainTimerDivider%20)) MainTimer1000ms();
		 }
private: System::Void FormImager_FormClosing(System::Object^  sender, System::Windows::Forms::FormClosingEventArgs^  e) { AppExit(); }
private: System::Void timer2_Tick(System::Object^  sender, System::EventArgs^  e) {	if (!ipcInitialized) InitIPC(); }
private: System::Void pictureBox_Paint(System::Object^  sender, System::Windows::Forms::PaintEventArgs^  e) {  e->Graphics->DrawImage( bmp, 0, 0 );  }
private: System::Void buttonFlagRenew_Click(System::Object^  sender, System::EventArgs^  e) { RenewFlag();	 }
private: System::Void buttonInit_Click(System::Object^  sender, System::EventArgs^  e){};
 
private: System::Void textBoxInstanceName_TextChanged(System::Object^  sender, System::EventArgs^  e) ;
private: System::Void buttonKill_Click(System::Object^  sender, System::EventArgs^  e) { if(ipc) ipc->CloseApplication(ImagerIndex); };
private: System::Void checkBoxBuffered_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {	ReleaseIPC(); };
};

} 

