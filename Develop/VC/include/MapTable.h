/**
 * @file		MapTable.h
 * @brief		���߸����� ������ �� ���̺� ���� Ŭ����
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
 * @brief	map<N, map<K, T*>* > �� �����ϴ� Ŭ����
			���� ������ �� �ʵ��� ��� Heap ���� ��(new/delete)
			������ ����ȭ ����

			N: �� �̸� Ÿ��, copy, =, == ������ ���� �ʿ�
			K: Ű �̸� Ÿ��, copy, =, == ������ ���� �ʿ�
			T: ������ Ÿ��, copy, = ������ ���� �ʿ�
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
	 * @brief	�����̺� ��ü�� Ŭ���� 
	**/
	void Clear();

	/**
	 * @fn				BOOL CreateMap(const N& MapName)
	 * @brief			�����̺� �� �� �߰�
	 * @param	MapName	������ �� �̸�
	 * @return			FALSE: ���� ����, �̹� ����
	**/
	BOOL CreateMap(const N& MapName);

	/**
	 * @fn				void DeleteMap(const N& MapName)
	 * @brief			�����̺��� �� ����
	 * @param	MapName	������ �� �̸�
	 * @return			������ ���� �� ����, -1: ���� ����
	**/
	int DeleteMap(const N& MapName);

	/**
	 * @fn				void ClearMap(const N& MapName)
	 * @brief			�ش� ���� Ŭ����
	 * @param	MapName	������ �� �̸�
	**/
	void ClearMap(const N& MapName);

	/**
	 * @fn				BOOL IsExistMap(const N& MapName)
	 * @brief			�����̺� �ش� ���� �����ϴ��� ����
	 * @param	MapName	�˻��� �� �̸� 
	 * @return			TRUE: ����, FALSE: �������� ����
	**/
	BOOL IsExistMap(const N& MapName);


	/**
	 * @fn				BOOL AddMapItem(const N& MapName, const K& KeyName, const T& Item)
	 * @brief			�ʿ� �� ������ �߰�
	 * @param	MapName	�� �̸�
	 * @param	KeyName	Ű �̸�
	 * @param	Item	������ �̸�
	 * @return			FALSE: �߰� ����, �̹� ����
	**/
	BOOL AddMapItem(const N& MapName, const K& KeyName, const T& Item);

	/**
	 * @fn				void RemoveMapItem(const N& MapName, const K& KeyName)
	 * @brief			�ʿ��� ������ ����
	 * @param	MapName	�� �̸�
	 * @param	KeyName	Ű �̸�
	 * @return			������ ���� Ű ����, -1: ���� ����
	**/
	int RemoveMapItem(const N& MapName, const K& KeyName);

	/**
	 * @fn				BOOL IsExistMapItem(const N& MapName, const K& KeyName)
	 * @brief			�ش� �ʿ� �ش� Ű���� �����ϴ� �� ���� ��ȯ
	 * @param	MapName	�� �̸�
	 * @param	KeyName	Ű �̸�
	 * @return			TRUE: ����, FALSE: �� Ȥ�� Ű�� �������� ����
	**/
	BOOL IsExistMapItem(const N& MapName, const K& KeyName);

	/**
	 * @fn						BOOL GetMapItem(const N& MapName, const K& KeyName, T& Item)
	 * @brief					������ ��ȯ
	 * @param	MapName			�� �̸�
	 * @param	KeyName			Ű �̸�
	 * @param [in,out]	Item	��ȯ�� ������
	 * @return					FALSE: ��ȯ����, �� Ȥ�� Ű�� �������� ����
	**/
	BOOL GetMapItem(const N& MapName, const K& KeyName, T& Item);

	/**
	 * @fn				void SetMapItem(const N& MapName, const K& KeyName, const T& Item)
	 * @brief			�ش� Ű�� ���� ����
	 * @param	MapName	�� �̸�
	 * @param	KeyName	Ű �̸�
	 * @param	Item	������
	 * @return			FALSE: ���� ����, �� Ȥ�� Ű�� �������� ����
	**/
	BOOL SetMapItem(const N& MapName, const K& KeyName, const T& Item);
	
	/**
	 * @fn				void SetMapItemEx(const N& MapName, const K& KeyName, const T& Item)
	 * @brief			�ش� Ű�� ���� ����, ���� �� Ȥ�� Ű�� ���ٸ� �ڵ� ������ ������ �߰�
	 * @param	MapName	�� �̸�
	 * @param	KeyName	Ű �̸�
	 * @param	Item	������
	**/
	void SetMapItemEx(const N& MapName, const K& KeyName, const T& Item);

protected:

	/**
	 * @fn				BOOL CreateMap_(const N& MapName)
	 * @brief			�����̺� �� �� �߰�
	 * @param	MapName	������ �� �̸�
	 * @return			FALSE: ���� ����, �̹� ����
	**/
	BOOL CreateMap_(const N& MapName);

	/**
	 * @fn				BOOL AddMapItem_(map<K, T*>* pMap, const K& KeyName, const T& Item)
	 * @brief			�ʿ� �� ������ �߰�
	 * @param	MapName	�� �̸�
	 * @param	KeyName	Ű �̸�
	 * @param	Item	������ �̸�
	 * @return			FALSE: �߰� ����, �̹� ����
	**/
	BOOL AddMapItem_(map<K, T*>* pMap, const K& KeyName, const T& Item);

	/**
	 * @fn				map<K, T*>* GetMap(const N& MapName)
	 * @brief			�� ��ü ��ȯ
	 * @param	MapName	��ȯ�� �� �̸�
	 * @return			NULL: �� ��ü ����
	**/
	map<K, T*>* GetMap_(const N& MapName);

	/**
	 * @fn				T* GetMapItem(const N& MapName, const K& KeyName)
	 * @brief			�� ������ ��ȯ
	 * @param	MapName	��ȯ�� �� �̸�
	 * @param	KeyName	Ű �̸�
	 * @return			NULL: ������ ��ü ����
	**/
	T* GetMapItem_(const N& MapName, const K& KeyName);
	T* GetMapItem_(map<K, T*>* pMap, const K& KeyName);

	/**
	 * @fn					BOOL SetMapItem_(const N& MapName, const K& KeyName, const T& Item,
	 * 						BOOL bAutoCreate = TRUE, BOOL bOverWrite = TRUE)
	 * @brief				������ ����
	 * @param	MapName		�� �̸�
	 * @param	KeyName		Ű �̸�
	 * @param	Item		������ 
	 * @param	bAutoCreate	�� Ȥ�� Ű�� ���� ���, �ڵ� ����
	 * @param	bOverWrite	������ ���, ����� ����
	 * @return				FALSE: �۾� ����
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