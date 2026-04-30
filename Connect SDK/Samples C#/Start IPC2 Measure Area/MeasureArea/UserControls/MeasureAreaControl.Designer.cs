namespace MeasureArea.UserControls
{
    partial class MeasureAreaControl
    {
        /// <summary> 
        /// Erforderliche Designervariable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary> 
        /// Verwendete Ressourcen bereinigen.
        /// </summary>
        /// <param name="disposing">True, wenn verwaltete Ressourcen gelöscht werden sollen; andernfalls False.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Vom Komponenten-Designer generierter Code

        /// <summary> 
        /// Erforderliche Methode für die Designerunterstützung. 
        /// Der Inhalt der Methode darf nicht mit dem Code-Editor geändert werden.
        /// </summary>
        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.moveDownBtn = new System.Windows.Forms.Button();
            this.moveUpBtn = new System.Windows.Forms.Button();
            this.button3 = new System.Windows.Forms.Button();
            this.button1 = new System.Windows.Forms.Button();
            this.listBox1 = new System.Windows.Forms.ListBox();
            this.autoApplyChk = new System.Windows.Forms.CheckBox();
            this.timer2 = new System.Windows.Forms.Timer(this.components);
            this.connectingLbl = new System.Windows.Forms.Label();
            this.label1 = new System.Windows.Forms.Label();
            this.textBoxInstanceName = new System.Windows.Forms.TextBox();
            this.measureAreaDetailsControl1 = new MeasureArea.UserControls.MeasureAreaDetailsControl();
            this.button2 = new System.Windows.Forms.Button();
            this.panel1 = new System.Windows.Forms.Panel();
            this.groupBox1.SuspendLayout();
            this.panel1.SuspendLayout();
            this.SuspendLayout();
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.moveDownBtn);
            this.groupBox1.Controls.Add(this.moveUpBtn);
            this.groupBox1.Controls.Add(this.button3);
            this.groupBox1.Controls.Add(this.button1);
            this.groupBox1.Controls.Add(this.listBox1);
            this.groupBox1.Enabled = false;
            this.groupBox1.Location = new System.Drawing.Point(4, 5);
            this.groupBox1.Margin = new System.Windows.Forms.Padding(4);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Padding = new System.Windows.Forms.Padding(4);
            this.groupBox1.Size = new System.Drawing.Size(405, 469);
            this.groupBox1.TabIndex = 0;
            this.groupBox1.TabStop = false;
            // 
            // moveDownBtn
            // 
            this.moveDownBtn.Location = new System.Drawing.Point(259, 160);
            this.moveDownBtn.Margin = new System.Windows.Forms.Padding(4);
            this.moveDownBtn.Name = "moveDownBtn";
            this.moveDownBtn.Size = new System.Drawing.Size(127, 31);
            this.moveDownBtn.TabIndex = 5;
            this.moveDownBtn.Text = "Down";
            this.moveDownBtn.UseVisualStyleBackColor = true;
            this.moveDownBtn.Click += new System.EventHandler(this.button5_Click);
            // 
            // moveUpBtn
            // 
            this.moveUpBtn.Location = new System.Drawing.Point(259, 123);
            this.moveUpBtn.Margin = new System.Windows.Forms.Padding(4);
            this.moveUpBtn.Name = "moveUpBtn";
            this.moveUpBtn.Size = new System.Drawing.Size(127, 30);
            this.moveUpBtn.TabIndex = 4;
            this.moveUpBtn.Text = "Up";
            this.moveUpBtn.UseVisualStyleBackColor = true;
            this.moveUpBtn.Click += new System.EventHandler(this.button4_Click);
            // 
            // button3
            // 
            this.button3.Location = new System.Drawing.Point(259, 73);
            this.button3.Margin = new System.Windows.Forms.Padding(4);
            this.button3.Name = "button3";
            this.button3.Size = new System.Drawing.Size(127, 42);
            this.button3.TabIndex = 3;
            this.button3.Text = "Remove";
            this.button3.UseVisualStyleBackColor = true;
            this.button3.Click += new System.EventHandler(this.button3_Click);
            // 
            // button1
            // 
            this.button1.Location = new System.Drawing.Point(259, 23);
            this.button1.Margin = new System.Windows.Forms.Padding(4);
            this.button1.Name = "button1";
            this.button1.Size = new System.Drawing.Size(127, 42);
            this.button1.TabIndex = 1;
            this.button1.Text = "Add";
            this.button1.UseVisualStyleBackColor = true;
            this.button1.Click += new System.EventHandler(this.button1_Click);
            // 
            // listBox1
            // 
            this.listBox1.FormattingEnabled = true;
            this.listBox1.ItemHeight = 16;
            this.listBox1.Location = new System.Drawing.Point(20, 23);
            this.listBox1.Margin = new System.Windows.Forms.Padding(4);
            this.listBox1.Name = "listBox1";
            this.listBox1.Size = new System.Drawing.Size(229, 340);
            this.listBox1.TabIndex = 0;
            this.listBox1.SelectedIndexChanged += new System.EventHandler(this.listBox1_SelectedIndexChanged);
            // 
            // autoApplyChk
            // 
            this.autoApplyChk.AutoSize = true;
            this.autoApplyChk.Checked = true;
            this.autoApplyChk.CheckState = System.Windows.Forms.CheckState.Checked;
            this.autoApplyChk.Location = new System.Drawing.Point(3, 21);
            this.autoApplyChk.Margin = new System.Windows.Forms.Padding(4);
            this.autoApplyChk.Name = "autoApplyChk";
            this.autoApplyChk.Size = new System.Drawing.Size(93, 20);
            this.autoApplyChk.TabIndex = 11;
            this.autoApplyChk.Text = "Auto apply";
            this.autoApplyChk.UseVisualStyleBackColor = true;
            this.autoApplyChk.CheckedChanged += new System.EventHandler(this.checkBox1_CheckedChanged);
            // 
            // timer2
            // 
            this.timer2.Tick += new System.EventHandler(this.timer2_Tick);
            // 
            // connectingLbl
            // 
            this.connectingLbl.Location = new System.Drawing.Point(4, 525);
            this.connectingLbl.Name = "connectingLbl";
            this.connectingLbl.Size = new System.Drawing.Size(282, 23);
            this.connectingLbl.TabIndex = 6;
            this.connectingLbl.TextChanged += new System.EventHandler(this.connectingLbl_TextChanged);
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(4, 490);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(103, 16);
            this.label1.TabIndex = 12;
            this.label1.Text = "Instance name : ";
            // 
            // textBoxInstanceName
            // 
            this.textBoxInstanceName.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.textBoxInstanceName.Location = new System.Drawing.Point(122, 486);
            this.textBoxInstanceName.Name = "textBoxInstanceName";
            this.textBoxInstanceName.Size = new System.Drawing.Size(182, 26);
            this.textBoxInstanceName.TabIndex = 13;
            this.textBoxInstanceName.TextChanged += new System.EventHandler(this.textBoxInstanceName_TextChanged);
            // 
            // measureAreaDetailsControl1
            // 
            this.measureAreaDetailsControl1.AutoApply = false;
            this.measureAreaDetailsControl1.Enabled = false;
            this.measureAreaDetailsControl1.frameMaxHeight = 0;
            this.measureAreaDetailsControl1.frameMaxWidth = 0;
            this.measureAreaDetailsControl1.Location = new System.Drawing.Point(417, 5);
            this.measureAreaDetailsControl1.Margin = new System.Windows.Forms.Padding(5);
            this.measureAreaDetailsControl1.model = null;
            this.measureAreaDetailsControl1.Name = "measureAreaDetailsControl1";
            this.measureAreaDetailsControl1.Size = new System.Drawing.Size(476, 469);
            this.measureAreaDetailsControl1.TabIndex = 9;
            // 
            // button2
            // 
            this.button2.Enabled = false;
            this.button2.Location = new System.Drawing.Point(228, 7);
            this.button2.Name = "button2";
            this.button2.Size = new System.Drawing.Size(245, 47);
            this.button2.TabIndex = 14;
            this.button2.Text = "Apply";
            this.button2.UseVisualStyleBackColor = true;
            this.button2.Click += new System.EventHandler(this.button2_Click);
            // 
            // panel1
            // 
            this.panel1.Controls.Add(this.autoApplyChk);
            this.panel1.Controls.Add(this.button2);
            this.panel1.Location = new System.Drawing.Point(417, 482);
            this.panel1.Name = "panel1";
            this.panel1.Size = new System.Drawing.Size(476, 60);
            this.panel1.TabIndex = 15;
            // 
            // MeasureAreaControl
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(8F, 16F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.SystemColors.Window;
            this.Controls.Add(this.panel1);
            this.Controls.Add(this.textBoxInstanceName);
            this.Controls.Add(this.connectingLbl);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.measureAreaDetailsControl1);
            this.Controls.Add(this.groupBox1);
            this.Margin = new System.Windows.Forms.Padding(4);
            this.Name = "MeasureAreaControl";
            this.Size = new System.Drawing.Size(897, 558);
            this.Load += new System.EventHandler(this.MeasureAreaControl_Load);
            this.groupBox1.ResumeLayout(false);
            this.panel1.ResumeLayout(false);
            this.panel1.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.Button moveDownBtn;
        private System.Windows.Forms.Button moveUpBtn;
        private System.Windows.Forms.Button button3;
        private System.Windows.Forms.Button button1;
        private System.Windows.Forms.ListBox listBox1;
        private System.Windows.Forms.Timer timer2;
        private MeasureAreaDetailsControl measureAreaDetailsControl1;
        private System.Windows.Forms.CheckBox autoApplyChk;
        private System.Windows.Forms.Label connectingLbl;
        private System.Windows.Forms.TextBox textBoxInstanceName;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Button button2;
        private System.Windows.Forms.Panel panel1;
    }
}
