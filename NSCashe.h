// NSCashe.h: interface for the CNSCashe class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NSCASHE_H_INCLUDED_)
#define AFX_NSCASHE_H_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

typedef struct _TNSEntry {
	DWORD dwIP;
	char *szName;
	_TNSEntry *next;
} TNSEntry, *PNSEntry;

class CNSCashe  
{
public:
	BOOL EnableNameResolve(BOOL bEnable = TRUE);
	char * GetNameByIP(DWORD dwIP);
	CNSCashe();
	virtual ~CNSCashe();

private:
	char * FindNameByIP(DWORD dwIP);
	PNSEntry m_NSStack;
	BOOL m_bNameResolve;
};

#endif // !defined(AFX_NSCASHE_H_INCLUDED_)
