
// usbCamConsoleDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "usbCamConsole.h"
#include "usbCamConsoleDlg.h"
#include "afxdialogex.h"

#include "Pintai.h"

#include <iostream>
#include <fstream>

#include "ImgFrame.h"
#include<time.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define FLASH_PAGE_SIZE 0x100
#define FLASH_SECTOR_SIZE 0x1000
#define FLASH_FPGA_START_ADDR 4*1024*1024
#define FLASH_FPGA_END_ADDR 6*1024*1024

#define FLASH_FPGA_START_PAGE (FLASH_FPGA_START_ADDR/FLASH_PAGE_SIZE)//16384, 0x4000
#define FLASH_FPGA_END_PAGE  FLASH_FPGA_END_ADDR/FLASH_PAGE_SIZE
#define FPGA_FILE_SIZE 1191788

#define FLASH_USER_START_PAGE 6*1024*1024/FLASH_PAGE_SIZE
#define FLASH_USER_END_PAGE (FLASH_USER_START_PAGE+4096/FLASH_PAGE_SIZE)



cq_uint32_t     g_iWidth;
cq_uint32_t     g_iHeight;
cq_uint8_t		g_byteBitDepthNo;
cq_uint8_t		g_byteResolutionType;

int savefile = 0;
HANDLE g_mutexDisp;
HANDLE g_mutexTimer;
int show_channel = 1;
int g_camsize = 3;
byte* imgBuf = NULL;
byte* imgBuf1 = NULL;
byte* imgBuf2 = NULL;
CCameraCtrl camctrl;
int f_testStatus = 0;
void  CusbCamConsoleDlg::Disp(LPVOID lpParam)
{
	int i = 0, j = 0, k = 0;
	CImgFrame* imgframe = (CImgFrame*)lpParam;
	cq_uint8_t* pDataBuffer = imgframe->m_imgBuf;
	imgframe->m_timeStamp;
	imgframe->mode;
	imgframe->mode2;
	imgframe->temper;
	f_testStatus = 1;
	int offset = 0;
	if (imgBuf == NULL)
	{
		imgBuf = new byte[camctrl.cam[0].height * camctrl.cam[0].width];
	}
	if (imgBuf1 == NULL)
	{
		imgBuf1 = new byte[camctrl.cam[1].height * camctrl.cam[1].width];
	}
	if (imgBuf2 == NULL)
	{
		imgBuf2 = new byte[camctrl.cam[2].height * camctrl.cam[2].width];
	}

	offset = 0;
	if (show_channel==1 || show_channel == 4)
	{
		memcpy(imgBuf, pDataBuffer + offset, camctrl.cam[0].height * camctrl.cam[0].width);
		offset += camctrl.cam[0].height * camctrl.cam[0].width;
		cv::Mat frame(camctrl.cam[0].height, camctrl.cam[0].width, CV_8UC1, imgBuf);
		cv::imshow("disp", frame);
		if (savefile)
		{
			cv::imwrite("snap1.jpg", frame);
		}
	}
	
	if (show_channel==2 || show_channel == 4)
	{
		memcpy(imgBuf1, pDataBuffer + offset, camctrl.cam[1].height * camctrl.cam[1].width);
		offset += camctrl.cam[1].height * camctrl.cam[1].width;
		cv::Mat frame1(camctrl.cam[1].height, camctrl.cam[1].width, CV_8UC1, imgBuf1);
		cv::imshow("cam1", frame1);
		if (savefile)
		{
			cv::imwrite("snap2.jpg", frame1);
		}
	}

	if (show_channel==3|| show_channel==4)
	{
		memcpy(imgBuf2, pDataBuffer + offset, camctrl.cam[2].height * camctrl.cam[2].width);
		offset += camctrl.cam[2].height * camctrl.cam[2].width;
		cv::Mat frame2(camctrl.cam[2].height, camctrl.cam[2].width, CV_8UC1, imgBuf2);
		cv::imshow("cam2", frame2);
		if (savefile)
		{
			cv::imwrite("snap3.jpg", frame2);
		}
	}
	savefile = 0;
	cv::waitKey(1);

	//ReleaseMutex(g_mutexDisp);
}
// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    

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
void CusbCamConsoleDlg::cmdTest()
{
	//cq_uint8_t usbids[8];
	//USB_ORDER sUsbOrder;
	//sUsbOrder.ReqCode = 0xB0;
	//sUsbOrder.DataBytes = 8;
	//sUsbOrder.pData = usbids;
	//sUsbOrder.Direction = 1;
	//sUsbOrder.Index = 0;
	//sUsbOrder.Value = 0;
	//int rst = SendOrder(m_pUsbHandle, &sUsbOrder);

	arbFuncStruct arbFunc;
	cq_uint8_t		chData[8];//usb3.0, 64 for usb2.0b
	arbFunc.FuncNum = 0xB0;
	arbFunc.order.ReqCode = arbFunc.FuncNum;
	arbFunc.order.pData = chData;
	arbFunc.order.Direction = ORDER_IN;
	arbFunc.order.DataBytes = 8;
	m_sensorInUse->ArbitrFunc((void*)& arbFunc);
}
void CusbCamConsoleDlg::readUserFlash()
{
	arbFuncStruct arbFunc;
	const int buffsize = 256;
	cq_uint8_t		chData[buffsize];
arbFunc.FuncNum = 0xE0;
arbFunc.order.ReqCode = arbFunc.FuncNum;
arbFunc.order.pData = chData;
arbFunc.order.Direction = ORDER_IN;
arbFunc.order.DataBytes = 256;
char* memblock = new char[(FLASH_USER_END_PAGE-FLASH_USER_START_PAGE)*FLASH_PAGE_SIZE];
memset(memblock, 0, (FLASH_USER_END_PAGE - FLASH_USER_START_PAGE) * FLASH_PAGE_SIZE);
long offset = 0;
int pagecnt = 0;
for (int pagecnt = FLASH_USER_START_PAGE; pagecnt < FLASH_USER_END_PAGE; pagecnt++)
{
	arbFunc.order.Index = pagecnt;
	m_sensorInUse->ArbitrFunc((void*)& arbFunc);
	memcpy(memblock+ offset, arbFunc.order.pData, buffsize);
	offset += arbFunc.order.DataBytes;
}
delete[]memblock;

}
void CusbCamConsoleDlg::writeUserFlash()
{
	arbFuncStruct arbFunc;
	const int buffsize = 256;
	cq_uint8_t		chData[buffsize];
	arbFunc.FuncNum = 0xE1;
	arbFunc.order.ReqCode = arbFunc.FuncNum;
	arbFunc.order.pData = chData;
	arbFunc.order.Direction = ORDER_OUT;
	long offset = 0;
	int pagecnt = 0;
	arbFunc.order.DataBytes = 256;
	char* memblock = new char[(FLASH_USER_END_PAGE - FLASH_USER_START_PAGE) * FLASH_PAGE_SIZE];
	for (int i = 0; i < (FLASH_USER_END_PAGE - FLASH_USER_START_PAGE) * FLASH_PAGE_SIZE; i++)
	{
		memblock[i]=i;
	}
	for (int pagecnt = FLASH_USER_START_PAGE; pagecnt < FLASH_USER_END_PAGE; pagecnt++)
	{
		arbFunc.order.Index = pagecnt;
		memcpy(arbFunc.order.pData,memblock + offset, buffsize);
		offset += arbFunc.order.DataBytes;
		m_sensorInUse->ArbitrFunc((void*)& arbFunc);
		//pagecnt++;
	}
	delete[]memblock;
}
void CusbCamConsoleDlg::eraseUserFlash()
{
	arbFuncStruct arbFunc;
	const int buffsize = 128;
	cq_uint8_t		chData[buffsize];
	arbFunc.FuncNum = 0xE2;
	arbFunc.order.ReqCode = arbFunc.FuncNum;
	arbFunc.order.pData = chData;
	arbFunc.order.Direction = ORDER_IN;
	arbFunc.order.DataBytes = 4;
	arbFunc.order.Value = 1;//for user sector
	arbFunc.order.Index = 0;//erase sector num,++ to erase more sector
	m_sensorInUse->ArbitrFunc((void*)& arbFunc);


}
void  CusbCamConsoleDlg::readFlash()
{
	arbFuncStruct arbFunc;

	const int buffsize = 128;
	cq_uint8_t		chData[buffsize];//usb3.0, 64 for usb2.0b
	arbFunc.FuncNum = 0xE0;
	arbFunc.order.ReqCode = arbFunc.FuncNum;
	arbFunc.order.pData = chData;
	arbFunc.order.Direction = ORDER_IN;
	//index= page address=start from page 16384(4M*256)
	long offset = 0;
	int pagecnt = 0;
	arbFunc.order.DataBytes = 128;
	int totalpage = FPGA_FILE_SIZE / FLASH_PAGE_SIZE + 1;
	char* memblock = new char[totalpage* FLASH_PAGE_SIZE];

	for (int i = FLASH_FPGA_START_PAGE; i < totalpage+ FLASH_FPGA_START_PAGE; i++)
	{
		arbFunc.order.Index = pagecnt;
		m_sensorInUse->ArbitrFunc((void*)& arbFunc);
		memcpy(memblock, arbFunc.order.pData, buffsize);
	}
	delete []memblock;
}
void CusbCamConsoleDlg::eraseFlashSector()
{
	arbFuncStruct arbFunc;
	const int buffsize = 256;
	cq_uint8_t		chData[buffsize];//usb3.0, 64 for usb2.0
	arbFunc.FuncNum = 0xE2;
	arbFunc.order.ReqCode = arbFunc.FuncNum;
	arbFunc.order.pData = chData;
	arbFunc.order.Direction = ORDER_IN;
	//index= page address=start from page 
	long offset = 0;
	int pagecnt = 0;
	arbFunc.order.DataBytes = 1;
	arbFunc.order.Value = 1;//is erase
	for (int i = FLASH_FPGA_START_PAGE; i < FLASH_FPGA_END_PAGE; i++)
	{
		arbFunc.order.Index = pagecnt;
		m_sensorInUse->ArbitrFunc((void*)& arbFunc);
	}
}
void CusbCamConsoleDlg::writeFPGAfile()
{
	long size;
	char* memblock;

	ifstream file("output_file.rbf", ios::in | ios::binary | ios::ate);
	if (file.is_open())
	{
		size = (long)file.tellg();//435403
		memblock = new char[size];

		file.seekg(0, ios::beg);
		file.read(memblock, size);
		file.close();
	
		arbFuncStruct arbFunc;
		const int buffsize = 256;
		cq_uint8_t		chData[buffsize];//usb3.0, 64 for usb2.0
		//erase fpga sector;
		arbFunc.FuncNum = 0xE2;
		int sector = 0;
		arbFunc.order.ReqCode = arbFunc.FuncNum;
		arbFunc.order.pData = chData;
		chData[0] = 2;
		arbFunc.order.Value = 2;
		arbFunc.order.Direction = ORDER_IN;
		for (int sector = 0; sector < size / FLASH_SECTOR_SIZE +2; sector++)//sector 0 for file size
		{
			arbFunc.order.Index = sector;
			m_sensorInUse->ArbitrFunc((void*)& arbFunc);
		}

		//write file size
		arbFunc.FuncNum = 0xE1;
		arbFunc.order.ReqCode = arbFunc.FuncNum;
		arbFunc.order.pData = chData;
		arbFunc.order.Direction = ORDER_OUT;
		arbFunc.order.DataBytes = 4;
		arbFunc.order.Index = FLASH_FPGA_START_PAGE;
		chData[0] = size & 0xff;
		chData[1] = size >> 8 & 0xff;
		chData[2] = size >> 16 & 0xff;
		chData[3] = size >> 24 & 0xff;
		m_sensorInUse->ArbitrFunc((void*)& arbFunc);
		//start write file to flash
		arbFunc.FuncNum = 0xE1;
		arbFunc.order.ReqCode = arbFunc.FuncNum;
		arbFunc.order.pData = chData;
		arbFunc.order.Direction = ORDER_OUT;
		arbFunc.order.DataBytes = buffsize;
		long offset = 0;
		int pagecnt = 1;//page 0 is taken for file size
		clock_t start, finish;
		double totaltime;
		start = clock();
		while (offset < size)
		{

			arbFunc.order.Index = FLASH_FPGA_START_PAGE + pagecnt;

			if (offset + buffsize <= size)
			{
				arbFunc.order.DataBytes = buffsize;
				memcpy(chData, memblock + offset, buffsize);
				offset += buffsize;
			}
			else if (offset + buffsize > size)
			{
				long wrsize = size - offset;
				arbFunc.order.DataBytes = wrsize;
				memcpy(chData, memblock + offset, wrsize);
				offset += wrsize;
			}
			m_sensorInUse->ArbitrFunc((void*)& arbFunc);
			pagecnt++;

		}

		finish = clock();
		totaltime = (double)(finish - start) / CLOCKS_PER_SEC;
		delete[] memblock;
	}
}
void CusbCamConsoleDlg::readEE()
{
	arbFuncStruct arbFunc;
	const int buffsize = 256;
	cq_uint8_t		chData[buffsize];
	arbFunc.FuncNum = 0xBB;
	arbFunc.order.ReqCode = arbFunc.FuncNum;
	arbFunc.order.pData = chData;
	arbFunc.order.Direction = ORDER_IN;
	arbFunc.order.DataBytes = 4;
	arbFunc.order.Value = 1;
	arbFunc.order.Index = 0;
	m_sensorInUse->ArbitrFunc((void*)& arbFunc);
	int data=chData[0];
}
void  CusbCamConsoleDlg::writeEE()
{
	arbFuncStruct arbFunc;
	const int buffsize = 256;
	cq_uint8_t		chData[buffsize];
	arbFunc.FuncNum = 0xBA;
	arbFunc.order.ReqCode = arbFunc.FuncNum;
	arbFunc.order.pData = chData;
	arbFunc.order.Direction = ORDER_OUT;
	arbFunc.order.Value = 1;
	arbFunc.order.Index = 0;
	arbFunc.order.DataBytes = 4;
	chData[0] = 0x11;
	chData[1] = 0x22;
	chData[2] = 0x33;
	chData[3] = 0x44;
	m_sensorInUse->ArbitrFunc((void*)& arbFunc);
}
void CusbCamConsoleDlg::readFpgaFile()
{
	long size;
	char* memblock;

	ifstream file("output_file.rbf", ios::in | ios::binary | ios::ate);
	if (file.is_open())
	{
		//file size is 1191788
		//from ue 1191787
		size = (long)file.tellg();
		memblock = new char[size];
		
		file.seekg(0, ios::beg);
		file.read(memblock, size);
		file.close();
		//write to flash
		arbFuncStruct arbFunc;
		const int buffsize = 512;
		cq_uint8_t		chData[buffsize];//usb3.0, 64 for usb2.0
		//arbFunc.FuncNum = 0xE0;//read flash
		//arbFunc.FuncNum = 0xE1;//write flash
		//arbFunc.FuncNum = 0xE2;//sector erase
		//arbFunc.FuncNum = 0xE3;//config
		arbFunc.FuncNum = 0xE4;
		arbFunc.order.ReqCode = arbFunc.FuncNum;
		arbFunc.order.pData = chData;
		arbFunc.order.Direction = ORDER_IN;
		arbFunc.order.DataBytes = 1;
		m_sensorInUse->ArbitrFunc((void*)& arbFunc);


		arbFunc.FuncNum = 0xE5;
		arbFunc.order.ReqCode = arbFunc.FuncNum;
		arbFunc.order.pData = chData;
		arbFunc.order.Direction = ORDER_OUT;
		arbFunc.order.DataBytes = buffsize;
		long offset = 0;
		int pagecnt = 0;
		clock_t start, finish;
		double totaltime;
		start = clock();
		while(offset<size)
		{

			arbFunc.order.Index = FLASH_FPGA_START_PAGE + pagecnt;

			if (offset + buffsize <= size)
			{
				arbFunc.order.DataBytes = buffsize;
				memcpy(chData, memblock + offset, buffsize);
				offset += buffsize;
			}
			else if (offset + buffsize > size)
			{
				long wrsize = size - offset;
				arbFunc.order.DataBytes = wrsize;
				memcpy(chData, memblock + offset, wrsize);
				offset += wrsize;
			}
			m_sensorInUse->ArbitrFunc((void*)&arbFunc);
			pagecnt++;

		}

		finish = clock();
		totaltime = (double)(finish - start) / CLOCKS_PER_SEC;
		arbFunc.FuncNum = 0xE6;
		arbFunc.order.ReqCode = arbFunc.FuncNum;
		arbFunc.order.pData = chData;
		arbFunc.order.Direction = ORDER_IN;
		arbFunc.order.DataBytes = 1;
		m_sensorInUse->ArbitrFunc((void*)& arbFunc);

		delete[] memblock;
	}
	else cout << "Unable to open file";
}

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
	g_iHeight=960*3;
	g_byteBitDepthNo=1;
	m_bIsCapturing = false;

	show_channel = 0;;
	g_camsize = 3;
	imgBuf = NULL;
	imgBuf1 = NULL;
	imgBuf2 = NULL;

	camctrl.init(m_sensorInUse);
}
CusbCamConsoleDlg::~CusbCamConsoleDlg()
{
	OnBnClickedButtonStopCap();

}

void CusbCamConsoleDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_CHECK_AUTOGAIN, cbAutoGain);
	DDX_Control(pDX, IDC_COMBO1, selectChannel);
	DDX_Control(pDX, IDC_FUNC_GENERAL, selectGenFunc);
	DDX_Control(pDX, IDC_FUNC_CAM, selectCamFunc);
}

BEGIN_MESSAGE_MAP(CusbCamConsoleDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_OPEN_USB, &CusbCamConsoleDlg::OnBnClickedButtonOpenUsb)
	ON_BN_CLICKED(IDC_BUTTON_CLOSE_USB, &CusbCamConsoleDlg::OnBnClickedButtonCloseUsb)
	ON_BN_CLICKED(IDC_BUTTON_STOP_CAP, &CusbCamConsoleDlg::OnBnClickedButtonStopCap)
	ON_BN_CLICKED(IDC_BUTTON_VEDIO_CAP, &CusbCamConsoleDlg::OnBnClickedButtonVedioCap)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_RADIO_RESOLU_1280_1024, &CusbCamConsoleDlg::OnBnClickedRadioResolu12801024)
	ON_BN_CLICKED(IDC_RADIO_TRIGMODE_AUTO, &CusbCamConsoleDlg::OnBnClickedRadioTrigmodeAuto)
	ON_BN_CLICKED(IDC_RADIO_TRIGMODE_FPGA, &CusbCamConsoleDlg::OnBnClickedRadioTrigmodeFpga)
	ON_BN_CLICKED(IDC_RADIO_TRIGMODE_SOFT, &CusbCamConsoleDlg::OnBnClickedRadioTrigmodeSoft)
	ON_BN_CLICKED(IDC_RADIO_TRIGMODE_EXT, &CusbCamConsoleDlg::OnBnClickedRadioTrigmodeExt)
	ON_EN_CHANGE(IDC_EDIT_FPGATRIG_FREQ, &CusbCamConsoleDlg::OnEnChangeEditFpgatrigFreq)
	ON_BN_CLICKED(IDC_RADIO_MIRROR_NORMAL, &CusbCamConsoleDlg::OnBnClickedRadioMirrorNormal)
	ON_BN_CLICKED(IDC_RADIO_MIRROR_X, &CusbCamConsoleDlg::OnBnClickedRadioMirrorX)
	ON_BN_CLICKED(IDC_RADIO_MIRROR_Y, &CusbCamConsoleDlg::OnBnClickedRadioMirrorY)
	ON_BN_CLICKED(IDC_RADIO_MIRROR_XY, &CusbCamConsoleDlg::OnBnClickedRadioMirrorXy)
	ON_BN_CLICKED(IDC_CHECK_AUTOGAIN, &CusbCamConsoleDlg::setAutoGainExpo)
	ON_BN_CLICKED(IDC_BUTTON_WR_DEV_SN, &CusbCamConsoleDlg::OnBnClickedButtonWrDevSn)
	ON_CBN_SELCHANGE(IDC_COMBO1, &CusbCamConsoleDlg::OnCbnSelchangeCombo1)
	ON_BN_CLICKED(IDC_BUTTON_CAM_FUNC, &CusbCamConsoleDlg::OnBnClickedButtonCamFunc)
	ON_BN_CLICKED(IDC_BUTTON_GEN_FUNC, &CusbCamConsoleDlg::OnBnClickedButtonGenFunc)
	ON_BN_CLICKED(IDC_BUTTON_TEST, &CusbCamConsoleDlg::OnBnClickedButtonTest)
	ON_BN_CLICKED(IDC_BUTTON_EE, &CusbCamConsoleDlg::OnBnClickedButtonTest2)

	ON_BN_CLICKED(IDC_BUTTON_RDTEMP, &CusbCamConsoleDlg::OnBnClickedButtonRdtemp)
	ON_BN_CLICKED(IDC_RADIO_MOTOR, &CusbCamConsoleDlg::OnBnClickedRadioIo1)
	ON_BN_CLICKED(IDC_RADIO_FAN, &CusbCamConsoleDlg::OnBnClickedRadioIo1)
	ON_BN_CLICKED(IDC_RADIO_IO0, &CusbCamConsoleDlg::OnBnClickedRadioIo1)
	ON_BN_CLICKED(IDC_RADIO_IO1, &CusbCamConsoleDlg::OnBnClickedRadioIo1)
	ON_BN_CLICKED(IDC_BUTTON_SET640, &CusbCamConsoleDlg::OnBnClickedButtonSet640)
	ON_BN_CLICKED(IDC_BUTTON_RDEE, &CusbCamConsoleDlg::OnBnClickedButtonRdee)
	ON_WM_DEVICECHANGE()
	ON_WM_CLOSE()
	
	
END_MESSAGE_MAP()


BOOL CusbCamConsoleDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

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


	SetIcon(m_hIcon, TRUE);			
	SetIcon(m_hIcon, FALSE);		

	selectChannel.InsertString(0, _T("0"));
	selectChannel.InsertString(1, _T("1"));
	selectChannel.InsertString(2, _T("2"));
	selectChannel.InsertString(3, _T("ALL"));
	int j = 0;
	selectGenFunc.InsertString(j++, _T("IO_1(0,1)"));
	selectGenFunc.InsertString(j++, _T("Motor(0,1)"));
	selectGenFunc.InsertString(j++, _T("Fan(0,1)"));
	selectGenFunc.InsertString(j++, _T("IO_0(0,1)"));
	selectGenFunc.InsertString(j++, _T("CamMode(0~3)"));
	selectGenFunc.InsertString(j++, _T("CamCnt(0~6)"));
	selectGenFunc.InsertString(j++, _T("T1(0~1000)"));
	selectGenFunc.InsertString(j++, _T("T2(0~1000)"));
	selectGenFunc.InsertString(j++, _T("T3(0~1000)"));
	selectGenFunc.InsertString(j++, _T("T4(0~1000)"));
	selectGenFunc.InsertString(j++, _T("T5(0~1000)"));
	
	j = 0;
	selectCamFunc.InsertString(j++, _T("ROI_Y_Start(0~959"));
	selectCamFunc.InsertString(j++, _T("ROI_X_Start(0~1279"));
	selectCamFunc.InsertString(j++, _T("ROI_Y_End(0~959"));
	selectCamFunc.InsertString(j++, _T("ROI_X_End(0~1279"));
	selectCamFunc.InsertString(j++, _T("Auto Gain(0,2"));
	selectCamFunc.InsertString(j++, _T("Expo(0~65535)"));
	selectCamFunc.InsertString(j++, _T("GainF(0~255)"));
	selectCamFunc.InsertString(j++, _T("GainG1(0~255)"));
	selectCamFunc.InsertString(j++, _T("GainB(0~255)"));
	selectCamFunc.InsertString(j++, _T("GainR(0~255)"));
	selectCamFunc.InsertString(j++, _T("GainG2(0~255)"));
	selectCamFunc.InsertString(j++, _T("Mirror(0,4,8,12)"));
	selectCamFunc.InsertString(j++, _T("Analog Gain(1,2,4,8)"));
	selectCamFunc.InsertString(j++, _T("PWM(0~100)"));

	/*((CButton*)GetDlgItem(IDC_CHECK_CAM1))->SetCheck(1);
	((CButton*)GetDlgItem(IDC_CHECK_CAM2))->SetCheck(1);
	((CButton*)GetDlgItem(IDC_CHECK_CAM3))->SetCheck(1);*/
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
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CusbCamConsoleDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}




void CusbCamConsoleDlg::OnBnClickedButtonOpenUsb()
{
	CString str;
	GetDlgItemText(IDC_EDIT_USBNUM, str);
	unsigned char usbnum = _tstoi(str);
	if (usbnum < 0)
		usbnum = 0;
	if (m_sensorInUse->OpenUSB(usbnum)<0)
	{
		SetDlgItemText(IDC_STATIC_STATUS, L"Usb Open failed");
		return;
	}
	m_bIsCamSelected=true;

	SetDlgItemText(IDC_STATIC_STATUS, L"usb OK");
	m_bUsbOpen = true;


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



void CusbCamConsoleDlg::OnBnClickedButtonCloseUsb()
{
	// TODO: �ڴ����ӿؼ�֪ͨ�����������
	if (!m_bUsbOpen)
	{
		SetDlgItemText(IDC_STATIC_STATUS, L"USB Closed");
		return;
	}
	OnBnClickedButtonStopCap();
	m_sensorInUse->CloseUSB();
	m_bUsbOpen = false;
}


void CusbCamConsoleDlg::OnBnClickedButtonStopCap()
{
	// TODO: �ڴ����ӿؼ�֪ͨ�����������
	if (false == m_bIsCapturing)
		return;

	WaitForSingleObject(g_mutexTimer, INFINITE);
	KillTimer(1);
	ReleaseMutex(g_mutexTimer);


	if(m_sensorInUse->StopCap()!=0)
	{
		SetDlgItemText(IDC_STATIC_STATUS,L"USB stop error");
		return;
	}
	WaitForSingleObject(g_mutexDisp, INFINITE);
	cv::destroyWindow("disp");
	cv::destroyWindow("cam1");
	cv::destroyWindow("cam2");
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
	SetDlgItemText(IDC_STATIC_STATUS, L"ͣusb closed");
	if (imgBuf != NULL)
	{
		delete imgBuf;
		imgBuf = NULL;
	}
	if (imgBuf1 != NULL)
	{
		delete imgBuf1;
		imgBuf1 = NULL;
	}
	if (imgBuf2 != NULL)
	{
		delete imgBuf2;
		imgBuf2 = NULL;
	}
	//h_vw.release();
}


void CusbCamConsoleDlg::OnBnClickedButtonVedioCap()
{
	// TODO: �ڴ����ӿؼ�֪ͨ�����������
		/************************************************************/
	if (!m_bUsbOpen)
	{
		SetDlgItemText(IDC_STATIC_STATUS, L"USB start cap");
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
	cv::namedWindow("cam1");
	cv::namedWindow("cam2");
	HWND hWnd = (HWND)cvGetWindowHandle("disp");//��ȡ�Ӵ��ڵ�HWND
	HWND hParentWnd = ::GetParent(hWnd);//��ȡ������HWND��������������Ҫ�õ�
	 //���ش��ڱ����� 
	long style = GetWindowLong(hParentWnd, GWL_STYLE);
	style &= ~(WS_SYSMENU);
	SetWindowLong(hParentWnd, GWL_STYLE, style);
	camctrl.getAllRes();

	if (m_sensorInUse->StartCap(camctrl.getTotalDataLen(), 1, CapImgEntry, this)<0)
	{
		SetDlgItemText(IDC_STATIC_STATUS, L"cap error");
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
	SetDlgItemText(IDC_STATIC_STATUS, L"cap...");
}


void CusbCamConsoleDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: �ڴ�������Ϣ������������/�����Ĭ��ֵ
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
				HWND hWnd = (HWND)cvGetWindowHandle("disp");
				HWND hParentWnd = ::GetParent(hWnd);		
				if(hParentWnd !=NULL)
				{
					::SetWindowText(hParentWnd,str);
				}			
				/*
				if (f_testStatus <2)
				{
					camctrl.setGenFunction(3, 0);
					Sleep(500);
					for (int ctrlcnt = 0; ctrlcnt < 5; ctrlcnt++)
					{
						Sleep(10);
						camctrl.setGenFunction(0, 1);
					}
					Sleep(20);
					camctrl.setGenFunction(3, 1);
					
					f_testStatus = 2;
				}
				*/
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
	// TODO: �ڴ����ӿؼ�֪ͨ�����������
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
	// TODO: �ڴ����ӿؼ�֪ͨ�����������
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
	// TODO: �ڴ����ӿؼ�֪ͨ�����������
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
		g_iHeight = 480;
	}

	SetDlgItemText(IDC_STATIC_STATUS, L"�ֱ���640x480skip��");
}


void CusbCamConsoleDlg::OnBnClickedRadioResolu640480Bin()
{
	// TODO: �ڴ����ӿؼ�֪ͨ�����������
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
		g_iHeight = 480;
	}

	SetDlgItemText(IDC_STATIC_STATUS, L"�ֱ���640x480bin��");
}

void CusbCamConsoleDlg::OnBnClickedRadioTrigmodeAuto()
{
	// TODO: �ڴ����ӿؼ�֪ͨ�����������
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
	// TODO: �ڴ����ӿؼ�֪ͨ�����������
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
	// TODO: �ڴ����ӿؼ�֪ͨ�����������
	if (!m_bUsbOpen)
	{
		SetDlgItemText(IDC_STATIC_STATUS, L"USBδ�򿪡�");
		return;
	}
	(m_bIsCamSelected ? m_sensorInUse->SetTrigMode(TRIGMODE_SOFT) : NULL);
	GetDlgItem(IDC_EDIT_FPGATRIG_FREQ)->EnableWindow(false);
	SetDlgItemText(IDC_STATIC_STATUS, L"����������");
}


void CusbCamConsoleDlg::OnBnClickedRadioTrigmodeExt()
{
#if 0
	// TODO: �ڴ����ӿؼ�֪ͨ�����������
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
	// TODO: �ڴ����ӿؼ�֪ͨ�����������
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
	// TODO: �ڴ����ӿؼ�֪ͨ�����������
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
	// TODO: �ڴ����ӿؼ�֪ͨ�����������
	if (!m_bUsbOpen)
	{
		SetDlgItemText(IDC_STATIC_STATUS, L"USBδ�򿪡�");
		return;
	}
	CString strAddr, strValue;

	GetDlgItemText(IDC_EDIT_FPGA_REGISTER_ADDR, strAddr);
	GetDlgItemText(IDC_EDIT_FPGA_REGISTER_VALUE, strValue);

	unsigned long iAddr = str2hex(strAddr);
	unsigned long iValue = str2hex(strValue);

	//(m_bIsCamSelected ? m_sensorInUse->WrFpgaReg(iAddr, iValue) : NULL);
	camctrl.cam[0].wrCamCmd(iAddr, iValue);

}


void CusbCamConsoleDlg::OnBnClickedButtonRdFpga()
{
	// TODO: �ڴ����ӿؼ�֪ͨ�����������
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
unsigned int CusbCamConsoleDlg::str2hex(CString str)
{
	int nLength = str.GetLength();
	int nBytes = WideCharToMultiByte(CP_ACP, 0, str, nLength, NULL, 0, NULL, NULL);
	char* p = new char[nBytes + 1];
	memset(p, 0, nLength + 1);
	WideCharToMultiByte(CP_OEMCP, 0, str, nLength, p, nBytes, NULL, NULL);
	p[nBytes] = 0;
	unsigned int a;
	sscanf(p, "%x", &a);
	delete[] p;
	return a;

}

void CusbCamConsoleDlg::OnEnChangeEditFpgatrigFreq()
{

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
	// TODO: �ڴ����ӿؼ�֪ͨ�����������
	if (!m_bUsbOpen)
	{
		SetDlgItemText(IDC_STATIC_STATUS, L"USBδ�򿪡�");
		return;
	}
	(m_bIsCamSelected ? m_sensorInUse->SoftTrig(): NULL);
	SetDlgItemText(IDC_STATIC_STATUS, L"Soft������");
}

void  CusbCamConsoleDlg::SetDlgText(int dlg,int v)
{
	CString temp;
	temp.Format(_T("%d"), v);
	SetDlgItemText(dlg, temp);
}
void CusbCamConsoleDlg::OnBnClickedRadioMirrorNormal()
{
	
	SetDlgText(IDC_EDIT_SENSOR_REGISTER_VALUE, 0);
}


void CusbCamConsoleDlg::OnBnClickedRadioMirrorX()
{
	cq_uint32_t v = 0x40 << 8;	
	SetDlgText(IDC_EDIT_SENSOR_REGISTER_VALUE,v );
}


void CusbCamConsoleDlg::OnBnClickedRadioMirrorY()
{
	cq_uint32_t v = 0x80 << 8;
	SetDlgText(IDC_EDIT_SENSOR_REGISTER_VALUE,v );
}


void CusbCamConsoleDlg::OnBnClickedRadioMirrorXy()
{
	cq_uint32_t v = 0xC0 << 8;
	SetDlgText(IDC_EDIT_SENSOR_REGISTER_VALUE, v);
}


void CusbCamConsoleDlg::setAutoGainExpo()
{
	int agae = 0;	
	if ((cbAutoGain.GetCheck() == true))
	{
		agae = 2;
	//	(m_bIsCamSelected ? m_sensorInUse->SetAutoGainExpo(true, false) : NULL);
		SetDlgItemText(IDC_STATIC_STATUS, L"�Զ����棬�ֶ��ع⡣");
	}
	else if ((cbAutoGain.GetCheck() == false))
	{
		agae = 0;
//		(m_bIsCamSelected ? m_sensorInUse->SetAutoGainExpo(false, false) : NULL);
		SetDlgItemText(IDC_STATIC_STATUS, L"�ֶ����棬�ֶ��ع⡣");
	}
	else;
	CString temp;
	temp.Format(_T("%d"), agae);
	SetDlgItemText(IDC_EDIT_SENSOR_REGISTER_VALUE,temp);
	//camctrl.setCamFunction(Func_SetAGAE, agae);

}


void CusbCamConsoleDlg::OnBnClickedButtonRdDevSn()
{
	// TODO: �ڴ����ӿؼ�֪ͨ�����������
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
	// TODO: �ڴ����ӿؼ�֪ͨ�����������
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

BOOL CusbCamConsoleDlg::OnDeviceChange(UINT nEventType, DWORD_PTR dwData)
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
	OnBnClickedButtonStopCap();
	CDialogEx::OnClose();
}


void CusbCamConsoleDlg::OnCbnSelchangeCombo1()
{
	
	for (int i = 0; i < 3; i++)
	{
		camctrl.cam[i].checked = 0;
	}
	show_channel = selectChannel.GetCurSel();
	camctrl.cam[show_channel].checked = 1;
}


void CusbCamConsoleDlg::OnBnClickedButtonCamFunc()
{

	
	OnCbnSelchangeCombo1();
	int camfunc = selectCamFunc.GetCurSel();
	CString str;
	GetDlgItemText(IDC_EDIT_SENSOR_REGISTER_VALUE, str);
	int v = _tstoi(str);
	if (v >=0)
	{
		camctrl.setCamFunction(camfunc, v);
	}

	
	

}


void CusbCamConsoleDlg::OnBnClickedButtonGenFunc()
{
	OnCbnSelchangeCombo1();
	int genfunc = selectGenFunc.GetCurSel();
	CString str;
	GetDlgItemText(IDC_EDIT_SENSOR_REGISTER_ADDR, str);
	int v = _tstoi(str);
	if (v >= 0)
	{
		camctrl.setGenFunction(genfunc, v);
	}
}


void CusbCamConsoleDlg::OnBnClickedButtonWrSen()
{
	OnCbnSelchangeCombo1();
	CString strAddr, strValue;

	GetDlgItemText(IDC_EDIT_SENSOR_REGISTER_ADDR, strAddr);
	GetDlgItemText(IDC_EDIT_SENSOR_REGISTER_VALUE, strValue);

	unsigned short iAddr = str2hex(strAddr);
	unsigned short iValue = str2hex(strValue);
	int temp = iAddr << 16 + iValue;
	camctrl.wrCamCmd(0x33bb011f, temp);
}



void CusbCamConsoleDlg::OnBnClickedButtonSave()
{
	savefile = 1;
}


void CusbCamConsoleDlg::OnBnClickedButtonTest()
{
	//writeFPGAfile();
	//readFpgaFile();
	/*
	writeEE();
	Sleep(100);
	readEE();
	readUserFlash();
	eraseUserFlash();
	writeUserFlash();
	*/
	
}


void CusbCamConsoleDlg::OnBnClickedButtonTest2()
{
	int pos = 0;
	int value = 0;
	CString str;
	arbFuncStruct arbFunc;
	const int buffsize = 256;
	cq_uint8_t		chData[buffsize];
	arbFunc.FuncNum = 0xBA;
	arbFunc.order.ReqCode = arbFunc.FuncNum;
	arbFunc.order.pData = chData;
	arbFunc.order.Direction = ORDER_OUT;
	arbFunc.order.Value = 1;
	arbFunc.order.Index = 0;
	arbFunc.order.DataBytes = 32;
	chData[pos++] = 'C';
	chData[pos++] = 'C';
	chData[pos++] = 'H';
	chData[pos++] = 'V';
	//dev ver
	GetDlgItemText(IDC_EDIT_FPGATRIG_FREQ2, str);
	value = _tstoi(str);
	chData[pos++] = value >> 8;
	chData[pos++] = value &0xff;
	//fpga ver
	GetDlgItemText(IDC_EDIT_FPGATRIG_FREQ3, str);
	value = _tstoi(str);
	chData[pos++] = value >> 8;
	chData[pos++] = value & 0xff;
	//sn
	GetDlgItemText(IDC_EDIT_FPGATRIG_FREQ4, str);
	value = _tstoi(str);
	chData[pos++] = value >> 8;
	chData[pos++] = value & 0xff;
	//sn2
	chData[pos++] = 0;
	chData[pos++] = 0;
	chData[pos++] = 1;//pintai
	//Date(191031)
	GetDlgItemText(IDC_EDIT_FPGATRIG_FREQ5, str);
	value = _tstoi(str);
	chData[pos++] = value >> 16;
	chData[pos++] = value >> 8;
	chData[pos++] = value & 0xff;
	m_sensorInUse->ArbitrFunc((void*)& arbFunc);

	str.ReleaseBuffer();

	
}


void CusbCamConsoleDlg::OnBnClickedButtonRdtemp()
{
	CString strValue;
	int temp = camctrl.getTemp();
	strValue.Format(_T("%d"), temp);
	SetDlgItemText(IDC_EDIT_FPGATRIG_FREQ6, strValue);

}

void CusbCamConsoleDlg::OnBnClickedRadioIo1()
{
	int btnfunc = 0;
	for (int i = 0; i < 4; i++)
	{
		CButton* m_ctlCheck = (CButton*)GetDlgItem(IDC_RADIO_IO1 + i);
		btnfunc = m_ctlCheck->GetCheck();
		if (btnfunc > 0)
			break;
	}
	CString str;
	GetDlgItemText(IDC_EDIT_SENSOR_REGISTER_ADDR, str);
	int value = _tstoi(str);
	camctrl.setGenFunction(btnfunc, value);
}



void CusbCamConsoleDlg::OnBnClickedButtonSet640()
{
	camctrl.setCamFunction(0, 0);
	camctrl.setCamFunction(1, 0);
	camctrl.setCamFunction(2, 639);
	camctrl.setCamFunction(3, 479);

}


void CusbCamConsoleDlg::OnBnClickedButtonRdee()
{

	int pos = 0;
	int value = 0;
	CString str;
	arbFuncStruct arbFunc;
	const int buffsize = 256;
	cq_uint8_t		chData[buffsize];

	arbFunc.FuncNum = 0xBB;
	arbFunc.order.ReqCode = arbFunc.FuncNum;
	arbFunc.order.pData = chData;
	arbFunc.order.Direction = ORDER_IN;
	arbFunc.order.DataBytes = 128;
	arbFunc.order.Value = 1;
	arbFunc.order.Index = 0;
	arbFunc.order.pData = chData;
	m_sensorInUse->ArbitrFunc((void*)& arbFunc);

	CString strValue;
	strValue.Format(_T("%c,%c,%c,%c"), chData[0], chData[1], chData[2], chData[3]);
	CString temp;
	for (int i = 4; i < 16; i++)
	{
		temp.Format(_T("% d"), chData[i]);
		strValue += temp;
	}
	SetDlgItemText(IDC_EDIT_RDEE, strValue);
	//watch chData
}
