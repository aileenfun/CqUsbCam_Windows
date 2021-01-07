
#include "stdafx.h"
#include "CqUsbCam_API.h"


/*
void BMPHeader(int lWidth, int lHeight, cq_byte_t* m_buf, BITMAPINFO* m_bmi)
{
	int mlBpp = 8;
	bool lReverse = false;
	BITMAPFILEHEADER bhh;
	BITMAPINFOHEADER bih;

	memset(m_bmi, 0, sizeof(BITMAPINFO));
	memset(&bhh, 0, sizeof(BITMAPFILEHEADER));
	memset(&bih, 0, sizeof(BITMAPINFOHEADER));

	int widthStep = (((lWidth * mlBpp) + 31) & (~31)) / 8; //每行实际占用的大小（每行都被填充到一个4字节边界）
	int QUADSize = mlBpp == 8 ? sizeof(RGBQUAD) * 256 : 0;

	//构造彩色图的文件头
	bhh.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) + (DWORD)sizeof(BITMAPINFOHEADER) + QUADSize;
	bhh.bfSize = sizeof(m_bmi->bmiHeader);//(DWORD)sizeof(BITMAPFILEHEADER) + (DWORD)sizeof(BITMAPINFOHEADER) + QUADSize + widthStep*lHeight;  
	bhh.bfReserved1 = 0;
	bhh.bfReserved2 = 0;
	bhh.bfType = 0x4d42;

	//构造彩色图的信息头
	bih.biBitCount = mlBpp;
	bih.biSize = sizeof(BITMAPINFOHEADER);
	bih.biHeight = (lReverse ? -1 : 1) * lHeight;
	bih.biWidth = lWidth;
	bih.biPlanes = 1;
	bih.biCompression = BI_RGB;
	bih.biSizeImage = widthStep * lHeight;
	bih.biXPelsPerMeter = 0;
	bih.biYPelsPerMeter = 0;
	bih.biClrUsed = 0;
	bih.biClrImportant = 0;

	//构造灰度图的调色版
	RGBQUAD rgbquad[256];
	if (mlBpp == 8)
	{
		for (int i = 0; i < 256; i++)
		{
			rgbquad[i].rgbBlue = i;
			rgbquad[i].rgbGreen = i;
			rgbquad[i].rgbRed = i;
			rgbquad[i].rgbReserved = 0;
			m_bmi->bmiColors[i].rgbRed = i;
			m_bmi->bmiColors[i].rgbGreen = i;
			m_bmi->bmiColors[i].rgbBlue = i;
			m_bmi->bmiColors[i].rgbReserved = 0;
		}

	}

	int DIBSize = widthStep * lHeight;

	cq_byte_t* bmpbuffer = new cq_byte_t[sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * 256 + DIBSize];
	memcpy(bmpbuffer, (LPSTR)&bhh, sizeof(BITMAPFILEHEADER));
	memcpy(bmpbuffer + sizeof(BITMAPFILEHEADER), (LPSTR)&bih, sizeof(BITMAPINFOHEADER));
	memcpy(bmpbuffer + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER), &rgbquad, sizeof(RGBQUAD) * 256);
	//memcpy(bmpbuffer+sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+sizeof(RGBQUAD)*256,m_buf,DIBSize);
	//m_picCtrl.Load(bmpbuffer,sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+sizeof(RGBQUAD)*256+DIBSize);

	m_bmi->bmiHeader = bih;

	delete bmpbuffer;
	bool b_save_file = false;
	if (snap)
	{
		CString strName;
		CString camFolder;
		camFolder.Format(L"d:\\c6UDP\\cam%d",0);
		if(CreateDirectory(camFolder,NULL)||ERROR_ALREADY_EXISTS == GetLastError())
		{
			int iFileIndex=1;
			do
			{
				strName.Format(L"d:\\c6UDP\\cam%d\\V_%d.jpg",0,iFileIndex);
				++iFileIndex;
			} while (_waccess(strName,0)==0);
			CT2CA pszConvertedAnsiString (strName);
			std::string cvfilename(pszConvertedAnsiString);

			CFile file;
	if(file.Open(strName,CFile::modeWrite | CFile::modeCreate))
	{
		file.Write((LPSTR)&bhh,sizeof(BITMAPFILEHEADER));
		file.Write((LPSTR)&bih,sizeof(BITMAPINFOHEADER));
		if(mlBpp==8) file.Write(&rgbquad,sizeof(RGBQUAD)*256);
		file.Write(m_buf,DIBSize);
		file.Close();
		return ;
	}
		}
		
	}

}
*/