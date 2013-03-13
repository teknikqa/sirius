//////////////////////////////////////////////////////////////////////
// Settings.cpp: implementation of the CSettings class.
// takes care of stored "Sirius" settings
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Sirius.h"
#include "Settings.h"
#include "PacketCapture.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

// sections
static const TCHAR szSectionMain[] = _T("main");
static const TCHAR szSectionFilters[] = _T("filters");
static const TCHAR szSectionMainWindow[] = _T("mainwindow");
static const TCHAR szSectionList[] = _T("list");
static const TCHAR szSectionLog[] = _T("log");

// keys
static const TCHAR szKeyOnTop[] = _T("alwaysontop");
static const TCHAR szKeyNSResolve[] = _T("nsresolve");
static const TCHAR szKeyPortResolve[] = _T("portresolve");
static const TCHAR szKeyFilterTCP[] = _T("filtertcp");
static const TCHAR szKeyFilterUDP[] = _T("filterudp");
static const TCHAR szKeyFilterICMP[] = _T("filtericmp");
static const TCHAR szKeyStatus[] = _T("status");
static const TCHAR szKeyTop[] = _T("top");
static const TCHAR szKeyBottom[] = _T("bottom");
static const TCHAR szKeyLeft[] = _T("left");
static const TCHAR szKeyRight[] = _T("right");
static const TCHAR szKeySrcAddr[] = _T("srcaddr");
static const TCHAR szKeyDestAddr[] = _T("destaddr");
static const TCHAR szKeySrcPort[] = _T("srcport");
static const TCHAR szKeyDestPort[] = _T("destport");
static const TCHAR szKeyProto[] = _T("proto");
static const TCHAR szKeyTraffic[] = _T("traffic");
static const TCHAR szKeyBS[] = _T("bs");
static const TCHAR szKeyABS[] = _T("abs");
static const TCHAR szKeyIdleSeconds[] = _T("idleseconds");
static const TCHAR szKeyUseLog[] = _T("uselogfile");
static const TCHAR szKeyLogType[] = _T("logtype");
static const TCHAR szKeyLogFile[] = _T("logfile");
static const TCHAR szKeyAutoDelete[] = _T("autodelete");
static const TCHAR szKeyMBDelete[] = _T("mbdelete");

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSettings::CSettings() : CIniFile()
{
	m_bAlwaysOnTop = GetInt(szSectionMain, szKeyOnTop, FALSE);
	m_bNSResolve = GetInt(szSectionMain, szKeyNSResolve, FALSE);
	m_bPortResolve = GetInt(szSectionMain, szKeyPortResolve, FALSE);
	m_dwIdleSeconds = GetDWORD(szSectionMain, szKeyIdleSeconds, 10);
	m_dwIdleSeconds *= 1000;
	
	m_bFilterTCP = GetInt(szSectionFilters, szKeyFilterTCP, FALSE);
	m_bFilterUDP = GetInt(szSectionFilters, szKeyFilterUDP, FALSE);
	m_bFilterICMP = GetInt(szSectionFilters, szKeyFilterICMP, FALSE);
	
	m_iMainStatus = GetInt(szSectionMainWindow, szKeyStatus, -1);
	m_iMainTop = GetInt(szSectionMainWindow, szKeyTop, -1);
	m_iMainBottom = GetInt(szSectionMainWindow, szKeyBottom, -1);
	m_iMainLeft = GetInt(szSectionMainWindow, szKeyLeft, -1);
	m_iMainRight = GetInt(szSectionMainWindow, szKeyRight, -1);

	m_iListSrcAddr = GetInt(szSectionList, szKeySrcAddr, 120);
	m_iListDestAddr = GetInt(szSectionList, szKeyDestAddr, 120);
	m_iListSrcPort = GetInt(szSectionList, szKeySrcPort, 80);
	m_iListDestPort = GetInt(szSectionList, szKeyDestPort, 80);
	m_iListProto = GetInt(szSectionList, szKeyProto, 50);
	m_iListTraffic = GetInt(szSectionList, szKeyTraffic, 100);
	m_iListBS = GetInt(szSectionList, szKeyBS, 80);
	m_iListABS = GetInt(szSectionList, szKeyABS, 80);

	// log settings load
	m_bUseLog = GetInt(szSectionLog, szKeyUseLog, FALSE);
	m_bAutoDelete = GetInt(szSectionLog, szKeyAutoDelete, FALSE);
	m_iMBDelete = GetInt(szSectionLog, szKeyMBDelete, 1);
	if ((1 > m_iMBDelete) || (999 < m_iMBDelete))
		m_iMBDelete = 1;				// restore default value
	g_lTotalDelBytes = m_iMBDelete * 1024 * 1024;
	m_iLogType = GetInt(szSectionLog, szKeyLogType, 0);
	if ((0 > m_iLogType) || (6 < m_iLogType))
		m_iLogType = 0;					// restore default value
	GetString(szSectionLog, szKeyLogFile, _T(""), m_sLogFile, MAX_PATH);
	if (NULL == m_sLogFile)
		_tcscpy(m_sLogFile, _T(""));
}

CSettings::~CSettings()
{
	SetInt(szSectionMain, szKeyOnTop, m_bAlwaysOnTop);
	SetInt(szSectionMain, szKeyNSResolve, m_bNSResolve);
	SetInt(szSectionMain, szKeyPortResolve, m_bPortResolve);
	SetDWORD(szSectionMain, szKeyIdleSeconds, m_dwIdleSeconds / 1000);

	SetInt(szSectionFilters, szKeyFilterTCP, m_bFilterTCP);
	SetInt(szSectionFilters, szKeyFilterUDP, m_bFilterUDP);
	SetInt(szSectionFilters, szKeyFilterICMP, m_bFilterICMP);
	
	SetInt(szSectionMainWindow, szKeyStatus, m_iMainStatus);
	SetInt(szSectionMainWindow, szKeyTop, m_iMainTop);
	SetInt(szSectionMainWindow, szKeyBottom, m_iMainBottom);
	SetInt(szSectionMainWindow, szKeyLeft, m_iMainLeft);
	SetInt(szSectionMainWindow, szKeyRight, m_iMainRight);

	SetInt(szSectionList, szKeySrcAddr, m_iListSrcAddr);
	SetInt(szSectionList, szKeyDestAddr, m_iListDestAddr);
	SetInt(szSectionList, szKeySrcPort, m_iListSrcPort);
	SetInt(szSectionList, szKeyDestPort, m_iListDestPort);
	SetInt(szSectionList, szKeyProto, m_iListProto);
	SetInt(szSectionList, szKeyTraffic, m_iListTraffic);
	SetInt(szSectionList, szKeyBS, m_iListBS);
	SetInt(szSectionList, szKeyABS, m_iListABS);

	SetInt(szSectionLog, szKeyUseLog, m_bUseLog);
	SetInt(szSectionLog, szKeyAutoDelete, m_bAutoDelete);
	SetInt(szSectionLog, szKeyMBDelete, m_iMBDelete);
	SetInt(szSectionLog, szKeyLogType, m_iLogType);
	SetString(szSectionLog, szKeyLogFile, m_sLogFile);
}
