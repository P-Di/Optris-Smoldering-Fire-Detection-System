using System;
using System.ComponentModel;
using System.Drawing;
using System.Windows.Forms;
using System.Runtime.InteropServices;
using Optris.Library.Rtsp;
using System.Collections.Generic;
using System.Net;
using System.Net.NetworkInformation;

namespace IPC2
{
    public partial class FormMain : Form
    {
#if _WIN64
        const Int32 OS = 64;
#else
        const Int32 OS = 32;
#endif
        public static readonly Int32 S_OK = 0;
        public static readonly Int32 S_FALSE = 1;

        private RtspStreamer _rtspStreamer;
        private int _rtspPort;
        private List<IPAddress> ipAddresses;

        public System.Version Version
        {
            get { return System.Reflection.Assembly.GetExecutingAssembly().GetName().Version; }
        }

        Int32 FrameWidth, FrameHeight, FrameDepth, FrameSize, MetadataSize;
        public IPC2 ipc;
        bool ipcInitialized = false, frameInitialized = false;
        Bitmap bmp;
        bool Connected = false;
        bool Colors = false;

        byte[] rgbValues;
        Int16[] Values;

        bool Painted = false;

        public FormMain()
        {
            InitializeComponent();
        }

        private void FormMain_Load(object sender, EventArgs e)
        {
            Text += String.Format(" (Rel. {0} (x{1}))", Version, OS);

            Init(160, 120, 2);
            ipc = new IPC2(1);
            timer2.Enabled = true;
            fillIPListBox();
        }

        private void FormMain_FormClosing(object sender, FormClosingEventArgs e)
        {
            ReleaseIPC();
            releaseStreamer();
        }

        void InitIPC()
        {
            Int64 hr;
            if ((ipc != null) && !ipcInitialized)
            {
                hr = IPC2.Init(0, textBoxInstanceName.Text);

                if (hr != S_OK)
                {
                    ipcInitialized = frameInitialized = false;
                }
                else
                {
                    ipc.OnServerStopped = new IPC2.delOnServerStopped(OnServerStopped);
                    IPC2.SetCallback_OnServerStopped(0, ipc.OnServerStopped);

                    ipc.OnFrameIRInit = new IPC2.delOnFrameInit(OnFrameInit);
                    Int32 u = IPC2.SetCallback_OnFrameInit(0, ipc.OnFrameIRInit);

                    ipc.OnNewFrameIREx2 = new IPC2.delOnNewFrameEx2(OnNewFrameEx2);
                    IPC2.SetCallback_OnNewFrameEx2(0, ipc.OnNewFrameIREx2);

                    ipc.OnInitCompleted = new IPC2.delOnInitCompleted(OnInitCompleted);
                    IPC2.SetCallback_OnInitCompleted(0, ipc.OnInitCompleted);

                    hr = IPC2.Run(0);
                    ipcInitialized = (hr == S_OK);
                }
                label1.Text = (hr != S_OK) ? "NOT CONNECTED" : "OK";

            }
        }

        private void ReleaseIPC()
        {
            Connected = false;
            if ((ipc != null) && ipcInitialized)
            {
                IPC2.Release(0);
                ipcInitialized = false;
            }
        }

        byte LoByte(Int16 val) { return BitConverter.GetBytes(val)[0]; }
        byte HiByte(Int16 val) { return BitConverter.GetBytes(val)[1]; }
        byte clip(Int32 val) { return (byte)((val <= 255) ? ((val > 0) ? val : 0) : 255); }


        void GetBitmap(Bitmap Bmp, Int16[] values)
        {
            Int32 stride_diff;
            // Lock the bitmap's bits.  
            System.Drawing.Rectangle rect = new System.Drawing.Rectangle(0, 0, Bmp.Width, Bmp.Height);
            System.Drawing.Imaging.BitmapData bmpData = Bmp.LockBits(rect, System.Drawing.Imaging.ImageLockMode.ReadWrite, Bmp.PixelFormat);
            stride_diff = bmpData.Stride - FrameWidth * 3;

            // Get the address of the first line.
            IntPtr ptr = bmpData.Scan0;

            if (Colors)
            {
                for (Int32 dst = 0, src = 0, y = 0; y < FrameHeight; y++, dst += stride_diff)
                    for (Int32 x = 0; x < FrameWidth; x++, src++, dst += 3)
                    {
                        Int32 C = (Int32)LoByte(values[src]) - 16;
                        Int32 D = (Int32)HiByte(values[src - (src % 2)]) - 128;
                        Int32 E = (Int32)HiByte(values[src - (src % 2) + 1]) - 128;
                        rgbValues[dst] = clip((298 * C + 516 * D + 128) >> 8);
                        rgbValues[dst + 1] = clip((298 * C - 100 * D - 208 * E + 128) >> 8);
                        rgbValues[dst + 2] = clip((298 * C + 409 * E + 128) >> 8);
                    }
            }
            else
            {
                Int16 mn, mx;
                GetBitmap_Limits(values, out mn, out mx);
                double Fact = 255.0 / (mx - mn);

                for (Int32 dst = 0, src = 0, y = 0; y < FrameHeight; y++, dst += stride_diff)
                    for (Int32 x = 0; x < FrameWidth; x++, src++, dst += 3)
                        rgbValues[dst] = rgbValues[dst + 1] = rgbValues[dst + 2] = (byte)Math.Min(Math.Max((Int32)(Fact * (values[src] - mn)), 0), 255);
            }

            // Copy the RGB values back to the bitmap
            System.Runtime.InteropServices.Marshal.Copy(rgbValues, 0, ptr, rgbValues.Length);

            // Unlock the bits.
            Bmp.UnlockBits(bmpData);
        }

        void GetBitmap_Limits(Int16[] Values, out Int16 min, out Int16 max)
        {
            Int32 y;
            double Sum, Mean, Variance;
            min = Int16.MinValue;
            max = Int16.MaxValue;
            if (Values == null) return;

            Sum = 0;
            for (y = 0; y < FrameSize; y++)
                Sum += Values[y];
            Mean = (double)Sum / FrameSize;
            Sum = 0;
            for (y = 0; y < FrameSize; y++)
                Sum += (Mean - Values[y]) * (Mean - Values[y]);
            Variance = Sum / FrameSize;
            Variance = Math.Sqrt(Variance);
            Variance *= 3;  // 3 Sigma
            min = (Int16)(Mean - Variance);
            max = (Int16)(Mean + Variance);
        }

        void Init(Int32 frameWidth, Int32 frameHeight, Int32 frameDepth)
        {
            FrameWidth = frameWidth;
            FrameHeight = frameHeight;
            FrameSize = FrameWidth * FrameHeight;
            FrameDepth = frameDepth;

            if (_rtspStreamer == null)
            {
                createStreamer();
            }

            timer1.Enabled = true;
            bmp = new Bitmap(FrameWidth, FrameHeight, System.Drawing.Imaging.PixelFormat.Format24bppRgb);
            System.Drawing.Rectangle rect = new System.Drawing.Rectangle(0, 0, bmp.Width, bmp.Height);
            System.Drawing.Imaging.BitmapData bmpData = bmp.LockBits(rect, System.Drawing.Imaging.ImageLockMode.ReadWrite, bmp.PixelFormat);
            Int32 stride = bmpData.Stride;
            bmp.UnlockBits(bmpData);
            rgbValues = new Byte[stride * FrameHeight];
            Values = new Int16[FrameSize];
            pictureBox.Size = new System.Drawing.Size(FrameWidth, FrameHeight);
            UpdateSize();
            frameInitialized = true;
        }

        void UpdateSize()
        {
            Size = new System.Drawing.Size(pictureBox.Right + 20, Math.Max(labelLink.Bottom, pictureBox.Bottom) + 50);
        }

        Int32 OnServerStopped(Int32 reason)
        {
            ReleaseIPC();
            Graphics g = Graphics.FromImage(bmp);
            g.FillRectangle(new SolidBrush(Color.Black), 0, 0, bmp.Width, bmp.Height);
            pictureBox.Invalidate();
            return 0;
        }

        Int32 OnFrameInit(Int32 frameWidth, Int32 frameHeight, Int32 frameDepth)
        {
            Init(frameWidth, frameHeight, frameDepth);
            return 0;
        }

        // will work with Imager.exe release > 3.0 only:
        Int32 OnNewFrameEx2(IntPtr data, IntPtr Metadata)
        {
            if (!frameInitialized)
                return S_FALSE;
            return NewFrame(data, (IPC2.FrameMetadata2)Marshal.PtrToStructure(Metadata, typeof(IPC2.FrameMetadata2)));
        }

        Int32 NewFrame(IntPtr data, IPC2.FrameMetadata2 Metadata)
        {
            labelFrameCounter.Text = "Frame counter: " + Metadata.CounterHW.ToString();

            switch (Metadata.FlagState)
            {
                case IPC2.FlagState.FlagOpen: labelFlag.Text = "open"; labelFlag.ForeColor = Color.Green; labelFlag.BackColor = labelFlag1.BackColor; break;
                case IPC2.FlagState.FlagClose: labelFlag.Text = "closed"; labelFlag.ForeColor = Color.White; labelFlag.BackColor = Color.Red; break;
                case IPC2.FlagState.FlagOpening: labelFlag.Text = "opening"; labelFlag.ForeColor = SystemColors.WindowText; labelFlag.BackColor = Color.Yellow; break;
                case IPC2.FlagState.FlagClosing: labelFlag.Text = "closing"; labelFlag.ForeColor = SystemColors.WindowText; labelFlag.BackColor = Color.Yellow; break;
                default: labelFlag.Text = ""; labelFlag.ForeColor = labelFlag1.ForeColor; labelFlag.BackColor = labelFlag1.BackColor; break;
            }

            for (Int32 x = 0; x < FrameSize; x++)
                Values[x] = Marshal.ReadInt16(data, x * 2);
            if (!Painted)
            {
                GetBitmap(bmp, Values);
                pictureBox.Invalidate();
                Painted = true;
            }

            if(_rtspStreamer != null)
                _rtspStreamer.FeedFrame((Bitmap)bmp.Clone());

            return 0;
        }

        private void btnStream_Click(object sender, EventArgs e)
        {
            if ((_rtspStreamer != null) && (!_rtspStreamer.IsRunning))
            {   // check if setting is changed:
                if (_rtspPort != (int)numericUpDownPort.Value)
                {
                    releaseStreamer();
                    createStreamer();
                }
            }

            if (_rtspStreamer != null)
            {
                try
                {
                    if (_rtspStreamer.IsRunning)
                    {
                        stopStreamer();
                    }
                    else
                    {
                        startStreamer();
                    }
                }
                catch (Exception ex)
                {
                    lblStatus.Text = ex.Message;
                    lblStatus.ForeColor = Color.Red;
                }
            }
        }

        Int32 OnInitCompleted()
        {
            label1.Text = "Connected with #" + IPC2.GetSerialNumber(0);
            Colors = ((TIPCMode)IPC2.GetIPCMode(0) == TIPCMode.Colors);
            Connected = true;
            UpdateSize();
            return S_OK;
        }

        private void timer2_Tick(object sender, EventArgs e)
        {
            if (!ipcInitialized || !Connected) InitIPC();
        }

        private void timer1_Tick(object sender, EventArgs e)
        {
            Painted = false;
        }

        private void pictureBox_Paint(object sender, PaintEventArgs e)
        {
            e.Graphics.DrawImage(bmp, 0, 0);
        }

        private void createStreamer()
        {
            try
            {
                _rtspPort = (int)numericUpDownPort.Value;
                _rtspStreamer = new RtspStreamer(new RtspStreamerOptions()
                {
                    CameraName = "IPC Example",
                    Port = _rtspPort,
                    MinUdpPortPair = 50000,
                    MaxUdpPortPair = 51000,
                    PacketMtu = 1400,
                    FpsNominal = 30
                });

                lblStatus.Text = "Streamer stopped";
                lblStatus.ForeColor = Color.Blue;
            }
            catch (Exception ex)
            {
                lblStatus.Text = ex.Message;
                lblStatus.ForeColor = Color.Red;
            }
        }

        private void releaseStreamer()
        {
            if(_rtspStreamer != null)
                _rtspStreamer.Stop();
            _rtspStreamer = null;
        }

        private void startStreamer()
        {
            btnStream.Text = "Stop stream";
            _rtspStreamer.Start();

            lblStatus.Text = "Streamer running";
            lblStatus.ForeColor = Color.Blue;

            numericUpDownPort.Enabled = false;
        }

        private void stopStreamer()
        {
            btnStream.Text = "Start stream";
            _rtspStreamer.Stop();

            lblStatus.Text = "Streamer stopped";
            lblStatus.ForeColor = Color.Blue;

            numericUpDownPort.Enabled = true;  
        }

        void fillIPListBox()
        {
            ipAddresses = new List<IPAddress>();
            ipAddresses.Add(IPAddress.Parse("127.0.0.1"));
            List<String> adapterplusipaddresses = GetAdapterPlusIPAddresses(ipAddresses);
            
            listBoxIP.Items.Clear();
            listBoxIP.Items.Add("Localhost (" + ipAddresses[0].ToString() + ")");
            foreach(String s in adapterplusipaddresses)
            {
                listBoxIP.Items.Add(s);
            }
            updateLink();
        }

        List<String> GetAdapterPlusIPAddresses(List<IPAddress> ipAddress)
        {
            NetworkInterface[] networkInterfaces = NetworkInterface.GetAllNetworkInterfaces();
		    List<String> result = new List<String>();
		    for (int i = 0; i<networkInterfaces.Length; i++)
            {

			    if ((networkInterfaces[i].NetworkInterfaceType == NetworkInterfaceType.Wireless80211
				    || networkInterfaces[i].NetworkInterfaceType == NetworkInterfaceType.Ethernet
				    || networkInterfaces[i].NetworkInterfaceType == NetworkInterfaceType.GigabitEthernet)
				    )
                {
				    try 
                    {
					    foreach(UnicastIPAddressInformation uipai in networkInterfaces[i].GetIPProperties().UnicastAddresses)
                        {
                            if (uipai.Address.AddressFamily.Equals(System.Net.Sockets.AddressFamily.InterNetwork))
                            {
                                String s = networkInterfaces[i].OperationalStatus == OperationalStatus.Up
                                    ? networkInterfaces[i].Description + " (" + uipai.Address.ToString() + ")"
                                    : networkInterfaces[i].Description + " ( Adapter not connected )";

                                result.Add(s);
                                ipAddress.Add(uipai.Address);
                                break;
                            }
                        }
                    }
				    catch (Exception ex)
				    {
					    break;
				    }
			    }
		    }
		    return result;
	    }

        private void listBoxIP_Click(object sender, EventArgs e)
        {
            updateLink();
        }

        private void numericUpDownPort_ValueChanged(object sender, EventArgs e)
        {
            updateLink();
        }

        private void updateLink()
        {
            int idx = Math.Max(listBoxIP.SelectedIndex, 0);
            String ip = ipAddresses[idx].ToString();
            String port = numericUpDownPort.Value.ToString();
            labelLinkRTSP.Text = "rtsp://" + ip + ":" + port;
        }

        private void labelLinkRTSP_Click(object sender, EventArgs e)
        {
            System.Windows.Forms.Clipboard.SetText(labelLinkRTSP.Text);
        }

    }
}



