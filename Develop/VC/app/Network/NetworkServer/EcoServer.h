#pragma once

#include <network/server/NetServer.h>
using namespace NetServer;

class CEcoServer : public CTcpServer
{
public:
	CEcoServer(CTcpServerCfg* pCfg, CTcpServerStat* pStat, CNetTrace* pTrace);
	~CEcoServer(void);

	// 요청한 operation 작업 완료후 사용자 함수 호출
	// [retVal] E_REQUEST_CLOSECONN - 연결 종료 요청
	// [in] nWsaLastError: completion 스레드에서의 완료 결과 코드 전달
	RET_CODE CompRecvOp(CNetServer* pThis, CNetCtx* pCtx, CNetIoData* pIoData, int nWsaLastError);
	RET_CODE CompSendOp(CNetServer* pThis, CNetCtx* pCtx, CNetIoData* pIoData, int nWsaLastError);
};
