// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "Sirius.h"
#include "MainFrm.h"
#include "PacketCapture.h"
#include "WPdpack\Include\packet32.h"
#include "SettingsDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_UPDATE_COMMAND_UI(ID_COMMAND_EXIT, OnUpdateCommandExit)
	ON_COMMAND(ID_COMMAND_EXIT, OnCommandExit)
	ON_UPDATE_COMMAND_UI(ID_VIEW_ALWAYSONTOP, OnUpdateViewAlwaysOnTop)
	ON_COMMAND(ID_VIEW_ALWAYSONTOP, OnViewAlwaysOnTop)
	ON_UPDATE_COMMAND_UI(ID_VIEW_RESOLVEADDRESSES, OnUpdateViewResolveAddresses)
	ON_COMMAND(ID_VIEW_RESOLVEADDRESSES, OnViewResolveAddresses)
	ON_UPDATE_COMMAND_UI(ID_VIEW_RESOLVEPORTNAMES, OnUpdateViewResolvePortNames)
	ON_COMMAND(ID_VIEW_RESOLVEPORTNAMES, OnViewResolvePortNames)
	ON_WM_CLOSE()
	ON_UPDATE_COMMAND_UI(ID_VIEW_SETTINGS, OnUpdateViewSettings)
	ON_COMMAND(ID_VIEW_SETTINGS, OnViewSettings)
	ON_WM_MEASUREITEM()
	ON_WM_MENUCHAR()
	ON_WM_INITMENUPOPUP()
	ON_UPDATE_COMMAND_UI(ID_HELP_DOCUMENTATION, OnUpdateHelpDocumentation)
	ON_COMMAND(ID_HELP_DOCUMENTATION, OnHelpDocumentation)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_BYTES_SEC
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
WSADATA wsadata;

	WSAStartup(0x0101, &wsadata);	
}

CMainFrame::~CMainFrame()
{
	m_menu.DestroyMenu();
	WSACleanup();
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if (!m_wndToolBar.CreateEx(this) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME)) {
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}
	if (!m_wndReBar.Create(this) ||
		!m_wndReBar.AddBar(&m_wndToolBar)) {
		TRACE0("Failed to create rebar\n");
		return -1;      // fail to create
	}

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}
	m_wndStatusBar.SetPaneInfo(1, ID_BYTES_SEC, SBPS_NORMAL, 200);

	// TODO: Remove this if you don't want tool tips
	m_wndToolBar.SetBarStyle(m_wndToolBar.GetBarStyle() |
		CBRS_TOOLTIPS | CBRS_FLYBY);

	// update application title
	CString sTitle;
	sTitle.LoadString(AFX_IDS_APP_TITLE);
	SetWindowText(sTitle);

	// init toolbar adapter combo
    #define ADAPTER_WIDTH 180 //the width of the combo box

    //First get the index of the placeholder's position in the toolbar
	int index = 0;
	while(m_wndToolBar.GetItemID(index) != IDC_ADAPTER_COMBO) index++;

	//next convert that button to a seperator and get its position
	m_wndToolBar.SetButtonInfo(index, IDC_ADAPTER_COMBO, TBBS_SEPARATOR, ADAPTER_WIDTH);
	CRect rect;
	m_wndToolBar.GetItemRect(index, &rect);

	//expand the rectangle to allow the combo box room to drop down
	rect.top += 1;
	rect.bottom += 200;

    // then .Create the combo box and show it
	if (!m_wndAdapter.Create(WS_CHILD|WS_VISIBLE | CBS_AUTOHSCROLL | 
                                       CBS_DROPDOWNLIST | CBS_HASSTRINGS ,
                                       rect, &m_wndToolBar, IDC_ADAPTER_COMBO))
	{
		TRACE0("Failed to create combo-box\n");
		return -1;
    }
	// set combo box font
	CFont *pFnt = m_wndStatusBar.GetFont();
	m_wndAdapter.SetFont(pFnt);

	m_wndAdapter.ShowWindow(SW_SHOW);

	//fill the combo box
	GetAdapterList();
	m_wndAdapter.SetCurSel(0);

	g_pwndAdapter = &m_wndAdapter;
	g_pwndStatusBar = &m_wndStatusBar;

	// init from .ini file
	if (g_Settings.m_bAlwaysOnTop) 
		::SetWindowPos(m_hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	else
		::SetWindowPos(m_hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	g_NSCashe.EnableNameResolve(g_Settings.m_bNSResolve);
	g_ServicesParser.EnableServicesResolve(g_Settings.m_bPortResolve);

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;

	cs.style &= ~FWS_ADDTOTITLE;

	// restore saved window position
	if ((-1 != g_Settings.m_iMainStatus) &&
		(-1 != g_Settings.m_iMainTop) &&
		(-1 != g_Settings.m_iMainBottom) &&
		(-1 != g_Settings.m_iMainLeft) &&
		(-1 != g_Settings.m_iMainRight)) {

		AfxGetApp()->m_nCmdShow = g_Settings.m_iMainStatus; // restore the window's status
		// restore the window's width and height
		cs.cx = g_Settings.m_iMainRight - g_Settings.m_iMainLeft;
		cs.cy = g_Settings.m_iMainBottom - g_Settings.m_iMainTop;
                             
		// the following correction is needed when the taskbar is
		// at the left or top and it is not "auto-hidden"
		RECT workArea;
		SystemParametersInfo(SPI_GETWORKAREA, 0, &workArea, 0);
		g_Settings.m_iMainLeft += workArea.left;
		g_Settings.m_iMainTop += workArea.top;
                             
		// make sure the window is not completely out of sight
		int max_x = GetSystemMetrics(SM_CXSCREEN) - GetSystemMetrics(SM_CXICON);
		int max_y = GetSystemMetrics(SM_CYSCREEN) - GetSystemMetrics(SM_CYICON);
		cs.x = min(g_Settings.m_iMainLeft, max_x);
		cs.y = min(g_Settings.m_iMainTop, max_y);
	}
		
	return TRUE;
}

void CMainFrame::OnClose() 
{
	WINDOWPLACEMENT wp;
	GetWindowPlacement(&wp);

	// store saved window position
	g_Settings.m_iMainStatus = wp.showCmd;
	g_Settings.m_iMainTop = wp.rcNormalPosition.top;
	g_Settings.m_iMainBottom = wp.rcNormalPosition.bottom;
	g_Settings.m_iMainLeft =  wp.rcNormalPosition.left;
	g_Settings.m_iMainRight = wp.rcNormalPosition.right;	
	
	CFrameWnd::OnClose();
}
/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG

HMENU CMainFrame::NewMenu()
{
  // Load the menu from the resources
  m_menu.LoadMenu(IDR_MAINFRAME);  
  m_menu.LoadToolbar(IDR_MAINFRAME);

  return(m_menu.Detach());
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame methods

int CMainFrame::GetAdapterList()
{
#define ADAPTER_LENGTH	(10000)
DWORD dwVersion, dwWindowsMajorVersion, dwAdapterLength = ADAPTER_LENGTH;
int iAdapterNum = 0;

// unicode strings (winnt) 
WCHAR		AdapterName[ADAPTER_LENGTH]; // string that contains a list of the network adapters
WCHAR		*temp;

// ascii strings (win95)
char		AdapterNamea[ADAPTER_LENGTH]; // string that contains a list of the network adapters
char		*tempa;

/* DWORD dwIP, dwMask;
BOOL bIpMask;
char szAdapterNameIPMask[512]; */

	/* the data returned by PacketGetAdapterNames is different in Win95 and in WinNT.
	 We have to check the os on which we are running */
	dwVersion = GetVersion();
	dwWindowsMajorVersion = (DWORD)(LOBYTE(LOWORD(dwVersion)));
	if (!(dwVersion >= 0x80000000 && dwWindowsMajorVersion >= 4)) {  // Windows NT 
		//#pragma warning( disable : 4133 )
		PacketGetAdapterNames((char *)AdapterName, &dwAdapterLength);
		//#pragma warning( default : 4133 )
		temp = AdapterName;
		while (*temp != 0) {
			// bIpMask = PacketGetNetInfo((char *)temp, &dwIP, &dwMask);
			if (0 == WideCharToMultiByte(CP_ACP, 0, temp, wcslen(temp), 
				AdapterNamea, 512, NULL, NULL)) {
				TRACE("Error converting to ANSI: %u\n", GetLastError());
			}
			/*if (bIpMask) {
				sprintf(&AdapterNamea[wcslen(temp)], " (ip:%u.%u.%u.%u mask:%u.%u.%u.%u)", 
					(BYTE)(dwIP >> 24), (BYTE)(dwIP >> 16), (BYTE)(dwIP >> 8), (BYTE)(dwIP), 
					(BYTE)(dwMask >> 24), (BYTE)(dwMask >> 16), (BYTE)(dwMask >> 8), (BYTE)(dwMask));
			}
			else
				AdapterNamea[wcslen(temp)] = 0;*/
			AdapterNamea[wcslen(temp)] = 0;
			m_wndAdapter.AddString(AdapterNamea);
			temp += wcslen(temp);
			temp++;
			iAdapterNum++;
		}
	}
	else { // windows 9x 
		PacketGetAdapterNames(AdapterNamea, &dwAdapterLength);
		tempa = AdapterNamea;
		while (*tempa != 0) {
			m_wndAdapter.AddString(tempa);
			tempa += strlen(tempa);
			tempa++;
			iAdapterNum++;
		}
	}
	
	// Find the longest string in the combo box.
	CString str;
	CSize   sz;
	int     dx = 0;
	CDC*    pDC = m_wndAdapter.GetDC();
	
	for (int i=0;i < m_wndAdapter.GetCount(); i++) {
		m_wndAdapter.GetLBText( i, str );
		sz = pDC->GetTextExtent(str);

		if (sz.cx > dx)
			dx = sz.cx;
	}
	m_wndAdapter.ReleaseDC(pDC);

	// Adjust the width for the vertical scroll bar and the left and right border.
	dx += ::GetSystemMetrics(SM_CXVSCROLL) + 2 * ::GetSystemMetrics(SM_CXEDGE);

	// Set the width of the list box so that every item is completely visible.
	m_wndAdapter.SetDroppedWidth(dx);

	return iAdapterNum;

#undef ADAPTER_LENGTH
}

void CMainFrame::OnUpdateCommandExit(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable();	
}

void CMainFrame::OnCommandExit() 
{
	PostQuitMessage(0);	
}

void CMainFrame::OnUpdateViewAlwaysOnTop(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable();
	pCmdUI->SetCheck(g_Settings.m_bAlwaysOnTop);
}

void CMainFrame::OnViewAlwaysOnTop() 
{
	g_Settings.m_bAlwaysOnTop = !g_Settings.m_bAlwaysOnTop;
	if (g_Settings.m_bAlwaysOnTop) 
		::SetWindowPos(m_hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	else
		::SetWindowPos(m_hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
}

void CMainFrame::OnUpdateViewResolveAddresses(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(g_Settings.m_bNSResolve);
}

void CMainFrame::OnViewResolveAddresses() 
{
	g_Settings.m_bNSResolve = !g_Settings.m_bNSResolve;
	g_NSCashe.EnableNameResolve(g_Settings.m_bNSResolve);
}

void CMainFrame::OnUpdateViewResolvePortNames(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(g_Settings.m_bPortResolve);
}

void CMainFrame::OnViewResolvePortNames() 
{
	g_Settings.m_bPortResolve = !g_Settings.m_bPortResolve;
	g_ServicesParser.EnableServicesResolve(g_Settings.m_bPortResolve);
}

void CMainFrame::OnUpdateViewSettings(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable();	
}

void CMainFrame::OnViewSettings() 
{
	CSettingsDlg dlg;
	dlg.m_dwIdleSeconds = g_Settings.m_dwIdleSeconds / 1000;
	dlg.m_bUseLog = g_Settings.m_bUseLog;
	dlg.m_iLogType = g_Settings.m_iLogType;
	dlg.m_sLogFile = g_Settings.m_sLogFile;
	dlg.m_bAutoDelete = g_Settings.m_bAutoDelete;
	dlg.m_iMBDelete = g_Settings.m_iMBDelete;
	if (IDOK == dlg.DoModal()) {
		g_Settings.m_dwIdleSeconds = dlg.m_dwIdleSeconds * 1000;
		g_Settings.m_bUseLog = dlg.m_bUseLog;
		g_Settings.m_iLogType = dlg.m_iLogType;
		_tcscpy(g_Settings.m_sLogFile, (LPCTSTR)dlg.m_sLogFile);
		g_Settings.m_bAutoDelete = dlg.m_bAutoDelete;
		g_Settings.m_iMBDelete = dlg.m_iMBDelete;
		g_lTotalDelBytes = dlg.m_iMBDelete * 1024 * 1024;
	}
}

void CMainFrame::OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct) 
{
BOOL setflag=FALSE;
	
	if(lpMeasureItemStruct->CtlType==ODT_MENU) {
		if(IsMenu((HMENU)lpMeasureItemStruct->itemID)) {
			CMenu* cmenu = 
			CMenu::FromHandle((HMENU)lpMeasureItemStruct->itemID);

			if(m_menu.IsMenu(cmenu)) {
				m_menu.MeasureItem(lpMeasureItemStruct);
				setflag=TRUE;
			}
		}
	}

	if (!setflag)	
		CFrameWnd::OnMeasureItem(nIDCtl, lpMeasureItemStruct);
}

LRESULT CMainFrame::OnMenuChar(UINT nChar, UINT nFlags, CMenu* pMenu) 
{
	if (m_menu.IsMenu(pMenu))
		return BCMenu::FindKeyboardShortcut(nChar, nFlags, pMenu);
	else
		return CFrameWnd::OnMenuChar(nChar, nFlags, pMenu);
}

void CMainFrame::OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu) 
{
	CFrameWnd::OnInitMenuPopup(pPopupMenu, nIndex, bSysMenu);
	
	if (!bSysMenu)
		if (m_menu.IsMenu(pPopupMenu))
			BCMenu::UpdateMenu(pPopupMenu);
}

void CMainFrame::OnUpdateHelpDocumentation(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable();	
}

void CMainFrame::OnHelpDocumentation() 
{
char szFileName[_MAX_PATH], *SlashPos;

	// create directory name
	GetModuleFileName(NULL, szFileName, _MAX_PATH);
	SlashPos = strrchr(szFileName, '\\');
	if (NULL == SlashPos)
		SlashPos = szFileName;
	else
		SlashPos++;
	strcpy(SlashPos, "help");
	// execute HTML file
	::ShellExecute(m_hWnd, "open", "Sirius.html", NULL, szFileName, SW_SHOWNORMAL);
}
