// ImageShow.cpp : implementation file
//

#include "stdafx.h"
#include "h264player.h"
#include "ImageShow.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CImageShow

CImageShow::CImageShow()
{
}

CImageShow::~CImageShow()
{
}


BEGIN_MESSAGE_MAP(CImageShow, CStatic)
	//{{AFX_MSG_MAP(CImageShow)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CImageShow message handlers

BOOL CImageShow::OnEraseBkgnd(CDC* pDC) 
{
	// TODO: Add your message handler code here and/or call default
	return CStatic::OnEraseBkgnd(pDC);
}

void CImageShow::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	// TODO: Add your message handler code here
	
	// Do not call CStatic::OnPaint() for painting messages
}
