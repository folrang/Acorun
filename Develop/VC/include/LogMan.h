//						LogMan.h
//			로그기록 클래스
//
//		최종 수정일: 2012/10/30
//		작성자: dede
//
////////////////////////////////////////////////////////////////////////////////////////////////////
#include "LogFile.h"
#include <string>
#include <map>
using namespace std;

#pragma once


namespace LogMan
{
	typedef basic_string<TCHAR, char_traits<TCHAR>, allocator<TCHAR> > tstring;
	typedef tstring	HANDLE_LOGFILE;
};

using namespace LogMan;


/////////////////////////////////////////////////////////////
//				CLogSingleInst
/////////////////////////////////////////////////////////////
class CLogSingleInst : public CLogFile
{
public:
	static CLogSingleInst* Instance() { return m_pInstance ? m_pInstance : m_pInstance = new CLogSingleInst(); }
	~CLogSingleInst(void) {}

	// [in] szDirName: 로그 폴더명
	BOOL Init(LPCTSTR szDirName = _T("Log"));
	void Uninit();
	
protected:
	CLogSingleInst(void) : CLogFile(TRUE, TRUE) {}
	
	bool Open(const _TCHAR* FilePath, const _TCHAR* sFormat)
	{
		return __super::Open(FilePath, sFormat);
	}

	// 로그 파일을 닫는다.
	void Close()
	{
		__super::Close();
	}

	static CLogSingleInst* m_pInstance;
};


/////////////////////////////////////////////////////////////
//				CLogMultiInst
/////////////////////////////////////////////////////////////
class CLogMultiInst
{
public:		
	typedef map<HANDLE_LOGFILE, CLogFile*>					MAP_LOGFILE;
	typedef map<HANDLE_LOGFILE, CLogFile*>::const_iterator	MAP_LOGFILE_CIT;
	typedef map<HANDLE_LOGFILE, CLogFile*>::iterator		MAP_LOGFILE_IT;
	typedef pair<HANDLE_LOGFILE, CLogFile*>					MAP_LOGFILE_ITEM;

	static CLogMultiInst* Instance() { return m_pInstance ? m_pInstance : m_pInstance = new CLogMultiInst(); }
	~CLogMultiInst(void);

	BOOL Init() { return TRUE; }
	void Uninit();
	
	// [in] szLogName: 로그 작업 구분 문자열
	// [in] szLogDirPath: 로그 폴더 전체 경로
	// [in] szFNameFormat: 로그파일 형식으로 반드시 "%d%d%d"(년월일)를 
	//                    포함하는 문자열이어야 한다. (ex: "report_%02d%02d%02d.log")
	// [retval] HANDLE_LOGFILE: 로그 파일 핸들
	//				성공여부: IsValidHandle() 로 확인
	HANDLE_LOGFILE Open(LPCTSTR szLogName, LPCTSTR szLogDirPath, LPCTSTR szFNameFormat = _T("%02d%02d%02d.log"));
	void Close(HANDLE_LOGFILE hLogFile);
	
	BOOL IsValidHandle(const HANDLE_LOGFILE& hLogFile) const { return (hLogFile != m_INVALID_HANDLE); }

	///////////////////////////////
	//		로깅 함수들
	//

	void AddLog(HANDLE_LOGFILE hLogFile, const TCHAR* pFormat, ...);
	void AddLogLine(HANDLE_LOGFILE hLogFile, const TCHAR* pFormat, ...);
	void AddLogA(HANDLE_LOGFILE hLogFile, const char* pFormat, ...);
	void AddLogLineA(HANDLE_LOGFILE hLogFile, const char* pFormat, ...);
	
	void AddLogS(HANDLE_LOGFILE hLogFile, const TCHAR* pMsg);
	void AddLogLineS(HANDLE_LOGFILE hLogFile, const TCHAR* pMsg);
	void AddLogSA(HANDLE_LOGFILE hLogFile, const char* pMsg);
	void AddLogLineSA(HANDLE_LOGFILE hLogFile, const char* pMsg);

protected:
	CLogMultiInst(void);
	CLogFile* GetLogFile(const HANDLE_LOGFILE& hLogFile);
	
	static CLogMultiInst* m_pInstance;
	
	static tstring m_INVALID_HANDLE;

	TCHAR m_szLogRootDir[MAX_PATH];
	MAP_LOGFILE m_FileMap;
	CRITICAL_SECTION m_CsFileMap;
};


/*
////////////////////////////////////
//   CLogSingleInst
////////////////////////////////////
#include "LogMan.h"
typedef CLogSingleInst CLogMan;

int _tmain(int argc, _TCHAR* argv[])
{
	CLogMan* pLogMan = CLogMan::Instance();
	if (pLogMan->Init())
	{
		pLogMan->AddLog(_T("AddLog, 가%d나%.2f다%c라\r\n"), 1, 2.2, 'c');
		pLogMan->AddLogA("AddLogA, 가%d나%.2f다%c라\r\n", 1, 2.2, 'c');

		pLogMan->AddLogS(_T("AddLogS, 가나다다\r\n"));
		pLogMan->AddLogSA("AddLogSA, 가나다다\r\n");

		pLogMan->AddLogLine(_T("AddLogLine, 가%d나%.2f다%c라"), 1, 2.2, 'c');
		pLogMan->AddLogLineA("AddLogLineA, 가%d나%.2f다%c라", 1, 2.2, 'c');

		pLogMan->AddLogLineS(_T("AddLogLineS, 가나다다"));
		pLogMan->AddLogLineSA("AddLogLineSA, 가나다다");
	}
	pLogMan->Uninit();
	delete pLogMan;

	return 0;
}
*/

/*
////////////////////////////////////
//   CLogMultiInst
////////////////////////////////////
#include "LogMan.h"
typedef CLogMultiInst CLogMan;

int _tmain(int argc, _TCHAR* argv[])
{
	CLogMan* pLogMan = CLogMan::Instance();
	pLogMan->Init();

	HANDLE_LOGFILE hLogFile;
	hLogFile = pLogMan->Open(_T("로직A 로그"), _T("D:\\Develop\\zTest\\LogMan_tst\\debug\\log"));
	if (pLogMan->IsValidHandle(hLogFile))
	{
		pLogMan->AddLog(hLogFile, _T("AddLog, 가%d나%.2f다%c라\r\n"), 1, 2.2, 'c');
		pLogMan->AddLogA(hLogFile, "AddLogA, 가%d나%.2f다%c라\r\n", 1, 2.2, 'c');

		pLogMan->AddLogS(hLogFile, _T("AddLogS, 가나다다\r\n"));
		pLogMan->AddLogSA(hLogFile, "AddLogSA, 가나다다\r\n");

		pLogMan->AddLogLine(hLogFile, _T("AddLogLine, 가%d나%.2f다%c라"), 1, 2.2, 'c');
		pLogMan->AddLogLineA(hLogFile, "AddLogLineA, 가%d나%.2f다%c라", 1, 2.2, 'c');

		pLogMan->AddLogLineS(hLogFile, _T("AddLogLineS, 가나다다"));
		pLogMan->AddLogLineSA(hLogFile, "AddLogLineSA, 가나다다");

		pLogMan->Close(hLogFile);
	}

	pLogMan->Uninit();
	delete pLogMan;

	return 0;
}
*/