//						LogMan.h
//			�αױ�� Ŭ����
//
//		���� ������: 2012/10/30
//		�ۼ���: dede
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

	// [in] szDirName: �α� ������
	BOOL Init(LPCTSTR szDirName = _T("Log"));
	void Uninit();
	
protected:
	CLogSingleInst(void) : CLogFile(TRUE, TRUE) {}
	
	bool Open(const _TCHAR* FilePath, const _TCHAR* sFormat)
	{
		return __super::Open(FilePath, sFormat);
	}

	// �α� ������ �ݴ´�.
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
	
	// [in] szLogName: �α� �۾� ���� ���ڿ�
	// [in] szLogDirPath: �α� ���� ��ü ���
	// [in] szFNameFormat: �α����� �������� �ݵ�� "%d%d%d"(�����)�� 
	//                    �����ϴ� ���ڿ��̾�� �Ѵ�. (ex: "report_%02d%02d%02d.log")
	// [retval] HANDLE_LOGFILE: �α� ���� �ڵ�
	//				��������: IsValidHandle() �� Ȯ��
	HANDLE_LOGFILE Open(LPCTSTR szLogName, LPCTSTR szLogDirPath, LPCTSTR szFNameFormat = _T("%02d%02d%02d.log"));
	void Close(HANDLE_LOGFILE hLogFile);
	
	BOOL IsValidHandle(const HANDLE_LOGFILE& hLogFile) const { return (hLogFile != m_INVALID_HANDLE); }

	///////////////////////////////
	//		�α� �Լ���
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
		pLogMan->AddLog(_T("AddLog, ��%d��%.2f��%c��\r\n"), 1, 2.2, 'c');
		pLogMan->AddLogA("AddLogA, ��%d��%.2f��%c��\r\n", 1, 2.2, 'c');

		pLogMan->AddLogS(_T("AddLogS, �����ٴ�\r\n"));
		pLogMan->AddLogSA("AddLogSA, �����ٴ�\r\n");

		pLogMan->AddLogLine(_T("AddLogLine, ��%d��%.2f��%c��"), 1, 2.2, 'c');
		pLogMan->AddLogLineA("AddLogLineA, ��%d��%.2f��%c��", 1, 2.2, 'c');

		pLogMan->AddLogLineS(_T("AddLogLineS, �����ٴ�"));
		pLogMan->AddLogLineSA("AddLogLineSA, �����ٴ�");
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
	hLogFile = pLogMan->Open(_T("����A �α�"), _T("D:\\Develop\\zTest\\LogMan_tst\\debug\\log"));
	if (pLogMan->IsValidHandle(hLogFile))
	{
		pLogMan->AddLog(hLogFile, _T("AddLog, ��%d��%.2f��%c��\r\n"), 1, 2.2, 'c');
		pLogMan->AddLogA(hLogFile, "AddLogA, ��%d��%.2f��%c��\r\n", 1, 2.2, 'c');

		pLogMan->AddLogS(hLogFile, _T("AddLogS, �����ٴ�\r\n"));
		pLogMan->AddLogSA(hLogFile, "AddLogSA, �����ٴ�\r\n");

		pLogMan->AddLogLine(hLogFile, _T("AddLogLine, ��%d��%.2f��%c��"), 1, 2.2, 'c');
		pLogMan->AddLogLineA(hLogFile, "AddLogLineA, ��%d��%.2f��%c��", 1, 2.2, 'c');

		pLogMan->AddLogLineS(hLogFile, _T("AddLogLineS, �����ٴ�"));
		pLogMan->AddLogLineSA(hLogFile, "AddLogLineSA, �����ٴ�");

		pLogMan->Close(hLogFile);
	}

	pLogMan->Uninit();
	delete pLogMan;

	return 0;
}
*/