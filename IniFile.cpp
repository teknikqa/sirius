
//////////////////////////////////////////////////////////////////////
// IniFile.cpp: implementation of the CIniFile class.
// Autor: Teknikqa (teknikqa@gmail.com, http://www.geocities.com/teknikqa
// Last modified 31.2005
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "IniFile.h"

static const TCHAR szExtention[] = _T(".dat");

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CIniFile::CIniFile()
{
TCHAR *PointPos, *SlashPos;
	GetModuleFileName(NULL, m_szFileName, FILE_NAME_LEN);
	PointPos = _tcsrchr(m_szFileName, _T('.'));
	if (PointPos == NULL) {	// no '.' in file name
		_tcscat(m_szFileName, szExtention);
		return;
	}
	SlashPos = _tcsrchr(m_szFileName, _T('\\'));
	// '\' is before '.' or no '\'
	if ((SlashPos == NULL) || (PointPos > SlashPos))
		*PointPos = 0;
	_tcscat(m_szFileName, szExtention);
}

CIniFile::CIniFile(TCHAR *szFileName)
{
	lstrcpyn(m_szFileName, szFileName, FILE_NAME_LEN);
}

CIniFile::~CIniFile()
{

}

//////////////////////////////////////////////////////////////////////
// Implementation
//////////////////////////////////////////////////////////////////////
void CIniFile::GetString(LPCTSTR lpSection, LPCTSTR lpKey, LPCTSTR lpDefault, LPTSTR lpResult, DWORD dwSize)
{
	GetPrivateProfileString(lpSection, lpKey, lpDefault, 
		lpResult, dwSize, m_szFileName);
}

void CIniFile::SetString(LPCTSTR lpSection, LPCTSTR lpKey, LPCTSTR lpString)
{
	WritePrivateProfileString(lpSection, lpKey, lpString, m_szFileName);
}

int CIniFile::GetInt(LPCTSTR lpSection, LPCTSTR lpKey, int iDefault)
{
	return GetPrivateProfileInt(lpSection, lpKey, iDefault, m_szFileName);
}

void CIniFile::SetInt(LPCTSTR lpSection, LPCTSTR lpKey, int iValue)
{
TCHAR szDefault[FILE_NAME_LEN];

	_itot(iValue, szDefault, 10);
	WritePrivateProfileString(lpSection, lpKey, szDefault, m_szFileName);
}

DWORD CIniFile::GetDWORD(LPCTSTR lpSection, LPCTSTR lpKey, DWORD dwDefault)
{
TCHAR szDefault[FILE_NAME_LEN];
TCHAR szBuf[FILE_NAME_LEN];
TCHAR *pc;

	_stprintf(szDefault, _T("%u"), dwDefault);
	GetPrivateProfileString(lpSection, lpKey, szDefault, szBuf, FILE_NAME_LEN, m_szFileName);
	return _tcstoul(szBuf, &pc, 10);
}

void CIniFile::SetDWORD(LPCTSTR lpSection, LPCTSTR lpKey, DWORD dwValue)
{
TCHAR szDefault[FILE_NAME_LEN];

	_stprintf(szDefault, _T("%u"), dwValue);
	WritePrivateProfileString(lpSection, lpKey, szDefault, m_szFileName);
}
