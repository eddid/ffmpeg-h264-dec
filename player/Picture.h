#pragma once
#include "windows.h"

class CPicture
{
public:
	CPicture(void);
	~CPicture(void);
	void CreateBmpInfo(BITMAPFILEHEADER &bmpheader,BITMAPINFO &bmpinfo,int flag);
	int  CreateBmp(char* filename);
	void ShowBmpImage(CDC* pDC,CRect rect,bool originalSize);  //originalSizeΪ�Ƿ�ԭ�ߴ���ʾ
	void ReleaseImage();

public:
	unsigned char* m_pBmpData;
	unsigned char* m_pRGBBuffer;

    DWORD m_width;
	DWORD m_height;

	int m_bpp;
};
