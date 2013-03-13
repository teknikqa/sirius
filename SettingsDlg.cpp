// SettingsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Sirius.h"
#include "SettingsDlg.h"
#include "PacketCapture.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static const TCHAR szFilter[] = _T("Log files (*.log)|*.log|Text files (*.txt)|*.txt|All Files (*.*)|*.*||");

/////////////////////////////////////////////////////////////////////////////
// CSettingsDlg dialog

CSettingsDlg::CSettingsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSettingsDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSettingsDlg)
	m_dwIdleSeconds = 0;
	m_bUseLog = FALSE;
	m_iLogType = 0;
	m_bAutoDelete = FALSE;
	m_iMBDelete = 0;
	m_sLogFile = _T("");
	//}}AFX_DATA_INIT
}


void CSettingsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSettingsDlg)
	DDX_Text(pDX, IDC_IDLESECONDS, m_dwIdleSeconds);
	DDV_MinMaxDWord(pDX, m_dwIdleSeconds, 1, 999);
	DDX_Check(pDX, IDC_USELOG, m_bUseLog);
	DDX_CBIndex(pDX, IDC_LOGTYPE, m_iLogType);
	DDX_Check(pDX, IDC_DELETELOG, m_bAutoDelete);
	DDX_Text(pDX, IDC_DELETEMB, m_iMBDelete);
	DDV_MinMaxInt(pDX, m_iMBDelete, 1, 999);
	DDX_Text(pDX, IDC_LOGFILE, m_sLogFile);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSettingsDlg, CDialog)
	//{{AFX_MSG_MAP(CSettingsDlg)
	ON_BN_CLICKED(IDC_USELOG, OnUseLog)
	ON_BN_CLICKED(IDC_BROWSE, OnBrowse)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSettingsDlg message handlers

BOOL CSettingsDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	SendDlgItemMessage(IDC_IDLESECONDS, EM_LIMITTEXT, (WPARAM)3, (LPARAM)0);
	SendDlgItemMessage(IDC_DELETEMB, EM_LIMITTEXT, (WPARAM)3, (LPARAM)0);
	SendDlgItemMessage(IDC_LOGFILE, EM_LIMITTEXT, (WPARAM)MAX_PATH, (LPARAM)0);

	EnableLogging();

	return TRUE;  // return TRUE unless you set the focus to a control
}

void CSettingsDlg::EnableLogging()
{
	GetDlgItem(IDC_STATIC1)->EnableWindow(m_bUseLog);
	GetDlgItem(IDC_LOGTYPE)->EnableWindow(m_bUseLog);
	GetDlgItem(IDC_STATIC2)->EnableWindow(m_bUseLog);
	GetDlgItem(IDC_LOGFILE)->EnableWindow(m_bUseLog);
	GetDlgItem(IDC_BROWSE)->EnableWindow(m_bUseLog);
	GetDlgItem(IDC_DELETELOG)->EnableWindow(m_bUseLog);
	GetDlgItem(IDC_DELETEMB)->EnableWindow(m_bUseLog);
	GetDlgItem(IDC_STATIC3)->EnableWindow(m_bUseLog);
}

void CSettingsDlg::OnUseLog() 
{
	UpdateData(TRUE);
	EnableLogging();	
}

void CSettingsDlg::OnOK() 
{
	if (!UpdateData(TRUE))
		return;
	if (m_bUseLog) {
		if (0 != _tcscmp(g_Settings.m_sLogFile, (LPCTSTR)m_sLogFile)) {
			// open the new file
			FILE *f_new = _tfopen((LPCTSTR)m_sLogFile, _T("a+"));
			if (NULL == f_new) {
				TCHAR message[MAX_PATH * 2];
				_stprintf(message, _T("File \"%s\" cannot be opened for writing!\r\nPlease specify a correct name"),
					(LPCTSTR)m_sLogFile);
				::MessageBox(NULL, message, g_szWarning, MB_OK | MB_ICONINFORMATION);
				return;
			}
			if (g_bStarted) {
				// capture started while we are changing log files!!!

				// Sirius runs in two threads: one is for sniffing
				// the second is for user interface
				// this is pseudo synchronization 
				// maybe I will do a real one someday (when I have more time)

				BOOL bOldVal = g_Settings.m_bUseLog;
				g_Settings.m_bUseLog = FALSE; // no more write operations

				if (bOldVal)
					Sleep(300); // give the sniffing thread some time to finish 
								// started log write operations

				FILE *f_temp = g_logfile;	// store the old file
				g_logfile = f_new;			// set the new file
				if (NULL != f_temp)
					fclose(f_temp);			// close the old file

				g_Settings.m_bUseLog = bOldVal;
			}
			else
				fclose(f_new);
		}
	}
	else 
		CloseLogFile();		
	CDialog::OnOK();
}

void CSettingsDlg::OnBrowse() 
{
	UpdateData(TRUE);
	CFileDialog fd(FALSE, _T("log"), g_Settings.m_sLogFile,
		OFN_CREATEPROMPT | OFN_ENABLESIZING | OFN_HIDEREADONLY | OFN_NOREADONLYRETURN, 
		szFilter);
	if (IDOK == fd.DoModal()) {
		m_sLogFile = fd.GetPathName();
		UpdateData(FALSE);
	}
}
