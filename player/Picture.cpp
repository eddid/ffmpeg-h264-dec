#include "StdAfx.h"
#include "Picture.h"


#define BUFLEN	1024*1024*3
CPicture::CPicture(void)
{
	m_pBmpData = NULL;
	m_pRGBBuffer = NULL;
	m_bpp = 24;
}

CPicture::~CPicture(void)
{
}

void CPicture::CreateBmpInfo(BITMAPFILEHEADER &bmpheader,BITMAPINFO &bmpinfo,int flag)//显示图片的时候flag为-1，截图创建图片的时候flag为1，否则opencv不能读取图片
{
	bmpheader.bfType = ('M'<<8)|'B';
    bmpheader.bfReserved1 = 0;
    bmpheader.bfReserved2 = 0;
    bmpheader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
    bmpheader.bfSize = bmpheader.bfOffBits + m_width*m_height*m_bpp/8;
	
    bmpinfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmpinfo.bmiHeader.biWidth = m_width;
    bmpinfo.bmiHeader.biHeight = m_height*flag;
    bmpinfo.bmiHeader.biPlanes = 1;
    bmpinfo.bmiHeader.biBitCount = m_bpp;
    bmpinfo.bmiHeader.biCompression = BI_RGB;
    bmpinfo.bmiHeader.biSizeImage = (m_width*m_bpp+31)/32*4*m_height;
    bmpinfo.bmiHeader.biXPelsPerMeter = 100;
    bmpinfo.bmiHeader.biYPelsPerMeter = 100;
    bmpinfo.bmiHeader.biClrUsed = 0;
    bmpinfo.bmiHeader.biClrImportant = 0;
}

int CPicture::CreateBmp(char *filename)
{
	BITMAPFILEHEADER bmpheader;
    BITMAPINFO bmpinfo;
    FILE *fp;
	
    fp = fopen(filename,"wb");
    if(!fp)return -1;
	
	CreateBmpInfo(bmpheader,bmpinfo,1);
	
    fwrite(&bmpheader,sizeof(BITMAPFILEHEADER),1,fp);
    fwrite(&bmpinfo.bmiHeader,sizeof(BITMAPINFOHEADER),1,fp);
	//fwrite(&bmpinfo,sizeof(BITMAPINFO),1,fp);
    fwrite(m_pBmpData,m_width*m_height*m_bpp/8,1,fp);  //这里如果把m_pBmpData换成m_pRGBBuffer,所截的图不正确，
    fclose(fp);										   //因为按下截图的按钮时m_pRGBBuffer正在被赋值，所以不能得到完整的一幅正确的图
	
    return 0;	
}

void CPicture::ShowBmpImage(CDC* pDC,CRect rect,bool originalSize)
{
	//////////////////////////////////////////////////////////////////////////
	BITMAPFILEHEADER bmpheader;
    BITMAPINFO bmpinfo;
	
    CreateBmpInfo(bmpheader,bmpinfo,-1);
	
	memcpy(m_pBmpData,&bmpheader,sizeof(BITMAPFILEHEADER));
	memcpy(m_pBmpData+sizeof(BITMAPFILEHEADER),&bmpinfo.bmiHeader,sizeof(BITMAPINFOHEADER));
	memcpy(m_pBmpData,m_pRGBBuffer,m_width * m_height * m_bpp/8);
	
	HBITMAP hBmp;
	CDC MemDC;
	
	hBmp = CreateDIBitmap(
		pDC->m_hDC,					// handle to device context
		(BITMAPINFOHEADER*)(&bmpinfo),   //	pointer to bitmap size and format data
		CBM_INIT,	// initialization flag
		m_pBmpData,	// pointer to initialization data
		&bmpinfo,	// pointer to bitmap color-format data
		DIB_RGB_COLORS	// color-data usage
		);
	
	if(hBmp == NULL)
	{
		pDC->DeleteDC();
		return;
	}
	
	MemDC.CreateCompatibleDC(pDC);	
	SelectObject(MemDC.m_hDC,hBmp);
	
	pDC->StretchBlt(rect.left,rect.top,rect.Width(),rect.Height(),&MemDC,0,0,this->m_width,this->m_height,SRCCOPY);
	pDC->SetStretchBltMode(COLORONCOLOR);
	
	DeleteObject(hBmp);
	MemDC.DeleteDC();
	//////////////////////////////////////////////////////////////////////////
	
}

void CPicture::ReleaseImage()
{
	if(m_pBmpData!=NULL) //free(m_pBmpData);
	delete m_pBmpData;
	m_pBmpData=NULL;
}
