// SHeapBuf_Test.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <SHeapBuf.h>
using namespace Network;

#include "MyA.h"


int _tmain(int argc, _TCHAR* argv[])
{
	/*
	BOOL bSucc;
	DWORD dwNum;
	char szStr[1024], szStr2[1024];
	strcpy_s(szStr, _countof(szStr), "");
	strcpy_s(szStr2, _countof(szStr), "");



	LPVOID pPtr = NULL;
	CNetBuf::RET_CODE retCode;
	CSHeapBufMan BufMan(4, DEFAULT_MAX_FREELIST_SIZE);


	CSHeapBuf* pBuf = dynamic_cast<CSHeapBuf*>(BufMan.AllocBuf());

	strcpy_s(szStr, _countof(szStr), "01234");
	retCode = pBuf->AddData(szStr, strlen(szStr));
	dwNum = pBuf->GetDataSize();

	strcpy_s(szStr, _countof(szStr), "ABC");
	retCode = pBuf->AddData(szStr, strlen(szStr));
	dwNum = pBuf->GetDataSize();

	strcpy_s(szStr, _countof(szStr), "가나다D");
	retCode = pBuf->AddData(szStr, strlen(szStr));
	dwNum = pBuf->GetDataSize();

	bSucc = pBuf->GetData(szStr2, 2,  -1);
	szStr2[dwNum - 2] = '\0';
	dwNum = pBuf->GetDataSize();

	pPtr = pBuf->GetDataPtr();
	
	pBuf->ResetDataSize(2, 4);
	dwNum = pBuf->GetDataSize();

	dwNum = pBuf->GetAvailMemSize();
	pPtr = pBuf->GetAvailMemPtr(16);
	dwNum = pBuf->GetDataSize();


	BufMan.DeallocBuf(pBuf);


	BufMan.Cleanup();

	*/

	CMyB MyB;
	MyB.Write("%s %d %u ABC", "우리집", 555, 1111);

	return 0;
}

