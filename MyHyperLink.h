// This source is copyrighted by Goran Mitrovic in 1999-2000. For more information, read documentation.

#if !defined(AFX_MYHYPERLINK_H__69B33B4A_DA25_47D5_8A9D_2E78F7A54605__INCLUDED_)
#define AFX_MYHYPERLINK_H__69B33B4A_DA25_47D5_8A9D_2E78F7A54605__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MyHyperLink.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMyHyperLink window

class CMyHyperLink : public CStatic
{
// Construction
public:
	CMyHyperLink();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMyHyperLink)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

// Implementation
public:
	void SetURL(CString &newurl);
	CString GetURL(void) const {return(url);}
	virtual ~CMyHyperLink();

	// Generated message map functions
protected:
	void PositionWindow(void);
	void GetHandCursor(void);
	static long GetRegKey(HKEY key2, const TCHAR *subkey, TCHAR *retdata);
	static HINSTANCE GotoURL(const TCHAR *url, long showcmd);

	bool mouseover;
	CString url;
	CFont font;
	CToolTipCtrl tooltip;
	HCURSOR cursor;
	//{{AFX_MSG(CMyHyperLink)
	afx_msg void OnClicked();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

private:
	enum {ID_MEMINFOURL_BROWSER=100, ID_MEMINFOURL_CLIPBOARD};

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MYHYPERLINK_H__69B33B4A_DA25_47D5_8A9D_2E78F7A54605__INCLUDED_)
