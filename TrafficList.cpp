//////////////////////////////////////////////////////////////////////
// TrafficList.cpp: implementation of the CTrafficList class.
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <winsock2.h>
#include "Sirius.h"
#include "TrafficList.h"
#include "PacketCapture.h"
#include "NSCashe.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

static const char szFrmChars[] = "\n\r\t\f\b\x07\x0B\0";

static char Protocol[256][20]={ 
		           "Reserved",     //  0
                   "ICMP",         //  1
                   "IGMP",         //  2
                   "GGP",          //  3
                   "IP",           //  4
                   "ST",           //  5
                   "TCP",          //  6
                   "UCL",          //  7
                   "EGP",          //  8
                   "IGP",          //  9
                   "BBN-RCC-MON",  // 10
                   "NVP-II",       // 11
                   "PUP",          // 12
                   "ARGUS",        // 13
                   "EMCON",        // 14
                   "XNET",         // 15
                   "CHAOS",        // 16
                   "UDP",          // 17
                   "MUX",          // 18
                   "DCN-MEAS",     // 19
                   "HMP",          // 20
                   "PRM",          // 21
                   "XNS-IDP",      // 22
                   "TRUNK-1",      // 23
                   "TRUNK-2",      // 24
                   "LEAF-1",       // 25
                   "LEAF-2",       // 26
                   "RDP",          // 27
                   "IRTP",         // 28
                   "ISO-TP4",      // 29
                   "NETBLT",       // 30
                   "MFE-NSP",      // 31
                   "MERIT-INP",    // 32
                   "SEP",          // 33
                   "3PC",          // 34
                   "IDPR",         // 35
                   "XTP",          // 36
                   "DDP",          // 37
                   "IDPR-CMTP",    // 38
                   "TP++",         // 39
                   "IL",           // 40
                   "SIP",          // 41
                   "SDRP",         // 42
                   "SIP-SR",       // 43
                   "SIP-FRAG",     // 44
                   "IDRP",         // 45
                   "RSVP",         // 46
                   "GRE",          // 47
                   "MHRP",         // 48
                   "BNA",          // 49
                   "SIPP-ESP",     // 50
                   "SIPP-AH",      // 51
                   "I-NLSP",       // 52
                   "SWIPE",        // 53
                   "NHRP",         // 54
                   "unknown (55)", // 55
                   "unknown (56)", // 56
                   "unknown (57)", // 57
                   "unknown (58)", // 58
                   "unknown (59)", // 59
                   "unknown (60)", // 60
                   "unknown (61)", // 61
                   "CFTP",         // 62
                   "unknown (63)", // 63
                   "SAT-EXPAK",    // 64
                   "KRYPTOLAN",    // 65
                   "RVD",          // 66
                   "IPPC",         // 67
                   "unknown (68)", // 68
                   "SAT-MON",      // 69
                   "VISA",         // 70
                   "IPCV",         // 71
                   "CPNX",         // 72
                   "CPHB",         // 73
                   "WSN",          // 74
                   "PVP",          // 75
                   "BR-SAT-MON",   // 76
                   "SUN-ND",       // 77
                   "WB-MON",       // 78
                   "WB-EXPAK",     // 79
                   "ISO-IP",       // 80
                   "VMTP",         // 81
                   "SECURE-VMTP",  // 82
                   "VINES",        // 83
                   "TTP",          // 84
                   "NSFNET-IGP",   // 85
                   "DGP",          // 86
                   "TCF",          // 87
                   "IGRP",         // 88
                   "OSPFIGP",      // 89
                   "Sprite-RPC",   // 90
                   "LARP",         // 91
                   "MTP",          // 92
                   "AX.25",        // 93
                   "IPIP",         // 94
                   "MICP",         // 95
                   "SCC-SP",       // 96
                   "ETHERIP",      // 97
                   "ENCAP",        // 98
                   "unknown (99)", // 99
                   "GMTP"          // 100
                  };

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTrafficList::CTrafficList()
{
	for (int index = 101; index < 256; index++)
		sprintf(Protocol[index], "unknown (%d)", index);
}

CTrafficList::~CTrafficList()
{
}

//////////////////////////////////////////////////////////////////////
// Implementation
//////////////////////////////////////////////////////////////////////

/****************************************************************
Show write information in pBuf (dwLen bytes long) to file (fh) 
formated as byte size HEX values
Format: 1. offset 2. HEX values (16 per line)
****************************************************************/
void CTrafficList::DumpBufferHex(const BYTE *pBuf, DWORD dwLen, FILE *fh)
{
DWORD dwCounter, dwCnt;

	for (dwCounter = 0; dwCounter < dwLen / 16; dwCounter++)
		fprintf(fh, "%08x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x\n",
			dwCounter * 16, pBuf[dwCounter * 16 + 0], pBuf[dwCounter * 16 + 1],
			pBuf[dwCounter * 16 + 2],pBuf[dwCounter * 16 + 3], pBuf[dwCounter * 16 + 4],
			pBuf[dwCounter * 16 + 5],pBuf[dwCounter * 16 + 6], pBuf[dwCounter * 16 + 7],
			pBuf[dwCounter * 16 + 8],pBuf[dwCounter * 16 + 9], pBuf[dwCounter * 16 + 10],
			pBuf[dwCounter * 16 + 11],pBuf[dwCounter * 16 + 12], pBuf[dwCounter * 16 + 13],
			pBuf[dwCounter * 16 + 14],pBuf[dwCounter * 16 + 15]);
	if (dwLen % 16 != 0) {
		fprintf(fh, "%08x ", dwCounter * 16);
		for (dwCnt = 0; dwCnt < dwLen % 16; dwCnt++)
			fprintf(fh, "%02x ", pBuf[dwCounter * 16 + dwCnt]);
		fprintf(fh, "\n");
	}
}
/****************************************************************
Show write information in pBuf (dwLen bytes long) to file (fh) 
formated as byte size HEX values followed by the plain text
Format: 1. offset 2. HEX values (16 per line) 3. Plain text chars (16 per line)
****************************************************************/
void CTrafficList::DumpBufferHexText(const BYTE *pBuf, DWORD dwLen, FILE *fh)
{
DWORD dwCounter, dwCnt;

	for (dwCounter = 0; dwCounter < dwLen / 16; dwCounter++) {
		fprintf(fh, "%08x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x ",
			dwCounter * 16, pBuf[dwCounter * 16], pBuf[dwCounter * 16 + 1],
			pBuf[dwCounter * 16 + 2], pBuf[dwCounter * 16 + 3], pBuf[dwCounter * 16 + 4],
			pBuf[dwCounter * 16 + 5], pBuf[dwCounter * 16 + 6], pBuf[dwCounter * 16 + 7],
			pBuf[dwCounter * 16 + 8], pBuf[dwCounter * 16 + 9], pBuf[dwCounter * 16 + 10],
			pBuf[dwCounter * 16 + 11], pBuf[dwCounter * 16 + 12], pBuf[dwCounter * 16 + 13],
			pBuf[dwCounter * 16 + 14], pBuf[dwCounter * 16 + 15]);

		for (dwCnt = 0; dwCnt < 16; dwCnt++)
			if (strchr(szFrmChars, pBuf[dwCounter * 16 + dwCnt]))
				fputc(' ', fh);
			else
				fputc(pBuf[dwCounter * 16 + dwCnt], fh);

		fprintf(fh, "\n");
	}
	if (dwLen % 16 != 0) {
		fprintf(fh, "%08x ", dwCounter * 16);
		for (dwCnt = 0; dwCnt < dwLen % 16; dwCnt++)
			fprintf(fh, "%02x ", pBuf[dwCounter * 16 + dwCnt]);
		for (;dwCnt < 16; dwCnt++)
			fprintf(fh, "   ");
		for (dwCnt = 0; dwCnt < dwLen % 16; dwCnt++)
			if (strchr(szFrmChars, pBuf[dwCounter * 16 + dwCnt]))
				fputc(' ', fh);
			else
				fputc(pBuf[dwCounter * 16 + dwCnt], fh);
		fprintf(fh, "\n");
	}
}
/****************************************************************
Show write information in pBuf (dwLen bytes long) to file (fh) 
formated as plain text
****************************************************************/
void CTrafficList::DumpBufferText(const BYTE *pBuf, DWORD dwLen, FILE *fh)
{
	fwrite(pBuf, 1, dwLen, fh);
}
/****************************************************************
Print standard header information:
1. time and date 2. protocol name 3. total packet length 4. source ip & port 
5. destination ip & port
note: in protocols without source and destonation ports (such as IP) this information
      is invalid
****************************************************************/
void CTrafficList::PrintStartInfo(const BYTE *wsb, FILE *fh)
{
SYSTEMTIME now;
	GetLocalTime(&now);
	fprintf(fh, "\n[%02d:%02d:%02d.%03d - %02d.%02d.%d]\n",
		now.wHour, now.wMinute, now.wSecond, now.wMilliseconds,
		now.wDay, now.wMonth, now.wYear);
	fprintf(fh, "Proto: %s ", Protocol[*(wsb + 9)]); /* protocol info */
	fprintf(fh, "len: %d ", ntohs(*(WORD *)(wsb + 2))); /* total length */
	fprintf(fh, "%d.%d.%d.%d:%d -> %d.%d.%d.%d:%d \n\n", 
		wsb[12], wsb[13], wsb[14], wsb[15], ntohs(*(WORD *)(wsb + 20)),
		wsb[16], wsb[17], wsb[18], wsb[19], ntohs(*(WORD *)(wsb + 22)));
}
/****************************************************************
return length of header of protocol byProto
note: ICMP has 28 bytes header (IP + ICMP) but if the ICMP code 
	  is Timestamp or Timestamp Reply it is 40 bytes ...so I always 
	  show the whole header info
****************************************************************/
DWORD CTrafficList::GetHdrLenByProto(BYTE byProto)
{
	switch (byProto) {
	case 4: return 20;		/* IP */
	case 17: return 28;		/* UDP */
	case 6: return 40;		/* TCP */
	case 2: return 28;		/* IGMP */
	}
	return 0;
}
/****************************************************************
Check the file size and delete it if too big
****************************************************************/
void CTrafficList::CheckAutoDelete()
{
	if (g_lTotalDelBytes < ftell(g_logfile)) {
		TRACE("Deleting file!\n");
		CloseLogFile();
		OpenLogFile(_T("w+"));
	}
}
/****************************************************************
Add/update packet rows
****************************************************************/
void CTrafficList::ShowPacket(const BYTE *pbyPacket)
{
DWORD dwHdrLen, dwPktLen, dwDataLen;
BYTE byProto;
int iPacket;

	byProto = pbyPacket[9];
	if (((6 == byProto) && (g_Settings.m_bFilterTCP)) || // filter TCP
		((17 == byProto) && (g_Settings.m_bFilterUDP)) || // filter UDP
		((1 == byProto) && (g_Settings.m_bFilterICMP))) // filter ICMP
		return;

	// general init
	dwHdrLen = GetHdrLenByProto(byProto);
	dwPktLen = (DWORD)ntohs(*(WORD *)(pbyPacket + 2));
	if (dwHdrLen >= dwPktLen) /* drop no-data packets */
		return;
	dwDataLen = dwPktLen - dwHdrLen; // remove header length
	g_dwTotalPackets++;
	g_dwTotalBytes += dwDataLen;
	sprintf(szNum, "All packets: %u     Total bytes: %u", g_dwTotalPackets, g_dwTotalBytes);
	g_pwndStatusBar->SetWindowText(szNum);

	// Fill address info
	CurAddr.byProto = byProto;
	CurAddr.dwSrcIP = *(DWORD *)(pbyPacket + 12);
	CurAddr.dwDestIP = *(DWORD *)(pbyPacket + 16);
	CurAddr.wSrcPort = *(WORD *)(pbyPacket + 20);
	CurAddr.wDestPort = *(WORD *)(pbyPacket + 22);

	// log packet
	if (NULL != g_logfile) {
		CheckAutoDelete();

		PrintStartInfo(pbyPacket, g_logfile); // log the standard header 

		switch (g_Settings.m_iLogType) {
		case 1: DumpBufferText(pbyPacket, dwPktLen, g_logfile); break;
		case 2: DumpBufferHex(pbyPacket, dwPktLen, g_logfile); break;
		case 3: DumpBufferHexText(pbyPacket + dwHdrLen, dwDataLen, g_logfile); break;
		case 4: DumpBufferText(pbyPacket + dwHdrLen, dwDataLen, g_logfile); break;
		case 5: DumpBufferHex(pbyPacket + dwHdrLen, dwDataLen, g_logfile); break;
		case 6: break;
		default: DumpBufferHexText(pbyPacket, dwPktLen, g_logfile);
		}
	}
	
	// show packet in list
	if (0 <= (iPacket = FindPacket())) {
		// update internal info
		PTrafInfo pTrafInfo = (PTrafInfo)g_pwndTraffic->GetItemData(iPacket);
		pTrafInfo->dwBytes += dwPktLen;
		pTrafInfo->dwTimeStamp = GetTickCount();
		// show changed bytes
		_ultoa(pTrafInfo->dwBytes, szNum, 10);
		g_pwndTraffic->SetItemText(iPacket, 5, szNum);
	}
	else {
		AddPacket(dwPktLen);
	}
}

/****************************************************************
Finds packet by item data
return item number or -1 on error
****************************************************************/
int CTrafficList::FindPacket()
{
int iItems = g_pwndTraffic->GetItemCount();

	for (int iCounter = 0; iCounter < iItems; iCounter++) {
		if (0 == memcmp(&CurAddr, (void *)g_pwndTraffic->GetItemData(iCounter), sizeof(TAddrInfo)))
			return iCounter;
	}
	return -1;
}
/****************************************************************
Adds a packet row depending on CurAddr and dwLen
****************************************************************/
void CTrafficList::AddPacket(DWORD dwLen)
{
	// fill the traffic info
	PTrafInfo pTrafInfo = new(TTrafInfo);
	memcpy(pTrafInfo, &CurAddr, sizeof(TAddrInfo));
	pTrafInfo->dwBytes = dwLen;
	pTrafInfo->dwBytesSec = 0;
	pTrafInfo->dwTimeStamp = GetTickCount();
	pTrafInfo->dwTimeStampStart = pTrafInfo->dwTimeStamp;

	// insert new row
	LV_ITEM lvi;
	lvi.iItem = g_pwndTraffic->GetItemCount(); // add at the end of the control
	lvi.iSubItem = 0;
	lvi.lParam = (LPARAM)pTrafInfo;
	lvi.mask = LVIF_TEXT | LVIF_PARAM;
	lvi.pszText = g_NSCashe.GetNameByIP(CurAddr.dwSrcIP); //inet_ntoa(*(in_addr *)(&CurAddr.dwSrcIP));

	int iIndex = g_pwndTraffic->InsertItem(&lvi);
	TCHAR *szPort;
	WORD wPort;
	if (-1 != iIndex) {
		g_pwndTraffic->SetItemText(iIndex, 2, g_NSCashe.GetNameByIP(CurAddr.dwDestIP)); // ip
		g_pwndTraffic->SetItemText(iIndex, 4, Protocol[CurAddr.byProto]); // protocol
		_ultoa(dwLen, szNum, 10);
		g_pwndTraffic->SetItemText(iIndex, 5, szNum); // packet length
		// source port
		wPort = htons(CurAddr.wSrcPort);
		if (NULL == (szPort = g_ServicesParser.GetServiceName(wPort, CurAddr.byProto))) {
			_ultoa((DWORD)wPort, szNum, 10);
			g_pwndTraffic->SetItemText(iIndex, 1, szNum);
		}
		else 
			g_pwndTraffic->SetItemText(iIndex, 1, szPort);
		// destination port
		wPort = htons(CurAddr.wDestPort); 
		if (NULL == (szPort = g_ServicesParser.GetServiceName(wPort, CurAddr.byProto))) {
			_ultoa((DWORD)wPort, szNum, 10);
			g_pwndTraffic->SetItemText(iIndex, 3, szNum);
		}
		else 
			g_pwndTraffic->SetItemText(iIndex, 3, szPort);
	}
	if (0 == lvi.iItem) // if this is the first item added invalidate to redwaw :)
		g_pwndTraffic->Invalidate();
}
/****************************************************************
removes all packets from the list and frees memory
****************************************************************/
void CTrafficList::ClearPackets()
{
	int iItems = g_pwndTraffic->GetItemCount();
	for (int iCounter = 0; iCounter < iItems; iCounter++)
		delete (PTrafInfo)g_pwndTraffic->GetItemData(iCounter);
	g_pwndTraffic->DeleteAllItems();
}
/****************************************************************
removes all packets depending on protocol information from the list and frees memory
****************************************************************/
void CTrafficList::FilterProto(BYTE byProto)
{
	PTrafInfo pTrafInfo;
	for (int iCounter = 0; iCounter < g_pwndTraffic->GetItemCount(); iCounter++) {
		pTrafInfo = (PTrafInfo)g_pwndTraffic->GetItemData(iCounter);
		if (byProto == pTrafInfo->addr.byProto) {
			// timeout .. packet row is about to be removed
			delete (PTrafInfo)g_pwndTraffic->GetItemData(iCounter);
			g_pwndTraffic->DeleteItem(iCounter);
			iCounter--;
			continue;
		}
	}
}


