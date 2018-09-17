/**
 * @file	IniFileMap.h
 * @brief	IniFile Ŭ�������� ��
 * @author	Dede
**/
#pragma once

#include "IniFile.h"
#include <map>
using namespace std;

typedef map<tstring, CIniFile* > MAP_INIFILEPTR;
typedef map<tstring, CIniFile* >::const_iterator MAP_INIFILEPTR_CIT;
typedef map<tstring, CIniFile* >::iterator MAP_INIFILEPTR_IT;

class CIniFileMap
{
public:
	CIniFileMap(void);
	~CIniFileMap(void);

	/**
	 * @brief				INI ������ �ε��ؼ� ���� �ʿ� �߰�
	 * @param	szName		INI������ ��ǥ�� �̸� (�ʿ��� ���� ��Ű �̸�)
	 * @param	szFilePath	INI ���� ��ü ���
	 * @return				FALSE: INI ���Ͽ� ������ ���ų�, �̹� �ʿ� ���� �̸��� �����ϴ� ���
	**/
	BOOL AddIniFile(LPCTSTR szName, LPCTSTR szFilePath);

	/**
	 * @brief				INI ������ �ε��ؼ� ���� �ʿ� �߰�
	 * @param	szName		INI������ ��ǥ�� �̸� (�ʿ��� ���� ��Ű �̸�)
	 * @param	filePaths	INI ���� ��ü ��ε��� ����Ʈ
	 * @return				FALSE: INI ���Ͽ� ������ ���ų�, �̹� �ʿ� ���� �̸��� �����ϴ� ���
	**/
	BOOL AddIniFiles(LPCTSTR szName, vector<tstring>& filePaths);

	/**
	 * @brief					Ű ���ڿ� ��ȯ 
	 * @param	szName			��Ű �̸�
	 * @param	szSectionName	���� �̸�
	 * @param	szKey			Ű �̸�
	 * @return					Ű ���� ��ȯ, ���ų� �����ϸ� "" �� ��ȯ
	**/
	tstring GetStrVal(LPCTSTR szName, LPCTSTR szSectionName, LPCTSTR szKey);

	/**
	 * @brief	IniFileMap �� ũ�� ��ȯ
	 * @return	The size. 
	**/
	int GetSize();
	/**
	 * @brief	�ش� �ʳ��� ���� ������ ��ȯ
	 * @return	The size. 
	**/
	int GetSectionSize(LPCTSTR szName);
	/**
	 * @brief	���ǳ��� Ű ������ ��ȯ
	 * @return	The size. 
	**/
	int GetKeySize(LPCTSTR szName, LPCTSTR szSectionName);

private:
	CRITICAL_SECTION* m_pCs;
	MAP_INIFILEPTR* m_pMap;
};
