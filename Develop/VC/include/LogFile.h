/**
 * @file		LogFile.h		
 * @version		1.4
 * @date		2012/10/30
 * @author		Dede
 * @warning		�ۼ���(dede)�� ���Ǿ��� �ش� �ҽ��� ���, �����ϴ� ���� ���մϴ�
 * @brief		������ �̿��� �α� ���
 * @par History   
				2012/10/29
				- AddLogX() �Լ� ������ ����

				- LOGFILE_H_OFF ����
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
	// [in] bFlushDefault: AddLog�� �ڵ� Flush ����
	// [in] bThreadSafe: ����ȭ ���� ����, TRUE �⺻
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

	// �α� ������ �����Ѵ�.
	// [in] FilePath: �α� ������ ����� ��θ�, ���������� �ڵ����� ���� (ex: "C:\\app\\log")
	// [in] sFormat: �α����� �������� �ݵ�� "%d%d%d"(�����)�� 
	//                    �����ϴ� ���ڿ��̾�� �Ѵ�. (ex: "report_%02d%02d%02d.log")
	// [retval] bool: ���������� �����ϸ� true, �����ϸ� false�� ��ȯ�Ѵ�.
	bool Open(const _TCHAR* FilePath, const _TCHAR* sFormat);
	
	// �α� ������ �ݴ´�.
	void Close();

	///////////////////////////////
	//		�α� �Լ���
	//

	void AddLog(const _TCHAR* pFormat, ...);
	void AddLogLine(const _TCHAR* pFormat, ...);
	void AddLogA(const char* pFormat, ...);
	void AddLogLineA(const char* pFormat, ...);

	void AddLogS(const _TCHAR* pMsg);
	void AddLogLineS(const _TCHAR* pMsg);
	void AddLogSA(const char* pMsg);
	void AddLogLineSA(const char* pMsg);
		
	// �α� ������ �÷��� �Ѵ�.
	void Flush() { fflush(_fp); }


	// �α� ���� ���� ��� �� ������ ����
	void ChangePath(const _TCHAR* FilePath, const _TCHAR* sFormat = _T(""));
	
	// ���� ���� ��� ��ȯ
	static LPCTSTR GetCurrentDirPath(LPTSTR szPath, DWORD dwPathBufSize);

protected:
	enum 
	{ 
		// ���� ������ ���� ���� ���۸� ���� ���� �ִ� ����
		//  �� �̻��̸� ���� ���
		MAX_VA_STACK_LOG_LEN = 1023 
	}; 
	
	// TCHAR Variable-argument �� ���� �޾Ƽ� ���� ó�� �Լ�
	// [in] bAddCrLf: �α� ���� "\r\n" �ڵ� �߰� ����
	void AddLog(bool bAddCrLf, const _TCHAR* pFormat, va_list pArg);
	void AddLogA(bool bAddCrLf, const char* pFormat, va_list pArg);

	// [in] bAddCrLf: �α� ���� "\r\n" �ڵ� �߰� ����
	void AddLogS(bool bAddCrLf, const _TCHAR* pMsg);
	void AddLogSA(bool bAddCrLf, const char* pMsg);


	// �Էµ� ��¥�� �ش�Ǵ� ���ο� �α� ������ �����Ѵ�.
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
		Log.AddLog(_T("AddLog, ��%d��%f��%c��\r\n"), 1, 2.2, 'c');
		Log.AddLogA("AddLogA, ��%d��%f��%c��\r\n", 1, 2.2, 'c');

		Log.AddLogS(_T("AddLogS, �����ٴ�\r\n"));
		Log.AddLogSA("AddLogSA, �����ٴ�\r\n");

		Log.AddLogLine(_T("AddLogLine, ��%d��%f��%c��"), 1, 2.2, 'c');
		Log.AddLogLineA("AddLogLineA, ��%d��%f��%c��", 1, 2.2, 'c');

		Log.AddLogLineS(_T("AddLogLineS, �����ٴ�"));
		Log.AddLogLineSA("AddLogLineSA, �����ٴ�");

		Log.Close();
	}

	return 0;
}
*/
