#pragma once
#include "stdafx.h"
#include "IPC2.h"


namespace StartListAreas {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Collections::Generic;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;	

	/// <summary>
	/// Zusammenfassung für Form1
	///
	/// Warnung: Wenn Sie den Namen dieser Klasse ändern, müssen Sie auch
	///          die Ressourcendateiname-Eigenschaft für das Tool zur Kompilierung verwalteter Ressourcen ändern,
	///          das allen RESX-Dateien zugewiesen ist, von denen diese Klasse abhängt.
	///          Anderenfalls können die Designer nicht korrekt mit den lokalisierten Ressourcen
	///          arbeiten, die diesem Formular zugewiesen sind.
	/// </summary>
	public ref class Form1 : public System::Windows::Forms::Form
	{
	public:
		Form1(void);

	protected:
		~Form1();		

	private:
		HRESULT OnServerStopped(int reason);	
		HRESULT OnInitCompleted(void);
		bool refreshList();
		
	
	private: System::Windows::Forms::TableLayoutPanel^  tableLayoutPanel1;
	protected: 
	private: System::Windows::Forms::Button^  button_remove;
	private: System::Windows::Forms::Button^  button_add;
	private: System::Windows::Forms::ListBox^  listBox1;
	private: System::Windows::Forms::Panel^  panel1;
	private: System::Windows::Forms::TextBox^  textBox1;
	private: System::Windows::Forms::ComboBox^  comboBox1;
	private: System::Windows::Forms::Label^  label3;
	private: System::Windows::Forms::Label^  label2;
	private: System::Windows::Forms::Label^  label1;
	private: System::Windows::Forms::NumericUpDown^  numericUpDown2;
	private: System::Windows::Forms::NumericUpDown^  numericUpDown1;
	private: System::Windows::Forms::Label^  label4;
	private: System::Windows::Forms::Button^  button_addp;


	private:
		/// <summary>
		/// Erforderliche Designervariable.
		/// </summary>
		System::ComponentModel::Container ^components;		
	private: System::Windows::Forms::Button^  button_remp;
	private: System::Windows::Forms::ListBox^  listBox_points;
	private: System::Windows::Forms::Label^  label5;
	private: System::Windows::Forms::NumericUpDown^  numericUpDown3;
	private: System::Windows::Forms::NumericUpDown^  numericUpDown4;
			 
			 IPC^ ipc;
	private: System::Windows::Forms::CheckBox^  checkBox_cs;

	private: System::Windows::Forms::CheckBox^  checkBox_hs;
	private: System::Windows::Forms::Label^  label6;
	private: System::Windows::Forms::Button^  button_log;


			 List<Point> ^area_points;
#pragma region Windows Form Designer generated code
		/// <summary>
		/// Erforderliche Methode für die Designerunterstützung.
		/// Der Inhalt der Methode darf nicht mit dem Code-Editor geändert werden.
		/// </summary>
		void InitializeComponent(void)
		{
			System::ComponentModel::ComponentResourceManager^  resources = (gcnew System::ComponentModel::ComponentResourceManager(Form1::typeid));
			this->tableLayoutPanel1 = (gcnew System::Windows::Forms::TableLayoutPanel());
			this->listBox1 = (gcnew System::Windows::Forms::ListBox());
			this->panel1 = (gcnew System::Windows::Forms::Panel());
			this->label6 = (gcnew System::Windows::Forms::Label());
			this->button_log = (gcnew System::Windows::Forms::Button());
			this->checkBox_cs = (gcnew System::Windows::Forms::CheckBox());
			this->checkBox_hs = (gcnew System::Windows::Forms::CheckBox());
			this->button_remp = (gcnew System::Windows::Forms::Button());
			this->listBox_points = (gcnew System::Windows::Forms::ListBox());
			this->label5 = (gcnew System::Windows::Forms::Label());
			this->numericUpDown3 = (gcnew System::Windows::Forms::NumericUpDown());
			this->numericUpDown4 = (gcnew System::Windows::Forms::NumericUpDown());
			this->label4 = (gcnew System::Windows::Forms::Label());
			this->button_addp = (gcnew System::Windows::Forms::Button());
			this->label3 = (gcnew System::Windows::Forms::Label());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->numericUpDown2 = (gcnew System::Windows::Forms::NumericUpDown());
			this->numericUpDown1 = (gcnew System::Windows::Forms::NumericUpDown());
			this->textBox1 = (gcnew System::Windows::Forms::TextBox());
			this->comboBox1 = (gcnew System::Windows::Forms::ComboBox());
			this->button_add = (gcnew System::Windows::Forms::Button());
			this->button_remove = (gcnew System::Windows::Forms::Button());
			this->tableLayoutPanel1->SuspendLayout();
			this->panel1->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown3))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown4))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown2))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown1))->BeginInit();
			this->SuspendLayout();
			// 
			// tableLayoutPanel1
			// 
			this->tableLayoutPanel1->CellBorderStyle = System::Windows::Forms::TableLayoutPanelCellBorderStyle::Inset;
			this->tableLayoutPanel1->ColumnCount = 2;
			this->tableLayoutPanel1->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Percent, 
				50)));
			this->tableLayoutPanel1->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Percent, 
				50)));
			this->tableLayoutPanel1->Controls->Add(this->listBox1, 1, 1);
			this->tableLayoutPanel1->Controls->Add(this->panel1, 0, 1);
			this->tableLayoutPanel1->Controls->Add(this->button_add, 0, 0);
			this->tableLayoutPanel1->Controls->Add(this->button_remove, 1, 0);
			this->tableLayoutPanel1->Dock = System::Windows::Forms::DockStyle::Fill;
			this->tableLayoutPanel1->Location = System::Drawing::Point(0, 0);
			this->tableLayoutPanel1->Name = L"tableLayoutPanel1";
			this->tableLayoutPanel1->RowCount = 2;
			this->tableLayoutPanel1->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Percent, 8.638743F)));
			this->tableLayoutPanel1->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Percent, 91.36126F)));
			this->tableLayoutPanel1->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Absolute, 20)));
			this->tableLayoutPanel1->Size = System::Drawing::Size(414, 373);
			this->tableLayoutPanel1->TabIndex = 0;
			// 
			// listBox1
			// 
			this->listBox1->Dock = System::Windows::Forms::DockStyle::Fill;
			this->listBox1->FormattingEnabled = true;
			this->listBox1->Location = System::Drawing::Point(211, 38);
			this->listBox1->Name = L"listBox1";
			this->listBox1->Size = System::Drawing::Size(198, 329);
			this->listBox1->TabIndex = 2;
			// 
			// panel1
			// 
			this->panel1->Controls->Add(this->label6);
			this->panel1->Controls->Add(this->button_log);
			this->panel1->Controls->Add(this->checkBox_cs);
			this->panel1->Controls->Add(this->checkBox_hs);
			this->panel1->Controls->Add(this->button_remp);
			this->panel1->Controls->Add(this->listBox_points);
			this->panel1->Controls->Add(this->label5);
			this->panel1->Controls->Add(this->numericUpDown3);
			this->panel1->Controls->Add(this->numericUpDown4);
			this->panel1->Controls->Add(this->label4);
			this->panel1->Controls->Add(this->button_addp);
			this->panel1->Controls->Add(this->label3);
			this->panel1->Controls->Add(this->label2);
			this->panel1->Controls->Add(this->label1);
			this->panel1->Controls->Add(this->numericUpDown2);
			this->panel1->Controls->Add(this->numericUpDown1);
			this->panel1->Controls->Add(this->textBox1);
			this->panel1->Controls->Add(this->comboBox1);
			this->panel1->Dock = System::Windows::Forms::DockStyle::Fill;
			this->panel1->Location = System::Drawing::Point(5, 38);
			this->panel1->Name = L"panel1";
			this->panel1->Size = System::Drawing::Size(198, 330);
			this->panel1->TabIndex = 3;
			// 
			// label6
			// 
			this->label6->AutoSize = true;
			this->label6->Location = System::Drawing::Point(11, 306);
			this->label6->Name = L"label6";
			this->label6->Size = System::Drawing::Size(45, 13);
			this->label6->TabIndex = 17;
			this->label6->Text = L"IPC Log";
			// 
			// button_log
			// 
			this->button_log->Anchor = System::Windows::Forms::AnchorStyles::Left;
			this->button_log->ForeColor = System::Drawing::Color::Red;
			this->button_log->Location = System::Drawing::Point(80, 303);
			this->button_log->Name = L"button_log";
			this->button_log->Size = System::Drawing::Size(108, 23);
			this->button_log->TabIndex = 16;
			this->button_log->Text = L"Display Log";
			this->button_log->UseVisualStyleBackColor = true;
			this->button_log->Click += gcnew System::EventHandler(this, &Form1::button_log_Click);
			// 
			// checkBox_cs
			// 
			this->checkBox_cs->AutoSize = true;
			this->checkBox_cs->Location = System::Drawing::Point(80, 129);
			this->checkBox_cs->Name = L"checkBox_cs";
			this->checkBox_cs->Size = System::Drawing::Size(40, 17);
			this->checkBox_cs->TabIndex = 15;
			this->checkBox_cs->Text = L"CS";
			this->checkBox_cs->UseVisualStyleBackColor = true;
			this->checkBox_cs->CheckedChanged += gcnew System::EventHandler(this, &Form1::checkBox_cs_CheckedChanged);
			// 
			// checkBox_hs
			// 
			this->checkBox_hs->AutoSize = true;
			this->checkBox_hs->Location = System::Drawing::Point(80, 106);
			this->checkBox_hs->Name = L"checkBox_hs";
			this->checkBox_hs->Size = System::Drawing::Size(41, 17);
			this->checkBox_hs->TabIndex = 14;
			this->checkBox_hs->Text = L"HS";
			this->checkBox_hs->UseVisualStyleBackColor = true;
			this->checkBox_hs->CheckedChanged += gcnew System::EventHandler(this, &Form1::checkBox_hs_CheckedChanged);
			// 
			// button_remp
			// 
			this->button_remp->Location = System::Drawing::Point(148, 250);
			this->button_remp->Name = L"button_remp";
			this->button_remp->Size = System::Drawing::Size(40, 23);
			this->button_remp->TabIndex = 13;
			this->button_remp->Text = L"Del";
			this->button_remp->UseVisualStyleBackColor = true;
			this->button_remp->Click += gcnew System::EventHandler(this, &Form1::button_remp_Click);
			// 
			// listBox_points
			// 
			this->listBox_points->FormattingEnabled = true;
			this->listBox_points->Location = System::Drawing::Point(80, 162);
			this->listBox_points->Name = L"listBox_points";
			this->listBox_points->Size = System::Drawing::Size(108, 82);
			this->listBox_points->TabIndex = 12;
			// 
			// label5
			// 
			this->label5->AutoSize = true;
			this->label5->Location = System::Drawing::Point(9, 83);
			this->label5->Name = L"label5";
			this->label5->Size = System::Drawing::Size(27, 13);
			this->label5->TabIndex = 11;
			this->label5->Text = L"Size";
			// 
			// numericUpDown3
			// 
			this->numericUpDown3->Location = System::Drawing::Point(80, 80);
			this->numericUpDown3->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {480, 0, 0, 0});
			this->numericUpDown3->Name = L"numericUpDown3";
			this->numericUpDown3->Size = System::Drawing::Size(51, 20);
			this->numericUpDown3->TabIndex = 10;
			// 
			// numericUpDown4
			// 
			this->numericUpDown4->Location = System::Drawing::Point(137, 80);
			this->numericUpDown4->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {640, 0, 0, 0});
			this->numericUpDown4->Name = L"numericUpDown4";
			this->numericUpDown4->Size = System::Drawing::Size(51, 20);
			this->numericUpDown4->TabIndex = 9;
			// 
			// label4
			// 
			this->label4->AutoSize = true;
			this->label4->Location = System::Drawing::Point(11, 164);
			this->label4->Name = L"label4";
			this->label4->Size = System::Drawing::Size(36, 13);
			this->label4->TabIndex = 8;
			this->label4->Text = L"Points";
			// 
			// button_addp
			// 
			this->button_addp->Location = System::Drawing::Point(80, 250);
			this->button_addp->Name = L"button_addp";
			this->button_addp->Size = System::Drawing::Size(62, 23);
			this->button_addp->TabIndex = 7;
			this->button_addp->Text = L"Add Point";
			this->button_addp->UseVisualStyleBackColor = true;
			this->button_addp->Click += gcnew System::EventHandler(this, &Form1::button_addp_Click);
			// 
			// label3
			// 
			this->label3->AutoSize = true;
			this->label3->Location = System::Drawing::Point(9, 7);
			this->label3->Name = L"label3";
			this->label3->Size = System::Drawing::Size(35, 13);
			this->label3->TabIndex = 6;
			this->label3->Text = L"Name";
			// 
			// label2
			// 
			this->label2->AutoSize = true;
			this->label2->Location = System::Drawing::Point(9, 34);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(38, 13);
			this->label2->TabIndex = 5;
			this->label2->Text = L"Shape";
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Location = System::Drawing::Point(9, 58);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(44, 13);
			this->label1->TabIndex = 4;
			this->label1->Text = L"Position";
			// 
			// numericUpDown2
			// 
			this->numericUpDown2->Location = System::Drawing::Point(137, 56);
			this->numericUpDown2->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {480, 0, 0, 0});
			this->numericUpDown2->Name = L"numericUpDown2";
			this->numericUpDown2->Size = System::Drawing::Size(51, 20);
			this->numericUpDown2->TabIndex = 3;
			// 
			// numericUpDown1
			// 
			this->numericUpDown1->Location = System::Drawing::Point(80, 56);
			this->numericUpDown1->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {640, 0, 0, 0});
			this->numericUpDown1->Name = L"numericUpDown1";
			this->numericUpDown1->Size = System::Drawing::Size(51, 20);
			this->numericUpDown1->TabIndex = 2;
			// 
			// textBox1
			// 
			this->textBox1->Location = System::Drawing::Point(80, 3);
			this->textBox1->Name = L"textBox1";
			this->textBox1->Size = System::Drawing::Size(108, 20);
			this->textBox1->TabIndex = 1;
			// 
			// comboBox1
			// 
			this->comboBox1->FormattingEnabled = true;
			this->comboBox1->Items->AddRange(gcnew cli::array< System::Object^  >(9) {L"Off", L"1x1", L"2x2", L"3x3", L"5x5", L"UserRect", L"Ellipse", 
				L"Polygon", L"Spline"});
			this->comboBox1->Location = System::Drawing::Point(80, 29);
			this->comboBox1->Name = L"comboBox1";
			this->comboBox1->Size = System::Drawing::Size(108, 21);
			this->comboBox1->TabIndex = 0;
			this->comboBox1->SelectedIndexChanged += gcnew System::EventHandler(this, &Form1::comboBox1_SelectedIndexChanged);
			// 
			// button_add
			// 
			this->button_add->Anchor = System::Windows::Forms::AnchorStyles::Left;
			this->button_add->Location = System::Drawing::Point(5, 6);
			this->button_add->Name = L"button_add";
			this->button_add->Size = System::Drawing::Size(75, 23);
			this->button_add->TabIndex = 0;
			this->button_add->Text = L"Add";
			this->button_add->UseVisualStyleBackColor = true;
			this->button_add->Click += gcnew System::EventHandler(this, &Form1::button_add_Click);
			// 
			// button_remove
			// 
			this->button_remove->Anchor = System::Windows::Forms::AnchorStyles::Left;
			this->button_remove->Location = System::Drawing::Point(211, 6);
			this->button_remove->Name = L"button_remove";
			this->button_remove->Size = System::Drawing::Size(75, 23);
			this->button_remove->TabIndex = 1;
			this->button_remove->Text = L"Remove";
			this->button_remove->UseVisualStyleBackColor = true;
			this->button_remove->Click += gcnew System::EventHandler(this, &Form1::button_remove_Click);
			// 
			// Form1
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(414, 373);
			this->Controls->Add(this->tableLayoutPanel1);
			this->Icon = (cli::safe_cast<System::Drawing::Icon^  >(resources->GetObject(L"$this.Icon")));
			this->MaximizeBox = false;
			this->MinimizeBox = false;
			this->Name = L"Form1";
			this->Text = L"Form1";
			this->tableLayoutPanel1->ResumeLayout(false);
			this->panel1->ResumeLayout(false);
			this->panel1->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown3))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown4))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown2))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown1))->EndInit();
			this->ResumeLayout(false);

		}

#pragma endregion

private: System::Void button_add_Click(System::Object^  sender, System::EventArgs^  e);
private: System::Void button_addp_Click(System::Object^  sender, System::EventArgs^  e);
private: System::Void button_remove_Click(System::Object^  sender, System::EventArgs^  e);		 
private: System::Void button_remp_Click(System::Object^  sender, System::EventArgs^  e);		 
private: System::Void checkBox_cs_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
private: System::Void checkBox_hs_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
private: System::Void comboBox1_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e);
private: System::Void button_log_Click(System::Object^  sender, System::EventArgs^  e);
};
}

