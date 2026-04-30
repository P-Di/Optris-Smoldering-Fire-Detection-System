namespace MeasureArea.UserControls
{
    partial class PolygonPointsControl
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
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.downBtn = new System.Windows.Forms.Button();
            this.upBtn = new System.Windows.Forms.Button();
            this.listBox1 = new System.Windows.Forms.ListBox();
            this.removeBtn = new System.Windows.Forms.Button();
            this.pointY = new System.Windows.Forms.NumericUpDown();
            this.addBtn = new System.Windows.Forms.Button();
            this.label2 = new System.Windows.Forms.Label();
            this.pointX = new System.Windows.Forms.NumericUpDown();
            this.label1 = new System.Windows.Forms.Label();
            this.groupBox1.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.pointY)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.pointX)).BeginInit();
            this.SuspendLayout();
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.downBtn);
            this.groupBox1.Controls.Add(this.upBtn);
            this.groupBox1.Controls.Add(this.listBox1);
            this.groupBox1.Controls.Add(this.removeBtn);
            this.groupBox1.Controls.Add(this.pointY);
            this.groupBox1.Controls.Add(this.addBtn);
            this.groupBox1.Controls.Add(this.label2);
            this.groupBox1.Controls.Add(this.pointX);
            this.groupBox1.Controls.Add(this.label1);
            this.groupBox1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.groupBox1.Location = new System.Drawing.Point(0, 0);
            this.groupBox1.Margin = new System.Windows.Forms.Padding(4);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Padding = new System.Windows.Forms.Padding(4);
            this.groupBox1.Size = new System.Drawing.Size(445, 215);
            this.groupBox1.TabIndex = 0;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "Polygon points";
            // 
            // downBtn
            // 
            this.downBtn.Location = new System.Drawing.Point(329, 91);
            this.downBtn.Margin = new System.Windows.Forms.Padding(4);
            this.downBtn.Name = "downBtn";
            this.downBtn.Size = new System.Drawing.Size(100, 28);
            this.downBtn.TabIndex = 8;
            this.downBtn.Text = "Down";
            this.downBtn.UseVisualStyleBackColor = true;
            this.downBtn.Click += new System.EventHandler(this.downBtn_Click);
            // 
            // upBtn
            // 
            this.upBtn.Location = new System.Drawing.Point(329, 55);
            this.upBtn.Margin = new System.Windows.Forms.Padding(4);
            this.upBtn.Name = "upBtn";
            this.upBtn.Size = new System.Drawing.Size(100, 28);
            this.upBtn.TabIndex = 7;
            this.upBtn.Text = "Up";
            this.upBtn.UseVisualStyleBackColor = true;
            this.upBtn.Click += new System.EventHandler(this.upBtn_Click);
            // 
            // listBox1
            // 
            this.listBox1.FormattingEnabled = true;
            this.listBox1.ItemHeight = 16;
            this.listBox1.Location = new System.Drawing.Point(15, 55);
            this.listBox1.Margin = new System.Windows.Forms.Padding(4);
            this.listBox1.Name = "listBox1";
            this.listBox1.Size = new System.Drawing.Size(192, 148);
            this.listBox1.TabIndex = 4;
            this.listBox1.SelectedIndexChanged += new System.EventHandler(this.listBox1_SelectedIndexChanged);
            // 
            // removeBtn
            // 
            this.removeBtn.Location = new System.Drawing.Point(221, 91);
            this.removeBtn.Margin = new System.Windows.Forms.Padding(4);
            this.removeBtn.Name = "removeBtn";
            this.removeBtn.Size = new System.Drawing.Size(100, 28);
            this.removeBtn.TabIndex = 6;
            this.removeBtn.Text = "Remove";
            this.removeBtn.UseVisualStyleBackColor = true;
            this.removeBtn.Click += new System.EventHandler(this.removeBtn_Click);
            // 
            // pointY
            // 
            this.pointY.Location = new System.Drawing.Point(148, 27);
            this.pointY.Margin = new System.Windows.Forms.Padding(4);
            this.pointY.Maximum = new decimal(new int[] {
            400,
            0,
            0,
            0});
            this.pointY.Name = "pointY";
            this.pointY.Size = new System.Drawing.Size(60, 22);
            this.pointY.TabIndex = 3;
            this.pointY.ValueChanged += new System.EventHandler(this.pointY_ValueChanged);
            // 
            // addBtn
            // 
            this.addBtn.Location = new System.Drawing.Point(221, 55);
            this.addBtn.Margin = new System.Windows.Forms.Padding(4);
            this.addBtn.Name = "addBtn";
            this.addBtn.Size = new System.Drawing.Size(100, 28);
            this.addBtn.TabIndex = 5;
            this.addBtn.Text = "Add";
            this.addBtn.UseVisualStyleBackColor = true;
            this.addBtn.Click += new System.EventHandler(this.addBtn_Click);
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(117, 30);
            this.label2.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(19, 16);
            this.label2.TabIndex = 2;
            this.label2.Text = "Y:";
            // 
            // pointX
            // 
            this.pointX.Location = new System.Drawing.Point(41, 27);
            this.pointX.Margin = new System.Windows.Forms.Padding(4);
            this.pointX.Maximum = new decimal(new int[] {
            400,
            0,
            0,
            0});
            this.pointX.Name = "pointX";
            this.pointX.Size = new System.Drawing.Size(60, 22);
            this.pointX.TabIndex = 1;
            this.pointX.ValueChanged += new System.EventHandler(this.pointX_ValueChanged);
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(11, 30);
            this.label1.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(18, 16);
            this.label1.TabIndex = 0;
            this.label1.Text = "X:";
            // 
            // PolygonPointsControl
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(8F, 16F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.Controls.Add(this.groupBox1);
            this.Margin = new System.Windows.Forms.Padding(4);
            this.Name = "PolygonPointsControl";
            this.Size = new System.Drawing.Size(445, 215);
            this.groupBox1.ResumeLayout(false);
            this.groupBox1.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.pointY)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.pointX)).EndInit();
            this.ResumeLayout(false);

        }

        #endregion
        private System.Windows.Forms.Button downBtn;
        private System.Windows.Forms.Button upBtn;
        private System.Windows.Forms.ListBox listBox1;
        private System.Windows.Forms.Button removeBtn;
        private System.Windows.Forms.NumericUpDown pointY;
        private System.Windows.Forms.Button addBtn;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.NumericUpDown pointX;
        private System.Windows.Forms.Label label1;
        public System.Windows.Forms.GroupBox groupBox1;
    }
}
