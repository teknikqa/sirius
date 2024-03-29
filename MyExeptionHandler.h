
#ifndef __MYEXHND_H__
#define __MYEXHND_H__

class MyExceptionHandler
{
    public:
    
    MyExceptionHandler( );
    ~MyExceptionHandler( );
    
    void SetLogFileName( PTSTR pszLogFileName );

    private:

    // entry point where control comes on an unhandled exception
    static LONG WINAPI MyUnhandledExceptionFilter(
                                PEXCEPTION_POINTERS pExceptionInfo );

    // where report info is extracted and generated 
    static void GenerateExceptionReport( PEXCEPTION_POINTERS pExceptionInfo );

    // Helper functions
    static LPTSTR GetExceptionString( DWORD dwCode );
    static BOOL GetLogicalAddress(  PVOID addr, PTSTR szModule, DWORD len,
                                    DWORD& section, DWORD& offset );
    static void IntelStackWalk( PCONTEXT pContext );
    static int __cdecl _tprintf(const TCHAR * format, ...);

    // Variables used by the class
    static TCHAR m_szLogFileName[MAX_PATH];
    static LPTOP_LEVEL_EXCEPTION_FILTER m_previousFilter;
    static HANDLE m_hReportFile;
};

extern MyExceptionHandler g_MyExceptionHandler;   //  global instance of class

#endif