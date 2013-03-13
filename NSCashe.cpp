//////////////////////////////////////////////////////////////////////
// NSCashe.cpp: implementation of the CNSCashe class.
// very simple internet name resolve and cache system by Teknikqa
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <winsock.h>
#include "Sirius.h"
#include "NSCashe.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CNSCashe::CNSCashe()
{
	m_NSStack = NULL;
	m_bNameResolve = FALSE;
}

CNSCashe::~CNSCashe()
{
	// free class memory
	PNSEntry pNSTemp, pNS = m_NSStack;
	while (NULL != pNS) {
		delete [] pNS->szName;
		pNSTemp = pNS;
		pNS = pNS->next;
		delete pNSTemp;
	}
}

char * CNSCashe::GetNameByIP(DWORD dwIP)
{
	if (!m_bNameResolve)
		return inet_ntoa(*(in_addr *)(&dwIP));
			
	// check is it already in cashe stack
	char *szName = FindNameByIP(dwIP);
	if (NULL != szName)
		return szName;
	
	// add new value
	PNSEntry pNewNS = new TNSEntry;
	pNewNS->dwIP = dwIP;
	HOSTENT *pHostEnt = gethostbyaddr((char *)&dwIP, sizeof(DWORD), AF_INET);
	char *szAddr;
	if (NULL == pHostEnt) {
		szAddr = new char [16];
		strcpy(szAddr, inet_ntoa(*(in_addr *)(&dwIP)));
	}
	else {
		szAddr = new char [strlen(pHostEnt->h_name)+2];
		strcpy(szAddr, pHostEnt->h_name);
	}
	pNewNS->szName = szAddr;
	pNewNS->next = m_NSStack;
	m_NSStack = pNewNS;
	return szAddr;
}

char * CNSCashe::FindNameByIP(DWORD dwIP)
{
	PNSEntry pNS = m_NSStack;
	while (NULL != pNS) {
		if (dwIP == pNS->dwIP)
			return pNS->szName;
		pNS = pNS->next;
	}
	return NULL;
}

BOOL CNSCashe::EnableNameResolve(BOOL bEnable)
{
	BOOL bOld = m_bNameResolve;
	m_bNameResolve = bEnable;
	return bOld;
}
