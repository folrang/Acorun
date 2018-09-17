/**
 * @file	IniFileMap.h
 * @brief	IniFile 클래스들의 맵
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
	 * @brief				INI 파일을 로딩해서 내부 맵에 추가
	 * @param	szName		INI파일을 대표할 이름 (맵에서 사용될 맵키 이름)
	 * @param	szFilePath	INI 파일 전체 경로
	 * @return				FALSE: INI 파일에 섹션이 없거나, 이미 맵에 동일 이름이 존재하는 경우
	**/
	BOOL AddIniFile(LPCTSTR szName, LPCTSTR szFilePath);

	/**
	 * @brief				INI 파일을 로딩해서 내부 맵에 추가
	 * @param	szName		INI파일을 대표할 이름 (맵에서 사용될 맵키 이름)
	 * @param	filePaths	INI 파일 전체 경로들의 리스트
	 * @return				FALSE: INI 파일에 섹션이 없거나, 이미 맵에 동일 이름이 존재하는 경우
	**/
	BOOL AddIniFiles(LPCTSTR szName, vector<tstring>& filePaths);

	/**
	 * @brief					키 문자열 반환 
	 * @param	szName			맴키 이름
	 * @param	szSectionName	섹션 이름
	 * @param	szKey			키 이름
	 * @return					키 값을 반환, 없거나 실패하면 "" 를 반환
	**/
	tstring GetStrVal(LPCTSTR szName, LPCTSTR szSectionName, LPCTSTR szKey);

	/**
	 * @brief	IniFileMap 의 크기 반환
	 * @return	The size. 
	**/
	int GetSize();
	/**
	 * @brief	해당 맵내의 세션 개수를 반환
	 * @return	The size. 
	**/
	int GetSectionSize(LPCTSTR szName);
	/**
	 * @brief	세션내의 키 개수를 반환
	 * @return	The size. 
	**/
	int GetKeySize(LPCTSTR szName, LPCTSTR szSectionName);

private:
	CRITICAL_SECTION* m_pCs;
	MAP_INIFILEPTR* m_pMap;
};
