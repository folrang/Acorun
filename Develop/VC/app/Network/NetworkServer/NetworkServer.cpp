// NetworkServer.cpp : �ܼ� ���� ���α׷��� ���� �������� �����մϴ�.
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
	MessageBox(NULL, _T("�۾��� ���� ��ų ����?"), _T("�۾� ����"), MB_ICONSTOP | MB_OK);
	
	SetEvent(g_hExitEvt);

	return 0;
}

void Run()
{
	// 0. �ʱ� ����
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


	// 1. ����� Buffer Manager �� ����
	CSHeapBufMan SHeapBufMan(DEFAULT_INIT_MEM_SIZE, DEFAULT_MAX_FREELIST_SIZE);

	// 2. IOCP Data Manager �� ����
	CNetIoDataMan IoDataMan(&SHeapBufMan);
	IoDataMan.SetMaxFreeListSize(10);

	// 3. CTcpServerCfg ����
	CTcpListenCtxMan ListenCtxMan;
	CTcpClientCtxMan ClientCtxMan;
	
	CTcpServerCfg Cfg(g_hExitEvt, &ListenCtxMan, &ClientCtxMan, &IoDataMan);
	
	// 4. CTcpServerCfg �� ����

	// ���� ���� IP ���� ����
	Cfg.AddBindInfo(ai, DEFAULT_BACKLOG);

	// Completion �������� �ּ�/�ִ� ���� ����
	Cfg.SetCompletionThread(DEFAULT_MIN_COMPLETION_THREAD, DEFAULT_MAX_COMPLETION_THREAD);

	// Ctx �޸� ���� ����
	Cfg.SetNetCtxInfo(10, 10);
	

	// TCP ���� ���� ����
	// Cfg.SetPendingAccepts(2, 10);
	// Cfg.SetForceCloseInfo()
	// SetAcceptExBufSize
	


	// 5. CTcpServer ���� �� ����
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

	// 9. ��
}