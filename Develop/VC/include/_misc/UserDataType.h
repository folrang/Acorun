/**
 * @file		UserDataType.h
 * @brief		RubyEx에서 사용되는 공통 타입들. 
 * @author		Dede
**/
#pragma once

#pragma warning(disable:4786)

#include <tchar.h>
#include <string>
#include <list>
#include <vector>
#include <map>

using namespace std;

// tstring 정의
#ifndef tstring
	#ifdef _UNICODE
	#define tstring std::wstring
	#else
	#define tstring std::string
	#endif
#endif

#ifndef ENTER_UDT_CS
	#define ENTER_UDT_CS(pCs)	if (pCs) EnterCriticalSection(pCs)
#endif
#ifndef LEAVE_UDT_CS
	#define LEAVE_UDT_CS(pCs)	if (pCs) LeaveCriticalSection(pCs)
#endif

typedef list<int> LST_NUM;
typedef list<int>::const_iterator LST_NUM_CIT;
typedef list<int>::iterator LST_NUM_IT;

typedef vector<tstring> VCT_STR;
typedef vector<tstring>::const_iterator VCT_STR_CIT;
typedef vector<tstring>::iterator VCT_STR_IT;

//////////////////////////////////////////////////////////////////////
//				CNumStrItem
// - long형(Key) 과 tstring 값으로 구성된 데이터 클래스
//////////////////////////////////////////////////////////////////////
class CNumStrItem
{
public:
	CNumStrItem() : m_Key(-1), m_Data(_T("")) {}
	CNumStrItem(long key, const tstring& data) : m_Key(key), m_Data(data) {}
	CNumStrItem(const CNumStrItem& rhs) { Copy(rhs); }
	~CNumStrItem() { Destroy(); }
	
	CNumStrItem& operator = (const CNumStrItem& rhs)
	{
		if (this != &rhs)
		{
			Destroy(); Copy(rhs);
		}
		return *this;
	}
	BOOL operator == (const CNumStrItem& rhs) { return (m_Key == rhs.m_Key); }

	BOOL operator < (const CNumStrItem& rhs) { return (m_Key < rhs.m_Key); }


protected:
	void Copy(const CNumStrItem& rhs)
	{
		m_Key = rhs.m_Key; m_Data = rhs.m_Data;
	}
	void Destroy() {}

public:
	long	m_Key;
	tstring m_Data;
};
typedef list<CNumStrItem> LST_NUMSTR;
typedef list<CNumStrItem>::const_iterator LST_NUMSTR_CIT;
typedef list<CNumStrItem>::iterator LST_NUMSTR_IT;

typedef vector<CNumStrItem> VCT_NUMSTR;
typedef vector<CNumStrItem>::const_iterator VCT_NUMSTR_CIT;
typedef vector<CNumStrItem>::iterator VCT_NUMSTR_IT;

//////////////////////////////////////////////////////////////////////
//				CStrStrItem
// - tstring형(Key) 과 tstring 값으로 구성된 데이터 클래스
//////////////////////////////////////////////////////////////////////
class CStrStrItem
{
public:
	CStrStrItem() : m_Key(_T("")), m_Data(_T("")) {}
	CStrStrItem(const tstring& key, const tstring& data) : m_Key(key), m_Data(data) {}
	CStrStrItem(const CStrStrItem& rhs) { Copy(rhs); }
	~CStrStrItem() { Destroy(); }
	
	CStrStrItem& operator = (const CStrStrItem& rhs)
	{
		if (this != &rhs)
		{
			Destroy(); Copy(rhs);
		}
		return *this;
	}
	BOOL operator == (const CStrStrItem& rhs) { return (m_Key == rhs.m_Key); }

	BOOL operator < (const CStrStrItem& rhs) { return (m_Key < rhs.m_Key); }

	void Clear() { m_Key = _T(""); m_Data = _T(""); }

protected:
	void Copy(const CStrStrItem& rhs)
	{
		m_Key = rhs.m_Key; m_Data = rhs.m_Data;
	}
	void Destroy() {}

public:
	tstring	m_Key;
	tstring m_Data;
};
typedef list<CStrStrItem> LST_STRSTR;
typedef list<CStrStrItem>::const_iterator LST_STRSTR_CIT;
typedef list<CStrStrItem>::iterator LST_STRSTR_IT;

/**
 * @class	CStrKeyMap<T>
 * @brief	맵키: tstring, 맵데이터: T 로 구성된 템플릿 맵 클레스
 *			스레드 동기화 지원
 *
 * @warning template class T 는 \n
 *			default contruct, operator =, ==, < 를 제공해야함
 *
 *			포인터 아이템인 경우, SetNullVal(NULL) 호출은 필수
**/
template <class T> 
class CStrKeyMap
{
public:

	/**
	 * @brief					생성자
	 * @brief	bPointerItem	아이템이 포인터 객체인지를 설정	
	 * @param	bThreadSafe		스레드 동기화 필요 여부
	**/
	CStrKeyMap(BOOL bThreadSafe = FALSE) : m_pCs(NULL)
	{
		if (bThreadSafe) 
		{
			m_pCs = new CRITICAL_SECTION;
			InitializeCriticalSection(m_pCs);
		}
	}
	
	~CStrKeyMap() 
	{
		if (m_pCs)
			DeleteCriticalSection(m_pCs);	
	}

	/**
	 * @fn				void SetNullVal(const T& item)
	 * @brief			GetXXX 관련 함수에서 찾고자 하는 아이템이 없는 경우, 반환되는 널 아이템
						Item T가 포인터 객체라면, NULL 지정 필요
						
						map<int* > MyMap;
						MyMap.SetDefaultItem(NULL);
						if (MyMap.GetItem(0) == NULL)
						{
							찾기 실패
						}
	 * @param	NullVal	NULL 아이템
	**/
	void SetNullVal(const T& NullVal) 
	{
		m_NULLVAL = NullVal;
	}

	/**
	 * @brief			맵에 이미 아이템이 존재하면 수정, 없는 경우에는 새로 추가
	 *
	 * @param	key		아이템 키
	 * @param	item	아이템 데이터 
	 *
	 * @return			TRUE: 새로추가
	 * @return			FALSE: 기존 데이터 수정
	**/
	BOOL SetItem(const tstring& key, const T& item)
	{
		BOOL retVal = FALSE;
		ENTER_UDT_CS(m_pCs);

		map<tstring, T>::iterator it = m_Map.find(key);
		if (it != m_Map.end())
		{
			it->second = item;
			retVal = TRUE;
		}
		else
		{
			retVal = m_Map.insert(make_pair(key, item)).second;
		}
		
		LEAVE_UDT_CS(m_pCs);
		return retVal;
	}

	/**
	 * @brief				존재여부를 리턴값으로 반환하고,
	 *						존재하면, key에 해당하는 T 아이템의 레퍼런스를 반환

	 * @param	key			찾고자하는 아이템 키
	 * @param [in,out] out	존재할 경우, 아이템 반환
	 
	 * @return				없는 경우에는 디폴트 T 객체반환하고 있으면 해당 T 객체를 반환
	 
	 * @sa					GetItemEx()
	**/
	BOOL GetItem(const tstring& key, T& out)
	{
		BOOL retVal = FALSE;
		ENTER_UDT_CS(m_pCs);

		map<tstring, T>::iterator it = m_Map.find(key);
		if (it != m_Map.end())
		{
			out = (*it).second;
			retVal = TRUE;
		}
		
		LEAVE_UDT_CS(m_pCs);
		return retVal;
	}
	
	/**
	 * @brief		key에 해당하는 T 아이템의 복사본을 반환 
	 * @param	key	찾고자하는 아이템 키
	 * @return		없는 경우에는 디폴트 T 객체반환하고 있으면 해당 T 객체를 반환
	 * @sa			GetItem()
	**/
	T GetItem(const tstring& key)
	{
		T retVal(m_NULLVAL);
		ENTER_UDT_CS(m_pCs);

		map<tstring, T>::iterator it = m_Map.find(key);
		if (it != m_Map.end())
			retVal = (*it).second;	
		
		LEAVE_UDT_CS(m_pCs);
		return retVal;
	}

	/**
	 * @brief		key에 해당하는 T 아이템의 포인터를 반환 (반환값 사용에 동기화 주의 요망)
	 * @param	key	찾고자하는 아이템 키
	 * @return		NULL: 객체 없음, bThreadSafe 모드에서는 리턴된 T* 사용에 주의 필요
	 * @sa			GetItemEx()
	**/
	T* GetItemEx(const tstring& key)
	{
		T* retVal = NULL;
		ENTER_UDT_CS(m_pCs);

		map<tstring, T>::iterator it = m_Map.find(key);
		if (it != m_Map.end())
			retVal = &((*it).second);	
		
		LEAVE_UDT_CS(m_pCs);
		return retVal;
	}

	/**
	 * @brief		해당 키값을 갖는 아이템 모두를 삭제
	 * @param	key	삭제할 아이템의 키
	 * @return		삭제후 맵 크기 반환
	**/
	int Erase(const tstring& key)
	{
		int nSize;
		ENTER_UDT_CS(m_pCs);
		nSize = m_Map.erase(key);
		LEAVE_UDT_CS(m_pCs);
		return (int)nSize;
	}

	/**
	 * @brief		key 가 존재하는지를 반환
	 * @param	key	찾고자하는 아이템 키
	 * @return		TRUE: 존재함, FALSE: 존재하지 않음
	**/
	BOOL IsExist(const tstring& key)
	{
		BOOL retVal = FALSE;
		ENTER_UDT_CS(m_pCs);

		map<tstring, T>::iterator it = m_Map.find(key);
		if (it != m_Map.end())
			retVal = TRUE;

		LEAVE_UDT_CS(m_pCs);
		return retVal;
	}

	BOOL IsEmpty()
	{
		BOOL retVal = FALSE;
		ENTER_UDT_CS(m_pCs);
		retVal = m_Map.empty();
		LEAVE_UDT_CS(m_pCs);
		return retVal;
	}

	int Size()
	{
		int retVal = 0;
		ENTER_UDT_CS(m_pCs);
		retVal = m_Map.size();
		LEAVE_UDT_CS(m_pCs);
		return retVal;
	}

	/**
	 * @brief 내부 맵을 클리어
	**/
	void Clear() 
	{ 
		ENTER_UDT_CS(m_pCs);
		m_Map.clear(); 
		LEAVE_UDT_CS(m_pCs);
	}

	/**
	 * @return	내부 STL::map 객체를 반환
	 * @warning 동기화 지원 모드(bThreadSafe가 TRUE)일경우, 동기화의 책임은 사용측에서 책임져야함
	**/
	map<tstring, T>& GetInnerMap() { return m_Map; }
	const map<tstring, T>* GetInnerMapPtr() const { return &m_Map; }

private:
	CRITICAL_SECTION* m_pCs;

	T m_NULLVAL;
	map<tstring, T> m_Map;
};
