/****************************************************************************************\
 * This is the implention file for the Caiqi usb camera for Linux/Mac						*
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
#include "StdAfx.h"
#include <process.h>
#include <assert.h>

#include "DataCapture.h"
//#include <unistd.h>
#include <stdio.h>
#include "Types.h"



CDataCapture::CDataCapture(const cq_uint32_t iWidth, const cq_uint32_t iHeight)
{

    m_iCount=0;

    m_pInData=NULL;
    m_pOutData=NULL;
    m_pReadBuff=NULL;

    m_bCapture=false;

    m_lRecvByteCnt=0;
    m_lRecvFrameCnt=0;

	m_iWidth=iWidth;
	m_iHeight=iHeight;


    /*cq_int32_t ret =pthread_mutex_init(&m_mutexThread, NULL);
    if(ret!=0)
        printf("pthread_mutex_init failed");
		*/
	m_mutexThread = CreateMutex(NULL, FALSE, NULL);

}

cq_int32_t CDataCapture::Open()
{
	m_iCount=0;		//数据计数器

	m_pOutData=NULL;
	try
	{
		m_pOutData=new cq_uint8_t[m_iWidth*m_iHeight+512];
	}
	catch(const bad_alloc& e)
	{
		printf("Alloc mem exception, try agian\n");
		return -1;
	}

	m_pInData=NULL;
	try
	{
		m_pInData=new cq_uint8_t[(m_iWidth*m_iHeight+512)*4];
	}
	catch(const bad_alloc& e)
	{
		printf("Alloc mem exception, try agian\n");
		return -1;
	}

	m_pReadBuff=NULL;
	try
	{
		m_pReadBuff=new cq_uint8_t[m_iWidth*m_iHeight+512];
	}
	catch(const bad_alloc& e)
	{
		printf("Alloc mem exception, try agian\n");
		return -1;
	}

	m_pInputframe=NULL;
	try
	{
		m_pInputframe=new CImgFrame(m_iWidth,m_iHeight,0);//added by qbc
	}
	catch(const bad_alloc& e)
	{
		printf("Alloc mem exception, try agian\n");
		return -1;
	}
	
	assert(NULL!=m_pInData);
	assert(NULL!=m_pOutData);
	assert(NULL!=m_pReadBuff);
	assert(NULL!=m_pInputframe);

	memset(m_pInData,0,(m_iWidth*m_iHeight+512)*4*sizeof(cq_byte_t));
	memset(m_pOutData,0,(m_iWidth*m_iHeight+512)*sizeof(cq_byte_t));
	memset(m_pReadBuff,0,(m_iWidth*m_iHeight+512)*sizeof(cq_byte_t));
	
	//memset(m_pInputframe,0,sizeof(CImgFrame));

	//pthread_create( &m_pThread, NULL, ThreadAdapter, this );
	m_hThread = (HANDLE)_beginthreadex(NULL, 0, ThreadAdapter, this, 0, NULL);

	m_bCapture=true;

	return 0;
}
cq_int32_t CDataCapture::Close()
{
    if(m_bCapture==false)
        return -1;

	m_bCapture=false;

    //pthread_mutex_lock(&m_mutexThread);
	WaitForSingleObject(m_mutexThread, 3000);

    if(m_pInputframe!=NULL)
    {
        delete m_pInputframe;//added byq qbc
        m_pInputframe=NULL;
    }
    if(m_pOutData!=NULL)
    {
        delete[] m_pOutData;
        m_pOutData=NULL;
    }
    if(m_pInData!=NULL)
    {
        delete[] m_pInData;
        m_pInData=NULL;
    }
    if(m_pReadBuff!=NULL)
    {
        delete[] m_pReadBuff;
        m_pInData=NULL;
    }
    //pthread_mutex_unlock(&m_mutexThread);
	ReleaseMutex(m_mutexThread);

    return 0;
}
// CDataCapture::ThreadAdapter(void* __this)
unsigned int __stdcall CDataCapture::ThreadAdapter(void* __this)
{
	CDataCapture* _this=(CDataCapture*) __this;
	_this->ThreadFunc();	

	return NULL;
}

//void CDataCapture::ThreadFunc()
int CDataCapture::ThreadFunc()
{
	cq_int64_t transferred=0;
	
	//pthread_mutex_lock(&m_mutexThread);
	m_pUsbHandle->BulkInEndPt->TimeOut=100;
	WaitForSingleObject(m_mutexThread, INFINITE);

	while (true==m_bCapture)
	{
		transferred=m_iWidth*m_iHeight+512;
		ReadData(m_pReadBuff,transferred);
        if(transferred>0)
		{
		    Input(m_pReadBuff,transferred);
		    m_lRecvByteCnt+=transferred;
		}
		Sleep(1);//deleted by qbc
	}
	ReleaseMutex(m_mutexThread);
	return 0;

}
cq_int32_t CDataCapture::ReadData( cq_uint8_t* pbuff, cq_int64_t &lBytes )
{
	if(m_pUsbHandle->IsOpen())
	{
		if(m_pUsbHandle->BulkInEndPt->GetXferSize()<lBytes)
		{
			m_pUsbHandle->BulkInEndPt->SetXferSize(lBytes);
		}
		if(m_pUsbHandle->BulkInEndPt->XferData((PUCHAR)pbuff,lBytes))
		{
			return 0;
		}
	}
	return -1;
}
//#define _USE55AA
cq_int32_t CDataCapture::Input(const cq_uint8_t* lpData, const cq_uint32_t dwSize )
{
#ifdef _USE55AA
	int iBytes=0;
	iBytes=dwSize+m_iCount;//m_iCount上一次拷贝剩余数据
	memcpy(m_pInData+m_iCount,lpData,dwSize);
	for(int i=0;i<iBytes;++i)
	{
		if(!m_bFindDbFive)
		{
			if(m_pInData[i]==0x55)
			{
				m_bFindDbFive=TRUE;
			}
			continue;
		}
		if(0xaa==m_pInData[i])
		{
			if((i+g_width_L+2)>=iBytes)//如果剩下的最后几个数据长度小于video_width*2+2行号个，不足以构成完整一行，拷贝到下一缓存
			{
				m_iCount=iBytes-i;
				memcpy(m_pInData,m_pInData+i,m_iCount);
				return 0;
			}
			m_iRowIndex=m_pInData[i+1];		//行号高8位
			m_iRowIndex<<=8;				 
			m_iRowIndex+=m_pInData[i+2];	//行号低8位
			if(m_iRowIndex>g_height+2)
			{
				//AfxMessageBox(L"行号出错");
				return 0;
				//exit(1);
			}
			memcpy(m_pOutData+m_iRowIndex*g_width_L,m_pInData+i+3,g_width_L);

			if(m_iRowIndex>=(g_height-1))
			{
				m_pDataProcess->Input(m_pOutData,g_height*g_width_L);

			}
			i=i+g_width_L+2;
		}
		m_bFindDbFive=FALSE;//找到0x55后，无论下个数据是不是0xaa都置状态位为FALSE,然后重新找0x55
	}
#else
    cq_uint32_t iBytes=0;
    iBytes=dwSize+m_iCount;//m_iCount上一次拷贝剩余数据
    cq_bool_t b_header=false/*, b_imu=false*/;
    cq_uint32_t datalen=m_iWidth*m_iHeight+16;// 16 added by qbc

    memcpy(m_pInData+m_iCount,lpData,dwSize);

    for(cq_uint32_t i=0;i<iBytes;++i)
    {
        
        if ((i + datalen) >= iBytes)
        {
            m_iCount = iBytes - i;
            memcpy(m_pInData, m_pInData + i, m_iCount);
            return 0;
        }

        if(m_pInData[i]==0x33&&m_pInData[i+1]==0xcc&&m_pInData[i+14]==0x22&&m_pInData[i+15]==0xdd&&b_header==false)
        {
            i=i+16;
            memcpy(m_pOutData,m_pInData+i,datalen);          

            memcpy(m_pInputframe->m_imgBuf,m_pOutData,m_iWidth*m_iHeight);

            m_pImgQueue->add(m_pInputframe);

            m_lRecvFrameCnt++;
        }

    }
#endif // _USE55AA
    return 0;
}

void CDataCapture::SetImgQueue(wqueue<CImgFrame*>  *pImgQueue)
{
	assert(NULL!=pImgQueue);
	m_pImgQueue=pImgQueue;
}
void CDataCapture::SetUsbHandle(CCyUSBDevice *pUsbHandle)
{
	assert(NULL!=pUsbHandle);
	m_pUsbHandle=pUsbHandle;
}


