// RegInfoMan.h: interface for the CRegInfoMan class.
//
// - ICD���� �ܼ��� ������Ʈ�� ���� ���� Ŭ����
// - INT, STRING ������ ���� �б�, ����� ������ ������Ʈ�� �Լ� ����
// - Code by DEDE (in TXX)
//
// [History]
// * 1.0.0.1
//	- Class Lib ���� �ۼ�
//	- 2002.4.1
// * 1.0.0.2
//	- RegReadInt(), RegReadString(), RegWriteInt(), RegWriteString() �޼��忡 
//		BOOL bCreate �ĸ����� �߰�
//	- 2003.2.4
// * 1.0.0.3
//	- FullPath �� �����ϴ� �ڵ忡�� wsprintf ��ſ� _stprintf�� ����ϵ��� ����
//	- 2004.10.19
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_REGINFOMAN_H__C7D4B610_6270_4957_9F48_05B53BE8403A__INCLUDED_)
#define AFX_REGINFOMAN_H__C7D4B610_6270_4957_9F48_05B53BE8403A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CRegInfoMan  
{
public:
	// hKey: �θ�Ű ����
	CRegInfoMan(HKEY hKey = HKEY_LOCAL_MACHINE);
	virtual ~CRegInfoMan();
	
	// [in] RootPath: ���� SubKey(ex:"SOFTWARE\\MyCompany")
	// Root Path�� �������� �ʰ�, �Ʒ� 4���� �����Լ��� ���� lpSubKey�� ������ SubKey�� �Է��Ͽ��� �ȴ�.
	void SetRootPath(_TCHAR* RootPath) { _tcscpy(_RootPath, RootPath); }
	const _TCHAR* GetRootPath() const { return _RootPath;}
	
	// [in] lpSubKey: _RootPath(ex:"SOFTWARE\\MyCompany") �Ʒ��� SubKey��(ex:"MyProduct\\service")
	// [in] lpValue: ���� value ��
	// [in, out] nResult: �ش� Ű�� ���� ���� ��� ��, retVal�� FALSE���� ApiError Code ��ȯ
	// [in] bCreate: Open ���н� Ű ������ �Ұ��� ����( TRUE��� �ݵ�� �����Ҽ� �ִ� ������ �־�� �Ѵ�)
	// [retVal]: TRUE - ����, FALSE - ����(nResult: LastError��ȯ)
	BOOL RegReadInt(LPCTSTR lpSubKey, LPCTSTR lpValue, INT* nResult, BOOL bCreate = TRUE);
	// [in] lpSubKey: _RootPath(ex:"SOFTWARE\\MyCompany") �Ʒ��� SubKey��(ex:"MyProduct\\service")
	// [in] lpValue: ���� value ��
	// [in, out] lpResult: �ش� Ű�� ���� ���� ��� ��, retVal�� FALSE���� NULL ��ȯ
	// [in] dwSize: ���� ���� ũ��
	// [in] bCreate: Open ���н� Ű ������ �Ұ��� ����( TRUE��� �ݵ�� �����Ҽ� �ִ� ������ �־�� �Ѵ�)
	// [retVal]: TRUE - ����, FALSE - ����(nResult: NULL��ȯ)
	BOOL RegReadString(LPCTSTR lpSubKey, LPCTSTR lpValue, LPTSTR lpResult, DWORD dwSize, BOOL bCreate = TRUE);
	
	// [in] lpSubKey: _RootPath(ex:"SOFTWARE\\MyCompany") �Ʒ��� SubKey��(ex:"MyProduct\\service")
	// [in] lpValue: ������ value ��
	// [in] nResult: ������ Ű��
	// [in] bCreate: Open ���н� Ű ������ �Ұ��� ����( TRUE��� �ݵ�� �����Ҽ� �ִ� ������ �־�� �Ѵ�)
	// [retVal]: TRUE - ����, FALSE - ����
	BOOL RegWriteInt(LPCTSTR lpSubKey, LPCTSTR lpValue, INT nData, BOOL bCreate = TRUE);
	// [in] lpSubKey: _RootPath(ex:"SOFTWARE\\MyCompany") �Ʒ��� SubKey��(ex:"MyProduct\\service")
	// [in] lpValue: ������ value ��
	// [in] lpData: ������ Ű ��
	// [in] bCreate: Open ���н� Ű ������ �Ұ��� ����( TRUE��� �ݵ�� �����Ҽ� �ִ� ������ �־�� �Ѵ�)
	// [retVal]: TRUE - ����, FALSE - ����
	BOOL RegWriteString(LPCTSTR lpSubKey, LPCTSTR lpValue, LPCTSTR lpData, BOOL bCreate = TRUE);

private:
	HKEY _hRootKey;	// �θ�Ű, ����Ʈ: HKEY_LOCAL_MACHINE
	_TCHAR _RootPath[MAX_PATH+1];
};

#endif // !defined(AFX_REGINFOMAN_H__C7D4B610_6270_4957_9F48_05B53BE8403A__INCLUDED_)
