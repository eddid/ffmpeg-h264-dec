#if !defined(AFX_IMAGESHOW_H__7D3044AC_9EFB_47B1_9666_4D4C01B49E86__INCLUDED_)
#define AFX_IMAGESHOW_H__7D3044AC_9EFB_47B1_9666_4D4C01B49E86__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ImageShow.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CImageShow window

class CImageShow : public CStatic
{
// Construction
public:
	CImageShow();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CImageShow)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CImageShow();

	// Generated message map functions
protected:
	//{{AFX_MSG(CImageShow)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_IMAGESHOW_H__7D3044AC_9EFB_47B1_9666_4D4C01B49E86__INCLUDED_)
