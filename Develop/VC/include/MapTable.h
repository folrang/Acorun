/**
 * @file		MapTable.h
 * @brief		이중맵으로 구성된 맵 테이블 구현 클래스
 * @author		Dede
**/
#pragma once


#include <map>
using namespace std;

#ifndef ENTER_UDT_CS
	#define ENTER_UDT_CS(pCs)	if (pCs) EnterCriticalSection(pCs)
#endif
#ifndef LEAVE_UDT_CS
	#define LEAVE_UDT_CS(pCs)	if (pCs) LeaveCriticalSection(pCs)
#endif


/**
 * @class	CMapTable
 * @brief	map<N, map<K, T*>* > 을 관리하는 클래스
			내부 아이템 및 맵등은 모두 Heap 생성 됨(new/delete)
			스레드 동기화 지원

			N: 맵 이름 타입, copy, =, == 연산자 제공 필요
			K: 키 이름 타입, copy, =, == 연산자 제공 필요
			T: 아이템 타입, copy, = 연산자 제공 필요
 * @author	Dede
 * @date	2009-08-16
**/
template <class N, class K, class T>
class CMapTable
{
public:
	CMapTable(BOOL bThreadSafe = FALSE) : m_pCs(NULL), m_pMapTable(NULL)
	{
		if (bThreadSafe) 
		{
			m_pCs = new CRITICAL_SECTION;
			InitializeCriticalSection(m_pCs);
		}

		m_pMapTable = new map<N, map<K, T*>* >();
	}
	~CMapTable()
	{
		Clear();
		delete m_pMapTable;

		if (m_pCs)
			DeleteCriticalSection(m_pCs);
	}

	/**
	 * @fn		void Clear()
	 * @brief	맵테이블 전체를 클리어 
	**/
	void Clear();

	/**
	 * @fn				BOOL CreateMap(const N& MapName)
	 * @brief			맵테이블에 새 맵 추가
	 * @param	MapName	생성할 맵 이름
	 * @return			FALSE: 생성 실패, 이미 존재
	**/
	BOOL CreateMap(const N& MapName);

	/**
	 * @fn				void DeleteMap(const N& MapName)
	 * @brief			맵테이블에서 맵 제거
	 * @param	MapName	삭제할 맵 이름
	 * @return			삭제후 남은 맵 개수, -1: 제거 실패
	**/
	int DeleteMap(const N& MapName);

	/**
	 * @fn				void ClearMap(const N& MapName)
	 * @brief			해당 맵을 클리어
	 * @param	MapName	정리할 맵 이름
	**/
	void ClearMap(const N& MapName);

	/**
	 * @fn				BOOL IsExistMap(const N& MapName)
	 * @brief			맵테이블에 해당 맵이 존재하는지 여부
	 * @param	MapName	검색할 맵 이름 
	 * @return			TRUE: 존재, FALSE: 존재하지 않음
	**/
	BOOL IsExistMap(const N& MapName);


	/**
	 * @fn				BOOL AddMapItem(const N& MapName, const K& KeyName, const T& Item)
	 * @brief			맵에 새 아이템 추가
	 * @param	MapName	맵 이름
	 * @param	KeyName	키 이름
	 * @param	Item	아이템 이름
	 * @return			FALSE: 추가 실패, 이미 존재
	**/
	BOOL AddMapItem(const N& MapName, const K& KeyName, const T& Item);

	/**
	 * @fn				void RemoveMapItem(const N& MapName, const K& KeyName)
	 * @brief			맵에서 아이템 제거
	 * @param	MapName	맵 이름
	 * @param	KeyName	키 이름
	 * @return			삭제후 남은 키 개수, -1: 제거 실패
	**/
	int RemoveMapItem(const N& MapName, const K& KeyName);

	/**
	 * @fn				BOOL IsExistMapItem(const N& MapName, const K& KeyName)
	 * @brief			해당 맵에 해당 키값이 존재하는 지 여부 반환
	 * @param	MapName	맵 이름
	 * @param	KeyName	키 이름
	 * @return			TRUE: 존재, FALSE: 맵 혹은 키가 존재하지 않음
	**/
	BOOL IsExistMapItem(const N& MapName, const K& KeyName);

	/**
	 * @fn						BOOL GetMapItem(const N& MapName, const K& KeyName, T& Item)
	 * @brief					아이템 반환
	 * @param	MapName			맵 이름
	 * @param	KeyName			키 이름
	 * @param [in,out]	Item	반환될 아이템
	 * @return					FALSE: 반환실패, 맵 혹은 키가 존재하지 않음
	**/
	BOOL GetMapItem(const N& MapName, const K& KeyName, T& Item);

	/**
	 * @fn				void SetMapItem(const N& MapName, const K& KeyName, const T& Item)
	 * @brief			해당 키에 값을 설정
	 * @param	MapName	맵 이름
	 * @param	KeyName	키 이름
	 * @param	Item	아이템
	 * @return			FALSE: 설정 실패, 맵 혹은 키가 존재하지 않음
	**/
	BOOL SetMapItem(const N& MapName, const K& KeyName, const T& Item);
	
	/**
	 * @fn				void SetMapItemEx(const N& MapName, const K& KeyName, const T& Item)
	 * @brief			해당 키에 값을 설정, 만일 맵 혹은 키가 없다면 자동 생성후 아이템 추가
	 * @param	MapName	맵 이름
	 * @param	KeyName	키 이름
	 * @param	Item	아이템
	**/
	void SetMapItemEx(const N& MapName, const K& KeyName, const T& Item);

protected:

	/**
	 * @fn				BOOL CreateMap_(const N& MapName)
	 * @brief			맵테이블에 새 맵 추가
	 * @param	MapName	생성할 맵 이름
	 * @return			FALSE: 생성 실패, 이미 존재
	**/
	BOOL CreateMap_(const N& MapName);

	/**
	 * @fn				BOOL AddMapItem_(map<K, T*>* pMap, const K& KeyName, const T& Item)
	 * @brief			맵에 새 아이템 추가
	 * @param	MapName	맵 이름
	 * @param	KeyName	키 이름
	 * @param	Item	아이템 이름
	 * @return			FALSE: 추가 실패, 이미 존재
	**/
	BOOL AddMapItem_(map<K, T*>* pMap, const K& KeyName, const T& Item);

	/**
	 * @fn				map<K, T*>* GetMap(const N& MapName)
	 * @brief			맵 객체 반환
	 * @param	MapName	반환할 맵 이름
	 * @return			NULL: 맵 객체 없음
	**/
	map<K, T*>* GetMap_(const N& MapName);

	/**
	 * @fn				T* GetMapItem(const N& MapName, const K& KeyName)
	 * @brief			맵 아이템 반환
	 * @param	MapName	반환할 맵 이름
	 * @param	KeyName	키 이름
	 * @return			NULL: 아이템 객체 없음
	**/
	T* GetMapItem_(const N& MapName, const K& KeyName);
	T* GetMapItem_(map<K, T*>* pMap, const K& KeyName);

	/**
	 * @fn					BOOL SetMapItem_(const N& MapName, const K& KeyName, const T& Item,
	 * 						BOOL bAutoCreate = TRUE, BOOL bOverWrite = TRUE)
	 * @brief				아이템 설정
	 * @param	MapName		맵 이름
	 * @param	KeyName		키 이름
	 * @param	Item		아이템 
	 * @param	bAutoCreate	맵 혹은 키가 없을 경우, 자동 생성
	 * @param	bOverWrite	존재할 경우, 덮어쓰기 여부
	 * @return				FALSE: 작업 실패
	**/
	BOOL SetMapItem_(const N& MapName, const K& KeyName, const T& Item, BOOL bAutoCreate = TRUE, BOOL bOverWrite = TRUE);

private:
	CRITICAL_SECTION* m_pCs;

	map<N, map<K, T*>* >* m_pMapTable;
};

template <class N, class K, class T>
void CMapTable<N, K, T>::Clear()
{
	map<N, map<K, T*>* >::iterator table_it;
	map<K, T* >* pMap = NULL;
	map<K, T* >::iterator map_it;
	T* pItem = NULL;

	ENTER_UDT_CS(m_pCs);

	for (table_it = m_pMapTable->begin(); table_it != m_pMapTable->end(); table_it++)
	{
		pMap = (*table_it).second;
		if (pMap)
		{
			for (map_it = pMap->begin(); map_it != pMap->end(); map_it++)
			{
				pItem =(*map_it).second;
				if (pItem)
					delete pItem;
			}
			delete pMap;
		}
	}
	m_pMapTable->clear();

	LEAVE_UDT_CS(m_pCs);
}

template <class N, class K, class T>
BOOL CMapTable<N, K, T>::CreateMap(const N& MapName)
{
	BOOL retVal = FALSE;
	ENTER_UDT_CS(m_pCs);

	retVal = CreateMap_(MapName);
	
	LEAVE_UDT_CS(m_pCs);
	return retVal;
}

template <class N, class K, class T>
BOOL CMapTable<N, K, T>::CreateMap_(const N& MapName)
{
	BOOL retVal = FALSE;
	map<K, T* >* pMap = new map<K, T* >();

	retVal = m_pMapTable->insert(make_pair(MapName, pMap)).second;

	if (!retVal)
		delete pMap;

	return retVal;
}

template <class N, class K, class T>
int CMapTable<N, K, T>::DeleteMap(const N& MapName)
{
	int nRemain = -1;
	map<K, T* >* pMap = NULL;

	ENTER_UDT_CS(m_pCs);
	
	pMap = GetMap_(MapName);
	if (pMap)
	{
		delete pMap;
		nRemain = m_pMapTable->erase(MapName);
	}
	
	LEAVE_UDT_CS(m_pCs);
	return nRemain;
}

template <class N, class K, class T>
void CMapTable<N, K, T>::ClearMap(const N& MapName)
{
	map<K, T* >* pMap = NULL;
	map<K, T* >::iterator map_it;
	T* pItem = NULL;

	ENTER_UDT_CS(m_pCs);
	
	pMap = GetMap_(MapName);
	if (pMap)
	{
		for (map_it = pMap->begin(); map_it != pMap->end(); map_it++)
		{
			pItem =(*map_it).second;
			if (pItem)
				delete pItem;
		}
		pMap->clear();
	}

	LEAVE_UDT_CS(m_pCs);
}

template <class N, class K, class T>
BOOL CMapTable<N, K, T>::IsExistMap(const N& MapName)
{
	map<K, T* >* pMap = NULL;

	ENTER_UDT_CS(m_pCs);
	
	pMap = GetMap_(MapName);

	LEAVE_UDT_CS(m_pCs);

	return (pMap ? TRUE : FALSE);
}

template <class N, class K, class T>
map<K, T*>* CMapTable<N, K, T>::GetMap_(const N& MapName)
{
	map<N, map<K, T*>* >::iterator table_it;
	map<K, T* >* pMap = NULL;

	table_it = m_pMapTable->find(MapName);
	if (table_it != m_pMapTable->end())
		pMap = (*table_it).second;

	return pMap;
}

template <class N, class K, class T>
BOOL CMapTable<N, K, T>::AddMapItem(const N& MapName, const K& KeyName, const T& Item)
{
	BOOL retVal = FALSE;
	map<K, T* >* pMap = NULL;

	ENTER_UDT_CS(m_pCs);
	
	pMap = GetMap_(MapName);
	if (pMap)
		retVal = AddMapItem_(pMap, KeyName, Item);

	LEAVE_UDT_CS(m_pCs);

	return retVal;
}

template <class N, class K, class T>
BOOL CMapTable<N, K, T>::AddMapItem_(map<K, T*>* pMap, const K& KeyName, const T& Item)
{
	BOOL retVal = FALSE;

	if (pMap)
	{
		T* pItem = new T(Item);
		retVal = pMap->insert(make_pair(KeyName, pItem)).second;
		if (!retVal)
			delete pItem;	
	}
	return retVal;
}

template <class N, class K, class T>
int CMapTable<N, K, T>::RemoveMapItem(const N& MapName, const K& KeyName)
{
	int nRemain = -1;
	map<K, T*>* pMap = NULL;
	T* pItem = NULL;

	ENTER_UDT_CS(m_pCs);
	
	pMap = GetMap_(MapName);
	if (pMap)
	{
		pItem = GetMapItem_(pMap, KeyName);
		if (pItem)
		{
			delete pItem;
			nRemain = pMap->erase(KeyName);
		}
	}
	
	LEAVE_UDT_CS(m_pCs);
	return nRemain;
}

template <class N, class K, class T>
BOOL CMapTable<N, K, T>::IsExistMapItem(const N& MapName, const K& KeyName)
{
	BOOL retVal = FALSE;
	T* pItem = NULL;

	ENTER_UDT_CS(m_pCs);
	
	pItem = GetMapItem_(MapName, KeyName);

	LEAVE_UDT_CS(m_pCs);

	return (pItem ? TRUE : FALSE);
}

template <class N, class K, class T>
BOOL CMapTable<N, K, T>::GetMapItem(const N& MapName, const K& KeyName, T& Item)
{
	BOOL retVal = FALSE;
	T* pItem = NULL;

	ENTER_UDT_CS(m_pCs);
	
	pItem = GetMapItem_(MapName, KeyName);
	if (pItem)
	{
		Item = *pItem;
		retVal = TRUE;
	}

	LEAVE_UDT_CS(m_pCs);
	return retVal;
}

template <class N, class K, class T>
BOOL CMapTable<N, K, T>::SetMapItem(const N& MapName, const K& KeyName, const T& Item)
{
	BOOL retVal = FALSE;

	ENTER_UDT_CS(m_pCs);
	
	retVal = SetMapItem_(MapName, KeyName, Item, FALSE, TRUE);
	
	LEAVE_UDT_CS(m_pCs);
	return retVal;
}

template <class N, class K, class T>
void CMapTable<N, K, T>::SetMapItemEx(const N& MapName, const K& KeyName, const T& Item)
{
	BOOL retVal = FALSE;

	ENTER_UDT_CS(m_pCs);
	
	retVal = SetMapItem_(MapName, KeyName, Item, TRUE, TRUE);
	
	LEAVE_UDT_CS(m_pCs);
}

template <class N, class K, class T>
T* CMapTable<N, K, T>::GetMapItem_(const N& MapName, const K& KeyName)
{
	map<K, T* >* pMap;
	map<K, T* >::iterator map_it;
	T* pItem = NULL;

	pMap = GetMap_(MapName);
	if (pMap)
	{
		map_it = pMap->find(KeyName);
		if (map_it != pMap->end())
			pItem = (*map_it).second;
	}

	return pItem;
}

template <class N, class K, class T>
T* CMapTable<N, K, T>::GetMapItem_(map<K, T*>* pMap, const K& KeyName)
{
	map<K, T* >::iterator map_it;
	T* pItem = NULL;

	if (pMap)
	{
		map_it = pMap->find(KeyName);
		if (map_it != pMap->end())
			pItem = (*map_it).second;
	}

	return pItem;
}

template <class N, class K, class T>
BOOL CMapTable<N, K, T>::SetMapItem_(const N& MapName, const K& KeyName, const T& Item, BOOL bAutoCreate /*= TRUE*/, BOOL bOverWrite /*= TRUE*/)
{
	BOOL retVal = FALSE;
	map<K, T*>* pMap = NULL;
	T* pItem = NULL;

	pMap = GetMap_(MapName);
	if (!pMap)
	{
		if (!bAutoCreate)
			return FALSE;
		
		CreateMap_(MapName);
		pMap = GetMap_(MapName);
	}
	
	pItem = GetMapItem_(pMap, KeyName);
	if (!pItem)
	{
		if (!bAutoCreate)
			return FALSE;
		
		retVal = AddMapItem_(pMap, KeyName, Item);
	}
	else if (bOverWrite)
	{
		(*pItem) = Item;
		retVal = TRUE;
	}
	else
		retVal = FALSE;

	return retVal;
}

/*
#include <UserDataType.h>
class CVar
{
public:
	CVar() {}
	CVar(const CVar& rhs)
	{
		m_nVal = rhs.m_nVal;
		m_sVal = rhs.m_sVal;
	}
	~CVar() {}

	CVar& operator = (const CVar& rhs)
	{
		if (this != &rhs)
		{
			m_nVal = rhs.m_nVal;
			m_sVal = rhs.m_sVal;
		}
		return *this;
	}

public:
	int m_nVal;
	tstring m_sVal;
};

int _tmain(int argc, _TCHAR* argv[])
{
	BOOL bSuc = FALSE;
	int nNum = 0;
	tstring mapName, keyName;
	CVar Var1, Var9;
	CMapTable<tstring, tstring, CVar> MapTable(TRUE);

	mapName = _T("Test_Map");
	bSuc = MapTable.CreateMap(mapName);
	
	keyName = _T("1.key1");
	Var1.m_nVal = 9;
	Var1.m_sVal = _T("aaa");
	bSuc = MapTable.AddMapItem(mapName, keyName, Var1);

	bSuc = MapTable.AddMapItem(mapName, keyName, Var1);

	keyName = _T("1.key2");
	Var1.m_nVal = 99;
	Var1.m_sVal = _T("bbb");
	bSuc = MapTable.AddMapItem(mapName, keyName, Var1);

	keyName = _T("1.key1");
	bSuc = MapTable.GetMapItem(mapName, keyName, Var9);

	keyName = _T("1.key1");
	nNum = MapTable.RemoveMapItem(mapName, keyName);
	

	keyName = _T("1.key1");
	Var1.m_nVal = 9;
	Var1.m_sVal = _T("aaa");
	bSuc = MapTable.AddMapItem(mapName, keyName, Var1);

	
	mapName = _T("Test_Map2");
	bSuc = MapTable.CreateMap(mapName);

	keyName = _T("2.key1");
	Var1.m_nVal = 9;
	Var1.m_sVal = _T("aaa");
	bSuc = MapTable.AddMapItem(mapName, keyName, Var1);
	
	bSuc = MapTable.IsExistMap(mapName);
	bSuc = MapTable.IsExistMapItem(mapName, keyName);

	MapTable.Clear();

	keyName = _T("1.key1");
	bSuc = MapTable.GetMapItem(mapName, keyName, Var9);

	return 0;
}
*/