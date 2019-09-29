#pragma once
#include "CqUsbCam.h"
#include <vector>
#define CAM_NUM 3
typedef enum CamRegAddr
{
	Reg_ROI_Y_START = 0x33bb0110,
	Reg_ROI_X_START,
	Reg_ROI_Y_END,
	Reg_ROI_X_END,
	Reg_AGAE,
	Reg_EXPO,
	Reg_GAIN_F,
	Reg_GAIN_G1,
	Reg_GAIN_B,
	Reg_GAIN_R,
	Reg_GAIN_G2,
	Reg_MIRROR,
	Reg_ANALOG_GAIN,
	Reg_PWM
};
typedef enum GeneralFunc
{
	Reg_MOTOR = 0x33bb0103,
	Reg_FAN,
	Reg_IO_0,
	Reg_CAM_MODE,
	Reg_CAM_CNT,
	Reg_LIGHT_T1,
	Reg_LIGHT_T2,
	Reg_LIGHT_T3,
	Reg_LIGHT_T4,
	Reg_LIGHT_T5,
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
	void init(CCqUsbCam* u)
	{
		height = 960;
		width = 1280;
		mode = 1;
		m_sensorInUse = u;
		Reg_CAM_OFFSET = 0x10;
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
		return width * height;
	}
	int wrThisCamCmd(cq_uint32_t func, cq_uint32_t v)
	{
		return wrCamCmd(func + Reg_ROI_Y_START + Reg_CAM_OFFSET * camNum, v);
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
	CCameraCmd cam[3];

	CCameraCtrl()
	{

	}
	void init(CCqUsbCam* u)
	{
		for (int i = 0; i < CAM_NUM; i++)
		{
			cam[i].camNum = i;
			cam[i].init(u);
		}
	}
	long getTotalDataLen()
	{
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
		for (int i = 0; i < CAM_NUM; i++)
		{

			cam[i].getCamRes();
		}
		return 0;
	}
	int setGenFunction(int s, int value)
	{
		cam[0].wrCamCmd(s + Reg_MOTOR, value);
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
				cam[i].wrThisCamCmd(s, value);
			}
		}
		return 0;
	}

};