using MeasureArea.classes;
using MeasureArea.DTOs;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;


namespace MeasureArea.services
{
    public class IPC2Service
    {
        //#########################################################################################################
        #region Varibles
        public static readonly Int32 S_OK = 0;
        public static readonly Int32 S_FALSE = 1;
        public IPC2 ipc;
        //bool ipcInitialized = false;
        bool frameInitialized = false;
        bool Connected = false;
        public MeasureAreaListVM model = new MeasureAreaListVM();
        public Int32 frameMaxWidth = 0;
        public Int32 frameMaxHeight = 0;
        public Int32 frameMaxDepth = 0;

        public string SerialNumber { get; set; }

        #endregion

        #region Delegate
        public delegate void AreaChanged();
        public AreaChanged _areachanged;
        //##################################
        public delegate void ServerStoped();
        public ServerStoped OnServerStoped;
        #endregion
        public IPC2Service()
        {
            SerialNumber = "Disconnected";
        }
        //#########################################################################################################
        public void init()
        {
            frameInitialized = true;
            ipc = new IPC2(1);
#if POLLING
	                                     Application.Idle +=  new EventHandler(this.Application_Idle);
#endif
        }

        //#########################################################################################################
        #region init

        void InitIPC(string InstanceName = "")
        {
            try
            {
                Int64 hr;
                if ((ipc != null) /*&& !ipcInitialized*/)
                {
                    hr = string.IsNullOrEmpty(InstanceName) ? IPC2.Init(0) : IPC2.Init(0, InstanceName);
                    if (hr != S_OK)
                    {
                        //ipcInitialized = 
                        frameInitialized = false;
                    }
                    else
                    {
#if !POLLING
                        ipc.OnInitCompleted = new IPC2.delOnInitCompleted(OnInitCompleted);
                        IPC2.SetCallback_OnInitCompleted(0, ipc.OnInitCompleted);
                        //##################################################################
                        ipc.OnConfigChanged = new IPC2.delOnConfigChanged(configchanged);
                        IPC2.SetCallback_OnConfigChanged(0, ipc.OnConfigChanged);
                        //##################################################################
                        ipc.OnFrameIRInit = new IPC2.delOnFrameInit(OnFrameInit);
                        Int32 result = IPC2.SetCallback_OnFrameInit(0, ipc.OnFrameIRInit);

                        //##################################################################
                        ipc.OnServerStopped = new IPC2.delOnServerStopped(OnServerStopped);
                        IPC2.SetCallback_OnServerStopped(0, ipc.OnServerStopped);

#endif
                        hr = IPC2.Run(0);
                        // ipcInitialized = (hr == S_OK);
                        Connected = (hr == S_OK);

                    }
                    //logg = (hr != S_OK) ? "NOT CONNECTED" : "OK";

                }

            }
            catch (Exception ex)
            {

            }
        }
        Int32 OnInitCompleted()
        {

            Connected = true;
            if (Connected)
            {
                SerialNumber = "Connected with    #" + IPC2.GetSerialNumber(0).ToString();
            }
            GetArea();
            return S_OK;
        }
        Int32 configchanged(Int32 reserved)
        {
            GetArea();
            return 0;
        }

        Int32 OnFrameInit(Int32 frameWidth, Int32 frameHeight, Int32 frameDepth)
        {
            //  Init(frameWidth, frameHeight, frameDepth);
            frameMaxWidth = frameWidth;
            frameMaxHeight = frameHeight;
            frameMaxDepth = frameDepth;
            return 0;
        }

        Int32 OnServerStopped(int reason)
        {
            SerialNumber = "Disconnected";
            Connected = false;
            //ipcInitialized = false;
            init();
            OnServerStoped.Invoke();
            return 0;
        }
        #endregion
        //#########################################################################################################
        public MeasureAreaListVM GetArea()
        {
            UInt16 ImagerIndex = 0;
            if (ipc == null) return null;
            model = new MeasureAreaListVM();

            var cnt = IPC2.GetMeasureAreaCount(0);
            for (int i = 0; i < cnt; i++)
            {
                MeasureAreaVM item = GetArea(i);
                model.Areas.Add(item);
            }
            _areachanged.Invoke();
            return model;
        }

        public MeasureAreaVM GetArea(int AreaIndex)
        {
            UInt16 ImagerIndex = 0;
            if (ipc == null) return null;
            var _AreaName = IPC2.GetMeasureAreaName(ImagerIndex, (UInt32)AreaIndex);
            IPC2.MeasureArea measureArea;
            IPC2.GetMeasureArea(ImagerIndex, (UInt32)AreaIndex, out measureArea);
            MeasureAreaVM item = new MeasureAreaVM()
            {
                Name = !string.IsNullOrEmpty(_AreaName) ? _AreaName : "<not named>",
                BindToTempProfile = (measureArea.BindToTempProfile > 0),
                distMax = measureArea.distMax,
                distMin = measureArea.distMin,
                Emissivity = measureArea.Emissivity,
                IsColdSpot = (measureArea.IsColdSpot > 0),
                IsHotSpot = (measureArea.IsHotSpot > 0),
                Location = new POINT() { x = measureArea.Location.x, y = measureArea.Location.y },
                Mode = (int)measureArea.Mode,
                Shape = (int)measureArea.Shape,
                ShowInDigDispGroup = (measureArea.ShowInDigDispGroup > 0),
                Size = new SIZE() { height = measureArea.Size.height, width = measureArea.Size.width },
                UseEmissivity = (measureArea.UseEmissivity > 0),
                Index = AreaIndex
            };
            return item;
        }
        //################################# Area ##################################################################
        #region Area
        public void AddArea(MeasureAreaVM input)
        {
            UInt16 ImagerIndex = 0;
            IPC2.MeasureArea measureArea;
            measureArea.BindToTempProfile = (UInt32)(input.BindToTempProfile == true ? 1 : 0);
            measureArea.distMax = input.distMax;
            measureArea.distMin = input.distMin;
            measureArea.Mode = input.Mode;
            measureArea.Shape = input.Shape;
            measureArea.Emissivity = input.Emissivity;
            measureArea.IsColdSpot = (UInt32)(input.IsColdSpot == true ? 1 : 0);
            measureArea.IsHotSpot = (UInt32)(input.IsHotSpot == true ? 1 : 0);
            measureArea.Location = new IPC2.POINT { x = input.Location.x, y = input.Location.y };
            measureArea.ShowInDigDispGroup = (UInt32)(input.ShowInDigDispGroup == true ? 1 : 0);
            measureArea.Size = new IPC2.SIZE { height = input.Size.height, width = input.Size.width };
            measureArea.UseEmissivity = (UInt32)(input.UseEmissivity == true ? 1 : 0);

            var MaxIndex = IPC2.GetMeasureAreaCount(0);
            var result = IPC2.SetMeasureArea(ImagerIndex, (uint)MaxIndex, ref measureArea, true);
            if (result == S_OK)
            {
                string name = GenerateNewAreaName(input.Name);
                IPC2.SetMeasureAreaName(ImagerIndex, (uint)MaxIndex, name);
            }
        }
        string GenerateNewAreaName(string Name)
        {
            string name = string.IsNullOrEmpty(Name) ? "Area 1" : Name;
            int index = 1;
            var nameParts = name.Split(' ');
            if (nameParts.Length > 1)
            {
                try
                {
                    index = int.Parse(nameParts.Last());
                    string NameWithNoIndex = name.Substring(0, name.LastIndexOf(nameParts.Last()) - 1) + " ";
                    var lastIndexName = model.Areas.Where(u => u.Name.StartsWith(NameWithNoIndex)).Select(s => { int temp = 1; bool isParsed = int.TryParse(s.Name.Replace(NameWithNoIndex, ""), out temp); return temp; }).OrderByDescending(o => o).FirstOrDefault();
                    name = NameWithNoIndex + (lastIndexName + 1);
                }
                catch (Exception ex)
                {
                }
            }
            return name;
        }
        //#########################################################################################################
        public void UpdateArea(MeasureAreaVM model)
        {
            UInt16 ImagerIndex = 0;
            IPC2.MeasureArea measureArea;
            measureArea.BindToTempProfile = (UInt32)(model.BindToTempProfile == true ? 1 : 0);
            measureArea.distMax = model.distMax;
            measureArea.distMin = model.distMin;
            measureArea.Mode = model.Mode;
            measureArea.Shape = model.Shape;
            measureArea.Emissivity = model.Emissivity;
            measureArea.IsColdSpot = (UInt32)(model.IsColdSpot == true ? 1 : 0);
            measureArea.IsHotSpot = (UInt32)(model.IsHotSpot == true ? 1 : 0);
            measureArea.Location = new IPC2.POINT { x = model.Location.x, y = model.Location.y };
            measureArea.ShowInDigDispGroup = (UInt32)(model.ShowInDigDispGroup == true ? 1 : 0);
            measureArea.Size = new IPC2.SIZE { height = model.Size.height, width = model.Size.width };
            measureArea.UseEmissivity = (UInt32)(model.UseEmissivity == true ? 1 : 0);
            var result = IPC2.SetMeasureArea(ImagerIndex, (uint)model.Index, ref measureArea, false);
            if (result == S_OK)
            {
                IPC2.SetMeasureAreaName(ImagerIndex, (uint)model.Index, model.Name);
            }
        }
        //#########################################################################################################
        public void RemoveArea(int index)
        {
            UInt16 ImagerIndex = 0;
            if (ipc == null) return;
            model = new MeasureAreaListVM();

            IPC2.RemoveMeasureArea(ImagerIndex, (UInt32)index);
            GetArea();
        }
        //#########################################################################################################
        public void RemoveAllArea()
        {
            UInt16 ImagerIndex = 0;
            if (ipc == null) return;
            model = new MeasureAreaListVM();
            var cnt = IPC2.GetMeasureAreaCount(0);
            for (int index = 0; index < cnt; index++)
            {
                IPC2.RemoveMeasureArea(ImagerIndex, (UInt32)0);
            }
        }
        //#########################################################################################################
        public void UpdateAreaIndex(int index, int value)
        {
            var result = IPC2.SetOpticsIndex((UInt16)index, (UInt16)value);
        }
        #endregion
        //################################# Area ###################################################################

        //################################# Area Point #############################################################
        public void AddMeasureAreaPoint(int index, DTOs.POINT point)
        {
            UInt16 ImagerIndex = 0;
            IPC2.POINT newPoint;
            newPoint.x = point.x;
            newPoint.y = point.y;
            var result = IPC2.AddMeasureAreaPoint(ImagerIndex, (UInt16)index, newPoint);
        }
        public List<DTOs.POINT> GetMeasureAreaPoints(int ImagerIndex, int measurAreaIndex)
        {
            List<DTOs.POINT> points = new List<POINT>();
            var cnt = IPC2.GetNumberPolygonPoints((UInt16)ImagerIndex, (UInt32)measurAreaIndex);
            for (int i = 0; i < cnt; i++)
            {
                IPC2.POINT point;
                IPC2.GetPolygonPoint((UInt16)ImagerIndex, (UInt32)measurAreaIndex, (UInt32)i, out point);
                points.Add(new DTOs.POINT() { x = point.x, y = point.y });
            }
            return points;
        }
        //public void RemoveMeasureAreaAllPoints(int ImagerIndex, int measurAreaIndex)
        //{          
        //    var resutl = IPC2.RemoveMeasureAreaAllPoints((UInt16)ImagerIndex, (UInt32)measurAreaIndex);           
        //}
        public void RemoveMeasureAreaPoint(int ImagerIndex, int measurAreaIndex, int PolygonIndex)
        {
            var val = IPC2.RemoveMeasureAreaPoint((UInt16)ImagerIndex, (UInt32)measurAreaIndex, (UInt32)PolygonIndex);
        }
        public void EditMeasureAreaPoint(int ImagerIndex, int measurAreaIndex, int PolygonIndex, DTOs.POINT point)
        {
            IPC2.POINT _point;
            _point.x = point.x;
            _point.y = point.y;
            var val = IPC2.EditPolygonPoint((UInt16)ImagerIndex, (UInt32)measurAreaIndex, (UInt32)PolygonIndex, out _point);

        }
        //################################# Area Point ############################################################
        public bool TryToConnect(string InstanceName)
        {
            if (/*!ipcInitialized ||*/ !Connected)
            {
                InitIPC(InstanceName);
            }
            //return ipcInitialized;
            return Connected;
        }
        //#########################################################################################################
        public void ReleaseIPC()
        {
            Connected = false;
            if ((ipc != null) /*&& ipcInitialized*/)
            {
                IPC2.Release(0);
                // ipcInitialized = false;
            }
            SerialNumber = "Disconnected";
        }


    }
}
