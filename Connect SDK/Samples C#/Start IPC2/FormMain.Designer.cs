namespace IPC2
{
    partial class FormMain
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

        #region Vom Windows Form-Designer generierter Code

        /// <summary>
        /// Erforderliche Methode für die Designerunterstützung.
        /// Der Inhalt der Methode darf nicht mit dem Code-Editor geändert werden.
        /// </summary>
        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(FormMain));
            this.pictureBox = new System.Windows.Forms.PictureBox();
            this.buttonFlagRenew = new System.Windows.Forms.Button();
            this.labelFlag = new System.Windows.Forms.Label();
            this.labelFlag1 = new System.Windows.Forms.Label();
            this.labelPIF = new System.Windows.Forms.Label();
            this.labelFrameCounter = new System.Windows.Forms.Label();
            this.textBoxInstanceName = new System.Windows.Forms.TextBox();
            this.labelInstanceName = new System.Windows.Forms.Label();
            this.labelTempTarget = new System.Windows.Forms.Label();
            this.label1 = new System.Windows.Forms.Label();
            this.timer1 = new System.Windows.Forms.Timer(this.components);
            this.timer2 = new System.Windows.Forms.Timer(this.components);
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox)).BeginInit();
            this.SuspendLayout();
            // 
            // pictureBox
            // 
            this.pictureBox.Location = new System.Drawing.Point(278, 12);
            this.pictureBox.Name = "pictureBox";
            this.pictureBox.Size = new System.Drawing.Size(194, 121);
            this.pictureBox.TabIndex = 84;
            this.pictureBox.TabStop = false;
            this.pictureBox.Paint += new System.Windows.Forms.PaintEventHandler(this.pictureBox_Paint);
            // 
            // buttonFlagRenew
            // 
            this.buttonFlagRenew.Location = new System.Drawing.Point(117, 125);
            this.buttonFlagRenew.Name = "buttonFlagRenew";
            this.buttonFlagRenew.Size = new System.Drawing.Size(100, 23);
            this.buttonFlagRenew.TabIndex = 86;
            this.buttonFlagRenew.Text = "Renew";
            this.buttonFlagRenew.UseVisualStyleBackColor = true;
            this.buttonFlagRenew.Click += new System.EventHandler(this.buttonFlagRenew_Click);
            // 
            // labelFlag
            // 
            this.labelFlag.AutoSize = true;
            this.labelFlag.Location = new System.Drawing.Point(49, 130);
            this.labelFlag.Name = "labelFlag";
            this.labelFlag.Size = new System.Drawing.Size(31, 13);
            this.labelFlag.TabIndex = 92;
            this.labelFlag.Text = "open";
            // 
            // labelFlag1
            // 
            this.labelFlag1.AutoSize = true;
            this.labelFlag1.Location = new System.Drawing.Point(9, 130);
            this.labelFlag1.Name = "labelFlag1";
            this.labelFlag1.Size = new System.Drawing.Size(30, 13);
            this.labelFlag1.TabIndex = 91;
            this.labelFlag1.Text = "Flag:";
            // 
            // labelPIF
            // 
            this.labelPIF.AutoSize = true;
            this.labelPIF.Location = new System.Drawing.Point(9, 108);
            this.labelPIF.Name = "labelPIF";
            this.labelPIF.Size = new System.Drawing.Size(26, 13);
            this.labelPIF.TabIndex = 90;
            this.labelPIF.Text = "PIF:";
            // 
            // labelFrameCounter
            // 
            this.labelFrameCounter.AutoSize = true;
            this.labelFrameCounter.Location = new System.Drawing.Point(9, 86);
            this.labelFrameCounter.Name = "labelFrameCounter";
            this.labelFrameCounter.Size = new System.Drawing.Size(123, 13);
            this.labelFrameCounter.TabIndex = 89;
            this.labelFrameCounter.Text = "Frame counter HW/SW:";
            // 
            // textBoxInstanceName
            // 
            this.textBoxInstanceName.Location = new System.Drawing.Point(95, 12);
            this.textBoxInstanceName.Name = "textBoxInstanceName";
            this.textBoxInstanceName.Size = new System.Drawing.Size(177, 20);
            this.textBoxInstanceName.TabIndex = 88;
            // 
            // labelInstanceName
            // 
            this.labelInstanceName.AutoSize = true;
            this.labelInstanceName.Location = new System.Drawing.Point(9, 15);
            this.labelInstanceName.Name = "labelInstanceName";
            this.labelInstanceName.Size = new System.Drawing.Size(80, 13);
            this.labelInstanceName.TabIndex = 87;
            this.labelInstanceName.Text = "Instance name:";
            // 
            // labelTempTarget
            // 
            this.labelTempTarget.AutoSize = true;
            this.labelTempTarget.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.labelTempTarget.Location = new System.Drawing.Point(9, 64);
            this.labelTempTarget.Name = "labelTempTarget";
            this.labelTempTarget.Size = new System.Drawing.Size(71, 13);
            this.labelTempTarget.TabIndex = 85;
            this.labelTempTarget.Text = "Target-Temp:";
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(9, 42);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(16, 13);
            this.label1.TabIndex = 83;
            this.label1.Text = "---";
            // 
            // timer1
            // 
            this.timer1.Tick += new System.EventHandler(this.timer1_Tick);
            // 
            // timer2
            // 
            this.timer2.Interval = 200;
            this.timer2.Tick += new System.EventHandler(this.timer2_Tick);
            // 
            // FormMain
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(484, 207);
            this.Controls.Add(this.pictureBox);
            this.Controls.Add(this.buttonFlagRenew);
            this.Controls.Add(this.labelFlag);
            this.Controls.Add(this.labelFlag1);
            this.Controls.Add(this.labelPIF);
            this.Controls.Add(this.labelFrameCounter);
            this.Controls.Add(this.textBoxInstanceName);
            this.Controls.Add(this.labelInstanceName);
            this.Controls.Add(this.labelTempTarget);
            this.Controls.Add(this.label1);
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.Name = "FormMain";
            this.Text = "Imager IPC Sample C# Application";
            this.Load += new System.EventHandler(this.FormMain_Load);
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.FormMain_FormClosing);
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.PictureBox pictureBox;
        private System.Windows.Forms.Button buttonFlagRenew;
        private System.Windows.Forms.Label labelFlag;
        private System.Windows.Forms.Label labelFlag1;
        private System.Windows.Forms.Label labelPIF;
        private System.Windows.Forms.Label labelFrameCounter;
        private System.Windows.Forms.TextBox textBoxInstanceName;
        private System.Windows.Forms.Label labelInstanceName;
        private System.Windows.Forms.Label labelTempTarget;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Timer timer1;
        private System.Windows.Forms.Timer timer2;
    }
}

