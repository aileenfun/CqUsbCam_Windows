#ifndef _DATAPROCESS_H_
#define _DATAPROCESS_H_

/****************************************************************************************\
 * This is the header file for the Caiqi usb camera for Linux/Mac					*
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
 

#include "ImgFrame.h"
#include "wqueue.h"
#include "Types.h"


class CDataProcess
{


public:
	CDataProcess();



	void Open();
	void Close();

	void SetImgQueue(wqueue<CImgFrame*>  *pImgQueue);
	void SetCallBackFunc(callback_t CallBackFunc, void *lpObject);	

	//void ThreadFunc();
	//static void* ThreadAdapter(void* __this);
	
	int ThreadFunc();
	static unsigned int __stdcall ThreadAdapter(void* __this);




	
private:
	//pthread_mutex_t m_mutexThread;
	HANDLE m_mutexThread;
	//pthread_t m_pThread; 
	HANDLE m_hThread;
	
 	wqueue<CImgFrame*>  *m_pImgQueue;	//

	cq_bool_t m_bProcess;
	callback_t m_CallBackFunc;
	void *m_lpObject;
	
};


 #endif //_DATAPROCESS_H_
