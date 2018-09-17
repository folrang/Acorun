/****************************************************************/
/*																*/
/*  RegKey.cpp: implementation of the CRegKey class.			*/
/*																*/
/*  This is an clone of the ATL CRegKey from Microsoft.			*/
/*																*/
/*  Modified for use with MFC by Pablo van der Meer				*/
/*  Copyright Pablo Software Solutions 2005						*/
/*	http://www.pablosoftwaresolutions.com						*/
/*																*/
/*  Last updated: April 30, 2005								*/
/*																*/
/****************************************************************/

#include "stdafx.h"
#include "RegKey4MFC.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


CRegKey::CRegKey()
{
	m_hKey = NULL;
}


CRegKey::~CRegKey()
{
	Close();
}


/********************************************************************/
/*																	*/
/* Function name : operator HKEY									*/
/* Description   : Converts CRegKey object to an HKEY.				*/
/*																	*/
/********************************************************************/
CRegKey::operator HKEY() const
{
	return m_hKey;
}


/********************************************************************/
/*																	*/
/* Function name : Detach											*/
/* Description   : Detaches m_hKey from the CregKey object			*/
/*																	*/
/********************************************************************/
HKEY CRegKey::Detach()
{
	HKEY hKey = m_hKey;
	m_hKey = NULL;
	return hKey;
}


/********************************************************************/
/*																	*/
/* Function name : Attach											*/
/* Description   : Attaches registry key handle to CRegKeyobject.	*/
/*																	*/
/********************************************************************/
void CRegKey::Attach(HKEY hKey)
{
	ASSERT(m_hKey == NULL);
	m_hKey = hKey;
}


/********************************************************************/
/*																	*/
/* Function name : DeleteSubKey										*/
/* Description   : Deletes the specified key.						*/
/*																	*/
/********************************************************************/
LONG CRegKey::DeleteSubKey(LPCTSTR lpszSubKey)
{
	ASSERT(m_hKey != NULL);
	return RegDeleteKey(m_hKey, lpszSubKey);
}


/********************************************************************/
/*																	*/
/* Function name : DeleteValue										*/
/* Description   : Deletes a value field of the key identified by	*/
/*				   m_hKey.											*/
/*																	*/
/********************************************************************/
LONG CRegKey::DeleteValue(LPCTSTR lpszValue)
{
	ASSERT(m_hKey != NULL);
	return RegDeleteValue(m_hKey, (LPTSTR)lpszValue);
}


/********************************************************************/
/*																	*/
/* Function name : Close											*/
/* Description   : Releases m_hKey.									*/
/*																	*/
/********************************************************************/
LONG CRegKey::Close()
{
	LONG lRes = ERROR_SUCCESS;
	if (m_hKey != NULL)
	{
		lRes = RegCloseKey(m_hKey);
		m_hKey = NULL;
	}
	return lRes;
}


/********************************************************************/
/*																	*/
/* Function name : Create											*/
/* Description   : Creates or opens the specified key.				*/
/*																	*/
/********************************************************************/
LONG CRegKey::Create(HKEY hKeyParent, LPCTSTR lpszKeyName,
	LPTSTR lpszClass, DWORD dwOptions, REGSAM samDesired,
	LPSECURITY_ATTRIBUTES lpSecAttr, LPDWORD lpdwDisposition)
{
	ASSERT(hKeyParent != NULL);
	DWORD dw;
	HKEY hKey = NULL;
	LONG lRes = RegCreateKeyEx(hKeyParent, lpszKeyName, 0,
		lpszClass, dwOptions, samDesired, lpSecAttr, &hKey, &dw);
	if (lpdwDisposition != NULL)
		*lpdwDisposition = dw;
	if (lRes == ERROR_SUCCESS)
	{
		lRes = Close();
		m_hKey = hKey;
	}
	return lRes;
}


/********************************************************************/
/*																	*/
/* Function name : Open												*/
/* Description   : Opens the specified key.							*/
/*																	*/
/********************************************************************/
LONG CRegKey::Open(HKEY hKeyParent, LPCTSTR lpszKeyName, REGSAM samDesired)
{
	ASSERT(hKeyParent != NULL);
	HKEY hKey = NULL;
	LONG lRes = RegOpenKeyEx(hKeyParent, lpszKeyName, 0, samDesired, &hKey);
	if (lRes == ERROR_SUCCESS)
	{
		lRes = Close();
		ASSERT(lRes == ERROR_SUCCESS);
		m_hKey = hKey;
	}
	return lRes;
}


/********************************************************************/
/*																	*/
/* Function name : QueryValue										*/
/* Description   : Retrieves the data for a specified value field.	*/
/*																	*/
/********************************************************************/
LONG CRegKey::QueryValue(DWORD& dwValue, LPCTSTR lpszValueName)
{
	DWORD dwType = NULL;
	DWORD dwCount = sizeof(DWORD);
	LONG lRes = RegQueryValueEx(m_hKey, (LPTSTR)lpszValueName, NULL, &dwType,
		(LPBYTE)&dwValue, &dwCount);
	ASSERT((lRes!=ERROR_SUCCESS) || (dwType == REG_DWORD));
	ASSERT((lRes!=ERROR_SUCCESS) || (dwCount == sizeof(DWORD)));
	return lRes;
}


/********************************************************************/
/*																	*/
/* Function name : QueryValue										*/
/* Description   : Retrieves the data for a specified value field.	*/
/*																	*/
/********************************************************************/
LONG CRegKey::QueryValue(LPTSTR szValue, LPCTSTR lpszValueName, DWORD* pdwCount)
{
	ASSERT(pdwCount != NULL);
	DWORD dwType = NULL;
	LONG lRes = RegQueryValueEx(m_hKey, (LPTSTR)lpszValueName, NULL, &dwType,
		(LPBYTE)szValue, pdwCount);
	ASSERT((lRes!=ERROR_SUCCESS) || (dwType == REG_SZ) ||
			 (dwType == REG_MULTI_SZ) || (dwType == REG_EXPAND_SZ));
	return lRes;
}


/********************************************************************/
/*																	*/
/* Function name : SetValue											*/
/* Description   : Stores data in a specified value field.			*/
/*																	*/
/********************************************************************/
LONG WINAPI CRegKey::SetValue(HKEY hKeyParent, LPCTSTR lpszKeyName, LPCTSTR lpszValue, LPCTSTR lpszValueName)
{
	ASSERT(lpszValue != NULL);
	CRegKey key;
	LONG lRes = key.Create(hKeyParent, lpszKeyName);
	if (lRes == ERROR_SUCCESS)
		lRes = key.SetValue(lpszValue, lpszValueName);
	return lRes;
}


/********************************************************************/
/*																	*/
/* Function name : SetKeyValue										*/
/* Description   : Stores data in a specified value field of a		*/
/*				   specified key.									*/
/*																	*/
/********************************************************************/
LONG CRegKey::SetKeyValue(LPCTSTR lpszKeyName, LPCTSTR lpszValue, LPCTSTR lpszValueName)
{
	ASSERT(lpszValue != NULL);
	CRegKey key;
	LONG lRes = key.Create(m_hKey, lpszKeyName);
	if (lRes == ERROR_SUCCESS)
		lRes = key.SetValue(lpszValue, lpszValueName);
	return lRes;
}


/********************************************************************/
/*																	*/
/* Function name : SetValue											*/
/* Description   : Stores data in a specified value field.			*/
/*																	*/
/********************************************************************/
LONG CRegKey::SetValue(DWORD dwValue, LPCTSTR lpszValueName)
{
	ASSERT(m_hKey != NULL);
	return RegSetValueEx(m_hKey, lpszValueName, NULL, REG_DWORD,
		(BYTE * const)&dwValue, sizeof(DWORD));
}


/********************************************************************/
/*																	*/
/* Function name : SetValue											*/
/* Description   : Stores data in a specified value field.			*/
/*																	*/
/********************************************************************/
LONG CRegKey::SetValue(LPCTSTR lpszValue, LPCTSTR lpszValueName)
{
	ASSERT(lpszValue != NULL);
	ASSERT(m_hKey != NULL);
	return RegSetValueEx(m_hKey, lpszValueName, NULL, REG_SZ,
		(BYTE * const)lpszValue, (lstrlen(lpszValue)+1)*sizeof(TCHAR));
}


/********************************************************************/
/*																	*/
/* Function name : RecurseDeleteKey									*/
/* Description   : Deletes the specified key and explicitly deletes */
/*				   all subkeys.										*/
/*																	*/
/********************************************************************/
LONG CRegKey::RecurseDeleteKey(LPCTSTR lpszKey)
{
	CRegKey key;
	LONG lRes = key.Open(m_hKey, lpszKey, KEY_READ | KEY_WRITE);
	if (lRes != ERROR_SUCCESS)
		return lRes;
	FILETIME time;
	DWORD dwSize = 256;
	TCHAR szBuffer[256];
	while (RegEnumKeyEx(key.m_hKey, 0, szBuffer, &dwSize, NULL, NULL, NULL,
		&time)==ERROR_SUCCESS)
	{
		lRes = key.RecurseDeleteKey(szBuffer);
		if (lRes != ERROR_SUCCESS)
			return lRes;
		dwSize = 256;
	}
	key.Close();
	return DeleteSubKey(lpszKey);
}

