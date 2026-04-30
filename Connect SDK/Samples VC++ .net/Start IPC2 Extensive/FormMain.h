#pragma once

#include "IPCSample.h"
#include "IPC2.h"

#define INVALIDPIXEL ((short)0x8000)

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

	private: System::Windows::Forms::GroupBox^  groupBoxFlag;
	private: System::Windows::Forms::Button^  buttonFlagClose;
	private: System::Windows::Forms::CheckBox^  checkBoxColors;
	private: System::Windows::Forms::GroupBox^  groupBoxVideo;
	private: System::Windows::Forms::GroupBox^  groupBoxMeasureAreas;
	private: System::Windows::Forms::ComboBox^  comboBoxOptics;
	private: System::Windows::Forms::TextBox^  textBoxInstanceName;
	private: System::Windows::Forms::Label^  labelInstanceName;
	private: System::Windows::Forms::Button^  buttonKill;
	private: System::Windows::Forms::Button^  buttonFlagRenew;
	private: System::Windows::Forms::ProgressBar^  progressBarInit;
	private: System::Windows::Forms::CheckBox^  checkBoxDock;
	private: System::Windows::Forms::GroupBox^  groupBoxHostAppl;
	private: System::Windows::Forms::Label^  labelHW;
	private: System::Windows::Forms::Label^  labelFW;
	private: System::Windows::Forms::Button^  button1;
	private: System::Windows::Forms::GroupBox^  groupBoxFile;
	private: System::Windows::Forms::ToolStrip^  toolStripFile;
	private: System::Windows::Forms::ToolStripButton^  toolStripButtonSnapshot;
	private: System::Windows::Forms::ToolStripButton^  toolStripButtonOpen;
	private: System::Windows::Forms::ToolStripButton^  toolStripButtonPlay;
	private: System::Windows::Forms::ToolStripButton^  toolStripButtonPause;
	private: System::Windows::Forms::ToolStripButton^  toolStripButtonStop;
	private: System::Windows::Forms::ToolStripButton^  toolStripButtonRecord;
	private: System::Windows::Forms::Label^  labelFrameCounter;
	private: System::Windows::Forms::Label^  labelPIF;
	private: System::Windows::Forms::NumericUpDown^  numericUpDownEmissivity;
	private: System::Windows::Forms::Label^  labelEmissivity;
	private: System::Windows::Forms::Label^  labelTempAmbient;
	private: System::Windows::Forms::NumericUpDown^  numericUpDownTempAmbient;
	private: System::Windows::Forms::Label^  labelFlag1;
	private: System::Windows::Forms::Label^  labelFlag;
	private: System::Windows::Forms::ComboBox^  comboBoxTempRange;
	private: System::Windows::Forms::Label^  label1;
	private: System::Windows::Forms::Label^  label2;
	private: System::Windows::Forms::Label^  labelFramerate;
	private: System::Windows::Forms::Timer^  timer1;
	private: System::Windows::Forms::Label^  labelTempChip;
	private: System::Windows::Forms::Label^  labelTempFlag;
	private: System::Windows::Forms::PictureBox^  pictureBox;
	private: System::Windows::Forms::GroupBox^  groupBoxOptics;
	private: System::Windows::Forms::Label^  labelVersionAppl;
	private: System::Windows::Forms::Timer^  timer2;
	private: System::Windows::Forms::Label^  labelVersionIPC;
	private: System::Windows::Forms::Label^  labelTempTarget;
	private: System::Windows::Forms::Label^  labelTempBox;
	private: System::Windows::Forms::Button^  buttonFlagOpen;
	private: System::Windows::Forms::GroupBox^  groupBoxRangeMode;
	private: System::Windows::Forms::RadioButton^  radioButtonRangeMinMax;
	private: System::Windows::Forms::RadioButton^  radioButtonRange3Sigma;
	private: System::Windows::Forms::RadioButton^  radioButtonRange1Sigma;
	private: System::Windows::Forms::Label^  labelOpticsFOV;
	private: System::Windows::Forms::Label^  labelTempRange;
	private: System::Windows::Forms::ComboBox^  comboBoxVideoFormats;
	private: System::Windows::Forms::Label^  labelVideoFormat;
    private: System::Windows::Forms::Label^  labelTempOptics;
    private: System::Windows::Forms::CheckBox^  checkBoxSpots;
	private: System::Windows::Forms::Button^  StartPixHiddenButton;
	private: System::Windows::Forms::TextBox^  startPixHiddenTextBox;

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
		void GetBitmap(Bitmap^ Bmp, array<short>^values);
		void GetBitmap_Limits(array<short>^Values, short *min, short *max);

		short FrameWidth, FrameHeight, FrameDepth, MetadataSize;
		double FrameRatio;
		int FrameSize;
		IPC^ ipc;
		bool ipcInitialized, frameInitialized;
		int FrameCounter0, FrameCounter1, LastFrameCounter, FC0, FC1;
		System::DateTime LastFrameTime;
		System::Collections::ArrayList^ List;
		double Tmax;
		Bitmap^ bmp;
		Device Dev;
		bool Connected;
		bool AppendLog;
		bool ResetPending;

		array<Byte>^rgbValues;
		array<short>^Values;

		int MainTimerDivider;
		bool Painted;
		bool Old_CorrEnable;
		USHORT Old_FlagMode;
		USHORT MeasureAreaCount;
		Drawing::Size Margin;
		Drawing::Rectangle^ OldApplRect;
		bool OldEmbeddedState;
        Drawing::Point HotSpot, ColdSpot; // Hot/Cold spot position
        Drawing::Pen ^HotPen, ^ColdPen, ^ContrastPen; // Pen for Hot/Cold spot
		System::Diagnostics::Process^ startedPix;
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
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->labelFramerate = (gcnew System::Windows::Forms::Label());
			this->timer1 = (gcnew System::Windows::Forms::Timer(this->components));
			this->labelTempChip = (gcnew System::Windows::Forms::Label());
			this->labelTempFlag = (gcnew System::Windows::Forms::Label());
			this->pictureBox = (gcnew System::Windows::Forms::PictureBox());
			this->groupBoxOptics = (gcnew System::Windows::Forms::GroupBox());
			this->labelTempAmbient = (gcnew System::Windows::Forms::Label());
			this->labelEmissivity = (gcnew System::Windows::Forms::Label());
			this->numericUpDownTempAmbient = (gcnew System::Windows::Forms::NumericUpDown());
			this->numericUpDownEmissivity = (gcnew System::Windows::Forms::NumericUpDown());
			this->comboBoxTempRange = (gcnew System::Windows::Forms::ComboBox());
			this->comboBoxVideoFormats = (gcnew System::Windows::Forms::ComboBox());
			this->comboBoxOptics = (gcnew System::Windows::Forms::ComboBox());
			this->labelVideoFormat = (gcnew System::Windows::Forms::Label());
			this->labelTempRange = (gcnew System::Windows::Forms::Label());
			this->labelOpticsFOV = (gcnew System::Windows::Forms::Label());
			this->labelVersionAppl = (gcnew System::Windows::Forms::Label());
			this->timer2 = (gcnew System::Windows::Forms::Timer(this->components));
			this->labelVersionIPC = (gcnew System::Windows::Forms::Label());
			this->labelTempTarget = (gcnew System::Windows::Forms::Label());
			this->labelTempBox = (gcnew System::Windows::Forms::Label());
			this->buttonFlagOpen = (gcnew System::Windows::Forms::Button());
			this->groupBoxRangeMode = (gcnew System::Windows::Forms::GroupBox());
			this->checkBoxSpots = (gcnew System::Windows::Forms::CheckBox());
			this->checkBoxColors = (gcnew System::Windows::Forms::CheckBox());
			this->radioButtonRangeMinMax = (gcnew System::Windows::Forms::RadioButton());
			this->radioButtonRange3Sigma = (gcnew System::Windows::Forms::RadioButton());
			this->radioButtonRange1Sigma = (gcnew System::Windows::Forms::RadioButton());
			this->groupBoxFlag = (gcnew System::Windows::Forms::GroupBox());
			this->buttonFlagRenew = (gcnew System::Windows::Forms::Button());
			this->buttonFlagClose = (gcnew System::Windows::Forms::Button());
			this->groupBoxVideo = (gcnew System::Windows::Forms::GroupBox());
			this->groupBoxMeasureAreas = (gcnew System::Windows::Forms::GroupBox());
			this->textBoxInstanceName = (gcnew System::Windows::Forms::TextBox());
			this->labelInstanceName = (gcnew System::Windows::Forms::Label());
			this->buttonKill = (gcnew System::Windows::Forms::Button());
			this->progressBarInit = (gcnew System::Windows::Forms::ProgressBar());
			this->checkBoxDock = (gcnew System::Windows::Forms::CheckBox());
			this->groupBoxHostAppl = (gcnew System::Windows::Forms::GroupBox());
			this->labelHW = (gcnew System::Windows::Forms::Label());
			this->labelFW = (gcnew System::Windows::Forms::Label());
			this->button1 = (gcnew System::Windows::Forms::Button());
			this->groupBoxFile = (gcnew System::Windows::Forms::GroupBox());
			this->toolStripFile = (gcnew System::Windows::Forms::ToolStrip());
			this->toolStripButtonOpen = (gcnew System::Windows::Forms::ToolStripButton());
			this->toolStripButtonPlay = (gcnew System::Windows::Forms::ToolStripButton());
			this->toolStripButtonPause = (gcnew System::Windows::Forms::ToolStripButton());
			this->toolStripButtonStop = (gcnew System::Windows::Forms::ToolStripButton());
			this->toolStripButtonRecord = (gcnew System::Windows::Forms::ToolStripButton());
			this->toolStripButtonSnapshot = (gcnew System::Windows::Forms::ToolStripButton());
			this->labelFrameCounter = (gcnew System::Windows::Forms::Label());
			this->labelPIF = (gcnew System::Windows::Forms::Label());
			this->labelFlag1 = (gcnew System::Windows::Forms::Label());
			this->labelFlag = (gcnew System::Windows::Forms::Label());
			this->labelTempOptics = (gcnew System::Windows::Forms::Label());
			this->StartPixHiddenButton = (gcnew System::Windows::Forms::Button());
			this->startPixHiddenTextBox = (gcnew System::Windows::Forms::TextBox());
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox))->BeginInit();
			this->groupBoxOptics->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->numericUpDownTempAmbient))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->numericUpDownEmissivity))->BeginInit();
			this->groupBoxRangeMode->SuspendLayout();
			this->groupBoxFlag->SuspendLayout();
			this->groupBoxVideo->SuspendLayout();
			this->groupBoxFile->SuspendLayout();
			this->toolStripFile->SuspendLayout();
			this->SuspendLayout();
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Location = System::Drawing::Point(21, 143);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(16, 13);
			this->label1->TabIndex = 0;
			this->label1->Text = L"---";
			// 
			// label2
			// 
			this->label2->AutoSize = true;
			this->label2->Location = System::Drawing::Point(21, 179);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(16, 13);
			this->label2->TabIndex = 1;
			this->label2->Text = L"---";
			// 
			// labelFramerate
			// 
			this->labelFramerate->AutoSize = true;
			this->labelFramerate->Location = System::Drawing::Point(21, 220);
			this->labelFramerate->Name = L"labelFramerate";
			this->labelFramerate->Size = System::Drawing::Size(16, 13);
			this->labelFramerate->TabIndex = 4;
			this->labelFramerate->Text = L"---";
			// 
			// timer1
			// 
			this->timer1->Tick += gcnew System::EventHandler(this, &FormMain::timer1_Tick);
			// 
			// labelTempChip
			// 
			this->labelTempChip->AutoSize = true;
			this->labelTempChip->ForeColor = System::Drawing::Color::Red;
			this->labelTempChip->Location = System::Drawing::Point(21, 265);
			this->labelTempChip->Name = L"labelTempChip";
			this->labelTempChip->Size = System::Drawing::Size(61, 13);
			this->labelTempChip->TabIndex = 5;
			this->labelTempChip->Text = L"Chip-Temp:";
			// 
			// labelTempFlag
			// 
			this->labelTempFlag->AutoSize = true;
			this->labelTempFlag->ForeColor = System::Drawing::Color::Green;
			this->labelTempFlag->Location = System::Drawing::Point(21, 280);
			this->labelTempFlag->Name = L"labelTempFlag";
			this->labelTempFlag->Size = System::Drawing::Size(60, 13);
			this->labelTempFlag->TabIndex = 6;
			this->labelTempFlag->Text = L"Flag-Temp:";
			// 
			// pictureBox
			// 
			this->pictureBox->Dock = System::Windows::Forms::DockStyle::Fill;
			this->pictureBox->Location = System::Drawing::Point(3, 16);
			this->pictureBox->Name = L"pictureBox";
			this->pictureBox->Size = System::Drawing::Size(194, 121);
			this->pictureBox->TabIndex = 8;
			this->pictureBox->TabStop = false;
			this->pictureBox->Paint += gcnew System::Windows::Forms::PaintEventHandler(this, &FormMain::pictureBox_Paint);
			// 
			// groupBoxOptics
			// 
			this->groupBoxOptics->Controls->Add(this->labelTempAmbient);
			this->groupBoxOptics->Controls->Add(this->labelEmissivity);
			this->groupBoxOptics->Controls->Add(this->numericUpDownTempAmbient);
			this->groupBoxOptics->Controls->Add(this->numericUpDownEmissivity);
			this->groupBoxOptics->Controls->Add(this->comboBoxTempRange);
			this->groupBoxOptics->Controls->Add(this->comboBoxVideoFormats);
			this->groupBoxOptics->Controls->Add(this->comboBoxOptics);
			this->groupBoxOptics->Controls->Add(this->labelVideoFormat);
			this->groupBoxOptics->Controls->Add(this->labelTempRange);
			this->groupBoxOptics->Controls->Add(this->labelOpticsFOV);
			this->groupBoxOptics->Location = System::Drawing::Point(281, 289);
			this->groupBoxOptics->Name = L"groupBoxOptics";
			this->groupBoxOptics->Size = System::Drawing::Size(200, 136);
			this->groupBoxOptics->TabIndex = 13;
			this->groupBoxOptics->TabStop = false;
			// 
			// labelTempAmbient
			// 
			this->labelTempAmbient->AutoSize = true;
			this->labelTempAmbient->Location = System::Drawing::Point(92, 69);
			this->labelTempAmbient->Name = L"labelTempAmbient";
			this->labelTempAmbient->Size = System::Drawing::Size(41, 13);
			this->labelTempAmbient->TabIndex = 84;
			this->labelTempAmbient->Text = L"TAmb.:";
			// 
			// labelEmissivity
			// 
			this->labelEmissivity->AutoSize = true;
			this->labelEmissivity->Location = System::Drawing::Point(6, 69);
			this->labelEmissivity->Name = L"labelEmissivity";
			this->labelEmissivity->Size = System::Drawing::Size(31, 13);
			this->labelEmissivity->TabIndex = 84;
			this->labelEmissivity->Text = L"Eps.:";
			// 
			// numericUpDownTempAmbient
			// 
			this->numericUpDownTempAmbient->DecimalPlaces = 1;
			this->numericUpDownTempAmbient->Increment = System::Decimal(gcnew cli::array< System::Int32 >(4) { 1, 0, 0, 65536 });
			this->numericUpDownTempAmbient->Location = System::Drawing::Point(134, 67);
			this->numericUpDownTempAmbient->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) { 200, 0, 0, 0 });
			this->numericUpDownTempAmbient->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) { 40, 0, 0, System::Int32::MinValue });
			this->numericUpDownTempAmbient->Name = L"numericUpDownTempAmbient";
			this->numericUpDownTempAmbient->Size = System::Drawing::Size(60, 20);
			this->numericUpDownTempAmbient->TabIndex = 83;
			this->numericUpDownTempAmbient->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) { 23, 0, 0, 0 });
			this->numericUpDownTempAmbient->ValueChanged += gcnew System::EventHandler(this, &FormMain::numericUpDownTempAmbient_ValueChanged);
			// 
			// numericUpDownEmissivity
			// 
			this->numericUpDownEmissivity->DecimalPlaces = 3;
			this->numericUpDownEmissivity->Increment = System::Decimal(gcnew cli::array< System::Int32 >(4) { 1, 0, 0, 196608 });
			this->numericUpDownEmissivity->Location = System::Drawing::Point(43, 67);
			this->numericUpDownEmissivity->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) { 11, 0, 0, 65536 });
			this->numericUpDownEmissivity->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) { 1, 0, 0, 65536 });
			this->numericUpDownEmissivity->Name = L"numericUpDownEmissivity";
			this->numericUpDownEmissivity->Size = System::Drawing::Size(48, 20);
			this->numericUpDownEmissivity->TabIndex = 83;
			this->numericUpDownEmissivity->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) { 10, 0, 0, 65536 });
			this->numericUpDownEmissivity->ValueChanged += gcnew System::EventHandler(this, &FormMain::numericUpDownEmissivity_ValueChanged);
			// 
			// comboBoxTempRange
			// 
			this->comboBoxTempRange->FormattingEnabled = true;
			this->comboBoxTempRange->Location = System::Drawing::Point(81, 40);
			this->comboBoxTempRange->Name = L"comboBoxTempRange";
			this->comboBoxTempRange->Size = System::Drawing::Size(113, 21);
			this->comboBoxTempRange->TabIndex = 67;
			this->comboBoxTempRange->SelectedIndexChanged += gcnew System::EventHandler(this, &FormMain::comboBoxTempRange_SelectedIndexChanged);
			// 
			// comboBoxVideoFormats
			// 
			this->comboBoxVideoFormats->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 6.75F, System::Drawing::FontStyle::Regular,
				System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(0)));
			this->comboBoxVideoFormats->FormattingEnabled = true;
			this->comboBoxVideoFormats->Location = System::Drawing::Point(9, 106);
			this->comboBoxVideoFormats->Name = L"comboBoxVideoFormats";
			this->comboBoxVideoFormats->Size = System::Drawing::Size(185, 20);
			this->comboBoxVideoFormats->TabIndex = 67;
			this->comboBoxVideoFormats->SelectedIndexChanged += gcnew System::EventHandler(this, &FormMain::comboBoxVideoFormats_SelectedIndexChanged);
			// 
			// comboBoxOptics
			// 
			this->comboBoxOptics->FormattingEnabled = true;
			this->comboBoxOptics->Location = System::Drawing::Point(81, 13);
			this->comboBoxOptics->Name = L"comboBoxOptics";
			this->comboBoxOptics->Size = System::Drawing::Size(113, 21);
			this->comboBoxOptics->TabIndex = 67;
			this->comboBoxOptics->SelectedIndexChanged += gcnew System::EventHandler(this, &FormMain::comboBoxOptics_SelectedIndexChanged);
			// 
			// labelVideoFormat
			// 
			this->labelVideoFormat->AutoSize = true;
			this->labelVideoFormat->Location = System::Drawing::Point(6, 90);
			this->labelVideoFormat->Name = L"labelVideoFormat";
			this->labelVideoFormat->Size = System::Drawing::Size(74, 13);
			this->labelVideoFormat->TabIndex = 59;
			this->labelVideoFormat->Text = L"Video formats:";
			// 
			// labelTempRange
			// 
			this->labelTempRange->AutoSize = true;
			this->labelTempRange->Location = System::Drawing::Point(6, 43);
			this->labelTempRange->Name = L"labelTempRange";
			this->labelTempRange->Size = System::Drawing::Size(70, 13);
			this->labelTempRange->TabIndex = 60;
			this->labelTempRange->Text = L"Temp. range:";
			// 
			// labelOpticsFOV
			// 
			this->labelOpticsFOV->AutoSize = true;
			this->labelOpticsFOV->Location = System::Drawing::Point(6, 16);
			this->labelOpticsFOV->Name = L"labelOpticsFOV";
			this->labelOpticsFOV->Size = System::Drawing::Size(40, 13);
			this->labelOpticsFOV->TabIndex = 59;
			this->labelOpticsFOV->Text = L"Optics:";
			// 
			// labelVersionAppl
			// 
			this->labelVersionAppl->AutoSize = true;
			this->labelVersionAppl->Location = System::Drawing::Point(23, 348);
			this->labelVersionAppl->Name = L"labelVersionAppl";
			this->labelVersionAppl->Size = System::Drawing::Size(59, 13);
			this->labelVersionAppl->TabIndex = 27;
			this->labelVersionAppl->Text = L"Host-Appl.:";
			// 
			// timer2
			// 
			this->timer2->Enabled = true;
			this->timer2->Interval = 200;
			this->timer2->Tick += gcnew System::EventHandler(this, &FormMain::timer2_Tick);
			// 
			// labelVersionIPC
			// 
			this->labelVersionIPC->AutoSize = true;
			this->labelVersionIPC->Location = System::Drawing::Point(23, 363);
			this->labelVersionIPC->Name = L"labelVersionIPC";
			this->labelVersionIPC->Size = System::Drawing::Size(40, 13);
			this->labelVersionIPC->TabIndex = 29;
			this->labelVersionIPC->Text = L"IPC-dll:";
			// 
			// labelTempTarget
			// 
			this->labelTempTarget->AutoSize = true;
			this->labelTempTarget->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->labelTempTarget->ForeColor = System::Drawing::Color::Blue;
			this->labelTempTarget->Location = System::Drawing::Point(21, 325);
			this->labelTempTarget->Name = L"labelTempTarget";
			this->labelTempTarget->Size = System::Drawing::Size(71, 13);
			this->labelTempTarget->TabIndex = 32;
			this->labelTempTarget->Text = L"Target-Temp:";
			// 
			// labelTempBox
			// 
			this->labelTempBox->AutoSize = true;
			this->labelTempBox->ForeColor = System::Drawing::Color::Magenta;
			this->labelTempBox->Location = System::Drawing::Point(21, 295);
			this->labelTempBox->Name = L"labelTempBox";
			this->labelTempBox->Size = System::Drawing::Size(58, 13);
			this->labelTempBox->TabIndex = 30;
			this->labelTempBox->Text = L"Box-Temp:";
			// 
			// buttonFlagOpen
			// 
			this->buttonFlagOpen->Location = System::Drawing::Point(16, 16);
			this->buttonFlagOpen->Name = L"buttonFlagOpen";
			this->buttonFlagOpen->Size = System::Drawing::Size(48, 23);
			this->buttonFlagOpen->TabIndex = 34;
			this->buttonFlagOpen->Text = L"Open";
			this->buttonFlagOpen->UseVisualStyleBackColor = true;
			this->buttonFlagOpen->Click += gcnew System::EventHandler(this, &FormMain::buttonFlagOpen_Click);
			// 
			// groupBoxRangeMode
			// 
			this->groupBoxRangeMode->Controls->Add(this->checkBoxSpots);
			this->groupBoxRangeMode->Controls->Add(this->checkBoxColors);
			this->groupBoxRangeMode->Controls->Add(this->radioButtonRangeMinMax);
			this->groupBoxRangeMode->Controls->Add(this->radioButtonRange3Sigma);
			this->groupBoxRangeMode->Controls->Add(this->radioButtonRange1Sigma);
			this->groupBoxRangeMode->Location = System::Drawing::Point(281, 153);
			this->groupBoxRangeMode->Name = L"groupBoxRangeMode";
			this->groupBoxRangeMode->Size = System::Drawing::Size(200, 75);
			this->groupBoxRangeMode->TabIndex = 57;
			this->groupBoxRangeMode->TabStop = false;
			this->groupBoxRangeMode->Text = L"Display range";
			// 
			// checkBoxSpots
			// 
			this->checkBoxSpots->AutoSize = true;
			this->checkBoxSpots->Location = System::Drawing::Point(95, 47);
			this->checkBoxSpots->Name = L"checkBoxSpots";
			this->checkBoxSpots->Size = System::Drawing::Size(53, 17);
			this->checkBoxSpots->TabIndex = 1;
			this->checkBoxSpots->Text = L"Spots";
			this->checkBoxSpots->UseVisualStyleBackColor = true;
			// 
			// checkBoxColors
			// 
			this->checkBoxColors->AutoSize = true;
			this->checkBoxColors->Location = System::Drawing::Point(16, 47);
			this->checkBoxColors->Name = L"checkBoxColors";
			this->checkBoxColors->Size = System::Drawing::Size(55, 17);
			this->checkBoxColors->TabIndex = 1;
			this->checkBoxColors->Text = L"Colors";
			this->checkBoxColors->UseVisualStyleBackColor = true;
			this->checkBoxColors->CheckedChanged += gcnew System::EventHandler(this, &FormMain::checkBoxColors_CheckedChanged);
			// 
			// radioButtonRangeMinMax
			// 
			this->radioButtonRangeMinMax->AutoSize = true;
			this->radioButtonRangeMinMax->Location = System::Drawing::Point(16, 16);
			this->radioButtonRangeMinMax->Name = L"radioButtonRangeMinMax";
			this->radioButtonRangeMinMax->Size = System::Drawing::Size(48, 17);
			this->radioButtonRangeMinMax->TabIndex = 0;
			this->radioButtonRangeMinMax->TabStop = true;
			this->radioButtonRangeMinMax->Text = L"< / >";
			this->radioButtonRangeMinMax->UseVisualStyleBackColor = true;
			// 
			// radioButtonRange3Sigma
			// 
			this->radioButtonRange3Sigma->AutoSize = true;
			this->radioButtonRange3Sigma->Location = System::Drawing::Point(134, 16);
			this->radioButtonRange3Sigma->Name = L"radioButtonRange3Sigma";
			this->radioButtonRange3Sigma->Size = System::Drawing::Size(63, 17);
			this->radioButtonRange3Sigma->TabIndex = 0;
			this->radioButtonRange3Sigma->TabStop = true;
			this->radioButtonRange3Sigma->Text = L"3 Sigma";
			this->radioButtonRange3Sigma->UseVisualStyleBackColor = true;
			// 
			// radioButtonRange1Sigma
			// 
			this->radioButtonRange1Sigma->AutoSize = true;
			this->radioButtonRange1Sigma->Location = System::Drawing::Point(70, 16);
			this->radioButtonRange1Sigma->Name = L"radioButtonRange1Sigma";
			this->radioButtonRange1Sigma->Size = System::Drawing::Size(63, 17);
			this->radioButtonRange1Sigma->TabIndex = 0;
			this->radioButtonRange1Sigma->TabStop = true;
			this->radioButtonRange1Sigma->Text = L"1 Sigma";
			this->radioButtonRange1Sigma->UseVisualStyleBackColor = true;
			// 
			// groupBoxFlag
			// 
			this->groupBoxFlag->Controls->Add(this->buttonFlagRenew);
			this->groupBoxFlag->Controls->Add(this->buttonFlagClose);
			this->groupBoxFlag->Controls->Add(this->buttonFlagOpen);
			this->groupBoxFlag->Location = System::Drawing::Point(281, 232);
			this->groupBoxFlag->Name = L"groupBoxFlag";
			this->groupBoxFlag->Size = System::Drawing::Size(200, 51);
			this->groupBoxFlag->TabIndex = 65;
			this->groupBoxFlag->TabStop = false;
			this->groupBoxFlag->Text = L"Flag";
			// 
			// buttonFlagRenew
			// 
			this->buttonFlagRenew->Location = System::Drawing::Point(124, 16);
			this->buttonFlagRenew->Name = L"buttonFlagRenew";
			this->buttonFlagRenew->Size = System::Drawing::Size(60, 23);
			this->buttonFlagRenew->TabIndex = 35;
			this->buttonFlagRenew->Text = L"Renew";
			this->buttonFlagRenew->UseVisualStyleBackColor = true;
			this->buttonFlagRenew->Click += gcnew System::EventHandler(this, &FormMain::buttonFlagRenew_Click);
			// 
			// buttonFlagClose
			// 
			this->buttonFlagClose->Location = System::Drawing::Point(70, 16);
			this->buttonFlagClose->Name = L"buttonFlagClose";
			this->buttonFlagClose->Size = System::Drawing::Size(48, 23);
			this->buttonFlagClose->TabIndex = 34;
			this->buttonFlagClose->Text = L"Close";
			this->buttonFlagClose->UseVisualStyleBackColor = true;
			this->buttonFlagClose->Click += gcnew System::EventHandler(this, &FormMain::buttonFlagClose_Click);
			// 
			// groupBoxVideo
			// 
			this->groupBoxVideo->Controls->Add(this->pictureBox);
			this->groupBoxVideo->Location = System::Drawing::Point(281, 7);
			this->groupBoxVideo->Name = L"groupBoxVideo";
			this->groupBoxVideo->Size = System::Drawing::Size(200, 140);
			this->groupBoxVideo->TabIndex = 66;
			this->groupBoxVideo->TabStop = false;
			// 
			// groupBoxMeasureAreas
			// 
			this->groupBoxMeasureAreas->Location = System::Drawing::Point(24, 464);
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
			this->buttonKill->Location = System::Drawing::Point(182, 131);
			this->buttonKill->Name = L"buttonKill";
			this->buttonKill->Size = System::Drawing::Size(93, 27);
			this->buttonKill->TabIndex = 72;
			this->buttonKill->Text = L"Close Host-Appl.";
			this->buttonKill->UseVisualStyleBackColor = true;
			this->buttonKill->Click += gcnew System::EventHandler(this, &FormMain::buttonKill_Click);
			// 
			// progressBarInit
			// 
			this->progressBarInit->Location = System::Drawing::Point(24, 248);
			this->progressBarInit->Name = L"progressBarInit";
			this->progressBarInit->Size = System::Drawing::Size(251, 14);
			this->progressBarInit->TabIndex = 74;
			this->progressBarInit->Visible = false;
			// 
			// checkBoxDock
			// 
			this->checkBoxDock->AutoSize = true;
			this->checkBoxDock->Location = System::Drawing::Point(281, 485);
			this->checkBoxDock->Name = L"checkBoxDock";
			this->checkBoxDock->Size = System::Drawing::Size(133, 17);
			this->checkBoxDock->TabIndex = 75;
			this->checkBoxDock->Text = L"Dock Main Application";
			this->checkBoxDock->UseVisualStyleBackColor = true;
			this->checkBoxDock->CheckedChanged += gcnew System::EventHandler(this, &FormMain::checkBoxDock_CheckedChanged);
			// 
			// groupBoxHostAppl
			// 
			this->groupBoxHostAppl->Location = System::Drawing::Point(487, 12);
			this->groupBoxHostAppl->Name = L"groupBoxHostAppl";
			this->groupBoxHostAppl->Size = System::Drawing::Size(360, 379);
			this->groupBoxHostAppl->TabIndex = 76;
			this->groupBoxHostAppl->TabStop = false;
			this->groupBoxHostAppl->Text = L"Host application";
			// 
			// labelHW
			// 
			this->labelHW->AutoSize = true;
			this->labelHW->Location = System::Drawing::Point(179, 348);
			this->labelHW->Name = L"labelHW";
			this->labelHW->Size = System::Drawing::Size(29, 13);
			this->labelHW->TabIndex = 27;
			this->labelHW->Text = L"HW:";
			// 
			// labelFW
			// 
			this->labelFW->AutoSize = true;
			this->labelFW->Location = System::Drawing::Point(179, 363);
			this->labelFW->Name = L"labelFW";
			this->labelFW->Size = System::Drawing::Size(27, 13);
			this->labelFW->TabIndex = 29;
			this->labelFW->Text = L"FW:";
			// 
			// button1
			// 
			this->button1->AutoSize = true;
			this->button1->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 7, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->button1->Location = System::Drawing::Point(232, 197);
			this->button1->Name = L"button1";
			this->button1->Size = System::Drawing::Size(40, 23);
			this->button1->TabIndex = 77;
			this->button1->Text = L"reset";
			this->button1->UseVisualStyleBackColor = true;
			this->button1->Click += gcnew System::EventHandler(this, &FormMain::button1_Click);
			// 
			// groupBoxFile
			// 
			this->groupBoxFile->Controls->Add(this->toolStripFile);
			this->groupBoxFile->Location = System::Drawing::Point(281, 431);
			this->groupBoxFile->Name = L"groupBoxFile";
			this->groupBoxFile->Size = System::Drawing::Size(200, 48);
			this->groupBoxFile->TabIndex = 80;
			this->groupBoxFile->TabStop = false;
			this->groupBoxFile->Text = L"File";
			// 
			// toolStripFile
			// 
			this->toolStripFile->CanOverflow = false;
			this->toolStripFile->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(6) {
				this->toolStripButtonOpen,
					this->toolStripButtonPlay, this->toolStripButtonPause, this->toolStripButtonStop, this->toolStripButtonRecord, this->toolStripButtonSnapshot
			});
			this->toolStripFile->LayoutStyle = System::Windows::Forms::ToolStripLayoutStyle::Flow;
			this->toolStripFile->Location = System::Drawing::Point(3, 16);
			this->toolStripFile->Name = L"toolStripFile";
			this->toolStripFile->Size = System::Drawing::Size(194, 23);
			this->toolStripFile->TabIndex = 0;
			this->toolStripFile->Text = L"toolStrip1";
			// 
			// toolStripButtonOpen
			// 
			this->toolStripButtonOpen->DisplayStyle = System::Windows::Forms::ToolStripItemDisplayStyle::Image;
			this->toolStripButtonOpen->Image = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"toolStripButtonOpen.Image")));
			this->toolStripButtonOpen->ImageTransparentColor = System::Drawing::Color::Magenta;
			this->toolStripButtonOpen->Name = L"toolStripButtonOpen";
			this->toolStripButtonOpen->Size = System::Drawing::Size(23, 20);
			this->toolStripButtonOpen->Text = L"Open";
			this->toolStripButtonOpen->Click += gcnew System::EventHandler(this, &FormMain::toolStripButtonOpen_Click);
			// 
			// toolStripButtonPlay
			// 
			this->toolStripButtonPlay->DisplayStyle = System::Windows::Forms::ToolStripItemDisplayStyle::Image;
			this->toolStripButtonPlay->Image = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"toolStripButtonPlay.Image")));
			this->toolStripButtonPlay->ImageTransparentColor = System::Drawing::Color::Magenta;
			this->toolStripButtonPlay->Name = L"toolStripButtonPlay";
			this->toolStripButtonPlay->Size = System::Drawing::Size(23, 20);
			this->toolStripButtonPlay->Text = L"Play";
			this->toolStripButtonPlay->Click += gcnew System::EventHandler(this, &FormMain::toolStripButtonPlay_Click);
			// 
			// toolStripButtonPause
			// 
			this->toolStripButtonPause->DisplayStyle = System::Windows::Forms::ToolStripItemDisplayStyle::Image;
			this->toolStripButtonPause->Image = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"toolStripButtonPause.Image")));
			this->toolStripButtonPause->ImageTransparentColor = System::Drawing::Color::Magenta;
			this->toolStripButtonPause->Name = L"toolStripButtonPause";
			this->toolStripButtonPause->Size = System::Drawing::Size(23, 20);
			this->toolStripButtonPause->Text = L"Pause";
			this->toolStripButtonPause->Click += gcnew System::EventHandler(this, &FormMain::toolStripButtonPause_Click);
			// 
			// toolStripButtonStop
			// 
			this->toolStripButtonStop->DisplayStyle = System::Windows::Forms::ToolStripItemDisplayStyle::Image;
			this->toolStripButtonStop->Image = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"toolStripButtonStop.Image")));
			this->toolStripButtonStop->ImageTransparentColor = System::Drawing::Color::Magenta;
			this->toolStripButtonStop->Name = L"toolStripButtonStop";
			this->toolStripButtonStop->Size = System::Drawing::Size(23, 20);
			this->toolStripButtonStop->Text = L"Stop";
			this->toolStripButtonStop->Click += gcnew System::EventHandler(this, &FormMain::toolStripButtonStop_Click);
			// 
			// toolStripButtonRecord
			// 
			this->toolStripButtonRecord->DisplayStyle = System::Windows::Forms::ToolStripItemDisplayStyle::Image;
			this->toolStripButtonRecord->Image = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"toolStripButtonRecord.Image")));
			this->toolStripButtonRecord->ImageTransparentColor = System::Drawing::Color::Magenta;
			this->toolStripButtonRecord->Name = L"toolStripButtonRecord";
			this->toolStripButtonRecord->Size = System::Drawing::Size(23, 20);
			this->toolStripButtonRecord->Text = L"Record";
			this->toolStripButtonRecord->Click += gcnew System::EventHandler(this, &FormMain::toolStripButtonRecord_Click);
			// 
			// toolStripButtonSnapshot
			// 
			this->toolStripButtonSnapshot->DisplayStyle = System::Windows::Forms::ToolStripItemDisplayStyle::Image;
			this->toolStripButtonSnapshot->Image = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"toolStripButtonSnapshot.Image")));
			this->toolStripButtonSnapshot->ImageTransparentColor = System::Drawing::Color::Magenta;
			this->toolStripButtonSnapshot->Name = L"toolStripButtonSnapshot";
			this->toolStripButtonSnapshot->Size = System::Drawing::Size(23, 20);
			this->toolStripButtonSnapshot->Text = L"Snapshot";
			this->toolStripButtonSnapshot->Click += gcnew System::EventHandler(this, &FormMain::toolStripButtonSnapshot_Click);
			// 
			// labelFrameCounter
			// 
			this->labelFrameCounter->AutoSize = true;
			this->labelFrameCounter->Location = System::Drawing::Point(23, 378);
			this->labelFrameCounter->Name = L"labelFrameCounter";
			this->labelFrameCounter->Size = System::Drawing::Size(123, 13);
			this->labelFrameCounter->TabIndex = 81;
			this->labelFrameCounter->Text = L"Frame counter HW/SW:";
			// 
			// labelPIF
			// 
			this->labelPIF->AutoSize = true;
			this->labelPIF->Location = System::Drawing::Point(23, 393);
			this->labelPIF->Name = L"labelPIF";
			this->labelPIF->Size = System::Drawing::Size(26, 13);
			this->labelPIF->TabIndex = 82;
			this->labelPIF->Text = L"PIF:";
			// 
			// labelFlag1
			// 
			this->labelFlag1->AutoSize = true;
			this->labelFlag1->Location = System::Drawing::Point(23, 437);
			this->labelFlag1->Name = L"labelFlag1";
			this->labelFlag1->Size = System::Drawing::Size(30, 13);
			this->labelFlag1->TabIndex = 82;
			this->labelFlag1->Text = L"Flag:";
			// 
			// labelFlag
			// 
			this->labelFlag->AutoSize = true;
			this->labelFlag->Location = System::Drawing::Point(59, 437);
			this->labelFlag->Name = L"labelFlag";
			this->labelFlag->Size = System::Drawing::Size(31, 13);
			this->labelFlag->TabIndex = 82;
			this->labelFlag->Text = L"open";
			// 
			// labelTempOptics
			// 
			this->labelTempOptics->AutoSize = true;
			this->labelTempOptics->ForeColor = System::Drawing::Color::Maroon;
			this->labelTempOptics->Location = System::Drawing::Point(21, 310);
			this->labelTempOptics->Name = L"labelTempOptics";
			this->labelTempOptics->Size = System::Drawing::Size(70, 13);
			this->labelTempOptics->TabIndex = 30;
			this->labelTempOptics->Text = L"Optics-Temp:";
			// 
			// StartPixHiddenButton
			// 
			this->StartPixHiddenButton->Location = System::Drawing::Point(24, 46);
			this->StartPixHiddenButton->Name = L"StartPixHiddenButton";
			this->StartPixHiddenButton->Size = System::Drawing::Size(246, 25);
			this->StartPixHiddenButton->TabIndex = 83;
			this->StartPixHiddenButton->Text = L"Start Hidden Pix ";
			this->StartPixHiddenButton->UseVisualStyleBackColor = true;
			this->StartPixHiddenButton->Click += gcnew System::EventHandler(this, &FormMain::StartPixHiddenButton_Click);
			// 
			// startPixHiddenTextBox
			// 
			this->startPixHiddenTextBox->Location = System::Drawing::Point(24, 77);
			this->startPixHiddenTextBox->Multiline = true;
			this->startPixHiddenTextBox->Name = L"startPixHiddenTextBox";
			this->startPixHiddenTextBox->Size = System::Drawing::Size(246, 48);
			this->startPixHiddenTextBox->TabIndex = 84;
			this->startPixHiddenTextBox->Text = L"\"C:\\Program Files (x86)\\Optris GmbH\\PIX Connect\\Imager.exe\" /Invisible /Nosplash "
				L"/Layout=Economy /IPC=Temps";
			// 
			// FormMain
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(859, 508);
			this->Controls->Add(this->startPixHiddenTextBox);
			this->Controls->Add(this->StartPixHiddenButton);
			this->Controls->Add(this->labelFlag);
			this->Controls->Add(this->labelFlag1);
			this->Controls->Add(this->labelPIF);
			this->Controls->Add(this->labelFrameCounter);
			this->Controls->Add(this->groupBoxFile);
			this->Controls->Add(this->button1);
			this->Controls->Add(this->groupBoxHostAppl);
			this->Controls->Add(this->checkBoxDock);
			this->Controls->Add(this->progressBarInit);
			this->Controls->Add(this->buttonKill);
			this->Controls->Add(this->textBoxInstanceName);
			this->Controls->Add(this->labelInstanceName);
			this->Controls->Add(this->groupBoxMeasureAreas);
			this->Controls->Add(this->groupBoxVideo);
			this->Controls->Add(this->groupBoxFlag);
			this->Controls->Add(this->groupBoxRangeMode);
			this->Controls->Add(this->labelTempTarget);
			this->Controls->Add(this->labelTempOptics);
			this->Controls->Add(this->labelTempBox);
			this->Controls->Add(this->labelFW);
			this->Controls->Add(this->labelVersionIPC);
			this->Controls->Add(this->labelHW);
			this->Controls->Add(this->labelVersionAppl);
			this->Controls->Add(this->groupBoxOptics);
			this->Controls->Add(this->labelTempFlag);
			this->Controls->Add(this->labelTempChip);
			this->Controls->Add(this->labelFramerate);
			this->Controls->Add(this->label2);
			this->Controls->Add(this->label1);
			this->Icon = (cli::safe_cast<System::Drawing::Icon^>(resources->GetObject(L"$this.Icon")));
			this->Name = L"FormMain";
			this->Text = L"Imager Extensive IPC Sample Application";
			this->FormClosing += gcnew System::Windows::Forms::FormClosingEventHandler(this, &FormMain::FormMain_FormClosing);
			this->Load += gcnew System::EventHandler(this, &FormMain::FormMain_Load);
			this->Move += gcnew System::EventHandler(this, &FormMain::FormMain_Move);
			this->Resize += gcnew System::EventHandler(this, &FormMain::FormMain_Resize);
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox))->EndInit();
			this->groupBoxOptics->ResumeLayout(false);
			this->groupBoxOptics->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->numericUpDownTempAmbient))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->numericUpDownEmissivity))->EndInit();
			this->groupBoxRangeMode->ResumeLayout(false);
			this->groupBoxRangeMode->PerformLayout();
			this->groupBoxFlag->ResumeLayout(false);
			this->groupBoxVideo->ResumeLayout(false);
			this->groupBoxFile->ResumeLayout(false);
			this->groupBoxFile->PerformLayout();
			this->toolStripFile->ResumeLayout(false);
			this->toolStripFile->PerformLayout();
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
	void GetSoftwareInfo(void);
	void GetDeviceInfo(void);
	void GetDeviceSetup(void);
	void GetOptics(void);
	void GetTempRanges(void);
	void GetVideoFormats(void);
	void SetOpticsIndex(void);
	void SetTempRangeIndex(void);
	void SetVideoFormatIndex(void);
	void RenewFlag(void);
	void SetFlag(bool flag);
	void SetEmissivity(void);
	void SetTempAmbient(void);
	void Init(int frameWidth, int frameHeight, int frameDepth);
	void InitMeasureAreas(void);
	void UpdateSize(void);
	void ResizeHostAppl(void);
	HRESULT NewFrame(short *ImgBuf, int frameCounter);
	void FileOpen(void);
    void DrawReticle(Drawing::Graphics ^g, Drawing::Point point, Drawing::Pen ^pen, int l);
	void StartPix();
	HRESULT OnServerStopped(int reason);
	HRESULT OnFrameInit(int frameWidth, int frameHeight, int frameDepth);
	HRESULT OnNewFrameEx(void * pBuffer, FrameMetadata2 *pMetadata);
	HRESULT OnNewFrame(char * pBuffer, int frameCounter);
	HRESULT OnInitCompleted(void);
	HRESULT OnConfigChanged(long reserved);
	HRESULT OnFileCommandReady(wchar_t *path);
	HRESULT OnNewNMEAString(wchar_t *path);

private: System::Void FormMain_Load(System::Object^  sender, System::EventArgs^  e) {AppInit(); } 
private: System::Void timer1_Tick(System::Object^  sender, System::EventArgs^  e)
		 {
			MainTimerDivider++;
			MainTimer100ms();
			if(!(MainTimerDivider%20)) MainTimer1000ms();
		 }
private: System::Void FormMain_FormClosing(System::Object^  sender, System::Windows::Forms::FormClosingEventArgs^  e) { AppExit(); }
private: System::Void timer2_Tick(System::Object^  sender, System::EventArgs^  e) {	if (!ipcInitialized || !Connected) InitIPC(); }
private: System::Void pictureBox_Paint(System::Object^  sender, System::Windows::Forms::PaintEventArgs^  e);
private: System::Void comboBoxOptics_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e) {	SetOpticsIndex();	 }
private: System::Void comboBoxTempRange_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e) { SetTempRangeIndex();	 }
private: System::Void comboBoxVideoFormats_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e) { SetVideoFormatIndex();	}
private: System::Void buttonFlagOpen_Click(System::Object^  sender, System::EventArgs^  e) { SetFlag(false); }
private: System::Void buttonFlagClose_Click(System::Object^  sender, System::EventArgs^  e) { SetFlag(true); }
private: System::Void buttonFlagRenew_Click(System::Object^  sender, System::EventArgs^  e) { RenewFlag();	 }
private: System::Void numericUpDownEmissivity_ValueChanged(System::Object^  sender, System::EventArgs^  e) {SetEmissivity(); }
private: System::Void numericUpDownTempAmbient_ValueChanged(System::Object^  sender, System::EventArgs^  e) { SetTempAmbient(); }
private: System::Void buttonInit_Click(System::Object^  sender, System::EventArgs^  e){};

private: System::Void textBoxInstanceName_TextChanged(System::Object^  sender, System::EventArgs^  e) ;
private: System::Void buttonKill_Click(System::Object^  sender, System::EventArgs^  e) { if(ipc) ipc->CloseApplication(0); };
private: System::Void FormMain_Resize(System::Object^  sender, System::EventArgs^  e) { ResizeHostAppl(); }
private: System::Void FormMain_Move(System::Object^  sender, System::EventArgs^  e) { ResizeHostAppl(); }
private: System::Void checkBoxDock_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
private: System::Void checkBoxBuffered_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {	ReleaseIPC(); };
private: System::Void button1_Click(System::Object^  sender, System::EventArgs^  e)  { ResetPending = true; }
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
private: System::Void checkBoxColors_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {	if(ipc && Connected) ipc->SetIPCMode(0, USHORT(checkBoxColors->Checked ? ipcColors : ipcTemps)); }
private: System::Void StartPixHiddenButton_Click(System::Object^  sender, System::EventArgs^  e){if(!startedPix || startedPix->HasExited) StartPix(); }
};

}

