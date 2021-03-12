/****************************************************************************************\
 * This is the main file for the Caiqi usb camera for Linux/Mac						*
 *                                                                                	*
 * Author              :        nxb     											* 												
 * License             :        GPL Ver 2.0                                       	*
 * Copyright           :        Caiqi Electronics Inc.								*
 * Date written        :        Oct 12, 2017                                    	*
 * Modification Notes  :                                                          	*
 *    1. nxb, Oct 12, 2017                                  						*
 *       Add documentation.                                          				*
 *                      															*
 *                                                                                	*
 \***************************************************************************************/

// CqUsbCam.cpp : ���� DLL Ӧ�ó���ĵ���������
//

#include "StdAfx.h"

#include "CqUsbCam.h"
#include "SendOrder.h"
#include "Types.h"
#include "./sensors/AR0135.h"
#include "./sensors/MT9V034.h"
#include "./sensors/MT9M001.h"
#include "./sensors/SC130GS.h"
#include "./sensors/MT9P031.h"
#include <assert.h>



#define ERR_ITF_NOT_CLAIMED 		-0x80
#define ERR_ITF_IS_ALREADY_CLAIMED	-0x81
#define ERR_NULL_FUNC_POINTER		-0x82

#define ERR_IS_CAPTURING		-0x83
#define ERR_IS_NOT_CAPTURING	-0x84


CCqUsbCam::CCqUsbCam(HANDLE h)
{
	m_pUsbHandle=NULL;

 	m_pImgQueue=NULL;
	m_pDataCap=NULL;
	m_pDataProc=NULL;

	m_bIsInterfaceClaimed=false; 		
	m_bIsCapturing=false;

	RegisterSensor_AR0135(m_sensorList);
	RegisterSensor_MT9V034(m_sensorList);
	RegisterSensor_MT9M001(m_sensorList);
	RegisterSensor_SC130GS(m_sensorList);
	RegisterSensor_MT9P031(m_sensorList);

	m_pUsbHandle=new CCyUSBDevice(h);
	assert(NULL!=m_pUsbHandle);

	//cq_uint8_t devCnt = m_pUsbHandle->DeviceCount();
}

CCqUsbCam::~CCqUsbCam()
{
	delete m_pUsbHandle;
}

cq_bool_t CCqUsbCam::Reset()
{
	m_pUsbHandle->ControlEndPt->Reset();
	m_pUsbHandle->BulkInEndPt->Reset();
	m_pUsbHandle->BulkOutEndPt->Reset();
	//m_pUsbHandle->IsocInEndPt->Reset();
	//m_pUsbHandle->IsocOutEndPt->Reset();
	//m_pUsbHandle->Reset();
	return 0;
}

cq_int32_t  CCqUsbCam::SelectSensor(string* pStrSensorName)
{
	assert(NULL!=pStrSensorName);

	list<tagSensor>::iterator i;      

	if(0==m_sensorList.size())
		return -1;

    for (i = m_sensorList.begin(); i != m_sensorList.end(); ++i)   
    {
		if((*pStrSensorName)==(*i).name)
		{
			m_sensorInUse=(*i);
			return 0;
		}
	}
	return -2;

}

cq_int32_t CCqUsbCam::OpenUSB(cq_uint32_t usbNum)
{

	if (m_pUsbHandle->Open(usbNum) == 0)
		return -1;
	if(m_pUsbHandle->IsOpen())
	{
		m_pUsbHandle->ControlEndPt->Target = TGT_DEVICE;
		m_pUsbHandle->ControlEndPt->ReqType = REQ_VENDOR;
		m_pUsbHandle->ControlEndPt->Value = 0;
		m_pUsbHandle->ControlEndPt->Index = 0;


		devInfo_t devInfo;
		//GetDevInfo(devInfo);

		string strSensorType;
		string strManufactureName;

		cq_int8_t chSensorType[DEV_INFOR_SENSOR_TYPE_LEN + 1];
		cq_int8_t chManufactureName[DEV_INFOR_MANUFACTURE_NAME_LEN + 1];

		memset(chSensorType, 0, sizeof(chSensorType));
		memset(chManufactureName, 0, sizeof(chManufactureName));

		memcpy(chSensorType, devInfo.sensorType, DEV_INFOR_SENSOR_TYPE_LEN);
		memcpy(chManufactureName, devInfo.manufactureName, DEV_INFOR_MANUFACTURE_NAME_LEN);
				
		if((!strcmp(chSensorType, "0134"))||(!strcmp(devInfo.sensorType, "0135")))
			strSensorType = "AR0135";
		else if(!strcmp(chSensorType, "V034"))
			strSensorType = "MT9V034";
		else if(!strcmp(chSensorType, "M001"))
			strSensorType = "MT9M001";
		else if(!strcmp(chSensorType, "P001"))
			strSensorType = "MT9P001";
		else
			//return -2;//sensor type����
			strSensorType = "SC130GS";
		strSensorType = "MT9V034";
		if(0 != SelectSensor(&strSensorType))
			return -3;//��֧�ָ��ͺ�sensor

		m_bIsInterfaceClaimed=true;

		return 0;
	}
	return -4;

}

cq_int32_t CCqUsbCam::CloseUSB()
{
    if(m_pUsbHandle->IsOpen())
	{
		m_pUsbHandle->Close();
		m_bIsInterfaceClaimed=false;
		return 0;
	}
	return -1;
}

cq_int32_t CCqUsbCam::GetDevCnt(cq_uint32_t& devCnt)
{
	devCnt=m_pUsbHandle->DeviceCount();
	return 0;
}
cq_int32_t CCqUsbCam::GetDevInfo(devInfo_t& devInfo)
{
	USB_ORDER sUsbOrder;
	sUsbOrder.ReqCode = 0xD1;
	sUsbOrder.DataBytes = 32;
	sUsbOrder.pData = (cq_uint8_t*)&devInfo;
	sUsbOrder.Direction = 1;
	sUsbOrder.Index = 0;
	sUsbOrder.Value = 0;
	SendOrder(m_pUsbHandle, &sUsbOrder);
	return 0;
}

cq_int32_t  CCqUsbCam::InitSensor()
{
	if(false == m_bIsInterfaceClaimed)
		return ERR_ITF_NOT_CLAIMED;

    if(NULL==m_sensorInUse.InitSensor)
		return ERR_NULL_FUNC_POINTER;

	return m_sensorInUse.InitSensor(m_pUsbHandle);
}


cq_int32_t  CCqUsbCam::StartCap(const cq_uint32_t iHeight, const cq_uint32_t iWidth, callback_t CallBackFunc, void *lpObject)
{
	if(false == m_bIsInterfaceClaimed)
		return ERR_ITF_NOT_CLAIMED;

	if(true ==m_bIsCapturing)
		return ERR_IS_CAPTURING;
	 
	if(NULL==m_sensorInUse.StartCap)
		return ERR_NULL_FUNC_POINTER;

	m_pImgQueue=new wqueue<CImgFrame*>;
	m_pDataCap=new CDataCapture(iWidth, iHeight);
	m_pDataProc=new CDataProcess();

	assert(NULL!=m_pImgQueue);
	assert(NULL!=m_pDataCap);
	assert(NULL!=m_pDataProc);


    m_pDataCap->SetUsbHandle(m_pUsbHandle);
    m_pDataCap->SetImgQueue(m_pImgQueue);

	m_pDataProc->SetCallBackFunc(CallBackFunc, lpObject);
    m_pDataProc->SetImgQueue(m_pImgQueue);


    m_pDataCap->Open();
	m_pDataProc->Open();

	m_sensorInUse.StartCap(m_pUsbHandle);

	m_bIsCapturing=true;
	return 0;
}


cq_int32_t  CCqUsbCam::StopCap()
{
	if(true!=m_bIsCapturing)
		return ERR_IS_NOT_CAPTURING;

	if(NULL==m_sensorInUse.StopCap)
		return ERR_NULL_FUNC_POINTER;

	//m_sensorInUse.StopCap(m_pUsbHandle);

	m_pDataProc->Close();//���뱣֤��ֹͣ�ɼ�֮ǰͣ�������̣߳�������������
    m_pDataCap->Close();

	m_sensorInUse.StopCap(m_pUsbHandle);
	

	if(NULL!=m_pImgQueue)	delete m_pImgQueue;
	if(NULL!=m_pDataCap)	delete m_pDataCap;
	if(NULL!=m_pDataProc)	delete m_pDataProc;
	
	m_bIsCapturing=false;
	return 0;
}

cq_int32_t CCqUsbCam::WrDevID(cq_uint8_t* chIdBuf, cq_uint32_t &length )
{
	USB_ORDER		sUsbOrder;

	sUsbOrder.pData=chIdBuf;
	sUsbOrder.ReqCode = 0xD0;
	sUsbOrder.DataBytes = length;
	sUsbOrder.Direction = 0;
	sUsbOrder.Index = 0;
	sUsbOrder.Value = 0;

	cq_int32_t ret = SendOrder(m_pUsbHandle, &sUsbOrder);
	length = sUsbOrder.DataBytes;
	return ret;
}
cq_int32_t CCqUsbCam::RdDevID(cq_uint8_t *chIdBuf, cq_uint32_t &length)
{
	USB_ORDER		sUsbOrder;

	sUsbOrder.pData=chIdBuf;
	sUsbOrder.ReqCode = 0xD1;
	sUsbOrder.DataBytes = length;
	sUsbOrder.Direction = 1;
	sUsbOrder.Index = 0;
	sUsbOrder.Value = 0;

	cq_int32_t ret = SendOrder(m_pUsbHandle, &sUsbOrder);
	length=sUsbOrder.DataBytes;
	return ret;
}

cq_int32_t CCqUsbCam::WrDevSN(cq_uint8_t* chSnBuf, cq_uint32_t &length )
{
	USB_ORDER		sUsbOrder;

	sUsbOrder.pData=chSnBuf;
	sUsbOrder.ReqCode = 0xD2;
	sUsbOrder.DataBytes = length;
	sUsbOrder.Direction = 0;
	sUsbOrder.Index = 0;
	sUsbOrder.Value = 0;

	cq_int32_t ret = SendOrder(m_pUsbHandle, &sUsbOrder);
	length = sUsbOrder.DataBytes;
	return ret;
}
cq_int32_t CCqUsbCam::RdDevSN(cq_uint8_t *chSnBuf, cq_uint32_t &length)
{
	USB_ORDER		sUsbOrder;

	sUsbOrder.pData=chSnBuf;
	sUsbOrder.ReqCode = 0xD3;
	sUsbOrder.DataBytes = length;
	sUsbOrder.Direction = 1;
	sUsbOrder.Index = 0;
	sUsbOrder.Value = 0;

	cq_int32_t ret = SendOrder(m_pUsbHandle, &sUsbOrder);
	length=sUsbOrder.DataBytes;
	return ret;
}
cq_int32_t CCqUsbCam::WrEeprom(const cq_uint32_t iAddr, const cq_uint8_t iValue)
{
	USB_ORDER		sUsbOrder;
	cq_uint8_t		chData[64];

	sUsbOrder.pData=chData;
	sUsbOrder.ReqCode = 0xF5;
	sUsbOrder.DataBytes = 2;
	sUsbOrder.Direction = 0;
	sUsbOrder.Index = iAddr&0xffff;
	sUsbOrder.Value = iValue&0xffff;

	return SendOrder(m_pUsbHandle, &sUsbOrder);
}
cq_int32_t CCqUsbCam::RrEeprom(const cq_uint32_t iAddr, cq_uint8_t * buffer, cq_uint32_t &length)
{
	USB_ORDER		sUsbOrder;

	sUsbOrder.pData = buffer;
	sUsbOrder.ReqCode = 0xF6;
	sUsbOrder.DataBytes = length;
	sUsbOrder.Direction = 1;
	sUsbOrder.Index = iAddr&0xffff;
	sUsbOrder.Value = 0;

	cq_int32_t ret = SendOrder(m_pUsbHandle, &sUsbOrder);
	length=sUsbOrder.DataBytes;
	return ret;
}

cq_int32_t CCqUsbCam::GetUsbSpeed(cq_uint32_t &chSpeedType)
{
	if(false == m_bIsInterfaceClaimed)
		return ERR_ITF_NOT_CLAIMED;

	if (m_pUsbHandle->bHighSpeed == true)
		chSpeedType=USB_SPEED_HIGH;
	else
		chSpeedType=USB_SPEED_SUPER;
	return 0;
}


cq_int32_t CCqUsbCam::SetAnalogGain(const cq_uint32_t chTrigType, const cq_uint32_t chGainType)
{
	if(false == m_bIsInterfaceClaimed)
		return ERR_ITF_NOT_CLAIMED;

    if(NULL==m_sensorInUse.SetAnalogGain)
		return ERR_NULL_FUNC_POINTER;
	return m_sensorInUse.SetAnalogGain(m_pUsbHandle, chTrigType, chGainType);
}

cq_int32_t CCqUsbCam::SetFpgaTrigFreq(const cq_uint32_t iFreqVal)
{
	if(false == m_bIsInterfaceClaimed)
		return ERR_ITF_NOT_CLAIMED;

    if(NULL==m_sensorInUse.SetFpgaTrigFreq)
		return ERR_NULL_FUNC_POINTER;
	return m_sensorInUse.SetFpgaTrigFreq(m_pUsbHandle, iFreqVal);
}

cq_int32_t CCqUsbCam::SetTrigMode(const cq_uint32_t chTrigType)
{
	if(false == m_bIsInterfaceClaimed)
		return ERR_ITF_NOT_CLAIMED;

    if(NULL==m_sensorInUse.SetTrigMode)
		return ERR_NULL_FUNC_POINTER;
	return m_sensorInUse.SetTrigMode(m_pUsbHandle, chTrigType);
}

cq_int32_t CCqUsbCam::SetExpoValue(const cq_uint32_t iExpoVal)
{
	if(false == m_bIsInterfaceClaimed)
		return ERR_ITF_NOT_CLAIMED;

    if(NULL==m_sensorInUse.SetExpoValue)
		return ERR_NULL_FUNC_POINTER;
	return m_sensorInUse.SetExpoValue(m_pUsbHandle, iExpoVal);
}

cq_int32_t CCqUsbCam::SetGainValue(const cq_uint32_t iGainVal)
{
	if(false == m_bIsInterfaceClaimed)
		return ERR_ITF_NOT_CLAIMED;

    if(NULL==m_sensorInUse.SetGainValue)
		return ERR_NULL_FUNC_POINTER;
	return m_sensorInUse.SetGainValue(m_pUsbHandle, iGainVal);
}

cq_int32_t CCqUsbCam::SetAutoGainExpo(const cq_bool_t bIsAutoGain, const cq_bool_t bIsAutoExpo)
{
	if(false == m_bIsInterfaceClaimed)
		return ERR_ITF_NOT_CLAIMED;

    if(NULL==m_sensorInUse.SetAutoGainExpo)
		return ERR_NULL_FUNC_POINTER;
	return m_sensorInUse.SetAutoGainExpo(m_pUsbHandle, bIsAutoGain, bIsAutoExpo);
}

cq_int32_t CCqUsbCam::SetResolution(const cq_uint32_t chResoluType)
{
	if(false == m_bIsInterfaceClaimed)
		return ERR_ITF_NOT_CLAIMED;

    if(NULL==m_sensorInUse.SetResolution)
		return ERR_NULL_FUNC_POINTER;
	return m_sensorInUse.SetResolution(m_pUsbHandle, chResoluType);
}

cq_int32_t CCqUsbCam::SetMirrorType(const cq_uint32_t chMirrorType)
{
	if(false == m_bIsInterfaceClaimed)
		return ERR_ITF_NOT_CLAIMED;

    if(NULL==m_sensorInUse.SetMirrorType)
		return ERR_NULL_FUNC_POINTER;
	return m_sensorInUse.SetMirrorType(m_pUsbHandle, chMirrorType);
}

cq_int32_t CCqUsbCam::SetBitDepth(const cq_uint32_t chBitDepthType)
{
	if(false == m_bIsInterfaceClaimed)
		return ERR_ITF_NOT_CLAIMED;

    if(NULL==m_sensorInUse.SetBitDepth)
		return ERR_NULL_FUNC_POINTER;
	return m_sensorInUse.SetBitDepth(m_pUsbHandle, chBitDepthType);
}

cq_int32_t CCqUsbCam::SendUsbSpeed2Fpga(const cq_uint32_t chSpeedType)
{
	if(false == m_bIsInterfaceClaimed)
		return ERR_ITF_NOT_CLAIMED;

    if(NULL==m_sensorInUse.SendUsbSpeed2Fpga)
		return ERR_NULL_FUNC_POINTER;
	return m_sensorInUse.SendUsbSpeed2Fpga(m_pUsbHandle, chSpeedType);
}
cq_int32_t CCqUsbCam::SoftTrig()
{
	if(false == m_bIsInterfaceClaimed)
		return ERR_ITF_NOT_CLAIMED;

    if(NULL==m_sensorInUse.SoftTrig)
		return ERR_NULL_FUNC_POINTER;
	return m_sensorInUse.SoftTrig(m_pUsbHandle);
}
cq_int32_t CCqUsbCam::WrSensorReg(const cq_uint32_t iAddr, const cq_uint32_t iValue)
{
	if(false == m_bIsInterfaceClaimed)
		return ERR_ITF_NOT_CLAIMED;

    if(NULL==m_sensorInUse.WrSensorReg)
		return ERR_NULL_FUNC_POINTER;
	return m_sensorInUse.WrSensorReg(m_pUsbHandle, iAddr, iValue);
}

cq_int32_t CCqUsbCam::RdSensorReg(const cq_uint32_t iAddr, cq_uint32_t &iValue)
{
	if(false == m_bIsInterfaceClaimed)
		return ERR_ITF_NOT_CLAIMED;

    if(NULL==m_sensorInUse.RdSensorReg)
		return ERR_NULL_FUNC_POINTER;
	return m_sensorInUse.RdSensorReg(m_pUsbHandle, iAddr, iValue);
}

cq_int32_t CCqUsbCam::WrFpgaReg(const cq_uint32_t 	iAddr, const cq_uint32_t iValue)
{
	if(false == m_bIsInterfaceClaimed)
		return ERR_ITF_NOT_CLAIMED;

    if(NULL==m_sensorInUse.WrFpgaReg)
		return ERR_NULL_FUNC_POINTER;
	return m_sensorInUse.WrFpgaReg(m_pUsbHandle, iAddr, iValue);
}

cq_int32_t CCqUsbCam::RdFpgaReg(const cq_uint32_t 	iAddr, cq_uint32_t &iValue)
{
	if(false == m_bIsInterfaceClaimed)
		return ERR_ITF_NOT_CLAIMED;

    if(NULL==m_sensorInUse.RdFpgaReg)
		return ERR_NULL_FUNC_POINTER;
	return m_sensorInUse.RdFpgaReg(m_pUsbHandle, iAddr, iValue);
}
cq_int32_t CCqUsbCam::ArbitrFunc(LPVOID arg)
{
	if (false == m_bIsInterfaceClaimed)
		return ERR_ITF_NOT_CLAIMED;

	if (NULL == m_sensorInUse.WrFpgaReg)
		return ERR_NULL_FUNC_POINTER;
	return m_sensorInUse.ArbitrFunc(m_pUsbHandle,arg);
}

void CCqUsbCam::GetRecvByteCnt(cq_uint64_t& byteCnt)
{
	assert(NULL!=m_pDataCap);
	byteCnt=m_pDataCap->m_lRecvByteCnt;
}
void CCqUsbCam::ClearRecvByteCnt()
{
	assert(NULL!=m_pDataCap);
	m_pDataCap->m_lRecvByteCnt=0;
}
void CCqUsbCam::GetRecvFrameCnt(cq_uint64_t& frameCnt)
{
	assert(NULL!=m_pDataCap);
	frameCnt=m_pDataCap->m_lRecvFrameCnt;
}
void CCqUsbCam::ClearRecvFrameCnt()
{
	assert(NULL!=m_pDataCap);
	m_pDataCap->m_lRecvFrameCnt=0;
}


vector<CCqUsbCam*> g_vecDev;


cq_int32_t CQUSBOpenUSB(cq_uint32_t devNum)
{
	if (devNum < 1)return devNum;
	if (devNum > g_vecDev.size())return -2;
	devNum = devNum - 1;

	if (g_vecDev[devNum]->m_bIsInterfaceClaimed == true)
		return -2;

	if (g_vecDev[devNum]->OpenUSB(devNum) < 0)// 打开USB失败
	{
		return -3;
	}
	else// 打开USB成功
	{
		return CQUSBGetUsbSpeed(devNum+1);
	}
}
 cq_int32_t CQUSBCloseUsb(cq_uint32_t devNum)
{
	if (devNum < 1)return devNum;
	if (devNum > g_vecDev.size())return -2;
	devNum = devNum - 1;

	g_vecDev[devNum]->CloseUSB();
	delete g_vecDev[devNum];
	vector<CCqUsbCam*>::iterator k = g_vecDev.begin() + devNum;
	g_vecDev.erase(k);
	return 0;
}

 cq_int32_t CQUSBGetUsbSpeed(cq_uint32_t devNum)
{
	if (devNum < 1)return devNum;
	if (devNum > g_vecDev.size())return -2;
	devNum = devNum - 1;

	unsigned int speed = 0;
	g_vecDev[devNum]->GetUsbSpeed(speed);
	if (speed == USB_SPEED_SUPER)
	{
		g_vecDev[devNum]->SendUsbSpeed2Fpga(USB_SPEED_SUPER);
		return 3;
	}
	else if (speed == USB_SPEED_HIGH)
	{
		g_vecDev[devNum]->SendUsbSpeed2Fpga(USB_SPEED_HIGH);
		return 2;
	}
	else
	{
		return 0;
	}
}
#ifdef CS_API
int cs_height;
int cs_width;
unsigned char* csImgBuff;

int imgready = 0;
void  CQUSBGetImgCallBack(LPVOID lpParam, LPVOID lpUser)
{
	const int camcnt = 1;
	CImgFrame* imgframe = (CImgFrame*)lpParam;
	CCqUsbCam* host = (CCqUsbCam*)lpUser;
	//cq_uint8_t* pDataBuffer = imgframe->m_imgBuf;
	//memcpy(csImgBuff, imgframe->m_imgBuf, cs_height*cs_width);
	host->csCBHandler(imgframe->m_imgBuf);
	//imgready = 1;
}

cq_int32_t CQUSBcsGetFrame(unsigned char* buff)
{
	while (!imgready)
	{
		Sleep(1);
	}
	memcpy(buff, csImgBuff, cs_height * cs_width);
	//cv::Mat frameGray(cv_height,cv_width,CV_8UC1,pimagebuf);
	//cv::Mat frameRGB;
	//cv::cvtColor(frameGray,frameRGB,CV_BayerBG2BGR);
	imgready = 0;
	return cs_height * cs_width;
}
cq_int32_t CQUSBAddInstance(csCallBackFuncDel cb)
{
	CCqUsbCam* dev = new CCqUsbCam(NULL);
	g_vecDev.push_back(dev);
	dev->csCBHandler = cb;

	return g_vecDev.size();
}
//cq_int32_t CQUSBcsInit( int w = 640, int h = 360)
//{

//	csImgBuff = new unsigned char[cs_width * cs_height];
//	memset(csImgBuff, 0, cs_width * cs_height);
//	return 1;
//}
#endif

 cq_int32_t CQUSBStartCap(cq_uint32_t devNum,int w, int h)
{
	if (devNum < 1)return devNum;
	if (devNum > g_vecDev.size())return -2;
	devNum = devNum - 1;
	cs_width = w;
	cs_height = h;
	if (g_vecDev[devNum]->StartCap(cs_height, cs_width, CQUSBGetImgCallBack, g_vecDev[devNum]) < 0)
	{
		return -1;
	}
	return 1;
}
 cq_int32_t CQUSBStopCap(cq_uint32_t devNum)
{
	if (devNum < 1)return devNum;
	if (devNum > g_vecDev.size())return -2;
	devNum = devNum - 1;

	g_vecDev[devNum]->StopCap();
}
class RegSettingsStruct2
{
public:
	int s;
	int funcNum;
	int value;
	int sendAddr;
public:
	vector<int> addrlist;
public:
	RegSettingsStruct2()
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
 RegSettingsStruct2 regSettings;
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
 void actCmd(int devnum)
 {
	 int SensorNum = 0;
	 //WrFpgaReg_t();
	 if (regSettings.funcNum < 4)
	 {
		 regSettings.sendAddr = regSettings.addrlist[regSettings.funcNum] + SensorNum;
	 }
	 else
	 {
		 regSettings.sendAddr = regSettings.addrlist[regSettings.funcNum];
	 }
	 g_vecDev[devnum]->WrFpgaReg(regSettings.sendAddr, regSettings.value);
	 //m_sensorInUse->ArbitrFunc(&regSettings);
 }

 cq_int32_t wrSensorCmd(int sensor, int sensorReg, int value,int devnum)
 {

	 regSettings.funcNum = Reg_SENSOR_REG1;
	 regSettings.value = sensorReg >> 8;
	 actCmd(devnum);


	 regSettings.funcNum = Reg_SENSOR_REG2;
	 regSettings.value = sensorReg & 0xff;
	 actCmd(devnum);

	 regSettings.funcNum = Reg_SENSOR_REG3;
	 regSettings.value = value >> 8;
	 actCmd(devnum);

	 regSettings.funcNum = Reg_SENSOR_REG4;
	 regSettings.value = value & 0xff;
	 actCmd(devnum);

	 regSettings.funcNum = Reg_SENSOR_SELECT;
	 regSettings.value = sensor;
	 actCmd(devnum);
	 return 0;
 }
 cq_int32_t CQUSBSetExpo_PLS1Cam(int expo, int devNum)
 {
	 if (devNum < 1)return devNum;
	 if (devNum > g_vecDev.size())return -2;
	 devNum = devNum - 1;

	 wrSensorCmd(0, 0x0202, expo, devNum);
 }
 cq_int32_t CQUSBSetGain_PLS1Cam(int gain, int devNum)
 {
	 if (devNum < 1)return devNum;
	 if (devNum > g_vecDev.size())return -2;
	 devNum = devNum - 1;
	 switch (gain)
	 {
	 case 0:
		 gain = 0x2008;
		 break;
	 case 1:
		 gain = 0x2010;
		 break;
	 case 2:
		 gain = 0x2020;
		 break;
	 case 4:
		 gain = 0x2030;
		 break;
	 case 8:
		 gain = 0x203F;
		 break;
	 default:
		 gain = 0x2008;
		 break;

	 }
	 wrSensorCmd(0, 0x305E, gain, devNum);
 }
 cq_int32_t CQUSBSetResolution(int res, int devNum)
 {
	 if (devNum < 1)return devNum;
	 if (devNum > g_vecDev.size())return -2;
	 devNum = devNum - 1;

	 g_vecDev[devNum]->WrFpgaReg(0x25, res);
	 return 1;
 }
 cq_int32_t CQUSBWrEeprom(int addr, int value, int devNum)
 {
	 if (devNum < 1)return devNum;
	 if (devNum > g_vecDev.size())return -2;
	 devNum = devNum - 1;
	 return g_vecDev[devNum]->WrEeprom(addr, value);
	 
 }
 cq_int32_t CQUSBRrEeprom(int addr, int devNum)
 {
	 if (devNum < 1)return devNum;
	 if (devNum > g_vecDev.size())return -2;
	 devNum = devNum - 1;
	 unsigned char tempbuf = 0;
	 unsigned int len = 1;
	 g_vecDev[devNum]->RrEeprom(addr, &tempbuf, len);
	 return tempbuf;
 }