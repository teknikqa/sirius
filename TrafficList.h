// TrafficList.h: interface for the CTrafficList class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TRAFFICLIST_H_INCLUDED_)
#define AFX_TRAFFICLIST_H_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

typedef struct {
	DWORD dwSrcIP;
	DWORD dwDestIP;
	WORD wSrcPort;
	WORD wDestPort;
	BYTE byProto;
} TAddrInfo, *PAddrInfo;

typedef struct {
	TAddrInfo addr;
	DWORD dwTimeStamp;
	DWORD dwTimeStampStart;
	DWORD dwBytes;
	DWORD dwBytesSec;
} TTrafInfo, *PTrafInfo;

class CTrafficList  
{
public:
	void FilterProto(BYTE byProto);
	void ClearPackets();
	void ShowPacket(const BYTE *pbyPacket);
	CTrafficList();
	virtual ~CTrafficList();

private:
	void CheckAutoDelete();
	void AddPacket(DWORD dwLen);
	int FindPacket();
	DWORD GetHdrLenByProto(BYTE byProto);
	TAddrInfo CurAddr;
	char szNum[300]; // used for numeric to string conversions

	// log functions
	void DumpBufferHex(const BYTE *pBuf, DWORD dwLen, FILE *fh);
	void DumpBufferHexText(const BYTE *pBuf, DWORD dwLen, FILE *fh);
	void DumpBufferText(const BYTE *pBuf, DWORD dwLen, FILE *fh);
	void PrintStartInfo(const BYTE *wsb, FILE *fh);

};

#endif // !defined(AFX_TRAFFICLIST_H_INCLUDED_)
