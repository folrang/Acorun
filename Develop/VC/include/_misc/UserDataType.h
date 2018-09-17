/**
 * @file		UserDataType.h
 * @brief		RubyEx���� ���Ǵ� ���� Ÿ�Ե�. 
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

// tstring ����
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
// - long��(Key) �� tstring ������ ������ ������ Ŭ����
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
// - tstring��(Key) �� tstring ������ ������ ������ Ŭ����
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
 * @brief	��Ű: tstring, �ʵ�����: T �� ������ ���ø� �� Ŭ����
 *			������ ����ȭ ����
 *
 * @warning template class T �� \n
 *			default contruct, operator =, ==, < �� �����ؾ���
 *
 *			������ �������� ���, SetNullVal(NULL) ȣ���� �ʼ�
**/
template <class T> 
class CStrKeyMap
{
public:

	/**
	 * @brief					������
	 * @brief	bPointerItem	�������� ������ ��ü������ ����	
	 * @param	bThreadSafe		������ ����ȭ �ʿ� ����
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
	 * @brief			GetXXX ���� �Լ����� ã���� �ϴ� �������� ���� ���, ��ȯ�Ǵ� �� ������
						Item T�� ������ ��ü���, NULL ���� �ʿ�
						
						map<int* > MyMap;
						MyMap.SetDefaultItem(NULL);
						if (MyMap.GetItem(0) == NULL)
						{
							ã�� ����
						}
	 * @param	NullVal	NULL ������
	**/
	void SetNullVal(const T& NullVal) 
	{
		m_NULLVAL = NullVal;
	}

	/**
	 * @brief			�ʿ� �̹� �������� �����ϸ� ����, ���� ��쿡�� ���� �߰�
	 *
	 * @param	key		������ Ű
	 * @param	item	������ ������ 
	 *
	 * @return			TRUE: �����߰�
	 * @return			FALSE: ���� ������ ����
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
	 * @brief				���翩�θ� ���ϰ����� ��ȯ�ϰ�,
	 *						�����ϸ�, key�� �ش��ϴ� T �������� ���۷����� ��ȯ

	 * @param	key			ã�����ϴ� ������ Ű
	 * @param [in,out] out	������ ���, ������ ��ȯ
	 
	 * @return				���� ��쿡�� ����Ʈ T ��ü��ȯ�ϰ� ������ �ش� T ��ü�� ��ȯ
	 
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
	 * @brief		key�� �ش��ϴ� T �������� ���纻�� ��ȯ 
	 * @param	key	ã�����ϴ� ������ Ű
	 * @return		���� ��쿡�� ����Ʈ T ��ü��ȯ�ϰ� ������ �ش� T ��ü�� ��ȯ
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
	 * @brief		key�� �ش��ϴ� T �������� �����͸� ��ȯ (��ȯ�� ��뿡 ����ȭ ���� ���)
	 * @param	key	ã�����ϴ� ������ Ű
	 * @return		NULL: ��ü ����, bThreadSafe ��忡���� ���ϵ� T* ��뿡 ���� �ʿ�
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
	 * @brief		�ش� Ű���� ���� ������ ��θ� ����
	 * @param	key	������ �������� Ű
	 * @return		������ �� ũ�� ��ȯ
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
	 * @brief		key �� �����ϴ����� ��ȯ
	 * @param	key	ã�����ϴ� ������ Ű
	 * @return		TRUE: ������, FALSE: �������� ����
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
	 * @brief ���� ���� Ŭ����
	**/
	void Clear() 
	{ 
		ENTER_UDT_CS(m_pCs);
		m_Map.clear(); 
		LEAVE_UDT_CS(m_pCs);
	}

	/**
	 * @return	���� STL::map ��ü�� ��ȯ
	 * @warning ����ȭ ���� ���(bThreadSafe�� TRUE)�ϰ��, ����ȭ�� å���� ��������� å��������
	**/
	map<tstring, T>& GetInnerMap() { return m_Map; }
	const map<tstring, T>* GetInnerMapPtr() const { return &m_Map; }

private:
	CRITICAL_SECTION* m_pCs;

	T m_NULLVAL;
	map<tstring, T> m_Map;
};
