#include "stdafx.h"
#include <share.h>		// _SH_DENYNO
#include "LogFile.h"
#include <AtlBase.h>

//////////////////////////////////////////////////////////////////////
//
//	CLogFile Class
//

// �α� ������ �����Ѵ�.
// [in] FilePath: �α� ������ ����� ��θ�, ���������� �ڵ����� ���� (ex: "C:\\app\\log")
// [in] sFormat: �α����� �������� �ݵ�� "%d%d%d"(�����)�� 
//                    �����ϴ� ���ڿ��̾�� �Ѵ�. (ex: "report_%d%d%d.log")
// [retval] bool: ���������� �����ϸ� true, �����ϸ� false�� ��ȯ�Ѵ�.
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

// �α� ������ �ݴ´�.
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
	// Ansi ��ȯ�� �α� ó��

	int nLen = _vscwprintf(pFormat, pArg);
	if (nLen > MAX_VA_STACK_LOG_LEN)
	{
		// �� ���� ���
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

	// ���Ͽ� ���
	char TimeStr[30];
	sprintf_s(TimeStr, _countof(TimeStr), "%02d:%02d:%02d.%03d ", st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);

	if (_pCs) EnterCriticalSection(_pCs);
	if (st.wDay != _St.wDay || _bChange) {
		// ��¥�� �ٸ��ų� ��ΰ� ����Ǿ��� ��� ���ο� �α� ������ �����Ѵ�.
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

	// ���Ͽ� ���
	char TimeStr[30];
	sprintf_s(TimeStr, _countof(TimeStr), "%02d:%02d:%02d.%03d ", st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);

	if (_pCs) EnterCriticalSection(_pCs);
	if (st.wDay != _St.wDay || _bChange) {
		// ��¥�� �ٸ��ų� ��ΰ� ����Ǿ��� ��� ���ο� �α� ������ �����Ѵ�.
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

// �Էµ� ��¥�� �ش�Ǵ� ���ο� �α� ������ �����Ѵ�.
inline bool CLogFile::Create(LPCTSTR FilePath, SYSTEMTIME& st)
{
	_TCHAR LogFile[_MAX_PATH], sFormat[64] = _T("%s");
	
	_tcscat_s(sFormat, _countof(sFormat), _Format); 
	_stprintf_s(LogFile, _countof(LogFile), sFormat, FilePath, st.wYear, st.wMonth, st.wDay);

	return (_fp = _tfsopen(LogFile, _T("a+"), _SH_DENYNO)) != NULL;
}

// �α� ���� ���� ��� �� ������ ����
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