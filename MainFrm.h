// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINFRM_H_INCLUDED_)
#define AFX_MAINFRM_H_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BCMenu.h"

class CMainFrame : public CFrameWnd
{
	
protected: // create from serialization only
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// Attributes
public:

// Operations
public:
	HMENU NewMenu();
	BCMenu m_menu;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
	CStatusBar  m_wndStatusBar;
	CToolBar    m_wndToolBar;
	CReBar      m_wndReBar;

// Generated message map functions
protected:
	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnUpdateCommandExit(CCmdUI* pCmdUI);
	afx_msg void OnCommandExit();
	afx_msg void OnUpdateViewAlwaysOnTop(CCmdUI* pCmdUI);
	afx_msg void OnViewAlwaysOnTop();
	afx_msg void OnUpdateViewResolveAddresses(CCmdUI* pCmdUI);
	afx_msg void OnViewResolveAddresses();
	afx_msg void OnUpdateViewResolvePortNames(CCmdUI* pCmdUI);
	afx_msg void OnViewResolvePortNames();
	afx_msg void OnClose();
	afx_msg void OnUpdateViewSettings(CCmdUI* pCmdUI);
	afx_msg void OnViewSettings();
	afx_msg void OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	afx_msg LRESULT OnMenuChar(UINT nChar, UINT nFlags, CMenu* pMenu);
	afx_msg void OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu);
	afx_msg void OnUpdateHelpDocumentation(CCmdUI* pCmdUI);
	afx_msg void OnHelpDocumentation();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	int GetAdapterList();
	CComboBox m_wndAdapter;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINFRM_H_INCLUDED_)
