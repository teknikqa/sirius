// IniFile.h: interface for the CIniFile class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_INIFILE_H_)
#define AFX_INIFILE_H_

#define FILE_NAME_LEN	500

class CIniFile  
{
public:
	CIniFile();
	CIniFile(TCHAR *szFileName);
	virtual ~CIniFile();

	void SetInt(LPCTSTR lpSection, LPCTSTR lpKey, int iValue);
	int GetInt(LPCTSTR lpSection, LPCTSTR lpKey, int iDefault);
	void SetString(LPCTSTR lpSection, LPCTSTR lpKey, LPCTSTR lpString);
	void GetString(LPCTSTR lpSection, LPCTSTR lpKey, LPCTSTR lpDefault, LPTSTR lpResult, DWORD dwSize);
	DWORD GetDWORD(LPCTSTR lpSection, LPCTSTR lpKey, DWORD dwDefault);
	void SetDWORD(LPCTSTR lpSection, LPCTSTR lpKey, DWORD dwValue);

private:
	TCHAR m_szFileName[FILE_NAME_LEN];

};

#endif // !defined(AFX_INIFILE_H_)
