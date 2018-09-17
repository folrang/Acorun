#include "StdAfx.h"
#include <network/server/NetServerCfg.h>

//////////////////////////////////////////////////////////
//			CNetServerCfg
//////////////////////////////////////////////////////////
CNetServerCfg::CNetServerCfg(HANDLE hServerExitEvt, CNetListenCtxMan* pListenCtxMan, CNetClientCtxMan* pClientCtxMan, CNetIoDataMan* pIoDataMan)
{
	::GetSystemInfo(&m_SysInfo);
	
	m_hExitEvt = hServerExitEvt;
	m_pListenCtxMan = pListenCtxMan;
	m_pClientCtxMan = pClientCtxMan;
	m_pIoDataMan = pIoDataMan;

	Reset();
}

CNetServerCfg::~CNetServerCfg()
{
}

void CNetServerCfg::Reset()
{
	m_BindList.clear();

	m_nMinCompletionThread = DEFAULT_MIN_COMPLETION_THREAD;
	m_nMaxCompletionThread = DEFAULT_MAX_COMPLETION_THREAD;

	m_nNetClientCtxFreeListSize = DEFAULT_NETCLIENTCTX_FREELIST_SIZE;
	m_nNetIoDataFreeListSize = DEFAULT_NETIODATA_FREELIST_SIZE;
}

void CNetServerCfg::AddBindInfo(const addrinfo& ai, int nBacklog)
{
	BIND_INFO newItem;
	memcpy(&newItem.ai, &ai, sizeof(addrinfo));
	newItem.backlog = nBacklog;

	m_BindList.push_back(newItem);
}

//////////////////////////////////////////////////////////
//			CTcpServerCfg
//////////////////////////////////////////////////////////
CTcpServerCfg::CTcpServerCfg(HANDLE hServerExitEvt, CTcpListenCtxMan* pListenCtxMan, CTcpClientCtxMan* pClientCtxMan, CNetIoDataMan* pIoDataMan)
								: CNetServerCfg(hServerExitEvt, pListenCtxMan, pClientCtxMan, pIoDataMan)
{
}

CTcpServerCfg::~CTcpServerCfg()
{
}

void CTcpServerCfg::Reset()
{
	CNetServerCfg::Reset();

	m_nMinPendingAccepts = DEFAULT_MIN_PENDING_ACCEPTS;
	m_nMaxPendingAccepts = DEFAULT_MAX_PENDING_ACCEPTS;

	m_bUseForceCloseConn = DEFAULT_FORCECONN_USE;
	m_nForceCloseIntervalTimeOut = DEFAULT_FORCECONN_INTERVAL_TIMEOUT;
	m_nForceClosePendingAcceptTimeOut = DEFAULT_FORCECONN_PENDING_ACCEPT_TIMEOUT;
	m_nForceCloseNoneLastOpConnTimeOut = DEFAULT_FORCECONN_NONE_LAST_OP_CONN_TIMEOUT;
	
	m_nAcceptBufSize = DEFAULT_WSA_BUF_SIZE;
}

//////////////////////////////////////////////////////////
//			CUdpServerCfg
//////////////////////////////////////////////////////////
void CUdpServerCfg::Reset()
{
	m_nMinPendingRecvs = DEFAULT_MIN_PENDING_RECVS;
	m_nMaxPendingRecvs = DEFAULT_MAX_PENDING_RECVS;
}
