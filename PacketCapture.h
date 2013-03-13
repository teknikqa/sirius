
#ifndef _PACKETCAPTURE_H_
#define _PACKETCAPTURE_H_

#include "TrafficList.h"
#include "NSCashe.h"
#include "ServicesParser.h"
#include "Settings.h"

extern CComboBox *g_pwndAdapter;
extern CListCtrl *g_pwndTraffic;
extern TCHAR g_szError[];
extern TCHAR g_szWarning[];
extern CStatusBar *g_pwndStatusBar; 
extern CTrafficList g_TrafficList;
extern CNSCashe g_NSCashe;
extern CServicesParser g_ServicesParser;
extern CSettings g_Settings;
extern DWORD g_dwTotalPackets;
extern DWORD g_dwTotalBytes;
extern volatile BOOL g_bTimerEvent;
extern DWORD g_dwBPS;
extern DWORD g_dwABS;
extern FILE *g_logfile; // log file handle
extern long g_lTotalDelBytes;
extern BOOL g_bStarted; // is capturing started

int InitializeCapture(const char *szAdapterName);
int StartCapture(const char *szAdapterName);
void EndCapture(void);
void OpenLogFile(TCHAR *mode);
void CloseLogFile(void);

#endif