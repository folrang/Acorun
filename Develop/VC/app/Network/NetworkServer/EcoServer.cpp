#include "StdAfx.h"
#include "EcoServer.h"

CEcoServer::CEcoServer(CTcpServerCfg* pCfg, CTcpServerStat* pStat, CNetTrace* pTrace) : CTcpServer(pCfg, pStat, pTrace)
{
}

CEcoServer::~CEcoServer(void)
{
}


// ��û�� operation �۾� �Ϸ��� ����� �Լ� ȣ��
// [retVal] E_REQUEST_CLOSECONN - ���� ���� ��û
// [in] nWsaLastError: completion �����忡���� �Ϸ� ��� �ڵ� ����
CNetServer::RET_CODE CEcoServer::CompRecvOp(CNetServer* pThis, CNetCtx* pCtx, CNetIoData* pIoData, int nWsaLastError)
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
		//strcpy_s((char*)pNetBuf->GetAvailMemPtr(128), 128, "OK 1234567890\r\n");
		//pNetBuf->ResetDataSize(0, strlen("OK 1234567890\r\n"));


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
	
	/*
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
	*/

	/*
	// 3) ���� ����
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