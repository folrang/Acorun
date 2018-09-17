#include "StdAfx.h"
#include "IniFileMap.h"
#include <Critsec.h>

CIniFileMap::CIniFileMap(void) : m_pCs(NULL), m_pMap(NULL)
{
	m_pCs = new CRITICAL_SECTION;
	InitializeCriticalSection(m_pCs);
	m_pMap = new MAP_INIFILEPTR;
}

CIniFileMap::~CIniFileMap(void)
{
	CIniFile* pIniFile = NULL;
	for (MAP_INIFILEPTR_CIT cit = m_pMap->begin(); cit != m_pMap->end(); cit++)
	{
		pIniFile = (*cit).second;
		pIniFile->Close();
		delete pIniFile;
	}
	
	DeleteCriticalSection(m_pCs);
	delete m_pCs;
}

BOOL CIniFileMap::AddIniFile(LPCTSTR szName, LPCTSTR szFilePath)
{
	BOOL retVal = FALSE;
	CIniFile* pIniFile = new CIniFile();
	if (pIniFile->Open(szFilePath))
	{
		EnterCriticalSection(m_pCs);
		retVal = m_pMap->insert(make_pair(szName, pIniFile)).second;
		LeaveCriticalSection(m_pCs);
	}

	if (!retVal)
		delete pIniFile;
		
	return retVal;
}

BOOL CIniFileMap::AddIniFiles(LPCTSTR szName, vector<tstring>& filePaths)
{
	int nSize = filePaths.size();
	if (nSize == 0) 
		return FALSE;

	BOOL retVal = FALSE;
	MAP_INI_SECSTION* pSectionMap = new MAP_INI_SECSTION();
	
	int i = 0;
	for (i = 0; i < nSize - 1; i++)
	{
		CIniFile IniFile(pSectionMap, FALSE);
		IniFile.Open(filePaths[i].c_str());
	}

	CIniFile* pIniFile = new CIniFile(pSectionMap, TRUE);
	if (pIniFile->Open(filePaths[nSize-1].c_str()))
	{
		EnterCriticalSection(m_pCs);
		retVal = m_pMap->insert(make_pair(szName, pIniFile)).second;
		LeaveCriticalSection(m_pCs);
	}

	if (!retVal)
		delete pIniFile;
		
	return retVal;
}
	
tstring CIniFileMap::GetStrVal(LPCTSTR szName, LPCTSTR szSectionName, LPCTSTR szKey)
{
	CAutoLeaveCritSec Acs(m_pCs);

	tstring retVal(_T(""));
	MAP_INIFILEPTR_CIT cit_Ini = m_pMap->find(szName);
	if (cit_Ini != m_pMap->end())
		retVal = (*cit_Ini).second->GetStrVal(szSectionName, szKey);
	
	return retVal;
}

int CIniFileMap::GetSize()
{
	CAutoLeaveCritSec Acs(m_pCs);
	return m_pMap->size();
}

int CIniFileMap::GetSectionSize(LPCTSTR szName)
{
	CAutoLeaveCritSec Acs(m_pCs);

	int retVal = 0;
	MAP_INIFILEPTR_CIT cit = m_pMap->find(szName);
	if (cit != m_pMap->end())
		retVal = (*cit).second->GetSectionSize();
	
	return retVal;
}

int CIniFileMap::GetKeySize(LPCTSTR szName, LPCTSTR szSectionName)
{
	CAutoLeaveCritSec Acs(m_pCs);

	int retVal = 0;
	MAP_INIFILEPTR_CIT cit = m_pMap->find(szName);
	if (cit != m_pMap->end())
		retVal = (*cit).second->GetKeySize(szSectionName);
	
	return retVal;
}
