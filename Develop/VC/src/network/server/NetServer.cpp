#include "StdAfx.h"
#include <network/server/NetServer.h>
#include <time.h>


GUID g_guidAcceptEx = WSAID_ACCEPTEX;
GUID g_guidGetAcceptExSockaddrs = WSAID_GETACCEPTEXSOCKADDRS;


//////////////////////////////////////////////////////////
//			CNetServer
//////////////////////////////////////////////////////////
CNetServer::CNetServer(CNetServerCfg* pCfg, CNetServerStat* pStat, CNetTrace* pTrace)
{
	m_pCfg = pCfg;
	m_pStat = pStat;
	m_pTrace = pTrace;
	
	m_hPort = NULL;
	m_hControlThread = NULL;
}

CNetServer::~CNetServer(void)
{
}

CNetServer::RET_CODE CNetServer::Initialize()
{	
	CNetServer::RET_CODE retVal = CNetServer::E_UNKNOWN;

	int i = 0;
	unsigned nThreadId = 0;
	HANDLE hThread = NULL;
	WSADATA wsd;
	
	CNetClientCtxMan* pNetClientCtxMan = m_pCfg->GetClientCtxMan();
	CNetIoDataMan* pNetIoDataMan = m_pCfg->GetIoDataMan();
	CNetListenCtxMan* pListenCtxMan = m_pCfg->GetListenCtxMan();
	
	// 0. ���� �ʱ�ȭ
	int nNetClientCtxFreeListSize = 0, nNetIoDataFreeListSize = 0;
	m_pCfg->GetNetCtxInfo(&nNetClientCtxFreeListSize, &nNetIoDataFreeListSize);

	pNetClientCtxMan->SetMaxFreeListSize(nNetClientCtxFreeListSize);
	pNetIoDataMan->SetMaxFreeListSize(nNetIoDataFreeListSize);

	// 1. ���� �ʱ�ȭ
	if (WSAStartup(MAKEWORD(2,2), &wsd) != 0)
		return CNetServer::E_FAIL_INIT_API;	

	// 2. IOCP ����
	// dwNumberOfConcurrentThreads�� ���� �̻����� 0������ ����
	m_hPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	if (m_hPort == NULL)
		return CNetServer::E_FAIL_INIT_API;

	// 3. Completion ������ ���� �� ����
	int nMinCompletionThread = m_pCfg->GetMinCompletionThread();

	for (i = 0; i < nMinCompletionThread; i++)
	{
		hThread = (HANDLE)_beginthreadex(NULL, 0, CompletionThread, (LPVOID)this, 0, &nThreadId);
		CloseHandle(hThread);
	}

	// 4. bind �� listen ����
	CNetListenCtx* pListenCtx = NULL;
	HANDLE hBindPort = NULL;
	int nRetCode = 0, nBacklog = 0;
	const LST_BIND_INFO& bindInfoList = m_pCfg->GetBindInfo();

	pListenCtxMan->SetMaxNetListenListSize((int)bindInfoList.size());
	
	for (LST_BIND_INFO_CIT cit = bindInfoList.begin(); cit != bindInfoList.end(); ++cit)
	{
		// ���ο� ListenCtx ��ü ����
		pListenCtx = pListenCtxMan->CreateCtx();
		pListenCtxMan->AddNetListenCtx(pListenCtx);
		
		SOCKET& s = pListenCtx->m_Socket;
		const addrinfo& ai = (*cit).ai;

		nBacklog = (*cit).backlog;
		memcpy(&pListenCtx->m_AddrInfo, &ai, sizeof(addrinfo));	

		s = socket(ai.ai_family, ai.ai_socktype, ai.ai_protocol);
		if (s == INVALID_SOCKET)
			return CNetServer::E_CREATE_LISTEN;
		
		// ���� Ȯ�� �Լ������͸� Provider�� �����ϴ� ���Ͽ� ����
		if (LoadWinSockExFunc(pListenCtx) == FALSE)
			return CNetServer::E_FAIL_INIT_API; 

		hBindPort = CreateIoCompletionPort((HANDLE)s, m_hPort, (ULONG_PTR)pListenCtx, 0);
        if (hBindPort != m_hPort)
			return CNetServer::E_CREATE_LISTEN;

		nRetCode = bind(s, ai.ai_addr, ai.ai_addrlen);
		if (nRetCode == SOCKET_ERROR)
			return CNetServer::E_CREATE_LISTEN;

		nRetCode = listen(s, nBacklog);
		if (nRetCode == SOCKET_ERROR)
			return CNetServer::E_CREATE_LISTEN;
		
		// ������ �۾� ����
		retVal = StartListen(pListenCtx);
		if (retVal != E_SUCCESS)
		{
			m_pTrace->WriteErrorMsg(_T("Initialize: StartListen failed: %d\n"), retVal);
			return retVal;
		}
	}


	// Control ������ ����
	m_hControlThread = (HANDLE)_beginthreadex(NULL, 0, ControlThread, (LPVOID)this, 0, &nThreadId);

	return CNetServer::E_SUCCESS;
}

void CNetServer::Uninitialize()
{
	int i = 0;

	// Control ������ ���� Ȯ��
	if (m_hControlThread) 
	{
		WaitForSingleObject(m_hControlThread, INFINITE);
		CloseHandle(m_hControlThread);
		m_hControlThread = NULL;
	}
	
	// Completion ������ ���� Ȯ��
	while (1)
	{
		if (m_pStat->GetCurrentCompletionCount() > 0)
		{
			Sleep(500);
			continue;
		}
		break;
	}
	
	// ���� Context�� ���� (ListenCtx ������ ClientCtx �����ؾ� ��)
	CNetListenCtxMan* pListenMan = m_pCfg->GetListenCtxMan();
	CNetClientCtxMan* pClientMan = m_pCfg->GetClientCtxMan();
	CNetIoDataMan* pIoDataMan = m_pCfg->GetIoDataMan();
	pListenMan->Cleanup();
	pClientMan->Cleanup();
	pIoDataMan->Cleanup();
	
	// ���̺귯�� ����
	CloseHandle(m_hPort);
	WSACleanup();
}

// Control ������
unsigned __stdcall CNetServer::ControlThread(LPVOID lpvThreadParam)
{
	CNetServer* pThis = (CNetServer*)lpvThreadParam;
	return pThis->ControlThreadMain(pThis);
}


// Completion ������
unsigned __stdcall CNetServer::CompletionThread(LPVOID lpvThreadParam)
{
	CNetServer* pThis = (CNetServer*)lpvThreadParam;
	CNetServerStat* pStat = pThis->m_pStat;
	CNetServerCfg* pCfg = pThis->m_pCfg;
	CNetTrace* pTrace = pThis->m_pTrace;

	CTcpServerCfg* pTcpCfg = NULL;
	CTcpServerStat* pTcpStat= NULL;

	if (pThis->m_Protocol == CNetServer::TCP)
	{
		pTcpCfg = (CTcpServerCfg*)pCfg;
		pTcpStat = (CTcpServerStat*)pStat;
	}

	HANDLE hComletionPort = pThis->m_hPort;
	int nErrorCode = 0;
	ULONG_PTR lpKey = NULL;
	OVERLAPPED *lpOl = NULL;
	DWORD dwBytesTransferred = 0, dwFlags = 0;
	CNetServer::RET_CODE rc = CNetServer::E_UNKNOWN;

	CNetIoData* pIoData = NULL;
	CNetCtx* pCtx = NULL;
	CTcpListenCtx* pTcpListenCtx = NULL;
	CTcpClientCtx* pTcpCtx = NULL;
	
	HANDLE hBindPort;
	SOCKADDR_STORAGE *pLocalSockaddr = NULL, *pRemoteSockaddr = NULL;
	int nLocalSockaddr = 0, nRemoteSockaddr = 0;
	char szClientIp[NI_MAXHOST], szClientPort[NI_MAXSERV];


	while (1)
	{
		// UDP ���
		//			lpKey: CUdpListenCtx
		//			lpOl: CNetIoData, CNetIoData->m_pNetCtx�� �̿��Ͽ� CUdpListenCtx ���� ����
		// TCP ���
		//		1) Accept Op ��Ȳ
		//			lpKey: CTcpListenCtx
		//			lpOl: CNetIoData, CNetIoData->m_pNetCtx�� �̿��Ͽ� CTcpListenCtx ���� ����
		//		2) Recv/Send Op ��Ȳ
		//			lpKey: CTcpClientCtx
		//			lpOl: CNetIoData, CNetIoData->m_pNetCtx�� �̿��Ͽ� CTcpClientCtx ���� ����
		//
		nErrorCode = GetQueuedCompletionStatus(
						hComletionPort,
						&dwBytesTransferred,
						(PULONG_PTR)&lpKey,
						&lpOl,
						INFINITE
						);
		
		// completion ������ ���� ��û
		if (lpKey == 0)
		{
			pTrace->WriteDebugMsg(_T("CompletionThread: Exit Evt\n"));
			return 0;
		}
		
		// Overlapped ����Ÿ ��ȯ
		pIoData = CONTAINING_RECORD(lpOl, CNetIoData, m_Ol);
		pCtx = pIoData->m_pNetCtx;

		if (nErrorCode == FALSE)
		{
			// WSAGetOverlappedResult ȣ���� �̿��Ͽ� ���� ���� �ڵ�� ��ȯ
			pTrace->WriteDebugMsg(_T("CompletionThread: GetQueuedCompletionStatus failed: %d [0x%x]\n"), GetLastError(), pIoData->m_Ol.Internal);

			nErrorCode = WSAGetOverlappedResult(
							pCtx->m_Socket,
							&pIoData->m_Ol,
							&dwBytesTransferred,
							FALSE,
							&dwFlags
							);
            if (nErrorCode == FALSE)
            {
				// IO �۾� ���� �߻�
                nErrorCode = WSAGetLastError();
				
				if (nErrorCode == WSAEWOULDBLOCK)
				{
					if (pIoData->m_Op == CNetIoData::OP_ACCEPT)
					{
						// �۾� ����, overlapped i/o ���۰� 0�� ���, ���� ���� �����͸� �ٽ� �޾ƾ� ��
						pThis->CompletionSuccessRecv(pThis, pCtx, pIoData, dwBytesTransferred, nErrorCode);
						continue;
					}
				}

				// locked page ����
				if (nErrorCode == WSAENOBUFS)
				{
					// locked page ����
					// IO �۾��� ���̰ų�, ���ʿ��� ������ �ݾ�� �� ��Ȳ.
					pTrace->WriteErrorMsg(_T("CompletionThread: locked page error: WSAENOBUFS\n"));
				}

				// I/O �۾� ������, 
				//		TCP�� ���: ���� ���� ó��
				//		UDP�� ���: ���Ǿ���, ���� �۾� �� ��û Ȥ�� ����� ���� �۾� ����
				if (pThis->m_Protocol == CNetServer::TCP)
				{
					pThis->CloseOp(pThis, pIoData, TRUE);
				}
            }

			continue;
		}
		

		if (pIoData->m_Op == CNetIoData::OP_ACCEPT)
		{
			// TCP, Accept �۾� ����
			pTcpListenCtx = (CTcpListenCtx*)lpKey;
			pTcpCtx = (CTcpClientCtx*)pCtx;

			// ����ctx�� PendingAccept ��Ͽ��� ����
			pTcpListenCtx->RemovePendingAccept(pIoData);
			
			// GetAcceptExSockaddrs() �Լ��� �̿��Ͽ� Ŭ���̾�Ʈ �ּ� ����
			pTcpListenCtx->GETACCEPTEXSOCKADDRS(
								pIoData->m_pBuf->GetAvailMemPtr(pTcpCfg->GetAcceptExBufSize()),
								pIoData->m_pBuf->GetAvailMemSize() - ((sizeof(SOCKADDR_STORAGE) + 16) * 2),
								sizeof(SOCKADDR_STORAGE) + 16,
								sizeof(SOCKADDR_STORAGE) + 16,
								(SOCKADDR **)&pLocalSockaddr,
								&nLocalSockaddr,
								(SOCKADDR **)&pRemoteSockaddr,
								&nRemoteSockaddr
								);
			
			szClientIp[0] = szClientPort[0] = '\0';
			if (CIpAddrResolver::GetNameInfo((sockaddr*)pRemoteSockaddr, nRemoteSockaddr, szClientIp, NI_MAXHOST, szClientPort, NI_MAXSERV) == FALSE)
			{
				// Ŭ���̾�Ʈ IP ���� ����, ���� ����
				pTrace->WriteErrorMsg(_T("CompletionThread: GetNameInfo failed\n"));
				pThis->CloseOp(pThis, pIoData, TRUE);
				continue;
			}
			pTcpCtx->m_Ip.assign(szClientIp);
			
			// Ŭ���̾�Ʈ IP ���� ���� ���� Ȯ��
			if (pThis->IsAcceptClient(pCfg, pTcpCtx->m_Ip) == FALSE)
			{
				// ���� ���� �� ���� ����
				pTrace->WriteDebugMsg(_T("CompletionThread: IsAcceptClient returned FALSE (%s)\n"), pTcpCtx->m_Ip.c_str());
				pThis->CloseOp(pThis, pIoData, TRUE);
				continue;
			}

			// client socket�� iocp�� �߰�
			hBindPort = CreateIoCompletionPort(
								(HANDLE)pTcpCtx->m_Socket,
								hComletionPort,
								(ULONG_PTR)pTcpCtx,
								0
								);
            if (hBindPort == NULL)
            {
                pTrace->WriteErrorMsg(_T("CompletionThread: CreateIoCompletionPort failed: %d\n"), GetLastError());
                pThis->CloseOp(pThis, pIoData, TRUE);
				continue;
            }
			
			// ���� �۾� ����
			pThis->CompletionSuccessRecv(pThis, pCtx, pIoData, dwBytesTransferred, 0);

			// AcceptEx �۾� �Ϸ�
			continue;
		}
		else if (pIoData->m_Op == CNetIoData::OP_WRITE)
		{
			// ���� �۾� ����
			pStat->AddSentBytes(dwBytesTransferred);
			
			pThis->CloseOp(pThis, pIoData);	// OV operation �Ϸ�

			pThis->CompSendOp(pThis, pCtx, pIoData, 0);
			
			// PostSendOp �۾� �Ϸ�
			continue;
		}
		else
		{
			// ���� �۾� ����
			rc = pThis->CompletionSuccessRecv(pThis, pCtx, pIoData, dwBytesTransferred, 0);
		}

	}	// while(1)
	
	return 0;
}

// Completion thread���� ������ Recv �۾��� ó�� ����
CNetServer::RET_CODE CNetServer::CompletionSuccessRecv(CNetServer* pThis, CNetCtx* pCtx, CNetIoData* pIoData, DWORD dwBytesTransferred, int nWsaLastError)
{
	CNetServerCfg* pCfg = (CNetServerCfg*)pThis->m_pCfg;
	CNetServerStat* pStat = (CNetServerStat*)pThis->m_pStat;
	CNetBuf* pBuf = pIoData->m_pBuf;
	
	pStat->AddRecvBytes(dwBytesTransferred);	// ��� ���� ����
	pBuf->ResetDataSize(pBuf->GetDataSize(), dwBytesTransferred);	// ���� ������ ������ ���� ����
	pThis->CloseOp(pThis, pIoData);	// OV operation �Ϸ�
	return pThis->CompRecvOp(pThis, pCtx, pIoData, 0);	// ����� �Լ� ȣ��
}


//////////////////////////////////////////////////////////
//			CTcpServer
//////////////////////////////////////////////////////////
CTcpServer::CTcpServer(CTcpServerCfg* pCfg, CTcpServerStat* pStat, CNetTrace* pTrace) : CNetServer(pCfg, pStat, pTrace)
{
	m_Protocol = CNetServer::TCP;
}

CTcpServer::~CTcpServer()
{
}

// TCP�� MS ���� Ȯ�� �Լ��� �ε�
// [in] pCtx: �ε��� Context(����)
// [retVal] FALSE: �ε� ����
BOOL CTcpServer::LoadWinSockExFunc(CNetListenCtx* pListenCtx)
{
	CTcpListenCtx* pTcpListenCtx = (CTcpListenCtx*)pListenCtx;

	DWORD dwBytes = 0;
	int nRetCode = 0;

	nRetCode = WSAIoctl(
					pTcpListenCtx->m_Socket,
					SIO_GET_EXTENSION_FUNCTION_POINTER,
					&g_guidAcceptEx,
					sizeof(g_guidAcceptEx),
					&pTcpListenCtx->ACCEPTEX,
					sizeof(pTcpListenCtx->ACCEPTEX),
					&dwBytes,
					NULL,
					NULL
					);
	if (nRetCode == SOCKET_ERROR)
	{
		m_pTrace->WriteErrorMsg(_T("WSAIoctl: SIO_GET_EXTENSION_FUNCTION_POINTER failed: %d\n"), WSAGetLastError());
		return FALSE;
	}

	nRetCode = WSAIoctl(
					pTcpListenCtx->m_Socket,
					SIO_GET_EXTENSION_FUNCTION_POINTER,
					&g_guidGetAcceptExSockaddrs,
					sizeof(g_guidGetAcceptExSockaddrs),
					&pTcpListenCtx->GETACCEPTEXSOCKADDRS,
					sizeof(pTcpListenCtx->GETACCEPTEXSOCKADDRS),
					&dwBytes,
					NULL,
					NULL
					);
	if (nRetCode == SOCKET_ERROR)
	{
		m_pTrace->WriteErrorMsg(_T("WSAIoctl: SIO_GET_EXTENSION_FUNCTION_POINTER faled: %d\n"), WSAGetLastError());
		return FALSE;
	}

	return TRUE;
}

// ���Ϲ��ε� �� ���� ���� �۾�
// [in] pCtx: ������ ������ Context
CNetServer::RET_CODE CTcpServer::StartListen(CNetListenCtx* pListenCtx)
{
	CTcpServer* pTcpServer = (CTcpServer*)this;
	CTcpListenCtx* pTcpListenCtx = (CTcpListenCtx*)pListenCtx;	
	CTcpServerCfg* pTcpCfg = (CTcpServerCfg*)m_pCfg;
	CNetServer::RET_CODE rc = CNetServer::E_UNKNOWN;
	
	// TCP �������Ͽ� FD_ACCEPT �˸� �̺�Ʈ ���
	int nRetCode = WSAEventSelect(pTcpListenCtx->m_Socket, pTcpListenCtx->m_hFdAcceptEvt, FD_ACCEPT);
	if (nRetCode == SOCKET_ERROR)
	{
		m_pTrace->WriteErrorMsg(_T("WSAEventSelect failed: %d\n"), WSAGetLastError());
		return CNetServer::E_FAIL_INIT_API;
	}
	
	// ���� ���Ͽ��� PostAcceptOp ����	
	for (int i = 0; i < pTcpCfg->GetMinPendingAccepts(); i++)
	{
		rc = PostAcceptOp(pTcpServer, pTcpListenCtx);
		if (rc != CNetServer::E_SUCCESS)
			m_pTrace->WriteErrorMsg(_T("StartListen: PostAcceptOp failed: %d\n"), rc);
	}

	return CNetServer::E_SUCCESS;
}

// TcpClientCtx �� NetIoData �� ������ ��, Accept ��� ����
CNetServer::RET_CODE CTcpServer::PostAcceptOp(CTcpServer* pTcpThis, CTcpListenCtx* pTcpListenCtx)
{
	CNetServer::RET_CODE retVal = CNetServer::E_UNKNOWN;
	CTcpServerCfg* pTcpCfg = (CTcpServerCfg*)pTcpThis->m_pCfg;
	CTcpServerStat* pTcpStat = (CTcpServerStat*)pTcpThis->m_pStat;
	CTcpClientCtxMan* pTcpCtxMan = (CTcpClientCtxMan*)pTcpCfg->GetClientCtxMan();
	CNetIoDataMan* pIoDataMan = pTcpCfg->GetIoDataMan();

	CTcpClientCtx* pTcpCtx = NULL;
	CNetIoData* pIoData = NULL;
	
	// ClientCtx �� NetIoData ����
	pTcpCtx = (CTcpClientCtx*)pTcpCtxMan->AllocCtx();
	if (pTcpCtx == NULL)
	{
		m_pTrace->WriteErrorMsg(_T("PostAcceptOp: CTcpClientCtxMan::AllocCtx() failed\n"));
		return CNetServer::E_FAIL_ALLOC_CTX;
	}
	pTcpCtx->m_pListenCtx = pTcpListenCtx;	// Ŭ���̾�Ʈ contxt�� ������ ��ü ����

	pIoData = pIoDataMan->AllocData();
	if (pIoData == NULL)
	{
		pTcpCtxMan->DeallocCtx(pTcpCtx);

		m_pTrace->WriteErrorMsg(_T("PostAcceptOp: CNetIoDataMan::AllocData() failed\n"));
		return CNetServer::E_FAIL_ALLOC_IODATA;
	}
	pIoData->m_pNetCtx = pTcpCtx;	// IO data�� Ŭ���̾�Ʈ context ������ ����


	// AcceptEx ����
	int nRetCode = 0;
	DWORD dwRecvBytes = 0;

	// Accept ���� Client ������ �̸� ����
	pIoData->m_Op = CNetIoData::OP_ACCEPT;
	pTcpCtx->m_Socket = socket(pTcpListenCtx->m_AddrInfo.ai_family, SOCK_STREAM, IPPROTO_TCP);
	
	if (pTcpCtx->m_Socket == INVALID_SOCKET)
	{
		pTcpCtxMan->DeallocCtx(pTcpCtx);
		pIoDataMan->DeallocData(pIoData);

		m_pTrace->WriteErrorMsg(_T("PostAcceptOp: socket failed: %d\n"), WSAGetLastError());
		return CNetServer::E_INVALID_SOCKET;
	}

	nRetCode = pTcpListenCtx->ACCEPTEX(
								pTcpListenCtx->m_Socket, 
								pTcpCtx->m_Socket,
								pIoData->m_pBuf->GetAvailMemPtr(pTcpCfg->GetAcceptExBufSize()),	// Page(4KByte) ũ�� ���
								pIoData->m_pBuf->GetAvailMemSize() - ((sizeof(SOCKADDR_STORAGE) + 16) * 2),
								sizeof(SOCKADDR_STORAGE) + 16,
								sizeof(SOCKADDR_STORAGE) + 16,
								&dwRecvBytes,
								&pIoData->m_Ol
								);
	if (nRetCode == FALSE)
	{
		if (WSAGetLastError() != WSA_IO_PENDING)
        {
			pTcpCtxMan->DeallocCtx(pTcpCtx);
			pIoDataMan->DeallocData(pIoData);

            m_pTrace->WriteDebugMsg(_T("PostAcceptOp: AcceptEx failed: %d\n"), WSAGetLastError());
            return CNetServer::E_FAIL_POST_ACCEPT;
        }
	}

	// AccepEx ��� ����
	pTcpListenCtx->AddPendingAccept(pIoData);
	pTcpStat->IncrementPendingAcceptCount();

	return CNetServer::E_SUCCESS;
}

// Control �������� ���� �Լ�
unsigned CTcpServer::ControlThreadMain(CNetServer* pNetServer)
{
	CTcpServer* pTcpThis = (CTcpServer*)pNetServer;
	CTcpServerCfg* pTcpCfg = (CTcpServerCfg*)pTcpThis->m_pCfg;
	CTcpListenCtxMan* pTcpListenCtxMan = (CTcpListenCtxMan*)pTcpCfg->GetListenCtxMan();
	CTcpClientCtxMan* pTcpClientCtxMan = (CTcpClientCtxMan*)pTcpCfg->GetClientCtxMan();
	CNetIoDataMan* pIoDataMan = pTcpCfg->GetIoDataMan();
	CTcpListenCtx* pTcpListenCtx = NULL;
	CNetTrace* pTrace = pTcpThis->m_pTrace;

	BOOL bUseForceConn = FALSE;
	int nForceCloseIntervalTimeOut = 0, nForceClosePendingAcceptTimeOut = 0, nForceCloseNoneLastOpConnTimeOut = 0;
	int nMinPendingAccepts = pTcpCfg->GetMinPendingAccepts();
	int nMaxPendingAccepts = pTcpCfg->GetMaxPendingAccepts();

	pTcpCfg->GetForceCloseInfo(&bUseForceConn, 
		&nForceCloseIntervalTimeOut, &nForceClosePendingAcceptTimeOut, &nForceCloseNoneLastOpConnTimeOut);

	int i = 0, nListenCtxIdx = 0, nListenCtxListSize = 0;
	int nForceClosedAccept = 0, nForceClosedConn = 0;
	int nBurstPendingAccepts = 0;
	DWORD dwWaitRet = 0, dwTimeOut = 0;
	HANDLE hWaitEvts[MAXIMUM_WAIT_OBJECTS];
	
	// ForceClose ��ƾ�� ������� ������, WaitFor ���Ѵ��
	dwTimeOut = (bUseForceConn ? nForceCloseIntervalTimeOut * 1000 : INFINITE);
	ZeroMemory(hWaitEvts, sizeof(HANDLE)*MAXIMUM_WAIT_OBJECTS);

	// WaitFor �̺�Ʈ�� �߰�
	hWaitEvts[i++] = pTcpCfg->GetExitEvt();

	for (VCT_PNETLISTENCTX_IT it = pTcpListenCtxMan->m_NetListenCtxList.begin(); it != pTcpListenCtxMan->m_NetListenCtxList.end(); ++it)
	{
		pTcpListenCtx = (CTcpListenCtx*)(*it);
		hWaitEvts[i++] = pTcpListenCtx->m_hFdAcceptEvt;
	}

	// WaitFor ���
	while (1)
	{
		dwWaitRet = WSAWaitForMultipleEvents(i, hWaitEvts, FALSE, dwTimeOut, FALSE);
		if (dwWaitRet == WAIT_FAILED)
		{
			m_pTrace->WriteErrorMsg(_T("CTcpServer::ControlThreadMain: WSAWaitForMultipleEvents failed: %d\n"), WSAGetLastError());
			break;
		}
		else if (dwWaitRet == WAIT_TIMEOUT)
		{
			// ForceCloseNoneLastOpConn ��ƾ ����
			nForceClosedConn = ForceCloseNoneLastOpConn(pTcpClientCtxMan, pIoDataMan, nForceCloseNoneLastOpConnTimeOut);
			
			// ForceClosePendingAccept ��ƾ ����
			// nMinPendingAccepts �̻��� ��� ���� ��Ŵ 
			nForceClosedAccept = ForceClosePendingAccept(pTcpThis, nMinPendingAccepts, nForceClosePendingAcceptTimeOut);

			pTrace->WriteDebugMsg(_T("CTcpServer::ControlThreadMain: ForceCloseNoneLastOpConn: %d, ForceClosePendingAccept: %d\n"), nForceClosedConn, nForceClosedAccept);
		}
		else if (dwWaitRet == WAIT_OBJECT_0)
		{
			// ���� ���� 
			pTrace->WriteDebugMsg(_T("CTcpServer::ControlThreadMain: received ExitEvt\n"));
			PostQueuedCompletionStatus(pTcpThis->m_hPort, 0, NULL, NULL);
			break;
		}
		else if (dwWaitRet > WAIT_OBJECT_0 && dwWaitRet < WAIT_OBJECT_0 + i)
		{
			// FD_ACCEPT �ñ׳� ��Ȳ
			nListenCtxIdx = dwWaitRet - WAIT_OBJECT_0 - 1;
			if (nListenCtxIdx >= nListenCtxListSize)
			{
				m_pTrace->WriteErrorMsg(_T("CTcpServer::ControlThreadMain: Invalid ListenCtxIdx(%d), TotalSize: %d\n"), nListenCtxIdx, nListenCtxListSize);
				break;
			}
			pTcpListenCtx = (CTcpListenCtx*)pTcpListenCtxMan->m_NetListenCtxList[nListenCtxIdx];

			// �ִ� ���� ������ŭ Accept ����Ŵ
			nBurstPendingAccepts = BurstPendingAccepts(pTcpThis, pTcpListenCtx, nMaxPendingAccepts);
			pTrace->WriteDebugMsg(_T("CTcpServer::ControlThreadMain:BurstPendingAccepts: %d\n"), nBurstPendingAccepts);
		}
		else
		{
			m_pTrace->WriteErrorMsg(_T("CTcpServer::ControlThreadMain: WSAWaitForMultipleEvents failed: %d\n"), WSAGetLastError());
			break;
		}
	}

	return 0;
}

// ���� ��ü����, ������ ���ų� Ȥ�� ������ ���� �۾��� ���� �е����� AceeptEx ���ϵ��� nMinPendingAccepts �������� ���� ����
// [in] nMinPendingAccepts: Accept ��� �ּ� ����
// [in] nPendingAcceptTimeOut: ������ ���ų�, ������ ���� �۾��� ���� ���ϵ��� �������� �ð�(��)
//							AcceptEx�� ����� ������ ���ÿ� �̷�� ���µ�, ���Ḹ �ΰ� ������ ���� Ŭ���̾�Ʈ�� ���� ���� (DOS ����)
// [retVal] int: ����� ���� ����
int CTcpServer::ForceClosePendingAccept(CTcpServer* pTcpThis, int nMinPendingAccepts, int nPendingAcceptTimeOut)
{
	int retVal = 0;
	int optVal = 0, optLen = 0;
	int nRetCode = 0;
	
	CTcpServerCfg* pTcpCfg = (CTcpServerCfg*)pTcpThis->m_pCfg;
	CTcpListenCtxMan* pTcpListenCtxMan = (CTcpListenCtxMan*)pTcpCfg->GetListenCtxMan();
	CNetIoDataMan* pIoDataMan = pTcpCfg->GetIoDataMan();
	CTcpListenCtx* pTcpListenCtx = NULL;
	CTcpClientCtx* pTcpCtx = NULL;
	CNetIoData* pIoData = NULL;
	LST_PNETIODATA_IT ioList_it;

	for (VCT_PNETLISTENCTX_IT it = pTcpListenCtxMan->m_NetListenCtxList.begin(); it != pTcpListenCtxMan->m_NetListenCtxList.end(); ++it)
	{
		pTcpListenCtx = (CTcpListenCtx*)(*it);
		LST_PNETIODATA& ioList = pTcpListenCtx->GetPendingAcceptIoList();


		EnterCriticalSection(&pTcpListenCtx->m_Cs); // PendingAcceptIoList ����ȭ

		for (ioList_it = ioList.begin(); ioList_it != ioList.end(); ++ioList_it)
		{
			pIoData = (*ioList_it);
			pTcpCtx = (CTcpClientCtx*)pIoData->m_pNetCtx;

			optLen = sizeof(optVal);
			nRetCode = getsockopt(
						pTcpCtx->m_Socket,
						SOL_SOCKET,
						SO_CONNECT_TIME,
						(char *)&optVal,
						&optLen
						);
			if (nRetCode == SOCKET_ERROR)
			{
				m_pTrace->WriteErrorMsg(_T("getsockopt: SO_CONNECT_TIME failed: %d\n"), WSAGetLastError());
			}
			else
			{
				// Ŭ���̾�Ʈ ���Ͽ� ������ ���ų� Ȥ�� ������ �۾��� ���� ���ϵ��� ����
				// ������ ����Ǹ�, completion �����忡�� �ҵ��� AcceptEx ȣ���� ���� ó����
				if ((optVal != 0xFFFFFFFF) && (optVal > nPendingAcceptTimeOut))
				{
					// �ּ� ��� ������ Ȯ��
					if ((int)ioList.size() > nMinPendingAccepts)
					{
						m_pTrace->WriteDebugMsg(_T("closing pending accept socket\n"));
						CloseOp(pTcpThis, pIoData, TRUE);
											
						retVal++;
					}
				}
			}
		}

		LeaveCriticalSection(&pTcpListenCtx->m_Cs);
		
	}

	return retVal;
}

// ���� ��ü����, ������ �۾��� �۵����� �ʴ� Ŭ���̾�Ʈ���� ���� ����
// [in] pTcpClientCtxMan: Ŭ���̾�Ʈ ���� ������ context ������ ���� ClientCtxMan
// [in] pIoDataMan: IO data ������ ���� CNetIoDataMan
// [in] nNoneLastOpConnTimeOut: ������ �۾�(Recv/Send) ���� �۵����� �ʴ� Ŭ���̾�Ʈ���� ���� ����(��)
// [retVal] int: ����� ���� ����
int CTcpServer::ForceCloseNoneLastOpConn(CTcpClientCtxMan* pTcpClientCtxMan, CNetIoDataMan* pIoDataMan, int nNoneLastOpConnTimeOut)
{
	EnterCriticalSection(&pTcpClientCtxMan->m_Cs);
	




	LeaveCriticalSection(&pTcpClientCtxMan->m_Cs);

	return 0;
}

// FD_ACCEPT ��Ȳ�̸�, �ִ��� ���� Aceept �� ��� ��Ű���� ��
// [in] pTcpListenCtx: ���� ��ü
// [in] nMaxPendingAccepts: Accept ������ �ִ� ����
// [retVal] Accept ��� ��Ų ����
int CTcpServer::BurstPendingAccepts(CTcpServer* pTcpThis, CTcpListenCtx* pTcpListenCtx, int nMaxPendingAccepts)
{
	// non-paged pool ���� ���� ��ó �ʿ�
	// �� Ŀ�ؼǴ� 2kb �� ���, Windows NT�� 128M, Windows 2000 �̻��� �������޸��� 1/4 ���� �Ҵ� ���� (p229)

	CNetServer::RET_CODE rc = CNetServer::E_UNKNOWN;
	int nCreatedAccept = 0, nNeedAccept = 0;
	nNeedAccept = nMaxPendingAccepts - pTcpListenCtx->GetPendingAcceptIoCount();

	// ���� ���Ͽ��� PostAcceptOp ����	
	for (int i = 0; i < nNeedAccept; i++)
	{
		rc = PostAcceptOp(pTcpThis, pTcpListenCtx);
		if (rc != CNetServer::E_SUCCESS)
		{
			m_pTrace->WriteErrorMsg(_T("StartListen: PostAcceptOp failed: %d\n"), rc);
			break;
		}
		nCreatedAccept++;
	}

	return nCreatedAccept;
}


// ���� ���� ���Ḧ ��û
// Ŭ���̾�Ʈ�� ���� IO �۾��� ������� ������ ���� ���� ��û
void CTcpServer::ForceCloseConn(CTcpClientCtx* pTcpCtx)
{
	EnterCriticalSection(&pTcpCtx->m_Cs);
	
	// ������ ������Ѽ� completion �����忡�� ������� Ŭ���̾�Ʈ�� I/O �۾����� ��� ���� ó���ǵ��� ��
	if (pTcpCtx->m_Socket != INVALID_SOCKET)
	{
		// graceful close
		shutdown(pTcpCtx->m_Socket, SD_BOTH);

		/*
		// LINGER �ɼ� ���
		LINGER  lingerStruct;
		lingerStruct.l_onoff = 1; 
		lingerStruct.l_linger = 0; 
		setsockopt(pTcpCtx->m_Socket, SOL_SOCKET, SO_LINGER, 
					(char *)&lingerStruct, sizeof(lingerStruct) ); 		
		*/

		closesocket(pTcpCtx->m_Socket);
		pTcpCtx->m_Socket = INVALID_SOCKET;
		pTcpCtx->m_bRequestedClose = TRUE;
	}

	LeaveCriticalSection(&pTcpCtx->m_Cs);
}

// pNetBuf�� �� �κа� pFindBuf �� nFindBufSize ��ŭ memcmp ���Ͽ� ���������� ��ȯ
BOOL CTcpServer::IsSameEndBuf(CNetBuf* pNetBuf, const BYTE* pFindBuf, DWORD dwFindBufSize)
{
	BOOL retVal = FALSE;
	
	DWORD dwNetBufDataSize = pNetBuf->GetDataSize();
	if (pFindBuf && (dwNetBufDataSize > dwFindBufSize))
	{
		BYTE* pPos = (BYTE*)pNetBuf->GetDataPtr();
		pPos = pPos + (dwNetBufDataSize - dwFindBufSize);

		retVal = (memcmp(pPos, pFindBuf, dwFindBufSize) == 0);
	}

	return retVal;
}

// �������ο� ������� Overlapped IO �۾� ���� �� Ŀ�ؼ� ���Ḧ ��û
// [in] pIoData: �Ϸ�� IO data
// [in] bRequestCloseConn: Ŀ�ؼ� ���� ��û ����
void CTcpServer::CloseOp(CNetServer* pThis, CNetIoData* pIoData, BOOL bRequestCloseConn)
{
	CTcpServer* pTcpThis = (CTcpServer*)pThis;
	CTcpServerCfg* pTcpCfg = (CTcpServerCfg*)pTcpThis->m_pCfg;
	CTcpServerStat* pTcpStat = (CTcpServerStat*)pTcpThis->m_pStat;
	CNetIoDataMan* pIoDataMan = pTcpCfg->GetIoDataMan();
	CTcpClientCtxMan* pTcpCtxMan = (CTcpClientCtxMan*)pTcpCfg->GetClientCtxMan();
	CTcpClientCtx* pTcpCtx = (CTcpClientCtx*)pIoData->m_pNetCtx;
	
	EnterCriticalSection(&pTcpCtx->m_Cs);

	time(&pTcpCtx->m_LastTime);		// �۾��ð� ����

	// Operation �Ϸ� ����
	if (pIoData->m_Op == CNetIoData::OP_READ)
	{
		pTcpCtx->m_nOutstandingRecv--;
		pTcpStat->DecrementOutstandingRecvs();
	}
	else if (pIoData->m_Op == CNetIoData::OP_WRITE)
	{
		pTcpCtx->m_nOutstandingSend--;
		pTcpStat->DecrementOutstandingSends();
	}

	// ���� ��û ó��
	// ������ ������Ѽ� completion �����忡�� ������� Ŭ���̾�Ʈ�� I/O �۾����� ��� ���� ó���ǵ��� ��
	if (bRequestCloseConn)
	{
		if (pTcpCtx->m_Socket != INVALID_SOCKET)
		{
			// graceful close
			shutdown(pTcpCtx->m_Socket, SD_BOTH);
			closesocket(pTcpCtx->m_Socket);
			pTcpCtx->m_Socket = INVALID_SOCKET;
			pTcpCtx->m_bRequestedClose = TRUE;
		}
	}

	// �����û�� �ְ�, I/O�۾��� ��� ����Ǿ����� ClientCtx ����
	if (pTcpCtx->m_bRequestedClose && pTcpCtx->m_nOutstandingRecv == 0 && pTcpCtx->m_nOutstandingSend == 0)
	{
		CTcpListenCtx* pTcpListenCtx = (CTcpListenCtx*)pTcpCtx->m_pListenCtx;

		pTcpCtxMan->DeallocCtx(pTcpCtx);

		// ���� ���� �� Ctx ���� �Ϸ�� ����
		CTcpServerStat* pTcpStat = (CTcpServerStat*)pTcpThis->m_pStat;
		pTcpStat->DecrementConnectionCount();


		// ���ο� Accept ���� ���� ����
		if (pTcpListenCtx->GetPendingAcceptIoCount() < pTcpCfg->GetMinPendingAccepts())
		{
			// Accept ��� ������ �ּҰ� �̸��̸� re-PostAccept
			CNetServer::RET_CODE rc;
			rc = PostAcceptOp(pTcpThis, pTcpListenCtx);
			if (rc != CNetServer::E_SUCCESS)
				m_pTrace->WriteErrorMsg(_T("CloseOp: PostAcceptOp failed: %d\n"), rc);
		}
	}

	LeaveCriticalSection(&pTcpCtx->m_Cs);
	
	if (pTcpCtx->m_bRequestedClose)
	{
		// ���� ���� ��û������ �Ϸ�� IO data�� ������Ŵ
		pIoDataMan->DeallocData(pIoData);
	}
}

// �ش� ���Ͽ� overlapped receive operation ����
// [in] pCtx: �۾� ���� context
// [in] pIoData: IO �۾��� ������
// [in] dwOpBufSize: IO �۾��� �ʿ��� ���� ũ��
// [retVal] E_SUCCESS - ����
//			��Ÿ	  - ����, ��û ���õ�
CNetServer::RET_CODE CTcpServer::PostRecvOp(CNetServer* pThis, CNetCtx* pCtx, CNetIoData* pIoData, DWORD dwOpBufSize)
{
	CNetServer::RET_CODE retVal = CNetServer::E_UNKNOWN;
	CNetBuf* pBuf = pIoData->m_pBuf;
	
	int nErroCode = 0, nLastError = 0;
	WSABUF wsaBuf;
    DWORD dwBytes = 0, dwFlags = 0;
    
	pIoData->m_Op = CNetIoData::OP_READ;
	wsaBuf.buf = (char*)pBuf->GetAvailMemPtr(dwOpBufSize);
	wsaBuf.len = pBuf->GetAvailMemSize();
	

	EnterCriticalSection(&pCtx->m_Cs);
	
	nErroCode = WSARecv(
					pCtx->m_Socket,
					&wsaBuf,
					1,
					&dwBytes,
					&dwFlags,
					&pIoData->m_Ol,
					NULL
					);

	if (nErroCode == SOCKET_ERROR)
    {
		nLastError = WSAGetLastError();
		if (nLastError != WSA_IO_PENDING)
		{
			// WSARecv ����

			if (nLastError == WSAENOBUFS)
			{
				// locked page ����
				// IO �۾��� ���̰ų�, ���ʿ��� ������ �ݾ�� �� ��Ȳ.
				m_pTrace->WriteErrorMsg(_T("PostRecvOp: locked page error: WSAENOBUFS\n"));
			}

			m_pTrace->WriteDebugMsg(_T("PostRecvOp: WSARecv* failed: %d\n"), nLastError);
			retVal = CNetServer::E_FAIL_POST_RECV;
		}
	}

	if (retVal != CNetServer::E_FAIL_POST_RECV)
	{
		// WSARecv ����
		pCtx->m_nOutstandingRecv++;
		pThis->m_pStat->IncrementOutstandingRecvs();
		retVal = CNetServer::E_SUCCESS;
    }
  
	LeaveCriticalSection(&pCtx->m_Cs);

    return retVal;
}

CNetServer::RET_CODE CTcpServer::PostSendOp(CNetServer* pThis, CNetCtx* pCtx, CNetIoData* pIoData)
{	
	CNetServer::RET_CODE retVal = CNetServer::E_UNKNOWN;
	CNetBuf* pBuf = pIoData->m_pBuf;
	
	int nErroCode = 0, nLastError = 0;
	WSABUF wsaBuf;
    DWORD dwBytes = 0;

	pIoData->m_Op = CNetIoData::OP_WRITE;
	wsaBuf.buf = (char*)pBuf->GetDataPtr();
	wsaBuf.len = pBuf->GetDataSize();


	EnterCriticalSection(&pCtx->m_Cs);

	nErroCode = WSASend(
					pCtx->m_Socket,
					&wsaBuf,
					1,
					&dwBytes,
					0,
					&pIoData->m_Ol,
					NULL
					);

	if (nErroCode == SOCKET_ERROR)
    {
		nLastError = WSAGetLastError();
		if (nLastError != WSA_IO_PENDING)
		{
			// WSASend ����

			if (nLastError == WSAENOBUFS)
			{
				// locked page ����
				// IO �۾��� ���̰ų�, ���ʿ��� ������ �ݾ�� �� ��Ȳ.
				m_pTrace->WriteErrorMsg(_T("PostSendOp: locked page error: WSAENOBUFS\n"));
			}
			
			m_pTrace->WriteDebugMsg(_T("PostSendOp: WSASend* failed: %d [internal = %d]\n"), nLastError, pIoData->m_Ol.Internal);
			retVal = CNetServer::E_FAIL_POST_SEND;
		}
	}

	if (retVal != CNetServer::E_FAIL_POST_SEND)
	{
		// WSARecv ����
		pCtx->m_nOutstandingSend++;
		pThis->m_pStat->IncrementOutstandingSends();
		retVal = CNetServer::E_SUCCESS;
    }
  
	LeaveCriticalSection(&pCtx->m_Cs);

    return retVal;
}

// [in] nWsaLastError: completion �����忡���� �Ϸ� ��� �ڵ� ����
CNetServer::RET_CODE CTcpServer::CompRecvOp(CNetServer* pThis, CNetCtx* pCtx, CNetIoData* pIoData, int nWsaLastError)
{
	CNetServer::RET_CODE retVal = CNetServer::E_UNKNOWN;

	CTcpServer* pTcpThis = (CTcpServer*)pThis;
	CNetBuf* pNetBuf = pIoData->m_pBuf;

	BYTE endMark[2] = {'\r', '\n'};
	if (IsSameEndBuf(pNetBuf, endMark, 2) == FALSE)
	{
		// ��Ȳ1. CTcpServerCfg::SetAcceptExBufSize(0) ���� �����Ͽ� 0 byte�� ������ ���.
		// if (pIoData->m_Op == OP_ACCEPT && nWsaLastError == WSAEWOULDBLOCK)
		pNetBuf->Clear();

		// ��Ȳ2. ū �������̰� ���۰� �����Ͽ� �� �������� ���� ���, ���� �����Ͱ� �� ����

		// ���۸� �ʱ�ȭ ��Ű�� �ʰ�, �� ���ۿ� �� �ٿ� �޵��� ��
		//pNetBuf->Clear();

		retVal = PostRecvOp(pThis, pCtx, pIoData, 4 * 1024);
		if (retVal != CNetServer::E_SUCCESS)
		{
			// ���� �۾� ����!!, ���� ����
			CloseOp(pThis, pIoData, TRUE);
			return CNetServer::E_SUCCESS;
		}
		
		printf("���� ���� ����");
		return CNetServer::E_SUCCESS;

	}


	// 1) ���� ������ ������ ���ؼ�, PostSendOp ȣ��
	{
		// ���� �ʱ�ȭ
		pNetBuf->Clear();


		// ���� ������ ����
		char szTest[128];
		strcpy_s(szTest, _countof(szTest), "OK 1234567890\r\n");

		// ����� ���� ����
		pNetBuf->AddData(szTest, strlen(szTest));
		
		// Ȥ��, NetBuf�� ���� �̿��� ������ ���
		strcpy_s((char*)pNetBuf->GetAvailMemPtr(128), 128, "OK 1234567890\r\n");
		pNetBuf->ResetDataSize(0, strlen("OK 1234567890\r\n"));


		// ������ ����
		retVal = PostSendOp(pThis, pCtx, pIoData);
		if (retVal != CNetServer::E_SUCCESS)
		{
			// ���� �۾� ����!!, ���� ����
			CloseOp(pThis, pIoData, TRUE);
			return CNetServer::E_SUCCESS;
		}

		printf("���� ���� ����");
		return CNetServer::E_SUCCESS;
	}
	
	// 2) ������ ������ ���ؼ�, PostRecvOp ȣ���Ͽ� ���� ���·� ���
	{
		// ���� �ʱ�ȭ
		pNetBuf->Clear();

		retVal = PostRecvOp(pThis, pCtx, pIoData, 4 * 1024);
		if (retVal != CNetServer::E_SUCCESS)
		{
			// ���� �۾� ����!!, ���� ����
			CloseOp(pThis, pIoData, TRUE);
			return CNetServer::E_SUCCESS;
		}
		
		printf("���� ���� ����");
		return CNetServer::E_SUCCESS;
	}

	// 3) ���� ����
	{
		CloseOp(pThis, pIoData, TRUE);
		return CNetServer::E_SUCCESS;
	}

	return CNetServer::E_SUCCESS;
}

CNetServer::RET_CODE CTcpServer::CompSendOp(CNetServer* pThis, CNetCtx* pCtx, CNetIoData* pIoData, int nWsaLastError)
{
	CNetServer::RET_CODE retVal = CNetServer::E_UNKNOWN;

	CTcpServer* pTcpThis = (CTcpServer*)pThis;
	CNetBuf* pNetBuf = pIoData->m_pBuf;

	
	// 1) ������ ������ ���ؼ�, PostRecvOp ȣ���Ͽ� ���� ���·� ���
	{
		// ���� �ʱ�ȭ
		pNetBuf->Clear();

		retVal = PostRecvOp(pThis, pCtx, pIoData, 4 * 1024);
		if (retVal != CNetServer::E_SUCCESS)
		{
			// ���� �۾� ����!!, ���� ����
			CloseOp(pThis, pIoData, TRUE);
			return CNetServer::E_SUCCESS;
		}
		
		printf("���� ���� ����");
		return CNetServer::E_SUCCESS;
	}

	// 2) ���� �����Ͱ� ���� �־, PostSendOp ȣ��
	{
		// ���� �ʱ�ȭ
		pNetBuf->Clear();

		// ���� ������ ����
		char szTest[128];
		strcpy_s(szTest, _countof(szTest), "OK 1234567890\r\n");

		// ����� ���� ����
		pNetBuf->AddData(szTest, strlen(szTest));
		
		// Ȥ��, NetBuf�� ���� �̿��� ������ ���
		strcpy_s((char*)pNetBuf->GetAvailMemPtr(128), 128, "OK 1234567890\r\n");
		pNetBuf->ResetDataSize(0, strlen("OK 1234567890\r\n"));


		// ������ ����
		retVal = PostSendOp(pThis, pCtx, pIoData);
		if (retVal != CNetServer::E_SUCCESS)
		{
			// ���� �۾� ����!!, ���� ����
			CloseOp(pThis, pIoData, TRUE);
			return CNetServer::E_SUCCESS;
		}

		printf("���� ���� ����");
		return CNetServer::E_SUCCESS;
	}


	// 3) ���� ����
	{
		CloseOp(pThis, pIoData, TRUE);
		return CNetServer::E_SUCCESS;
	}

	return CNetServer::E_SUCCESS;
}

//////////////////////////////////////////////////////////
//			CUdpServer
//////////////////////////////////////////////////////////
CUdpServer::CUdpServer(CUdpServerCfg* pCfg, CUdpServerStat* pStat, CNetTrace* pTrace) : CNetServer(pCfg, pStat, pTrace)
{
	m_Protocol = CNetServer::UDP;
}

CUdpServer::~CUdpServer()
{
}

// Control �������� ���� �Լ�
unsigned CUdpServer::ControlThreadMain(CNetServer* pNetServer)
{ 
	CUdpServer* pUdpThis = (CUdpServer*)pNetServer;
	CUdpServerCfg* pUdpCfg = (CUdpServerCfg*)pUdpThis->m_pCfg;
	CNetTrace* pTrace = pUdpThis->m_pTrace;
	DWORD dwWaitRet = 0;

	dwWaitRet = WaitForSingleObject(pUdpCfg->GetExitEvt(), INFINITE);
	if (dwWaitRet == WAIT_OBJECT_0)
	{
		// ���� ���� 
		pTrace->WriteDebugMsg(_T("CUdpServer::ControlThreadMain: received ExitEvt\n"));
		PostQueuedCompletionStatus(pUdpThis->m_hPort, 0, NULL, NULL);
	}
	else
	{
		m_pTrace->WriteErrorMsg(_T("CUdpServer::ControlThreadMain: WaitForSingleObject failed(dwWaitRet: %d)\n"), dwWaitRet);
	}
	
	return 0;
}


// MS ���� Ȯ�� �Լ��� �ε�
// [in] pCtx: ������ ������ Context
// [retVal] FALSE: �ε� ����
BOOL CUdpServer::LoadWinSockExFunc(CNetListenCtx* pListenCtx) 
{
	CUdpListenCtx* pUdpListenCtx = (CUdpListenCtx*)pListenCtx;
	DWORD dwBytes = 0;
	int nOptVal = 0;
	int nRetCode = 0;

	// SIO_UDP_CONNRESET ����
    nRetCode = WSAIoctl(
				pUdpListenCtx->m_Socket,
				SIO_UDP_CONNRESET,
				&nOptVal,
				sizeof(nOptVal),
				NULL,
				0,
			    &dwBytes,
				NULL,
				NULL
            );
    if (nRetCode == SOCKET_ERROR)
    {
        m_pTrace->WriteErrorMsg(_T("WSAIoctl: SIO_UDP_CONNRESET failed: %d\n"), WSAGetLastError());
		return FALSE;
    }

	return TRUE; 
}

// ���Ϲ��ε� �� ���� ���Ŀ� ȣ��
// [in] pListenCtx: ������ ������ Context
CNetServer::RET_CODE CUdpServer::StartListen(CNetListenCtx* pListenCtx) 
{
	CUdpListenCtx* pUdpListenCtx = (CUdpListenCtx*)pListenCtx;
	CUdpServerCfg* pUdpCfg = (CUdpServerCfg*)m_pCfg;
	CNetIoDataMan* pIoDataMan = (CNetIoDataMan*)pUdpCfg->GetIoDataMan();
	CNetIoData* pIoData = NULL;
	CNetServer::RET_CODE rc = CNetServer::E_UNKNOWN;

	// ���� ���Ͽ��� PostRecvOp ����	
	for (int i = 0; i < pUdpCfg->GetMinPendingRecvs(); i++)
	{
		pIoData = pIoDataMan->AllocData();
		pIoData->m_pNetCtx = pUdpListenCtx;

		// CNetBuf�� ���� ũ�⸸ŭ�� Overlapped ���� ũ��� ����
		rc = PostRecvOp(this, pUdpListenCtx, pIoData, pIoData->m_pBuf->GetAvailMemSize());   

		if (rc != CNetServer::E_SUCCESS)
		{
			m_pTrace->WriteErrorMsg(_T("StartListen: PostRecvOp failed: %d\n"), rc);
			pIoDataMan->DeallocData(pIoData);
		}
	}

	return CNetServer::E_SUCCESS; 
}

// �������ο� ������� Overlapped IO �۾� ���� �� Ŀ�ؼ� ���Ḧ ��û
// bRequestCloseConn ��û ���Ŀ��� pIoData ��ü ������
// [in] pIoData: �Ϸ�� IO data
// [in] bRequestCloseConn: TCP Ŀ�ؼ� ���� ��û ����
void CUdpServer::CloseOp(CNetServer* pThis, CNetIoData* pIoData, BOOL bRequestCloseConn)
{
	CUdpServer* pUdpThis = (CUdpServer*)pThis;
	CUdpServerCfg* pUdpCfg = (CUdpServerCfg*)pUdpThis->m_pCfg;
	CUdpServerStat* pUdpStat = (CUdpServerStat*)pUdpThis->m_pStat;
	CNetIoDataMan* pIoDataMan = pUdpCfg->GetIoDataMan();
	CUdpListenCtx* pUdpListenCtx = (CUdpListenCtx*)pIoData->m_pNetCtx;
		
	// Operation �Ϸ� ����
	if (pIoData->m_Op == CNetIoData::OP_READ)
	{
		InterlockedDecrement(&pUdpListenCtx->m_nOutstandingRecv);
		pUdpStat->DecrementOutstandingRecvs();
	}
	else if (pIoData->m_Op == CNetIoData::OP_WRITE)
	{
		InterlockedDecrement(&pUdpListenCtx->m_nOutstandingSend);
		pUdpStat->DecrementOutstandingSends();
	}
	
	if (pUdpListenCtx->m_nOutstandingRecv < pUdpCfg->GetMinPendingRecvs())
	{
		// Recv ��� ������ ������ �Ϸ�� op�� �ʱ�ȭ ��Ű��, �ٽ� Recv ���
		pIoData->Reset();
		CNetServer::RET_CODE rc = PostRecvOp(pThis, pUdpListenCtx, pIoData, pIoData->m_pBuf->GetAvailMemSize());
		if (rc != CNetServer::E_SUCCESS)
		{
			m_pTrace->WriteErrorMsg(_T("CloseOp: PostRecvOp failed: %d\n"), rc);
			pIoDataMan->DeallocData(pIoData);
		}
	}
	else
	{
		pIoDataMan->DeallocData(pIoData);
	}
}

// �ش� ���Ͽ� overlapped receive operation ����
// [in] pCtx: �۾� ���� context
// [in] pIoData: IO �۾��� ������
// [in] dwOpBufSize: IO �۾��� �ʿ��� ���� ũ��
// [retVal] E_SUCCESS - ����
//			��Ÿ	  - ����, ��û ���õ�
CNetServer::RET_CODE CUdpServer::PostRecvOp(CNetServer* pThis, CNetCtx* pCtx, CNetIoData* pIoData, DWORD dwOpBufSize)
{
	CNetServer::RET_CODE retVal = CNetServer::E_UNKNOWN;
	CUdpListenCtx* pUdpListenCtx = (CUdpListenCtx*)pCtx;
	CNetBuf* pBuf = pIoData->m_pBuf;
	
	int nErroCode = 0, nLastError = 0;
	WSABUF wsaBuf;
    DWORD dwBytes = 0, dwFlags = 0;
    
	pIoData->m_Op = CNetIoData::OP_READ;
	wsaBuf.buf = (char*)pBuf->GetAvailMemPtr(dwOpBufSize);
	wsaBuf.len = pBuf->GetAvailMemSize();
	

	EnterCriticalSection(&pUdpListenCtx->m_Cs);
	
	nErroCode = WSARecvFrom(
					pUdpListenCtx->m_Socket,
					&wsaBuf,
					1,
					&dwBytes,
					&dwFlags,
					(SOCKADDR *)&pUdpListenCtx->m_ClientAddr,
					&pUdpListenCtx->m_nClientAddrLen,
					&pIoData->m_Ol,
					NULL
					);

	if (nErroCode == SOCKET_ERROR)
    {
		nLastError = WSAGetLastError();
		if (nLastError != WSA_IO_PENDING)
		{
			// WSARecv ����

			if (nLastError == WSAENOBUFS)
			{
				// locked page ����
				// IO �۾��� ���̰ų�, ���ʿ��� ������ �ݾ�� �� ��Ȳ.
				m_pTrace->WriteErrorMsg(_T("PostRecvOp: locked page error: WSAENOBUFS\n"));
			}

			m_pTrace->WriteDebugMsg(_T("PostRecvOp: WSARecv* failed: %d\n"), nLastError);
			retVal = CNetServer::E_FAIL_POST_RECV;
		}
	}

	if (retVal != CNetServer::E_FAIL_POST_RECV)
	{
		// WSARecv ����
		pUdpListenCtx->m_nOutstandingRecv++;
		pThis->m_pStat->IncrementOutstandingRecvs();
		retVal = CNetServer::E_SUCCESS;
    }
  
	LeaveCriticalSection(&pUdpListenCtx->m_Cs);

    return retVal;
}

CNetServer::RET_CODE CUdpServer::PostSendOp(CNetServer* pThis, CNetCtx* pCtx, CNetIoData* pIoData)
{	
	CNetServer::RET_CODE retVal = CNetServer::E_UNKNOWN;
	CUdpListenCtx* pUdpListenCtx = (CUdpListenCtx*)pCtx;
	CNetBuf* pBuf = pIoData->m_pBuf;
	
	int nErroCode = 0, nLastError = 0;
	WSABUF wsaBuf;
    DWORD dwBytes = 0;

	pIoData->m_Op = CNetIoData::OP_WRITE;
	wsaBuf.buf = (char*)pBuf->GetDataPtr();
	wsaBuf.len = pBuf->GetDataSize();


	EnterCriticalSection(&pUdpListenCtx->m_Cs);

	// RecvFrom ���� ���� Ŭ���̾�Ʈ IP�� SendTo�ÿ��� ������
	nErroCode = WSASendTo(
					pUdpListenCtx->m_Socket,
					&wsaBuf,
					1,
					&dwBytes,
					0,
					(SOCKADDR *)&pUdpListenCtx->m_ClientAddr,
					pUdpListenCtx->m_nClientAddrLen,
					&pIoData->m_Ol,
					NULL
					);

	if (nErroCode == SOCKET_ERROR)
    {
		nLastError = WSAGetLastError();
		if (nLastError != WSA_IO_PENDING)
		{
			// WSASend ����

			if (nLastError == WSAENOBUFS)
			{
				// locked page ����
				// IO �۾��� ���̰ų�, ���ʿ��� ������ �ݾ�� �� ��Ȳ.
				m_pTrace->WriteErrorMsg(_T("PostSendOp: locked page error: WSAENOBUFS\n"));
			}
			
			m_pTrace->WriteDebugMsg(_T("PostSendOp: WSASend* failed: %d [internal = %d]\n"), nLastError, pIoData->m_Ol.Internal);
			retVal = CNetServer::E_FAIL_POST_SEND;
		}
	}

	if (retVal != CNetServer::E_FAIL_POST_SEND)
	{
		// WSARecv ����
		pUdpListenCtx->m_nOutstandingSend++;
		pThis->m_pStat->IncrementOutstandingSends();
		retVal = CNetServer::E_SUCCESS;
    }
  
	LeaveCriticalSection(&pUdpListenCtx->m_Cs);

    return retVal;
}