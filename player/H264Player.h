// H264Player.h : main header file for the H264PLAYER application
//

#if !defined(AFX_H264PLAYER_H__99793513_FABE_4ABA_938F_5C5D2E305AD4__INCLUDED_)
#define AFX_H264PLAYER_H__99793513_FABE_4ABA_938F_5C5D2E305AD4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CH264PlayerApp:
// See H264Player.cpp for the implementation of this class
//

class CH264PlayerApp : public CWinApp
{
public:
	CH264PlayerApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CH264PlayerApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CH264PlayerApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_H264PLAYER_H__99793513_FABE_4ABA_938F_5C5D2E305AD4__INCLUDED_)
