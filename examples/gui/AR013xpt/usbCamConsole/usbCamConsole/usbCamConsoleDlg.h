
// usbCamConsoleDlg.h : ͷ�ļ�
//

#pragma once

#include "CqUsbCam.h"
#include "SensorCapbablity.h"
#include <cv.hpp>
#include <opencv.hpp>
//#include <opencv2/core/core.hpp>
//#include <opencv2/highgui/highgui.hpp>
#include "afxwin.h"
#include "Pintai.h"


class CusbCamConsoleDlg : public CDialogEx
{
private:
	cq_bool_t		m_bUsbOpen;
	cq_bool_t		m_bIsCapturing;
	cq_bool_t		m_bIsCamSelected;
	cq_int64_t      m_lBytePerSecond;
	CCqUsbCam*		m_sensorInUse;
	cq_uint32_t		m_iDevCnt;

	cq_uint32_t     g_iWidth;
	cq_uint32_t     g_iHeight;
	cq_uint8_t		g_byteBitDepthNo;
	cq_uint8_t		g_byteResolutionType;

	HANDLE g_mutexDisp;
	HANDLE g_mutexTimer;
	int show_channel;
	int g_camsize;
	byte* imgBuf ;
	byte* imgBuf1;
	byte* imgBuf2 ;
	CCameraCtrl camctrl;

// ����
public:
	CusbCamConsoleDlg(CWnd* pParent = NULL);	// ��׼���캯��
	~CusbCamConsoleDlg();

	static void CapImgEntry(LPVOID lpParam, LPVOID lpObject){ ((CusbCamConsoleDlg *)lpObject)->Disp(lpParam); };

// �Ի�������
	enum { IDD = IDD_USBCAMCONSOLE_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnBnClickedButtonOpenUsb();
	afx_msg void OnBnClickedButtonCloseUsb();
	afx_msg void OnBnClickedButtonStopCap();
	afx_msg void OnBnClickedButtonVedioCap();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
private:
	void Disp(void *lpParam);

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
	unsigned int str2hex(CString str);
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
	afx_msg BOOL OnDeviceChange(UINT nEventType, DWORD_PTR dwData);
	afx_msg void OnClose();
	afx_msg void OnCbnSelchangeCombo1();
	CComboBox selectChannel;
	CComboBox selectGenFunc;
	CComboBox selectCamFunc;
	afx_msg void OnBnClickedButtonCamFunc();
	afx_msg void OnBnClickedButtonGenFunc();
	void SetDlgText(int dlg, int v);
	afx_msg void OnBnClickedButtonWrSen();
	afx_msg void OnBnClickedButtonSave();
	void readFpgaFile();
	void eraseFlashSector();
	void readFlash();
	void cmdTest();
};
