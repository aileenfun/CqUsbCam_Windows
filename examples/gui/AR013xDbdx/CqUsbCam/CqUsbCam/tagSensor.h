#ifndef _TAGSENSOR_H_
#define _TAGSENSOR_H_

/****************************************************************************************\
 * This is the header file for the Caiqi usb camera for Linux/Mac						*
 *                                                                                	*
 * Author              :        nxb     											* 												
 * License             :        GPL Ver 2.0                                       	*
 * Copyright           :        Caiqi Electronics Inc.								*
 * Date written        :        Oct 12, 2017                                    	*
 * Modification Notes  :                                                          	*
 *    1. nxb, Oct 12, 2017                                  						*
 *       Add documentation.       
 *    2. nxb, nov 05, 2017
 *	 Add interface as follows
 *	 	 typedef cq_int32_t (*WrEeprom_t)(CCyUSBDevice *, const cq_uint32_t, const cq_uint8_t);
 *		 typedef cq_int32_t (*RdEeprom_t)(CCyUSBDevice *, const cq_uint32_t, cq_uint8_t *, cq_int32_t &);
 *		 typedef cq_int32_t (*WrDevID_t)(CCyUSBDevice *, cq_uint8_t*, cq_uint32_t &);
 *		 typedef cq_int32_t (*RdDevID_t)(CCyUSBDevice *, cq_uint8_t *, cq_uint32_t &);
 *		 typedef cq_int32_t (*WrDevSN_t)(CCyUSBDevice *, cq_uint8_t*, cq_uint32_t &);
 *		 typedef cq_int32_t (*RdDevSN_t)(CCyUSBDevice *, cq_uint8_t * cq_uint32_t &);
 *		 typedef cq_int32_t (*SoftTrig_t)(CCyUSBDevice*);                                   				*
 *                      															*
 *                                                                                	*
 \***************************************************************************************/
 
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <WinDef.h>
using namespace std;

#include "Types.h"
#include "CyAPI.h"
//#include "cyusb.h"

typedef cq_int32_t (*InitSensor_t)(CCyUSBDevice *);
typedef cq_int32_t (*WrSensorReg_t)(CCyUSBDevice *, const cq_uint32_t, const cq_uint32_t);
typedef cq_int32_t (*RdSensorReg_t)(CCyUSBDevice *, const cq_uint32_t, cq_uint32_t &);
typedef cq_int32_t (*WrFpgaReg_t)(CCyUSBDevice *, const cq_uint32_t, const cq_uint32_t);
typedef cq_int32_t (*RdFpgaReg_t)(CCyUSBDevice *, const cq_uint32_t, cq_uint32_t &);
typedef cq_int32_t (*SetAnalogGain_t)(CCyUSBDevice *, const cq_uint32_t, const cq_uint32_t);
typedef cq_int32_t (*SetFpgaTrigFreq_t)(CCyUSBDevice *, const cq_uint32_t);
typedef cq_int32_t (*SetTrigMode_t)(CCyUSBDevice *, const cq_uint32_t);
typedef cq_int32_t (*SetExpoValue_t)(CCyUSBDevice *, const cq_uint32_t);
typedef cq_int32_t (*SetGainValue_t)(CCyUSBDevice *, const cq_uint32_t);
typedef cq_int32_t (*SetAutoGainExpo_t)(CCyUSBDevice *, const cq_bool_t, const cq_bool_t);
typedef cq_int32_t (*SetResolution_t)(CCyUSBDevice *, const cq_uint32_t);
typedef cq_int32_t (*SetMirrorType_t)(CCyUSBDevice *, const cq_uint32_t);
typedef cq_int32_t (*SetBitDepth_t)(CCyUSBDevice *, const cq_uint32_t);


typedef cq_int32_t (*StartCap_t)(CCyUSBDevice *);
typedef cq_int32_t (*StopCap_t)(CCyUSBDevice *);
typedef cq_int32_t (*SendUsbSpeed2Fpga_t)(CCyUSBDevice *, const cq_uint32_t);


typedef cq_int32_t (*WrEeprom_t)(CCyUSBDevice *, const cq_uint32_t, const cq_uint8_t);
typedef cq_int32_t (*RdEeprom_t)(CCyUSBDevice *, const cq_uint32_t, cq_uint8_t *, cq_int32_t &);
typedef cq_int32_t (*WrDevID_t)(CCyUSBDevice *, cq_uint8_t*, cq_uint32_t &);
typedef cq_int32_t (*RdDevID_t)(CCyUSBDevice *, cq_uint8_t *, cq_uint32_t &);
typedef cq_int32_t (*WrDevSN_t)(CCyUSBDevice *, cq_uint8_t*, cq_uint32_t &);
typedef cq_int32_t(*RdDevSN_t)(CCyUSBDevice *, cq_uint8_t *, cq_uint32_t &);
typedef cq_int32_t (*SoftTrig_t)(CCyUSBDevice*);
typedef cq_int32_t(*ArbitrFunc_t)(CCyUSBDevice*, LPVOID lpParam);
typedef struct 
{
	string 				name;
	cq_uint8_t 			iicAddr;
	cq_uint8_t			regAddrLen;
	cq_uint8_t			regValueLen;

	InitSensor_t		InitSensor;
	WrSensorReg_t		WrSensorReg;
	RdSensorReg_t		RdSensorReg;
	WrFpgaReg_t			WrFpgaReg;
	RdFpgaReg_t			RdFpgaReg;
	SetAnalogGain_t		SetAnalogGain;
	SetFpgaTrigFreq_t	SetFpgaTrigFreq;
	SetTrigMode_t		SetTrigMode;
	SetExpoValue_t		SetExpoValue;
	SetGainValue_t		SetGainValue;
	SetAutoGainExpo_t	SetAutoGainExpo;
	SetResolution_t		SetResolution;
	SetMirrorType_t		SetMirrorType;
	SetBitDepth_t		SetBitDepth;
	StartCap_t 			StartCap;
	StopCap_t 			StopCap;
	SendUsbSpeed2Fpga_t	SendUsbSpeed2Fpga;

	WrEeprom_t 			WrEeprom;
	RdEeprom_t 			RdEeprom;
	WrDevID_t 			WrDevID;
	RdDevID_t 			RdDevID;
	WrDevSN_t 			WrDevSN;
	RdDevSN_t 			RdDevSN;
	SoftTrig_t 			SoftTrig;

	ArbitrFunc_t       ArbitrFunc;
} tagSensor;


#endif //_TAGSENSOR_H_
