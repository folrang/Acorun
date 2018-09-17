#include "StdAfx.h"
#include "IniFile.h"
#include <AtlBase.h> 	// CX2X

CIniFile::CIniFile()
{
	_tcscpy(m_szPath, _T(""));
	m_pSectionMap = new MAP_INI_SECSTION();
	m_bDestroySectionMap = TRUE;
}

// [in] pSectionMapPtr: new 생성된 객체
// [in] bDestroySectionMap: pSectionMap를 Clear 및 Destroy 시킬지 여부
//			FALSE: Close() 및 pSectionMap 맵을 Clear() 작업이 내부에서 발생되지 않으며, 
//				소멸자 함수에서도 pSectionMap 객체를 삭제하지 않음
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

	// ini 파일내의 세션명들 가져옴
	// pBuf: "세션명1\0세션명2\0....\0세션명n\0\0"
	dwRetChars = GetPrivateProfileSectionNames(pSectionNamesBuf, CIniFile::MAX_SECTIONNAMES_SIZE, m_szPath);

	if (dwRetChars)
	{
		tstring sectionName;
		MAP_INI_STRKEY_STRVAL* pValMap = NULL;
		_TCHAR szSectionName[256];
		_TCHAR* pPos;
		
		pPos = pSectionNamesBuf;

		// 세션명 리스트 파싱
		while (pPos = GetToken(pPos, szSectionName))
		{
			if (*szSectionName)
			{
				sectionName = szSectionName;
				if (m_pSectionMap->find(sectionName) == m_pSectionMap->end())
				{
					// 세션명과 빈 key:val 맵을 맵에 추가
					pValMap = new MAP_INI_STRKEY_STRVAL;
					m_pSectionMap->insert(make_pair(sectionName, pValMap));
				}
			}
		}
		
		// 한 세션 내의 내용을 추출하여 파싱 및 key맵에 추가
		_TCHAR* pSectionCtxBuf = new _TCHAR [CIniFile::MAX_SECTION_CONTENT_SIZE];
		
		for (MAP_INI_SECSTION_IT itSectionMap = m_pSectionMap->begin(); itSectionMap != m_pSectionMap->end(); ++itSectionMap)
		{
			
			//_tprintf(_T("\n[%s]\n"), itSectionMap->first.c_str());

			// 세션내의 내용 얻오오기
			dwRetChars = GetPrivateProfileSection(itSectionMap->first.c_str(), pSectionCtxBuf, CIniFile::MAX_SECTION_CONTENT_SIZE, m_szPath);

			if (dwRetChars)
			{
				_TCHAR szLine[CIniFile::MAX_LINE_SIZE];
				tstring key, val;

				pPos = pSectionCtxBuf;
				
				// 세션내의 한 라인(key=val) 얻오오기
				while (pPos = GetToken(pPos, szLine))
				{
					if (*szLine)
					{
						// key 와 val 분리
						SpliceKeyVal(szLine, key, val);

						if (!key.empty())
						{
							// val 맵에 추가
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

// 현재 내용을 szFilePath 경로로 새로 작성한다
// [in] szFilePath: 저장할 경로, NULL 인 경우에는 오픈된 경로에 재저장
// [retVal] 성공 여부
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
		// 세션내의 key=value 맵을 삭제
		delete it->second;
	}
	m_pSectionMap->clear();
}

// 해당 세션 및 키에 해당하는 값을 반환
// [retVal] 없는 경우, tstring("") 스트링을 반환
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

// "토큰1\0토큰1\0....\0토큰n\0\0"
// [세션이름] 리스트 혹은 키&값(key=val) 리스트에서 한개의 아이템을 추출 및 반환
// 토큰 리스트 문자열에서 첫 토큰을 out에 복사하고, 다음 위치를 반환
// [in] szSrc: "토큰1\0토큰1\0....\0토큰n\0\0" 와 같은 포맷 문자열
// [out] outToken: 추출된 토큰이름
// [retVal] NULL 일 경우, 다음 토큰 없음
LPTSTR CIniFile::GetToken(LPTSTR szSrc, LPTSTR outToken) const
{
	_tcscpy(outToken, _T(""));
	
	_TCHAR* pPos = szSrc;
	
	if (*pPos == '\0')
	{
		// 아이템 문자열의 끝인 경우
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

// "key=val" 로 구성된 문자열을 key 와 val 추출하여 반환
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

// str 문자열의 앞뒤 공백 제거 후 반환
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

// 세션명 변경
void CIniFile::SetSectionName(LPCTSTR szNewName, LPCTSTR szOldName)
{
	tstring oldName(szOldName), newName(szNewName);
	(*m_pSectionMap)[newName] = (*m_pSectionMap)[oldName];
	m_pSectionMap->erase(oldName);
}

// 키이름 변경
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

// 값 변경
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

// 섹션 개수 반환
int CIniFile::GetSectionSize()
{
	return (int)m_pSectionMap->size();
}

// 섹션내 키 개수 반환
int CIniFile::GetKeySize(LPCTSTR szSectionName)
{
	int retVal = 0;
	MAP_INI_SECSTION_CIT cit = m_pSectionMap->find(szSectionName);
	if (cit != m_pSectionMap->end())
		retVal = (int)(*cit).second->size();
	
	return retVal;
}