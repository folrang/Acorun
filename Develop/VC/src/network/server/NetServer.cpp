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
	
	// 0. 서버 초기화
	int nNetClientCtxFreeListSize = 0, nNetIoDataFreeListSize = 0;
	m_pCfg->GetNetCtxInfo(&nNetClientCtxFreeListSize, &nNetIoDataFreeListSize);

	pNetClientCtxMan->SetMaxFreeListSize(nNetClientCtxFreeListSize);
	pNetIoDataMan->SetMaxFreeListSize(nNetIoDataFreeListSize);

	// 1. 윈속 초기화
	if (WSAStartup(MAKEWORD(2,2), &wsd) != 0)
		return CNetServer::E_FAIL_INIT_API;	

	// 2. IOCP 생성
	// dwNumberOfConcurrentThreads는 가장 이상적인 0값으로 셋팅
	m_hPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	if (m_hPort == NULL)
		return CNetServer::E_FAIL_INIT_API;

	// 3. Completion 스레드 생성 및 시작
	int nMinCompletionThread = m_pCfg->GetMinCompletionThread();

	for (i = 0; i < nMinCompletionThread; i++)
	{
		hThread = (HANDLE)_beginthreadex(NULL, 0, CompletionThread, (LPVOID)this, 0, &nThreadId);
		CloseHandle(hThread);
	}

	// 4. bind 및 listen 시작
	CNetListenCtx* pListenCtx = NULL;
	HANDLE hBindPort = NULL;
	int nRetCode = 0, nBacklog = 0;
	const LST_BIND_INFO& bindInfoList = m_pCfg->GetBindInfo();

	pListenCtxMan->SetMaxNetListenListSize((int)bindInfoList.size());
	
	for (LST_BIND_INFO_CIT cit = bindInfoList.begin(); cit != bindInfoList.end(); ++cit)
	{
		// 새로운 ListenCtx 객체 생성
		pListenCtx = pListenCtxMan->CreateCtx();
		pListenCtxMan->AddNetListenCtx(pListenCtx);
		
		SOCKET& s = pListenCtx->m_Socket;
		const addrinfo& ai = (*cit).ai;

		nBacklog = (*cit).backlog;
		memcpy(&pListenCtx->m_AddrInfo, &ai, sizeof(addrinfo));	

		s = socket(ai.ai_family, ai.ai_socktype, ai.ai_protocol);
		if (s == INVALID_SOCKET)
			return CNetServer::E_CREATE_LISTEN;
		
		// 윈속 확장 함수포인터를 Provider가 제공하는 소켓에 연결
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
		
		// 리슨후 작업 실행
		retVal = StartListen(pListenCtx);
		if (retVal != E_SUCCESS)
		{
			m_pTrace->WriteErrorMsg(_T("Initialize: StartListen failed: %d\n"), retVal);
			return retVal;
		}
	}


	// Control 스레드 생성
	m_hControlThread = (HANDLE)_beginthreadex(NULL, 0, ControlThread, (LPVOID)this, 0, &nThreadId);

	return CNetServer::E_SUCCESS;
}

void CNetServer::Uninitialize()
{
	int i = 0;

	// Control 스레드 종료 확인
	if (m_hControlThread) 
	{
		WaitForSingleObject(m_hControlThread, INFINITE);
		CloseHandle(m_hControlThread);
		m_hControlThread = NULL;
	}
	
	// Completion 스레드 종료 확인
	while (1)
	{
		if (m_pStat->GetCurrentCompletionCount() > 0)
		{
			Sleep(500);
			continue;
		}
		break;
	}
	
	// 사용된 Context들 해제 (ListenCtx 해제후 ClientCtx 해제해야 함)
	CNetListenCtxMan* pListenMan = m_pCfg->GetListenCtxMan();
	CNetClientCtxMan* pClientMan = m_pCfg->GetClientCtxMan();
	CNetIoDataMan* pIoDataMan = m_pCfg->GetIoDataMan();
	pListenMan->Cleanup();
	pClientMan->Cleanup();
	pIoDataMan->Cleanup();
	
	// 라이브러리 해제
	CloseHandle(m_hPort);
	WSACleanup();
}

// Control 스레드
unsigned __stdcall CNetServer::ControlThread(LPVOID lpvThreadParam)
{
	CNetServer* pThis = (CNetServer*)lpvThreadParam;
	return pThis->ControlThreadMain(pThis);
}


// Completion 스레드
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
		// UDP 경우
		//			lpKey: CUdpListenCtx
		//			lpOl: CNetIoData, CNetIoData->m_pNetCtx를 이용하여 CUdpListenCtx 참조 가능
		// TCP 경우
		//		1) Accept Op 상황
		//			lpKey: CTcpListenCtx
		//			lpOl: CNetIoData, CNetIoData->m_pNetCtx를 이용하여 CTcpListenCtx 참조 가능
		//		2) Recv/Send Op 상황
		//			lpKey: CTcpClientCtx
		//			lpOl: CNetIoData, CNetIoData->m_pNetCtx를 이용하여 CTcpClientCtx 참조 가능
		//
		nErrorCode = GetQueuedCompletionStatus(
						hComletionPort,
						&dwBytesTransferred,
						(PULONG_PTR)&lpKey,
						&lpOl,
						INFINITE
						);
		
		// completion 스레드 종료 요청
		if (lpKey == 0)
		{
			pTrace->WriteDebugMsg(_T("CompletionThread: Exit Evt\n"));
			return 0;
		}
		
		// Overlapped 데이타 변환
		pIoData = CONTAINING_RECORD(lpOl, CNetIoData, m_Ol);
		pCtx = pIoData->m_pNetCtx;

		if (nErrorCode == FALSE)
		{
			// WSAGetOverlappedResult 호출을 이용하여 윈속 에러 코드로 변환
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
				// IO 작업 오류 발생
                nErrorCode = WSAGetLastError();
				
				if (nErrorCode == WSAEWOULDBLOCK)
				{
					if (pIoData->m_Op == CNetIoData::OP_ACCEPT)
					{
						// 작업 성공, overlapped i/o 버퍼가 0인 경우, 실제 수신 데이터를 다시 받아야 함
						pThis->CompletionSuccessRecv(pThis, pCtx, pIoData, dwBytesTransferred, nErrorCode);
						continue;
					}
				}

				// locked page 오류
				if (nErrorCode == WSAENOBUFS)
				{
					// locked page 오류
					// IO 작업을 줄이거나, 불필요한 연결을 닫어야 할 상황.
					pTrace->WriteErrorMsg(_T("CompletionThread: locked page error: WSAENOBUFS\n"));
				}

				// I/O 작업 에러시, 
				//		TCP인 경우: 연결 종료 처리
				//		UDP인 경우: 정의없음, 실패 작업 재 요청 혹은 사용자 지정 작업 진행
				if (pThis->m_Protocol == CNetServer::TCP)
				{
					pThis->CloseOp(pThis, pIoData, TRUE);
				}
            }

			continue;
		}
		

		if (pIoData->m_Op == CNetIoData::OP_ACCEPT)
		{
			// TCP, Accept 작업 성공
			pTcpListenCtx = (CTcpListenCtx*)lpKey;
			pTcpCtx = (CTcpClientCtx*)pCtx;

			// 리슨ctx의 PendingAccept 목록에서 제거
			pTcpListenCtx->RemovePendingAccept(pIoData);
			
			// GetAcceptExSockaddrs() 함수를 이용하여 클라이언트 주소 얻어옴
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
				// 클라이언트 IP 추출 실패, 연결 종료
				pTrace->WriteErrorMsg(_T("CompletionThread: GetNameInfo failed\n"));
				pThis->CloseOp(pThis, pIoData, TRUE);
				continue;
			}
			pTcpCtx->m_Ip.assign(szClientIp);
			
			// 클라이언트 IP 접속 가능 여부 확인
			if (pThis->IsAcceptClient(pCfg, pTcpCtx->m_Ip) == FALSE)
			{
				// 연결 거절 및 연결 종료
				pTrace->WriteDebugMsg(_T("CompletionThread: IsAcceptClient returned FALSE (%s)\n"), pTcpCtx->m_Ip.c_str());
				pThis->CloseOp(pThis, pIoData, TRUE);
				continue;
			}

			// client socket를 iocp에 추가
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
			
			// 수신 작업 성공
			pThis->CompletionSuccessRecv(pThis, pCtx, pIoData, dwBytesTransferred, 0);

			// AcceptEx 작업 완료
			continue;
		}
		else if (pIoData->m_Op == CNetIoData::OP_WRITE)
		{
			// 전송 작업 성공
			pStat->AddSentBytes(dwBytesTransferred);
			
			pThis->CloseOp(pThis, pIoData);	// OV operation 완료

			pThis->CompSendOp(pThis, pCtx, pIoData, 0);
			
			// PostSendOp 작업 완료
			continue;
		}
		else
		{
			// 수신 작업 성공
			rc = pThis->CompletionSuccessRecv(pThis, pCtx, pIoData, dwBytesTransferred, 0);
		}

	}	// while(1)
	
	return 0;
}

// Completion thread에서 성공한 Recv 작업의 처리 내용
CNetServer::RET_CODE CNetServer::CompletionSuccessRecv(CNetServer* pThis, CNetCtx* pCtx, CNetIoData* pIoData, DWORD dwBytesTransferred, int nWsaLastError)
{
	CNetServerCfg* pCfg = (CNetServerCfg*)pThis->m_pCfg;
	CNetServerStat* pStat = (CNetServerStat*)pThis->m_pStat;
	CNetBuf* pBuf = pIoData->m_pBuf;
	
	pStat->AddRecvBytes(dwBytesTransferred);	// 통계 정보 갱신
	pBuf->ResetDataSize(pBuf->GetDataSize(), dwBytesTransferred);	// 수신 버퍼의 데이터 길이 수정
	pThis->CloseOp(pThis, pIoData);	// OV operation 완료
	return pThis->CompRecvOp(pThis, pCtx, pIoData, 0);	// 사용자 함수 호출
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

// TCP용 MS 윈속 확장 함수들 로딩
// [in] pCtx: 로딩할 Context(소켓)
// [retVal] FALSE: 로딩 실패
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

// 소켓바인드 및 리슨 이후 작업
// [in] pCtx: 리스닝 소켓의 Context
CNetServer::RET_CODE CTcpServer::StartListen(CNetListenCtx* pListenCtx)
{
	CTcpServer* pTcpServer = (CTcpServer*)this;
	CTcpListenCtx* pTcpListenCtx = (CTcpListenCtx*)pListenCtx;	
	CTcpServerCfg* pTcpCfg = (CTcpServerCfg*)m_pCfg;
	CNetServer::RET_CODE rc = CNetServer::E_UNKNOWN;
	
	// TCP 리슨소켓에 FD_ACCEPT 알림 이벤트 등록
	int nRetCode = WSAEventSelect(pTcpListenCtx->m_Socket, pTcpListenCtx->m_hFdAcceptEvt, FD_ACCEPT);
	if (nRetCode == SOCKET_ERROR)
	{
		m_pTrace->WriteErrorMsg(_T("WSAEventSelect failed: %d\n"), WSAGetLastError());
		return CNetServer::E_FAIL_INIT_API;
	}
	
	// 리슨 소켓에서 PostAcceptOp 실행	
	for (int i = 0; i < pTcpCfg->GetMinPendingAccepts(); i++)
	{
		rc = PostAcceptOp(pTcpServer, pTcpListenCtx);
		if (rc != CNetServer::E_SUCCESS)
			m_pTrace->WriteErrorMsg(_T("StartListen: PostAcceptOp failed: %d\n"), rc);
	}

	return CNetServer::E_SUCCESS;
}

// TcpClientCtx 및 NetIoData 를 생성한 후, Accept 대기 시작
CNetServer::RET_CODE CTcpServer::PostAcceptOp(CTcpServer* pTcpThis, CTcpListenCtx* pTcpListenCtx)
{
	CNetServer::RET_CODE retVal = CNetServer::E_UNKNOWN;
	CTcpServerCfg* pTcpCfg = (CTcpServerCfg*)pTcpThis->m_pCfg;
	CTcpServerStat* pTcpStat = (CTcpServerStat*)pTcpThis->m_pStat;
	CTcpClientCtxMan* pTcpCtxMan = (CTcpClientCtxMan*)pTcpCfg->GetClientCtxMan();
	CNetIoDataMan* pIoDataMan = pTcpCfg->GetIoDataMan();

	CTcpClientCtx* pTcpCtx = NULL;
	CNetIoData* pIoData = NULL;
	
	// ClientCtx 와 NetIoData 생성
	pTcpCtx = (CTcpClientCtx*)pTcpCtxMan->AllocCtx();
	if (pTcpCtx == NULL)
	{
		m_pTrace->WriteErrorMsg(_T("PostAcceptOp: CTcpClientCtxMan::AllocCtx() failed\n"));
		return CNetServer::E_FAIL_ALLOC_CTX;
	}
	pTcpCtx->m_pListenCtx = pTcpListenCtx;	// 클라이언트 contxt에 리스너 객체 연결

	pIoData = pIoDataMan->AllocData();
	if (pIoData == NULL)
	{
		pTcpCtxMan->DeallocCtx(pTcpCtx);

		m_pTrace->WriteErrorMsg(_T("PostAcceptOp: CNetIoDataMan::AllocData() failed\n"));
		return CNetServer::E_FAIL_ALLOC_IODATA;
	}
	pIoData->m_pNetCtx = pTcpCtx;	// IO data에 클라이언트 context 정보를 연결


	// AcceptEx 실행
	int nRetCode = 0;
	DWORD dwRecvBytes = 0;

	// Accept 전에 Client 소켓을 미리 생성
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
								pIoData->m_pBuf->GetAvailMemPtr(pTcpCfg->GetAcceptExBufSize()),	// Page(4KByte) 크기 배수
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

	// AccepEx 대기 성공
	pTcpListenCtx->AddPendingAccept(pIoData);
	pTcpStat->IncrementPendingAcceptCount();

	return CNetServer::E_SUCCESS;
}

// Control 스레드의 메인 함수
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
	
	// ForceClose 루틴을 사용하지 않으면, WaitFor 무한대기
	dwTimeOut = (bUseForceConn ? nForceCloseIntervalTimeOut * 1000 : INFINITE);
	ZeroMemory(hWaitEvts, sizeof(HANDLE)*MAXIMUM_WAIT_OBJECTS);

	// WaitFor 이벤트들 추가
	hWaitEvts[i++] = pTcpCfg->GetExitEvt();

	for (VCT_PNETLISTENCTX_IT it = pTcpListenCtxMan->m_NetListenCtxList.begin(); it != pTcpListenCtxMan->m_NetListenCtxList.end(); ++it)
	{
		pTcpListenCtx = (CTcpListenCtx*)(*it);
		hWaitEvts[i++] = pTcpListenCtx->m_hFdAcceptEvt;
	}

	// WaitFor 대기
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
			// ForceCloseNoneLastOpConn 루틴 실행
			nForceClosedConn = ForceCloseNoneLastOpConn(pTcpClientCtxMan, pIoDataMan, nForceCloseNoneLastOpConnTimeOut);
			
			// ForceClosePendingAccept 루틴 실행
			// nMinPendingAccepts 이상은 대기 종료 시킴 
			nForceClosedAccept = ForceClosePendingAccept(pTcpThis, nMinPendingAccepts, nForceClosePendingAcceptTimeOut);

			pTrace->WriteDebugMsg(_T("CTcpServer::ControlThreadMain: ForceCloseNoneLastOpConn: %d, ForceClosePendingAccept: %d\n"), nForceClosedConn, nForceClosedAccept);
		}
		else if (dwWaitRet == WAIT_OBJECT_0)
		{
			// 서버 종료 
			pTrace->WriteDebugMsg(_T("CTcpServer::ControlThreadMain: received ExitEvt\n"));
			PostQueuedCompletionStatus(pTcpThis->m_hPort, 0, NULL, NULL);
			break;
		}
		else if (dwWaitRet > WAIT_OBJECT_0 && dwWaitRet < WAIT_OBJECT_0 + i)
		{
			// FD_ACCEPT 시그널 상황
			nListenCtxIdx = dwWaitRet - WAIT_OBJECT_0 - 1;
			if (nListenCtxIdx >= nListenCtxListSize)
			{
				m_pTrace->WriteErrorMsg(_T("CTcpServer::ControlThreadMain: Invalid ListenCtxIdx(%d), TotalSize: %d\n"), nListenCtxIdx, nListenCtxListSize);
				break;
			}
			pTcpListenCtx = (CTcpListenCtx*)pTcpListenCtxMan->m_NetListenCtxList[nListenCtxIdx];

			// 최대 가능 개수만큼 Accept 대기시킴
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

// 서버 전체에서, 연결이 없거나 혹은 연결후 최초 작업이 없는 패딩중인 AceeptEx 소켓들을 nMinPendingAccepts 개수까지 강제 종료
// [in] nMinPendingAccepts: Accept 대기 최소 개수
// [in] nPendingAcceptTimeOut: 연결이 없거나, 연결후 최초 작업이 없는 소켓들의 강제종료 시간(초)
//							AcceptEx는 연결과 수신이 동시에 이루어 지는데, 연결만 맺고 수신이 없는 클라이언트들 강제 종료 (DOS 방지)
// [retVal] int: 종료된 연결 개수
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


		EnterCriticalSection(&pTcpListenCtx->m_Cs); // PendingAcceptIoList 동기화

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
				// 클라이언트 소켓에 연결이 없거나 혹은 연결후 작업이 없는 소켓들은 종료
				// 소켓이 종료되면, completion 스레드에서 팬딩된 AcceptEx 호출이 실패 처리됨
				if ((optVal != 0xFFFFFFFF) && (optVal > nPendingAcceptTimeOut))
				{
					// 최소 대기 설정값 확인
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

// 서버 전체에서, 마지막 작업후 작동하지 않는 클라이언트들을 강제 종료
// [in] pTcpClientCtxMan: 클라이언트 연결 종료후 context 삭제를 위한 ClientCtxMan
// [in] pIoDataMan: IO data 생성을 위한 CNetIoDataMan
// [in] nNoneLastOpConnTimeOut: 마지막 작업(Recv/Send) 이후 작동하지 않는 클라이언트들의 강제 종료(초)
// [retVal] int: 종료된 연결 개수
int CTcpServer::ForceCloseNoneLastOpConn(CTcpClientCtxMan* pTcpClientCtxMan, CNetIoDataMan* pIoDataMan, int nNoneLastOpConnTimeOut)
{
	EnterCriticalSection(&pTcpClientCtxMan->m_Cs);
	




	LeaveCriticalSection(&pTcpClientCtxMan->m_Cs);

	return 0;
}

// FD_ACCEPT 상황이며, 최대한 많은 Aceept 를 대기 시키도록 함
// [in] pTcpListenCtx: 리슨 객체
// [in] nMaxPendingAccepts: Accept 가능한 최대 개수
// [retVal] Accept 대기 시킨 개수
int CTcpServer::BurstPendingAccepts(CTcpServer* pTcpThis, CTcpListenCtx* pTcpListenCtx, int nMaxPendingAccepts)
{
	// non-paged pool 부족 에러 대처 필요
	// 총 커넥션당 2kb 로 잡고, Windows NT는 128M, Windows 2000 이상은 물리적메모리의 1/4 까지 할당 가능 (p229)

	CNetServer::RET_CODE rc = CNetServer::E_UNKNOWN;
	int nCreatedAccept = 0, nNeedAccept = 0;
	nNeedAccept = nMaxPendingAccepts - pTcpListenCtx->GetPendingAcceptIoCount();

	// 리슨 소켓에서 PostAcceptOp 실행	
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


// 강제 연결 종료를 요청
// 클라이언트의 현재 IO 작업과 상관없이 강제로 연결 종료 요청
void CTcpServer::ForceCloseConn(CTcpClientCtx* pTcpCtx)
{
	EnterCriticalSection(&pTcpCtx->m_Cs);
	
	// 소켓을 종료시켜서 completion 스레드에서 대기중인 클라이언트의 I/O 작업들이 모두 실패 처리되도록 함
	if (pTcpCtx->m_Socket != INVALID_SOCKET)
	{
		// graceful close
		shutdown(pTcpCtx->m_Socket, SD_BOTH);

		/*
		// LINGER 옵션 사용
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

// pNetBuf의 뒷 부분과 pFindBuf 를 nFindBufSize 만큼 memcmp 비교하여 동일한지를 반환
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

// 성공여부에 상관없이 Overlapped IO 작업 종료 와 커넥션 종료를 요청
// [in] pIoData: 완료된 IO data
// [in] bRequestCloseConn: 커넥션 종료 요청 여부
void CTcpServer::CloseOp(CNetServer* pThis, CNetIoData* pIoData, BOOL bRequestCloseConn)
{
	CTcpServer* pTcpThis = (CTcpServer*)pThis;
	CTcpServerCfg* pTcpCfg = (CTcpServerCfg*)pTcpThis->m_pCfg;
	CTcpServerStat* pTcpStat = (CTcpServerStat*)pTcpThis->m_pStat;
	CNetIoDataMan* pIoDataMan = pTcpCfg->GetIoDataMan();
	CTcpClientCtxMan* pTcpCtxMan = (CTcpClientCtxMan*)pTcpCfg->GetClientCtxMan();
	CTcpClientCtx* pTcpCtx = (CTcpClientCtx*)pIoData->m_pNetCtx;
	
	EnterCriticalSection(&pTcpCtx->m_Cs);

	time(&pTcpCtx->m_LastTime);		// 작업시간 변경

	// Operation 완료 설정
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

	// 종료 요청 처리
	// 소켓을 종료시켜서 completion 스레드에서 대기중인 클라이언트의 I/O 작업들이 모두 실패 처리되도록 함
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

	// 종료요청이 있고, I/O작업이 모두 종료되었으면 ClientCtx 삭제
	if (pTcpCtx->m_bRequestedClose && pTcpCtx->m_nOutstandingRecv == 0 && pTcpCtx->m_nOutstandingSend == 0)
	{
		CTcpListenCtx* pTcpListenCtx = (CTcpListenCtx*)pTcpCtx->m_pListenCtx;

		pTcpCtxMan->DeallocCtx(pTcpCtx);

		// 연결 종료 및 Ctx 해제 완료된 상태
		CTcpServerStat* pTcpStat = (CTcpServerStat*)pTcpThis->m_pStat;
		pTcpStat->DecrementConnectionCount();


		// 새로운 Accept 실행 여부 결정
		if (pTcpListenCtx->GetPendingAcceptIoCount() < pTcpCfg->GetMinPendingAccepts())
		{
			// Accept 대기 개수가 최소값 미만이면 re-PostAccept
			CNetServer::RET_CODE rc;
			rc = PostAcceptOp(pTcpThis, pTcpListenCtx);
			if (rc != CNetServer::E_SUCCESS)
				m_pTrace->WriteErrorMsg(_T("CloseOp: PostAcceptOp failed: %d\n"), rc);
		}
	}

	LeaveCriticalSection(&pTcpCtx->m_Cs);
	
	if (pTcpCtx->m_bRequestedClose)
	{
		// 연결 종료 요청이후의 완료된 IO data는 삭제시킴
		pIoDataMan->DeallocData(pIoData);
	}
}

// 해당 소켓에 overlapped receive operation 실행
// [in] pCtx: 작업 소켓 context
// [in] pIoData: IO 작업별 데이터
// [in] dwOpBufSize: IO 작업에 필요한 버퍼 크기
// [retVal] E_SUCCESS - 성공
//			기타	  - 실패, 요청 무시됨
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
			// WSARecv 실패

			if (nLastError == WSAENOBUFS)
			{
				// locked page 오류
				// IO 작업을 줄이거나, 불필요한 연결을 닫어야 할 상황.
				m_pTrace->WriteErrorMsg(_T("PostRecvOp: locked page error: WSAENOBUFS\n"));
			}

			m_pTrace->WriteDebugMsg(_T("PostRecvOp: WSARecv* failed: %d\n"), nLastError);
			retVal = CNetServer::E_FAIL_POST_RECV;
		}
	}

	if (retVal != CNetServer::E_FAIL_POST_RECV)
	{
		// WSARecv 성공
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
			// WSASend 실패

			if (nLastError == WSAENOBUFS)
			{
				// locked page 오류
				// IO 작업을 줄이거나, 불필요한 연결을 닫어야 할 상황.
				m_pTrace->WriteErrorMsg(_T("PostSendOp: locked page error: WSAENOBUFS\n"));
			}
			
			m_pTrace->WriteDebugMsg(_T("PostSendOp: WSASend* failed: %d [internal = %d]\n"), nLastError, pIoData->m_Ol.Internal);
			retVal = CNetServer::E_FAIL_POST_SEND;
		}
	}

	if (retVal != CNetServer::E_FAIL_POST_SEND)
	{
		// WSARecv 성공
		pCtx->m_nOutstandingSend++;
		pThis->m_pStat->IncrementOutstandingSends();
		retVal = CNetServer::E_SUCCESS;
    }
  
	LeaveCriticalSection(&pCtx->m_Cs);

    return retVal;
}

// [in] nWsaLastError: completion 스레드에서의 완료 결과 코드 전달
CNetServer::RET_CODE CTcpServer::CompRecvOp(CNetServer* pThis, CNetCtx* pCtx, CNetIoData* pIoData, int nWsaLastError)
{
	CNetServer::RET_CODE retVal = CNetServer::E_UNKNOWN;

	CTcpServer* pTcpThis = (CTcpServer*)pThis;
	CNetBuf* pNetBuf = pIoData->m_pBuf;

	BYTE endMark[2] = {'\r', '\n'};
	if (IsSameEndBuf(pNetBuf, endMark, 2) == FALSE)
	{
		// 상황1. CTcpServerCfg::SetAcceptExBufSize(0) 으로 셋팅하여 0 byte를 수신한 경우.
		// if (pIoData->m_Op == OP_ACCEPT && nWsaLastError == WSAEWOULDBLOCK)
		pNetBuf->Clear();

		// 상황2. 큰 데이터이고 버퍼가 부족하여 다 가져오지 못한 경우, 받을 데이터가 더 있음

		// 버퍼를 초기화 시키지 않고, 이 버퍼에 덧 붙여 받도록 함
		//pNetBuf->Clear();

		retVal = PostRecvOp(pThis, pCtx, pIoData, 4 * 1024);
		if (retVal != CNetServer::E_SUCCESS)
		{
			// 수신 작업 오류!!, 연결 종료
			CloseOp(pThis, pIoData, TRUE);
			return CNetServer::E_SUCCESS;
		}
		
		printf("수신 상태 성공");
		return CNetServer::E_SUCCESS;

	}


	// 1) 다음 데이터 전송을 위해서, PostSendOp 호출
	{
		// 버퍼 초기화
		pNetBuf->Clear();


		// 보낼 데이터 구성
		char szTest[128];
		strcpy_s(szTest, _countof(szTest), "OK 1234567890\r\n");

		// 사용자 버퍼 복사
		pNetBuf->AddData(szTest, strlen(szTest));
		
		// 혹은, NetBuf를 직접 이용한 데이터 기록
		strcpy_s((char*)pNetBuf->GetAvailMemPtr(128), 128, "OK 1234567890\r\n");
		pNetBuf->ResetDataSize(0, strlen("OK 1234567890\r\n"));


		// 데이터 전송
		retVal = PostSendOp(pThis, pCtx, pIoData);
		if (retVal != CNetServer::E_SUCCESS)
		{
			// 전송 작업 오류!!, 연결 종료
			CloseOp(pThis, pIoData, TRUE);
			return CNetServer::E_SUCCESS;
		}

		printf("전송 상태 성공");
		return CNetServer::E_SUCCESS;
	}
	
	// 2) 데이터 수신을 위해서, PostRecvOp 호출하여 수신 상태로 대기
	{
		// 버퍼 초기화
		pNetBuf->Clear();

		retVal = PostRecvOp(pThis, pCtx, pIoData, 4 * 1024);
		if (retVal != CNetServer::E_SUCCESS)
		{
			// 수신 작업 오류!!, 연결 종료
			CloseOp(pThis, pIoData, TRUE);
			return CNetServer::E_SUCCESS;
		}
		
		printf("수신 상태 성공");
		return CNetServer::E_SUCCESS;
	}

	// 3) 연결 종료
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

	
	// 1) 데이터 수신을 위해서, PostRecvOp 호출하여 수신 상태로 대기
	{
		// 버퍼 초기화
		pNetBuf->Clear();

		retVal = PostRecvOp(pThis, pCtx, pIoData, 4 * 1024);
		if (retVal != CNetServer::E_SUCCESS)
		{
			// 수신 작업 오류!!, 연결 종료
			CloseOp(pThis, pIoData, TRUE);
			return CNetServer::E_SUCCESS;
		}
		
		printf("수신 상태 성공");
		return CNetServer::E_SUCCESS;
	}

	// 2) 보낼 데이터가 남아 있어서, PostSendOp 호출
	{
		// 버퍼 초기화
		pNetBuf->Clear();

		// 보낼 데이터 구성
		char szTest[128];
		strcpy_s(szTest, _countof(szTest), "OK 1234567890\r\n");

		// 사용자 버퍼 복사
		pNetBuf->AddData(szTest, strlen(szTest));
		
		// 혹은, NetBuf를 직접 이용한 데이터 기록
		strcpy_s((char*)pNetBuf->GetAvailMemPtr(128), 128, "OK 1234567890\r\n");
		pNetBuf->ResetDataSize(0, strlen("OK 1234567890\r\n"));


		// 데이터 전송
		retVal = PostSendOp(pThis, pCtx, pIoData);
		if (retVal != CNetServer::E_SUCCESS)
		{
			// 전송 작업 오류!!, 연결 종료
			CloseOp(pThis, pIoData, TRUE);
			return CNetServer::E_SUCCESS;
		}

		printf("전송 상태 성공");
		return CNetServer::E_SUCCESS;
	}


	// 3) 연결 종료
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

// Control 스레드의 메인 함수
unsigned CUdpServer::ControlThreadMain(CNetServer* pNetServer)
{ 
	CUdpServer* pUdpThis = (CUdpServer*)pNetServer;
	CUdpServerCfg* pUdpCfg = (CUdpServerCfg*)pUdpThis->m_pCfg;
	CNetTrace* pTrace = pUdpThis->m_pTrace;
	DWORD dwWaitRet = 0;

	dwWaitRet = WaitForSingleObject(pUdpCfg->GetExitEvt(), INFINITE);
	if (dwWaitRet == WAIT_OBJECT_0)
	{
		// 서버 종료 
		pTrace->WriteDebugMsg(_T("CUdpServer::ControlThreadMain: received ExitEvt\n"));
		PostQueuedCompletionStatus(pUdpThis->m_hPort, 0, NULL, NULL);
	}
	else
	{
		m_pTrace->WriteErrorMsg(_T("CUdpServer::ControlThreadMain: WaitForSingleObject failed(dwWaitRet: %d)\n"), dwWaitRet);
	}
	
	return 0;
}


// MS 윈속 확장 함수들 로딩
// [in] pCtx: 연결할 소켓의 Context
// [retVal] FALSE: 로딩 실패
BOOL CUdpServer::LoadWinSockExFunc(CNetListenCtx* pListenCtx) 
{
	CUdpListenCtx* pUdpListenCtx = (CUdpListenCtx*)pListenCtx;
	DWORD dwBytes = 0;
	int nOptVal = 0;
	int nRetCode = 0;

	// SIO_UDP_CONNRESET 리셋
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

// 소켓바인드 및 리슨 이후에 호출
// [in] pListenCtx: 리스닝 소켓의 Context
CNetServer::RET_CODE CUdpServer::StartListen(CNetListenCtx* pListenCtx) 
{
	CUdpListenCtx* pUdpListenCtx = (CUdpListenCtx*)pListenCtx;
	CUdpServerCfg* pUdpCfg = (CUdpServerCfg*)m_pCfg;
	CNetIoDataMan* pIoDataMan = (CNetIoDataMan*)pUdpCfg->GetIoDataMan();
	CNetIoData* pIoData = NULL;
	CNetServer::RET_CODE rc = CNetServer::E_UNKNOWN;

	// 리슨 소켓에서 PostRecvOp 실행	
	for (int i = 0; i < pUdpCfg->GetMinPendingRecvs(); i++)
	{
		pIoData = pIoDataMan->AllocData();
		pIoData->m_pNetCtx = pUdpListenCtx;

		// CNetBuf의 여유 크기만큼을 Overlapped 버퍼 크기로 설정
		rc = PostRecvOp(this, pUdpListenCtx, pIoData, pIoData->m_pBuf->GetAvailMemSize());   

		if (rc != CNetServer::E_SUCCESS)
		{
			m_pTrace->WriteErrorMsg(_T("StartListen: PostRecvOp failed: %d\n"), rc);
			pIoDataMan->DeallocData(pIoData);
		}
	}

	return CNetServer::E_SUCCESS; 
}

// 성공여부에 상관없이 Overlapped IO 작업 종료 와 커넥션 종료를 요청
// bRequestCloseConn 요청 이후에만 pIoData 객체 삭제됨
// [in] pIoData: 완료된 IO data
// [in] bRequestCloseConn: TCP 커넥션 종료 요청 여부
void CUdpServer::CloseOp(CNetServer* pThis, CNetIoData* pIoData, BOOL bRequestCloseConn)
{
	CUdpServer* pUdpThis = (CUdpServer*)pThis;
	CUdpServerCfg* pUdpCfg = (CUdpServerCfg*)pUdpThis->m_pCfg;
	CUdpServerStat* pUdpStat = (CUdpServerStat*)pUdpThis->m_pStat;
	CNetIoDataMan* pIoDataMan = pUdpCfg->GetIoDataMan();
	CUdpListenCtx* pUdpListenCtx = (CUdpListenCtx*)pIoData->m_pNetCtx;
		
	// Operation 완료 설정
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
		// Recv 대기 개수가 작으면 완료된 op를 초기화 시키고, 다시 Recv 대기
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

// 해당 소켓에 overlapped receive operation 실행
// [in] pCtx: 작업 소켓 context
// [in] pIoData: IO 작업별 데이터
// [in] dwOpBufSize: IO 작업에 필요한 버퍼 크기
// [retVal] E_SUCCESS - 성공
//			기타	  - 실패, 요청 무시됨
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
			// WSARecv 실패

			if (nLastError == WSAENOBUFS)
			{
				// locked page 오류
				// IO 작업을 줄이거나, 불필요한 연결을 닫어야 할 상황.
				m_pTrace->WriteErrorMsg(_T("PostRecvOp: locked page error: WSAENOBUFS\n"));
			}

			m_pTrace->WriteDebugMsg(_T("PostRecvOp: WSARecv* failed: %d\n"), nLastError);
			retVal = CNetServer::E_FAIL_POST_RECV;
		}
	}

	if (retVal != CNetServer::E_FAIL_POST_RECV)
	{
		// WSARecv 성공
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

	// RecvFrom 에서 받은 클라이언트 IP를 SendTo시에도 전달함
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
			// WSASend 실패

			if (nLastError == WSAENOBUFS)
			{
				// locked page 오류
				// IO 작업을 줄이거나, 불필요한 연결을 닫어야 할 상황.
				m_pTrace->WriteErrorMsg(_T("PostSendOp: locked page error: WSAENOBUFS\n"));
			}
			
			m_pTrace->WriteDebugMsg(_T("PostSendOp: WSASend* failed: %d [internal = %d]\n"), nLastError, pIoData->m_Ol.Internal);
			retVal = CNetServer::E_FAIL_POST_SEND;
		}
	}

	if (retVal != CNetServer::E_FAIL_POST_SEND)
	{
		// WSARecv 성공
		pUdpListenCtx->m_nOutstandingSend++;
		pThis->m_pStat->IncrementOutstandingSends();
		retVal = CNetServer::E_SUCCESS;
    }
  
	LeaveCriticalSection(&pUdpListenCtx->m_Cs);

    return retVal;
}