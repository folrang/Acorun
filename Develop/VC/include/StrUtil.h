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

// tstring ����
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
	 * @brief				������ �������� �ҽ� ���ڿ��� �и��Ͽ� ����Ʈ�� ��ȯ
	 * @param	src			�и��� ���ڿ�
	 * @param	delimeters	������ ���� ���ڿ�, ex) "\t,. \n"
	 * @param	bUnique		TRUE: �ߺ��Է� üũ�Ͽ� �� token �� �߰�, FALSE: �ߺ����ο� ������� ��� �߰�
	 * @param	bNullInsert TRUE: ���ڿ� token �� �߰�
	 * @param	nMaxFindCount	ALL_FIND_COUNT: ��ü
	 * @param [in,out]	out	��� ��ȯ ����Ʈ 
	 * @return				��� ��ȯ ����Ʈ 
	**/
	list<tstring>& Split1(const tstring& src, const tstring& delimeters, 
		BOOL bUnique, BOOL bNullInsert, int nMaxFindCount,
		list<tstring>& out);

	/**
	 * @brief				������ �������� �ҽ� ���ڿ��� �и��Ͽ� ����Ʈ�� ��ȯ
	 * @param	src			�и��� ���ڿ�
	 * @param	delimeters	������ ���� ���ڿ�, ex) "\t,. \n"
	 * @param	bUnique		TRUE: �ߺ��Է� üũ�Ͽ� �� token �� �߰�, FALSE: �ߺ����ο� ������� ��� �߰�
	 * @param	bNullInsert TRUE: ���ڿ� token �� �߰�
	 * @param	nMaxFindCount	ALL_FIND_COUNT: ��ü
	 * @param [in,out]	out	��� ��ȯ ����Ʈ 
	 * @return				��� ��ȯ ����Ʈ 
	**/
	vector<tstring>& Split1(const tstring& src, const tstring& delimeters, 
		BOOL bUnique, BOOL bNullInsert, int nMaxFindCount,
		vector<tstring>& out);

	/**
	 * @brief					��ġ ���ڿ��� �ҽ� ���ڿ��� �и��Ͽ� ����Ʈ�� ��ȯ
	 * @param	src				�и��� ���ڿ� 
	 * @param	matchStr		�����ڹ��ڿ�. ex) ";", "\n", "\r\n" 
	 * @param	bUnique		TRUE: �ߺ��Է� üũ�Ͽ� �� token �� �߰�, FALSE: �ߺ����ο� ������� ��� �߰�
	 * @param	bNullInsert		TRUE: ���ڿ� token �� �߰�
	 * @param	nMaxFindCount	ALL_FIND_COUNT: ��ü
	 * @param [in,out]	out		��� ��ȯ ����Ʈ
	 * @return					��� ��ȯ ����Ʈ
	**/
	list<tstring>& Split2(const tstring& src, const tstring& matchStr, 
		BOOL bUnique, BOOL bNullInsert, int nMaxFindCount,
		list<tstring>& out);
	
	// "key=val" �� ������ ���ڿ��� key �� val �����Ͽ� ��ȯ
	void SpliceKeyVal(const tstring& src, tstring& key, tstring& val, TCHAR aDelemiter = '=');


	/**
	 * @brief				find ��Ȯ�� ��ġ�ϴ� ���ڿ��� replacewith ���ڿ��� ��� ġȯ
	 *						find() �̿�
	 * @param [in,out]	src	source for the. 
	 * @param	find		�˻� ���ڿ�
	 * @param	replacewith	The replacewith. 
	 * @return				. 
	**/
	tstring& Replace(tstring& src, LPCTSTR find, LPCTSTR replacewith, BOOL bAll = FALSE);
	tstring& Replace(const tstring& src, LPCTSTR find, LPCTSTR replacewith, tstring& out, BOOL bAll = FALSE);
	//tstring ReplaceAll(const tstring& src, const tstring& find, const tstring& replacewith);

	/**
	 * @brief				find (ã���� �ϴ� ���ڵ� ����) ����(��)�� replacewith ���ڿ��� ��� ġȯ
	 *						find_first_of() �̿�
	 * @param [in,out]	src	source for the. 
	 * @param	find		ã�����ϴ� ���ڵ��� ���� ex: "&;#$"
	 * @param	replacewith	The replacewith. 
	 * @return				. 
	**/
	tstring& Replace1(tstring& src, LPCTSTR find, LPCTSTR replacewith, BOOL bAll = FALSE);

	// str ���ڿ��� �յ� ����(' ') ���� �� ��ȯ
	tstring& Trim(tstring& str, TCHAR removeChar = ' ');
	
	/**
	 * @brief				���� ���ڿ�, "2.1MByte"
	 * @param	nByte		
	 * @param [in,out]	out	
	 * @return				. 
	**/
	//string& StrUtil::StrFormatByteSize(UINT64 nByte, tstring& out);


	tstring& FormatString(tstring& out, LPCTSTR fmt, ...);
	
	/**
	 * @brief				������ ascii ���ڿ����� �����Ѵٸ� �Ʒ� �Լ��� ���, ������ ���ϸ� A2WS, W2AS
	**/
	wstring& A2WAscii(const string& src, wstring& dst);
	string& W2AAscii(const wstring& src, string& dst);
	wstring A2WAscii(const string& src);
	string W2AAscii(const wstring& src);

	/**
	 * @brief				������ ascii ���ڿ����� �����Ѵٸ� A2WAscii, W2AAscii �Լ��� ����� ����
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