// Settings.h: interface for the CSettings class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SETTINGS_H_INCLUDED_)
#define AFX_SETTINGS_H_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "IniFile.h"

class CSettings : private CIniFile  
{
public:
	CSettings();
	virtual ~CSettings();
	
	BOOL m_bAlwaysOnTop;
	BOOL m_bNSResolve;
	BOOL m_bPortResolve;
	DWORD m_dwIdleSeconds;
	
	BOOL m_bFilterUDP;
	BOOL m_bFilterTCP;
	BOOL m_bFilterICMP;
	
	int  m_iMainStatus;
	int  m_iMainTop;
	int  m_iMainBottom;
	int  m_iMainLeft;
	int  m_iMainRight;

	int  m_iListSrcAddr;
	int  m_iListDestAddr;
	int  m_iListSrcPort;
	int  m_iListDestPort;
	int  m_iListProto;
	int  m_iListTraffic;
	int  m_iListBS;
	int  m_iListABS;

	BOOL m_bUseLog;
	BOOL m_bAutoDelete;
	int  m_iMBDelete;
	int  m_iLogType;
	TCHAR m_sLogFile[MAX_PATH];
};

#endif // !defined(AFX_SETTINGS_H_INCLUDED_)
