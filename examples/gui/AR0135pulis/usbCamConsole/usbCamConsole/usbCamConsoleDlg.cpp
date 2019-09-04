
// usbCamConsoleDlg.cpp : ʵ���ļ�
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
void  Disp(LPVOID lpParam)
{
	cq_uint8_t *pDataBuffer = (cq_uint8_t*)lpParam;
	int sig_width = 640;
	int sig_height= 360;
	int imglen = 640 * 360;
	cv::Mat framelist[5];
	cv::Mat rframelist[5];
	cv::Mat dispframelist[5];
	cv::Mat oneframe(360 * 5, 640, CV_8UC1, pDataBuffer);
	for (int i = 0; i < 5; i++)
	{
		cv::Mat tempframe(360, 640, CV_8UC1, pDataBuffer + i * imglen);
		framelist[i] = tempframe.clone();
	}
	cv::Mat mergeimg;
	mergeimg.create(1080, 1920, CV_8UC1);
	
	//framelist[0].copyTo(mergeimg(cv::Rect(sig_width * 0, sig_height*0, sig_width, sig_height)));//1
	//framelist[1].copyTo(mergeimg(cv::Rect(sig_width * 0, sig_height*2, sig_width, sig_height)));//2
	//framelist[2].copyTo(mergeimg(cv::Rect(sig_width * 1, sig_height*1,sig_width, sig_height)));//3
	//framelist[3].copyTo(mergeimg(cv::Rect(sig_width * 2, sig_height*0, sig_width, sig_height)));//4
	//framelist[4].copyTo(mergeimg(cv::Rect(sig_width * 2, sig_height*2, sig_width, sig_height)));//5
	try
	{

	for(int k=0;k<5;k++)
	{
		cv::resize(framelist[k],rframelist[k],cv::Size(framelist[k].cols* mplsdisp->mplsCam[k].dispmtx.zoom,framelist[k].rows* mplsdisp->mplsCam[k].dispmtx.zoom));
		int x=(mplsdisp->mplsCam[k].dispmtx.zoom-1)*sig_width/2;
		int y=(mplsdisp->mplsCam[k].dispmtx.zoom-1)*sig_height/2;
		
		dispframelist[k]=rframelist[k](cv::Rect(x,y,sig_width,sig_height));
		mplsdisp->mplsCam[k].imgDrawCross.drawCross(dispframelist[k],k);
	}
	dispframelist[mplsdisp->mplsCam[0].dispmtx.dispslot].copyTo(mergeimg(cv::Rect(sig_width * 0, sig_height*0, sig_width, sig_height)));//1
	dispframelist[mplsdisp->mplsCam[1].dispmtx.dispslot].copyTo(mergeimg(cv::Rect(sig_width * 0, sig_height*2, sig_width, sig_height)));//2
	dispframelist[mplsdisp->mplsCam[2].dispmtx.dispslot].copyTo(mergeimg(cv::Rect(sig_width * 1, sig_height*1,sig_width, sig_height)));//3
	dispframelist[mplsdisp->mplsCam[3].dispmtx.dispslot].copyTo(mergeimg(cv::Rect(sig_width * 2, sig_height*0, sig_width, sig_height)));//4
	dispframelist[mplsdisp->mplsCam[4].dispmtx.dispslot].copyTo(mergeimg(cv::Rect(sig_width * 2, sig_height*2, sig_width, sig_height)));//5
	//cv::imshow("resize",dispframelist[0]);
	cv::imshow("disp", mergeimg);
	//cv::imshow("disp", oneframe);
	cv::waitKey(1);
	/**************
	(0, 0)--11111111111--(w,0)--00000000000--(w*2,0)--44444444444--
	(0,h)---00000000000--(w,h)--33333333333--(w*2,h)--00000000000--
	(0,h*2)-22222222222--(w,h*2)--00000000000--(w*2,h*2)--55555555555--
	*****************/
	}
	catch (cv::Exception & e)
	{
		cerr << e.msg << endl; // output exception message
		std::wstring stemp = s2ws(e.msg);
		LPCWSTR result = stemp.c_str();
		OutputDebugString(result);
	}
}
// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
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


// CusbCamConsoleDlg �Ի���



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

	mpls->saveParams();
	mpls->readParams();
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
	ON_BN_CLICKED(IDC_BUTTON_EXPO_SET, &CusbCamConsoleDlg::OnBnClickedButtonExpoSet)
	ON_BN_CLICKED(IDC_BUTTON_CROSS_SET, &CusbCamConsoleDlg::OnBnClickedButtonCrossSet)
	ON_BN_CLICKED(IDC_BUTTON_SAVE_CONFIG2, &CusbCamConsoleDlg::OnBnClickedButtonSaveConfig2)
	ON_BN_CLICKED(IDC_BUTTON_LOAD_CONFIG, &CusbCamConsoleDlg::OnBnClickedButtonLoadConfig)
END_MESSAGE_MAP()


// CusbCamConsoleDlg ��Ϣ�������

BOOL CusbCamConsoleDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
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

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��
	
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
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
	CWnd:: SetWindowPos(&CWnd::wndTopMost,  0, 0, 0, 0, SWP_NOMOVE | SWP_NOREPOSITION | SWP_NOSIZE);
	cv::namedWindow("disp");
	cv::moveWindow("disp", 0, 0);
	HWND hWnd = (HWND)cvGetWindowHandle("disp");//��ȡ�Ӵ��ڵ�HWND
	HWND hParentWnd = ::GetParent(hWnd);//��ȡ������HWND��������������Ҫ�õ�
	 //���ش��ڱ����� 
	long style = GetWindowLong(hParentWnd, GWL_STYLE);
	style &= ~(WS_SYSMENU);
	SetWindowLong(hParentWnd, GWL_STYLE, style);
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CusbCamConsoleDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}




void CusbCamConsoleDlg::OnBnClickedButtonOpenUsb()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if (m_sensorInUse->OpenUSB(0)<0)
	{
		SetDlgItemText(IDC_STATIC_STATUS, L"��USBʧ�ܡ�");
		return;
	}
	m_bIsCamSelected=true;

	SetDlgItemText(IDC_STATIC_STATUS, L"��USB�ɹ���");
	m_bUsbOpen = true;
}


void CusbCamConsoleDlg::OnBnClickedButtonInitSensor()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if (!m_bUsbOpen)
	{
		SetDlgItemText(IDC_STATIC_STATUS, L"USBδ�򿪡�");
		return;
	}
	m_sensorInUse->InitSensor();
	SetDlgItemText(IDC_STATIC_STATUS, L"��ʼ��sensor�ɹ���");
}


void CusbCamConsoleDlg::OnBnClickedButtonCloseUsb()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if (!m_bUsbOpen)
	{
		SetDlgItemText(IDC_STATIC_STATUS, L"USBδ�򿪻��Ѿ��رա�");
		return;
	}
	OnBnClickedButtonStopCap();
	m_sensorInUse->CloseUSB();
	m_bUsbOpen = false;
}


void CusbCamConsoleDlg::OnBnClickedButtonStopCap()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if (false == m_bIsCapturing)
		return;

	WaitForSingleObject(g_mutexTimer, INFINITE);
	KillTimer(1);
	ReleaseMutex(g_mutexTimer);


	if(m_sensorInUse->StopCap()!=0)
	{
		SetDlgItemText(IDC_STATIC_STATUS,L"USB��δ�ɼ�");
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
	SetDlgItemText(IDC_STATIC_STATUS, L"ֹͣ�ɼ���");

	//h_vw.release();
}


void CusbCamConsoleDlg::OnBnClickedButtonVedioCap()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
		/************************************************************/
	if (!m_bUsbOpen)
	{
		SetDlgItemText(IDC_STATIC_STATUS, L"USBδ�򿪡�");
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
	//HWND hWnd = (HWND)cvGetWindowHandle("disp");//��ȡ�Ӵ��ڵ�HWND
	//HWND hParentWnd = ::GetParent(hWnd);//��ȡ������HWND��������������Ҫ�õ�
	// //���ش��ڱ����� 
	//long style = GetWindowLong(hParentWnd, GWL_STYLE);
	//style &= ~(WS_SYSMENU);
	//SetWindowLong(hParentWnd, GWL_STYLE, style);

	g_byteBitDepthNo = 1;
	g_iHeight = 360*5;
	g_iWidth = 640;
	if(m_sensorInUse->StartCap(g_iHeight, (g_byteBitDepthNo == 1 ? g_iWidth : g_iWidth * 2), Disp)<0)
	{
		SetDlgItemText(IDC_STATIC_STATUS, L"USB�豸��ʧ�ܣ�");
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
	SetDlgItemText(IDC_STATIC_STATUS, L"�ɼ���...");
}


void CusbCamConsoleDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
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
				//HWND hWnd = (HWND)cvGetWindowHandle("disp");//��ȡ�Ӵ��ڵ�HWND
				//HWND hParentWnd = ::GetParent(hWnd);//��ȡ������HWND��������������Ҫ�õ�			
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
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if (!m_bUsbOpen)
	{
		SetDlgItemText(IDC_STATIC_STATUS, L"USBδ�򿪡�");
		return;
	}

	if (m_bIsCamSelected)
	{
		m_sensorInUse->SetResolution(RESOLU_1280_720);
		g_byteResolutionType = RESOLU_1280_720;
		g_iWidth = 1280 ;
		g_iHeight = 720;
	}

	SetDlgItemText(IDC_STATIC_STATUS, L"�ֱ���1280x720��");
}


void CusbCamConsoleDlg::OnBnClickedRadioResolu1280960()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if (!m_bUsbOpen)
	{
		SetDlgItemText(IDC_STATIC_STATUS, L"USBδ�򿪡�");
		return;
	}

	if (m_bIsCamSelected)
	{
		m_sensorInUse->SetResolution(RESOLU_1280_960);
		g_byteResolutionType = RESOLU_1280_960;
		g_iWidth = 1280 ;
		g_iHeight = 960;
	}

	SetDlgItemText(IDC_STATIC_STATUS, L"�ֱ���1280x960��");
}


void CusbCamConsoleDlg::OnBnClickedRadioResolu640480Skip()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if (!m_bUsbOpen)
	{
		SetDlgItemText(IDC_STATIC_STATUS, L"USBδ�򿪡�");
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

	SetDlgItemText(IDC_STATIC_STATUS, L"�ֱ���640x480skip��");
}


void CusbCamConsoleDlg::OnBnClickedRadioResolu640480Bin()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if (!m_bUsbOpen)
	{
		SetDlgItemText(IDC_STATIC_STATUS, L"USBδ�򿪡�");
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

	SetDlgItemText(IDC_STATIC_STATUS, L"�ֱ���640x480bin��");
}

void CusbCamConsoleDlg::OnBnClickedRadioTrigmodeAuto()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if (!m_bUsbOpen)
	{
		SetDlgItemText(IDC_STATIC_STATUS, L"USBδ�򿪡�");
		return;
	}
	(m_bIsCamSelected ? m_sensorInUse->SetTrigMode(TRIGMODE_AUTO) : NULL);

	GetDlgItem(IDC_EDIT_FPGATRIG_FREQ)->EnableWindow(false);
	SetDlgItemText(IDC_STATIC_STATUS, L"�Զ�������");
}


void CusbCamConsoleDlg::OnBnClickedRadioTrigmodeFpga()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if (!m_bUsbOpen)
	{
		SetDlgItemText(IDC_STATIC_STATUS, L"USBδ�򿪡�");
		return;
	}
	(m_bIsCamSelected ? m_sensorInUse->SetTrigMode(TRIGMODE_FPGA) : NULL);

	GetDlgItem(IDC_EDIT_FPGATRIG_FREQ)->EnableWindow(true);
	SetDlgItemText(IDC_STATIC_STATUS, L"FPGA������");
}


void CusbCamConsoleDlg::OnBnClickedRadioTrigmodeSoft()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if (!m_bUsbOpen)
	{
		SetDlgItemText(IDC_STATIC_STATUS, L"USBδ�򿪡�");
		return;
	}
	(m_bIsCamSelected ? m_sensorInUse->SetTrigMode(TRIGMODE_SOFT) : NULL);
	GetDlgItem(IDC_EDIT_FPGATRIG_FREQ)->EnableWindow(false);
	SetDlgItemText(IDC_STATIC_STATUS, L"���������");
}


void CusbCamConsoleDlg::OnBnClickedRadioTrigmodeExt()
{
#if 0
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if (!m_bUsbOpen)
	{
		SetDlgItemText(IDC_STATIC_STATUS, L"USBδ�򿪡�");
		return;
	}
	(m_bIsCamSelected ? m_sensorInUse->SetTrigMode(TRIGMODE_SIGNAL) : NULL);
	GetDlgItem(IDC_EDIT_FPGATRIG_FREQ)->EnableWindow(false);
	SetDlgItemText(IDC_STATIC_STATUS, L"�ⲿ�źŴ�����");
#endif
}


void CusbCamConsoleDlg::OnBnClickedButtonWrSensor()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if (!m_bUsbOpen)
	{
		SetDlgItemText(IDC_STATIC_STATUS, L"USBδ�򿪡�");
		return;
	}
	CString strAddr, strValue;

	GetDlgItemText(IDC_EDIT_SENSOR_REGISTER_ADDR, strAddr);
	GetDlgItemText(IDC_EDIT_SENSOR_REGISTER_VALUE, strValue);

	unsigned short iAddr = str2hex(strAddr);
	unsigned short iValue = str2hex(strValue);

	(m_bIsCamSelected ? m_sensorInUse->WrSensorReg(iAddr, iValue) : NULL);
	SetDlgItemText(IDC_STATIC_STATUS, L"����Sensor�Ĵ����ɹ���");
}


void CusbCamConsoleDlg::OnBnClickedButtonRdSensor()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if (!m_bUsbOpen)
	{
		SetDlgItemText(IDC_STATIC_STATUS, L"USBδ�򿪡�");
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

	SetDlgItemText(IDC_STATIC_STATUS, L"��ȡSensor�Ĵ����ɹ���");
}


void CusbCamConsoleDlg::OnBnClickedButtonWrFpga()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if (!m_bUsbOpen)
	{
		SetDlgItemText(IDC_STATIC_STATUS, L"USBδ�򿪡�");
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
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if (!m_bUsbOpen)
	{
		SetDlgItemText(IDC_STATIC_STATUS, L"USBδ�򿪡�");
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
	// TODO:  ����ÿؼ��� RICHEDIT �ؼ���������
	// ���ʹ�֪ͨ��������д CDialogEx::OnInitDialog()
	// ���������� CRichEditCtrl().SetEventMask()��
	// ͬʱ�� ENM_CHANGE ��־�������㵽�����С�

	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	if (!m_bUsbOpen)
	{
		SetDlgItemText(IDC_STATIC_STATUS, L"USBδ�򿪡�");
		return;
	}

	SetDlgItemText(IDC_STATIC_STATUS, L"FPGA����Ƶ���趨�ɹ���");
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
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if (!m_bUsbOpen)
	{
		SetDlgItemText(IDC_STATIC_STATUS, L"USBδ�򿪡�");
		return;
	}
	(m_bIsCamSelected ? m_sensorInUse->SoftTrig(): NULL);
	SetDlgItemText(IDC_STATIC_STATUS, L"Soft������");
}


void CusbCamConsoleDlg::OnBnClickedRadioMirrorNormal()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if (!m_bUsbOpen)
	{
		SetDlgItemText(IDC_STATIC_STATUS, L"USBδ�򿪡�");
		return;
	}
	(m_bIsCamSelected ? m_sensorInUse->SetMirrorType(MIRROR_NORMAL) : NULL);
}


void CusbCamConsoleDlg::OnBnClickedRadioMirrorX()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if (!m_bUsbOpen)
	{
		SetDlgItemText(IDC_STATIC_STATUS, L"USBδ�򿪡�");
		return;
	}
	(m_bIsCamSelected ? m_sensorInUse->SetMirrorType(MIRROR_X) : NULL);
}


void CusbCamConsoleDlg::OnBnClickedRadioMirrorY()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if (!m_bUsbOpen)
	{
		SetDlgItemText(IDC_STATIC_STATUS, L"USBδ�򿪡�");
		return;
	}
	(m_bIsCamSelected ? m_sensorInUse->SetMirrorType(MIRROR_Y) : NULL);
}


void CusbCamConsoleDlg::OnBnClickedRadioMirrorXy()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if (!m_bUsbOpen)
	{
		SetDlgItemText(IDC_STATIC_STATUS, L"USBδ�򿪡�");
		return;
	}
	(m_bIsCamSelected ? m_sensorInUse->SetMirrorType(MIRROR_XY) : NULL);
}


void CusbCamConsoleDlg::setAutoGainExpo()
{
	if (!m_bUsbOpen)
	{
		SetDlgItemText(IDC_STATIC_STATUS, L"USBδ�򿪡�");
		return;
	}

	if ((cbAutoGain.GetCheck() == true) && (cbAutoExpo.GetCheck() == true))
	{
		(m_bIsCamSelected ? m_sensorInUse->SetAutoGainExpo(true, true) : NULL);
		SetDlgItemText(IDC_STATIC_STATUS, L"�Զ����棬�Զ��ع⡣");
	}	
	else if ((cbAutoGain.GetCheck() == true) && (cbAutoExpo.GetCheck() == false))
	{
		(m_bIsCamSelected ? m_sensorInUse->SetAutoGainExpo(true, false) : NULL);
		SetDlgItemText(IDC_STATIC_STATUS, L"�Զ����棬�ֶ��ع⡣");
	}
	else if ((cbAutoGain.GetCheck() == false) && (cbAutoExpo.GetCheck() == true))
	{
		(m_bIsCamSelected ? m_sensorInUse->SetAutoGainExpo(false, true) : NULL);
		SetDlgItemText(IDC_STATIC_STATUS, L"�ֶ����棬�Զ��ع⡣");
	}
	else if ((cbAutoGain.GetCheck() == false) && (cbAutoExpo.GetCheck() == false))
	{
		(m_bIsCamSelected ? m_sensorInUse->SetAutoGainExpo(false, false) : NULL);
		SetDlgItemText(IDC_STATIC_STATUS, L"�ֶ����棬�ֶ��ع⡣");
	}
	else;
}




void CusbCamConsoleDlg::OnEnChangeEditGainValue()
{
	// TODO:  ����ÿؼ��� RICHEDIT �ؼ���������
	// ���ʹ�֪ͨ��������д CDialogEx::OnInitDialog()
	// ���������� CRichEditCtrl().SetEventMask()��
	// ͬʱ�� ENM_CHANGE ��־�������㵽�����С�

	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	if (!m_bUsbOpen)
	{
		SetDlgItemText(IDC_STATIC_STATUS, L"USBδ�򿪡�");
		return;
	}
	
	CString str;
	GetDlgItemText(IDC_EDIT_GAIN_VALUE ,str);
	unsigned short GainValue= _tstoi(str);
	str.ReleaseBuffer();

	if(GainValue>0)
	{
		(m_bIsCamSelected ? m_sensorInUse->SetGainValue(GainValue & 0xffff) : NULL);
		SetDlgItemText(IDC_STATIC_STATUS, L"����ֵ�趨�ɹ���");
	}
	else
	{
		SetDlgItemText(IDC_STATIC_STATUS,L"Check Gain?");
	}
}


void CusbCamConsoleDlg::OnEnChangeEditExpoValue()
{
	// TODO:  ����ÿؼ��� RICHEDIT �ؼ���������
	// ���ʹ�֪ͨ��������д CDialogEx::OnInitDialog()
	// ���������� CRichEditCtrl().SetEventMask()��
	// ͬʱ�� ENM_CHANGE ��־�������㵽�����С�

	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	if (!m_bUsbOpen)
	{
		SetDlgItemText(IDC_STATIC_STATUS, L"USBδ�򿪡�");
		return;
	}
	
	CString str;
	GetDlgItemText(IDC_EDIT_EXPO_VALUE ,str);
	unsigned short ExpoValue= _tstoi(str);
	str.ReleaseBuffer();
	if(/*cbAutoExpo.GetCheck()==false&&*/ExpoValue>0)
	{
		(m_bIsCamSelected ? m_sensorInUse->SetExpoValue(ExpoValue) : NULL);
		SetDlgItemText(IDC_STATIC_STATUS, L"�ع�ֵ�趨�ɹ���");
	}
	else
	{
		SetDlgItemText(IDC_STATIC_STATUS,L"Check Expo?");
	}
}


void CusbCamConsoleDlg::OnBnClickedButtonCheckSpeed()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if (!m_bUsbOpen)
	{
		SetDlgItemText(IDC_STATIC_STATUS, L"USBδ�򿪡�");
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
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if (!m_bUsbOpen)
	{
		SetDlgItemText(IDC_STATIC_STATUS, L"USBδ�򿪡�");
		return;
	}
	CString strAddr, strValue;

	GetDlgItemText(IDC_EDIT_EEPROM_ADDR, strAddr);
	GetDlgItemText(IDC_EDIT_EEPROM_VALUE, strValue);

	cq_uint16_t iAddr = str2hex(strAddr);
	cq_uint16_t iValue = str2hex(strValue);

	m_sensorInUse->WrEeprom(iAddr, iValue);
	SetDlgItemText(IDC_STATIC_STATUS, L"дEEPROM��");
}


void CusbCamConsoleDlg::OnBnClickedButtonRdEeprom()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if (!m_bUsbOpen)
	{
		SetDlgItemText(IDC_STATIC_STATUS, L"USBδ�򿪡�");
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
	SetDlgItemText(IDC_STATIC_STATUS, L"��EEPROM��");
}


void CusbCamConsoleDlg::OnBnClickedButtonRdDevId()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if (!m_bUsbOpen)
	{
		SetDlgItemText(IDC_STATIC_STATUS, L"USBδ�򿪡�");
		return;
	}
	CString strValue;
	cq_uint8_t iValue = 0;
	cq_uint32_t len=1;
	m_sensorInUse->RdDevID(&iValue,len);
	strValue.Format(_T("%02x"), iValue);
	SetDlgItemText(IDC_EDIT_DEVID, strValue);
	SetDlgItemText(IDC_STATIC_STATUS, L"��ID�ɹ���");
}


void CusbCamConsoleDlg::OnBnClickedButtonWrDevId()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if (!m_bUsbOpen)
	{
		SetDlgItemText(IDC_STATIC_STATUS, L"USBδ�򿪡�");
		return;
	}
	CString strValue;
	GetDlgItemText(IDC_EDIT_DEVID, strValue);
	cq_uint8_t iValue = str2hex(strValue);
	cq_uint32_t len=1;
	m_sensorInUse->WrDevID(&iValue,len);
	SetDlgItemText(IDC_STATIC_STATUS, L"����ID�ɹ���");
}


void CusbCamConsoleDlg::OnBnClickedButtonRdDevSn()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if (!m_bUsbOpen)
	{
		SetDlgItemText(IDC_STATIC_STATUS, L"USBδ�򿪡�");
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
	SetDlgItemText(IDC_STATIC_STATUS, L"��SN�ɹ���");
}


void CusbCamConsoleDlg::OnBnClickedButtonWrDevSn()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if (!m_bUsbOpen)
	{
		SetDlgItemText(IDC_STATIC_STATUS, L"USBδ�򿪡�");
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
	SetDlgItemText(IDC_STATIC_STATUS, L"����SN�ɹ���");
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
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
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

	mpls->setFunction(Reg_ROI_STEP, roistep);

	//0,1,2,3
	//1,2,4,8
	mpls->setFunction(Reg_ROI,GetValue(IDC_RADIO_X1, IDC_RADIO_Y2));
	

}

void CusbCamConsoleDlg::OnBnClickedRadioGain1()
{
	OnBnClickedRadioCam1();

	wrSensorCmd(0, 0x305E, 0x2010);//set sensor gain =1;
	int GainX=GetValue(IDC_RADIO_GAIN1, IDC_RADIO_GAIN6);
	mpls->setFunction(Reg_Gain, GainX);

	
	
}


void CusbCamConsoleDlg::OnBnClickedRadioSkip1()
{
	OnBnClickedRadioCam1();
	
	mpls->setFunction(Reg_SKIP, GetCheckedRadioButton(IDC_RADIO_SKIP1, IDC_RADIO_SKIP3) - IDC_RADIO_SKIP1);
	
}


void CusbCamConsoleDlg::OnBnClickedRadioDp1()
{
	OnBnClickedRadioCam1();
	if (mpls->getSelectedCnt() > 1)
	{
		SetDlgItemText(IDC_STATIC_STATUS, L"ֻ��ѡ��һ�����");
		return;
	}
		
	int selected_disp = GetCheckedRadioButton(IDC_RADIO_DP1, IDC_RADIO_DP5) - IDC_RADIO_DP1;
	mpls->setFunction(Func_DispSlot, selected_disp);
	//mpls->dispmtx.dispslot[selected_disp] = selected_cam;
}



void CusbCamConsoleDlg::OnBnClickedRadioDigx1()
{
	
	OnBnClickedRadioCam1();
	//x0 =1,x1=2 x2=4
	float resize =GetCheckedRadioButton(IDC_RADIO_DIGX1, IDC_RADIO_DIGX6) - IDC_RADIO_DIGX1;
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
	
}


void CusbCamConsoleDlg::OnBnClickedRadioCx1()
{
	//cross disp ctrl
	CString str;
	GetDlgItemText(IDC_EDIT_THICK, str);
	unsigned char crossThickness = _tstoi(str);
	GetDlgItemText(IDC_EDIT_THICK_STEP, str);
	unsigned char crossStep = _tstoi(str);
	str.ReleaseBuffer();

	int xy = GetCheckedRadioButton(IDC_RADIO_CX1, IDC_RADIO_CY2) - IDC_RADIO_CX1;

	mpls->setCrossParam(xy, crossStep, crossThickness);

}

void CusbCamConsoleDlg::wrSensorCmd(int sensor, int sensorReg, int value)
{
	
}
void CusbCamConsoleDlg::OnBnClickedButtonExpoSet()
{
	CString str;
	GetDlgItemText(IDC_EDIT_EXPO, str);
	int expo = _tstoi(str);
	
	wrSensorCmd(0, 0x0202, expo);
}


void CusbCamConsoleDlg::OnBnClickedButtonCrossSet()
{
	CString str;
	GetDlgItemText(IDC_EDIT_THICK, str);
	unsigned char crossThickness = _tstoi(str);

	for (int i = 0; i < 5; i++)
	{
		if (mpls->mplsCam[i].checked == 1)
		{
			mpls->mplsCam[i].imgDrawCross.thickness = crossThickness;
		}
		
	}
}

void CusbCamConsoleDlg::OnBnClickedButtonSaveConfig2()
{
	
	mpls->saveParams();
	Sleep(2000); 
	SetDlgItemText(IDC_STATIC_STATUS, L"����ɹ�");
}


void CusbCamConsoleDlg::OnBnClickedButtonLoadConfig()
{
	mpls->readParams();
	SetDlgItemText(IDC_STATIC_STATUS, L"��ȡ�ɹ�");
}
