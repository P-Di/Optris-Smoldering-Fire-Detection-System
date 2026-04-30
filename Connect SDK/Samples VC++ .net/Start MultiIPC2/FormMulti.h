#pragma once

#include "FormImager.h"

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;


namespace IPCSample {

	/// <summary>
	/// Zusammenfassung für FormMulti
	///
	/// Warnung: Wenn Sie den Namen dieser Klasse ändern, müssen Sie auch
	///          die Ressourcendateiname-Eigenschaft für das Tool zur Kompilierung verwalteter Ressourcen ändern,
	///          das allen RESX-Dateien zugewiesen ist, von denen diese Klasse abhängt.
	///          Anderenfalls können die Designer nicht korrekt mit den lokalisierten Ressourcen
	///          arbeiten, die diesem Formular zugewiesen sind.
	/// </summary>
	public ref class FormMulti : public System::Windows::Forms::Form
	{
	public:
		FormMulti(HWND hostHandle);
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
		~FormMulti()
		{
			if (components)
			{
				delete components;
			}
		}
		array<FormImager^> ^formsImager;
		IPC^ ipc;

	private:
		/// <summary>
		/// Erforderliche Designervariable.
		/// </summary>
		System::ComponentModel::Container ^components;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// Erforderliche Methode für die Designerunterstützung.
		/// Der Inhalt der Methode darf nicht mit dem Code-Editor geändert werden.
		/// </summary>
		void InitializeComponent(void)
		{
			System::ComponentModel::ComponentResourceManager^  resources = (gcnew System::ComponentModel::ComponentResourceManager(FormMulti::typeid));
			this->SuspendLayout();
			// 
			// FormMulti
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(400, 480);
			this->Icon = (cli::safe_cast<System::Drawing::Icon^  >(resources->GetObject(L"$this.Icon")));
			this->Name = L"FormMulti";
			this->Text = L"Imager Multi IPC Sample Application";
			this->Load += gcnew System::EventHandler(this, &FormMulti::FormMulti_Load);
			this->FormClosing += gcnew System::Windows::Forms::FormClosingEventHandler(this, &FormMulti::FormMulti_FormClosing);
			this->ResumeLayout(false);

		}
#pragma endregion
	private: System::Void FormMulti_Load(System::Object^  sender, System::EventArgs^  e);
	private: System::Void FormMulti_FormClosing(System::Object^  sender, System::Windows::Forms::FormClosingEventArgs^  e);
	private: System::Void FormImager_OnResize(System::Object^  sender, System::EventArgs^  e);
	};
}
