namespace MeasureArea
{
    partial class Form1
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
            this.measureAreaControl1 = new MeasureArea.UserControls.MeasureAreaControl();
            this.SuspendLayout();
            // 
            // measureAreaControl1
            // 
            this.measureAreaControl1.BackColor = System.Drawing.SystemColors.Window;
            this.measureAreaControl1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.measureAreaControl1.Location = new System.Drawing.Point(0, 0);
            this.measureAreaControl1.Margin = new System.Windows.Forms.Padding(5);
            this.measureAreaControl1.Name = "measureAreaControl1";
            this.measureAreaControl1.Size = new System.Drawing.Size(917, 555);
            this.measureAreaControl1.TabIndex = 0;
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(8F, 16F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(917, 555);
            this.Controls.Add(this.measureAreaControl1);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedToolWindow;
            this.Margin = new System.Windows.Forms.Padding(4);
            this.Name = "Form1";
            this.Text = "Measure Area";
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.Form1_FormClosing);
            this.ResumeLayout(false);

        }

        #endregion

        private UserControls.MeasureAreaControl measureAreaControl1;
    }
}

