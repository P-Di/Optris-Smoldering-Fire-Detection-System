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
            this.labelFlag = new System.Windows.Forms.Label();
            this.labelFlag1 = new System.Windows.Forms.Label();
            this.labelFrameCounter = new System.Windows.Forms.Label();
            this.textBoxInstanceName = new System.Windows.Forms.TextBox();
            this.labelInstanceName = new System.Windows.Forms.Label();
            this.label1 = new System.Windows.Forms.Label();
            this.timer1 = new System.Windows.Forms.Timer(this.components);
            this.timer2 = new System.Windows.Forms.Timer(this.components);
            this.btnStream = new System.Windows.Forms.Button();
            this.lblStatus = new System.Windows.Forms.Label();
            this.labelPort = new System.Windows.Forms.Label();
            this.numericUpDownPort = new System.Windows.Forms.NumericUpDown();
            this.labelLink = new System.Windows.Forms.Label();
            this.listBoxIP = new System.Windows.Forms.ListBox();
            this.labelLinkRTSP = new System.Windows.Forms.Label();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.numericUpDownPort)).BeginInit();
            this.SuspendLayout();
            // 
            // pictureBox
            // 
            this.pictureBox.Location = new System.Drawing.Point(471, 15);
            this.pictureBox.Margin = new System.Windows.Forms.Padding(4);
            this.pictureBox.Name = "pictureBox";
            this.pictureBox.Size = new System.Drawing.Size(259, 149);
            this.pictureBox.TabIndex = 84;
            this.pictureBox.TabStop = false;
            this.pictureBox.Paint += new System.Windows.Forms.PaintEventHandler(this.pictureBox_Paint);
            // 
            // labelFlag
            // 
            this.labelFlag.AutoSize = true;
            this.labelFlag.Location = new System.Drawing.Point(58, 104);
            this.labelFlag.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
            this.labelFlag.Name = "labelFlag";
            this.labelFlag.Size = new System.Drawing.Size(40, 17);
            this.labelFlag.TabIndex = 92;
            this.labelFlag.Text = "open";
            // 
            // labelFlag1
            // 
            this.labelFlag1.AutoSize = true;
            this.labelFlag1.Location = new System.Drawing.Point(13, 104);
            this.labelFlag1.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
            this.labelFlag1.Name = "labelFlag1";
            this.labelFlag1.Size = new System.Drawing.Size(39, 17);
            this.labelFlag1.TabIndex = 91;
            this.labelFlag1.Text = "Flag:";
            // 
            // labelFrameCounter
            // 
            this.labelFrameCounter.AutoSize = true;
            this.labelFrameCounter.Location = new System.Drawing.Point(12, 77);
            this.labelFrameCounter.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
            this.labelFrameCounter.Name = "labelFrameCounter";
            this.labelFrameCounter.Size = new System.Drawing.Size(104, 17);
            this.labelFrameCounter.TabIndex = 89;
            this.labelFrameCounter.Text = "Frame counter:";
            // 
            // textBoxInstanceName
            // 
            this.textBoxInstanceName.Location = new System.Drawing.Point(127, 15);
            this.textBoxInstanceName.Margin = new System.Windows.Forms.Padding(4);
            this.textBoxInstanceName.Name = "textBoxInstanceName";
            this.textBoxInstanceName.Size = new System.Drawing.Size(235, 22);
            this.textBoxInstanceName.TabIndex = 88;
            // 
            // labelInstanceName
            // 
            this.labelInstanceName.AutoSize = true;
            this.labelInstanceName.Location = new System.Drawing.Point(12, 18);
            this.labelInstanceName.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
            this.labelInstanceName.Name = "labelInstanceName";
            this.labelInstanceName.Size = new System.Drawing.Size(104, 17);
            this.labelInstanceName.TabIndex = 87;
            this.labelInstanceName.Text = "Instance name:";
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(12, 52);
            this.label1.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(23, 17);
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
            // btnStream
            // 
            this.btnStream.Location = new System.Drawing.Point(15, 166);
            this.btnStream.Margin = new System.Windows.Forms.Padding(4);
            this.btnStream.Name = "btnStream";
            this.btnStream.Size = new System.Drawing.Size(100, 28);
            this.btnStream.TabIndex = 93;
            this.btnStream.Text = "Start stream";
            this.btnStream.UseVisualStyleBackColor = true;
            this.btnStream.Click += new System.EventHandler(this.btnStream_Click);
            // 
            // lblStatus
            // 
            this.lblStatus.AutoSize = true;
            this.lblStatus.Location = new System.Drawing.Point(125, 172);
            this.lblStatus.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
            this.lblStatus.Name = "lblStatus";
            this.lblStatus.Size = new System.Drawing.Size(0, 17);
            this.lblStatus.TabIndex = 94;
            // 
            // labelPort
            // 
            this.labelPort.AutoSize = true;
            this.labelPort.Location = new System.Drawing.Point(13, 139);
            this.labelPort.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
            this.labelPort.Name = "labelPort";
            this.labelPort.Size = new System.Drawing.Size(38, 17);
            this.labelPort.TabIndex = 91;
            this.labelPort.Text = "Port:";
            // 
            // numericUpDownPort
            // 
            this.numericUpDownPort.Location = new System.Drawing.Point(53, 137);
            this.numericUpDownPort.Maximum = new decimal(new int[] {
            60000,
            0,
            0,
            0});
            this.numericUpDownPort.Minimum = new decimal(new int[] {
            1,
            0,
            0,
            0});
            this.numericUpDownPort.Name = "numericUpDownPort";
            this.numericUpDownPort.Size = new System.Drawing.Size(72, 22);
            this.numericUpDownPort.TabIndex = 95;
            this.numericUpDownPort.Value = new decimal(new int[] {
            6000,
            0,
            0,
            0});
            this.numericUpDownPort.ValueChanged += new System.EventHandler(this.numericUpDownPort_ValueChanged);
            // 
            // labelLink
            // 
            this.labelLink.AutoSize = true;
            this.labelLink.Location = new System.Drawing.Point(9, 330);
            this.labelLink.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
            this.labelLink.Name = "labelLink";
            this.labelLink.Size = new System.Drawing.Size(207, 17);
            this.labelLink.TabIndex = 91;
            this.labelLink.Text = "Link (click to copy to clipboard):";
            // 
            // listBoxIP
            // 
            this.listBoxIP.FormattingEnabled = true;
            this.listBoxIP.ItemHeight = 16;
            this.listBoxIP.Location = new System.Drawing.Point(12, 204);
            this.listBoxIP.Name = "listBoxIP";
            this.listBoxIP.Size = new System.Drawing.Size(437, 116);
            this.listBoxIP.TabIndex = 98;
            this.listBoxIP.Click += new System.EventHandler(this.listBoxIP_Click);
            // 
            // labelLinkRTSP
            // 
            this.labelLinkRTSP.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            this.labelLinkRTSP.Location = new System.Drawing.Point(224, 329);
            this.labelLinkRTSP.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
            this.labelLinkRTSP.Name = "labelLinkRTSP";
            this.labelLinkRTSP.Padding = new System.Windows.Forms.Padding(2);
            this.labelLinkRTSP.Size = new System.Drawing.Size(225, 25);
            this.labelLinkRTSP.TabIndex = 91;
            this.labelLinkRTSP.Tag = "";
            this.labelLinkRTSP.Text = "rtsp://";
            this.labelLinkRTSP.Click += new System.EventHandler(this.labelLinkRTSP_Click);
            // 
            // FormMain
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(8F, 16F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(729, 363);
            this.Controls.Add(this.listBoxIP);
            this.Controls.Add(this.numericUpDownPort);
            this.Controls.Add(this.lblStatus);
            this.Controls.Add(this.btnStream);
            this.Controls.Add(this.pictureBox);
            this.Controls.Add(this.labelFlag);
            this.Controls.Add(this.labelLinkRTSP);
            this.Controls.Add(this.labelLink);
            this.Controls.Add(this.labelPort);
            this.Controls.Add(this.labelFlag1);
            this.Controls.Add(this.labelFrameCounter);
            this.Controls.Add(this.textBoxInstanceName);
            this.Controls.Add(this.labelInstanceName);
            this.Controls.Add(this.label1);
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.Margin = new System.Windows.Forms.Padding(4);
            this.Name = "FormMain";
            this.Text = "Imager IPC RSTP Sample C# Application";
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.FormMain_FormClosing);
            this.Load += new System.EventHandler(this.FormMain_Load);
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.numericUpDownPort)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.PictureBox pictureBox;
        private System.Windows.Forms.Label labelFlag;
        private System.Windows.Forms.Label labelFlag1;
        private System.Windows.Forms.Label labelFrameCounter;
        private System.Windows.Forms.TextBox textBoxInstanceName;
        private System.Windows.Forms.Label labelInstanceName;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Timer timer1;
        private System.Windows.Forms.Timer timer2;
        private System.Windows.Forms.Button btnStream;
        private System.Windows.Forms.Label lblStatus;
        private System.Windows.Forms.Label labelPort;
        private System.Windows.Forms.NumericUpDown numericUpDownPort;
        private System.Windows.Forms.Label labelLink;
        private System.Windows.Forms.ListBox listBoxIP;
        private System.Windows.Forms.Label labelLinkRTSP;
    }
}

