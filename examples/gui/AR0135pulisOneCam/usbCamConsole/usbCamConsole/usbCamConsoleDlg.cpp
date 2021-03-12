
// usbCamConsoleDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "usbCamConsole.h"
#include "usbCamConsoleDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

cq_uint32_t     g_iWidth;
cq_uint32_t     g_iHeight;
cq_uint8_t		g_byteBitDepthNo;
cq_uint8_t		g_byteResolutionType;

HANDLE g_mutexDisp;
HANDLE g_mutexTimer;
std::wstring s2ws(const std::string& s)
{
	int len;
	int slength = (int)s.length() + 1;
	len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0);
	wchar_t* buf = new wchar_t[len];
	MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
	std::wstring r(buf);
	delete[] buf;
	return r;
}

//Cdisp_matrix dispmtx;
//CimgDrawCross imgDrawCross;
PLSFiveCam* mplsdisp;
#define camcnt 1
int b_save_file = 0;
BOOL CusbCamConsoleDlg::PreTranslateMessage(MSG* pMsg)
{
	if ((pMsg->message == WM_KEYDOWN) && (pMsg->wParam == VK_RETURN))
	{
		return TRUE;
	}
	return CDialogEx::PreTranslateMessage(pMsg);

}
void CusbCamConsoleDlg::Disp(LPVOID lpParam)
{
	CImgFrame* imgframe = (CImgFrame*)lpParam;
	cq_uint8_t* pDataBuffer = imgframe->m_imgBuf;
	cv::Mat tempframe(imgframe->m_height, imgframe->m_width, CV_8UC1, pDataBuffer );

	cv::imshow("disp", tempframe);	
	if (f_snap)
	{
		f_snap = 0;
		CString strName;
		CString camFolder;
				int iFileIndex = 1;
				do
				{
					strName.Format(L"c:\\c6UDP\\V_%d.bmp", iFileIndex);
					++iFileIndex;
				} while (_waccess(strName, 0) == 0);
				CT2CA pszConvertedAnsiString(strName);
				std::string cvfilename(pszConvertedAnsiString);
				
				cv::imwrite(cvfilename, tempframe);
	}
	cv::waitKey(1);

	//cv::Mat mergeimg;
	//mergeimg.create(360, 640, CV_8UC1);
	
	//framelist[0].copyTo(mergeimg(cv::Rect(sig_width * 0, sig_height*0, sig_width, sig_height)));//1
	//framelist[1].copyTo(mergeimg(cv::Rect(sig_width * 0, sig_height*2, sig_width, sig_height)));//2
	//framelist[2].copyTo(mergeimg(cv::Rect(sig_width * 1, sig_height*1,sig_width, sig_height)));//3
	//framelist[3].copyTo(mergeimg(cv::Rect(sig_width * 2, sig_height*0, sig_width, sig_height)));//4
	//framelist[4].copyTo(mergeimg(cv::Rect(sig_width * 2, sig_height*2, sig_width, sig_height)));//5
	/*
	try
	{
		int k = 0;
		cv::resize(framelist[0],rframelist[0],cv::Size(framelist[0].cols* mplsdisp->mplsCam[0].dispmtx.zoom,framelist[k].rows* mplsdisp->mplsCam[k].dispmtx.zoom));
		int x=(mplsdisp->mplsCam[k].dispmtx.zoom-1)*sig_width/2;
		int y=(mplsdisp->mplsCam[k].dispmtx.zoom-1)*sig_height/2;
		mergeimg = rframelist[k](cv::Rect(x, y, sig_width, sig_height));
		//dispframelist[k]=rframelist[k](cv::Rect(x,y,sig_width,sig_height));
		doGamma(mergeimg, mergeimg);
		mplsdisp->mplsCam[2].imgDrawCross.drawCross(mergeimg,2);
	
	//dispframelist[mplsdisp->mplsCam[0].dispmtx.dispslot].copyTo(mergeimg(cv::Rect(sig_width * 0, sig_height*0, sig_width, sig_height)));//1
	//dispframelist[mplsdisp->mplsCam[1].dispmtx.dispslot].copyTo(mergeimg(cv::Rect(sig_width * 0, sig_height*2, sig_width, sig_height)));//2
	//dispframelist[mplsdisp->mplsCam[2].dispmtx.dispslot].copyTo(mergeimg(cv::Rect(sig_width * 1, sig_height*1,sig_width, sig_height)));//3
	//dispframelist[mplsdisp->mplsCam[3].dispmtx.dispslot].copyTo(mergeimg(cv::Rect(sig_width * 2, sig_height*0, sig_width, sig_height)));//4
	//dispframelist[mplsdisp->mplsCam[4].dispmtx.dispslot].copyTo(mergeimg(cv::Rect(sig_width * 2, sig_height*2, sig_width, sig_height)));//5
	//cv::namedWindow("disp");
	//cv::imshow("disp", mergeimg);
	if (f_snap)
	{
		f_snap = 0;
		cv::imwrite("snap.jpg", mergeimg);
	}
	cv::waitKey(1);
	}
	catch (cv::Exception & e)
	{
		cerr << e.msg << endl; // output exception message
		std::wstring stemp = s2ws(e.msg);
		LPCWSTR result = stemp.c_str();
		OutputDebugString(result);
	}
	*/
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

	m_sensorInUse = new CCqUsbCam(NULL);

	assert(m_sensorInUse);


	g_mutexDisp= CreateMutex(NULL, FALSE, NULL);
	g_mutexTimer = CreateMutex(NULL, FALSE, NULL);

	g_iWidth=1280;
	g_iHeight=720;
	g_byteBitDepthNo=1;
	m_bIsCapturing = false;
	selectCam = 0;
	mpls = new PLSFiveCam(m_sensorInUse);
	mplsdisp = mpls;
	f_snap = 0;
	gamma = 1;
	
}
CusbCamConsoleDlg::~CusbCamConsoleDlg()
{
	OnBnClickedButtonStopCap();

}

void CusbCamConsoleDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

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

	ON_WM_DEVICECHANGE()
	ON_WM_CLOSE()
	
	
	ON_BN_CLICKED(IDC_RADIO_X1, &CusbCamConsoleDlg::OnBnClickedRadioX1)
	ON_BN_CLICKED(IDC_RADIO_X2, &CusbCamConsoleDlg::OnBnClickedRadioX1)
	ON_BN_CLICKED(IDC_RADIO_Y1, &CusbCamConsoleDlg::OnBnClickedRadioX1)
	ON_BN_CLICKED(IDC_RADIO_Y2, &CusbCamConsoleDlg::OnBnClickedRadioX1)
	
	ON_BN_CLICKED(IDC_RADIO_GAIN1, &CusbCamConsoleDlg::OnBnClickedRadioGain1)
	ON_BN_CLICKED(IDC_RADIO_GAIN2, &CusbCamConsoleDlg::OnBnClickedRadioGain1)
	ON_BN_CLICKED(IDC_RADIO_GAIN3, &CusbCamConsoleDlg::OnBnClickedRadioGain1)
	ON_BN_CLICKED(IDC_RADIO_GAIN4, &CusbCamConsoleDlg::OnBnClickedRadioGain1)
	ON_BN_CLICKED(IDC_RADIO_GAIN5, &CusbCamConsoleDlg::OnBnClickedRadioGain1)
	ON_BN_CLICKED(IDC_RADIO_GAIN6, &CusbCamConsoleDlg::OnBnClickedRadioGain1)

	
	ON_BN_CLICKED(IDC_RADIO_SKIP1, &CusbCamConsoleDlg::OnBnClickedRadioSkip1)
	ON_BN_CLICKED(IDC_RADIO_SKIP2, &CusbCamConsoleDlg::OnBnClickedRadioSkip1)
	ON_BN_CLICKED(IDC_RADIO_SKIP3, &CusbCamConsoleDlg::OnBnClickedRadioSkip1)
	
	ON_BN_CLICKED(IDC_RADIO_DP1, &CusbCamConsoleDlg::OnBnClickedRadioDp1)
	ON_BN_CLICKED(IDC_RADIO_DP2, &CusbCamConsoleDlg::OnBnClickedRadioDp1)
	ON_BN_CLICKED(IDC_RADIO_DP3, &CusbCamConsoleDlg::OnBnClickedRadioDp1)
	ON_BN_CLICKED(IDC_RADIO_DP4, &CusbCamConsoleDlg::OnBnClickedRadioDp1)
	ON_BN_CLICKED(IDC_RADIO_DP5, &CusbCamConsoleDlg::OnBnClickedRadioDp1)

	ON_BN_CLICKED(IDC_RADIO_DIGX1, &CusbCamConsoleDlg::OnBnClickedRadioDigx1)
	ON_BN_CLICKED(IDC_RADIO_DIGX2, &CusbCamConsoleDlg::OnBnClickedRadioDigx1)
	ON_BN_CLICKED(IDC_RADIO_DIGX3, &CusbCamConsoleDlg::OnBnClickedRadioDigx1)
	ON_BN_CLICKED(IDC_RADIO_DIGX4, &CusbCamConsoleDlg::OnBnClickedRadioDigx1)
	ON_BN_CLICKED(IDC_RADIO_DIGX5, &CusbCamConsoleDlg::OnBnClickedRadioDigx1)
	ON_BN_CLICKED(IDC_RADIO_DIGX6, &CusbCamConsoleDlg::OnBnClickedRadioDigx1)

	ON_BN_CLICKED(IDC_CK_CAM1, &CusbCamConsoleDlg::OnBnClickedRadioCam1)
	ON_BN_CLICKED(IDC_CK_CAM2, &CusbCamConsoleDlg::OnBnClickedRadioCam1)
	ON_BN_CLICKED(IDC_CK_CAM3, &CusbCamConsoleDlg::OnBnClickedRadioCam1)
	ON_BN_CLICKED(IDC_CK_CAM4, &CusbCamConsoleDlg::OnBnClickedRadioCam1)
	ON_BN_CLICKED(IDC_CK_CAM5, &CusbCamConsoleDlg::OnBnClickedRadioCam1)
	ON_BN_CLICKED(IDC_RADIO_CX1, &CusbCamConsoleDlg::OnBnClickedRadioCx1)
	ON_BN_CLICKED(IDC_RADIO_CX2, &CusbCamConsoleDlg::OnBnClickedRadioCx1)
	ON_BN_CLICKED(IDC_RADIO_CY1, &CusbCamConsoleDlg::OnBnClickedRadioCx1)
	ON_BN_CLICKED(IDC_RADIO_CY2, &CusbCamConsoleDlg::OnBnClickedRadioCx1)
	ON_BN_CLICKED(IDC_BUTTON_EXPO_SET, &CusbCamConsoleDlg::OnBnClickedButtonExpoSet)
	ON_BN_CLICKED(IDC_BUTTON_CROSS_SET, &CusbCamConsoleDlg::OnBnClickedButtonCrossSet)
	ON_BN_CLICKED(IDC_BUTTON_SAVE_CONFIG2, &CusbCamConsoleDlg::OnBnClickedButtonSaveConfig2)
	ON_BN_CLICKED(IDC_BUTTON_LOAD_CONFIG, &CusbCamConsoleDlg::OnBnClickedButtonLoadConfig)
	ON_BN_CLICKED(IDC_BTN_SNAP, &CusbCamConsoleDlg::OnBnClickedBtnSnap)
	ON_BN_CLICKED(IDC_BUTTON_EXPO_SET3, &CusbCamConsoleDlg::OnBnClickedButtonExpoSet3)
	ON_BN_CLICKED(IDC_BUTTON_WR_EEPROM, &CusbCamConsoleDlg::OnBnClickedButtonWrEeprom)
	ON_BN_CLICKED(IDC_BUTTON_RD_EEPROM, &CusbCamConsoleDlg::OnBnClickedButtonRdEeprom)
END_MESSAGE_MAP()


// CusbCamConsoleDlg 消息处理程序

//void CusbCamConsoleDlg::on_mouse(int event, int x, int y, int flags, void* ustc)
void on_mouse(int event, int x, int y, int flags, void* ustc)
{
	CusbCamConsoleDlg* app= (CusbCamConsoleDlg*)ustc;
	int camnum = -1;
	int roix = 0;
	int roiy = 0;
	if ((flags & CV_EVENT_FLAG_LBUTTON))
	{
		if (x < 640 && y < 360)
		{
			camnum = 0;
			roix = x;
			roiy = y;
		}
		else if(y>360*2&&y<360*3&&x<640)
		{
			camnum = 1;
			roix = x;
			roiy = y - 360 * 2;
		}
		else if (y > 360 && y < 360 * 2 && x>640 && x < 640 * 2)
		{
			camnum = 3;
			roix = x - 640;
			roiy = y - 360;
		}
		else if (x > 640 * 2 && x < 640 * 3 && y < 360)
		{
			camnum = 4;
			roix = x - 640 * 2;
			roiy = y;
		}
		else if (x > 640 * 2 && x < 640 * 3 && y>360 * 2 && y < 360 * 3)
		{
			camnum = 5;
			roix = x - 640*2;
			roiy = y - 360 * 2;
		}
		//assume now center point is 0
		

		//x-,x+,y-,y+
		//0, 1, 2, 3
		//1, 2, 4, 8
		CButton* m_ctlCheck = (CButton*)(app->GetDlgItem(IDC_RADIO_SKIP1));
		m_ctlCheck->SetCheck(1);
		m_ctlCheck = (CButton*)(app->GetDlgItem(IDC_RADIO_SKIP2));
		m_ctlCheck->SetCheck(0);
		m_ctlCheck = (CButton*)(app->GetDlgItem(IDC_RADIO_SKIP3));
		m_ctlCheck->SetCheck(0);
		app->OnBnClickedRadioSkip1();
		int axis = 0;
		if (roix < 0)
		{
			axis = 1;
		}
		else if (roix > 0)
		{
			axis = 2;
		}
		int roistep = roix - 360;
		app->mpls->setFunction(Func_ROI_STEP, roistep*4);
		Sleep(100);
		app->mpls->setFunction(Func_ROI,axis );
		if (roiy < 0)
		{
			axis = 4;
		}
		if (roiy > 0)
		{
			axis = 8;
		}
		roistep = roiy - 160;
		Sleep(100);
		app->mpls->setFunction(Func_ROI_STEP, roistep*4);
		Sleep(100);
		app->mpls->setFunction(Func_ROI, axis);

	}

}
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
	(GetDlgItem(IDC_EDIT_GAMMA)->SetWindowTextW(_T("1")));
	OnBnClickedButtonExpoSet3();	
	return TRUE; 
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
	
	CWnd:: SetWindowPos(&CWnd::wndTopMost,  0, 0, 0, 0, SWP_NOMOVE | SWP_NOREPOSITION | SWP_NOSIZE);
	HWND hWnd = (HWND)cvGetWindowHandle("disp");//获取子窗口的HWND
	HWND hParentWnd = ::GetParent(hWnd);//获取父窗口HWND。父窗口是我们要用的
	

	 //隐藏窗口标题栏 
	//long style = GetWindowLong(hParentWnd, GWL_STYLE);
	//style &= ~(WS_SYSMENU);
	//SetWindowLong(hParentWnd, GWL_STYLE, style);
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CusbCamConsoleDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}




void CusbCamConsoleDlg::OnBnClickedButtonOpenUsb()
{
	
	if (m_sensorInUse->OpenUSB(0)<0)
	{
		SetDlgItemText(IDC_STATIC_STATUS, L"打开USB失败。");
		return;
	}
	m_bIsCamSelected=true;

	SetDlgItemText(IDC_STATIC_STATUS, L"打开USB成功。");
	m_bUsbOpen = true;
	OnBnClickedButtonCheckSpeed();
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

	//cv::namedWindow("disp");
	//cv::moveWindow("disp", 0, 0);
	//cv::setMouseCallback("disp", on_mouse, this);
	//HWND hWnd = (HWND)cvGetWindowHandle("disp");//获取子窗口的HWND
	//HWND hParentWnd = ::GetParent(hWnd);//获取父窗口HWND。父窗口是我们要用的
	 //隐藏窗口标题栏 
	//long style = GetWindowLong(hParentWnd, GWL_STYLE);
	// &= ~(WS_SYSMENU);
	//SetWindowLong(hParentWnd, GWL_STYLE, style);

	g_byteBitDepthNo = 1;
	g_iHeight = 3120;
	g_iWidth = 4208;
	if(m_sensorInUse->StartCap(g_iHeight, g_iWidth, CapImgEntry,this)<0)
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
	cv::namedWindow("disp",0);
	cv::moveWindow("disp", 0, 0);
	m_bIsCapturing = true;
	SetDlgItemText(IDC_STATIC_STATUS, L"采集中...");
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
				//str.Format(L"%d Fps     %0.4f MBs", iFrameCntPerSec,float(iByteCntPerSec)/1024.0/1024.0);		 
				//HWND hWnd = (HWND)cvGetWindowHandle("disp");//获取子窗口的HWND
				//HWND hParentWnd = ::GetParent(hWnd);//获取父窗口HWND。父窗口是我们要用的			
				//if(hParentWnd !=NULL)
				//{
				//	::SetWindowText(hParentWnd,str);
				//}			
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
		m_sensorInUse->SetResolution(RESOLU_1280_720);
		g_byteResolutionType = RESOLU_1280_720;
		g_iWidth = 1280 ;
		g_iHeight = 720;
	}

	SetDlgItemText(IDC_STATIC_STATUS, L"分辨率1280x720。");
}


void CusbCamConsoleDlg::OnBnClickedRadioResolu1280960()
{
	// TODO: 在此添加控件通知处理程序代码
	if (!m_bUsbOpen)
	{
		SetDlgItemText(IDC_STATIC_STATUS, L"USB未打开。");
		return;
	}

	if (m_bIsCamSelected)
	{
		m_sensorInUse->SetResolution(RESOLU_1280_960);
		g_byteResolutionType = RESOLU_1280_960;
		g_iWidth = 1280 ;
		g_iHeight = 960;
	}

	SetDlgItemText(IDC_STATIC_STATUS, L"分辨率1280x960。");
}


void CusbCamConsoleDlg::OnBnClickedRadioResolu640480Skip()
{
	// TODO: 在此添加控件通知处理程序代码
	if (!m_bUsbOpen)
	{
		SetDlgItemText(IDC_STATIC_STATUS, L"USB未打开。");
		return;
	}

	if (m_bIsCamSelected)
	{
		m_sensorInUse->SetResolution(RESOLU_640_480_SKIP);
		g_byteResolutionType = RESOLU_640_480;
		g_iWidth = 640 ;
		//g_iHeight = 480;
		g_iHeight = 360 * 5;
	}

	SetDlgItemText(IDC_STATIC_STATUS, L"分辨率640x480skip。");
}


void CusbCamConsoleDlg::OnBnClickedRadioResolu640480Bin()
{
	// TODO: 在此添加控件通知处理程序代码
	if (!m_bUsbOpen)
	{
		SetDlgItemText(IDC_STATIC_STATUS, L"USB未打开。");
		return;
	}

	if (m_bIsCamSelected)
	{
		m_sensorInUse->SetResolution(RESOLU_640_480_BIN);
		g_byteResolutionType = RESOLU_640_480;
		g_iWidth = 640 ;
		//g_iHeight = 480;
		g_iHeight = 360 * 5;
	}

	SetDlgItemText(IDC_STATIC_STATUS, L"分辨率640x480bin。");
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
#if 0
	// TODO: 在此添加控件通知处理程序代码
	if (!m_bUsbOpen)
	{
		SetDlgItemText(IDC_STATIC_STATUS, L"USB未打开。");
		return;
	}
	(m_bIsCamSelected ? m_sensorInUse->SetTrigMode(TRIGMODE_SIGNAL) : NULL);
	GetDlgItem(IDC_EDIT_FPGATRIG_FREQ)->EnableWindow(false);
	SetDlgItemText(IDC_STATIC_STATUS, L"外部信号触发。");
#endif
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

	unsigned short iAddr = str2hex(strAddr);
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
	unsigned short iAddr = str2hex(strAddr);	
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
/*
struct arbFuncStruct
{
	int FuncNum;

	USB_ORDER order;
};*/
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
	const int buffsize = 256;
	cq_uint8_t		chData[buffsize];
	arbFuncStruct arbFunc;
	arbFunc.FuncNum = 0xBA;
	arbFunc.order.ReqCode = arbFunc.FuncNum;
	arbFunc.order.pData = chData;
	arbFunc.order.Direction = ORDER_OUT;
	arbFunc.order.DataBytes = 2;
	arbFunc.order.Value = 1;
	arbFunc.order.Index = 0;
	arbFunc.order.pData = chData;
	memcpy(chData, &iValue, sizeof(cq_uint16_t));
	m_sensorInUse->ArbitrFunc((void*)&arbFunc);

	//m_sensorInUse->WrEeprom(iAddr, iValue);
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
	const int buffsize = 256;
	cq_uint8_t		chData[buffsize];
	arbFuncStruct arbFunc;
	arbFunc.FuncNum = 0xBB;
	arbFunc.order.ReqCode = arbFunc.FuncNum;
	arbFunc.order.pData = chData;
	arbFunc.order.Direction = ORDER_IN;
	arbFunc.order.DataBytes = 2;
	arbFunc.order.Value = 1;
	arbFunc.order.Index = 0;
	arbFunc.order.pData = chData;
	m_sensorInUse->ArbitrFunc((void*)&arbFunc);

	CString strAddr;
	GetDlgItemText(IDC_EDIT_EEPROM_ADDR, strAddr);
	cq_uint32_t iAddr = str2hex(strAddr);
	cq_uint16_t irxval;
	cq_uint32_t len=1;
	memcpy(&irxval,chData,2);
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
		//MessageBox(L"-----");
		return TRUE;
	}
	if(m_iDevCnt<devCnt)
	{
		m_iDevCnt=devCnt;
		//MessageBox(L"+++++");
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



int CusbCamConsoleDlg::GetValue(int idc1,int idc2)
{
	int temp = 0;
	int rst = 0;
	temp = GetCheckedRadioButton(idc1, idc2) - idc1;
	rst = 1 << temp;
	return rst;
}
void CusbCamConsoleDlg::OnBnClickedRadioX1()
{
	OnBnClickedRadioCam1();

	CString str;
	GetDlgItemText(IDC_EDIT1, str);
	unsigned char roistep = _tstoi(str);
	str.ReleaseBuffer();

	mpls->setFunction(Func_ROI_STEP, roistep);

	//0,1,2,3
	//1,2,4,8
	mpls->setFunction(Func_ROI,GetValue(IDC_RADIO_X1, IDC_RADIO_Y2));
	

}

void CusbCamConsoleDlg::OnBnClickedRadioGain1()
{
	OnBnClickedRadioCam1();

	int GainX=GetValue(IDC_RADIO_GAIN1, IDC_RADIO_GAIN6);
	mpls->setFunction(Func_Gain, GainX);

}


void CusbCamConsoleDlg::OnBnClickedRadioSkip1()
{
	OnBnClickedRadioCam1();
	
	mpls->setFunction(Func_SKIP, GetCheckedRadioButton(IDC_RADIO_SKIP1, IDC_RADIO_SKIP3) - IDC_RADIO_SKIP1);
	
}


void CusbCamConsoleDlg::OnBnClickedRadioDp1()
{
	/*OnBnClickedRadioCam1();
	if (mpls->getSelectedCnt() > 1)
	{
		SetDlgItemText(IDC_STATIC_STATUS, L"只能选中一个相机");
		return;
	}*/
		
	int selected_disp = GetCheckedRadioButton(IDC_RADIO_DP1, IDC_RADIO_DP5) - IDC_RADIO_DP1;
	m_sensorInUse->WrFpgaReg(0x25, selected_disp);
	//mpls->setFunction(Func_DispSlot, selected_disp);
	//mpls->dispmtx.dispslot[selected_disp] = selected_cam;
}



void CusbCamConsoleDlg::OnBnClickedRadioDigx1()
{
	
	OnBnClickedRadioCam1();
	//x0 =1,x1=2 x2=4
	float resize =GetCheckedRadioButton(IDC_RADIO_DIGX1, IDC_RADIO_DIGX6) - IDC_RADIO_DIGX1+1;
	mpls->setFunction(Func_Zoom, resize);
	
}


void CusbCamConsoleDlg::OnBnClickedRadioCam1()
{
	//camera select
	for (int i = 0; i < 5; i++)
	{
		CButton* m_ctlCheck = (CButton*)GetDlgItem(IDC_CK_CAM1+i);
		mpls->setSelectCamera(i, m_ctlCheck->GetCheck());
	}
	CButton* m_ctlCheck = (CButton*)GetDlgItem(IDC_CK_CAM1 + 2);
	mpls->setSelectCamera(2, 1);
}


void CusbCamConsoleDlg::OnBnClickedRadioCx1()
{
	OnBnClickedRadioCam1();
	CString str;
	GetDlgItemText(IDC_EDIT_THICK, str);
	unsigned char crossThickness = _tstoi(str);
	GetDlgItemText(IDC_EDIT_THICK_STEP, str);
	unsigned char crossStep = _tstoi(str);
	str.ReleaseBuffer();

	int xy = GetCheckedRadioButton(IDC_RADIO_CX1, IDC_RADIO_CY2)- IDC_RADIO_CX1;

	mpls->setCrossParam(xy, crossStep, crossThickness);

}

void CusbCamConsoleDlg::OnBnClickedButtonExpoSet()
{
	OnBnClickedRadioCam1();
	CString str;
	GetDlgItemText(IDC_EDIT_EXPO, str);
	int expo = _tstoi(str);
	
	mpls->setFunction(Func_Expo, expo);
}


void CusbCamConsoleDlg::OnBnClickedButtonCrossSet()
{
	OnBnClickedRadioCam1();
	CString str;
	GetDlgItemText(IDC_EDIT_THICK, str);
	unsigned char crossThickness = _tstoi(str);
	mpls->setCrossParam(0, 0, crossThickness);

}

void CusbCamConsoleDlg::OnBnClickedButtonSaveConfig2()
{
	BOOL isOpen = FALSE;		//是否打开(否则为保存)
	CString defaultDir = L"";	//默认打开的文件路径
	CString fileName = L"camConf";			//默认打开的文件名
	CString filter = L"";	//文件过虑的类型
	CFileDialog openFileDlg(isOpen, defaultDir, fileName, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, filter, NULL);
	openFileDlg.GetOFN().lpstrInitialDir = L"";
	INT_PTR result = openFileDlg.DoModal();
	CString filePath ;
	if (result == IDOK) {
		filePath = openFileDlg.GetPathName();
	}
	//CWnd::SetDlgItemTextW(IDC_EDIT_DEST, filePath);
	ofstream outfile;
	outfile.open(filePath, ios::binary | ios::out);
	outfile.write((char*) mpls, sizeof(PLSFiveCam));

	Sleep(2000); 
	SetDlgItemText(IDC_STATIC_STATUS, L"保存成功");
}


void CusbCamConsoleDlg::OnBnClickedButtonLoadConfig()
{
	BOOL isOpen = TRUE;		//是否打开(否则为保存)
	CString defaultDir = L"";	//默认打开的文件路径
	CString fileName = L"camConf";			//默认打开的文件名
	CString filter = L"";	//文件过虑的类型
	CFileDialog openFileDlg(isOpen, defaultDir, fileName, OFN_HIDEREADONLY | OFN_READONLY, filter, NULL);
	openFileDlg.GetOFN().lpstrInitialDir = L"";
	INT_PTR result = openFileDlg.DoModal();
	CString filePath ;
	if (result == IDOK) {
		filePath = openFileDlg.GetPathName();
	}
	ifstream infile;
	infile.open(filePath, ios::binary | ios::in);
	PLSFiveCam* readconfig = new PLSFiveCam(m_sensorInUse);
	infile.read((char*)readconfig, sizeof(PLSFiveCam));
	mpls->readParams(readconfig);
	SetDlgItemText(IDC_STATIC_STATUS, L"读取成功");
	//delete readconfig;
}



void CusbCamConsoleDlg::OnBnClickedBtnSnap()
{
	f_snap = 1;

}

void CusbCamConsoleDlg::OnBnClickedButtonExpoSet3()
{
	CString str;
	GetDlgItemText(IDC_EDIT_GAMMA, str);
	gamma = _tstof(str);
	for (int i = 0; i < 256; i++)
	{
		lut[i] = cv::saturate_cast<uchar>(pow((float)(i / 255.0), gamma) * 255.0f);
	}
}
void CusbCamConsoleDlg::doGamma(cv::Mat& src, cv::Mat& dst)
{
	dst = src.clone();
	const int channels = dst.channels();


	cv::MatIterator_<uchar> it, end;
	for (it = dst.begin<uchar>(), end = dst.end<uchar>(); it != end; it++)
		//*it = pow((float)(((*it))/255.0), fGamma) * 255.0;
		*it = lut[(*it)];

}
