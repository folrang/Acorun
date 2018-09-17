#include "stdafx.h"
#include <share.h>		// _SH_DENYNO
#include "LogFile.h"
#include <AtlBase.h>

//////////////////////////////////////////////////////////////////////
//
//	CLogFile Class
//

// 로그 파일을 오픈한다.
// [in] FilePath: 로그 파일이 저장될 경로명, 서브폴더는 자동생성 않함 (ex: "C:\\app\\log")
// [in] sFormat: 로그파일 형식으로 반드시 "%d%d%d"(년월일)를 
//                    포함하는 문자열이어야 한다. (ex: "report_%d%d%d.log")
// [retval] bool: 성공적으로 오픈하면 true, 실패하면 false를 반환한다.
bool CLogFile::Open(const _TCHAR* FilePath, const _TCHAR* sFormat)
{
#ifdef LOGFILE_H_OFF
	return true;
#endif;

	if (!_fp) {
		GetLocalTime(&_St);
		_tcscpy_s(_FilePath, _countof(_FilePath), FilePath);
		_tcscpy_s(_Format, _countof(_Format), sFormat);
		int len = (int)_tcslen(_FilePath);
		if (_FilePath[len-1] != '\\') {
			_FilePath[len] = '\\';
			_FilePath[len+1] = 0;
		}
		return Create(_FilePath, _St);
	}	
	return FALSE;
}

// 로그 파일을 닫는다.
void CLogFile::Close()
{
	if (_fp) {
		fclose(_fp);
		_fp = NULL;
	}
}

void CLogFile::AddLog(const _TCHAR* pFormat, ...)
{
	va_list	pArg;
	va_start(pArg, pFormat);
	AddLog(false, pFormat, pArg);
	va_end(pArg);
}

void CLogFile::AddLogLine(const _TCHAR* pFormat, ...)
{
	va_list	pArg;
	va_start(pArg, pFormat);
	AddLog(true, pFormat, pArg);
	va_end(pArg);
}

void CLogFile::AddLogA(const char* pFormat, ...)
{
	va_list	pArg;
	va_start(pArg, pFormat);
	AddLogA(false, pFormat, pArg);
	va_end(pArg);
}

void CLogFile::AddLogLineA(const char* pFormat, ...)
{
	va_list	pArg;
	va_start(pArg, pFormat);
	AddLogA(true, pFormat, pArg);
	va_end(pArg);
}

void CLogFile::AddLogS(const _TCHAR* pMsg)
{
	AddLogS(false, pMsg);
}

void CLogFile::AddLogLineS(const _TCHAR* pMsg)
{
	AddLogS(true, pMsg);
}

void CLogFile::AddLogSA(const char* pMsg)
{
	AddLogSA(false, pMsg);
}

void CLogFile::AddLogLineSA(const char* pMsg)
{
	AddLogSA(true, pMsg);
}

void CLogFile::AddLog(bool bAddCrLf, const _TCHAR* pFormat, va_list pArg)
{
#ifdef _UNICODE
	// Ansi 변환후 로깅 처리

	int nLen = _vscwprintf(pFormat, pArg);
	if (nLen > MAX_VA_STACK_LOG_LEN)
	{
		// 힙 버퍼 사용
		WCHAR* pBufW = (WCHAR*)malloc((nLen + 1) * sizeof(WCHAR));
		nLen = vswprintf_s(pBufW, nLen + 1, pFormat, pArg);

		CW2AEX<(MAX_VA_STACK_LOG_LEN + 1) * 2> BufA(pBufW);
		AddLogSA(bAddCrLf, (LPCSTR)BufA);

		free(pBufW);
	}
	else
	{
		WCHAR* pBufW = (WCHAR*)_alloca((nLen + 1) * sizeof(WCHAR));
		nLen = vswprintf_s(pBufW, nLen + 1, pFormat, pArg);

		CW2AEX<(MAX_VA_STACK_LOG_LEN + 1)> BufA(pBufW);
		AddLogSA(bAddCrLf, (LPCSTR)BufA);
	}

#else

	AddLogA(bAddCrLf, pFormat, pArg);

#endif
}

void CLogFile::AddLogS(bool bAddCrLf, const _TCHAR* pMsg)
{
#ifdef _UNICODE
	CT2AEX<1024> pBufA(pMsg);
	AddLogSA(bAddCrLf, (LPCSTR)pBufA);
#else
	AddLogSA(bAddCrLf, pMsg);
#endif
}

void CLogFile::AddLogA(bool bAddCrLf, const char* pFormat, va_list pArg)
{
	if (!_fp) return;	

	SYSTEMTIME st;
	GetLocalTime(&st);

	// 파일에 기록
	char TimeStr[30];
	sprintf_s(TimeStr, _countof(TimeStr), "%02d:%02d:%02d.%03d ", st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);

	if (_pCs) EnterCriticalSection(_pCs);
	if (st.wDay != _St.wDay || _bChange) {
		// 날짜가 다르거나 경로가 변경되었을 경우 새로운 로그 파일을 생성한다.
		fclose(_fp);
		_bChange = false;
		_St = st;
		if (!Create(_FilePath, _St)) {
			if (_pCs) LeaveCriticalSection(_pCs);
			return;
		}
	}

	fputs(TimeStr, _fp);
	vfprintf(_fp, pFormat, pArg);
	if (bAddCrLf)
		fputs("\r\n", _fp);

	if (_bDefaultFlush) Flush();
	if (_pCs) LeaveCriticalSection(_pCs);
}


void CLogFile::AddLogSA(bool bAddCrLf, const char* pMsg)
{
	if (!_fp) return;	

	SYSTEMTIME st;
	GetLocalTime(&st);

	// 파일에 기록
	char TimeStr[30];
	sprintf_s(TimeStr, _countof(TimeStr), "%02d:%02d:%02d.%03d ", st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);

	if (_pCs) EnterCriticalSection(_pCs);
	if (st.wDay != _St.wDay || _bChange) {
		// 날짜가 다르거나 경로가 변경되었을 경우 새로운 로그 파일을 생성한다.
		fclose(_fp);
		_bChange = false;
		_St = st;
		if (!Create(_FilePath, _St)) {
			if (_pCs) LeaveCriticalSection(_pCs);
			return;
		}
	}

	fputs(TimeStr, _fp);
	fputs(pMsg, _fp);
	if (bAddCrLf)
		fputs("\r\n", _fp);

	if (_bDefaultFlush) Flush();
	if (_pCs) LeaveCriticalSection(_pCs);
}

// 입력된 날짜에 해당되는 새로운 로그 파일을 생성한다.
inline bool CLogFile::Create(LPCTSTR FilePath, SYSTEMTIME& st)
{
	_TCHAR LogFile[_MAX_PATH], sFormat[64] = _T("%s");
	
	_tcscat_s(sFormat, _countof(sFormat), _Format); 
	_stprintf_s(LogFile, _countof(LogFile), sFormat, FilePath, st.wYear, st.wMonth, st.wDay);

	return (_fp = _tfsopen(LogFile, _T("a+"), _SH_DENYNO)) != NULL;
}

// 로그 파일 저장 경로 및 포맷을 변경
void CLogFile::ChangePath(const _TCHAR* FilePath, const _TCHAR* sFormat)
{
	_tcscpy_s(_FilePath, _countof(_FilePath), FilePath);
	int len = (int)_tcslen(_FilePath);
	if (_FilePath[len-1] != '\\') {
		_FilePath[len] = '\\';
		_FilePath[len+1] = 0;
	}
	if (sFormat[0] != '\0') _tcscpy_s(_Format, _countof(_Format), sFormat);
	_bChange = true;
}

LPCTSTR CLogFile::GetCurrentDirPath(LPTSTR szPath, DWORD dwPathBufSize)
{
	GetModuleFileName(NULL, szPath, dwPathBufSize);
	TCHAR* pEnd = _tcsrchr(szPath, '\\');
	*pEnd = '\0';
	return szPath;
}