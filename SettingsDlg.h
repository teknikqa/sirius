#if !defined(AFX_SETTINGSDLG_H__8A1FA329_C233_44FD_AD62_8D3BFBC4515D__INCLUDED_)
#define AFX_SETTINGSDLG_H__8A1FA329_C233_44FD_AD62_8D3BFBC4515D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SettingsDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSettingsDlg dialog

class CSettingsDlg : public CDialog
{
// Construction
public:
	int GetLanType();
	void SetLanType(int iLanType);
	CSettingsDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSettingsDlg)
	enum { IDD = IDD_SETTINGS };
	DWORD	m_dwIdleSeconds;
	BOOL	m_bUseLog;
	int		m_iLogType;
	BOOL	m_bAutoDelete;
	int		m_iMBDelete;
	CString	m_sLogFile;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSettingsDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSettingsDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnUseLog();
	virtual void OnOK();
	afx_msg void OnBrowse();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	void EnableLogging();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SETTINGSDLG_H__8A1FA329_C233_44FD_AD62_8D3BFBC4515D__INCLUDED_)
