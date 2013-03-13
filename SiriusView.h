// SiriusView.h : interface of the CSiriusView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_SIRIUSVIEW_H_INCLUDED_)
#define AFX_SIRIUSVIEW_H_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#ifndef SORTHEADERCTRL_H
	#include "SortHeaderCtrl.h"
#endif	// SORTHEADERCTRL_H

class CSiriusView : public CListView
{
protected: // create from serialization only
	CSiriusView();
	DECLARE_DYNCREATE(CSiriusView)

// Attributes
public:
	CSiriusDoc* GetDocument();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSiriusView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	protected:
	virtual void OnInitialUpdate(); // called first time after construct
	//}}AFX_VIRTUAL

// Implementation
public:
	void Sort(int iColumn, BOOL bAscending);
	static void CALLBACK MyTimerProc(HWND hWnd, UINT nMsg, UINT nIDEvent, DWORD dwTime);
	virtual ~CSiriusView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

// Generated message map functions
protected:
	//{{AFX_MSG(CSiriusView)
	afx_msg void OnUpdateCommandStart(CCmdUI* pCmdUI);
	afx_msg void OnUpdateCommandStop(CCmdUI* pCmdUI);
	afx_msg void OnPaint();
	afx_msg void OnCommandStart();
	afx_msg void OnCommandStop();
	afx_msg void OnDestroy();
	afx_msg void OnColumnClick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnUpdateFilterRemoveSorting(CCmdUI* pCmdUI);
	afx_msg void OnFilterRemoveSorting();
	afx_msg void OnUpdateFilterTcp(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFilterUdp(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFilterIcmp(CCmdUI* pCmdUI);
	afx_msg void OnFilterTcp();
	afx_msg void OnFilterUdp();
	afx_msg void OnFilterIcmp();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	void CreateMutexName(const char *szInput, char *szOutput);
	UINT m_wTimer;
	HANDLE m_hAdapterMutex;

	CSortHeaderCtrl m_ctlHeader;
	int m_iNumColumns;
};

#ifndef _DEBUG  // debug version in SiriusView.cpp
inline CSiriusDoc* CSiriusView::GetDocument()
   { return (CSiriusDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SIRIUSVIEW_H_INCLUDED_)
