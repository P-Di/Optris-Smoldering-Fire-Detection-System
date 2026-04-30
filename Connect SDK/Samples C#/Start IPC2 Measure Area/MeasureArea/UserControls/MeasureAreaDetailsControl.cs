using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Linq;
using System.Text;

using System.Windows.Forms;
using MeasureArea.DTOs;
using MeasureArea.services;

namespace MeasureArea.UserControls
{
    public partial class MeasureAreaDetailsControl : UserControl
    {
        public MeasureAreaVM model { get; set; }
        bool _AutoApply = false;
        public bool AutoApply { get { return _AutoApply; } set { _AutoApply = value; polygonPointsControl1.AutoSave = value; } }

        public Int32 frameMaxWidth { get; set; }
        public Int32 frameMaxHeight { get; set; }

       public IPC2Service _iPC2Service;

        public void ShowModel()
        {
            location_X.Maximum = ((decimal)frameMaxWidth<1 ? 700: (decimal)frameMaxWidth); // some timmes get too late response and set it 0
            location_Y.Maximum = ((decimal)frameMaxHeight < 1 ? 700 : (decimal)frameMaxHeight); // some timmes get too late response and set it 0

            if (model != null)
            {
                bool AutoApplyCache=AutoApply;
                AutoApply = false;
                location_X.Value = model.Location.x < location_X.Maximum ? model.Location.x : 1; //model.Location.x;
                location_Y.Value = model.Location.y < location_Y.Maximum ? model.Location.y : 1; //model.Location.y;
                Size_Height.Value = model.Size.height< Size_Height.Maximum? model.Size.height : 1;
                Size_Width.Value = model.Size.width < Size_Width.Maximum ? model.Size.width : 1;
                HotSpotCheckbox.Checked = model.IsHotSpot;
                ColdSpotCheckbox.Checked = model.IsColdSpot;
                areaNameTxt.Text = model.Name;
                shapeCombo.SelectedIndex = model.Shape - 1;
                modelCombo.SelectedIndex = model.Mode;
                ShowInDigitalDisplayGroup.Checked = model.ShowInDigDispGroup;
                distMin.Value = (decimal) model.distMin;
                distMax.Value = (decimal)model.distMax;
                emissivityChk.Checked = model.UseEmissivity;
                emissivityValu.Value =(decimal) model.Emissivity;                
                if (model.Shape == 7 || model.Shape == 8)
                {
                    polygonPointsControl1.AreaIndex = model.Index;
                    polygonPointsControl1.RefreshPointList();
                    polygonPointsControl1.bind();
                }
                AutoApply = AutoApplyCache;
            }
        }

        public void bind()  // befor save use this for update model
        {
            if (model != null)
            {
                model.Location.x = (int)location_X.Value;
                model.Location.y = (int)location_Y.Value;
                model.Size.height = (int)Size_Height.Value;
                model.Size.width = (int)Size_Width.Value;
                model.IsHotSpot = HotSpotCheckbox.Checked;
                model.IsColdSpot = ColdSpotCheckbox.Checked;
                model.Name = areaNameTxt.Text;
                model.Shape = shapeCombo.SelectedIndex + 1;
                model.Mode = modelCombo.SelectedIndex;
                model.ShowInDigDispGroup = ShowInDigitalDisplayGroup.Checked;
                model.distMin = (float)distMin.Value;
                model.distMax = (float)distMax.Value;
                model.UseEmissivity = emissivityChk.Checked;
                model.Emissivity = (float)emissivityValu.Value;
            }
        }
        #region Delegate
        public delegate void AreaChanged();
        public AreaChanged _areachanged;
        #endregion
       
        public void savePolygon()
        {
            if (model == null) return;
            if (model.Shape == 7 || model.Shape == 8)
            {               
                polygonPointsControl1.save();
            }
        }
       public  void save()
        {
            if (model == null) return;
            _areachanged.Invoke();          
        }
        void saveChange()
        {
              if(AutoApply) // if auto apply enabled then saving Values
              {                
                save();
              }
        }

        public MeasureAreaDetailsControl()
        {
            InitializeComponent();
            emissivityValu.Increment=  0.001M;
            distMax.Increment = 0.1M;
            distMin.Increment = 0.1M;
        }

        private void shapeCombo_SelectedIndexChanged(object sender, EventArgs e)
        {
            groupBox4.Visible = true;
            HotSpotCheckbox.Enabled = ColdSpotCheckbox.Enabled =  (shapeCombo.SelectedIndex == 5 ? false : true);
           
            Size_Width.Enabled = Size_Height.Enabled = (shapeCombo.SelectedIndex < 4) ? false :true;
            if (shapeCombo.SelectedIndex == 5)
            {
                HotSpotCheckbox.Checked = ColdSpotCheckbox.Checked = false;
            }
            else if(shapeCombo.SelectedIndex == 6)
            {
                var polygonPoints = _iPC2Service.GetMeasureAreaPoints(0, model.Index);
                polygonPointsControl1.points = polygonPoints;
                polygonPointsControl1.AreaIndex = model.Index;
                polygonPointsControl1._iPC2Service = _iPC2Service;
                polygonPointsControl1.bind();
                polygonPointsControl1.groupBox1.Text = "Polygon points";
                groupBox4.Visible = false;
                polygonPointsControl1.Visible = true;
                groupBox4.Visible = false;
            }
            else if (shapeCombo.SelectedIndex == 7)
            {
                var polygonPoints = _iPC2Service.GetMeasureAreaPoints(0, model.Index);
                polygonPointsControl1.points = polygonPoints;
                polygonPointsControl1.AreaIndex = model.Index;
                polygonPointsControl1._iPC2Service = _iPC2Service;
                polygonPointsControl1.bind();
                polygonPointsControl1.groupBox1.Text = "Curve points";
                groupBox4.Visible = false;
                polygonPointsControl1.Visible = true;
                groupBox4.Visible = false;
            }
            saveChange();
        }

        private void modelCombo_SelectedIndexChanged(object sender, EventArgs e)
        {
            panel1.Visible= (modelCombo.SelectedIndex == 3 ? true:false);
            saveChange();           
        }
        private void emissivityChk_CheckedChanged(object sender, EventArgs e)
        {
            emissivityValu.Enabled = emissivityChk.Checked;
            saveChange();
        }
        private void areaNameTxt_TextChanged(object sender, EventArgs e)
        {
            saveChange();
        }
        private void distMin_ValueChanged(object sender, EventArgs e)
        {
            saveChange();
        }
        private void bindTempChk_CheckedChanged(object sender, EventArgs e)
        {
            saveChange();
        }
        private void ShowInDigitalDisplayGroup_CheckedChanged(object sender, EventArgs e)
        {
            saveChange();
        }
        private void location_X_ValueChanged(object sender, EventArgs e)
        {
            saveChange();
        }
        private void location_Y_ValueChanged(object sender, EventArgs e)
        {
            saveChange();
        }
        private void Size_Width_ValueChanged(object sender, EventArgs e)
        {
            model.Size.width = (int)Size_Width.Value;
            saveChange();          
        }
        private void Size_Height_ValueChanged(object sender, EventArgs e)
        {
            model.Size.height = (int)Size_Height.Value;
            saveChange();           
        }
        private void HotSpotCheckbox_CheckedChanged(object sender, EventArgs e)
        {
            location_X.Enabled=location_Y.Enabled=!(HotSpotCheckbox.Checked || ColdSpotCheckbox.Checked);
            if (AutoApply)
            {
                AutoApply = false;
                ColdSpotCheckbox.Checked = false;
                AutoApply = true;
            }
            saveChange();
        }
        private void ColdSpotCheckbox_CheckedChanged(object sender, EventArgs e)
        {
            location_X.Enabled = location_Y.Enabled = !(HotSpotCheckbox.Checked || ColdSpotCheckbox.Checked);
            if (AutoApply)
            {
                AutoApply = false;
                HotSpotCheckbox.Checked = false;
                AutoApply = true;
            }
            saveChange();
        }

        private void emissivityValu_ValueChanged(object sender, EventArgs e)
        {
            saveChange();
        }
    }
}
