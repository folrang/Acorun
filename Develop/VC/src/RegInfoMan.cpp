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

// [in] lpSubKey: _RootPath(ex:"SOFTWARE\\MyCompany") �Ʒ��� SubKey��(ex:"MyProduct\\service")
// [in] lpValue: ���� value ��
// [in, out] nResult: �ش� Ű�� ���� ���� ��� ��, retVal�� FALSE���� ApiError Code ��ȯ
// [retVal]: TRUE - ����, FALSE - ����(nResult: LastError��ȯ)
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

// [in] lpSubKey: _RootPath(ex:"SOFTWARE\\MyCompany") �Ʒ��� SubKey��(ex:"MyProduct\\service")
// [in] lpValue: ���� value ��
// [in, out] lpResult: �ش� Ű�� ���� ���� ��� ��, retVal�� FALSE���� NULL ��ȯ
// [in] dwSize: ���� ���� ũ��
// [retVal]: TRUE - ����, FALSE - ����(nResult: NULL��ȯ)
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
	// ������Ʈ�� ���� �� ���ڿ��� ��� lpResult�� �� ���ڿ��� ó���Ѵ�.
	if (dwRead == 0) lpResult[0] = 0;
	RegCloseKey(hKey);
	return retVal;
}

// [in] lpSubKey: _RootPath(ex:"SOFTWARE\\MyCompany") �Ʒ��� SubKey��(ex:"MyProduct\\service")
// [in] lpValue: ���� value ��
// [in] nResult: ������ Ű��
// [retVal]: TRUE - ����, FALSE - ����
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

// [in] lpSubKey: _RootPath(ex:"SOFTWARE\\MyCompany") �Ʒ��� SubKey��(ex:"MyProduct\\service")
// [in] lpValue: ���� value ��
// [in] lpData: ������ Ű ��
// [retVal]: TRUE - ����, FALSE - ����
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