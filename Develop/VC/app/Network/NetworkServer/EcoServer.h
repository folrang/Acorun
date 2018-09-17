#pragma once

#include <network/server/NetServer.h>
using namespace NetServer;

class CEcoServer : public CTcpServer
{
public:
	CEcoServer(CTcpServerCfg* pCfg, CTcpServerStat* pStat, CNetTrace* pTrace);
	~CEcoServer(void);

	// ��û�� operation �۾� �Ϸ��� ����� �Լ� ȣ��
	// [retVal] E_REQUEST_CLOSECONN - ���� ���� ��û
	// [in] nWsaLastError: completion �����忡���� �Ϸ� ��� �ڵ� ����
	RET_CODE CompRecvOp(CNetServer* pThis, CNetCtx* pCtx, CNetIoData* pIoData, int nWsaLastError);
	RET_CODE CompSendOp(CNetServer* pThis, CNetCtx* pCtx, CNetIoData* pIoData, int nWsaLastError);
};
