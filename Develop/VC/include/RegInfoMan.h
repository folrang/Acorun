// RegInfoMan.h: interface for the CRegInfoMan class.
//
// - ICD보다 단순한 레지스트리 관련 서비스 클래스
// - INT, STRING 데이터 값의 읽기, 쓰기등 간단한 레지스트리 함수 제공
// - Code by DEDE (in TXX)
//
// [History]
// * 1.0.0.1
//	- Class Lib 최초 작성
//	- 2002.4.1
// * 1.0.0.2
//	- RegReadInt(), RegReadString(), RegWriteInt(), RegWriteString() 메서드에 
//		BOOL bCreate 파리미터 추가
//	- 2003.2.4
// * 1.0.0.3
//	- FullPath 를 구성하는 코드에서 wsprintf 대신에 _stprintf를 사용하도록 수정
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
	// hKey: 부모키 지정
	CRegInfoMan(HKEY hKey = HKEY_LOCAL_MACHINE);
	virtual ~CRegInfoMan();
	
	// [in] RootPath: 기준 SubKey(ex:"SOFTWARE\\MyCompany")
	// Root Path를 설정하지 않고, 아래 4가지 서비스함수의 인자 lpSubKey에 완전한 SubKey를 입력하여도 된다.
	void SetRootPath(_TCHAR* RootPath) { _tcscpy(_RootPath, RootPath); }
	const _TCHAR* GetRootPath() const { return _RootPath;}
	
	// [in] lpSubKey: _RootPath(ex:"SOFTWARE\\MyCompany") 아래의 SubKey값(ex:"MyProduct\\service")
	// [in] lpValue: 읽을 value 값
	// [in, out] nResult: 해당 키에 대한 읽은 결과 값, retVal이 FALSE에는 ApiError Code 반환
	// [in] bCreate: Open 실패시 키 생성을 할것인 여부( TRUE라면 반드시 생성할수 있는 권한이 있어야 한다)
	// [retVal]: TRUE - 성공, FALSE - 실패(nResult: LastError반환)
	BOOL RegReadInt(LPCTSTR lpSubKey, LPCTSTR lpValue, INT* nResult, BOOL bCreate = TRUE);
	// [in] lpSubKey: _RootPath(ex:"SOFTWARE\\MyCompany") 아래의 SubKey값(ex:"MyProduct\\service")
	// [in] lpValue: 읽을 value 값
	// [in, out] lpResult: 해당 키에 대한 읽은 결과 값, retVal이 FALSE에는 NULL 반환
	// [in] dwSize: 설정 버퍼 크기
	// [in] bCreate: Open 실패시 키 생성을 할것인 여부( TRUE라면 반드시 생성할수 있는 권한이 있어야 한다)
	// [retVal]: TRUE - 성공, FALSE - 실패(nResult: NULL반환)
	BOOL RegReadString(LPCTSTR lpSubKey, LPCTSTR lpValue, LPTSTR lpResult, DWORD dwSize, BOOL bCreate = TRUE);
	
	// [in] lpSubKey: _RootPath(ex:"SOFTWARE\\MyCompany") 아래의 SubKey값(ex:"MyProduct\\service")
	// [in] lpValue: 설정할 value 값
	// [in] nResult: 설정할 키값
	// [in] bCreate: Open 실패시 키 생성을 할것인 여부( TRUE라면 반드시 생성할수 있는 권한이 있어야 한다)
	// [retVal]: TRUE - 성공, FALSE - 실패
	BOOL RegWriteInt(LPCTSTR lpSubKey, LPCTSTR lpValue, INT nData, BOOL bCreate = TRUE);
	// [in] lpSubKey: _RootPath(ex:"SOFTWARE\\MyCompany") 아래의 SubKey값(ex:"MyProduct\\service")
	// [in] lpValue: 설정할 value 값
	// [in] lpData: 설정할 키 값
	// [in] bCreate: Open 실패시 키 생성을 할것인 여부( TRUE라면 반드시 생성할수 있는 권한이 있어야 한다)
	// [retVal]: TRUE - 성공, FALSE - 실패
	BOOL RegWriteString(LPCTSTR lpSubKey, LPCTSTR lpValue, LPCTSTR lpData, BOOL bCreate = TRUE);

private:
	HKEY _hRootKey;	// 부모키, 디폴트: HKEY_LOCAL_MACHINE
	_TCHAR _RootPath[MAX_PATH+1];
};

#endif // !defined(AFX_REGINFOMAN_H__C7D4B610_6270_4957_9F48_05B53BE8403A__INCLUDED_)
