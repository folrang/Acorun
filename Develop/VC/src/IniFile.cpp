#include "StdAfx.h"
#include "IniFile.h"
#include <AtlBase.h> 	// CX2X

CIniFile::CIniFile()
{
	_tcscpy(m_szPath, _T(""));
	m_pSectionMap = new MAP_INI_SECSTION();
	m_bDestroySectionMap = TRUE;
}

// [in] pSectionMapPtr: new ������ ��ü
// [in] bDestroySectionMap: pSectionMap�� Clear �� Destroy ��ų�� ����
//			FALSE: Close() �� pSectionMap ���� Clear() �۾��� ���ο��� �߻����� ������, 
//				�Ҹ��� �Լ������� pSectionMap ��ü�� �������� ����
CIniFile::CIniFile(MAP_INI_SECSTION* pSectionMap, BOOL bDestroySectionMap)
{
	m_pSectionMap = pSectionMap;
	m_bDestroySectionMap = bDestroySectionMap;
}

CIniFile::~CIniFile(void)
{
	if (m_bDestroySectionMap)
	{
		Close();
		delete m_pSectionMap;
	}
}

int CIniFile::Open(LPCTSTR szFilePath)
{
	BOOL retVal = FALSE;
	_tcscpy(m_szPath, szFilePath);

	_TCHAR* pSectionNamesBuf = new _TCHAR [CIniFile::MAX_SECTIONNAMES_SIZE];
	
	DWORD dwRetChars;

	// ini ���ϳ��� ���Ǹ�� ������
	// pBuf: "���Ǹ�1\0���Ǹ�2\0....\0���Ǹ�n\0\0"
	dwRetChars = GetPrivateProfileSectionNames(pSectionNamesBuf, CIniFile::MAX_SECTIONNAMES_SIZE, m_szPath);

	if (dwRetChars)
	{
		tstring sectionName;
		MAP_INI_STRKEY_STRVAL* pValMap = NULL;
		_TCHAR szSectionName[256];
		_TCHAR* pPos;
		
		pPos = pSectionNamesBuf;

		// ���Ǹ� ����Ʈ �Ľ�
		while (pPos = GetToken(pPos, szSectionName))
		{
			if (*szSectionName)
			{
				sectionName = szSectionName;
				if (m_pSectionMap->find(sectionName) == m_pSectionMap->end())
				{
					// ���Ǹ�� �� key:val ���� �ʿ� �߰�
					pValMap = new MAP_INI_STRKEY_STRVAL;
					m_pSectionMap->insert(make_pair(sectionName, pValMap));
				}
			}
		}
		
		// �� ���� ���� ������ �����Ͽ� �Ľ� �� key�ʿ� �߰�
		_TCHAR* pSectionCtxBuf = new _TCHAR [CIniFile::MAX_SECTION_CONTENT_SIZE];
		
		for (MAP_INI_SECSTION_IT itSectionMap = m_pSectionMap->begin(); itSectionMap != m_pSectionMap->end(); ++itSectionMap)
		{
			
			//_tprintf(_T("\n[%s]\n"), itSectionMap->first.c_str());

			// ���ǳ��� ���� �������
			dwRetChars = GetPrivateProfileSection(itSectionMap->first.c_str(), pSectionCtxBuf, CIniFile::MAX_SECTION_CONTENT_SIZE, m_szPath);

			if (dwRetChars)
			{
				_TCHAR szLine[CIniFile::MAX_LINE_SIZE];
				tstring key, val;

				pPos = pSectionCtxBuf;
				
				// ���ǳ��� �� ����(key=val) �������
				while (pPos = GetToken(pPos, szLine))
				{
					if (*szLine)
					{
						// key �� val �и�
						SpliceKeyVal(szLine, key, val);

						if (!key.empty())
						{
							// val �ʿ� �߰�
							itSectionMap->second->insert(make_pair(key, val));

							//_tprintf(_T("\t%s=%s\n"), key.c_str(), val.c_str());
						}
					}
				}
			}
		}

		delete [] pSectionCtxBuf;
	}

	delete [] pSectionNamesBuf;

	return (int)m_pSectionMap->size();
}

// ���� ������ szFilePath ��η� ���� �ۼ��Ѵ�
// [in] szFilePath: ������ ���, NULL �� ��쿡�� ���µ� ��ο� ������
// [retVal] ���� ����
BOOL CIniFile::ReWrite(LPCTSTR szFilePath /*= NULL*/)
{
	BOOL retVal = FALSE;
	HANDLE hFile = CreateFile(szFilePath, GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile != INVALID_HANDLE_VALUE )
	{
		int nBufSize = 0;
		tstring context;
		context = MakeIniFileContext(context);

		DWORD dwWritten;
		LPCSTR pBuf = CT2CAEX<DEFAULT_FILE_SIZE>(context.c_str());
		retVal = WriteFile(hFile, pBuf, (DWORD)strlen(pBuf), &dwWritten, NULL);
		CloseHandle(hFile);
	}
	return retVal;
}

void CIniFile::Close()
{
	if (!m_bDestroySectionMap)
		return;

	for (MAP_INI_SECSTION_IT it = m_pSectionMap->begin(); it != m_pSectionMap->end(); ++it)
	{
		// ���ǳ��� key=value ���� ����
		delete it->second;
	}
	m_pSectionMap->clear();
}

// �ش� ���� �� Ű�� �ش��ϴ� ���� ��ȯ
// [retVal] ���� ���, tstring("") ��Ʈ���� ��ȯ
tstring CIniFile::GetStrVal(LPCTSTR szSectionName, LPCTSTR szKey)
{
	tstring retVal(_T(""));
	MAP_INI_SECSTION_CIT citSectionMap = m_pSectionMap->find(szSectionName);
	if (citSectionMap != m_pSectionMap->end())
	{
		MAP_INI_STRKEY_STRVAL* pValMap = citSectionMap->second;
		MAP_INI_STRKEY_STRVAL_CIT citValMap = pValMap->find(szKey);
		if (citValMap != pValMap->end())
			retVal = citValMap->second;
	}

	return retVal;
}

LPCTSTR CIniFile::GetVal(LPCTSTR szSectionName, LPCTSTR szKey)
{
	tstring retVal(_T(""));
	MAP_INI_SECSTION_CIT citSectionMap = m_pSectionMap->find(szSectionName);
	if (citSectionMap != m_pSectionMap->end())
	{
		MAP_INI_STRKEY_STRVAL* pValMap = citSectionMap->second;
		MAP_INI_STRKEY_STRVAL_CIT citValMap = pValMap->find(szKey);
		if (citValMap != pValMap->end())
			return citValMap->second.c_str();
	}

	return NULL;
}


tstring& CIniFile::MakeIniFileContext(tstring& out) const
{
	out.clear();

	MAP_INI_STRKEY_STRVAL* pValMap = NULL;
	_TCHAR szLine[1024];
	for (MAP_INI_SECSTION_CIT citSection = m_pSectionMap->begin(); citSection != m_pSectionMap->end(); ++citSection)
	{
		_stprintf(szLine, _T("[%s]\r\n"), citSection->first.c_str());
		out += szLine;
		
		pValMap = citSection->second;
		for (MAP_INI_STRKEY_STRVAL_CIT citKey = pValMap->begin(); citKey != pValMap->end(); ++citKey)
		{
			_stprintf(szLine, _T("%s=%s\r\n"), citKey->first.c_str(), citKey->second.c_str());
			out += szLine;
		}
		out += tstring(_T("\r\n"));
	}
	return out;
}

// "��ū1\0��ū1\0....\0��ūn\0\0"
// [�����̸�] ����Ʈ Ȥ�� Ű&��(key=val) ����Ʈ���� �Ѱ��� �������� ���� �� ��ȯ
// ��ū ����Ʈ ���ڿ����� ù ��ū�� out�� �����ϰ�, ���� ��ġ�� ��ȯ
// [in] szSrc: "��ū1\0��ū1\0....\0��ūn\0\0" �� ���� ���� ���ڿ�
// [out] outToken: ����� ��ū�̸�
// [retVal] NULL �� ���, ���� ��ū ����
LPTSTR CIniFile::GetToken(LPTSTR szSrc, LPTSTR outToken) const
{
	_tcscpy(outToken, _T(""));
	
	_TCHAR* pPos = szSrc;
	
	if (*pPos == '\0')
	{
		// ������ ���ڿ��� ���� ���
		return NULL;
	}

	UINT_PTR nOffSet = 0;
	while (1)
	{
		if (*pPos == '\0')
		{
			nOffSet = pPos - szSrc;
			_tcsncpy(outToken, szSrc, nOffSet);
			outToken[nOffSet] = '\0';
			return ++pPos;
		}
		pPos++;
	}
	return NULL;
}

// "key=val" �� ������ ���ڿ��� key �� val �����Ͽ� ��ȯ
void CIniFile::SpliceKeyVal(const tstring& src, tstring& key, tstring& val) const
{
	key.clear(); val.clear();
	
	tstring::size_type pPos; 

	pPos = src.find_first_of('=');
	if (pPos != tstring::npos)
	{
		key = src.substr(0, pPos);
		val = src.substr(pPos+1);
	}
	else
	{
		key = src;
	}
	key = Trim(key);
	val = Trim(val);
}

// str ���ڿ��� �յ� ���� ���� �� ��ȯ
tstring& CIniFile::Trim(tstring& str) const
{
	if (str.empty())
		return str;

	tstring::size_type sPos, ePos;

	if ((sPos = str.find_first_not_of(' ')) == tstring::npos)
		sPos = 0;

	if ((ePos = str.find_last_not_of(' ')) == tstring::npos)
		ePos = str.length();
	
	return (str = str.substr(sPos, ePos-sPos+1));
}

// ���Ǹ� ����
void CIniFile::SetSectionName(LPCTSTR szNewName, LPCTSTR szOldName)
{
	tstring oldName(szOldName), newName(szNewName);
	(*m_pSectionMap)[newName] = (*m_pSectionMap)[oldName];
	m_pSectionMap->erase(oldName);
}

// Ű�̸� ����
void CIniFile::SetKeyName(LPCTSTR szSectionName, LPCTSTR szNewKeyName, LPCTSTR szOldKeyName)
{
	MAP_INI_SECSTION_IT it = m_pSectionMap->find(szSectionName);
	if (it != m_pSectionMap->end())
	{
		MAP_INI_STRKEY_STRVAL& valMap = *(it->second);

		tstring oldName(szOldKeyName), newName(szNewKeyName);
		valMap[newName] = valMap[oldName];
		valMap.erase(oldName);
	}
}

// �� ����
void CIniFile::SetStrVal(LPCTSTR szSectionName, LPCTSTR szKey, LPCTSTR szNewVal)
{
	MAP_INI_SECSTION_IT it = m_pSectionMap->find(szSectionName);
	if (it != m_pSectionMap->end())
	{
		MAP_INI_STRKEY_STRVAL* pValMap = it->second;
		MAP_INI_STRKEY_STRVAL_IT itValMap = pValMap->find(szKey);
		
		if (itValMap != pValMap->end())
		{
			itValMap->second = szNewVal;
		}
	}
}

tstring CIniFile::GetIniKeyVal(LPCTSTR szFilePath, LPCTSTR szSectionName, LPCTSTR szKeyName)
{
	tstring retVal(_T(""));
	_TCHAR szKeyVal[MAX_LINE_SIZE];
	
	retVal = CIniFile::GetIniKeyVal(szFilePath, szSectionName, szKeyName, szKeyVal, MAX_LINE_SIZE);
	return retVal;
}

LPCTSTR CIniFile::GetIniKeyVal(LPCTSTR szFilePath, LPCTSTR szSectionName, LPCTSTR szKeyName, LPTSTR szKeyVal, DWORD dwKeyValBufSize)
{
	ZeroMemory(szKeyVal, sizeof(_TCHAR)*dwKeyValBufSize);
	DWORD dwChars = GetPrivateProfileString(szSectionName, szKeyName, NULL, szKeyVal, dwKeyValBufSize, szFilePath);
	return szKeyVal;
}

BOOL CIniFile::SetIniKeyVal(LPCTSTR szFilePath, LPCTSTR szSectionName, LPCTSTR szKeyName, LPCTSTR szKeyVal)
{
	return WritePrivateProfileString(szSectionName, szKeyName, szKeyVal, szFilePath);
}

// ���� ���� ��ȯ
int CIniFile::GetSectionSize()
{
	return (int)m_pSectionMap->size();
}

// ���ǳ� Ű ���� ��ȯ
int CIniFile::GetKeySize(LPCTSTR szSectionName)
{
	int retVal = 0;
	MAP_INI_SECSTION_CIT cit = m_pSectionMap->find(szSectionName);
	if (cit != m_pSectionMap->end())
		retVal = (int)(*cit).second->size();
	
	return retVal;
}