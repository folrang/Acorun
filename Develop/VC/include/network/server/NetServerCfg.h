#pragma once

#include <network/IpAddrResolver.h>
#include <network/server/NetCtx.h>
#include <list>
using namespace std;
using namespace NetServer;

namespace NetServer
{
	#define DEFAULT_BACKLOG					5 //SOMAXCONN		// 기본 backlog 값 (5), Windows7 에서는 0xffff ?

	#define DEFAULT_MIN_COMPLETION_THREAD	0				// Completion 스레드의 기본 최소 개수 (0: CPU 개수만큼 생성)
	#define DEFAULT_MAX_COMPLETION_THREAD	32				// Completion 스레드의 기본 최대 개수

	#define DEFAULT_NETCLIENTCTX_FREELIST_SIZE	0			// 0 - FreeList 사용 않함
	#define DEFAULT_NETIODATA_FREELIST_SIZE		0			// 0 - FreeList 사용 않함


	//////////////////////////////////////////////////////////
	//			TCP 설정
	//

	#define DEFAULT_MIN_PENDING_ACCEPTS		2				// 리슨소켓별 Accept 대기하는 기본 최소 개수
	#define DEFAULT_MAX_PENDING_ACCEPTS		10	/*~200*/	// 리슨소켓별 Accept 대기하는 기본 최대 개수

	#define DEFAULT_FORCECONN_USE			1	/*0,1*/		// ForceConn 루틴 사용 여부
	#define DEFAULT_FORCECONN_INTERVAL_TIMEOUT				120		// ForceConn 루틴 실행 간격(초)
	#define DEFAULT_FORCECONN_PENDING_ACCEPT_TIMEOUT		120		// 연결이 없거나, 
																	// 연결후 최초 작업이 없는 소켓들의 강제 종료 TimeOut
	#define DEFAULT_FORCECONN_NONE_LAST_OP_CONN_TIMEOUT		500		// 마지막 작업후 작동하지 않는 클라이이언트들의 강제 종료 TimeOut

	#define DEFAULT_WSA_BUF_SIZE	1 * 4096				// AceeptEx, Send, Recv 작업시 WSA 버퍼의 기본 크기 (4KByte 배수)

	typedef struct _BIND_INFO
	{
		addrinfo ai;
		int backlog;
	} BIND_INFO;
	typedef list<BIND_INFO>					LST_BIND_INFO;
	typedef list<BIND_INFO>::const_iterator	LST_BIND_INFO_CIT;
	typedef list<BIND_INFO>::iterator		LST_BIND_INFO_IT;


	//////////////////////////////////////////////////////////
	//			UDP 설정
	//
	#define DEFAULT_MIN_PENDING_RECVS		20				// 리슨소켓별 Recv 대기하는 기본 최소 개수
	#define DEFAULT_MAX_PENDING_RECVS		100	/*~200*/	// 리슨소켓별 Recv 대기하는 기본 최대 개수

	//////////////////////////////////////////////////////////
	//				CNetServerCfg
	//
	//
	class CNetServerCfg
	{
	public:
		CNetServerCfg(
			HANDLE hServerExitEvt,				// 서버 종료 이벤트
			CNetListenCtxMan* pListenCtxMan,	// 리슨 context 매니져
			CNetClientCtxMan* pClientCtxMan,	// 클라이언트 context 매니져
			CNetIoDataMan* pIoDataMan			// Overlapped I/O 데이터 매니져
			);
		virtual ~CNetServerCfg();

		HANDLE GetExitEvt() const;
		CNetListenCtxMan* GetListenCtxMan();
		CNetClientCtxMan* GetClientCtxMan();
		CNetIoDataMan* GetIoDataMan();
		
		// SYSTEM_INFO 구조체 반환
		const SYSTEM_INFO& GetSystemInfo() const { return m_SysInfo; }

		// 리슨 소켓의 바인드 정보 추가
		//	- IpAddrResolver.h 파일의 GetFirstAddrInfo() 함수 참조
		// [in] addrInfo: address 정보
		// [in] nBacklog: 연결 대기 큐의 개수 지정 (listen()의 backlog 파라미터 설명 참조)
		//				권장은 SOMAXCONN(SP 정의한 상수값, 5) 이하
		//				Bluetooth 와 같은 경우는 낮게, 2~4 정도
		//				최대는 200 (WindowsNT기반), 빈번한 접속예상일 경우에는 최대 200 이하 값 정도로 설정
		void AddBindInfo(const addrinfo& ai, int nBacklog = DEFAULT_BACKLOG);
		// [in] szIp: 서버 IPV4 혹은 IPV6
		// [in] nPort: 포트 번호
		// [in] bTcp: TCP/UDP 여부
		// [in] nBacklog
		//void AddBindInfo(const char* szIp, int nPort, BOOL bTcp, int nBacklog = DEFAULT_BACKLOG);
		const LST_BIND_INFO& GetBindInfo() const;

		// Completion 스레드의 최소/최대 개수 지정
		// [in] nMinThread: 최소 스레드 개수 (0: CPU 개수만큼)
		// [in] nMaxThread: 최대 스레드 개수 (MAX_COMPLETION_THREAD 한계)
		void SetCompletionThread(int nMinThread = DEFAULT_MIN_COMPLETION_THREAD, int nMaxThread = DEFAULT_MAX_COMPLETION_THREAD);
		int GetMinCompletionThread() const;
		int GetMaxCompletionThread() const;
		
		// NetCtx 설정 정보
		// [in] nNetClientCtxFreeListSize: CNetClientCtxMan의 FreeList 크기 (0 - 사용하지 않음)
		// [in] nNetIoDataFreeListSize: CNetIoDataMan의 FreeList 크기 (0 - 사용하지 않음)
		void SetNetCtxInfo(
			int nNetClientCtxFreeListSize = DEFAULT_NETCLIENTCTX_FREELIST_SIZE, 
			int nNetIoDataFreeListSize = DEFAULT_NETIODATA_FREELIST_SIZE
			);
		void GetNetCtxInfo(int* nNetClientCtxFreeListSize, int* nNetIoDataFreeListSize);

	protected:
		virtual void Reset();

	protected:
		SYSTEM_INFO m_SysInfo;
		HANDLE m_hExitEvt;
		CNetListenCtxMan* m_pListenCtxMan;
		CNetClientCtxMan* m_pClientCtxMan;
		CNetIoDataMan* m_pIoDataMan;

		LST_BIND_INFO m_BindList;

		int m_nMinCompletionThread;
		int m_nMaxCompletionThread;

		int m_nNetClientCtxFreeListSize;
		int m_nNetIoDataFreeListSize;
	};



	//////////////////////////////////////////////////////////
	//				CTcpServerCfg
	//
	//
	class CTcpServerCfg : public CNetServerCfg
	{
	public:
		CTcpServerCfg(HANDLE hServerExitEvt, CTcpListenCtxMan* pListenCtxMan, CTcpClientCtxMan* pClientCtxMan, CNetIoDataMan* pIoDataMan);
		virtual ~CTcpServerCfg();

		// 리슨소켓별, Aceept 대기 정보
		// [in] nMaxAccepts: TcpServer 내에서 Accept 가능한 최대 개수
		void SetPendingAccepts(int nMinAccepts = DEFAULT_MIN_PENDING_ACCEPTS, int nMaxAccepts = DEFAULT_MAX_PENDING_ACCEPTS);
		int GetMinPendingAccepts() const;
		int GetMaxPendingAccepts() const;

		// 클라이언트 강제 종료 정보
		// [in] bUse: 루틴 사용 여부
		// [in] nIntervalTimeOut: 루틴 적용 간격(초)
		// [in] nPendingAcceptTimeOut: 연결이 없거나, 연결후 최초 작업이 없는 소켓들의 강제종료 시간(초)
		//							AcceptEx는 연결과 수신이 동시에 이루어 지는데, 연결만 맺고 수신이 없는 클라이언트들 강제 종료 (DOS 방지)
		// [in] nNoneLastOpConnTimeOut: 마지막 작업(Recv/Send) 이후 작동하지 않는 클라이언트들의 강제 종료(초)
		void SetForceCloseInfo(BOOL bUse = DEFAULT_FORCECONN_USE, 
								int nIntervalTimeOut = DEFAULT_FORCECONN_INTERVAL_TIMEOUT, 
								int nPendingAcceptTimeOut = DEFAULT_FORCECONN_PENDING_ACCEPT_TIMEOUT,
								int nNoneLastOpConnTimeOut = DEFAULT_FORCECONN_NONE_LAST_OP_CONN_TIMEOUT);
		void GetForceCloseInfo(BOOL* bUse, int* nIntervalTimeOut, int* nPendingAcceptTimeOut, int* nNoneLastOpConnTimeOut) const;

		// AcceptEx 버퍼 길이, 최초 접속후 받을 데이터의 버퍼 크기
		// 서버전략이 대용량 데이터 처리(hign throughput)용 보다는 대량 연결(maximizing connections)용 서버 전략이면,
		//	nAcceptBufSize 을 0 으로 설정하여 각 연결별로 locked page를 최소화 하는 방법도 있음.
		//	그러나 특별한 경우가 아니면, Page 크기(4KByte) 의 배수로 지정할길 권장함
		void SetAcceptExBufSize(int nAcceptBufSize = DEFAULT_WSA_BUF_SIZE);
		int GetAcceptExBufSize() const;

	protected:
		virtual void Reset();

	protected:
		int m_nMinPendingAccepts;
		int m_nMaxPendingAccepts;

		BOOL m_bUseForceCloseConn;
		int m_nForceCloseIntervalTimeOut;
		int m_nForceClosePendingAcceptTimeOut;
		int m_nForceCloseNoneLastOpConnTimeOut;

		int m_nAcceptBufSize;
	};



	//////////////////////////////////////////////////////////
	//				CUdpServerCfg
	//
	//
	class CUdpServerCfg : public CNetServerCfg
	{
	public:
		CUdpServerCfg(HANDLE hServerExitEvt, CUdpListenCtxMan* pListenCtxMan, CUdpClientCtxMan* pClientCtxMan, CNetIoDataMan* pIoDataMan) 
			: CNetServerCfg(hServerExitEvt, pListenCtxMan, pClientCtxMan, pIoDataMan) {}
		virtual ~CUdpServerCfg() {}

		// 리슨소켓별, Recv 대기 개수
		void SetPendingRecvs(int nMinPendingRecvs = DEFAULT_MIN_PENDING_RECVS, int nMaxPendingRecvs = DEFAULT_MAX_PENDING_RECVS);
		int GetMinPendingRecvs() const;
		int GetMaxPendingRecvs() const;

	protected:
		virtual void Reset();

	protected:
		int m_nMinPendingRecvs;
		int m_nMaxPendingRecvs;
	};
	
	

};


//////////////////////////////////////////////////////////
inline HANDLE CNetServerCfg::GetExitEvt() const
{
	return m_hExitEvt;
}

inline CNetListenCtxMan* CNetServerCfg::GetListenCtxMan()
{
	return m_pListenCtxMan;
}

inline CNetClientCtxMan* CNetServerCfg::GetClientCtxMan()
{
	return m_pClientCtxMan;
}

inline CNetIoDataMan* CNetServerCfg::GetIoDataMan()
{
	return m_pIoDataMan;
}


inline const LST_BIND_INFO& CNetServerCfg::GetBindInfo() const
{
	return m_BindList;
}

inline void CNetServerCfg::SetCompletionThread(int nMinThread, int nMaxThread)
{
	m_nMinCompletionThread = (nMinThread > nMaxThread ? nMaxThread : nMinThread);
	m_nMaxCompletionThread = nMaxThread;
}

inline int CNetServerCfg::GetMinCompletionThread() const
{
	return (m_nMinCompletionThread == 0 ? m_SysInfo.dwNumberOfProcessors : m_nMinCompletionThread);
}

inline int CNetServerCfg::GetMaxCompletionThread() const
{
	return m_nMaxCompletionThread;
}


inline void CNetServerCfg::SetNetCtxInfo(int nNetClientCtxFreeListSize, int nNetIoDataFreeListSize)
{
	m_nNetClientCtxFreeListSize = nNetClientCtxFreeListSize;
	m_nNetIoDataFreeListSize = nNetIoDataFreeListSize;
}

inline void CNetServerCfg::GetNetCtxInfo(int* nNetClientCtxFreeListSize, int* nNetIoDataFreeListSize)
{
	*nNetClientCtxFreeListSize = m_nNetClientCtxFreeListSize;
	*nNetIoDataFreeListSize = m_nNetIoDataFreeListSize;
}


//////////////////////////////////////////////////////////

// 리슨소켓별, Aceept 대기 정보
inline void CTcpServerCfg::SetPendingAccepts(int nMinAccepts, int nMaxAccepts)
{
	m_nMinPendingAccepts = nMinAccepts;
	m_nMaxPendingAccepts = nMaxAccepts;
}

inline int CTcpServerCfg::GetMinPendingAccepts() const
{
	return (m_nMinPendingAccepts > m_nMaxPendingAccepts ? m_nMaxPendingAccepts : m_nMinPendingAccepts);
}

inline int CTcpServerCfg::GetMaxPendingAccepts() const
{
	return (m_nMaxPendingAccepts < m_nMinPendingAccepts ? m_nMinPendingAccepts : m_nMaxPendingAccepts);
}

inline void CTcpServerCfg::SetForceCloseInfo(BOOL bUse, int nIntervalTimeOut, int nPendingAcceptTimeOut, int nNoneLastOpConnTimeOut)
{
	m_bUseForceCloseConn = bUse;
	m_nForceCloseIntervalTimeOut = nIntervalTimeOut;
	m_nForceClosePendingAcceptTimeOut = nPendingAcceptTimeOut;
	m_nForceCloseNoneLastOpConnTimeOut = nNoneLastOpConnTimeOut;
}

inline void CTcpServerCfg::GetForceCloseInfo(BOOL* bUse, int* nIntervalTimeOut, int* nPendingAcceptTimeOut, int* nNoneLastOpConnTimeOut) const
{
	*bUse = m_bUseForceCloseConn;
	*nIntervalTimeOut = m_nForceCloseIntervalTimeOut;
	*nPendingAcceptTimeOut = m_nForceClosePendingAcceptTimeOut;
	*nNoneLastOpConnTimeOut = m_nForceCloseNoneLastOpConnTimeOut;
}

// AcceptEx 버퍼 길이, 최초 접속후 받을 데이터의 버퍼 크기
inline void CTcpServerCfg::SetAcceptExBufSize(int nAcceptBufSize)
{
	m_nAcceptBufSize = nAcceptBufSize;
}

inline int CTcpServerCfg::GetAcceptExBufSize() const
{
	return m_nAcceptBufSize;
}


// 리슨소켓별, Recv 대기 개수
inline void CUdpServerCfg::SetPendingRecvs(int nMinPendingRecvs, int nMaxPendingRecvs)
{
	m_nMinPendingRecvs = nMinPendingRecvs;
	m_nMaxPendingRecvs = nMaxPendingRecvs;
}

inline int CUdpServerCfg::GetMinPendingRecvs() const
{
	return m_nMinPendingRecvs;
}

inline int CUdpServerCfg::GetMaxPendingRecvs() const
{
	return m_nMaxPendingRecvs;
}