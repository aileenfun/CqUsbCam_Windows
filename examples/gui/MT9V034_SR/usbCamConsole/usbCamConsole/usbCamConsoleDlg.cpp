
// usbCamConsoleDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "usbCamConsole.h"
#include "usbCamConsoleDlg.h"
#include "afxdialogex.h"

#include <iostream>
#include <fstream>
ofstream myfile;


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

cq_uint32_t     g_iWidth;
cq_uint32_t     g_iHeight;
cq_uint8_t		g_byteBitDepthNo;
cq_uint8_t		g_byteResolutionType;

HANDLE g_mutexDisp;
HANDLE g_mutexTimer;

cq_uint64_t getTimeStamp64(cq_uint8_t* p)
{
	cq_uint64_t timestamp = 0;
	int j = 2;
	for (int i = 7; i >= 4; i--)
	{
		cq_uint64_t temp = p[j];
		j = j + 1;
		timestamp += temp<<i * 8;
	}
	j = 10;
	for (int i = 3; i >= 0; i--)
	{
		cq_uint64_t temp = p[j];
		timestamp += temp<<i * 8;
		j = j + 1;
	}
	return timestamp;
}
void CusbCamConsoleDlg::Disp(LPVOID lpParam)
{
	int i = 0, j = 0, k = 0;
	CImgFrame* imgframe = (CImgFrame*)lpParam;
	cq_uint64_t timestamp = getTimeStamp64(imgframe->m_imgHead);
	printf("img time stamp:%d", timestamp);

	cq_uint8_t* pDataBuffer = imgframe->m_imgBuf;
	
	cv::Mat frame(imgframe->m_height, imgframe->m_width, (g_byteBitDepthNo==1? CV_8UC1: CV_16UC1) ,pDataBuffer);
	
	//WaitForSingleObject(g_mutexDisp, INFINITE); 
	cv::imshow("disp",frame);
	myfile << timestamp<<","<<readAD(0)<<"\n";
	cv::waitKey(1);
	//ReleaseMutex(g_mutexDisp);
}
// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CusbCamConsoleDlg 对话框




CusbCamConsoleDlg::CusbCamConsoleDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CusbCamConsoleDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);


	m_bUsbOpen=FALSE;
	m_bIsCapturing = false;

	m_lBytePerSecond=0;

	m_bIsCamSelected = false;

	m_sensorInUse = new CCqUsbCam(this);

	assert(m_sensorInUse);


	g_mutexDisp= CreateMutex(NULL, FALSE, NULL);
	g_mutexTimer = CreateMutex(NULL, FALSE, NULL);

	g_iWidth=752;
	g_iHeight=480;
	g_byteBitDepthNo=1;
	m_bIsCapturing = false;


}
CusbCamConsoleDlg::~CusbCamConsoleDlg()
{
	OnBnClickedButtonStopCap();

}
void CusbCamConsoleDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_CHECK_AUTOGAIN, cbAutoGain);
	DDX_Control(pDX, IDC_CHECK_AUTO_EXPO, cbAutoExpo);
	DDX_Control(pDX, IDC_CHECK_LIGHT, m_checkLight);
}

BEGIN_MESSAGE_MAP(CusbCamConsoleDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_OPEN_USB, &CusbCamConsoleDlg::OnBnClickedButtonOpenUsb)
	ON_BN_CLICKED(IDC_BUTTON_INIT_SENSOR, &CusbCamConsoleDlg::OnBnClickedButtonInitSensor)
	ON_BN_CLICKED(IDC_BUTTON_CLOSE_USB, &CusbCamConsoleDlg::OnBnClickedButtonCloseUsb)
	ON_BN_CLICKED(IDC_BUTTON_STOP_CAP, &CusbCamConsoleDlg::OnBnClickedButtonStopCap)
	ON_BN_CLICKED(IDC_BUTTON_VEDIO_CAP, &CusbCamConsoleDlg::OnBnClickedButtonVedioCap)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_RADIO_RESOLU_1280_1024, &CusbCamConsoleDlg::OnBnClickedRadioResolu12801024)
	ON_BN_CLICKED(IDC_RADIO_TRIGMODE_AUTO, &CusbCamConsoleDlg::OnBnClickedRadioTrigmodeAuto)
	ON_BN_CLICKED(IDC_RADIO_TRIGMODE_FPGA, &CusbCamConsoleDlg::OnBnClickedRadioTrigmodeFpga)
	ON_BN_CLICKED(IDC_RADIO_TRIGMODE_SOFT, &CusbCamConsoleDlg::OnBnClickedRadioTrigmodeSoft)
	ON_BN_CLICKED(IDC_RADIO_TRIGMODE_EXT, &CusbCamConsoleDlg::OnBnClickedRadioTrigmodeExt)
	ON_BN_CLICKED(IDC_BUTTON_WR_SENSOR, &CusbCamConsoleDlg::OnBnClickedButtonWrSensor)
	ON_BN_CLICKED(IDC_BUTTON_RD_SENSOR, &CusbCamConsoleDlg::OnBnClickedButtonRdSensor)
	ON_BN_CLICKED(IDC_BUTTON_WR_FPGA, &CusbCamConsoleDlg::OnBnClickedButtonWrFpga)
	ON_BN_CLICKED(IDC_BUTTON_RD_FPGA, &CusbCamConsoleDlg::OnBnClickedButtonRdFpga)
	ON_EN_CHANGE(IDC_EDIT_FPGATRIG_FREQ, &CusbCamConsoleDlg::OnEnChangeEditFpgatrigFreq)
	ON_BN_CLICKED(IDC_BUTTON_SOFT_TRIG, &CusbCamConsoleDlg::OnBnClickedButtonSoftTrig)
	ON_BN_CLICKED(IDC_RADIO_MIRROR_NORMAL, &CusbCamConsoleDlg::OnBnClickedRadioMirrorNormal)
	ON_BN_CLICKED(IDC_RADIO_MIRROR_X, &CusbCamConsoleDlg::OnBnClickedRadioMirrorX)
	ON_BN_CLICKED(IDC_RADIO_MIRROR_Y, &CusbCamConsoleDlg::OnBnClickedRadioMirrorY)
	ON_BN_CLICKED(IDC_RADIO_MIRROR_XY, &CusbCamConsoleDlg::OnBnClickedRadioMirrorXy)
	ON_BN_CLICKED(IDC_CHECK_AUTOGAIN, &CusbCamConsoleDlg::setAutoGainExpo)
	ON_EN_CHANGE(IDC_EDIT_GAIN_VALUE, &CusbCamConsoleDlg::OnEnChangeEditGainValue)
	ON_BN_CLICKED(IDC_CHECK_AUTO_EXPO, &CusbCamConsoleDlg::setAutoGainExpo)
	ON_EN_CHANGE(IDC_EDIT_EXPO_VALUE, &CusbCamConsoleDlg::OnEnChangeEditExpoValue)
	ON_BN_CLICKED(IDC_BUTTON_CHECK_SPEED, &CusbCamConsoleDlg::OnBnClickedButtonCheckSpeed)
	ON_BN_CLICKED(IDC_BUTTON_WR_EEPROM, &CusbCamConsoleDlg::OnBnClickedButtonWrEeprom)
	ON_BN_CLICKED(IDC_BUTTON_RD_EEPROM, &CusbCamConsoleDlg::OnBnClickedButtonRdEeprom)
	ON_BN_CLICKED(IDC_BUTTON_RD_DEV_ID, &CusbCamConsoleDlg::OnBnClickedButtonRdDevId)
	ON_BN_CLICKED(IDC_BUTTON_WR_DEV_ID, &CusbCamConsoleDlg::OnBnClickedButtonWrDevId)
	ON_BN_CLICKED(IDC_BUTTON_RD_DEV_SN, &CusbCamConsoleDlg::OnBnClickedButtonRdDevSn)
	ON_BN_CLICKED(IDC_BUTTON_WR_DEV_SN, &CusbCamConsoleDlg::OnBnClickedButtonWrDevSn)
	ON_BN_CLICKED(IDC_RADIO_RESOLU_640_480, &CusbCamConsoleDlg::OnBnClickedRadioResolu640480)

	ON_WM_DEVICECHANGE()
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_CHECK_LIGHT, &CusbCamConsoleDlg::OnBnClickedCheckLight)
	ON_BN_CLICKED(IDC_BUTTON_PWMFREQ_SET, &CusbCamConsoleDlg::OnBnClickedButtonPwmfreqSet)
	ON_BN_CLICKED(IDC_BUTTON_PWMWIDTH_SET, &CusbCamConsoleDlg::OnBnClickedButtonPwmwidthSet)
	ON_BN_CLICKED(IDC_BUTTON_ADC_RD, &CusbCamConsoleDlg::OnBnClickedButtonAdcRd)
END_MESSAGE_MAP()


// CusbCamConsoleDlg 消息处理程序

BOOL CusbCamConsoleDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	m_sensorInUse->GetDevCnt(m_iDevCnt);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CusbCamConsoleDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CusbCamConsoleDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CusbCamConsoleDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}




void CusbCamConsoleDlg::OnBnClickedButtonOpenUsb()
{
	// TODO: 在此添加控件通知处理程序代码

	if (m_sensorInUse->OpenUSB(0)<0)
	{
		SetDlgItemText(IDC_STATIC_STATUS, L"打开USB失败。");
		return;
	}

	m_bIsCamSelected=true;

	SetDlgItemText(IDC_STATIC_STATUS, L"打开USB成功。");
	m_bUsbOpen = true;
	m_sensorInUse->InitSensor();
}


void CusbCamConsoleDlg::OnBnClickedButtonInitSensor()
{
	// TODO: 在此添加控件通知处理程序代码
	if (!m_bUsbOpen)
	{
		SetDlgItemText(IDC_STATIC_STATUS, L"USB未打开。");
		return;
	}
	m_sensorInUse->InitSensor();
	SetDlgItemText(IDC_STATIC_STATUS, L"初始化sensor成功。");
}


void CusbCamConsoleDlg::OnBnClickedButtonCloseUsb()
{
	// TODO: 在此添加控件通知处理程序代码
	if (!m_bUsbOpen)
	{
		SetDlgItemText(IDC_STATIC_STATUS, L"USB未打开或已经关闭。");
		return;
	}
	OnBnClickedButtonStopCap();
	m_sensorInUse->CloseUSB();
	m_bUsbOpen = false;
}


void CusbCamConsoleDlg::OnBnClickedButtonStopCap()
{
	// TODO: 在此添加控件通知处理程序代码
	if (false == m_bIsCapturing)
		return;

	WaitForSingleObject(g_mutexTimer, INFINITE);
	KillTimer(1);
	ReleaseMutex(g_mutexTimer);


	if(m_sensorInUse->StopCap()!=0)
	{
		SetDlgItemText(IDC_STATIC_STATUS,L"USB尚未采集");
		return;
	}
	WaitForSingleObject(g_mutexDisp, INFINITE);
	cv::destroyWindow("disp");
	ReleaseMutex(g_mutexDisp);

#if 0
	GetDlgItem(IDC_RADIO1)->EnableWindow(true);
	GetDlgItem(IDC_RADIO2)->EnableWindow(true);
	GetDlgItem(IDC_RADIO3)->EnableWindow(true);
	GetDlgItem(IDC_RADIO4)->EnableWindow(true);

	GetDlgItem(IDC_RADIO_8BIT_DEPTH)->EnableWindow(true);
	GetDlgItem(IDC_RADIO_16BIT_DEPTH)->EnableWindow(true);
	GetDlgItem(IDC_RADIO5)->EnableWindow(true);

	GetDlgItem(IDC_CHECK_SAVE_VEDIO)->EnableWindow(true);
#endif
	m_bIsCapturing = false;
	SetDlgItemText(IDC_STATIC_STATUS, L"停止采集。");

	myfile.close();

	//h_vw.release();
}


void CusbCamConsoleDlg::OnBnClickedButtonVedioCap()
{
	// TODO: 在此添加控件通知处理程序代码
		/************************************************************/
	if (!m_bUsbOpen)
	{
		SetDlgItemText(IDC_STATIC_STATUS, L"USB未打开。");
		return;
	}
	if (true == m_bIsCapturing)
		return;
#if 0
	GetDlgItem(IDC_RADIO1)->EnableWindow(false);
	GetDlgItem(IDC_RADIO2)->EnableWindow(false);
	GetDlgItem(IDC_RADIO3)->EnableWindow(false);
	GetDlgItem(IDC_RADIO4)->EnableWindow(false);

	GetDlgItem(IDC_RADIO_8BIT_DEPTH)->EnableWindow(false);
	GetDlgItem(IDC_RADIO_16BIT_DEPTH)->EnableWindow(false);
	GetDlgItem(IDC_RADIO5)->EnableWindow(false);

	GetDlgItem(IDC_CHECK_SAVE_VEDIO)->EnableWindow(false);
#endif

	cv::namedWindow("disp");
	HWND hWnd = (HWND)cvGetWindowHandle("disp");//获取子窗口的HWND
	HWND hParentWnd = ::GetParent(hWnd);//获取父窗口HWND。父窗口是我们要用的
	 //隐藏窗口标题栏 
	long style = GetWindowLong(hParentWnd, GWL_STYLE);
	style &= ~(WS_SYSMENU);
	SetWindowLong(hParentWnd, GWL_STYLE, style);


	if(m_sensorInUse->StartCap(g_iHeight, (g_byteBitDepthNo == 1 ? g_iWidth : g_iWidth * 2), CapImgEntry,this)<0)
	{
		SetDlgItemText(IDC_STATIC_STATUS, L"USB设备打开失败！");
#if 0
		GetDlgItem(IDC_RADIO1)->EnableWindow(true);
		GetDlgItem(IDC_RADIO2)->EnableWindow(true);
		GetDlgItem(IDC_RADIO3)->EnableWindow(true);
		GetDlgItem(IDC_RADIO4)->EnableWindow(true);

		GetDlgItem(IDC_RADIO_8BIT_DEPTH)->EnableWindow(true);
		GetDlgItem(IDC_RADIO_16BIT_DEPTH)->EnableWindow(true);
		GetDlgItem(IDC_RADIO5)->EnableWindow(true);

		GetDlgItem(IDC_CHECK_SAVE_VEDIO)->EnableWindow(true);
#endif
		return;
	}
	/************************************************************/

	SetTimer(1, 1000, NULL);

	m_bIsCapturing = true;
	SetDlgItemText(IDC_STATIC_STATUS, L"采集中...");
	myfile.open("example.txt");
}


void CusbCamConsoleDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	unsigned long iFrameCntPerSec =0;
	unsigned long iByteCntPerSec = 0;

	int i = 0, j = 0;

	CString str;
	switch(nIDEvent)
	{
		case 1:
			{
				WaitForSingleObject(g_mutexTimer, INFINITE);
				m_sensorInUse->GetRecvByteCnt(iByteCntPerSec);
				m_sensorInUse->ClearRecvByteCnt();
				m_sensorInUse->GetRecvFrameCnt(iFrameCntPerSec);
				m_sensorInUse->ClearRecvFrameCnt();
				str.Format(L"%d Fps     %0.4f MBs", iFrameCntPerSec,float(iByteCntPerSec)/1024.0/1024.0);		 
				HWND hWnd = (HWND)cvGetWindowHandle("disp");//获取子窗口的HWND
				HWND hParentWnd = ::GetParent(hWnd);//获取父窗口HWND。父窗口是我们要用的			
				if(hParentWnd !=NULL)
				{
					::SetWindowText(hParentWnd,str);
				}			
				break;
				ReleaseMutex(g_mutexTimer);
			}
		default:
			break;
	}
	CDialogEx::OnTimer(nIDEvent);
}



void CusbCamConsoleDlg::OnBnClickedRadioResolu12801024()
{
	// TODO: 在此添加控件通知处理程序代码
	if (!m_bUsbOpen)
	{
		SetDlgItemText(IDC_STATIC_STATUS, L"USB未打开。");
		return;
	}

	if (m_bIsCamSelected)
	{
		m_sensorInUse->SetResolution(RESOLU_752_480);
		g_byteResolutionType = RESOLU_752_480;
		g_iWidth = 752 ;
		g_iHeight = 480;
	}

	SetDlgItemText(IDC_STATIC_STATUS, L"分辨率752*480。");
}
void CusbCamConsoleDlg::OnBnClickedRadioResolu640480()
{
	// TODO: 在此添加控件通知处理程序代码
	if (!m_bUsbOpen)
	{
		SetDlgItemText(IDC_STATIC_STATUS, L"USB未打开。");
		return;
	}

	if (m_bIsCamSelected)
	{
		m_sensorInUse->SetResolution(RESOLU_640_480);
		g_byteResolutionType = RESOLU_640_480;
		g_iWidth = 640 ;
		g_iHeight = 480;
	}

	SetDlgItemText(IDC_STATIC_STATUS, L"分辨率640*480。");
}


void CusbCamConsoleDlg::OnBnClickedRadioTrigmodeAuto()
{
	// TODO: 在此添加控件通知处理程序代码
	if (!m_bUsbOpen)
	{
		SetDlgItemText(IDC_STATIC_STATUS, L"USB未打开。");
		return;
	}
	(m_bIsCamSelected ? m_sensorInUse->SetTrigMode(TRIGMODE_AUTO) : NULL);

	GetDlgItem(IDC_EDIT_FPGATRIG_FREQ)->EnableWindow(false);
	SetDlgItemText(IDC_STATIC_STATUS, L"自动触发。");
}


void CusbCamConsoleDlg::OnBnClickedRadioTrigmodeFpga()
{
	// TODO: 在此添加控件通知处理程序代码
	if (!m_bUsbOpen)
	{
		SetDlgItemText(IDC_STATIC_STATUS, L"USB未打开。");
		return;
	}
	(m_bIsCamSelected ? m_sensorInUse->SetTrigMode(TRIGMODE_FPGA) : NULL);

	GetDlgItem(IDC_EDIT_FPGATRIG_FREQ)->EnableWindow(true);
	SetDlgItemText(IDC_STATIC_STATUS, L"FPGA触发。");
}


void CusbCamConsoleDlg::OnBnClickedRadioTrigmodeSoft()
{
	// TODO: 在此添加控件通知处理程序代码
	if (!m_bUsbOpen)
	{
		SetDlgItemText(IDC_STATIC_STATUS, L"USB未打开。");
		return;
	}
	(m_bIsCamSelected ? m_sensorInUse->SetTrigMode(TRIGMODE_SOFT) : NULL);
	GetDlgItem(IDC_EDIT_FPGATRIG_FREQ)->EnableWindow(false);
	SetDlgItemText(IDC_STATIC_STATUS, L"软件触发。");
}


void CusbCamConsoleDlg::OnBnClickedRadioTrigmodeExt()
{
	// TODO: 在此添加控件通知处理程序代码
	if (!m_bUsbOpen)
	{
		SetDlgItemText(IDC_STATIC_STATUS, L"USB未打开。");
		return;
	}
	(m_bIsCamSelected ? m_sensorInUse->SetTrigMode(TRIGMODE_SIGNAL) : NULL);
	GetDlgItem(IDC_EDIT_FPGATRIG_FREQ)->EnableWindow(false);
	SetDlgItemText(IDC_STATIC_STATUS, L"外部信号触发。");
}


void CusbCamConsoleDlg::OnBnClickedButtonWrSensor()
{
	// TODO: 在此添加控件通知处理程序代码
	if (!m_bUsbOpen)
	{
		SetDlgItemText(IDC_STATIC_STATUS, L"USB未打开。");
		return;
	}
	CString strAddr, strValue;

	GetDlgItemText(IDC_EDIT_SENSOR_REGISTER_ADDR, strAddr);
	GetDlgItemText(IDC_EDIT_SENSOR_REGISTER_VALUE, strValue);

	unsigned char iAddr = str2hex(strAddr);
	unsigned short iValue = str2hex(strValue);

	(m_bIsCamSelected ? m_sensorInUse->WrSensorReg(iAddr, iValue) : NULL);
	SetDlgItemText(IDC_STATIC_STATUS, L"设置Sensor寄存器成功。");
}


void CusbCamConsoleDlg::OnBnClickedButtonRdSensor()
{
	// TODO: 在此添加控件通知处理程序代码
	if (!m_bUsbOpen)
	{
		SetDlgItemText(IDC_STATIC_STATUS, L"USB未打开。");
		return;
	}
	CString strAddr;

	GetDlgItemText(IDC_EDIT_SENSOR_REGISTER_ADDR, strAddr);
	unsigned int iValue = 0;
	unsigned char iAddr = str2hex(strAddr);	
	(m_bIsCamSelected ? m_sensorInUse->RdSensorReg(iAddr, iValue) : NULL);
	CString s_temp;
	s_temp.Format(_T("%04x"), iValue&0xffff);
	SetDlgItemText(IDC_EDIT_SENSOR_REGISTER_VALUE, s_temp);

	SetDlgItemText(IDC_STATIC_STATUS, L"读取Sensor寄存器成功。");
}


void CusbCamConsoleDlg::OnBnClickedButtonWrFpga()
{
	// TODO: 在此添加控件通知处理程序代码
	if (!m_bUsbOpen)
	{
		SetDlgItemText(IDC_STATIC_STATUS, L"USB未打开。");
		return;
	}
	CString strAddr, strValue;

	GetDlgItemText(IDC_EDIT_FPGA_REGISTER_ADDR, strAddr);
	GetDlgItemText(IDC_EDIT_FPGA_REGISTER_VALUE, strValue);

	unsigned char iAddr = str2hex(strAddr);
	unsigned char iValue = str2hex(strValue);

	(m_bIsCamSelected ? m_sensorInUse->WrFpgaReg(iAddr, iValue) : NULL);
}


void CusbCamConsoleDlg::OnBnClickedButtonRdFpga()
{
	// TODO: 在此添加控件通知处理程序代码
	if (!m_bUsbOpen)
	{
		SetDlgItemText(IDC_STATIC_STATUS, L"USB未打开。");
		return;
	}
	CString strAddr;
	GetDlgItemText(IDC_EDIT_FPGA_REGISTER_ADDR, strAddr);
	unsigned char iAddr = str2hex(strAddr);
	unsigned int iValue = 0;
	(m_bIsCamSelected ? m_sensorInUse->RdFpgaReg(iAddr, iValue) : NULL);
	CString s_temp;
	s_temp.Format(_T("%02x"), iValue&0xff);
	SetDlgItemText(IDC_EDIT_FPGA_REGISTER_VALUE, s_temp);

}
int CusbCamConsoleDlg::str2hex(CString str)
{
	int nLength = str.GetLength();
	int nBytes = WideCharToMultiByte(CP_ACP, 0, str, nLength, NULL, 0, NULL, NULL);
	char* p = new char[nBytes + 1];
	memset(p, 0, nLength + 1);
	WideCharToMultiByte(CP_OEMCP, 0, str, nLength, p, nBytes, NULL, NULL);
	p[nBytes] = 0;
	int a;
	sscanf(p, "%x", &a);
	delete[] p;
	return a;

}

void CusbCamConsoleDlg::OnEnChangeEditFpgatrigFreq()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialogEx::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
	if (!m_bUsbOpen)
	{
		SetDlgItemText(IDC_STATIC_STATUS, L"USB未打开。");
		return;
	}

	SetDlgItemText(IDC_STATIC_STATUS, L"FPGA触发频率设定成功。");
	CString str;
	GetDlgItemText(IDC_EDIT_FPGATRIG_FREQ ,str);
	unsigned char fpgafreq= _tstoi(str);
	str.ReleaseBuffer();
	if(fpgafreq>0)
	{
		(m_bIsCamSelected ? m_sensorInUse->SetFpgaTrigFreq(fpgafreq & 0xff): NULL);
	}
	else
	{
		SetDlgItemText(IDC_STATIC_STATUS,L"Check Trig value");
	}
}


void CusbCamConsoleDlg::OnBnClickedButtonSoftTrig()
{
	// TODO: 在此添加控件通知处理程序代码
	if (!m_bUsbOpen)
	{
		SetDlgItemText(IDC_STATIC_STATUS, L"USB未打开。");
		return;
	}
	(m_bIsCamSelected ? m_sensorInUse->SoftTrig(): NULL);
	SetDlgItemText(IDC_STATIC_STATUS, L"Soft触发。");
}


void CusbCamConsoleDlg::OnBnClickedRadioMirrorNormal()
{
	// TODO: 在此添加控件通知处理程序代码
	if (!m_bUsbOpen)
	{
		SetDlgItemText(IDC_STATIC_STATUS, L"USB未打开。");
		return;
	}
	(m_bIsCamSelected ? m_sensorInUse->SetMirrorType(MIRROR_NORMAL) : NULL);
}


void CusbCamConsoleDlg::OnBnClickedRadioMirrorX()
{
	// TODO: 在此添加控件通知处理程序代码
	if (!m_bUsbOpen)
	{
		SetDlgItemText(IDC_STATIC_STATUS, L"USB未打开。");
		return;
	}
	(m_bIsCamSelected ? m_sensorInUse->SetMirrorType(MIRROR_X) : NULL);
}


void CusbCamConsoleDlg::OnBnClickedRadioMirrorY()
{
	// TODO: 在此添加控件通知处理程序代码
	if (!m_bUsbOpen)
	{
		SetDlgItemText(IDC_STATIC_STATUS, L"USB未打开。");
		return;
	}
	(m_bIsCamSelected ? m_sensorInUse->SetMirrorType(MIRROR_Y) : NULL);
}


void CusbCamConsoleDlg::OnBnClickedRadioMirrorXy()
{
	// TODO: 在此添加控件通知处理程序代码
	if (!m_bUsbOpen)
	{
		SetDlgItemText(IDC_STATIC_STATUS, L"USB未打开。");
		return;
	}
	(m_bIsCamSelected ? m_sensorInUse->SetMirrorType(MIRROR_XY) : NULL);
}


void CusbCamConsoleDlg::setAutoGainExpo()
{
	if (!m_bUsbOpen)
	{
		SetDlgItemText(IDC_STATIC_STATUS, L"USB未打开。");
		return;
	}

	if ((cbAutoGain.GetCheck() == true) && (cbAutoExpo.GetCheck() == true))
	{
		(m_bIsCamSelected ? m_sensorInUse->SetAutoGainExpo(true, true) : NULL);
		SetDlgItemText(IDC_STATIC_STATUS, L"自动增益，自动曝光。");
	}	
	else if ((cbAutoGain.GetCheck() == true) && (cbAutoExpo.GetCheck() == false))
	{
		(m_bIsCamSelected ? m_sensorInUse->SetAutoGainExpo(true, false) : NULL);
		SetDlgItemText(IDC_STATIC_STATUS, L"自动增益，手动曝光。");
	}
	else if ((cbAutoGain.GetCheck() == false) && (cbAutoExpo.GetCheck() == true))
	{
		(m_bIsCamSelected ? m_sensorInUse->SetAutoGainExpo(false, true) : NULL);
		SetDlgItemText(IDC_STATIC_STATUS, L"手动增益，自动曝光。");
	}
	else if ((cbAutoGain.GetCheck() == false) && (cbAutoExpo.GetCheck() == false))
	{
		(m_bIsCamSelected ? m_sensorInUse->SetAutoGainExpo(false, false) : NULL);
		SetDlgItemText(IDC_STATIC_STATUS, L"手动增益，手动曝光。");
	}
	else;
}




void CusbCamConsoleDlg::OnEnChangeEditGainValue()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialogEx::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
	if (!m_bUsbOpen)
	{
		SetDlgItemText(IDC_STATIC_STATUS, L"USB未打开。");
		return;
	}
	
	CString str;
	GetDlgItemText(IDC_EDIT_GAIN_VALUE ,str);
	unsigned short GainValue= _tstoi(str);
	str.ReleaseBuffer();

	if(GainValue>0)
	{
		(m_bIsCamSelected ? m_sensorInUse->SetGainValue(GainValue & 0xffff) : NULL);
		SetDlgItemText(IDC_STATIC_STATUS, L"增益值设定成功。");
	}
	else
	{
		SetDlgItemText(IDC_STATIC_STATUS,L"Check Gain?");
	}
}


void CusbCamConsoleDlg::OnEnChangeEditExpoValue()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialogEx::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
	if (!m_bUsbOpen)
	{
		SetDlgItemText(IDC_STATIC_STATUS, L"USB未打开。");
		return;
	}
	
	CString str;
	GetDlgItemText(IDC_EDIT_EXPO_VALUE ,str);
	unsigned short ExpoValue= _tstoi(str);
	str.ReleaseBuffer();
	if(/*cbAutoExpo.GetCheck()==false&&*/ExpoValue>0)
	{
		(m_bIsCamSelected ? m_sensorInUse->SetExpoValue(ExpoValue) : NULL);
		SetDlgItemText(IDC_STATIC_STATUS, L"曝光值设定成功。");
	}
	else
	{
		SetDlgItemText(IDC_STATIC_STATUS,L"Check Expo?");
	}
}


void CusbCamConsoleDlg::OnBnClickedButtonCheckSpeed()
{
	// TODO: 在此添加控件通知处理程序代码
	if (!m_bUsbOpen)
	{
		SetDlgItemText(IDC_STATIC_STATUS, L"USB未打开。");
		return;
	}

	unsigned int speed = 0;
	m_sensorInUse->GetUsbSpeed(speed);
	if (speed == USB_SPEED_SUPER)
	{
		CString str("USB 3.0 device found");
		SetDlgItemText(IDC_STATIC_STATUS, str);
		m_sensorInUse->SendUsbSpeed2Fpga(USB_SPEED_SUPER);
	}
	else if (speed == USB_SPEED_HIGH)
	{
		CString str("USB 2.0 device found");
		SetDlgItemText(IDC_STATIC_STATUS, str);
		m_sensorInUse->SendUsbSpeed2Fpga(USB_SPEED_HIGH);
	}
	else
	{
		CString str("Unknown USB speed");
		SetDlgItemText(IDC_STATIC_STATUS, str);
	}

}


void CusbCamConsoleDlg::OnBnClickedButtonWrEeprom()
{
	// TODO: 在此添加控件通知处理程序代码
	if (!m_bUsbOpen)
	{
		SetDlgItemText(IDC_STATIC_STATUS, L"USB未打开。");
		return;
	}
	CString strAddr, strValue;

	GetDlgItemText(IDC_EDIT_EEPROM_ADDR, strAddr);
	GetDlgItemText(IDC_EDIT_EEPROM_VALUE, strValue);

	cq_uint16_t iAddr = str2hex(strAddr);
	cq_uint16_t iValue = str2hex(strValue);

	m_sensorInUse->WrEeprom(iAddr, iValue);
	SetDlgItemText(IDC_STATIC_STATUS, L"写EEPROM。");
}


void CusbCamConsoleDlg::OnBnClickedButtonRdEeprom()
{
	// TODO: 在此添加控件通知处理程序代码
	if (!m_bUsbOpen)
	{
		SetDlgItemText(IDC_STATIC_STATUS, L"USB未打开。");
		return;
	}
	CString strAddr;
	GetDlgItemText(IDC_EDIT_EEPROM_ADDR, strAddr);
	cq_uint32_t iAddr = str2hex(strAddr);
	cq_uint8_t irxval;
	cq_uint32_t len=1;
	m_sensorInUse->RrEeprom(iAddr, &irxval,len);
	CString s_temp;
	s_temp.Format(_T("%02x"), irxval);
	SetDlgItemText(IDC_EDIT_EEPROM_VALUE, s_temp);
	SetDlgItemText(IDC_STATIC_STATUS, L"读EEPROM。");
}


void CusbCamConsoleDlg::OnBnClickedButtonRdDevId()
{
	// TODO: 在此添加控件通知处理程序代码
	if (!m_bUsbOpen)
	{
		SetDlgItemText(IDC_STATIC_STATUS, L"USB未打开。");
		return;
	}
	CString strValue;
	cq_uint8_t iValue = 0;
	cq_uint32_t len=1;
	m_sensorInUse->RdDevID(&iValue,len);
	strValue.Format(_T("%02x"), iValue);
	SetDlgItemText(IDC_EDIT_DEVID, strValue);
	SetDlgItemText(IDC_STATIC_STATUS, L"读ID成功。");
}


void CusbCamConsoleDlg::OnBnClickedButtonWrDevId()
{
	// TODO: 在此添加控件通知处理程序代码
	if (!m_bUsbOpen)
	{
		SetDlgItemText(IDC_STATIC_STATUS, L"USB未打开。");
		return;
	}
	CString strValue;
	GetDlgItemText(IDC_EDIT_DEVID, strValue);
	cq_uint8_t iValue = str2hex(strValue);
	cq_uint32_t len=1;
	m_sensorInUse->WrDevID(&iValue,len);
	SetDlgItemText(IDC_STATIC_STATUS, L"设置ID成功。");
}


void CusbCamConsoleDlg::OnBnClickedButtonRdDevSn()
{
	// TODO: 在此添加控件通知处理程序代码
	if (!m_bUsbOpen)
	{
		SetDlgItemText(IDC_STATIC_STATUS, L"USB未打开。");
		return;
	}
	CString strValue;
	cq_uint8_t iValue[4] = {0};
	cq_uint32_t len=4;
	m_sensorInUse->RdDevSN(iValue,len);
	cq_int32_t temp=iValue[0]<<8*0;
	temp+=iValue[1]<<8*1;
	temp+=iValue[2]<<8*2;
	temp+=iValue[3]<<8*3;
	strValue.Format(_T("%02x"), temp);
	SetDlgItemText(IDC_EDIT_SN, strValue);
	SetDlgItemText(IDC_STATIC_STATUS, L"读SN成功。");
}


void CusbCamConsoleDlg::OnBnClickedButtonWrDevSn()
{
	// TODO: 在此添加控件通知处理程序代码
	if (!m_bUsbOpen)
	{
		SetDlgItemText(IDC_STATIC_STATUS, L"USB未打开。");
		return;
	}
	CString strValue;
	GetDlgItemText(IDC_EDIT_SN, strValue);
	cq_int32_t temp = str2hex(strValue);
	cq_uint8_t iValue[4]={0};
	iValue[0]=(temp>>8*0)&0xff;
	iValue[1]=(temp>>8*1)&0xff;
	iValue[2]=(temp>>8*2)&0xff;
	iValue[3]=(temp>>8*3)&0xff;
	cq_uint32_t len=4;
	m_sensorInUse->WrDevSN(iValue,len);
	SetDlgItemText(IDC_STATIC_STATUS, L"设置SN成功。");
}

BOOL CusbCamConsoleDlg::OnDeviceChange( UINT nEventType, DWORD dwData )
{
	cq_uint32_t devCnt=0;
	m_sensorInUse->GetDevCnt(devCnt);

	if(m_iDevCnt>devCnt)
	{

		m_iDevCnt=devCnt;
		MessageBox(L"-----");
		return TRUE;
	}
	if(m_iDevCnt<devCnt)
	{
		m_iDevCnt=devCnt;
		MessageBox(L"+++++");
		return TRUE;
	}

	m_iDevCnt=devCnt;
	return TRUE;
}


void CusbCamConsoleDlg::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	OnBnClickedButtonStopCap();
	CDialogEx::OnClose();
}


void CusbCamConsoleDlg::OnBnClickedCheckLight()
{
	if(m_checkLight.GetCheck() == true)
	{
		m_sensorInUse->WrFpgaReg(0x0b, 1);
	}
	else
	{
		m_sensorInUse->WrFpgaReg(0x0b, 0);
	}

}


void CusbCamConsoleDlg::OnBnClickedButtonPwmfreqSet()
{
	if (!m_bUsbOpen)
	{
		SetDlgItemText(IDC_STATIC_STATUS, L"USB未打开。");
		return;
	}
	CString strAddr, strValue;

	GetDlgItemText(IDC_EDIT_PWMFREQ, strValue);
	int iValue = _tstoi(strValue);
	int high = iValue >> 8;
	int low = iValue & 0xff;

	m_sensorInUse->WrFpgaReg(0x0c, high);
	m_sensorInUse->WrFpgaReg(0x0d, low);

}


void CusbCamConsoleDlg::OnBnClickedButtonPwmwidthSet()
{
	if (!m_bUsbOpen)
	{
		SetDlgItemText(IDC_STATIC_STATUS, L"USB未打开。");
		return;
	}
	CString strAddr, strValue;

	GetDlgItemText(IDC_EDIT_PWMWIDTH, strValue);
	int iValue = _tstoi(strValue);
	m_sensorInUse->WrFpgaReg(0x0e, iValue);
	
}

int CusbCamConsoleDlg::readAD(int ch)
{

	switch (ch)
	{
	case 0:
		ch = 0x88;
		break;
	case 1:
		ch = 0xc8;
		break;
	}
	unsigned int iValue = 0;
	arbFuncStruct arb;

	cq_uint8_t  chData[64];
	arb.order.pData = chData;
	arb.order.ReqCode = 0xFF;
	arb.order.DataBytes = 2;
	arb.order.Direction = ORDER_IN;
	arb.order.Index = ch & 0xff;



	m_sensorInUse->ArbitrFunc((void*)&arb);
	cq_uint8_t rxval[2];
	memcpy(rxval, chData, 2);
	cq_uint16_t irxval = rxval[0] << 8;
	irxval += rxval[1];
	iValue = (irxval/16)& 0xffff;
	return iValue;
}
void CusbCamConsoleDlg::OnBnClickedButtonAdcRd()
{
	if (!m_bUsbOpen)
	{
		SetDlgItemText(IDC_STATIC_STATUS, L"USB未打开。");
		return;
	}
	CString strAddr, strValue;

	GetDlgItemText(IDC_EDIT_ADC_CHANNEL, strValue);
	int ch = _tstoi(strValue);
	unsigned int iValue =readAD(ch);

	CString s_temp;
	s_temp.Format(_T("%02d"), iValue);
	SetDlgItemText(IDC_EDIT_ADC_RD, s_temp);
}
