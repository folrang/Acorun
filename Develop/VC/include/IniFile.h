//////////////////////////////////////////////////////////////////////
//				IniFile.h
//							Code by DEDE
//
// [Abstract]
// - INI 파일을 읽고, 세션별 데이터 관리 하도록 하는 서비스 클래스
//
// [History]
// * 2009/04/09
// 
// * 2010/11/15
//		- 64bit 지원를 위한 수정
//
//////////////////////////////////////////////////////////////////////
#pragma once

#include <string>
#include <map>

#pragma warning( disable : 4996)

// tstring 정의
#ifndef tstring
	#ifdef UNICODE
	#define tstring std::wstring
	#else
	#define tstring std::string
	#endif
#endif

using namespace std;

#ifndef MAP_INI_STRKEY_STRVAL
	typedef map<tstring, tstring> MAP_INI_STRKEY_STRVAL;
	typedef map<tstring, tstring>::const_iterator MAP_INI_STRKEY_STRVAL_CIT;
	typedef map<tstring, tstring>::iterator MAP_INI_STRKEY_STRVAL_IT;
#endif

#ifndef MAP_INI_SECSTION
	typedef map<tstring, MAP_INI_STRKEY_STRVAL* > MAP_INI_SECSTION;
	typedef map<tstring, MAP_INI_STRKEY_STRVAL* >::const_iterator MAP_INI_SECSTION_CIT;
	typedef map<tstring, MAP_INI_STRKEY_STRVAL* >::iterator MAP_INI_SECSTION_IT;
#endif


// ini 파일내의 [세션명] 은 영문으로만 기재
class CIniFile
{
public:
	CIniFile();
	// 여러 ini 파일을 단일 SectionMap으로 관리할 시에 사용되는 생성자 함수
	// [in] pSectionMapPtr: new 생성된 객체
	// [in] bDestroySectionMap: pSectionMap를 Clear 및 Destroy 시킬지 여부
	//			FALSE: Close() 및 pSectionMap 맵을 Clear() 작업이 내부에서 발생되지 않으며, 
	//				소멸자 함수에서도 pSectionMap 객체를 삭제하지 않음
	CIniFile(MAP_INI_SECSTION* pSectionMapPtr, BOOL bDestroySectionMap);
	~CIniFile(void);

	// Ini 파일을 열어서 초기 구성
	// [retVal] 추가된 세션 개수
	int Open(LPCTSTR szFilePath);
	
	// 현재 내용을 szFilePath 경로로 새로 작성한다 (주석은 제외)
	// [in] szFilePath: 저장할 경로, NULL 인 경우에는 오픈된 경로에 재저장
	// [retVal] 성공 여부
	BOOL ReWrite(LPCTSTR szFilePath = NULL);
	
	// Open 성공이면 Close 호출
	void Close();
	
	// 해당 세션 및 키에 해당하는 값을 반환
	// [retVal] 없는 경우, tstring("") 스트링을 반환
	tstring GetStrVal(LPCTSTR szSectionName, LPCTSTR szKey);
	LPCTSTR GetVal(LPCTSTR szSectionName, LPCTSTR szKey);
	
	// 세션명 변경
	void SetSectionName(LPCTSTR szNewName, LPCTSTR szOldName);
	
	// 키이름 변경
	void SetKeyName(LPCTSTR szSectionName, LPCTSTR szNewKeyName, LPCTSTR szOldKeyName);

	// 값 변경
	void SetStrVal(LPCTSTR szSectionName, LPCTSTR szKey, LPCTSTR szNewVal);

	LPCTSTR GetPath() { return m_szPath; }
	
	// 섹션 개수 반환
	int GetSectionSize();

	// 섹션내 키 개수 반환
	int GetKeySize(LPCTSTR szSectionName);

	// 지정한 파일, 섹션, 키이름에 해당하는 키 값을 찾아서 반환
	// [out] szKeyVal: 키값, "" : 없거나 실패할 경우 반환
	// [in] dwKeyValBufSize: szKeyVal 버퍼 크기
	// [retVal] szKeyVal을 반환
	static LPCTSTR GetIniKeyVal(LPCTSTR szFilePath, LPCTSTR szSectionName, LPCTSTR szKeyName, LPTSTR szKeyVal, DWORD dwKeyValBufSize);
	static tstring GetIniKeyVal(LPCTSTR szFilePath, LPCTSTR szSectionName, LPCTSTR szKeyName);
	
	// 지정한 파일, 섹션, 키이름에 키값 설정
	//	- 파일 및 섹션, 키이름 들은 자동 생성 됨
	// [retVal] FALSE: 설정 실패
	static BOOL SetIniKeyVal(LPCTSTR szFilePath, LPCTSTR szSectionName, LPCTSTR szKeyName, LPCTSTR szKeyVal);

	static int Convert2Int(const tstring& in)
	{
		return Convert2Int(in.c_str());
	}
	static int Convert2Int(LPCTSTR szIn)
	{
		return _ttoi(szIn);
	}

	static UINT Convert2UInt(LPCTSTR szIn)
	{
		return _tcstoul(szIn, NULL, 10);
	}



	MAP_INI_SECSTION* GetInnerMap() { return m_pSectionMap; }

protected:
	enum
	{
		MAX_SECTIONNAMES_SIZE		= 1024,		// 세션 이름 리스트 문자열의 최대 버퍼 크기
		DEFAULT_FILE_SIZE			= 2048,		// ini의 디폴트 파일 사이즈 (내부버퍼크기 할당)
		MAX_SECTION_CONTENT_SIZE	= 32768,	// 한 세션 내용의 최대 버퍼 크기 (MSDN GetPrivateProfileSection 참고)
		MAX_LINE_SIZE				= 1024		// 한 라인의 최대 버퍼 길이
	};

	tstring& MakeIniFileContext(tstring& out) const;

	// "토큰1\0토큰1\0....\0토큰n\0\0"
	// [세션이름] 리스트 혹은 키&값(key=val) 리스트에서 한개의 아이템을 추출 및 반환
	// 토큰 리스트 문자열에서 첫 토큰을 out에 복사하고, 다음 위치를 반환
	// [in] szSrc: "토큰1\0토큰1\0....\0토큰n\0\0" 와 같은 포맷 문자열
	// [out] outToken: 추출된 토큰이름
	// [retVal] NULL 일 경우, 다음 토큰 없음
	LPTSTR GetToken(LPTSTR szSrc, LPTSTR outToken) const;

	// "key=val" 로 구성된 문자열을 key 와 val 추출하여 반환
	// key 와 val 문자열은 Trim 적용
	void SpliceKeyVal(const tstring& src, tstring& key, tstring& val) const;

	// str 문자열의 앞뒤 공백 제거 후 반환
	tstring& Trim(tstring& str) const;

private:
	_TCHAR m_szPath[MAX_PATH +1];
	BOOL m_bDestroySectionMap;
	MAP_INI_SECSTION* m_pSectionMap;	// 맵키: 세션명, 맵값: 세션 내용 (ini key : ini value)
};


/*
int _tmain(int argc, _TCHAR* argv[])
{
	BOOL bSuc;
	tstring szVal;
	int nVal;

	CIniFile Ini;
	bSuc = Ini.Open(_T("D:\\test\\test.ini"));
	if (bSuc)
	{
		szVal = Ini.GetStrVal(_T("FTP_LIST"), _T("FTP_C6_DIR"));
		szVal = Ini.GetStrVal(_T("CMOS_DEFINE"), _T("NUM01"));

		szVal = Ini.GetStrVal(_T("FTP_LIST"), _T("Btn_Diplay_Chk2"));
		nVal = Ini.Convert2Int(szVal);

		Ini.SetSectionName(_T("BUFFER_XXXX"), _T("BUFFER_SIZE_DEFINE"));
		Ini.SetKeyName(_T("BUFFER_XXXX"), _T("HOSTRECV_XXXX"), _T("HOSTRECV_MAX_SIZE"));
		Ini.SetStrVal(_T("BUFFER_XXXX"), _T("HOSTRECV_XXXX"), _T("4444"));

		bSuc = Ini.ReWrite(_T("D:\\test\\test2.ini"));

		Ini.Close();
	}
	
	return 0;
}
*/