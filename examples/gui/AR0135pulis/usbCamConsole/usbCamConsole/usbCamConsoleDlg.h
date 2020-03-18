
// usbCamConsoleDlg.h : 头文件
//

#pragma once

#include "CqUsbCam.h"
#include "SensorCapbablity.h"
#include <cv.hpp>
#include <opencv.hpp>
#include <opencv2/core/core.hpp>
//#include <opencv2/highgui/highgui.hpp>
#include "afxwin.h"
#include <iostream> 
#include <fstream> 
#include <string>
#include <functional>
// CusbCamConsoleDlg 对话框
enum PLSRegFunc
{
	Reg_SKIP,
	Reg_Gain,
	Reg_ROI,
	Reg_ROI_STEP,
	Reg_SENSOR_SELECT,
	Reg_SENSOR_REG1,
	Reg_SENSOR_REG2,
	Reg_SENSOR_REG3,
	Reg_SENSOR_REG4
};
enum PLSFunc
{
	Func_SKIP,
	Func_Gain,
	Func_ROI_STEP,
	Func_Expo,
	Func_Zoom,
	Func_DispSlot,
	Func_ROI,

};
class Cdisp_matrix
{

public:
	int dispslot;
	int zoom;

	Cdisp_matrix()
	{

	}
	int init(int s)
	{
		dispslot = s;
		zoom = 1;
		return s;
	}
};


class CimgDrawCross
{
public:
	CvPoint point;
	CvScalar color;
	int size;
	int thickness;
	int imgGain;
	int crossStep;
	CimgDrawCross()
	{
		for (int i = 0; i < 5; i++)
		{
			point.x = 640 / 2;
			point.y = 360 / 2;

			size = 10;
			thickness = 2;
			imgGain = 1;
		}
		color = CV_RGB(255, 255, 255);

	}
public:
	void drawCross(cv::Mat img, int s)
	{
		if (thickness > 0)
		{
			cv::line(img, cvPoint(point.x - size / 2, point.y), cvPoint(point.x + size / 2, point.y), color, thickness, 8, 0);
			cv::line(img, cvPoint(point.x, point.y - size / 2), cvPoint(point.x, point.y + size / 2), color, thickness, 8, 0);
		}
		return;
	}
};

class RegSettingsStruct
{
public:
	int s;
	int funcNum;
	int value;
	int sendAddr;
public:
	vector<int> addrlist;
public:
	RegSettingsStruct()
	{
		addrlist.push_back(0x0b);
		addrlist.push_back(0x10);
		addrlist.push_back(0x15);
		addrlist.push_back(0x1A);
		addrlist.push_back(0x20);
		addrlist.push_back(0x21);
		addrlist.push_back(0x22);
		addrlist.push_back(0x23);
		addrlist.push_back(0x24);
	}
};


class SensorSettingClass
{
public:
	int Gain;
	int Skip;
	CimgDrawCross imgDrawCross;
	int zoom;
	int ROIStep;
	int ROIposX;
	int ROIposY;
	int expo;
	RegSettingsStruct regSettings;
	SensorSettingClass()
	{
		Gain = 0;
		Skip = 0;
		zoom = 0;
		ROIStep = 0;
		ROIposX = 0;
		ROIposY = 0;
		expo = 0;
	}

};
typedef int(*fp)(int);
class PLSCameraClass
{
public:
	cq_uint32_t CameraNum;
	cq_uint32_t checked;
	CCqUsbCam* m_sensorInUse;
	RegSettingsStruct regSettings;
	CimgDrawCross imgDrawCross;
	SensorSettingClass sensorSettings;
	vector<int(PLSCameraClass:: *)(int)> funclist;
	Cdisp_matrix dispmtx;
	int roiStep;
	int roiPos;
	PLSCameraClass()
	{
		/*
			Func_SKIP,
			Func_Gain,
			Func_ROI_STEP,
			Func_Expo,
			Func_Zoom,
			Func_DispSlot

			Func_ROI,
			*/
		funclist.push_back(&PLSCameraClass::setSkip);
		funclist.push_back(&PLSCameraClass::setGain);
		funclist.push_back(&PLSCameraClass::setROI_Step);
		funclist.push_back(&PLSCameraClass::setExpo);
		funclist.push_back(&PLSCameraClass::setZoom);
		funclist.push_back(&PLSCameraClass::setDispSlot);
		funclist.push_back(&PLSCameraClass::setROI);
		

	}
	int init(int c)
	{
		dispmtx.init(CameraNum);
		regSettings.s = CameraNum;
		return CameraNum;
	}
	int setDispSlot(int s)
	{
		if (s == -1)
			return dispmtx.dispslot;
		dispmtx.dispslot = s;
		return s;
	}
	int useFuncList(int s, int v)
	{
		return (this->*funclist[s])(v);
		
	}
	int wrSensorCmd(int sensor, int sensorReg, int value)
	{

		regSettings.funcNum = Reg_SENSOR_REG1;
		regSettings.value = sensorReg >> 8;
		actCmd();

		
		regSettings.funcNum = Reg_SENSOR_REG2;
		regSettings.value = sensorReg & 0xff;
		actCmd();
		
		regSettings.funcNum = Reg_SENSOR_REG3;
		regSettings.value = value >> 8;
		actCmd();

		regSettings.funcNum = Reg_SENSOR_REG4;
		regSettings.value = value & 0xff;
		actCmd();

		regSettings.funcNum = Reg_SENSOR_SELECT;
		regSettings.value = sensor;
		actCmd();
		return 0;
	}
	int setROI(int roi)
	{
		int x=0,y=0;

		switch (roi)
		{
		case 1:
			x = sensorSettings.ROIposX - sensorSettings.ROIStep;
			
			break;
		case 2:
			x = sensorSettings.ROIposX + sensorSettings.ROIStep;
			
			break;
		case 3:
			y = sensorSettings.ROIposY - sensorSettings.ROIStep;
			break;
		case 4:
			y= sensorSettings.ROIposY + sensorSettings.ROIStep;
			break;
		}
		if (0)
		{
			//make sure roi is in range
		}
		regSettings.funcNum = Reg_ROI;
		regSettings.value = 0;
		actCmd();
		Sleep(200);
		sensorSettings.ROIposX = x;
		sensorSettings.ROIposY = y;
		regSettings.funcNum = Reg_ROI;
		regSettings.value = roi;
		actCmd();
		return 0;
		
	}
	int setROI_Step(int step)
	{
		if (step == -1)
			step = sensorSettings.ROIStep;
		if (sensorSettings.ROIStep == step)
			return step;
		sensorSettings.ROIStep = step;
		regSettings.funcNum = Reg_ROI_STEP;
		regSettings.value = step;
		actCmd();
		return step;
	}

	int setGain(int gain)
	{
		if(gain==-1)
		gain = sensorSettings.Gain;
		sensorSettings.Gain = gain;
		wrSensorCmd(0, 0x305E, 0x2010);//set sensor gain =1;
		regSettings.funcNum = Reg_Gain;
		regSettings.value = gain < 16 ? gain : 8;
		actCmd();
		switch (gain)
		{
		case 16://16
			//wrSensorCmd(CameraNum+1, 0x305E, 0x2020);
			wrSensorCmd(0, 0x305E, 0x2020);
			break;
		case 32://32
			//wrSensorCmd(CameraNum+1, 0x305E, 0x2030);
			wrSensorCmd(0, 0x305E, 0x2030);
			break;
		}
		
		return gain;

	}

	int setSkip(int skip)
	{
		if(skip==-1)
			skip = sensorSettings.Skip;
		sensorSettings.Skip = skip;
		regSettings.funcNum = Reg_SKIP;
		regSettings.value = skip;
		actCmd();
		
		return skip;
	}

	int setExpo(int expo)
	{
		if(expo==-1)
		expo = sensorSettings.expo;
		
		sensorSettings.expo = expo;

		wrSensorCmd(0, 0x0202, expo);
		
		
		return expo;
	}
	int setZoom(int zoom)
	{
		if (zoom == -1)
			return zoom;
		dispmtx.zoom = zoom;
		return 0;
	}
	void actCmd()
	{
		if (m_sensorInUse == NULL)
			return;

		//WrFpgaReg_t();
		if (regSettings.funcNum < 4)
		{
			regSettings.sendAddr = regSettings.addrlist[regSettings.funcNum] + CameraNum;
		}
		else
		{
			regSettings.sendAddr = regSettings.addrlist[regSettings.funcNum];
		}
		m_sensorInUse->WrFpgaReg(regSettings.sendAddr, regSettings.value);
		//m_sensorInUse->ArbitrFunc(&regSettings);
	}
};
class PLSFiveCam
{
public:
	PLSCameraClass mplsCam[5];
	PLSFiveCam(CCqUsbCam* m_sensorInUse)
	{
		for (int i = 0; i < 5; i++)
		{
			mplsCam[i].CameraNum = i;
			mplsCam[i].m_sensorInUse = m_sensorInUse;
			mplsCam[i].checked = 0;
			mplsCam[i].init(i);
		}
	}
	int getSelectedCnt()
	{
		int cnt = 0;
		for (int i = 0; i < 5; i++)
		{
			if (mplsCam[i].checked == 1)
			{
				cnt++;
			}
		}
		return cnt;

	}
	std::string saveFileName(int c)
	{
		std::stringstream ss;
		ss << c;
		std::string filename = "camSetting" + ss.str() + ".bin";
		return filename;
	}
	int saveParams()
	{

		return 0;
	}
	int readParams(PLSFiveCam *camconf)
	{
		for (int i = 0; i < 5; i++)
		{		
			memcpy(&mplsCam[i].sensorSettings, &camconf->mplsCam[i].sensorSettings, sizeof(SensorSettingClass));
			memcpy(&mplsCam[i].dispmtx, &camconf->mplsCam[i].dispmtx, sizeof(Cdisp_matrix));
			memcpy(&mplsCam[i].imgDrawCross, &camconf->mplsCam[i].imgDrawCross, sizeof(CimgDrawCross));

			mplsCam[i].checked = 1;
			
			for (int k = Func_SKIP; k < Func_ROI; k++)
			{
				mplsCam[i].useFuncList(k, -1);
				Sleep(10);
			}

			//ROI func must be careful
			
		}
		return 0;
	}
	int setCrossParam(int xy, int crossStep,int crossThickness)
	{
		for (int i = 0; i < 5; i++)
		{
			//imgDrawCross.thickness[i] = 0;
			if (mplsCam[i].checked == 1)
			{
				mplsCam[i].imgDrawCross.thickness = crossThickness;
				if (crossStep == 0)
					continue;
				mplsCam[i].imgDrawCross.crossStep = crossStep;
				int x= 0,y = 0;
				switch (xy)
				{
				case 0:
					x = mplsCam[i].imgDrawCross.point.x - crossStep;
					break;
				case 2:
					x = mplsCam[i].imgDrawCross.point.x + crossStep;
					break;
				case 1:
					y = mplsCam[i].imgDrawCross.point.y - crossStep;
					break;
				case 3:
					y = mplsCam[i].imgDrawCross.point.y + crossStep;
					break;
				}
				if (x > 0 && x < 640)
				{
					mplsCam[i].imgDrawCross.point.x = x;
				}
				if (y > 0 && y < 640)
				{
					mplsCam[i].imgDrawCross.point.y = y;
				}
			}
		}
		return 0;
	}
	int setSelectCamera(int c,int s)
	{
		mplsCam[c].checked= s;
		return 0;
	}

	int setFunction(int s,int value)
	{
		for (int i = 0; i < 5; i++)
		{
			if (mplsCam[i].checked == 1)
			{
				mplsCam[i].useFuncList(s, value);
				/*
				switch (s)
				{
				case Reg_Gain:
					mplsCam[i].setGain(value);
					break;
				case Reg_ROI:
					mplsCam[i].setROI(value);
					break;
				case Reg_ROI_STEP:
					mplsCam[i].setROI_Step(value);
					break;
				case Reg_SKIP:
					mplsCam[i].setSkip(value);
					break;

				}
				*/
			}
		}

		return 0;
	}
};

 
class CusbCamConsoleDlg : public CDialogEx
{
private:
	cq_bool_t		m_bUsbOpen;
	cq_bool_t		m_bIsCapturing;
	cq_bool_t		m_bIsCamSelected;
	cq_int64_t      m_lBytePerSecond;
	CCqUsbCam*		m_sensorInUse;
	cq_uint32_t		m_iDevCnt;


// 构造
public:
	CusbCamConsoleDlg(CWnd* pParent = NULL);	// 标准构造函数

	~CusbCamConsoleDlg();
	static void CapImgEntry(LPVOID lpParam, LPVOID lpObject) { ((CusbCamConsoleDlg*)lpObject)->Disp(lpParam); };
// 对话框数据
	enum { IDD = IDD_USBCAMCONSOLE_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	int f_snap;
	float gamma;
private:
	void Disp(void* lpParam);
public:
	afx_msg void OnBnClickedButtonOpenUsb();
	afx_msg void OnBnClickedButtonInitSensor();
	afx_msg void OnBnClickedButtonCloseUsb();
	afx_msg void OnBnClickedButtonStopCap();
	afx_msg void OnBnClickedButtonVedioCap();


	afx_msg void OnTimer(UINT_PTR nIDEvent);
private:

public:

	afx_msg void OnBnClickedRadioResolu12801024();
	afx_msg void OnBnClickedRadioTrigmodeAuto();
	afx_msg void OnBnClickedRadioTrigmodeFpga();
	afx_msg void OnBnClickedRadioTrigmodeSoft();
	afx_msg void OnBnClickedRadioTrigmodeExt();
	afx_msg void OnBnClickedButtonWrSensor();
	afx_msg void OnBnClickedButtonRdSensor();
	afx_msg void OnBnClickedButtonWrFpga();
	afx_msg void OnBnClickedButtonRdFpga();
	int str2hex(CString str);
	afx_msg void OnEnChangeEditFpgatrigFreq();
	afx_msg void OnBnClickedButtonSoftTrig();
	afx_msg void OnBnClickedRadioMirrorNormal();
	afx_msg void OnBnClickedRadioMirrorX();
	afx_msg void OnBnClickedRadioMirrorY();
	afx_msg void OnBnClickedRadioMirrorXy();
	afx_msg void setAutoGainExpo();
	afx_msg void OnEnChangeEditGainValue();
	afx_msg void OnEnChangeEditExpoValue();
private:
	CButton cbAutoGain;
public:
	CButton cbAutoExpo;
	afx_msg void OnBnClickedButtonCheckSpeed();
	afx_msg void OnBnClickedButtonWrEeprom();
	afx_msg void OnBnClickedButtonRdEeprom();
	afx_msg void OnBnClickedButtonRdDevId();
	afx_msg void OnBnClickedButtonWrDevId();
	afx_msg void OnBnClickedButtonRdDevSn();
	afx_msg void OnBnClickedButtonWrDevSn();
	afx_msg void OnBnClickedRadioResolu1280960();
	afx_msg void OnBnClickedRadioResolu640480Skip();
	afx_msg void OnBnClickedRadioResolu640480Bin();
	afx_msg BOOL OnDeviceChange(UINT nEventType, DWORD dwData );
	afx_msg void OnClose();

	afx_msg void OnBnClickedRadioX1();
	afx_msg void OnBnClickedRadioGain1();
	afx_msg void OnBnClickedRadioSkip1();
	afx_msg void OnBnClickedRadioDp1();
	afx_msg void OnBnClickedRadioDigx1();
	int GetValue(int idc1, int idc2);
	afx_msg void OnBnClickedRadioCam1();
	int selectCam;
	PLSFiveCam* mpls;

	afx_msg void OnBnClickedRadioCx1();
	afx_msg void OnBnClickedButtonExpoSet();
	void CusbCamConsoleDlg::wrSensorCmd(int sensor, int sensorReg, int value);
	afx_msg void OnBnClickedButtonCrossSet();
	afx_msg void OnBnClickedButtonSaveConfig2();
	
	afx_msg void OnBnClickedButtonLoadConfig();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedBtnSnap();
	afx_msg void OnEnChangeEdit3();
	afx_msg void OnBnClickedButtonExpoSet3();
	unsigned char lut[256];

	void doGamma(cv::Mat& src, cv::Mat& dst);
	//void on_mouse(int event, int x, int y, int flags, void* ustc);
};
