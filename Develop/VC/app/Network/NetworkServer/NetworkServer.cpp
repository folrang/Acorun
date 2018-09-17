// NetworkServer.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include "EcoServer.h"
#include <network\SHeapBuf.h>
#include <network\IpAddrResolver.h>

using namespace Network;
using namespace NetServer;


HANDLE g_hExitEvt = NULL;

void Run();
unsigned __stdcall WaitUserExitThread(LPVOID lpvThreadParam);

int _tmain(int argc, _TCHAR* argv[])
{
	g_hExitEvt = CreateEvent(NULL, TRUE, FALSE, NULL);

	unsigned nThreadId;
	HANDLE hWorkThread = (HANDLE)_beginthreadex(NULL, 0, WaitUserExitThread, NULL, 0, &nThreadId);

	/////////
	
	Run();

	/////////

	WaitForSingleObject(hWorkThread, INFINITE);
	CloseHandle(hWorkThread);

	CloseHandle(g_hExitEvt);
	return 0;
}

unsigned __stdcall WaitUserExitThread(LPVOID lpvThreadParam)
{
	MessageBox(NULL, _T("작업을 중지 시킬 가요?"), _T("작업 중지"), MB_ICONSTOP | MB_OK);
	
	SetEvent(g_hExitEvt);

	return 0;
}

void Run()
{
	// 0. 초기 셋팅
	char szIp[16], szPort[8];
	strcpy(szIp, "116.126.102.90");
	strcpy(szPort, "8088");

	addrinfo ai;
	CIpAddrResolver IpAddr;
	if (IpAddr.GetFirstAddrInfo(szIp, szPort, &ai) == CIpAddrResolver::RET_ERR_FAIL)
	{
		_tprintf(_T("Critical Fail, Incorrect IP"));
		return;
	}


	// 1. 사용할 Buffer Manager 를 생성
	CSHeapBufMan SHeapBufMan(DEFAULT_INIT_MEM_SIZE, DEFAULT_MAX_FREELIST_SIZE);

	// 2. IOCP Data Manager 를 생성
	CNetIoDataMan IoDataMan(&SHeapBufMan);
	IoDataMan.SetMaxFreeListSize(10);

	// 3. CTcpServerCfg 생성
	CTcpListenCtxMan ListenCtxMan;
	CTcpClientCtxMan ClientCtxMan;
	
	CTcpServerCfg Cfg(g_hExitEvt, &ListenCtxMan, &ClientCtxMan, &IoDataMan);
	
	// 4. CTcpServerCfg 값 셋팅

	// 서버 리슨 IP 정보 설정
	Cfg.AddBindInfo(ai, DEFAULT_BACKLOG);

	// Completion 스레드의 최소/최대 개수 지정
	Cfg.SetCompletionThread(DEFAULT_MIN_COMPLETION_THREAD, DEFAULT_MAX_COMPLETION_THREAD);

	// Ctx 메모리 관리 설정
	Cfg.SetNetCtxInfo(10, 10);
	

	// TCP 전용 값들 설정
	// Cfg.SetPendingAccepts(2, 10);
	// Cfg.SetForceCloseInfo()
	// SetAcceptExBufSize
	


	// 5. CTcpServer 생성 및 실행
	CTcpServerStat Stat;
	CConsoleTrace Trace(TRUE);

	CNetServer::RET_CODE retCode;
	CTcpServer Server(&Cfg, &Stat, &Trace);
	retCode = Server.Initialize();
	if (retCode == CNetServer::E_SUCCESS)
	{
		Server.Uninitialize();
	}
	else
	{
		_tprintf(_T("Fail Initialize(), %d"), retCode);
	}

	// 9. 끝
}