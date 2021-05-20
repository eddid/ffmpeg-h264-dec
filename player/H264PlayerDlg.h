// H264PlayerDlg.h : header file
//

#if !defined(AFX_H264PLAYERDLG_H__EB529B2A_63F1_4F56_B5DA_3D69C01BC37B__INCLUDED_)
#define AFX_H264PLAYERDLG_H__EB529B2A_63F1_4F56_B5DA_3D69C01BC37B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CH264PlayerDlg dialog

#include "Decode.h"
#include "Picture.h"
#include "H264PlayerDlg.h"
#include "ImageShow.h"

class CH264PlayerDlg : public CDialog
{
// Construction
public:
	CH264PlayerDlg(CWnd* pParent = NULL);	// standard constructor
	BOOL PreTranslateMessage(MSG* pMsg);    // 截获键盘消息

	void FullScreen();                      // 全屏
	void ExitFullScreen();					// 退出全屏
	static DWORD DecodeThread(void *pDlg);  // 解码线程
	void Display(CDC* pDC);                 // 显示视频图像
	CRect OnResetRect(CRect* rcScreen,DWORD bmpWidth, DWORD bmpHeight);  //使视频图像按原尺寸比例居中显示
	void ResourceReleaseAndReconvertSet();   // 释放资源并还原某些标志的设置

// Dialog Data
	//{{AFX_DATA(CH264PlayerDlg)
	enum { IDD = IDD_H264PLAYER_DIALOG };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CH264PlayerDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

public:
	
	WINDOWPLACEMENT m_OldWndPlacement;
	CRect m_FullScreenRect;
	BOOL m_bFullScreen;

	CButton	m_stop;
	CButton	m_play;
	//CImageShow	m_show;
	CStatic	m_show;

	CDecode m_decode;  //解码器
	CPicture m_bmpImage; //视频图像
	CDC* pDCShow ;
	CRect rectShow;
	char* m_inFilePath;
	HANDLE hDecodeThread;
	DWORD dwThreadID;
    BOOL m_bDecoding;
	BOOL m_bIsEndThread;
	BOOL m_isOver;
	BOOL m_firstOpen;    //第一次打开窗口，用于OnSize()中控制m_OriginalSize
	BOOL m_OriginalSize; //是否按原尺寸比例显示
	BOOL m_bIsFullScreen; //是否进入全屏模式
	BOOL m_bIsSuspend;   //暂停

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CH264PlayerDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPlay();
	afx_msg void OnFileOpen();
	afx_msg void OnStop();
	afx_msg void OnViewFullscrren();
	afx_msg void OnViewOriginalsize();
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnAboutbox();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_H264PLAYERDLG_H__EB529B2A_63F1_4F56_B5DA_3D69C01BC37B__INCLUDED_)
