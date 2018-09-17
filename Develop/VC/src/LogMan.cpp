#include "StdAfx.h"
#include "LogMan.h"


CLogSingleInst* CLogSingleInst::m_pInstance = NULL;
CLogMultiInst* CLogMultiInst::m_pInstance = NULL;
HANDLE_LOGFILE CLogMultiInst::m_INVALID_HANDLE = _T("`:[j");

/////////////////////////////////////////////////////////////
//					CLogSingleMan
/////////////////////////////////////////////////////////////
BOOL CLogSingleInst::Init(LPCTSTR szDirName)
{
	TCHAR szCurrDir[MAX_PATH], szLogDirPath[MAX_PATH];
	CLogFile::GetCurrentDirPath(szCurrDir, _countof(szCurrDir));
	
	_stprintf_s(szLogDirPath, _countof(szLogDirPath), _T("%s\\%s"), szCurrDir, szDirName);
	CreateDirectory(szLogDirPath, NULL);

	return Open(szLogDirPath, _T("%02d%02d%02d.log"));
}

void CLogSingleInst::Uninit()
{
	Close();
}

/////////////////////////////////////////////////////////////
//					CLogMultiInst
/////////////////////////////////////////////////////////////
CLogMultiInst::CLogMultiInst(void)
{
	InitializeCriticalSectionAndSpinCount(&m_CsFileMap, 4000);
}

CLogMultiInst::~CLogMultiInst(void)
{
	DeleteCriticalSection(&m_CsFileMap);
}

void CLogMultiInst::Uninit()
{
	EnterCriticalSection(&m_CsFileMap);
	for (MAP_LOGFILE_IT it = m_FileMap.begin(); it != m_FileMap.end(); ++it)
	{
		delete (*it).second;
	}
	m_FileMap.clear();
	LeaveCriticalSection(&m_CsFileMap);
}

// [in] szLogName: 로그 작업 구분 문자열
// [in] szLogDirPath: 로그 폴더 전체 경로
// [in] szFNameFormat: 로그파일 형식으로 반드시 "%d%d%d"(년월일)를 
//                    포함하는 문자열이어야 한다. (ex: "report_%02d%02d%02d.log")
// [retval] HANDLE_LOGFILE: 로그 파일 핸들
HANDLE_LOGFILE CLogMultiInst::Open(LPCTSTR szLogName, LPCTSTR szLogDirPath, LPCTSTR szFNameFormat)
{
	HANDLE_LOGFILE hLogFile(szLogName);

	EnterCriticalSection(&m_CsFileMap);
	
	MAP_LOGFILE_IT it = m_FileMap.find(hLogFile);
	if (it == m_FileMap.end())
	{
		// 신규 파일
		CLogFile* pLog = new CLogFile(TRUE, TRUE);
		if (pLog->Open(szLogDirPath, szFNameFormat))
		{
			m_FileMap.insert(make_pair(hLogFile, pLog));
		}
		else
		{
			delete pLog;
			hLogFile = m_INVALID_HANDLE;
		}
	}

	LeaveCriticalSection(&m_CsFileMap);
	
	return hLogFile;
}

void CLogMultiInst::Close(HANDLE_LOGFILE hLogFile)
{
	EnterCriticalSection(&m_CsFileMap);

	MAP_LOGFILE_IT it = m_FileMap.find(hLogFile);
	if (it != m_FileMap.end())
	{
		delete (*it).second;
		m_FileMap.erase(it);
	}

	LeaveCriticalSection(&m_CsFileMap);
}

CLogFile* CLogMultiInst::GetLogFile(const HANDLE_LOGFILE& hLogFile)
{
	CLogFile* retVal = NULL;
	
	EnterCriticalSection(&m_CsFileMap);

	MAP_LOGFILE_IT it = m_FileMap.find(hLogFile);
	if (it != m_FileMap.end())
	{
		retVal = (*it).second;
	}

	LeaveCriticalSection(&m_CsFileMap);

	return retVal;
}

void CLogMultiInst::AddLog(HANDLE_LOGFILE hLogFile, const TCHAR* pFormat, ...)
{
	CLogFile* pLog = GetLogFile(hLogFile);
	if (!pLog)
		return;

	va_list	pArg;
	va_start(pArg, pFormat);
	pLog->AddLog(false, pFormat, pArg);
	va_end(pArg);
}

void CLogMultiInst::AddLogLine(HANDLE_LOGFILE hLogFile, const TCHAR* pFormat, ...)
{
	CLogFile* pLog = GetLogFile(hLogFile);
	if (!pLog)
		return;

	va_list	pArg;
	va_start(pArg, pFormat);
	pLog->AddLog(true, pFormat, pArg);
	va_end(pArg);
}

void CLogMultiInst::AddLogA(HANDLE_LOGFILE hLogFile, const char* pFormat, ...) 
{
	CLogFile* pLog = GetLogFile(hLogFile);
	if (!pLog)
		return;

	va_list	pArg;
	va_start(pArg, pFormat);
	pLog->AddLogA(false, pFormat, pArg);
	va_end(pArg);
}

void CLogMultiInst::AddLogLineA(HANDLE_LOGFILE hLogFile, const char* pFormat, ...)
{
	CLogFile* pLog = GetLogFile(hLogFile);
	if (!pLog)
		return;

	va_list	pArg;
	va_start(pArg, pFormat);
	pLog->AddLogA(true, pFormat, pArg);
	va_end(pArg);
}

void CLogMultiInst::AddLogS(HANDLE_LOGFILE hLogFile, const TCHAR* pMsg)
{
	CLogFile* pLog = GetLogFile(hLogFile);
	if (!pLog)
		return;

	pLog->AddLogS(false, pMsg);
}

void CLogMultiInst::AddLogLineS(HANDLE_LOGFILE hLogFile, const TCHAR* pMsg)
{
	CLogFile* pLog = GetLogFile(hLogFile);
	if (!pLog)
		return;

	pLog->AddLogS(true, pMsg);
}

void CLogMultiInst::AddLogSA(HANDLE_LOGFILE hLogFile, const char* pMsg)
{
	CLogFile* pLog = GetLogFile(hLogFile);
	if (!pLog)
		return;

	pLog->AddLogSA(false, pMsg);
}

void CLogMultiInst::AddLogLineSA(HANDLE_LOGFILE hLogFile, const char* pMsg)
{
	CLogFile* pLog = GetLogFile(hLogFile);
	if (!pLog)
		return;

	pLog->AddLogSA(true, pMsg);
}