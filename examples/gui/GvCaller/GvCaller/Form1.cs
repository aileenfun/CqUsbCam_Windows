using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Drawing.Imaging;
using System.Runtime.InteropServices;
using System.IO;
using Emgu.CV;
using Emgu.CV.Structure;
using Emgu.Util.TypeEnum;
using Emgu.CV.CvEnum;
using Emgu.CV.UI;


namespace GvCaller
{

    public partial class Form1 : Form
    {
        [DllImport(@"CqUsbCam.dll", EntryPoint = "CQUSBAddInstance", CallingConvention = CallingConvention.Cdecl)]
        public static extern int CQUSBAddInstance(CallBack cb);

        [DllImport(@"CqUsbCam.dll", EntryPoint = "CQUSBOpenUSB", CallingConvention = CallingConvention.Cdecl)]
        public static extern int CQUSBOpenUSB(int devNum);

        [DllImport(@"CqUsbCam.dll", EntryPoint = "CQUSBCloseUsb", CallingConvention = CallingConvention.Cdecl)]
        public static extern int CQUSBCloseUsb(int devNum);


        //[DllImport(@"CqUsbCam.dll", EntryPoint = "CQUSBcsInit", CallingConvention = CallingConvention.Cdecl)]
        //public static extern int CQUSBcsInit( int w, int h);

        
        [DllImport(@"CqUsbCam.dll", EntryPoint = "CQUSBStartCap", CallingConvention = CallingConvention.Cdecl)]
        public static extern int CQUSBStartCap(int devNum, int w, int h);

        [DllImport(@"CqUsbCam.dll", EntryPoint = "CQUSBStopCap", CallingConvention = CallingConvention.Cdecl)]
        public static extern int CQUSBStopCap(int devNum);

        [DllImport(@"CqUsbCam.dll", EntryPoint = "CQUSBcsGetFrame", CallingConvention = CallingConvention.Cdecl)]
        public static extern int CQUSBcsGetFrame(IntPtr imgbuf);

        [DllImport(@"CqUsbCam.dll", EntryPoint = "CQUSBSetExpo_PLS1Cam", CallingConvention = CallingConvention.Cdecl)]
        public static extern int CQUSBSetExpo_PLS1Cam(int expo,int devNum);

        [DllImport(@"CqUsbCam.dll", EntryPoint = "CQUSBSetGain_PLS1Cam", CallingConvention = CallingConvention.Cdecl)]
        public static extern int CQUSBSetGain_PLS1Cam(int gain, int devNum);

        [DllImport(@"CqUsbCam.dll", EntryPoint = "CQUSBSetResolution", CallingConvention = CallingConvention.Cdecl)]
        public static extern int CQUSBSetResolution(int res, int devNum);

        [DllImport(@"CqUsbCam.dll", EntryPoint = "CQUSBWrEeprom", CallingConvention = CallingConvention.Cdecl)]
        public static extern int CQUSBWrEE(int addr,int value, int devNum);

        [DllImport(@"CqUsbCam.dll", EntryPoint = "CQUSBRrEeprom", CallingConvention = CallingConvention.Cdecl)]
        public static extern int CQUSBRdEE(int addr,int devNum);

        public delegate int CallBack(IntPtr buff);
        public CallBack mInstance;
        public CallBack mInstance2;
        //change resolution here before set roi
        //if xend-xstart=639, then imgwidth=640
        //if yend-ystart=479, then imgheight=480
        int imgwidth = 4208;
        int imgheight = 3120;
        int devNum = 0;
        int devNum2 = 0;
        int usbSpeed = 0;
        
        Bitmap bmp;
        Bitmap bmp2;
        Bitmap LastMap;
        Bitmap LastMap2;

        public Form1()
        {

            InitializeComponent();
            initBitMap();
            mInstance = new CallBack(callbackfunc);
            devNum = CQUSBAddInstance(mInstance);
           

            mInstance2 = new CallBack(callbackfunc2);
            devNum2 = CQUSBAddInstance(mInstance2);
           



        }

        public int callbackfunc(IntPtr buff)
        {
            //buffer stores [1280*1024+1280*4]
            //in which 1280*1024 is image data
            //1280*4 is coordinates.
            //coords data formates is [16b y, 16b x]
            //[0x00 0x01, 0x00 0x03]; 
            //On y=1, x= 3, is the laser point.
            //if the result is invalid, x will be 0x07 0xff,
            // such as
            //[0x00 0x01, 0x07 0xff]
            
            byte[] pixelValues = new byte[imgwidth * imgheight];
            byte[] totalbuff = new byte[imgwidth * imgheight];
            Marshal.Copy(buff, totalbuff, 0, imgwidth * imgheight);
            Buffer.BlockCopy(totalbuff, 0, pixelValues, 0, imgwidth * imgheight);
            
            var bmp2 = bitmap8bpp(pixelValues, imgwidth, imgheight);
            //Image<Gray, byte> image = new Image<Gray, byte>(imgwidth, imgheight);

            LastMap =(Bitmap) bmp2.Clone();
            // pictureBox1.Image =bmp2 ;
            pictureBox1.Image = LastMap;

            return 1;
        }
        public int callbackfunc2(IntPtr buff)
        {
            
            byte[] pixelValues = new byte[imgwidth * imgheight];
            byte[] totalbuff = new byte[imgwidth * imgheight];
            Marshal.Copy(buff, totalbuff, 0, imgwidth * imgheight);
            Buffer.BlockCopy(totalbuff, 0, pixelValues, 0, imgwidth * imgheight);

            var bmp2 = bitmap8bpp2(pixelValues, imgwidth, imgheight);
            //Image<Gray, byte> image = new Image<Gray, byte>(imgwidth, imgheight);

            LastMap2 = (Bitmap)bmp2.Clone();
            // pictureBox1.Image =bmp2 ;
            pictureBox2.Image = LastMap2;
            
            return 1;
        }
        int initBitMap()
        {
            bmp = new Bitmap(imgwidth, imgheight, PixelFormat.Format8bppIndexed);
            ColorPalette monopalette = bmp.Palette;
            Color[] ent = monopalette.Entries;
            for (int i = 0; i < 256; i++)
            {
                ent[i] = Color.FromArgb(i, i, i);
            }
            bmp.Palette = monopalette;
            
            bmp2=new Bitmap(imgwidth, imgheight, PixelFormat.Format8bppIndexed);
            bmp2.Palette = monopalette;
            return 1;
        }
        private Bitmap bitmap8bpp(byte[] pixelValues, int width, int height)
        {
            Rectangle dimension = new Rectangle(0, 0, bmp.Width, bmp.Height);
            BitmapData picData = bmp.LockBits(dimension, ImageLockMode.ReadWrite, bmp.PixelFormat);
            IntPtr pixelStartAddress = picData.Scan0;
            System.Runtime.InteropServices.Marshal.Copy(pixelValues, 0, pixelStartAddress, pixelValues.Length);
            bmp.UnlockBits(picData);
            return bmp;
        }
        private Bitmap bitmap8bpp2(byte[] pixelValues, int width, int height)
        {
            Rectangle dimension = new Rectangle(0, 0, bmp2.Width, bmp2.Height);
            BitmapData picData = bmp2.LockBits(dimension, ImageLockMode.ReadWrite, bmp2.PixelFormat);
            IntPtr pixelStartAddress = picData.Scan0;
            System.Runtime.InteropServices.Marshal.Copy(pixelValues, 0, pixelStartAddress, pixelValues.Length);
            bmp2.UnlockBits(picData);
            return bmp2;
        }
        private void button1_Click(object sender, EventArgs e)
        {
            Random r = new Random();
            byte[] pixelValues = new byte[imgwidth * imgheight];
            int size = Marshal.SizeOf(pixelValues[0]) * pixelValues.Length;
            IntPtr p_temp = new IntPtr();
            p_temp = Marshal.AllocHGlobal(size);
            CQUSBcsGetFrame(p_temp);
            Marshal.Copy(p_temp, pixelValues, 0, imgwidth * imgheight);
            var bmp = bitmap8bpp(pixelValues, imgwidth, imgheight);

            pictureBox1.Image = bmp;
        }

        private void SavePicButton_Click(object sender, EventArgs e)
        {
            SaveFileDialog savePicDialog = new SaveFileDialog();
            savePicDialog.RestoreDirectory = true;
            savePicDialog.Filter = "(*.jpg)|*.jpg";
            if ( savePicDialog.ShowDialog()==DialogResult.OK)
            {
                string name = savePicDialog.FileName;

                Image<Gray, byte> img = new Image<Gray, byte>(imgwidth, imgheight);

                if (LastMap!=null)
                {
                    img.Bitmap = LastMap;
                }
                img.Save(name);
                
                // File.WriteAllText(name,"helloworld!");
            }

        }

        private void StopButton_Click(object sender, EventArgs e)
        {
            int res= CQUSBStopCap(devNum);
            CQUSBStopCap(devNum2);
        }

        private void TestButton_Click(object sender, EventArgs e)
        {
            CQUSBOpenUSB(devNum);

            CQUSBOpenUSB(devNum2);

        }

        private void start_Click(object sender, EventArgs e)
        {
            initBitMap();
            CQUSBStartCap(devNum, imgwidth, imgheight);
            CQUSBStartCap(devNum2 ,imgwidth, imgheight);
            if (usbSpeed > 0)
            {
                
            }
            else
            {
                //open usb error
            }
        }

        private void SetExpo_Click(object sender, EventArgs e)
        {
            int expo=int.Parse(tb_expo.Text);
            CQUSBSetExpo_PLS1Cam(expo,devNum);
            CQUSBSetExpo_PLS1Cam(expo, devNum2);
        }

        private void button1_Click_1(object sender, EventArgs e)//setROI
        {

            initBitMap();
        }

        private void btnWrEE_Click(object sender, EventArgs e)
        {
            int addr = int.Parse(textBox1.Text);
            int value = int.Parse(textBox2.Text);
            CQUSBWrEE(addr, value, devNum);
        }

        private void btnRdEE_Click(object sender, EventArgs e)
        {
            int addr = int.Parse(textBox1.Text);
            int value=CQUSBRdEE(addr,  devNum);
            textBox2.Text = value.ToString();

        }

        private void radioButton1_CheckedChanged(object sender, EventArgs e)
        {
            CQUSBSetResolution(0, devNum);
            imgwidth = 4208;
            imgheight = 3120;

        }

        private void radioButton2_CheckedChanged(object sender, EventArgs e)
        {
            CQUSBSetResolution(1, devNum);
            imgwidth = 2104;
            imgheight = 1560;
        }

        private void radioButton3_CheckedChanged(object sender, EventArgs e)
        {
            CQUSBSetResolution(2, devNum);
            imgwidth = 1052;
            imgheight = 780;
        }

        private void radioButton4_CheckedChanged(object sender, EventArgs e)
        {
            CQUSBSetResolution(3, devNum);
            imgwidth = 528;
            imgheight = 390;
        }

        private void button1_Click_2(object sender, EventArgs e)
        {
            int gain = int.Parse(tb_gain.Text);
            CQUSBSetGain_PLS1Cam(gain, devNum);
        }
    }
}
