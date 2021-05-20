// H264PlayerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "H264Player.h"
#include "H264PlayerDlg.h"

#define BUFLEN  1024*1024*3

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();
	
	// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA
	
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	
	// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
//{{AFX_MSG_MAP(CAboutDlg)
// No message handlers
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CH264PlayerDlg dialog

CH264PlayerDlg::CH264PlayerDlg(CWnd* pParent /*=NULL*/)
: CDialog(CH264PlayerDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CH264PlayerDlg)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	
	m_bFullScreen = FALSE;
	hDecodeThread=NULL;
	m_bDecoding=FALSE;
	m_OriginalSize=TRUE;
	m_isOver=FALSE;
	m_bIsSuspend=TRUE;
	m_bIsEndThread=FALSE;
	m_inFilePath="";
	m_firstOpen=TRUE;
	m_bIsFullScreen = FALSE; 
}


void CH264PlayerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CH264PlayerDlg)
	DDX_Control(pDX, IDC_STOP, m_stop);
	DDX_Control(pDX, IDC_PLAY, m_play);
	DDX_Control(pDX, IDC_SHOW, m_show);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CH264PlayerDlg, CDialog)
//{{AFX_MSG_MAP(CH264PlayerDlg)
ON_WM_SYSCOMMAND()
ON_WM_PAINT()
ON_WM_QUERYDRAGICON()
ON_WM_SIZE()
ON_BN_CLICKED(IDC_PLAY, OnPlay)
ON_COMMAND(IDR_FILE_OPEN, OnFileOpen)
ON_BN_CLICKED(IDC_STOP, OnStop)
ON_COMMAND(IDR_VIEW_FULLSCRREN, OnViewFullscrren)
ON_COMMAND(IDR_VIEW_ORIGINALSIZE, OnViewOriginalsize)
ON_COMMAND(IDR_ABOUTBOX, OnAboutbox)
ON_WM_ERASEBKGND()
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CH264PlayerDlg message handlers

BOOL CH264PlayerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	// Add "About..." menu item to system menu.
	
	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);
	
	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}
	
	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here

    //初始化控件的大小及位置
	CRect rect;
	GetClientRect(&rect);
	
	if(IsWindow(m_show.GetSafeHwnd()))    //显示控件（静态图片控件）
		m_show.MoveWindow(0,0,rect.Width(),rect.Height()-70);
	
	if(IsWindow(m_play.GetSafeHwnd()))    //播放按钮
		m_play.MoveWindow(10,rect.Height()-40,30,30);
	
	if(IsWindow(m_stop.GetSafeHwnd()))    //停止按钮
		m_stop.MoveWindow(50,rect.Height()-40,30,30);
	
	pDCShow = m_show.GetDC();
	//CDC* bgDC=m_show.GetDC();
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CH264PlayerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CH264PlayerDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting
		
		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);
		
		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;
		
		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{	
        CDialog::OnPaint();
		
		/*CBitmap bitmap;
		bitmap.LoadBitmap(IDB_BG);
		
		BITMAP bmp;
		bitmap.GetBitmap(&bmp);
		
		CDC dcCompatible;
		dcCompatible.CreateCompatibleDC(pDCShow);
		
		dcCompatible.SelectObject(&bitmap);
		
		CRect rect;
		m_show.GetClientRect(&rect);
		
		pDCShow->StretchBlt(0,0,rect.Width()+1,rect.Height()+1,&dcCompatible,0,0,bmp.bmWidth,bmp.bmHeight,SRCCOPY);
		dcCompatible.DeleteDC();*/
		
        //显示播放器的黑色屏幕
		CBrush *pOldBrush,newBrush;
	    CRect rect;
		m_show.GetClientRect(&rect);

		newBrush.CreateSolidBrush(RGB(0,0,0));
		pOldBrush = (CBrush*)pDCShow->SelectObject(&newBrush);
        pDCShow->FillRect(&rect,&newBrush);
		(CBrush*)pDCShow->SelectObject(pOldBrush);

		if(!m_bIsSuspend)   //如果处于暂停状态，就重绘当前帧
		{   
			this->Display(pDCShow);
		}
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CH264PlayerDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CH264PlayerDlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
	
	//改变控件的大小和位置
	if(IsWindow(m_show.GetSafeHwnd()))             //显示控件（静态图片控件）
		m_show.MoveWindow(0,0,cx,cy-70);
	
	if(IsWindow(m_play.GetSafeHwnd()))			   //播放按钮
		m_play.MoveWindow(10,cy-40,30,30);
	
	if(IsWindow(m_stop.GetSafeHwnd()))             //停止按钮
		m_stop.MoveWindow(50,cy-40,30,30);
	
	//设置按原尺寸比例显示
	if(m_firstOpen)  //第一次打开就按原尺寸比例显示
	{
		m_OriginalSize=TRUE;
	}
	else
	{
		m_OriginalSize=FALSE;
	}
	m_firstOpen=FALSE;
	Invalidate();
}

//全屏显示
void CH264PlayerDlg::FullScreen()
{
	ModifyStyle(WS_THICKFRAME, DS_MODALFRAME);//将对话框的border的选项选择为dialog frame，若不改，就不能完成全屏显示
	
	GetWindowPlacement(&m_OldWndPlacement); //保存原来窗口的显示状态以及被恢复的、最大化的和最小化的窗口位置。切出全屏时要使用该信息
	CRect WindowRect;     
	GetWindowRect(&WindowRect);     
	CRect ClientRect;     
	RepositionBars(0, 0xffff, AFX_IDW_PANE_FIRST, reposQuery, &ClientRect);     
	ClientToScreen(&ClientRect); 
	
	//获取屏幕的分辨率     
	int nFullWidth=GetSystemMetrics(SM_CXSCREEN);     
	int nFullHeight=GetSystemMetrics(SM_CYSCREEN);
	
	/*
	将除控制条外的客户区全屏显示到从(0,0)到(nFullWidth, nFullHeight)区域, 
	将(0,0)和(nFullWidth, nFullHeight)两个点外扩充原窗口和除控制条之外的
	客户区位置间的差值, 就得到全屏显示的窗口位置
	*/
	m_FullScreenRect.left=WindowRect.left-ClientRect.left-1;     
	m_FullScreenRect.top=WindowRect.top-ClientRect.top-1;     
	m_FullScreenRect.right=WindowRect.right-ClientRect.right+nFullWidth+2;     
	m_FullScreenRect.bottom=WindowRect.bottom-ClientRect.bottom+nFullHeight+71;  
	m_bFullScreen=TRUE;   //设置全屏显示标志为TRUE
	
	//进入全屏显示状态     
	WINDOWPLACEMENT wndpl;     
	wndpl.length=sizeof(WINDOWPLACEMENT);     
	wndpl.flags=0;     
	wndpl.showCmd=SW_SHOWNORMAL;     
	wndpl.rcNormalPosition=m_FullScreenRect;     
	SetWindowPlacement(&wndpl);
	m_bIsFullScreen = TRUE;
}

//退出全屏
void CH264PlayerDlg::ExitFullScreen()
{
	ModifyStyle(DS_MODALFRAME, WS_THICKFRAME); //将对话框的border的选项选择为Resizing，若不改，就不能调整对话框大小
	SetWindowPlacement(&m_OldWndPlacement);
	m_bIsFullScreen = FALSE;
}

//截获键盘消息，基于对话框的程序不能使用OnKeyDown()函数来响应键盘消息，必须重载PreTranslateMessage(MSG* pMsg)
BOOL CH264PlayerDlg::PreTranslateMessage(MSG* pMsg)     
{   
    if(pMsg->message ==WM_KEYDOWN && pMsg->wParam == VK_ESCAPE && m_bFullScreen)   
	{   
		m_bFullScreen = FALSE;
		ExitFullScreen();
		return TRUE;
	}
	else if(pMsg->message ==WM_KEYDOWN && pMsg->wParam == VK_ESCAPE && !m_bFullScreen)
		return TRUE;
	else
		return   CDialog::PreTranslateMessage(pMsg);   
} 

//播放
void CH264PlayerDlg::OnPlay() 
{
	// TODO: Add your control notification handler code here
	if(this->m_inFilePath=="")
	{
		AfxMessageBox("请选择选择播放文件！");
	}
	else
	{
		if(!m_bDecoding)
		{
			//开始解码线程
			m_bDecoding=TRUE;
			hDecodeThread=CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)DecodeThread,(void *)this,0,&dwThreadID);
		}
		else
		{
			if(hDecodeThread)
			{
				//如果有的话，暂停解码线程
				if(m_bIsSuspend)
				{
					SuspendThread(hDecodeThread);
					m_bIsSuspend=FALSE;
					SetDlgItemText(IDC_PLAY,_T("播放"));
				}
				else
				{
					ResumeThread(hDecodeThread);
					m_bIsSuspend=TRUE;
					SetDlgItemText(IDC_PLAY,_T("暂停"));
				}
			}
		}
	}
}

//解码线程
DWORD CH264PlayerDlg::DecodeThread(void *pDlg)
{
	CH264PlayerDlg* pPlayDlg=(CH264PlayerDlg*)pDlg;
	pPlayDlg->SetDlgItemText(IDC_PLAY,"暂停");
	
	pPlayDlg->m_bmpImage.m_pBmpData = new BYTE [BUFLEN];  //为视频图像分配内存
	//pPlayDlg->m_inFilePath="test.264";
	if(!(pPlayDlg->m_decode.InitDecode(pPlayDlg->m_inFilePath)))
	{
		pPlayDlg->SetDlgItemText(IDC_PLAY,"播放");
		
		pPlayDlg->m_bDecoding=FALSE;
		pPlayDlg->m_bIsEndThread=FALSE;
		pPlayDlg->m_bmpImage.ReleaseImage();
		CloseHandle(pPlayDlg->hDecodeThread);
		pPlayDlg->hDecodeThread=NULL;
		return 0;
	}
	while(pPlayDlg->m_decode.GetNextFrame())
	{
		if(pPlayDlg->m_bIsEndThread)
			break;
		pPlayDlg->m_decode.ImgConvert();
		//解码当前帧的图像数据都保存到m_bmpImage对象中
		pPlayDlg->m_bmpImage.m_pRGBBuffer = pPlayDlg->m_decode.GetBmpData();
		pPlayDlg->m_bmpImage.m_width = pPlayDlg->m_decode.GetFrameWidth();
		pPlayDlg->m_bmpImage.m_height = pPlayDlg->m_decode.GetFrameHeight();
		//每解码一帧就显示一帧
		pPlayDlg->Display(pPlayDlg->pDCShow);
		Sleep(20);
	}
	
	pPlayDlg->m_isOver=TRUE;
	pPlayDlg->SetDlgItemText(IDC_PLAY,"播放");
	pPlayDlg->Invalidate();//播放结束，刷新视图
	
	pPlayDlg->ResourceReleaseAndReconvertSet();
	
	return 0;
}

void CH264PlayerDlg::Display(CDC* pDC)
{
	m_show.GetClientRect(&rectShow);
	CRect rectTemp=OnResetRect(&rectShow,m_bmpImage.m_width, m_bmpImage.m_height);
	
	if(m_OriginalSize)  //按原始尺寸播放
	{	
		SetWindowPos(NULL, 0, 0, m_bmpImage.m_width+7, m_bmpImage.m_height+124, SWP_NOZORDER | SWP_NOMOVE);
		m_show.SetWindowPos(NULL, 0, 0, m_bmpImage.m_width, m_bmpImage.m_height,SWP_NOZORDER | SWP_NOMOVE);
		m_bmpImage.ShowBmpImage(pDC,rectShow,true);
	}
	else
	{		
		m_bmpImage.ShowBmpImage(pDC,rectTemp,false);
	}
}

//使视频图像按原尺寸比例居中显示
CRect CH264PlayerDlg::OnResetRect(CRect *rcScreen, DWORD picWidth, DWORD picHeight)
{
	CRect rect(rcScreen);
	double dWidth = rcScreen->Width();
	double dHeight = rcScreen->Height();
	double dAspectRatio = dWidth/dHeight;
	
	double dPictureWidth = picWidth;
	double dPictureHeight = picHeight;
	double dPictureAspectRatio = dPictureWidth/dPictureHeight;
	
	//If the aspect ratios are the same then the screen rectangle
	// will do, otherwise we need to calculate the new rectangle
	
	if (dPictureAspectRatio > dAspectRatio)
	{
		int nNewHeight = (int)(dWidth/dPictureWidth*dPictureHeight);
		//让图像居中
		int nCenteringFactor= (rcScreen->Height() - nNewHeight) / 2;
		rect.SetRect( 0,
			nCenteringFactor,
			(int)dWidth,
			nNewHeight + nCenteringFactor);
		
	}
	else if (dPictureAspectRatio < dAspectRatio)
	{
		int nNewWidth =  (int)(dHeight/dPictureHeight*dPictureWidth);
		//让图像居中
		int nCenteringFactor= (rcScreen->Width() - nNewWidth) / 2;
		rect.SetRect( nCenteringFactor, 
			0,
			nNewWidth + nCenteringFactor,
			(int)(dHeight));
	}
	
	return rect;
}

//释放资源并还原某些标志的设置
void CH264PlayerDlg::ResourceReleaseAndReconvertSet()
{
	m_bDecoding=FALSE;
	m_bIsEndThread=FALSE;
	
	m_bmpImage.ReleaseImage();
	m_decode.ReleseObj();
	CloseHandle(this->hDecodeThread);
	hDecodeThread=NULL;
}

void CH264PlayerDlg::OnFileOpen() 
{
	// TODO: Add your command handler code here
	CFileDialog dlg(TRUE,NULL,NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
		"264 Files(*.264)|*.264|All Files(*.*)|*.*| |",AfxGetMainWnd());
	if(dlg.DoModal() == IDOK)
	{
		if(m_bDecoding)
		{		
			m_bDecoding=FALSE;
			m_OriginalSize=TRUE;
			m_bIsEndThread=TRUE;
			TerminateThread(hDecodeThread,0);  //终止线程
			ResourceReleaseAndReconvertSet();  //释放资源
		}
		CString filePath=dlg.GetPathName();
		//获得文件路径，并转换路径格式，不能使用windows平台的路径格式
		filePath.Replace('\\','/');     //将文件路径中的'\'替换为'/'
		//将CString字符串转为char*
		int nLen =filePath.GetLength()+1;
		m_inFilePath = new char[nLen];   
		m_inFilePath[nLen] = '\0';
		strcpy(m_inFilePath,(char*)filePath.GetBuffer(0));
		Invalidate();
		//MessageBox(m_inFilePath);
		
		if(m_inFilePath!="")
		{
			m_bDecoding=TRUE;
			hDecodeThread=CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)DecodeThread,(void *)this,0,&dwThreadID);
		}
	}
}

void CH264PlayerDlg::OnStop() 
{
	// TODO: Add your control notification handler code here
	if(hDecodeThread!=NULL)
	{
		m_bIsEndThread=TRUE;  //设置线程结束标志
		if(!m_bIsSuspend)  //如果是暂停时结束的话，先唤醒线程再结束
		{
			ResumeThread(hDecodeThread);
			m_bDecoding=FALSE;
			m_bIsSuspend=TRUE;
		}
		Invalidate();
	}
}

void CH264PlayerDlg::OnViewFullscrren() 
{
	// TODO: Add your command handler code here
	FullScreen();
}

void CH264PlayerDlg::OnViewOriginalsize() 
{
	// TODO: Add your command handler code here
	m_OriginalSize=TRUE;
	Invalidate();
}

void CH264PlayerDlg::OnLButtonDblClk(UINT nFlags, CPoint point)   // 双击“进入/退出”全屏
{
	// TODO: Add your message handler code here and/or call default
	
	if(!m_bIsFullScreen)
		FullScreen();
	else
		ExitFullScreen();
	
	CDialog::OnLButtonDblClk(nFlags, point);
}

void CH264PlayerDlg::OnAboutbox() 
{
	// TODO: Add your command handler code here
	CAboutDlg dlg;
	dlg.DoModal();
}


BOOL CH264PlayerDlg::OnEraseBkgnd(CDC* pDC) 
{
	// TODO: Add your message handler code here and/or call default

	return CDialog::OnEraseBkgnd(pDC);
	
}

HBRUSH CH264PlayerDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	// TODO: Change any attributes of the DC here
	//   TODO:   Change   any   attributes   of   the   DC   here    

	// TODO: Return a different brush if the default is not desired
	return hbr;
}
