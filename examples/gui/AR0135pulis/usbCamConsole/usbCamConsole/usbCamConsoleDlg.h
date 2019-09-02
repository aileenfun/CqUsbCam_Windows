
// usbCamConsoleDlg.h : 头文件
//

#pragma once

#include "CqUsbCam.h"
#include "SensorCapbablity.h"
#include <cv.hpp>
#include <opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "afxwin.h"

// CusbCamConsoleDlg 对话框

class Cdisp_matrix
{

public:
	int dispslot[5];
	int resize[5];

	Cdisp_matrix()
	{
		for (int i = 0; i < 5; i++)
		{
			dispslot[i] = i;
			resize[i] = 1;
		}
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

struct RegSettingsStruct
{
	int s;
	int funcNum;
	int value;
};

class SensorSettingClass
{
public:
	int camNum;
	int Gain;
	int Skip;
	CimgDrawCross imgDrawCross;
	int zoom;
	int dispPos;
	int ROIStep;
	int ROIposX;
	int ROIposY;
	int expo;
	RegSettingsStruct regSettings;
};

SensorSettingClass sensorSettings;
class PLSFiveCam
{
public:
	PLSCameraClass mplsCam[5];
	Cdisp_matrix dispmtx;
	PLSFiveCam(CCqUsbCam* m_sensorInUse)
	{
		for (int i = 0; i < 5; i++)
		{
			mplsCam[i].CameraNum = i;
			mplsCam[i].m_sensorInUse = m_sensorInUse;
		}
	}
	int setCrossParam(int xy, int crossStep,int crossThickness)
	{
		for (int i = 0; i < 5; i++)
		{
			//imgDrawCross.thickness[i] = 0;
			if (mplsCam[i].checked == 1)
			{
				mplsCam[i].imgDrawCross.crossStep = crossStep;
				mplsCam[i].imgDrawCross.thickness = crossThickness;
				switch (xy)
				{
				case 0:
					mplsCam[i].imgDrawCross.point.x = mplsCam[i].imgDrawCross.point.x - crossStep;
					break;
				case 1:
					mplsCam[i].imgDrawCross.point.x = mplsCam[i].imgDrawCross.point.x + crossStep;
					break;
				case 2:
					mplsCam[i].imgDrawCross.point.y = mplsCam[i].imgDrawCross.point.y - crossStep;
					break;
				case 3:
					mplsCam[i].imgDrawCross.point.y = mplsCam[i].imgDrawCross.point.y + crossStep;
					break;
				}
			}
		}
	}
	int setSelectCamera(int c,int s)
	{
		mplsCam[c].checked= s;
	}
	int setFunction(int s,int value)
	{
		for (int i = 0; i < 5; i++)
		{
			if (mplsCam[i].checked == 1)
			{
				switch (s)
				{
				case Func_Gain:
					mplsCam[i].setGain(value);
					break;
				case Func_ROI:
					mplsCam[i].setROI(value);
					break;
				case Func_ROI_STEP:
					mplsCam[i].setROI_Step(value);
					break;
				case Func_SKIP:
					mplsCam[i].setSkip(value);
					break;

				}
			}
		}
	}
};
 class PLSCameraClass
{
public:
	cq_uint32_t CameraNum;
	cq_uint32_t checked;
	CCqUsbCam* m_sensorInUse;
	RegSettingsStruct regSettings;
	CimgDrawCross imgDrawCross;
	SensorSettingClass sensorSettings;
	int wrSensorCmd(int sensor, int sensorReg, int value)
	{
		
		regSettings.funcNum = Func_SENSOR_REG1;
		regSettings.value = sensorReg >> 8;
		actCmd();

		
		regSettings.funcNum = Func_SENSOR_REG2;
		regSettings.value = sensorReg & 0xff;
		actCmd();

		
		regSettings.funcNum = Func_SENSOR_REG3;
		regSettings.value = value >> 8;
		actCmd();

		
		regSettings.funcNum = Func_SENSOR_REG4;
		regSettings.value = value && 0xff;
		actCmd();

		
		regSettings.funcNum = Func_SENSOR_SELECT;
		regSettings.value = CameraNum+1;
		actCmd();
	}
	int setROI(int roi)
	{
		switch (roi)
		{
		case 1:
			sensorSettings.ROIposX -= sensorSettings.ROIStep;
			break;
		case 2:
			sensorSettings.ROIposX += sensorSettings.ROIStep;
			break;
		case 3:
			sensorSettings.ROIposY -= sensorSettings.ROIStep;
			break;
		case 4:
			sensorSettings.ROIposY += sensorSettings.ROIStep;
			break;
		}
		regSettings.funcNum = Func_ROI;
		regSettings.value = roi;
		actCmd();
	}
	int setROI_Step(int step)
	{
		sensorSettings.ROIStep = step;
		regSettings.funcNum = Func_ROI_STEP;
		regSettings.value = step;
		actCmd();
	}
	int setGain(int gain)
	{
		sensorSettings.Gain = gain;
		wrSensorCmd(0, 0x305E, 0x2010);//set sensor gain =1;
		regSettings.funcNum = Func_Gain;
		regSettings.value = gain < 16 ? gain : 8;
		actCmd();
		switch (gain)
		{
		case 16://16
			wrSensorCmd(CameraNum + 1, 0x305E, 0x2020);
		break;
		case 32://32
			wrSensorCmd(CameraNum + 1, 0x305E, 0x2030);
		break;
		}

	}
	int setSkip(int skip)
	{
		sensorSettings.Skip = skip;
		regSettings.funcNum = Func_SKIP;
		regSettings.value = skip;
		actCmd();
	}
	int setExpo(int expo)
	{
		sensorSettings.expo = expo;
		wrSensorCmd(0, 0x0202, expo);
	}
	int setZoom(int zoom)
	{
		sensorSettings.zoom = zoom;
	}
	int dispMatrix;
	int roiStep;
	int roiPos;

	void actCmd()
	{
		m_sensorInUse->ArbitrFunc(&regSettings);
	}
};
 enum PLSFunc
 {
	 Func_SKIP,
	 Func_Gain,
	 Func_ROI,
	 Func_ROI_STEP,
	 Func_SENSOR_SELECT,
	 Func_SENSOR_REG1,
	 Func_SENSOR_REG2,
	 Func_SENSOR_REG3,
	 Func_SENSOR_REG4

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

private:

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
	
};
