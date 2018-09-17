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
	//  - NetServer ������ ����, ���� �Լ� ��ȯ �ʿ�
	//
	class CNetServerStat
	{
	public:
		// [in] bUseStaticsPerSec: �ʴ� ��� ���� ���� ����
		CNetServerStat(BOOL bUseStaticsPerSec = FALSE);
		virtual ~CNetServerStat(void);

		// ��� ���� ���� ����
		void StartStatics();

		// �ʴ� ��� ���� ����
		void CalculateStaticsPerSecond();

		//////////////////////////////////////////////////////////
		// Completion ������ ����
		//
		void IncrementCompletionCount();
		void DecrementCompletionCount();
		LONG GetCurrentCompletionCount();

		//////////////////////////////////////////////////////////
		// ���� bytes
		//
		void AddRecvBytes(LONG nBytes);
		LONG GetTotalRecvBytes();			// �� ���� bytes
		LONG GetRecvBps();					// ���� �ʴ� ���� bytes
		LONG GetRecvAverageBps();			// ��� �ʴ� ���� bytes

		//////////////////////////////////////////////////////////
		// �۽� bytes
		//
		void AddSentBytes(LONG nBytes);
		LONG GetTotalSentBytes();				// �� �۽� bytes
		LONG GetSentBps();					// ���� �ʴ� �۽� bytes
		LONG GetSentAverageBps();			// ��� �ʴ� �۽� bytes

		//////////////////////////////////////////////////////////
		// ��/���� �Ϸ� ��� ����
		//
		void IncrementOutstandingRecvs();
		void DecrementOutstandingRecvs();
		LONG GetCurrentOutstandingRecvs();

		void IncrementOutstandingSends();
		void DecrementOutstandingSends();
		LONG GetCurrentOutstandingSends();


		//////////////////////////////////////////////////////////
		// TCP��, Accept ������� Ŭ���̾�Ʈ ���� ����
		//
		void IncrementPendingAcceptCount();
		void DecrementPendingAcceptCount();
		LONG GetCurrentPendingAcceptCount();


		//////////////////////////////////////////////////////////
		// TCP��, ���� ����
		//
		void IncrementConnectionCount();
		void DecrementConnectionCount();
		LONG GetCurrentConnectionCount();	// ���� ���� ����
		LONG GetTotalConnectionCount();		// �� ���� ��
		LONG GetCps();						// ���� �ʴ� �����
		LONG GetAverageCps();				// ��� �ʴ� �����
	
	protected:
		BOOL m_bUseStaticsPerSec;
		LONG m_nStartTime;
		LONG m_nStartTimeLast;
				
		// ��� ����
		LONG m_nCompletionThread;			// completion ������ ��

		LONG m_nRecvBytesTotal;				// ��ü ���� bytes
		LONG m_nRecvBytesLast;
		LONG m_nRecvBps;
		LONG m_nRecvAverageBps;
		
		LONG m_nSentBytesTotal;				// ��ü �۽� bytes
		LONG m_nSentBytesLast;
		LONG m_nSentBps;
		LONG m_nSentAverageBps;
		
		LONG m_nConnectionCount;			// ���� TCP ���� ����
		LONG m_nConnectionCountTotal;		// ��ü TCP ���� ����
		LONG m_nConnectionCountLast;
		LONG m_nCps;
		LONG m_nAverageCps;
		
		LONG m_nPendingAcceptCount;		// TCP ���� ����� ����
		LONG m_nOutstandingRecvs;		// ���� ����� ����
		LONG m_nOutstandingSends;		// �۽� ����� ����
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
// ��� ���� ���� ����
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
// ���� bytes
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
// �۽� bytes
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
// ��/���� �Ϸ� ��� ����
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
// TCP��, Accept ������� Ŭ���̾�Ʈ ���� ����
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
// TCP��, ���� ����
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