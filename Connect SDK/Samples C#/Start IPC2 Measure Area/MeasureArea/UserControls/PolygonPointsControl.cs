using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Linq;
using System.Text;

using System.Windows.Forms;
using MeasureArea.services;

namespace MeasureArea.UserControls
{
    public partial class PolygonPointsControl : UserControl
    {
        public List<DTOs.POINT> points { get; set; }
        public IPC2Service _iPC2Service { get; set; }
        public int AreaIndex { get; set; }
        public bool AutoSave { get; set; }   
        
        int selectedIndex = 0;

        #region function
        public void RefreshPointList()
        {
            if (AreaIndex >= 0)
            {
                var polygonPoints = _iPC2Service.GetMeasureAreaPoints(0, AreaIndex);
                points = polygonPoints;
                removeBtn.Enabled = points.Count > 3 ? true : false;
                if (selectedIndex >= points.Count || selectedIndex < 0) selectedIndex = polygonPoints.Count > 0 ? 0 : -1;
            }
        }
        public void bind()
        {
            listBox1.Items.Clear();
            if (points != null)
            {
                for (int i = 0; i < points.Count; i++)
                {
                    var item = points[i];
                    listBox1.Items.Add("P" + (i + 1) + ": [" + item.x + ", " + item.y + "]");
                }
            }

            if (selectedIndex > points.Count - 1) selectedIndex = points.Count - 1;
            listBox1.SelectedIndex = selectedIndex = (selectedIndex >= 0 && selectedIndex <= points.Count - 1) ? selectedIndex : (points.Count > 0 ? 0 : -1);
        }

        public void save()
        {
            if (points==null || points.Count<3 ) return;
            selectedIndex = listBox1.SelectedIndex;
                          
                _iPC2Service.EditMeasureAreaPoint(0, AreaIndex, listBox1.SelectedIndex, points[selectedIndex]);
                points[selectedIndex].Ischanged = false;
                // when multi points changed and then change autoSave = True, then user like to save all changed points.
                for (int i = 0; i < points.Count; i++)
                {
                    if(points[i].Ischanged) 
                    _iPC2Service.EditMeasureAreaPoint(0, AreaIndex, i, points[i]);
                    points[i].Ischanged = false;
                }
            RefreshPointList();
            bind();
        }

        #endregion

        public PolygonPointsControl()
        {
            InitializeComponent();
            selectedIndex = 0;
        }

        private void addBtn_Click(object sender, EventArgs e)
        {
            DTOs.POINT newPoint = new DTOs.POINT() { x = (int)pointX.Value, y = (int)pointY.Value };
            _iPC2Service.AddMeasureAreaPoint(AreaIndex, newPoint);
            RefreshPointList();
            bind();
        }

        private void removeBtn_Click(object sender, EventArgs e)
        {
            selectedIndex = listBox1.SelectedIndex;
            _iPC2Service.RemoveMeasureAreaPoint(0, AreaIndex, selectedIndex);
            selectedIndex = selectedIndex - 1 >= 0 ? selectedIndex - 1 : -1;
            RefreshPointList();
            bind();
        }

        private void upBtn_Click(object sender, EventArgs e)
        {
            selectedIndex = listBox1.SelectedIndex;
            DTOs.POINT CurrentPoint = new DTOs.POINT { x = (int)points[selectedIndex].x, y = (int)points[selectedIndex].y };
            DTOs.POINT nextPoint = new DTOs.POINT { x = (int)points[selectedIndex - 1].x, y = (int)points[selectedIndex - 1].y };

            _iPC2Service.EditMeasureAreaPoint(0, AreaIndex, selectedIndex - 1, CurrentPoint);
            _iPC2Service.EditMeasureAreaPoint(0, AreaIndex, selectedIndex, nextPoint);
            RefreshPointList();
            bind();
            listBox1.SelectedIndex = selectedIndex - 1;
        }

        private void downBtn_Click(object sender, EventArgs e)
        {
            selectedIndex = listBox1.SelectedIndex;
            DTOs.POINT CurrentPoint = new DTOs.POINT { x = (int)points[selectedIndex].x, y = (int)points[selectedIndex].y };
            DTOs.POINT nextPoint = new DTOs.POINT { x = (int)points[selectedIndex + 1].x, y = (int)points[selectedIndex + 1].y };

            _iPC2Service.EditMeasureAreaPoint(0, AreaIndex, selectedIndex + 1, CurrentPoint);
            _iPC2Service.EditMeasureAreaPoint(0, AreaIndex, selectedIndex, nextPoint);
            RefreshPointList();
            bind();
            listBox1.SelectedIndex = selectedIndex + 1;

        }

        private void listBox1_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (points == null || points.Count < 1) RefreshPointList();   // if autoApply selected middle of it
            
            bool AutosaveValue = AutoSave;
            AutoSave = false; 
            selectedIndex = listBox1.SelectedIndex;
            if (selectedIndex >= 0)
            {
                var currentPoint = points[selectedIndex];
                pointX.Value = currentPoint.x;
                pointY.Value = currentPoint.y;
            }
            upBtn.Enabled = (listBox1.SelectedIndex == 0 ? false : true);
            downBtn.Enabled = (listBox1.SelectedIndex == listBox1.Items.Count - 1 ? false : true);
            AutoSave = AutosaveValue;
        }
        private void pointX_ValueChanged(object sender, EventArgs e)
        {
            if (points == null || points.Count < 1) RefreshPointList();   // if autoApply selected middle of it
            points[listBox1.SelectedIndex] = new DTOs.POINT { x = (int)pointX.Value, y = (int)pointY.Value , Ischanged=true };
            if (AutoSave) save();
            bind(); 
        }

        private void pointY_ValueChanged(object sender, EventArgs e)
        {
            if (points == null || points.Count < 1) RefreshPointList();   // if autoApply selected middle of it
            points[listBox1.SelectedIndex] = new DTOs.POINT { x = (int)pointX.Value, y = (int)pointY.Value, Ischanged=true };
            if (AutoSave) save();
            bind();
        }

    }
}
