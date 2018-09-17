/**
 * @file		LogFile.h		
 * @version		1.4
 * @date		2012/10/30
 * @author		Dede
 * @warning		작성자(dede)의 동의없이 해당 소스를 사용, 변경하는 것을 금합니다
 * @brief		파일을 이용한 로그 기록
 * @par History   
				2012/10/29
				- AddLogX() 함수 로직들 수정

				- LOGFILE_H_OFF 지원
**/

#if !defined(AFX_LOGFILE_H__B9E4C625_AF6A_471F_95B9_D19B1D8CF5EF__INCLUDED_)
#define AFX_LOGFILE_H__B9E4C625_AF6A_471F_95B9_D19B1D8CF5EF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CLogMultiInst;

//////////////////////////////////////////////////////////////////////
//
// CLogFile Class
//
//
class CLogFile  
{
	friend class CLogMultiInst;

public:	
	// [in] bFlushDefault: AddLog후 자동 Flush 여부
	// [in] bThreadSafe: 동기화 지원 여부, TRUE 기본
	CLogFile(bool bFlushDefault = TRUE, bool bThreadSafe= TRUE) : _fp(NULL), _bChange(false), _pCs(NULL) {
		_bDefaultFlush = bFlushDefault;

		if (bThreadSafe) 
		{
			_pCs = new CRITICAL_SECTION();
			InitializeCriticalSectionAndSpinCount(_pCs, 4000);
		}
	}
	virtual ~CLogFile() {
		if (_pCs) 
		{
			DeleteCriticalSection(_pCs);
			delete _pCs;
		}
	}

	// 로그 파일을 오픈한다.
	// [in] FilePath: 로그 파일이 저장될 경로명, 서브폴더는 자동생성 않함 (ex: "C:\\app\\log")
	// [in] sFormat: 로그파일 형식으로 반드시 "%d%d%d"(년월일)를 
	//                    포함하는 문자열이어야 한다. (ex: "report_%02d%02d%02d.log")
	// [retval] bool: 성공적으로 오픈하면 true, 실패하면 false를 반환한다.
	bool Open(const _TCHAR* FilePath, const _TCHAR* sFormat);
	
	// 로그 파일을 닫는다.
	void Close();

	///////////////////////////////
	//		로깅 함수들
	//

	void AddLog(const _TCHAR* pFormat, ...);
	void AddLogLine(const _TCHAR* pFormat, ...);
	void AddLogA(const char* pFormat, ...);
	void AddLogLineA(const char* pFormat, ...);

	void AddLogS(const _TCHAR* pMsg);
	void AddLogLineS(const _TCHAR* pMsg);
	void AddLogSA(const char* pMsg);
	void AddLogLineSA(const char* pMsg);
		
	// 로그 파일을 플러시 한다.
	void Flush() { fflush(_fp); }


	// 로그 파일 저장 경로 및 포맷을 변경
	void ChangePath(const _TCHAR* FilePath, const _TCHAR* sFormat = _T(""));
	
	// 현재 폴더 경로 반환
	static LPCTSTR GetCurrentDirPath(LPTSTR szPath, DWORD dwPathBufSize);

protected:
	enum 
	{ 
		// 가변 인자형 파일 쓰기 버퍼를 위한 스택 최대 길이
		//  그 이상이면 힙을 사용
		MAX_VA_STACK_LOG_LEN = 1023 
	}; 
	
	// TCHAR Variable-argument 를 직접 받아서 실제 처리 함수
	// [in] bAddCrLf: 로그 끝에 "\r\n" 자동 추가 여부
	void AddLog(bool bAddCrLf, const _TCHAR* pFormat, va_list pArg);
	void AddLogA(bool bAddCrLf, const char* pFormat, va_list pArg);

	// [in] bAddCrLf: 로그 끝에 "\r\n" 자동 추가 여부
	void AddLogS(bool bAddCrLf, const _TCHAR* pMsg);
	void AddLogSA(bool bAddCrLf, const char* pMsg);


	// 입력된 날짜에 해당되는 새로운 로그 파일을 생성한다.
	bool Create(LPCTSTR FilePath, SYSTEMTIME& st);

private:	
	FILE*      _fp;
	SYSTEMTIME _St;
	_TCHAR     _FilePath[MAX_PATH];
	_TCHAR     _Format[32];
	bool	   _bChange;
	bool	   _bDefaultFlush;		
	CRITICAL_SECTION* _pCs;
};

#endif // !defined(AFX_LOGFILE_H__B9E4C625_AF6A_471F_95B9_D19B1D8CF5EF__INCLUDED_)

/*
#include "LogFile.h"
int _tmain(int argc, _TCHAR* argv[])
{
	CLogFile Log;
	if (Log.Open(_T("D:\\log"), _T("%d%d%d.log")))
	{
		Log.AddLog(_T("AddLog, 가%d나%f다%c라\r\n"), 1, 2.2, 'c');
		Log.AddLogA("AddLogA, 가%d나%f다%c라\r\n", 1, 2.2, 'c');

		Log.AddLogS(_T("AddLogS, 가나다다\r\n"));
		Log.AddLogSA("AddLogSA, 가나다다\r\n");

		Log.AddLogLine(_T("AddLogLine, 가%d나%f다%c라"), 1, 2.2, 'c');
		Log.AddLogLineA("AddLogLineA, 가%d나%f다%c라", 1, 2.2, 'c');

		Log.AddLogLineS(_T("AddLogLineS, 가나다다"));
		Log.AddLogLineSA("AddLogLineSA, 가나다다");

		Log.Close();
	}

	return 0;
}
*/
