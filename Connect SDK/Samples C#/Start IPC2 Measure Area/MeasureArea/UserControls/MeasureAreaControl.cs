using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Linq;
using System.Text;

using System.Windows.Forms;
using MeasureArea.classes;
using MeasureArea.services;
using MeasureArea.DTOs;

namespace MeasureArea.UserControls
{

    public partial class MeasureAreaControl : UserControl
    {
        IPC2Service _iPC2Service = new IPC2Service();
        int selectedIndexInList = 0;
        public MeasureAreaControl()
        {
            InitializeComponent();
        }
        public void FormMain_FormClosing(object sender, FormClosingEventArgs e)
        {
            // when parent form closing call this function
#if POLLING 
                                       Application.Idle -=  new EventHandler(this.Application_Idle);
#endif
            _iPC2Service.ReleaseIPC();
        }

        //#########################################################################################################
        private void MeasureAreaControl_Load(object sender, EventArgs e)
        {
            try
            {
                _iPC2Service.init();
                _iPC2Service.OnServerStoped = new IPC2Service.ServerStoped(IPCServerStoped);
                _iPC2Service._areachanged = new IPC2Service.AreaChanged(FillAreasInList);
                timer2.Enabled = true; // try to connect
                measureAreaDetailsControl1._areachanged = new MeasureAreaDetailsControl.AreaChanged(ApplyChange);
            }
            catch (Exception ex)
            {

            }
            measureAreaDetailsControl1._iPC2Service = _iPC2Service;
        }
        private void timer2_Tick(object sender, EventArgs e)
        {
            try
            {
                if (_iPC2Service.TryToConnect(textBoxInstanceName.Text))
                {
                    timer2.Enabled = false;
                  groupBox1.Enabled=measureAreaDetailsControl1.Enabled= panel1.Enabled= true;
                }
                connectingLbl.Text = _iPC2Service.SerialNumber;
            }
            catch (Exception ex)
            {
            }
        }
        void IPCServerStoped()
        {
            connectingLbl.Text = _iPC2Service.SerialNumber;
            groupBox1.Enabled = measureAreaDetailsControl1.Enabled = panel1.Enabled=   false;
            timer2.Enabled = true;
        }
        void FillAreasInList()
        {
            connectingLbl.Text = _iPC2Service.SerialNumber;
            if (_iPC2Service.model != null && _iPC2Service.model.Areas != null)
            {
                measureAreaDetailsControl1.frameMaxWidth = _iPC2Service.frameMaxWidth;
                measureAreaDetailsControl1.frameMaxHeight = _iPC2Service.frameMaxHeight;

                listBox1.Items.Clear();
                for (int i = 0; i < _iPC2Service.model.Areas.Count; i++)
                {
                    var item = _iPC2Service.model.Areas[i];
                    listBox1.Items.Add(item.Name);
                }
            }
            listBox1.SelectedIndex = listBox1.Items.Count > 0 ? (selectedIndexInList >= 0 && selectedIndexInList < listBox1.Items.Count ? selectedIndexInList : 0) : -1;

        }
        void ApplyChange()
        {
            measureAreaDetailsControl1.bind();
            _iPC2Service.UpdateArea(measureAreaDetailsControl1.model);
            _iPC2Service.model.Areas[measureAreaDetailsControl1.model.Index] = _iPC2Service.GetArea(measureAreaDetailsControl1.model.Index);
            measureAreaDetailsControl1.model = _iPC2Service.model.Areas[measureAreaDetailsControl1.model.Index];
            FillAreasInList();
        }
        private void listBox1_SelectedIndexChanged(object sender, EventArgs e)
        {
            moveUpBtn.Enabled = (listBox1.SelectedIndex == 0 ? false : true);
            moveDownBtn.Enabled = (listBox1.SelectedIndex == listBox1.Items.Count - 1 ? false : true);
            int selectedIndex = listBox1.SelectedIndex;
            var item = _iPC2Service.model.Areas[selectedIndex];
            measureAreaDetailsControl1.model = item;
            measureAreaDetailsControl1.AutoApply = false;
            measureAreaDetailsControl1.ShowModel();
            measureAreaDetailsControl1.AutoApply = autoApplyChk.Checked;
            selectedIndexInList = selectedIndex;
        }
        private void button1_Click(object sender, EventArgs e)
        {
            // add new area
            int selectedIndex = listBox1.SelectedIndex;
            if (selectedIndex < 0)
            {
                MeasureAreaVM NewModel = new MeasureAreaVM()
                {
                    BindToTempProfile = false,
                    distMax = 0,
                    distMin = 0,
                    Emissivity = 0,
                    Index = -1,
                    IsColdSpot = false,
                    IsHotSpot = false,
                    Location = new DTOs.POINT() { x = 0, y = 0 },
                    Size = new DTOs.SIZE() { height = 1, width = 1 },
                    Mode = 0,
                    Name = "",
                    Shape = 1,
                    ShowInDigDispGroup = false,
                    UseEmissivity = false
                };
                measureAreaDetailsControl1.model = NewModel;
                measureAreaDetailsControl1.ShowModel();
            }
            //####### bind and create new #####################
            measureAreaDetailsControl1.bind();
            _iPC2Service.AddArea(measureAreaDetailsControl1.model);
            //###### polygon point ############################
            if (measureAreaDetailsControl1.model.Index>-1 && (measureAreaDetailsControl1.model.Shape==7 || measureAreaDetailsControl1.model.Shape==8))
            {
                int newAreaIndex = _iPC2Service.model.Areas.Max(u => u.Index)+1;
                var points = _iPC2Service.GetMeasureAreaPoints(0, measureAreaDetailsControl1.model.Index);
                for (int p = 0; p < points.Count; p++)
                {
                    _iPC2Service.AddMeasureAreaPoint(newAreaIndex, points[p]);
                }
                for (int o = 0; o < 4; o++) // had a 4 point by default! so we remove them
                {
                    _iPC2Service.RemoveMeasureAreaPoint(0, newAreaIndex, 0);
                }                
            }
           
            selectedIndex = selectedIndex < 0 ? 0 : selectedIndex++;
            _iPC2Service.GetArea();
        }
        private void button3_Click(object sender, EventArgs e)
        {
            // remove Area
            if (listBox1.SelectedIndex >= 0)
            {
                int selectedIndex = listBox1.SelectedIndex;
                var item = _iPC2Service.model.Areas[selectedIndex];
                var result = MessageBox.Show("remove area : " + item.Name, "Are you sure ?", MessageBoxButtons.YesNo);
                if (result == DialogResult.Yes)
                {
                    selectedIndexInList = selectedIndex - 1;
                    _iPC2Service.RemoveArea(item.Index);
                }
            }
        }
        private void button4_Click(object sender, EventArgs e)
        {
            moveUpBtn.Enabled = false;
            //change area index : move up
            int selectedIndex = listBox1.SelectedIndex;
            // save polygon points before remove area
            var polygonAreaList = _iPC2Service.model.Areas.Where(u => u.Shape == 7 || u.Shape == 8).Select(s => new { NewIndx = (s.Index == selectedIndex ? selectedIndex - 1 : (s.Index == selectedIndex - 1 ? selectedIndex : s.Index)), data = s, points = _iPC2Service.GetMeasureAreaPoints(0, s.Index) }).ToList();
            //###### try to swap and save changes #############################################
            _iPC2Service.model.Areas[selectedIndex].Index--;
            _iPC2Service.model.Areas[selectedIndex - 1].Index++;
            //###### remove all Area and add from order #######################################
            var sortedList = _iPC2Service.model.Areas.OrderBy(o => o.Index).ToList();
            //---------------------------------------------------------------------------------
            _iPC2Service.RemoveAllArea();
            for (int i = 0; i < sortedList.Count; i++)
            {
                _iPC2Service.AddArea(sortedList[i]);
                if (polygonAreaList.Any(a => a.NewIndx == sortedList[i].Index))
                {
                    var A_points = polygonAreaList.First(o => o.NewIndx == sortedList[i].Index).points;
                    for (int p = 0; p < A_points.Count; p++)
                    {
                        _iPC2Service.AddMeasureAreaPoint(i, A_points[p]);
                    }
                    for (int o = 0; o < 4; o++) // when delete and add new Area , then had a 4 point by default! so we remove them
                    {
                        _iPC2Service.RemoveMeasureAreaPoint(0, i, 0);
                    }
                }
            }

            //###### refresh Area #######################################          
            _iPC2Service.GetArea();
            selectedIndexInList = (selectedIndexInList > 0 ? (selectedIndexInList - 1) : 0);
            listBox1.SelectedIndex = selectedIndexInList;

        }
        private void button5_Click(object sender, EventArgs e)
        {
            moveDownBtn.Enabled = false;
            //change area index : move down
            int selectedIndex = listBox1.SelectedIndex;
            if (selectedIndex < listBox1.Items.Count - 1)
            {
                // save polygon points before remove area
                var polygonAreaList = _iPC2Service.model.Areas.Where(u => u.Shape == 7 || u.Shape == 8).Select(s => new { NewIndx = (s.Index == selectedIndex ? selectedIndex + 1 : (s.Index == selectedIndex + 1 ? selectedIndex : s.Index)), data = s, points = _iPC2Service.GetMeasureAreaPoints(0, s.Index) }).ToList();
                //###### try to swap and save changes #############################################
                _iPC2Service.model.Areas[selectedIndex].Index++;
                var itemNext = _iPC2Service.model.Areas[selectedIndex + 1].Index--;
                //###### remove all Area and add from order #######################################
                var sortedList = _iPC2Service.model.Areas.OrderBy(o => o.Index).ToList();
                _iPC2Service.RemoveAllArea();
                for (int i = 0; i < sortedList.Count; i++)
                {
                    _iPC2Service.AddArea(sortedList[i]);
                    if (polygonAreaList.Any(a => a.NewIndx == sortedList[i].Index))
                    {
                        var A_points = polygonAreaList.First(o => o.NewIndx == sortedList[i].Index).points;
                        for (int p = 0; p < A_points.Count; p++)
                        {
                            _iPC2Service.AddMeasureAreaPoint(i, A_points[p]);
                        }
                        for (int o = 0; o < 4; o++)// when delete and add new Area , then had a 4 point by default! so we remove them
                        {
                            _iPC2Service.RemoveMeasureAreaPoint(0, i, 0);
                        }
                    }
                }
            }
            //###### refresh Area #######################################         
            _iPC2Service.GetArea();
            selectedIndexInList = (selectedIndexInList < listBox1.Items.Count - 2 ? selectedIndexInList + 1 : listBox1.Items.Count - 1);
            listBox1.SelectedIndex = selectedIndexInList;
        }
       
        private void checkBox1_CheckedChanged(object sender, EventArgs e)
        {
            measureAreaDetailsControl1.AutoApply = autoApplyChk.Checked;
            if (autoApplyChk.Checked == true)
            {
                button2.Enabled = false;
                measureAreaDetailsControl1.savePolygon();
                measureAreaDetailsControl1.save();
            }
            else
            {
                button2.Enabled = true;
            }
        }      
        private void textBoxInstanceName_TextChanged(object sender, EventArgs e)
        {
            _iPC2Service.ReleaseIPC();
            groupBox1.Enabled = measureAreaDetailsControl1.Enabled =panel1.Enabled=  false;
            timer2.Enabled = true;
        }

        private void connectingLbl_TextChanged(object sender, EventArgs e)
        {
            connectingLbl.ForeColor= connectingLbl.Text.StartsWith("Connected")? Color.Green : Color.Red;
        }

        private void button2_Click(object sender, EventArgs e)
        {
          
                measureAreaDetailsControl1.savePolygon();
                measureAreaDetailsControl1.save();
            
        }
        //#########################################################################################################

    }
}
