namespace MeasureArea.UserControls
{
    partial class MeasureAreaDetailsControl
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
            this.groupBox2 = new System.Windows.Forms.GroupBox();
            this.flowLayoutPanel1 = new System.Windows.Forms.FlowLayoutPanel();
            this.groupBox4 = new System.Windows.Forms.GroupBox();
            this.Size_Height = new System.Windows.Forms.NumericUpDown();
            this.Size_Width = new System.Windows.Forms.NumericUpDown();
            this.location_Y = new System.Windows.Forms.NumericUpDown();
            this.location_X = new System.Windows.Forms.NumericUpDown();
            this.ColdSpotCheckbox = new System.Windows.Forms.CheckBox();
            this.HotSpotCheckbox = new System.Windows.Forms.CheckBox();
            this.label10 = new System.Windows.Forms.Label();
            this.label9 = new System.Windows.Forms.Label();
            this.label8 = new System.Windows.Forms.Label();
            this.label7 = new System.Windows.Forms.Label();
            this.label6 = new System.Windows.Forms.Label();
            this.label5 = new System.Windows.Forms.Label();
            this.label4 = new System.Windows.Forms.Label();
            this.panel1 = new System.Windows.Forms.Panel();
            this.label11 = new System.Windows.Forms.Label();
            this.distMin = new System.Windows.Forms.NumericUpDown();
            this.distMax = new System.Windows.Forms.NumericUpDown();
            this.emissivityValu = new System.Windows.Forms.NumericUpDown();
            this.ShowInDigitalDisplayGroup = new System.Windows.Forms.CheckBox();
            this.emissivityChk = new System.Windows.Forms.CheckBox();
            this.modelCombo = new System.Windows.Forms.ComboBox();
            this.shapeCombo = new System.Windows.Forms.ComboBox();
            this.areaNameTxt = new System.Windows.Forms.TextBox();
            this.label3 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.label1 = new System.Windows.Forms.Label();
            this.polygonPointsControl1 = new MeasureArea.UserControls.PolygonPointsControl();
            this.groupBox2.SuspendLayout();
            this.flowLayoutPanel1.SuspendLayout();
            this.groupBox4.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.Size_Height)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.Size_Width)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.location_Y)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.location_X)).BeginInit();
            this.panel1.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.distMin)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.distMax)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.emissivityValu)).BeginInit();
            this.SuspendLayout();
            // 
            // groupBox2
            // 
            this.groupBox2.BackColor = System.Drawing.SystemColors.Menu;
            this.groupBox2.Controls.Add(this.flowLayoutPanel1);
            this.groupBox2.Controls.Add(this.panel1);
            this.groupBox2.Controls.Add(this.emissivityValu);
            this.groupBox2.Controls.Add(this.ShowInDigitalDisplayGroup);
            this.groupBox2.Controls.Add(this.emissivityChk);
            this.groupBox2.Controls.Add(this.modelCombo);
            this.groupBox2.Controls.Add(this.shapeCombo);
            this.groupBox2.Controls.Add(this.areaNameTxt);
            this.groupBox2.Controls.Add(this.label3);
            this.groupBox2.Controls.Add(this.label2);
            this.groupBox2.Controls.Add(this.label1);
            this.groupBox2.Dock = System.Windows.Forms.DockStyle.Fill;
            this.groupBox2.Location = new System.Drawing.Point(0, 0);
            this.groupBox2.Margin = new System.Windows.Forms.Padding(4);
            this.groupBox2.Name = "groupBox2";
            this.groupBox2.Padding = new System.Windows.Forms.Padding(4);
            this.groupBox2.Size = new System.Drawing.Size(484, 476);
            this.groupBox2.TabIndex = 2;
            this.groupBox2.TabStop = false;
            this.groupBox2.Text = "Measure area";
            // 
            // flowLayoutPanel1
            // 
            this.flowLayoutPanel1.BackColor = System.Drawing.SystemColors.Window;
            this.flowLayoutPanel1.Controls.Add(this.groupBox4);
            this.flowLayoutPanel1.Controls.Add(this.polygonPointsControl1);
            this.flowLayoutPanel1.Location = new System.Drawing.Point(0, 252);
            this.flowLayoutPanel1.Margin = new System.Windows.Forms.Padding(4);
            this.flowLayoutPanel1.Name = "flowLayoutPanel1";
            this.flowLayoutPanel1.Size = new System.Drawing.Size(484, 224);
            this.flowLayoutPanel1.TabIndex = 37;
            // 
            // groupBox4
            // 
            this.groupBox4.Controls.Add(this.Size_Height);
            this.groupBox4.Controls.Add(this.Size_Width);
            this.groupBox4.Controls.Add(this.location_Y);
            this.groupBox4.Controls.Add(this.location_X);
            this.groupBox4.Controls.Add(this.ColdSpotCheckbox);
            this.groupBox4.Controls.Add(this.HotSpotCheckbox);
            this.groupBox4.Controls.Add(this.label10);
            this.groupBox4.Controls.Add(this.label9);
            this.groupBox4.Controls.Add(this.label8);
            this.groupBox4.Controls.Add(this.label7);
            this.groupBox4.Controls.Add(this.label6);
            this.groupBox4.Controls.Add(this.label5);
            this.groupBox4.Controls.Add(this.label4);
            this.groupBox4.Location = new System.Drawing.Point(4, 4);
            this.groupBox4.Margin = new System.Windows.Forms.Padding(4);
            this.groupBox4.Name = "groupBox4";
            this.groupBox4.Padding = new System.Windows.Forms.Padding(4);
            this.groupBox4.Size = new System.Drawing.Size(472, 210);
            this.groupBox4.TabIndex = 3;
            this.groupBox4.TabStop = false;
            this.groupBox4.Text = "Position";
            // 
            // Size_Height
            // 
            this.Size_Height.Location = new System.Drawing.Point(191, 127);
            this.Size_Height.Margin = new System.Windows.Forms.Padding(4);
            this.Size_Height.Maximum = new decimal(new int[] {
            10000,
            0,
            0,
            0});
            this.Size_Height.Name = "Size_Height";
            this.Size_Height.Size = new System.Drawing.Size(83, 22);
            this.Size_Height.TabIndex = 34;
            this.Size_Height.Value = new decimal(new int[] {
            1,
            0,
            0,
            0});
            this.Size_Height.ValueChanged += new System.EventHandler(this.Size_Height_ValueChanged);
            // 
            // Size_Width
            // 
            this.Size_Width.Location = new System.Drawing.Point(191, 92);
            this.Size_Width.Margin = new System.Windows.Forms.Padding(4);
            this.Size_Width.Maximum = new decimal(new int[] {
            10000,
            0,
            0,
            0});
            this.Size_Width.Name = "Size_Width";
            this.Size_Width.Size = new System.Drawing.Size(83, 22);
            this.Size_Width.TabIndex = 33;
            this.Size_Width.Value = new decimal(new int[] {
            1,
            0,
            0,
            0});
            this.Size_Width.ValueChanged += new System.EventHandler(this.Size_Width_ValueChanged);
            // 
            // location_Y
            // 
            this.location_Y.Location = new System.Drawing.Point(191, 58);
            this.location_Y.Margin = new System.Windows.Forms.Padding(4);
            this.location_Y.Maximum = new decimal(new int[] {
            10000,
            0,
            0,
            0});
            this.location_Y.Name = "location_Y";
            this.location_Y.Size = new System.Drawing.Size(83, 22);
            this.location_Y.TabIndex = 32;
            this.location_Y.Value = new decimal(new int[] {
            1,
            0,
            0,
            0});
            this.location_Y.ValueChanged += new System.EventHandler(this.location_Y_ValueChanged);
            // 
            // location_X
            // 
            this.location_X.Location = new System.Drawing.Point(191, 25);
            this.location_X.Margin = new System.Windows.Forms.Padding(4);
            this.location_X.Maximum = new decimal(new int[] {
            10000,
            0,
            0,
            0});
            this.location_X.Name = "location_X";
            this.location_X.Size = new System.Drawing.Size(83, 22);
            this.location_X.TabIndex = 31;
            this.location_X.Value = new decimal(new int[] {
            1,
            0,
            0,
            0});
            this.location_X.ValueChanged += new System.EventHandler(this.location_X_ValueChanged);
            // 
            // ColdSpotCheckbox
            // 
            this.ColdSpotCheckbox.AutoSize = true;
            this.ColdSpotCheckbox.Location = new System.Drawing.Point(336, 68);
            this.ColdSpotCheckbox.Margin = new System.Windows.Forms.Padding(4);
            this.ColdSpotCheckbox.Name = "ColdSpotCheckbox";
            this.ColdSpotCheckbox.Size = new System.Drawing.Size(86, 20);
            this.ColdSpotCheckbox.TabIndex = 30;
            this.ColdSpotCheckbox.Text = "Cold spot";
            this.ColdSpotCheckbox.UseVisualStyleBackColor = true;
            this.ColdSpotCheckbox.CheckedChanged += new System.EventHandler(this.ColdSpotCheckbox_CheckedChanged);
            // 
            // HotSpotCheckbox
            // 
            this.HotSpotCheckbox.AutoSize = true;
            this.HotSpotCheckbox.Location = new System.Drawing.Point(336, 28);
            this.HotSpotCheckbox.Margin = new System.Windows.Forms.Padding(4);
            this.HotSpotCheckbox.Name = "HotSpotCheckbox";
            this.HotSpotCheckbox.Size = new System.Drawing.Size(79, 20);
            this.HotSpotCheckbox.TabIndex = 29;
            this.HotSpotCheckbox.Text = "Hot spot";
            this.HotSpotCheckbox.UseVisualStyleBackColor = true;
            this.HotSpotCheckbox.CheckedChanged += new System.EventHandler(this.HotSpotCheckbox_CheckedChanged);
            // 
            // label10
            // 
            this.label10.AutoSize = true;
            this.label10.Location = new System.Drawing.Point(136, 132);
            this.label10.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
            this.label10.Name = "label10";
            this.label10.Size = new System.Drawing.Size(46, 16);
            this.label10.TabIndex = 11;
            this.label10.Text = "Height";
            // 
            // label9
            // 
            this.label9.AutoSize = true;
            this.label9.Location = new System.Drawing.Point(136, 95);
            this.label9.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
            this.label9.Name = "label9";
            this.label9.Size = new System.Drawing.Size(44, 16);
            this.label9.TabIndex = 10;
            this.label9.Text = "Width:";
            // 
            // label8
            // 
            this.label8.AutoSize = true;
            this.label8.Location = new System.Drawing.Point(136, 58);
            this.label8.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
            this.label8.Name = "label8";
            this.label8.Size = new System.Drawing.Size(19, 16);
            this.label8.TabIndex = 9;
            this.label8.Text = "Y:";
            // 
            // label7
            // 
            this.label7.AutoSize = true;
            this.label7.Location = new System.Drawing.Point(136, 28);
            this.label7.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
            this.label7.Name = "label7";
            this.label7.Size = new System.Drawing.Size(18, 16);
            this.label7.TabIndex = 8;
            this.label7.Text = "X:";
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.Location = new System.Drawing.Point(13, 105);
            this.label6.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(36, 16);
            this.label6.TabIndex = 7;
            this.label6.Text = "Size:";
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Location = new System.Drawing.Point(13, 44);
            this.label5.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(54, 16);
            this.label5.TabIndex = 6;
            this.label5.Text = "(Center)";
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(13, 28);
            this.label4.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(64, 16);
            this.label4.TabIndex = 5;
            this.label4.Text = "Location :";
            // 
            // panel1
            // 
            this.panel1.Controls.Add(this.label11);
            this.panel1.Controls.Add(this.distMin);
            this.panel1.Controls.Add(this.distMax);
            this.panel1.Location = new System.Drawing.Point(141, 126);
            this.panel1.Margin = new System.Windows.Forms.Padding(4);
            this.panel1.Name = "panel1";
            this.panel1.Size = new System.Drawing.Size(161, 33);
            this.panel1.TabIndex = 35;
            this.panel1.Visible = false;
            // 
            // label11
            // 
            this.label11.AutoSize = true;
            this.label11.Location = new System.Drawing.Point(133, 7);
            this.label11.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
            this.label11.Name = "label11";
            this.label11.Size = new System.Drawing.Size(27, 16);
            this.label11.TabIndex = 34;
            this.label11.Text = "[\'C]";
            // 
            // distMin
            // 
            this.distMin.DecimalPlaces = 1;
            this.distMin.Location = new System.Drawing.Point(4, 5);
            this.distMin.Margin = new System.Windows.Forms.Padding(4);
            this.distMin.Maximum = new decimal(new int[] {
            10000,
            0,
            0,
            0});
            this.distMin.Name = "distMin";
            this.distMin.Size = new System.Drawing.Size(60, 22);
            this.distMin.TabIndex = 32;
            this.distMin.Value = new decimal(new int[] {
            40,
            0,
            0,
            0});
            this.distMin.ValueChanged += new System.EventHandler(this.distMin_ValueChanged);
            // 
            // distMax
            // 
            this.distMax.DecimalPlaces = 1;
            this.distMax.Location = new System.Drawing.Point(75, 5);
            this.distMax.Margin = new System.Windows.Forms.Padding(4);
            this.distMax.Maximum = new decimal(new int[] {
            10000,
            0,
            0,
            0});
            this.distMax.Name = "distMax";
            this.distMax.Size = new System.Drawing.Size(60, 22);
            this.distMax.TabIndex = 33;
            this.distMax.Value = new decimal(new int[] {
            1,
            0,
            0,
            0});
            this.distMax.ValueChanged += new System.EventHandler(this.distMin_ValueChanged);
            // 
            // emissivityValu
            // 
            this.emissivityValu.DecimalPlaces = 3;
            this.emissivityValu.Enabled = false;
            this.emissivityValu.Location = new System.Drawing.Point(128, 193);
            this.emissivityValu.Margin = new System.Windows.Forms.Padding(4);
            this.emissivityValu.Maximum = new decimal(new int[] {
            1,
            0,
            0,
            0});
            this.emissivityValu.Name = "emissivityValu";
            this.emissivityValu.Size = new System.Drawing.Size(83, 22);
            this.emissivityValu.TabIndex = 14;
            this.emissivityValu.Value = new decimal(new int[] {
            1,
            0,
            0,
            0});
            this.emissivityValu.ValueChanged += new System.EventHandler(this.emissivityValu_ValueChanged);
            // 
            // ShowInDigitalDisplayGroup
            // 
            this.ShowInDigitalDisplayGroup.AutoSize = true;
            this.ShowInDigitalDisplayGroup.Location = new System.Drawing.Point(25, 223);
            this.ShowInDigitalDisplayGroup.Margin = new System.Windows.Forms.Padding(4);
            this.ShowInDigitalDisplayGroup.Name = "ShowInDigitalDisplayGroup";
            this.ShowInDigitalDisplayGroup.Size = new System.Drawing.Size(199, 20);
            this.ShowInDigitalDisplayGroup.TabIndex = 12;
            this.ShowInDigitalDisplayGroup.Text = "Show in digital display group";
            this.ShowInDigitalDisplayGroup.UseVisualStyleBackColor = true;
            this.ShowInDigitalDisplayGroup.CheckedChanged += new System.EventHandler(this.ShowInDigitalDisplayGroup_CheckedChanged);
            // 
            // emissivityChk
            // 
            this.emissivityChk.AutoSize = true;
            this.emissivityChk.Location = new System.Drawing.Point(25, 197);
            this.emissivityChk.Margin = new System.Windows.Forms.Padding(4);
            this.emissivityChk.Name = "emissivityChk";
            this.emissivityChk.Size = new System.Drawing.Size(98, 20);
            this.emissivityChk.TabIndex = 11;
            this.emissivityChk.Text = "Emissivity : ";
            this.emissivityChk.UseVisualStyleBackColor = true;
            this.emissivityChk.CheckedChanged += new System.EventHandler(this.emissivityChk_CheckedChanged);
            // 
            // modelCombo
            // 
            this.modelCombo.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.modelCombo.FormattingEnabled = true;
            this.modelCombo.Items.AddRange(new object[] {
            "Minimum",
            "Maximum",
            "Mean value",
            "Distribution [%]",
            "Area within Limits"});
            this.modelCombo.Location = new System.Drawing.Point(141, 92);
            this.modelCombo.Margin = new System.Windows.Forms.Padding(4);
            this.modelCombo.Name = "modelCombo";
            this.modelCombo.Size = new System.Drawing.Size(255, 24);
            this.modelCombo.TabIndex = 9;
            this.modelCombo.SelectedIndexChanged += new System.EventHandler(this.modelCombo_SelectedIndexChanged);
            // 
            // shapeCombo
            // 
            this.shapeCombo.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.shapeCombo.FormattingEnabled = true;
            this.shapeCombo.Items.AddRange(new object[] {
            "Measure point (1x1)",
            "Measure point (2x2)",
            "Measure point (3x3)",
            "Measure point (5x5)",
            "User def. rectangle",
            "Ellipse",
            "Polygon",
            "Curve (Splines)",
            "Matrix"});
            this.shapeCombo.Location = new System.Drawing.Point(141, 59);
            this.shapeCombo.Margin = new System.Windows.Forms.Padding(4);
            this.shapeCombo.Name = "shapeCombo";
            this.shapeCombo.Size = new System.Drawing.Size(255, 24);
            this.shapeCombo.TabIndex = 8;
            this.shapeCombo.SelectedIndexChanged += new System.EventHandler(this.shapeCombo_SelectedIndexChanged);
            // 
            // areaNameTxt
            // 
            this.areaNameTxt.Location = new System.Drawing.Point(141, 27);
            this.areaNameTxt.Margin = new System.Windows.Forms.Padding(4);
            this.areaNameTxt.Name = "areaNameTxt";
            this.areaNameTxt.Size = new System.Drawing.Size(255, 22);
            this.areaNameTxt.TabIndex = 7;
            this.areaNameTxt.TextChanged += new System.EventHandler(this.areaNameTxt_TextChanged);
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(21, 96);
            this.label3.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(42, 16);
            this.label3.TabIndex = 6;
            this.label3.Text = "Mode";
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(21, 63);
            this.label2.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(47, 16);
            this.label2.TabIndex = 5;
            this.label2.Text = "Shape";
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(21, 31);
            this.label1.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(44, 16);
            this.label1.TabIndex = 4;
            this.label1.Text = "Name";
            // 
            // polygonPointsControl1
            // 
            this.polygonPointsControl1._iPC2Service = null;
            this.polygonPointsControl1.AreaIndex = 0;
            this.polygonPointsControl1.AutoSave = false;
            this.polygonPointsControl1.Location = new System.Drawing.Point(4, 222);
            this.polygonPointsControl1.Margin = new System.Windows.Forms.Padding(4);
            this.polygonPointsControl1.Name = "polygonPointsControl1";
            this.polygonPointsControl1.points = null;
            this.polygonPointsControl1.Size = new System.Drawing.Size(472, 210);
            this.polygonPointsControl1.TabIndex = 4;
            // 
            // MeasureAreaDetailsControl
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(8F, 16F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.Controls.Add(this.groupBox2);
            this.Margin = new System.Windows.Forms.Padding(4);
            this.Name = "MeasureAreaDetailsControl";
            this.Size = new System.Drawing.Size(484, 476);
            this.groupBox2.ResumeLayout(false);
            this.groupBox2.PerformLayout();
            this.flowLayoutPanel1.ResumeLayout(false);
            this.groupBox4.ResumeLayout(false);
            this.groupBox4.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.Size_Height)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.Size_Width)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.location_Y)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.location_X)).EndInit();
            this.panel1.ResumeLayout(false);
            this.panel1.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.distMin)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.distMax)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.emissivityValu)).EndInit();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.GroupBox groupBox2;
        private System.Windows.Forms.NumericUpDown emissivityValu;
        private System.Windows.Forms.CheckBox ShowInDigitalDisplayGroup;
        private System.Windows.Forms.CheckBox emissivityChk;
        private System.Windows.Forms.ComboBox modelCombo;
        private System.Windows.Forms.ComboBox shapeCombo;
        private System.Windows.Forms.TextBox areaNameTxt;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.GroupBox groupBox4;
        private System.Windows.Forms.NumericUpDown Size_Height;
        private System.Windows.Forms.NumericUpDown Size_Width;
        private System.Windows.Forms.NumericUpDown location_Y;
        private System.Windows.Forms.NumericUpDown location_X;
        private System.Windows.Forms.CheckBox ColdSpotCheckbox;
        private System.Windows.Forms.CheckBox HotSpotCheckbox;
        private System.Windows.Forms.Label label10;
        private System.Windows.Forms.Label label9;
        private System.Windows.Forms.Label label8;
        private System.Windows.Forms.Label label7;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.Panel panel1;
        private System.Windows.Forms.Label label11;
        private System.Windows.Forms.NumericUpDown distMin;
        private System.Windows.Forms.NumericUpDown distMax;
        private System.Windows.Forms.FlowLayoutPanel flowLayoutPanel1;
        private PolygonPointsControl polygonPointsControl1;
    }
}
