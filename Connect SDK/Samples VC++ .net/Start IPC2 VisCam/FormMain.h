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
	/// Zusammenfassung für FormMain
	///
	/// Warnung: Wenn Sie den Namen dieser Klasse ändern, müssen Sie auch
	///          die Ressourcendateiname-Eigenschaft für das Tool zur Kompilierung verwalteter Ressourcen ändern,
	///          das allen RESX-Dateien zugewiesen ist, von denen diese Klasse abhängt.
	///          Anderenfalls können die Designer nicht korrekt mit den lokalisierten Ressourcen
	///          arbeiten, die diesem Formular zugewiesen sind.
	/// </summary>
	public ref class FormMain : public System::Windows::Forms::Form
	{
	public:	FormMain(HWND hostHandle);
	private: System::Windows::Forms::GroupBox^  groupBoxVideoIR;
	private: System::Windows::Forms::GroupBox^  groupBoxMeasureAreas;
	private: System::Windows::Forms::TextBox^  textBoxInstanceName;
	private: System::Windows::Forms::Label^  labelInstanceName;
	private: System::Windows::Forms::Button^  buttonKill;
	private: System::Windows::Forms::Button^  buttonFlag;
	private: System::Windows::Forms::ProgressBar^  progressBarInit;
	private: System::Windows::Forms::Label^  labelFrameIR;

	private: System::Windows::Forms::GroupBox^  groupBoxVideoVis;
	private: System::Windows::Forms::Label^  label2;
	private: System::Windows::Forms::Label^  labelFrameVis;
	private: System::Windows::Forms::PictureBox^  pictureBoxVis;
	private: System::Windows::Forms::Label^  label1;
	private: System::Windows::Forms::Timer^  timer1;
	private: System::Windows::Forms::PictureBox^  pictureBoxIR;
	private: System::Windows::Forms::Timer^  timer2;
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
		/// <summary>
		/// Verwendete Ressourcen bereinigen.
		/// </summary>
		~FormMain();
		void GetBitmap(Bitmap^ Bmp, array<short>^values, array<Byte> ^rgbValues, bool color);
		void GetBitmap_Limits(array<short>^Values, short *min, short *max);

		short FrameIRWidth, FrameIRHeight, FrameIRDepth;
		double FrameIRRatio;
		int FrameIRSize, MetadataSize;
		short FrameVisWidth, FrameVisHeight, FrameVisDepth;
		double FrameVisRatio;
		int FrameVisSize;
		IPC^ ipc;
		bool ipcInitialized;
		bool frameInitialized, visframeInitialized;
		System::DateTime LastFrameIRTime, LastFrameVisTime;
		System::Collections::ArrayList ^ListIR, ^ListVis;
		double FramerateIR, FramerateVis;
		Bitmap ^bmpIR, ^bmpVis;
		Device Dev;
		bool Connected;
		bool Colors;

		array<Byte> ^rgbValuesIR, ^rgbValuesVis;
		array<short> ^ValuesIR, ^ValuesVis;

		int MainTimerDivider;
		bool PaintedIR, PaintedVis;
		bool Old_CorrEnable;
		USHORT Old_FlagMode;
		USHORT MeasureAreaCount;
		Drawing::Size Margin;

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
			System::ComponentModel::ComponentResourceManager^  resources = (gcnew System::ComponentModel::ComponentResourceManager(FormMain::typeid));
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->timer1 = (gcnew System::Windows::Forms::Timer(this->components));
			this->pictureBoxIR = (gcnew System::Windows::Forms::PictureBox());
			this->timer2 = (gcnew System::Windows::Forms::Timer(this->components));
			this->buttonFlag = (gcnew System::Windows::Forms::Button());
			this->groupBoxVideoIR = (gcnew System::Windows::Forms::GroupBox());
			this->groupBoxMeasureAreas = (gcnew System::Windows::Forms::GroupBox());
			this->textBoxInstanceName = (gcnew System::Windows::Forms::TextBox());
			this->labelInstanceName = (gcnew System::Windows::Forms::Label());
			this->buttonKill = (gcnew System::Windows::Forms::Button());
			this->progressBarInit = (gcnew System::Windows::Forms::ProgressBar());
			this->labelFrameIR = (gcnew System::Windows::Forms::Label());
			this->groupBoxVideoVis = (gcnew System::Windows::Forms::GroupBox());
			this->pictureBoxVis = (gcnew System::Windows::Forms::PictureBox());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->labelFrameVis = (gcnew System::Windows::Forms::Label());
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->pictureBoxIR))->BeginInit();
			this->groupBoxVideoIR->SuspendLayout();
			this->groupBoxVideoVis->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->pictureBoxVis))->BeginInit();
			this->SuspendLayout();
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Location = System::Drawing::Point(21, 58);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(16, 13);
			this->label1->TabIndex = 0;
			this->label1->Text = L"---";
			// 
			// timer1
			// 
			this->timer1->Tick += gcnew System::EventHandler(this, &FormMain::timer1_Tick);
			// 
			// pictureBoxIR
			// 
			this->pictureBoxIR->Dock = System::Windows::Forms::DockStyle::Fill;
			this->pictureBoxIR->Location = System::Drawing::Point(3, 16);
			this->pictureBoxIR->Name = L"pictureBoxIR";
			this->pictureBoxIR->Size = System::Drawing::Size(194, 121);
			this->pictureBoxIR->TabIndex = 8;
			this->pictureBoxIR->TabStop = false;
			this->pictureBoxIR->Paint += gcnew System::Windows::Forms::PaintEventHandler(this, &FormMain::pictureBoxIR_Paint);
			// 
			// timer2
			// 
			this->timer2->Enabled = true;
			this->timer2->Interval = 200;
			this->timer2->Tick += gcnew System::EventHandler(this, &FormMain::timer2_Tick);
			// 
			// buttonFlag
			// 
			this->buttonFlag->Location = System::Drawing::Point(228, 78);
			this->buttonFlag->Name = L"buttonFlag";
			this->buttonFlag->Size = System::Drawing::Size(47, 23);
			this->buttonFlag->TabIndex = 35;
			this->buttonFlag->Text = L"Flag";
			this->buttonFlag->UseVisualStyleBackColor = true;
			this->buttonFlag->Click += gcnew System::EventHandler(this, &FormMain::buttonFlagRenew_Click);
			// 
			// groupBoxVideoIR
			// 
			this->groupBoxVideoIR->Controls->Add(this->pictureBoxIR);
			this->groupBoxVideoIR->Location = System::Drawing::Point(24, 164);
			this->groupBoxVideoIR->Name = L"groupBoxVideoIR";
			this->groupBoxVideoIR->Size = System::Drawing::Size(200, 140);
			this->groupBoxVideoIR->TabIndex = 66;
			this->groupBoxVideoIR->TabStop = false;
			// 
			// groupBoxMeasureAreas
			// 
			this->groupBoxMeasureAreas->Location = System::Drawing::Point(24, 310);
			this->groupBoxMeasureAreas->Name = L"groupBoxMeasureAreas";
			this->groupBoxMeasureAreas->Size = System::Drawing::Size(130, 30);
			this->groupBoxMeasureAreas->TabIndex = 67;
			this->groupBoxMeasureAreas->TabStop = false;
			this->groupBoxMeasureAreas->Text = L"Measure areas:";
			// 
			// textBoxInstanceName
			// 
			this->textBoxInstanceName->Location = System::Drawing::Point(107, 20);
			this->textBoxInstanceName->Name = L"textBoxInstanceName";
			this->textBoxInstanceName->Size = System::Drawing::Size(168, 20);
			this->textBoxInstanceName->TabIndex = 71;
			this->textBoxInstanceName->TextChanged += gcnew System::EventHandler(this, &FormMain::textBoxInstanceName_TextChanged);
			// 
			// labelInstanceName
			// 
			this->labelInstanceName->AutoSize = true;
			this->labelInstanceName->Location = System::Drawing::Point(21, 23);
			this->labelInstanceName->Name = L"labelInstanceName";
			this->labelInstanceName->Size = System::Drawing::Size(80, 13);
			this->labelInstanceName->TabIndex = 70;
			this->labelInstanceName->Text = L"Instance name:";
			// 
			// buttonKill
			// 
			this->buttonKill->Location = System::Drawing::Point(182, 46);
			this->buttonKill->Name = L"buttonKill";
			this->buttonKill->Size = System::Drawing::Size(93, 27);
			this->buttonKill->TabIndex = 72;
			this->buttonKill->Text = L"Close Host-Appl.";
			this->buttonKill->UseVisualStyleBackColor = true;
			this->buttonKill->Click += gcnew System::EventHandler(this, &FormMain::buttonKill_Click);
			// 
			// progressBarInit
			// 
			this->progressBarInit->Location = System::Drawing::Point(24, 87);
			this->progressBarInit->Name = L"progressBarInit";
			this->progressBarInit->Size = System::Drawing::Size(196, 14);
			this->progressBarInit->TabIndex = 74;
			this->progressBarInit->Visible = false;
			// 
			// labelFrameIR
			// 
			this->labelFrameIR->AutoSize = true;
			this->labelFrameIR->Location = System::Drawing::Point(24, 148);
			this->labelFrameIR->Name = L"labelFrameIR";
			this->labelFrameIR->Size = System::Drawing::Size(43, 13);
			this->labelFrameIR->TabIndex = 81;
			this->labelFrameIR->Text = L"0 / 0Hz";
			// 
			// groupBoxVideoVis
			// 
			this->groupBoxVideoVis->Controls->Add(this->pictureBoxVis);
			this->groupBoxVideoVis->Location = System::Drawing::Point(230, 164);
			this->groupBoxVideoVis->Name = L"groupBoxVideoVis";
			this->groupBoxVideoVis->Size = System::Drawing::Size(200, 140);
			this->groupBoxVideoVis->TabIndex = 66;
			this->groupBoxVideoVis->TabStop = false;
			// 
			// pictureBoxVis
			// 
			this->pictureBoxVis->Dock = System::Windows::Forms::DockStyle::Fill;
			this->pictureBoxVis->Location = System::Drawing::Point(3, 16);
			this->pictureBoxVis->Name = L"pictureBoxVis";
			this->pictureBoxVis->Size = System::Drawing::Size(194, 121);
			this->pictureBoxVis->TabIndex = 8;
			this->pictureBoxVis->TabStop = false;
			this->pictureBoxVis->Paint += gcnew System::Windows::Forms::PaintEventHandler(this, &FormMain::pictureBoxVis_Paint);
			// 
			// label2
			// 
			this->label2->AutoSize = true;
			this->label2->Location = System::Drawing::Point(21, 130);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(136, 13);
			this->label2->TabIndex = 81;
			this->label2->Text = L"Frame counter / frame rate:";
			// 
			// labelFrameVis
			// 
			this->labelFrameVis->AutoSize = true;
			this->labelFrameVis->Location = System::Drawing::Point(227, 148);
			this->labelFrameVis->Name = L"labelFrameVis";
			this->labelFrameVis->Size = System::Drawing::Size(43, 13);
			this->labelFrameVis->TabIndex = 81;
			this->labelFrameVis->Text = L"0 / 0Hz";
			// 
			// FormMain
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(535, 433);
			this->Controls->Add(this->buttonFlag);
			this->Controls->Add(this->label2);
			this->Controls->Add(this->labelFrameVis);
			this->Controls->Add(this->labelFrameIR);
			this->Controls->Add(this->progressBarInit);
			this->Controls->Add(this->buttonKill);
			this->Controls->Add(this->textBoxInstanceName);
			this->Controls->Add(this->labelInstanceName);
			this->Controls->Add(this->groupBoxMeasureAreas);
			this->Controls->Add(this->groupBoxVideoVis);
			this->Controls->Add(this->groupBoxVideoIR);
			this->Controls->Add(this->label1);
			this->Icon = (cli::safe_cast<System::Drawing::Icon^  >(resources->GetObject(L"$this.Icon")));
			this->Name = L"FormMain";
			this->Text = L"Imager IPC Sample Application";
			this->Load += gcnew System::EventHandler(this, &FormMain::FormMain_Load);
			this->FormClosing += gcnew System::Windows::Forms::FormClosingEventHandler(this, &FormMain::FormMain_FormClosing);
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->pictureBoxIR))->EndInit();
			this->groupBoxVideoIR->ResumeLayout(false);
			this->groupBoxVideoVis->ResumeLayout(false);
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->pictureBoxVis))->EndInit();
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
	double GetFramerate(System::Collections::ArrayList ^List);
	HRESULT MainTimer100ms(void);
	HRESULT MainTimer1000ms(void);
	void GetSoftwareInfo(void);
	void GetDeviceInfo(void);
	void GetDeviceSetup(void);
	void RenewFlag(void);
	void SetFlag(bool flag);
	void InitIR(int frameWidth, int frameHeight, int frameDepth);
	void InitVis(int frameWidth, int frameHeight, int frameDepth);
	void InitMeasureAreas(void);
	void UpdateSize(void);
	void FileOpen(void);

	HRESULT OnServerStopped(int reason);
	HRESULT OnFrameIRInit(int frameWidth, int frameHeight, int frameDepth);
	HRESULT OnNewFrameIR(void * pBuffer, FrameMetadata2 *pMetadata);
	HRESULT OnFrameVisInit(int frameWidth, int frameHeight, int frameDepth);
	HRESULT OnNewFrameVis(void * pBuffer, FrameMetadata2 *pMetadata);
	HRESULT OnInitCompleted(void);
	HRESULT OnConfigChanged(long reserved);
	HRESULT OnFileCommandReady(wchar_t *path);

private: System::Void FormMain_Load(System::Object^  sender, System::EventArgs^  e) {AppInit(); } 
private: System::Void timer1_Tick(System::Object^  sender, System::EventArgs^  e)
		 {
			MainTimerDivider++;
			MainTimer100ms();
			if(!(MainTimerDivider%20)) MainTimer1000ms();
		 }
private: System::Void FormMain_FormClosing(System::Object^  sender, System::Windows::Forms::FormClosingEventArgs^  e) { AppExit(); }
private: System::Void timer2_Tick(System::Object^  sender, System::EventArgs^  e) {	if (!ipcInitialized) InitIPC(); }
private: System::Void pictureBoxIR_Paint(System::Object^  sender, System::Windows::Forms::PaintEventArgs^  e) {	e->Graphics->DrawImage( bmpIR, 0, 0 ); }
private: System::Void pictureBoxVis_Paint(System::Object^  sender, System::Windows::Forms::PaintEventArgs^  e) { e->Graphics->DrawImage( bmpVis, 0, 0 ); }
private: System::Void buttonFlagOpen_Click(System::Object^  sender, System::EventArgs^  e) { SetFlag(false); }
private: System::Void buttonFlagClose_Click(System::Object^  sender, System::EventArgs^  e) { SetFlag(true); }
private: System::Void buttonFlagRenew_Click(System::Object^  sender, System::EventArgs^  e) { RenewFlag();	 }
private: System::Void buttonInit_Click(System::Object^  sender, System::EventArgs^  e){};

private: System::Void textBoxInstanceName_TextChanged(System::Object^  sender, System::EventArgs^  e) ;
private: System::Void buttonKill_Click(System::Object^  sender, System::EventArgs^  e) { if(ipc) ipc->CloseApplication(0); };
private: System::Void checkBoxDock_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
private: System::Void checkBoxBuffered_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {	ReleaseIPC(); };
private: System::Void buttonSnapshot_Click(System::Object^  sender, System::EventArgs^  e) {  }
private: System::Void buttonRecord_Click(System::Object^  sender, System::EventArgs^  e) {  }
private: System::Void buttonStop_Click(System::Object^  sender, System::EventArgs^  e) {  }
private: System::Void buttonOpen_Click(System::Object^  sender, System::EventArgs^  e) {	 }
private: System::Void toolStripButtonOpen_Click(System::Object^  sender, System::EventArgs^  e) {	FileOpen();	 }
private: System::Void toolStripButtonPlay_Click(System::Object^  sender, System::EventArgs^  e) {	if(ipc) ipc->FilePlay(0);	 }
private: System::Void toolStripButtonPause_Click(System::Object^  sender, System::EventArgs^  e) {	if(ipc) ipc->FilePause(0);	 }
private: System::Void toolStripButtonStop_Click(System::Object^  sender, System::EventArgs^  e) {	if(ipc) ipc->FileStop(0);	 }
private: System::Void toolStripButtonRecord_Click(System::Object^  sender, System::EventArgs^  e) {	if(ipc) ipc->FileRecord(0);	 }
private: System::Void toolStripButtonSnapshot_Click(System::Object^  sender, System::EventArgs^  e) {	if(ipc) ipc->FileSnapshot(0);	 }
};

}

