#include "StdAfx.h"
#include <network/server/NetServerStat.h>

using namespace NetServer;

////////////////////////////////////////////////////////////////////////////////////////////////////
//						CNetServerStat
////////////////////////////////////////////////////////////////////////////////////////////////////
CNetServerStat::CNetServerStat(BOOL bUseStaticsPerSec)
{
	m_bUseStaticsPerSec = bUseStaticsPerSec;
	
	m_nStartTime = 0;
	m_nStartTimeLast = 0;
			
	m_nCompletionThread = 0;

	m_nRecvBytesTotal = 0;
	m_nRecvBytesLast = 0;
	m_nRecvBps = 0;
	m_nRecvAverageBps = 0;
	
	m_nSentBytesTotal = 0;
	m_nSentBytesLast = 0;
	m_nSentBps = 0;
	m_nSentAverageBps = 0;
	
	m_nConnectionCount = 0;
	m_nConnectionCountTotal = 0;
	m_nConnectionCountLast = 0;
	m_nCps = 0;
	m_nAverageCps = 0;
	
	m_nPendingAcceptCount = 0;
	m_nOutstandingRecvs = 0;
	m_nOutstandingSends = 0;
}

CNetServerStat::~CNetServerStat(void)
{
}

// 초당 통계 정보 수집
void CNetServerStat::CalculateStaticsPerSecond()
{
	if (!m_bUseStaticsPerSec)
		return;

	LONG nCurrentTime = GetTickCount(), nElapsed = 0;
	
	// 평균 통계
	nElapsed = (nCurrentTime - m_nStartTime) / 1000;
	if (nElapsed == 0)
		return;
	
	m_nSentAverageBps = m_nRecvBytesTotal / nElapsed;
	m_nRecvAverageBps = m_nRecvBytesTotal / nElapsed;
	m_nAverageCps = m_nConnectionCountTotal / nElapsed;

	// 초당 통계
	nElapsed = (nCurrentTime - m_nStartTimeLast) / 1000;
	if (nElapsed == 0)	
		return;
	
	m_nSentBps = m_nSentBytesLast / nElapsed;
	m_nRecvBps = m_nRecvBytesLast / nElapsed;
	m_nCps = m_nConnectionCountLast / nElapsed;

	InterlockedExchange(&m_nSentBytesLast, 0);
    InterlockedExchange(&m_nRecvBytesLast, 0);
    InterlockedExchange(&m_nConnectionCountLast, 0);
}
