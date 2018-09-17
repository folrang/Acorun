/**
 * @file	StrUtil.h
 * @brief	
 *
**/
#pragma once

#include <string>
#include <list>
#include <vector>
#include <atlbase.h> // CA2WEX
using namespace std;

// tstring 정의
#ifndef tstring
	#ifdef _UNICODE
	#define tstring std::wstring
	#else
	#define tstring std::string
	#endif
#endif

namespace StrUtil
{
	const int ALL_FIND_COUNT = -1;

	/**
	 * @brief				구분자 집합으로 소스 문자열을 분리하여 리스트로 반환
	 * @param	src			분리할 문자열
	 * @param	delimeters	구분자 집합 문자열, ex) "\t,. \n"
	 * @param	bUnique		TRUE: 중복입력 체크하여 새 token 만 추가, FALSE: 중복여부와 상관없이 모두 추가
	 * @param	bNullInsert TRUE: 빈문자열 token 도 추가
	 * @param	nMaxFindCount	ALL_FIND_COUNT: 전체
	 * @param [in,out]	out	결과 반환 리스트 
	 * @return				결과 반환 리스트 
	**/
	list<tstring>& Split1(const tstring& src, const tstring& delimeters, 
		BOOL bUnique, BOOL bNullInsert, int nMaxFindCount,
		list<tstring>& out);

	/**
	 * @brief				구분자 집합으로 소스 문자열을 분리하여 리스트로 반환
	 * @param	src			분리할 문자열
	 * @param	delimeters	구분자 집합 문자열, ex) "\t,. \n"
	 * @param	bUnique		TRUE: 중복입력 체크하여 새 token 만 추가, FALSE: 중복여부와 상관없이 모두 추가
	 * @param	bNullInsert TRUE: 빈문자열 token 도 추가
	 * @param	nMaxFindCount	ALL_FIND_COUNT: 전체
	 * @param [in,out]	out	결과 반환 리스트 
	 * @return				결과 반환 리스트 
	**/
	vector<tstring>& Split1(const tstring& src, const tstring& delimeters, 
		BOOL bUnique, BOOL bNullInsert, int nMaxFindCount,
		vector<tstring>& out);

	/**
	 * @brief					매치 문자열로 소스 문자열을 분리하여 리스트로 반환
	 * @param	src				분리할 문자열 
	 * @param	matchStr		구분자문자열. ex) ";", "\n", "\r\n" 
	 * @param	bUnique		TRUE: 중복입력 체크하여 새 token 만 추가, FALSE: 중복여부와 상관없이 모두 추가
	 * @param	bNullInsert		TRUE: 빈문자열 token 도 추가
	 * @param	nMaxFindCount	ALL_FIND_COUNT: 전체
	 * @param [in,out]	out		결과 반환 리스트
	 * @return					결과 반환 리스트
	**/
	list<tstring>& Split2(const tstring& src, const tstring& matchStr, 
		BOOL bUnique, BOOL bNullInsert, int nMaxFindCount,
		list<tstring>& out);
	
	// "key=val" 로 구성된 문자열을 key 와 val 추출하여 반환
	void SpliceKeyVal(const tstring& src, tstring& key, tstring& val, TCHAR aDelemiter = '=');


	/**
	 * @brief				find 정확히 일치하는 문자열을 replacewith 문자열로 모두 치환
	 *						find() 이용
	 * @param [in,out]	src	source for the. 
	 * @param	find		검색 문자열
	 * @param	replacewith	The replacewith. 
	 * @return				. 
	**/
	tstring& Replace(tstring& src, LPCTSTR find, LPCTSTR replacewith, BOOL bAll = FALSE);
	tstring& Replace(const tstring& src, LPCTSTR find, LPCTSTR replacewith, tstring& out, BOOL bAll = FALSE);
	//tstring ReplaceAll(const tstring& src, const tstring& find, const tstring& replacewith);

	/**
	 * @brief				find (찾고자 하는 문자들 집합) 문자(들)을 replacewith 문자열로 모두 치환
	 *						find_first_of() 이용
	 * @param [in,out]	src	source for the. 
	 * @param	find		찾고자하는 문자들의 집합 ex: "&;#$"
	 * @param	replacewith	The replacewith. 
	 * @return				. 
	**/
	tstring& Replace1(tstring& src, LPCTSTR find, LPCTSTR replacewith, BOOL bAll = FALSE);

	// str 문자열의 앞뒤 문자(' ') 제거 후 반환
	tstring& Trim(tstring& str, TCHAR removeChar = ' ');
	
	/**
	 * @brief				포맷 문자열, "2.1MByte"
	 * @param	nByte		
	 * @param [in,out]	out	
	 * @return				. 
	**/
	//string& StrUtil::StrFormatByteSize(UINT64 nByte, tstring& out);


	tstring& FormatString(tstring& out, LPCTSTR fmt, ...);
	
	/**
	 * @brief				원본이 ascii 문자열임을 보장한다면 아래 함수를 사용, 보장을 못하면 A2WS, W2AS
	**/
	wstring& A2WAscii(const string& src, wstring& dst);
	string& W2AAscii(const wstring& src, string& dst);
	wstring A2WAscii(const string& src);
	string W2AAscii(const wstring& src);

	/**
	 * @brief				원본이 ascii 문자열임을 보장한다면 A2WAscii, W2AAscii 함수를 사용을 권함
	**/
	wstring& A2WS(const string& src, wstring& dst);
	string& W2AS(const wstring& src, string& dst);
}


inline wstring& StrUtil::A2WAscii(const string& src, wstring& dst)
{
	return dst.assign(src.begin(), src.end());
}

inline string& StrUtil::W2AAscii(const wstring& src, string& dst)
{
	return dst.assign(src.begin(), src.end());
}

inline wstring StrUtil::A2WAscii(const string& src)
{
	return wstring(src.begin(), src.end());
}

inline string StrUtil::W2AAscii(const wstring& src)
{
	return string(src.begin(), src.end());
}

inline wstring& StrUtil::A2WS(const string& src, wstring& dst)
{
	return dst = CA2WEX<256>(src.c_str());
}

inline string& StrUtil::W2AS(const wstring& src, string& dst)
{
	return dst = CW2AEX<256>(src.c_str());
}