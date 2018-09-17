#pragma once

#include "stdafx.h"

class CMyA
{
public:
	CMyA() {}
	virtual ~CMyA() {}

	virtual void Write(char* szFormat, ...)
	{
		va_list vl;
		char dbgbuf[2048];

		va_start(vl, szFormat);
		wvsprintf(dbgbuf, szFormat, vl);
		va_end(vl);

		printf("CMyA, ");
		printf(dbgbuf);
		printf("\n");
	}
};

class CMyB : public CMyA
{
public:
	CMyB() {}
	virtual ~CMyB() {}

	virtual void Write(char* szFormat, ...)
	{
		va_list vl;
		char dbgbuf[2048];

		va_start(vl, szFormat);
		wvsprintf(dbgbuf, szFormat, vl);
		va_end(vl);

		printf("CMyB, ");
		printf(dbgbuf);
		printf("\n");
	}
};

