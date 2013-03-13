// This source comes from Goran Mitrovic - based on Chris Maunder's work
// modified by Teknikqa
// MyHyperLink.cpp : implementation file

#include "stdafx.h"
#include "MyHyperLink.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMyHyperLink

CMyHyperLink::CMyHyperLink()
{
	mouseover=FALSE;
	cursor=NULL;
	return;
}

CMyHyperLink::~CMyHyperLink()
{
	if(cursor) {
		::DestroyCursor(cursor);
		cursor=NULL;
	}
	return;
}


BEGIN_MESSAGE_MAP(CMyHyperLink, CStatic)
	//{{AFX_MSG_MAP(CMyHyperLink)
	ON_CONTROL_REFLECT(BN_CLICKED, OnClicked)
	ON_WM_MOUSEMOVE()
	ON_WM_SETCURSOR()
	ON_WM_CTLCOLOR_REFLECT()
	ON_WM_RBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMyHyperLink message handlers

BOOL CMyHyperLink::PreTranslateMessage(MSG* pMsg) 
{
	tooltip.RelayEvent(pMsg);

	return(CStatic::PreTranslateMessage(pMsg));
}

void CMyHyperLink::OnClicked() 
{
	GotoURL(url, SW_SHOWMAXIMIZED);
	mouseover=FALSE;
	ReleaseCapture();
	RedrawWindow();
	return;
}

HBRUSH CMyHyperLink::CtlColor(CDC* pDC, UINT nCtlColor) 
{
	ASSERT(nCtlColor==CTLCOLOR_STATIC);

	pDC->SetTextColor(mouseover?::GetSysColor(COLOR_HIGHLIGHT):RGB(0, 0, 238));
	pDC->SetBkMode(TRANSPARENT);
	return((HBRUSH)::GetStockObject(NULL_BRUSH));
}

void CMyHyperLink::OnMouseMove(UINT nFlags, CPoint point) 
{
	CStatic::OnMouseMove(nFlags, point);

	if(mouseover) {
		CRect rect;
		GetClientRect(rect);
		if(!rect.PtInRect(point)) {
			mouseover=FALSE;
			ReleaseCapture();
			RedrawWindow();
		}
	} else {
		mouseover=TRUE;
		RedrawWindow();
		SetCapture();
	}
	return;
}

BOOL CMyHyperLink::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	if(cursor) {
		::SetCursor(cursor);
		return(TRUE);
	}

	return(CStatic::OnSetCursor(pWnd, nHitTest, message));
}

void CMyHyperLink::PreSubclassWindow() 
{
	::SetWindowLong(this->m_hWnd, GWL_STYLE, GetStyle()|SS_NOTIFY);
	::SetWindowLong(this->m_hWnd, GWL_EXSTYLE, GetExStyle()&(~WS_EX_NOPARENTNOTIFY));
	if(url.IsEmpty()) GetWindowText(url);

	LOGFONT lf;
	GetFont()->GetLogFont(&lf);
	lf.lfUnderline=TRUE;
	font.CreateFontIndirect(&lf);
	SetFont(&font);

	PositionWindow();
	GetHandCursor();

	CRect rect; 
	GetClientRect(rect);
	tooltip.Create(this);
	tooltip.AddTool(this, url, rect, 1);

	CStatic::PreSubclassWindow();
	return;
}

void CMyHyperLink::SetURL(CString &newurl)
{
	url = newurl;
	PositionWindow();
	tooltip.UpdateTipText(url, this, 1);
	return;
}

void CMyHyperLink::PositionWindow()
{
	CRect rect;
	GetWindowRect(rect);
	CWnd *parent = GetParent();
	if(parent) parent->ScreenToClient(rect);
	CString text;
	GetWindowText(text);

	CDC *hdc=GetDC();
	CFont *oldfont = hdc->SelectObject(&font);
	CSize size = hdc->GetTextExtent(text);
	hdc->SelectObject(oldfont);
	ReleaseDC(hdc);

	DWORD style = GetStyle();
	if (style&SS_CENTERIMAGE) {
		rect.DeflateRect(0, (rect.Height() - size.cy) / 2);
	} else {
		rect.bottom=rect.top + size.cy;
	}

	if (style&SS_CENTER) {
		rect.DeflateRect((rect.Width() - size.cx) / 2, 0);
	} else if (style & SS_RIGHT) {
		rect.left=rect.right - size.cx;
	} else {
		rect.right=rect.left + size.cx;
	}

	SetWindowPos(NULL, rect.left, rect.top, rect.Width(), rect.Height(), SWP_NOZORDER | SWP_NOACTIVATE);
	return;
}

void CMyHyperLink::GetHandCursor()
{
	if(!cursor) cursor=::LoadCursor(NULL, MAKEINTRESOURCE(32649)/*IDC_HAND*/);
	if(!cursor) {
		CString windir;
		::GetWindowsDirectory(windir.GetBuffer(MAX_PATH), MAX_PATH);
		windir.ReleaseBuffer();
		windir += _T("\\winhlp32.exe");
		HMODULE module=::LoadLibrary(windir);
		if(module) {
			HCURSOR cursor2 = ::LoadCursor(module, MAKEINTRESOURCE(106));
			if(cursor2) cursor = CopyCursor(cursor2);
			::FreeLibrary(module);
		}
	}
	return;
}

long CMyHyperLink::GetRegKey(HKEY key2, const TCHAR *subkey, TCHAR *retdata)
{
	HKEY key = NULL;
	long retval = ::RegOpenKeyEx(key2, subkey, 0, KEY_QUERY_VALUE, &key);
	if (retval == ERROR_SUCCESS) {
		TCHAR data[MAX_PATH];
		long datasize=sizeof(data);
		::RegQueryValue(key, NULL, data, &datasize);
		_tcscpy(retdata, data);
		::RegCloseKey(key);
	}
	return(retval);
}

HINSTANCE CMyHyperLink::GotoURL(const TCHAR *url, long showcmd)
{
	TCHAR key[MAX_PATH+MAX_PATH];
	HINSTANCE ret=::ShellExecute(NULL, _T("open"), url, NULL, NULL, showcmd);
	if((DWORD)(ret) <= HINSTANCE_ERROR) {
		if(GetRegKey(HKEY_CLASSES_ROOT, _T(".htm"), key)==ERROR_SUCCESS) {
			_tcscat(key, _T("\\shell\\open\\command"));
			if(GetRegKey(HKEY_CLASSES_ROOT, key, key)==ERROR_SUCCESS) {
				char *pos;
				pos = _tcsstr(key, _T("\"%1\""));
				if(!pos) {
					pos = _tcsstr(key, _T("%1"));
					if(!pos) {
						pos = key + _tcslen(key)-1;
					} else {
						*pos=0;
					}
				} else {
					*pos = _T('\0');
				}
				_tcscat(pos, _T(" "));
				_tcscat(pos, url);
				ret = (HINSTANCE)(::WinExec(key, showcmd));
			}
		}
	}
	return(ret);
}

#pragma warning(push)
#pragma warning(disable: 4706)
void CMyHyperLink::OnRButtonDown(UINT nFlags, CPoint point) 
{
	CMenu menu;
	menu.CreatePopupMenu();
	menu.AppendMenu(MF_STRING, ID_MEMINFOURL_BROWSER, _T("Goto &URL"));
	menu.AppendMenu(MF_STRING, ID_MEMINFOURL_CLIPBOARD, _T("Copy URL to &clipboard"));
	ClientToScreen(&point);
	switch(menu.TrackPopupMenu(TPM_RIGHTBUTTON|TPM_LEFTALIGN|TPM_TOPALIGN|TPM_RETURNCMD, point.x, point.y, this)) {
	case ID_MEMINFOURL_BROWSER:
		GotoURL(url, SW_SHOWMAXIMIZED);
		break;
	case ID_MEMINFOURL_CLIPBOARD:
		{
			HGLOBAL cliphandle;
			if (cliphandle = ::GlobalAlloc(GHND, url.GetLength()+1)) {
				bool failed = TRUE;
				TCHAR *clip;
				if (clip = (TCHAR *)::GlobalLock(cliphandle)) {
					_tcscpy(clip, url);
					::GlobalUnlock(cliphandle);
					if (OpenClipboard()) {
						::EmptyClipboard();
						::SetClipboardData(CF_TEXT, cliphandle);
						::CloseClipboard();
						failed = FALSE;
					}
				}
				if (failed) 
					::GlobalFree(cliphandle);
			}
		}
		break;
	}
	mouseover = FALSE;
	ReleaseCapture();
	RedrawWindow();

	CStatic::OnRButtonDown(nFlags, point);
	return;
}
#pragma warning(pop)
