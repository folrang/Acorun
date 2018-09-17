/**
 * @file		VarMapTable.h
 * @brief		App 사용된 맵 테이블
 * @author		Dede
**/
#pragma once

#include <MapTable.h>
#include <comdef.h>	// _variant_t

#define VARMAP_NAME_DEBUG	_T("Debug")

#define VARMAP_NAME_APPCFG	_T("AppCfg")


typedef CMapTable<tstring, tstring, _variant_t> CVarMapTable;

