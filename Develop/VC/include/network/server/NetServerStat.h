#pragma once

namespace NetServer
{
};
using namespace NetServer;

namespace NetServer
{
	////////////////////////////////////////////////////////////////////////////////////////////////////
	//				CNetServerStat
	//
	//  - NetServer 성능을 위해, 빠른 함수 반환 필요
	//
	class CNetServerStat
	{
	public:
		// [in] bUseStaticsPerSec: 초당 통계 정보 수집 여부
		CNetServerStat(BOOL bUseStaticsPerSec = FALSE);
		virtual ~CNetServerStat(void);

		// 통계 정보 수집 시작
		void StartStatics();

		// 초당 통계 정보 수집
		void CalculateStaticsPerSecond();

		//////////////////////////////////////////////////////////
		// Completion 스레드 개수
		//
		void IncrementCompletionCount();
		void DecrementCompletionCount();
		LONG GetCurrentCompletionCount();

		//////////////////////////////////////////////////////////
		// 수신 bytes
		//
		void AddRecvBytes(LONG nBytes);
		LONG GetTotalRecvBytes();			// 총 수신 bytes
		LONG GetRecvBps();					// 현재 초당 수신 bytes
		LONG GetRecvAverageBps();			// 평균 초당 수신 bytes

		//////////////////////////////////////////////////////////
		// 송신 bytes
		//
		void AddSentBytes(LONG nBytes);
		LONG GetTotalSentBytes();				// 총 송신 bytes
		LONG GetSentBps();					// 현재 초당 송신 bytes
		LONG GetSentAverageBps();			// 평균 초당 송신 bytes

		//////////////////////////////////////////////////////////
		// 송/수신 완료 대기 개수
		//
		void IncrementOutstandingRecvs();
		void DecrementOutstandingRecvs();
		LONG GetCurrentOutstandingRecvs();

		void IncrementOutstandingSends();
		void DecrementOutstandingSends();
		LONG GetCurrentOutstandingSends();


		//////////////////////////////////////////////////////////
		// TCP용, Accept 대기중인 클라이언트 소켓 개수
		//
		void IncrementPendingAcceptCount();
		void DecrementPendingAcceptCount();
		LONG GetCurrentPendingAcceptCount();


		//////////////////////////////////////////////////////////
		// TCP용, 접속 개수
		//
		void IncrementConnectionCount();
		void DecrementConnectionCount();
		LONG GetCurrentConnectionCount();	// 현재 연결 개수
		LONG GetTotalConnectionCount();		// 총 연결 수
		LONG GetCps();						// 현재 초당 연결수
		LONG GetAverageCps();				// 평균 초당 연결수
	
	protected:
		BOOL m_bUseStaticsPerSec;
		LONG m_nStartTime;
		LONG m_nStartTimeLast;
				
		// 통계 정보
		LONG m_nCompletionThread;			// completion 스레드 수

		LONG m_nRecvBytesTotal;				// 전체 수신 bytes
		LONG m_nRecvBytesLast;
		LONG m_nRecvBps;
		LONG m_nRecvAverageBps;
		
		LONG m_nSentBytesTotal;				// 전체 송신 bytes
		LONG m_nSentBytesLast;
		LONG m_nSentBps;
		LONG m_nSentAverageBps;
		
		LONG m_nConnectionCount;			// 현재 TCP 접속 개수
		LONG m_nConnectionCountTotal;		// 전체 TCP 접속 개수
		LONG m_nConnectionCountLast;
		LONG m_nCps;
		LONG m_nAverageCps;
		
		LONG m_nPendingAcceptCount;		// TCP 접속 대기중 개수
		LONG m_nOutstandingRecvs;		// 수신 대기중 개수
		LONG m_nOutstandingSends;		// 송신 대기중 개수
	};

	
	////////////////////////////////////////////////////////////////////////////////////////////////////
	//				CTcpServerStat
	//
	class CTcpServerStat : public CNetServerStat
	{
	public:
		CTcpServerStat() {}
		~CTcpServerStat() {}
	};


	////////////////////////////////////////////////////////////////////////////////////////////////////
	//				CUdpServerStat
	//
	class CUdpServerStat : public CNetServerStat
	{
	public:
		CUdpServerStat() {}
		~CUdpServerStat() {}
	};

};



////////////////////////////////////////////////////////////////////////////////////////////////////
// 통계 정보 수집 시작
inline void CNetServerStat::StartStatics()
{
	if (m_nStartTime == 0)
		m_nStartTime = GetTickCount();
}

inline void CNetServerStat::IncrementCompletionCount()
{
	InterlockedIncrement(&m_nCompletionThread);
}

inline void CNetServerStat::DecrementCompletionCount()
{
	InterlockedDecrement(&m_nCompletionThread);
}

inline LONG CNetServerStat::GetCurrentCompletionCount()
{
	return m_nCompletionThread;
}

//////////////////////////////////////////////////////////
// 수신 bytes
//
inline void CNetServerStat::AddRecvBytes(LONG nBytes)
{
	InterlockedExchangeAdd(&m_nRecvBytesTotal, nBytes);
	if (m_bUseStaticsPerSec)
		InterlockedExchangeAdd(&m_nRecvBytesLast, nBytes);
}

inline LONG CNetServerStat::GetTotalRecvBytes()
{
	return m_nRecvBytesTotal;
}

inline LONG CNetServerStat::GetRecvBps()
{
	return m_nRecvBps;
}

inline LONG CNetServerStat::GetRecvAverageBps()
{
	return m_nRecvAverageBps;
}

//////////////////////////////////////////////////////////
// 송신 bytes
//
inline void CNetServerStat::AddSentBytes(LONG nBytes)
{
	InterlockedExchangeAdd(&m_nSentBytesTotal, nBytes);
	if (m_bUseStaticsPerSec)
		InterlockedExchangeAdd(&m_nSentBytesLast, nBytes);
}

inline LONG CNetServerStat::GetTotalSentBytes()
{
	return m_nSentBytesTotal;
}

inline LONG CNetServerStat::GetSentBps()
{
	return m_nSentBps;
}

inline LONG CNetServerStat::GetSentAverageBps()
{
	return m_nSentAverageBps;
}

//////////////////////////////////////////////////////////
// 송/수신 완료 대기 개수
//
inline void CNetServerStat::IncrementOutstandingRecvs()
{
	InterlockedIncrement(&m_nOutstandingRecvs);
}

inline void CNetServerStat::DecrementOutstandingRecvs()
{
	InterlockedDecrement(&m_nOutstandingRecvs);
}

inline LONG CNetServerStat::GetCurrentOutstandingRecvs()
{
	return m_nOutstandingRecvs;
}

inline void CNetServerStat::IncrementOutstandingSends()
{
	InterlockedIncrement(&m_nOutstandingRecvs);
}

inline void CNetServerStat::DecrementOutstandingSends()
{
	InterlockedDecrement(&m_nOutstandingRecvs);
}

inline LONG CNetServerStat::GetCurrentOutstandingSends()
{
	return m_nOutstandingRecvs;
}

//////////////////////////////////////////////////////////
// TCP용, Accept 대기중인 클라이언트 소켓 개수
//
inline void CNetServerStat::IncrementPendingAcceptCount()
{
	InterlockedIncrement(&m_nPendingAcceptCount);
}

inline void CNetServerStat::DecrementPendingAcceptCount()
{
	InterlockedDecrement(&m_nPendingAcceptCount);
}

inline LONG CNetServerStat::GetCurrentPendingAcceptCount()
{
	return m_nPendingAcceptCount;
}

//////////////////////////////////////////////////////////
// TCP용, 접속 개수
//
inline void CNetServerStat::IncrementConnectionCount()
{
	InterlockedIncrement(&m_nConnectionCount);
	InterlockedIncrement(&m_nConnectionCountTotal);
	if (m_bUseStaticsPerSec)
		InterlockedIncrement(&m_nConnectionCountLast);
}

inline void CNetServerStat::DecrementConnectionCount()
{
	InterlockedDecrement(&m_nConnectionCount);
}

inline LONG CNetServerStat::GetCurrentConnectionCount()
{
	return m_nConnectionCount;
}

inline LONG CNetServerStat::GetTotalConnectionCount()
{
	return m_nConnectionCountTotal;
}

inline LONG CNetServerStat::GetCps()
{
	return m_nCps;
}

inline LONG CNetServerStat::GetAverageCps()
{
	return m_nAverageCps;
}