//////////////////////////////////////////////////////////////////////
// ServicesParser.cpp: implementation of the CServicesParser class.
// parses the popular services file in order to map port number to
// service name
// Author: Teknikqa - http://www.geocities.com/teknikqa (teknikqa@gmail.com)
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Sirius.h"
#include "ServicesParser.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define MAX_LINE (300)

const TCHAR szServicesName[] = _T("services");
const TCHAR szStopChars[] = _T(" \r\n");

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CServicesParser::CServicesParser()
{
TCHAR *SlashPos;
	GetModuleFileName(NULL, m_szFileName, MAX_PATH);
	SlashPos = _tcsrchr(m_szFileName, _T('\\'));
	if (SlashPos == NULL) 	// no '\\' in file name! huh?
		_tcscpy(m_szFileName, szServicesName);
	else {
		SlashPos++;
		_tcscpy(SlashPos, szServicesName);
	}
	LoadFile();
}

CServicesParser::CServicesParser(TCHAR *szFileName)
{
	lstrcpyn(m_szFileName, szFileName, MAX_PATH);
	LoadFile();
}

CServicesParser::~CServicesParser()
{
	// free class memory
	PSvcEntry pSvcTemp, pSvc = m_pSvcStack;
	while (NULL != pSvc) {
		delete [] pSvc->szName;
		pSvcTemp = pSvc;
		pSvc = pSvc->next;
		delete pSvcTemp;
	}
}
//////////////////////////////////////////////////////////////////////
// Implementation
//////////////////////////////////////////////////////////////////////

BOOL CServicesParser::LoadFile()
{
TCHAR szLine[MAX_LINE];

	// initialization
	m_pSvcStack = NULL;
	m_bUseServices = FALSE;

	FILE *f = _tfopen(m_szFileName, _T("rt"));
	if (NULL == f) {
		TRACE("Cannot open file: %s\n", szServicesName);
		return FALSE;
	}
	while (!feof(f)) {
		_fgetts(szLine, MAX_LINE, f);
		ParseLine(szLine);
	}
	fclose(f);
	return TRUE;
}

void CServicesParser::ParseLine(TCHAR *szLine)
{
	if ((_T('#') == szLine[0]) || (_T('\r') == szLine[0]) || (_T('\n') == szLine[0]))
		return; // comment or eol
	
	int iStartIndex, iEndIndex, iIndex = 0;
	while (_T(' ') == szLine[iIndex]) { iIndex++; } // skip starting spaces
	
	iStartIndex = iIndex;
	while (_T(' ') != szLine[iIndex]) { iIndex++; } // find end of chars
	iEndIndex = iIndex;

	while (_T(' ') == szLine[iIndex]) { iIndex++; } // skip spaces between service name and number
	
	// find port number
	TCHAR *szEndChar;
	DWORD dwPort = _tcstoul(&szLine[iIndex], &szEndChar, 10);
	if ((0 == dwPort) || (65535 < dwPort))
		return;

	while (_T('/') != szLine[iIndex]) { iIndex++; } // find end of chars
	iIndex++;

	// find service name
	BYTE byProto;
	int iStartService = iIndex;
	TCHAR szService[MAX_LINE];
	while (NULL == _tcschr(szStopChars, szLine[iIndex])) { iIndex++; } // find end of chars
	lstrcpyn(szService, &szLine[iStartService], (iIndex - iStartService) + 1);
	_tcslwr(szService);
	if (0 == _tcscmp(szService, "tcp"))
		byProto = 6;
	else if (0 == _tcscmp(szService, "udp"))
		byProto = 17;
	else if (0 == _tcscmp(szService, "icmp"))
		byProto = 1;
	else
		return;
	
	// add service to stack
	PSvcEntry pNewSvc = new TSvcEntry;
	pNewSvc->byProto = byProto;
	pNewSvc->wPort = (WORD)dwPort;
	pNewSvc->szName = new TCHAR [(iEndIndex - iStartIndex) + 2];
	lstrcpyn(pNewSvc->szName, &szLine[iStartIndex], (iEndIndex - iStartIndex) + 1);
	pNewSvc->next = m_pSvcStack;
	m_pSvcStack = pNewSvc;
}

TCHAR * CServicesParser::GetServiceName(WORD wPort, BYTE byProto)
{
	if (!m_bUseServices)
		return NULL;
	PSvcEntry pSvc = m_pSvcStack;
	while (NULL != pSvc) {
		if ((wPort == pSvc->wPort) && (byProto == pSvc->byProto))
			return pSvc->szName;
		pSvc = pSvc->next;
	}
	return NULL;
	
}

BOOL CServicesParser::EnableServicesResolve(BOOL bNewState)
{
	BOOL bOld = m_bUseServices;
	m_bUseServices = bNewState;
	return bOld;
}
