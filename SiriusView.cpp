// SiriusView.cpp : implementation of the CSiriusView class
//

#include "stdafx.h"
#include "Sirius.h"
#include "SiriusDoc.h"
#include "SiriusView.h"
#include "PacketCapture.h"
#include "TrafficList.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define TIMER_ID		(1)

// global file vars
static int s_iSortColumn;
static BOOL s_bSortAscending;
static LVFINDINFO fi;

int CALLBACK CompareFunction(LPARAM lParam1,LPARAM lParam2,LPARAM lParamData);

/////////////////////////////////////////////////////////////////////////////
// data for the list view control

#define NUM_COLUMNS	8

static _TCHAR *_gszColumnLabel[NUM_COLUMNS] =
{
	_T("Source"), _T("src port"), _T("Destination"), _T("dest port"), _T("Proto"), _T("Traffic"), _T("bytes/sec"), _T("Average b/s")
};

static int _gnColumnFmt[NUM_COLUMNS] = 
{
	LVCFMT_LEFT, LVCFMT_RIGHT, LVCFMT_LEFT, LVCFMT_RIGHT, LVCFMT_RIGHT, LVCFMT_RIGHT, LVCFMT_RIGHT, LVCFMT_RIGHT
};

static int _gnColumnWidth[NUM_COLUMNS] = 
{
	120, 80, 120, 80, 50, 100, 80, 80
};

static BOOL _gbColumnNumber[NUM_COLUMNS] = 
{
	FALSE, TRUE, FALSE, TRUE, FALSE, TRUE, TRUE, TRUE
};

/////////////////////////////////////////////////////////////////////////////
// CSiriusView

IMPLEMENT_DYNCREATE(CSiriusView, CListView)

BEGIN_MESSAGE_MAP(CSiriusView, CListView)
	//{{AFX_MSG_MAP(CSiriusView)
	ON_UPDATE_COMMAND_UI(ID_COMMAND_START, OnUpdateCommandStart)
	ON_UPDATE_COMMAND_UI(ID_COMMAND_STOP, OnUpdateCommandStop)
	ON_WM_PAINT()
	ON_COMMAND(ID_COMMAND_START, OnCommandStart)
	ON_COMMAND(ID_COMMAND_STOP, OnCommandStop)
	ON_WM_DESTROY()
	ON_NOTIFY_REFLECT(LVN_COLUMNCLICK, OnColumnClick)
	ON_UPDATE_COMMAND_UI(ID_FILTER_REMOVESORTING, OnUpdateFilterRemoveSorting)
	ON_COMMAND(ID_FILTER_REMOVESORTING, OnFilterRemoveSorting)
	ON_UPDATE_COMMAND_UI(ID_FILTER_TCP, OnUpdateFilterTcp)
	ON_UPDATE_COMMAND_UI(ID_FILTER_UDP, OnUpdateFilterUdp)
	ON_UPDATE_COMMAND_UI(ID_FILTER_ICMP, OnUpdateFilterIcmp)
	ON_COMMAND(ID_FILTER_TCP, OnFilterTcp)
	ON_COMMAND(ID_FILTER_UDP, OnFilterUdp)
	ON_COMMAND(ID_FILTER_ICMP, OnFilterIcmp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSiriusView construction/destruction

CSiriusView::CSiriusView()
{
	m_hAdapterMutex = NULL;
	memset(&fi, 0, sizeof(LVFINDINFO));
	fi.flags = LVFI_PARAM;
	s_iSortColumn = -1;
}

CSiriusView::~CSiriusView()
{
}

/////////////////////////////////////////////////////////////////////////////
// CSiriusView drawing

void CSiriusView::OnDraw(CDC* pDC)
{
	CSiriusDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	// TODO: add draw code for native data here
}

void CSiriusView::OnInitialUpdate()
{
	CListView::OnInitialUpdate();

	// my code
	_gnColumnWidth[0] = g_Settings.m_iListSrcAddr;
	_gnColumnWidth[2] = g_Settings.m_iListDestAddr;
	_gnColumnWidth[1] = g_Settings.m_iListSrcPort;
	_gnColumnWidth[3] = g_Settings.m_iListDestPort;
	_gnColumnWidth[4] = g_Settings.m_iListProto;
	_gnColumnWidth[5] = g_Settings.m_iListTraffic;
	_gnColumnWidth[6] = g_Settings.m_iListBS;
	_gnColumnWidth[7] = g_Settings.m_iListABS;
	
	CListCtrl& lc_AddrList = GetListCtrl();

	// set report style
	long lStyle = GetWindowLong(lc_AddrList.m_hWnd, GWL_STYLE);
	lStyle &= ~(LVS_TYPEMASK);  // turn off all the style (view mode) bits
	lStyle |= LVS_REPORT;       // Set the new style for the control
	SetWindowLong(lc_AddrList.m_hWnd, GWL_STYLE, lStyle);

	// set other styles
	DWORD dwExStyle = lc_AddrList.GetExtendedStyle();
	lc_AddrList.SetExtendedStyle(dwExStyle | 
		LVS_EX_FULLROWSELECT | LVS_EX_HEADERDRAGDROP | LVS_EX_GRIDLINES);

	// insert columns
	LV_COLUMN lvc;
	lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;

	for(int iCounter = 0; iCounter < NUM_COLUMNS; iCounter++) {
		lvc.iSubItem = iCounter;
		lvc.pszText = _gszColumnLabel[iCounter];
		lvc.cx = _gnColumnWidth[iCounter];
		lvc.fmt = _gnColumnFmt[iCounter];
		lc_AddrList.InsertColumn(iCounter, &lvc);
	}

	g_pwndTraffic = &GetListCtrl();

	VERIFY(m_ctlHeader.SubclassWindow(lc_AddrList.GetHeaderCtrl()->GetSafeHwnd()));
}

/////////////////////////////////////////////////////////////////////////////
// CSiriusView diagnostics

#ifdef _DEBUG
void CSiriusView::AssertValid() const
{
	CListView::AssertValid();
}

void CSiriusView::Dump(CDumpContext& dc) const
{
	CListView::Dump(dc);
}

CSiriusDoc* CSiriusView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CSiriusDoc)));
	return (CSiriusDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CSiriusView message handlers

void CSiriusView::OnUpdateCommandStart(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(!g_bStarted);	
}

void CSiriusView::OnUpdateCommandStop(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(g_bStarted);	
}

void CSiriusView::OnPaint() 
{
    if (GetListCtrl().GetItemCount() == 0)  { // show empty list message
        CPaintDC dc( this );
        int nSavedDC = dc.SaveDC();
        
        CRect rc;
        GetClientRect( &rc );
        
        CHeaderCtrl* pHC;
        pHC = GetListCtrl().GetHeaderCtrl();
        if (pHC != NULL) {
            CRect rcH;
            pHC->GetItemRect( 0, &rcH );
            rc.top += rcH.bottom;
        }
        rc.top += 10;
        
        CString strText;
        if (g_bStarted)
			strText.LoadString(IDS_NODATA);
		else
			strText.LoadString(IDS_BEGIN);
        
        dc.SetTextColor( ::GetSysColor( COLOR_WINDOWTEXT ) );
        dc.SetBkColor( ::GetSysColor( COLOR_WINDOW ) );
        dc.SelectStockObject( ANSI_VAR_FONT );
        dc.DrawText( strText, rc, DT_CENTER | DT_WORDBREAK | DT_NOPREFIX | DT_NOCLIP );
        
        dc.RestoreDC(nSavedDC);
    }
    else {
        Default();
    }
}

void CSiriusView::CreateMutexName(const char *szInput, char *szOutput)
{
	strcpy(szOutput, "st_"); // mutex name begin (st comes from Sirius)
	strcat(szOutput, szInput); // adds Input as the rest of the mutex name
	int iLen = strlen(szOutput);
	// replace all '\\' with '_'
	for (int i = 0; i < iLen; i++) 
		if ('\\' == szOutput[i])
			szOutput[i] = '_';
}

void CSiriusView::OnCommandStart() 
{
	// get selected adapter
	char szAdapter[512], szMutexName[512];
	szAdapter[0] = 0;
	g_pwndAdapter->GetWindowText(szAdapter, 512);
	if (0 == szAdapter[0])
		return;
	// remove ip and mask
	char *pszIpMask = strstr(szAdapter, " (ip:");
	if (pszIpMask)
		*pszIpMask = 0;
	// check if we have such adapter already started
	CreateMutexName(szAdapter, szMutexName);
	m_hAdapterMutex = OpenMutex(MUTEX_ALL_ACCESS, FALSE, szMutexName);
	if (m_hAdapterMutex != NULL) {
		MessageBox("This adapter is already opened\n\rin other instance of Show Traffic", 
			g_szError, MB_OK | MB_ICONSTOP);
		CloseHandle(m_hAdapterMutex);
		return;
	}
	m_hAdapterMutex = CreateMutex(NULL, FALSE, szMutexName);
	
	if (0 != StartCapture(szAdapter))
		return;	

	g_bStarted = TRUE;
	g_pwndTraffic->Invalidate();

	::SetTimer(m_hWnd, TIMER_ID, 1000, &(CSiriusView::MyTimerProc)); // init statistics timer

	// update application title
	g_pwndAdapter->GetWindowText(szAdapter, 512);
	CString sTitle;
	sTitle.LoadString(AFX_IDS_APP_TITLE);
	sTitle += " - [";
	sTitle += szAdapter;
	sTitle += "]";
	GetParentFrame()->SetWindowText(sTitle);
}

void CSiriusView::OnCommandStop() 
{
	EndCapture();
	g_bStarted = FALSE;
	KillTimer(TIMER_ID); // kill statistics timer
	CloseHandle(m_hAdapterMutex);
	// update application title
	CString sTitle;
	sTitle.LoadString(AFX_IDS_APP_TITLE);
	GetParentFrame()->SetWindowText(sTitle);
}

void CSiriusView::OnDestroy() 
{
HDITEM hdi;

	// store columns width
	CHeaderCtrl *pHdr = GetListCtrl().GetHeaderCtrl();
	hdi.mask = HDI_WIDTH;
	pHdr->GetItem(0, &hdi);
	g_Settings.m_iListSrcAddr = hdi.cxy;
	pHdr->GetItem(2, &hdi);
	g_Settings.m_iListDestAddr = hdi.cxy;
	pHdr->GetItem(1, &hdi);
	g_Settings.m_iListSrcPort = hdi.cxy;
	pHdr->GetItem(3, &hdi);
	g_Settings.m_iListDestPort = hdi.cxy;
	pHdr->GetItem(4, &hdi);
	g_Settings.m_iListProto = hdi.cxy;
	pHdr->GetItem(5, &hdi);
	g_Settings.m_iListTraffic = hdi.cxy;
	pHdr->GetItem(6, &hdi);
	g_Settings.m_iListBS = hdi.cxy;
	pHdr->GetItem(7, &hdi);
	g_Settings.m_iListABS = hdi.cxy;

	if (g_bStarted) {
		OnCommandStop();
		g_bStarted = FALSE;
	}
	CListView::OnDestroy();
}

void CALLBACK CSiriusView::MyTimerProc(HWND hWnd, UINT nMsg, UINT nIDEvent, DWORD dwTime)
{
	TRACE("Timer called\n");
	if (TIMER_ID == nIDEvent) {
		g_bTimerEvent = TRUE;
		char szNum[300];
		sprintf(szNum, "b/s: %u   avr b/s: %u", g_dwBPS, g_dwABS);
		g_pwndStatusBar->SetPaneText(1, szNum, FALSE);
	}
}

void CSiriusView::OnColumnClick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
	
	const int iColumn = pNMListView->iSubItem;

	// if it's a second click on the same column then reverse the sort order,
	// otherwise sort the new column in ascending order.
	Sort( iColumn, iColumn == s_iSortColumn ? !s_bSortAscending : TRUE );

	*pResult = 0;
}

void CSiriusView::Sort(int iColumn, BOOL bAscending)
{
	s_iSortColumn = iColumn;
	s_bSortAscending = bAscending;

	// show the appropriate arrow in the header control.
	m_ctlHeader.SetSortArrow(s_iSortColumn, s_bSortAscending );

	g_pwndTraffic->SortItems(CompareFunction, (DWORD)g_pwndTraffic);
}

BOOL IsNumber(LPCTSTR pszText)
{
	for( int i = 0; i < lstrlen( pszText ); i++ )
		if( !_istdigit( pszText[ i ] ) )
			return false;

	return true;
}


int NumberCompare( LPCTSTR pszNumber1, LPCTSTR pszNumber2 )
{
	const int iNumber1 = atoi( pszNumber1 );
	const int iNumber2 = atoi( pszNumber2 );

	if( iNumber1 < iNumber2 )
		return -1;
	
	if( iNumber1 > iNumber2 )
		return 1;

	return 0;
}

int CALLBACK CompareFunction( LPARAM lParam1, LPARAM lParam2, LPARAM lParamData )
{
	CListCtrl *pListCtrl = (CListCtrl *)lParamData;
	fi.lParam = lParam1;
	int iItem1 = pListCtrl->FindItem(&fi);
	fi.lParam = lParam2;
	int iItem2 = pListCtrl->FindItem(&fi);
	if ((-1 == iItem1) || (-1 == iItem2))
		return 0;
	CString strItem1 = pListCtrl->GetItemText(iItem1, s_iSortColumn);
	CString strItem2 = pListCtrl->GetItemText(iItem2, s_iSortColumn);

	if (_gbColumnNumber[s_iSortColumn])
		return s_bSortAscending ? NumberCompare( strItem1, strItem2 ) : NumberCompare( strItem2, strItem1 );
	else // text.
		return s_bSortAscending ? lstrcmp( strItem1, strItem2 ) : lstrcmp( strItem2, strItem1 );
}



void CSiriusView::OnUpdateFilterRemoveSorting(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable((0 <= s_iSortColumn) ? TRUE : FALSE);
}

void CSiriusView::OnFilterRemoveSorting() 
{
	s_iSortColumn = -1;
	m_ctlHeader.SetSortArrow(s_iSortColumn, s_bSortAscending );
}

void CSiriusView::OnUpdateFilterTcp(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable();
	pCmdUI->SetCheck(g_Settings.m_bFilterTCP);	
}

void CSiriusView::OnUpdateFilterUdp(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable();
	pCmdUI->SetCheck(g_Settings.m_bFilterUDP);	
}

void CSiriusView::OnUpdateFilterIcmp(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable();
	pCmdUI->SetCheck(g_Settings.m_bFilterICMP);	
}

void CSiriusView::OnFilterTcp() 
{
	g_Settings.m_bFilterTCP = !g_Settings.m_bFilterTCP;
	if (g_Settings.m_bFilterTCP)  // we are about to remove all filtered
		g_TrafficList.FilterProto(6); // 6 = TCP
}

void CSiriusView::OnFilterUdp() 
{
	g_Settings.m_bFilterUDP = !g_Settings.m_bFilterUDP;
	if (g_Settings.m_bFilterUDP)  // we are about to remove all filtered
		g_TrafficList.FilterProto(17); // 17 = UDP
}

void CSiriusView::OnFilterIcmp() 
{
	g_Settings.m_bFilterICMP = !g_Settings.m_bFilterICMP;
	if (g_Settings.m_bFilterICMP)  // we are about to remove all filtered
		g_TrafficList.FilterProto(1); // 1 = ICMP
}
