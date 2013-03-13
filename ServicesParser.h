// ServicesParser.h: interface for the CServicesParser class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SERVICESPARSER_H_INCLUDED_)
#define AFX_SERVICESPARSER_H_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

typedef struct _TSvcEntry {
	WORD wPort;
	BYTE byProto;
	TCHAR *szName;
	_TSvcEntry *next;
} TSvcEntry, *PSvcEntry;

class CServicesParser  
{
public:
	BOOL EnableServicesResolve(BOOL bNewState = TRUE);
	TCHAR * GetServiceName(WORD wPort, BYTE byProto);
	CServicesParser();
	CServicesParser(TCHAR *szFileName);
	virtual ~CServicesParser();

private:
	void ParseLine(TCHAR *szLine);
	BOOL LoadFile();
	TCHAR m_szFileName[MAX_PATH];
	BOOL m_bUseServices;
	PSvcEntry m_pSvcStack;
};

#endif // !defined(AFX_SERVICESPARSER_H_INCLUDED_)
