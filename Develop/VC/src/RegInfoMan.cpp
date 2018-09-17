// RegInfoMan.cpp: implementation of the CRegInfoMan class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <stdio.h>
#include "RegInfoMan.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRegInfoMan::CRegInfoMan(HKEY hKey /*= HKEY_LOCAL_MACHINE*/) : _hRootKey(hKey)
{
	_tcscpy(_RootPath, _T(""));
}

CRegInfoMan::~CRegInfoMan()
{

}

// [in] lpSubKey: _RootPath(ex:"SOFTWARE\\MyCompany") 아래의 SubKey값(ex:"MyProduct\\service")
// [in] lpValue: 읽을 value 값
// [in, out] nResult: 해당 키에 대한 읽은 결과 값, retVal이 FALSE에는 ApiError Code 반환
// [retVal]: TRUE - 성공, FALSE - 실패(nResult: LastError반환)
BOOL CRegInfoMan::RegReadInt(LPCTSTR lpSubKey, LPCTSTR lpValue, INT* nResult, BOOL bCreate /*= TRUE*/)
{
	HKEY hKey;
	BOOL retVal = TRUE;
	DWORD dwDisp, dwSize;
	_TCHAR FullPath[MAX_PATH+1];
	if (_tcslen(_RootPath) > 0)
		_stprintf(FullPath, _T("%s\\%s"), _RootPath, lpSubKey);
	else
		_tcscpy(FullPath, lpSubKey);
	
	if (RegOpenKeyEx(_hRootKey, FullPath, 0, KEY_READ, &hKey) != ERROR_SUCCESS) {
		if (bCreate) {
			if (RegCreateKeyEx(_hRootKey, FullPath, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_READ, NULL, &hKey, &dwDisp)
				!= ERROR_SUCCESS) {
				*nResult = GetLastError();
				return FALSE;
			}
		} else {
			*nResult = GetLastError();
			return FALSE;
		}
	}
	dwSize = sizeof(LONG);
	if (RegQueryValueEx(hKey, lpValue, 0, NULL, (LPBYTE)nResult, &dwSize) != ERROR_SUCCESS) {
		*nResult = GetLastError();
		retVal = FALSE;
	}
	RegCloseKey(hKey);
	return retVal;
}

// [in] lpSubKey: _RootPath(ex:"SOFTWARE\\MyCompany") 아래의 SubKey값(ex:"MyProduct\\service")
// [in] lpValue: 읽을 value 값
// [in, out] lpResult: 해당 키에 대한 읽은 결과 값, retVal이 FALSE에는 NULL 반환
// [in] dwSize: 설정 버퍼 크기
// [retVal]: TRUE - 성공, FALSE - 실패(nResult: NULL반환)
BOOL CRegInfoMan::RegReadString(LPCTSTR lpSubKey, LPCTSTR lpValue, LPTSTR lpResult, DWORD dwSize, BOOL bCreate /*= TRUE*/)
{
	HKEY hKey;
	BOOL retVal = TRUE;
	DWORD dwDisp, dwRead;
	_TCHAR FullPath[MAX_PATH+1];
	if (_tcslen(_RootPath) > 0)
		_stprintf(FullPath, _T("%s\\%s"), _RootPath, lpSubKey);
	else
		_tcscpy(FullPath, lpSubKey);

	if (RegOpenKeyEx(_hRootKey, FullPath, 0, KEY_READ, &hKey) != ERROR_SUCCESS) {		
		if (bCreate) {
			if (RegCreateKeyEx(_hRootKey, FullPath, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_READ, NULL, &hKey, &dwDisp)
				!= ERROR_SUCCESS) {
				return FALSE;
			}
		} else {
			return FALSE;
		}
	}
	dwRead = dwSize;
	if (RegQueryValueEx(hKey, lpValue, 0, NULL, (LPBYTE)lpResult, &dwRead) != ERROR_SUCCESS) {
		lpResult[0] = 0;
		retVal = FALSE;
	}
	// 레지스트리 값이 빈 문자열일 경우 lpResult를 빈 문자열로 처리한다.
	if (dwRead == 0) lpResult[0] = 0;
	RegCloseKey(hKey);
	return retVal;
}

// [in] lpSubKey: _RootPath(ex:"SOFTWARE\\MyCompany") 아래의 SubKey값(ex:"MyProduct\\service")
// [in] lpValue: 읽을 value 값
// [in] nResult: 설정할 키값
// [retVal]: TRUE - 성공, FALSE - 실패
BOOL CRegInfoMan::RegWriteInt(LPCTSTR lpSubKey, LPCTSTR lpValue, INT nData, BOOL bCreate /*= TRUE*/)
{
	HKEY hKey;
	BOOL retVal = TRUE;
	DWORD dwDisp;
	_TCHAR FullPath[MAX_PATH+1];
	if (_tcslen(_RootPath) > 0)
		_stprintf(FullPath, _T("%s\\%s"), _RootPath, lpSubKey);
	else
		_tcscpy(FullPath, lpSubKey);
	
	if (RegOpenKeyEx(_hRootKey, FullPath, 0, KEY_WRITE, &hKey) != ERROR_SUCCESS) {
		if (bCreate) {
			if (RegCreateKeyEx(_hRootKey, FullPath, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKey, &dwDisp)
				!= ERROR_SUCCESS) {
				return FALSE;
			}
		} else {
			return FALSE;
		}
	}
	if (RegSetValueEx(hKey, lpValue, 0, REG_DWORD, (LPBYTE)&nData, sizeof(INT))
		!= ERROR_SUCCESS) {
		retVal = FALSE;
	}
	RegCloseKey(hKey);
	return retVal;
}

// [in] lpSubKey: _RootPath(ex:"SOFTWARE\\MyCompany") 아래의 SubKey값(ex:"MyProduct\\service")
// [in] lpValue: 읽을 value 값
// [in] lpData: 설정할 키 값
// [retVal]: TRUE - 성공, FALSE - 실패
BOOL CRegInfoMan::RegWriteString(LPCTSTR lpSubKey, LPCTSTR lpValue, LPCTSTR lpData, BOOL bCreate /*= TRUE*/)
{
	HKEY hKey;
	BOOL retVal = TRUE;
	DWORD dwDisp;
	_TCHAR FullPath[MAX_PATH+1];
	if (_tcslen(_RootPath) > 0)
		_stprintf(FullPath, _T("%s\\%s"), _RootPath, lpSubKey);
	else
		_tcscpy(FullPath, lpSubKey);
	
	if (RegOpenKeyEx(_hRootKey, FullPath, 0, KEY_WRITE, &hKey) != ERROR_SUCCESS) {
		if (bCreate) {
			if (RegCreateKeyEx(_hRootKey, FullPath, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKey, &dwDisp)
				!= ERROR_SUCCESS) {
				return FALSE;
			}
		} else {
			return FALSE;
		}
	}
	if (RegSetValueEx(hKey, lpValue, 0, REG_SZ, (LPBYTE)lpData, _tcslen(lpData) * sizeof(_TCHAR))
		!= ERROR_SUCCESS) {
		retVal = FALSE;
	}
	RegCloseKey(hKey);
	return retVal;
}