
/****************************************************************
Author: Teknikqa - http://www.geocities.com/teknikqa (teknikqa@gmail.com)
****************************************************************/

#include "stdafx.h"
#include "Settings.h"
#include "ServicesParser.h"
#include "NSCashe.h"
#include "TrafficList.h"
#include "PacketCapture.h"
#include "WPdpack\Include\packet32.h"
#include "WPdpack\Include\ntddndis.h"

#ifndef __NetBSD__
#define BPF_ALIGNMENT sizeof(int)
#else
#define BPF_ALIGNMENT sizeof(long)
#endif
#define BPF_WORDALIGN(x) (((x)+(BPF_ALIGNMENT-1))&~(BPF_ALIGNMENT-1))

// global vars
CComboBox *g_pwndAdapter = NULL;	// for global usage (not in this file)
CListCtrl *g_pwndTraffic = NULL;
CStatusBar *g_pwndStatusBar = NULL; 
CTrafficList g_TrafficList;
CNSCashe g_NSCashe;
CServicesParser g_ServicesParser;
CSettings g_Settings;
DWORD g_dwTotalPackets;
DWORD g_dwTotalBytes;
TCHAR g_szError[] = "Error"; // error message title
TCHAR g_szWarning[] = "Warning"; // warning message title
volatile BOOL g_bTimerEvent = FALSE; // timer event
DWORD g_dwBPS;	// bytes per second
DWORD g_dwABS;	// average bytes per second
FILE *g_logfile = NULL; // log file handle
long g_lTotalDelBytes = 0; 
BOOL g_bStarted = FALSE; // is capturing started

#define BUFFER_SIZE		(1048576) /* 1M */

// local file vars
static int iLanDataOffset;			/* offset of IP data in LAN data frame */
static DWORD dwBytesSec;
static DWORD dwTimeStampStart;
static BOOL bThreadActive = TRUE;	/* is the thread active? */
static HANDLE hSniffThread;			/* sniff-thread handle */
static DWORD dwSniffThreadID;		/* sniff-thread id */
LPADAPTER  lpAdapter = 0;			/* define a pointer to an ADAPTER structure */
LPPACKET   lpPacket;				/* define a pointer to a PACKET structure */
char buffer[BUFFER_SIZE];			/* buffer to hold the data coming from the driver */
char szErrorString[300];			/* buffer to hold error messages */

/****************************************************************
Check network type and specify an offset of real data in iLanDataOffset
****************************************************************/
int GetLanType(void)
{
BOOL bResult;
NetType stNetType;

	bResult = PacketGetNetType(lpAdapter, &stNetType);
	if (bResult) {
		switch (stNetType.LinkType) {
		case NdisMedium802_5: iLanDataOffset = 22; return 0; // Token Ring (802.5) 
		//case NdisMedium802_3: iLanDataOffset = 14; return 0; // Ethernet (802.3)
		default: iLanDataOffset = 14; return 0; // Ethernet (802.3) ??? and others
		}
	}
	::MessageBox(NULL, "Cannot get network type!\n", g_szError, MB_OK | MB_ICONSTOP);
	return 3;
}
/****************************************************************
create sniffing socket and bind it to the adapter iAdapterNr
****************************************************************/
int InitializeCapture(const char *szAdapterName)
{
	lpAdapter = PacketOpenAdapter((char *)szAdapterName);
	
	if (!lpAdapter || (lpAdapter->hFile == INVALID_HANDLE_VALUE)) {
		sprintf(szErrorString, "Unable to open the driver, Error Code : %u", GetLastError()); 
		::MessageBox(NULL, szErrorString, g_szError, MB_OK | MB_ICONSTOP);
		return 1;
	}	

	// set the network adapter in promiscuous mode 
	PacketSetHwFilter(lpAdapter, NDIS_PACKET_TYPE_PROMISCUOUS);

	// set a 2Mb buffer in the driver 
	PacketSetBuff(lpAdapter, 2048 * 1024);

	// set a 1 second read timeout
	PacketSetReadTimeout(lpAdapter, 1000);
	
	// allocate and initialize a packet structure that will be used to receive the packets.
	if (NULL == (lpPacket = PacketAllocatePacket())) {
		::MessageBox(NULL, "Failed to allocate the LPPACKET structure.", g_szError, MB_OK | MB_ICONSTOP);
		return 2;
	}
	PacketInitPacket(lpPacket, (char*)buffer, BUFFER_SIZE);

	return GetLanType();
}
/****************************************************************
stop sniffing thread and close sniffing socket
****************************************************************/
void EndCapture(void)
{
	if (bThreadActive) {
		bThreadActive = FALSE;
		Sleep(500);
		TerminateThread(hSniffThread, dwSniffThreadID);
		g_TrafficList.ClearPackets();
		// free packet space
		PacketCloseAdapter(lpAdapter);
		PacketFreePacket(lpPacket);
		CloseLogFile();
	}
	g_dwTotalBytes = g_dwTotalPackets = 0;
}
/****************************************************************
timer
****************************************************************/
void _stdcall OnTimer()
{
	PTrafInfo pTrafInfo;
	DWORD dwPacketTime;
	char szNum[300];
	DWORD dwSeconds;
	for (int iCounter = 0; iCounter < g_pwndTraffic->GetItemCount(); iCounter++) {
		pTrafInfo = (PTrafInfo)g_pwndTraffic->GetItemData(iCounter);
		dwPacketTime = GetTickCount() - pTrafInfo->dwTimeStamp;
		if (g_Settings.m_dwIdleSeconds < dwPacketTime) {
			// timeout .. packet row is about to be removed
			delete (PTrafInfo)g_pwndTraffic->GetItemData(iCounter);
			g_pwndTraffic->DeleteItem(iCounter);
			iCounter--;
			continue;
		}
		// show bytes per second
		_ultoa(pTrafInfo->dwBytes - pTrafInfo->dwBytesSec, szNum, 10);
		g_pwndTraffic->SetItemText(iCounter, 6, szNum);
		pTrafInfo->dwBytesSec = pTrafInfo->dwBytes;
		// show average bytes per second
		if ((pTrafInfo->dwTimeStamp > pTrafInfo->dwTimeStampStart) && 
			(0 != (dwSeconds = (pTrafInfo->dwTimeStamp - pTrafInfo->dwTimeStampStart) / 1000))) {
			_ultoa(pTrafInfo->dwBytes / dwSeconds, szNum, 10);
			g_pwndTraffic->SetItemText(iCounter, 7, szNum);
		}
	}
	// show total info 
	DWORD dwTicks = GetTickCount();
	if ((dwTicks > dwTimeStampStart) && 
		(0 != (dwSeconds = (dwTicks - dwTimeStampStart) / 1000))) {
		g_dwABS = g_dwTotalBytes / dwSeconds;
	}
	else
		g_dwABS = 0;

	g_dwBPS = g_dwTotalBytes - dwBytesSec;
	dwBytesSec = g_dwTotalBytes;

	g_bTimerEvent = FALSE; // remove timer event
}
/****************************************************************
sniffing thread
****************************************************************/
DWORD _stdcall SniffThread(void *param)
{
BYTE *wsb, *pbyBeginBuf, *pbyEndBuf;
int iCapLen, iHdrLen;

	while (bThreadActive) {
		if (PacketReceivePacket(lpAdapter, lpPacket, TRUE)) {
			pbyBeginBuf = (BYTE *)lpPacket->Buffer;
			pbyEndBuf = pbyBeginBuf + lpPacket->ulBytesReceived;
			while (pbyBeginBuf < pbyEndBuf) {
				iCapLen = ((struct bpf_hdr *)pbyBeginBuf)->bh_caplen;
				iHdrLen = ((struct bpf_hdr *)pbyBeginBuf)->bh_hdrlen;
				wsb = pbyBeginBuf + iHdrLen + iLanDataOffset; 
				if (0x45 == *wsb)  { // IP packets only plz 
					g_TrafficList.ShowPacket(wsb);
				}
				pbyBeginBuf += BPF_WORDALIGN(iCapLen + iHdrLen);
			}
		}
		if (g_bTimerEvent)
			OnTimer();
	}
	
	return 0;
}
/****************************************************************
init all and start sniffing thread
****************************************************************/
int StartCapture(const char *szAdapterName)
{
	if (0 != InitializeCapture(szAdapterName)) 
		return 1;

	// init statistic vars
	dwTimeStampStart = GetTickCount();
	dwBytesSec = 0;

	bThreadActive = TRUE;
	if (NULL == (hSniffThread = CreateThread(NULL, 0, SniffThread, 0, 0, &dwSniffThreadID))) {
		::MessageBox(NULL, "Thread  creation failed!", g_szError, MB_OK | MB_ICONSTOP);
		return 1;
	}
	SetThreadPriority(hSniffThread, THREAD_PRIORITY_HIGHEST);
	g_dwTotalBytes = g_dwTotalPackets = 0;

	OpenLogFile(_T("a+"));

	return 0;	
}

/****************************************************************
open log file for writing
****************************************************************/
void OpenLogFile(TCHAR *mode)
{
	if (g_Settings.m_bUseLog) {
		g_logfile = _tfopen(g_Settings.m_sLogFile, mode);
		if (NULL == g_logfile) {
			TCHAR message[MAX_PATH * 2];
			_stprintf(message, _T("Cannot open log file \"%s\" for writing!\r\nThe logging will be disabled.\r\nYou could enable it from the Settings."),
				g_Settings.m_sLogFile);
			::MessageBox(NULL, message, g_szWarning, MB_OK | MB_ICONINFORMATION);
			g_Settings.m_bUseLog = FALSE;
		}
	}
}

/****************************************************************
close the log file
****************************************************************/
void CloseLogFile(void)
{
	if (NULL != g_logfile) {
		fclose(g_logfile);
		g_logfile = NULL;
	}
}
