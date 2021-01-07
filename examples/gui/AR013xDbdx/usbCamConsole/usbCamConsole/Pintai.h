#pragma once
#include "CqUsbCam.h"
#include <vector>


struct arbFuncStruct
{
	int FuncNum;

	USB_ORDER order;
};
typedef enum CamRegAddr
{
	Reg_AGAE=0x33bb0114,
	Reg_EXPO= 0x33bb0115,
	Reg_GAIN_F= 0x33bb0116,
	Reg_MIRROR= 0x33bb011B,
	Reg_ANALOG_GAIN= 0x33bb011C,
};


typedef enum GeneralFunc
{
	Reg_CAM_MODE=0x33bb0106,	
};

class CCameraCmd
{
public:
	CCqUsbCam* m_sensorInUse;
	vector<int(CCameraCmd::*)(int)> funclist;
	int camNum;
	int checked;
	cq_uint32_t Reg_CAM_OFFSET;
	int height;
	int width;
	int mode;
	cq_uint32_t camTotalNum;
	
	void init(CCqUsbCam* u)
	{
		height = 960;
		width = 1280;
		mode = 1;
		m_sensorInUse = u;
		Reg_CAM_OFFSET = 0x10;
		camTotalNum = 0;
		int rst = 0;
		rst=m_sensorInUse->RdFpgaReg(0x92, camTotalNum);

		

	}
	
	int getCamRes()
	{

		unsigned int  regvalue = 0;
		m_sensorInUse->RdFpgaReg(0x93 + camNum * 4, regvalue);
		height = regvalue << 8;
		m_sensorInUse->RdFpgaReg(0x94 + camNum * 4, regvalue);
		height += regvalue;
		m_sensorInUse->RdFpgaReg(0x95 + camNum * 4, regvalue);
		width = regvalue << 8;
		m_sensorInUse->RdFpgaReg(0x96 + camNum * 4, regvalue);
		width += regvalue;
	
		if (width * height == 0)
		{
			mode = 0;
		}
		return width * height;
	}
	int wrThisCamCmd(cq_uint32_t func, cq_uint32_t v)
	{
		return wrCamCmd(func + Reg_CAM_OFFSET * camNum, v);
	}
	int wrCamCmd(cq_uint32_t addr, cq_uint32_t value)
	{
		int temp = 0;
		for (int i = 0; i < 4; i++)
		{
			temp = ((addr >> (i * 8)) & 0xff);
			m_sensorInUse->WrFpgaReg(0x70 + i, temp);

		}
		for (int i = 0; i < 4; i++)
		{
			temp = ((value >> (i * 8)) & 0xff);
			m_sensorInUse->WrFpgaReg(0x74 + i, temp);

		}
		return 0;
	}
};


class CCameraCtrl
{
public:
	CCameraCmd* cam;
	int CAM_NUM;
	vector<CamRegAddr> camAddrList;
	CCameraCtrl()
	{
		camAddrList.push_back(Reg_AGAE);
		camAddrList.push_back(Reg_EXPO);
		camAddrList.push_back(Reg_GAIN_F);
		camAddrList.push_back(Reg_MIRROR);
		camAddrList.push_back(Reg_ANALOG_GAIN);
	}
	void init(CCqUsbCam* u)
	{
		CAM_NUM = 0;
		CCameraCmd cam0;
		cam0.camNum = 0;
		cam0.init(u);

		if (cam0.camTotalNum > 0)
		{
			CAM_NUM=cam0.camTotalNum;
			cam = new CCameraCmd[CAM_NUM];
			for (int i = 0; i < CAM_NUM; i++)
			{
				cam[i].camNum = i;
				cam[i].init(u);
			}
		}
	}
	long getTotalDataLen()
	{
		getAllRes();
		long datalen = 0;

		for (int i = 0; i < CAM_NUM; i++)
		{
			if (cam[i].mode == 1)
			{
				datalen += cam[i].width * cam[i].height;
			}
		}
		
		return datalen;
	}
	int getAllRes()
	{
		int validCam=0;
		for (int i = 0; i < CAM_NUM; i++)
		{

			if (cam[i].getCamRes() > 0)
				validCam++;
		}
		return validCam;
	}
	int setGenFunction(int s, int value)
	{
		cam[0].wrCamCmd(Reg_CAM_MODE, value);
		//cam[0].wrCamCmd(s + Reg_MOTOR, value);
		return 0;
	}
	int wrCamCmd(int addr, int value)
	{
		for (int i = 0; i < CAM_NUM; i++)
		{
			if (cam[i].checked == 1)
			{
				cam[i].wrCamCmd(addr, value);
			}
		}
		return 0;
	}
	int setCamFunction(int s, int value)
	{
		for (int i = 0; i < CAM_NUM; i++)
		{
			if (cam[i].checked == 1)
			{
				//cam[i].useFuncList(s, value);
				cam[i].wrThisCamCmd(camAddrList[s], value);
			}
		}
		return 0;
	}
	int isCQ()
	{
		char cchv[] = { 'C','C','H','V' };
		cq_uint32_t value[4];
		for (int i = 0; i < 4; i++)
		{
			cam[0].m_sensorInUse->RdFpgaReg(0x80 + i, value[i]);
			if (value[i] != cchv[i])
				return -1;
		}
		return 1;
	}

	int getVer(cq_uint32_t *v)
	{
		if (v == NULL)
			return -1;
		cq_uint32_t value[4];
		for (int i = 0; i < 4; i++)
		{
			cam[0].m_sensorInUse->RdFpgaReg(0x84 + i, value[i]);
		}
		memcpy(v, value, 4 * sizeof(cq_uint32_t));

	}
	int getSerialNum(cq_uint32_t *v)
	{
		if (v == NULL)
			return -1;
		cq_uint32_t value[4];
		for (int i = 0; i < 4; i++)
		{
			cam[0].m_sensorInUse->RdFpgaReg(0x8C + i, value[i]);
		}
		memcpy(v, value, 4 * sizeof(cq_uint32_t));
	}
	int getTemp()
	{
		cq_uint32_t value;
		cam[0].m_sensorInUse->RdFpgaReg(0x91, value);
		return value;
	}

};