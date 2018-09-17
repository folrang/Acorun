#include "StdAfx.h"
#include "EcoServer.h"

CEcoServer::CEcoServer(CTcpServerCfg* pCfg, CTcpServerStat* pStat, CNetTrace* pTrace) : CTcpServer(pCfg, pStat, pTrace)
{
}

CEcoServer::~CEcoServer(void)
{
}


// 요청한 operation 작업 완료후 사용자 함수 호출
// [retVal] E_REQUEST_CLOSECONN - 연결 종료 요청
// [in] nWsaLastError: completion 스레드에서의 완료 결과 코드 전달
CNetServer::RET_CODE CEcoServer::CompRecvOp(CNetServer* pThis, CNetCtx* pCtx, CNetIoData* pIoData, int nWsaLastError)
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
		//strcpy_s((char*)pNetBuf->GetAvailMemPtr(128), 128, "OK 1234567890\r\n");
		//pNetBuf->ResetDataSize(0, strlen("OK 1234567890\r\n"));


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
	
	/*
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
	*/

	/*
	// 3) 연결 종료
	{
		CloseOp(pThis, pIoData, TRUE);
		return CNetServer::E_SUCCESS;
	}
	*/


	return CNetServer::E_SUCCESS;
}

CNetServer::RET_CODE CEcoServer::CompSendOp(CNetServer* pThis, CNetCtx* pCtx, CNetIoData* pIoData, int nWsaLastError)
{
	CNetServer::RET_CODE retVal = CNetServer::E_UNKNOWN;

	CTcpServer* pTcpThis = (CTcpServer*)pThis;
	CNetBuf* pNetBuf = pIoData->m_pBuf;


	pNetBuf->Clear();

	return CNetServer::E_SUCCESS;
}