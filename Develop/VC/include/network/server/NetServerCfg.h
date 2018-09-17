#pragma once

#include <network/IpAddrResolver.h>
#include <network/server/NetCtx.h>
#include <list>
using namespace std;
using namespace NetServer;

namespace NetServer
{
	#define DEFAULT_BACKLOG					5 //SOMAXCONN		// �⺻ backlog �� (5), Windows7 ������ 0xffff ?

	#define DEFAULT_MIN_COMPLETION_THREAD	0				// Completion �������� �⺻ �ּ� ���� (0: CPU ������ŭ ����)
	#define DEFAULT_MAX_COMPLETION_THREAD	32				// Completion �������� �⺻ �ִ� ����

	#define DEFAULT_NETCLIENTCTX_FREELIST_SIZE	0			// 0 - FreeList ��� ����
	#define DEFAULT_NETIODATA_FREELIST_SIZE		0			// 0 - FreeList ��� ����


	//////////////////////////////////////////////////////////
	//			TCP ����
	//

	#define DEFAULT_MIN_PENDING_ACCEPTS		2				// �������Ϻ� Accept ����ϴ� �⺻ �ּ� ����
	#define DEFAULT_MAX_PENDING_ACCEPTS		10	/*~200*/	// �������Ϻ� Accept ����ϴ� �⺻ �ִ� ����

	#define DEFAULT_FORCECONN_USE			1	/*0,1*/		// ForceConn ��ƾ ��� ����
	#define DEFAULT_FORCECONN_INTERVAL_TIMEOUT				120		// ForceConn ��ƾ ���� ����(��)
	#define DEFAULT_FORCECONN_PENDING_ACCEPT_TIMEOUT		120		// ������ ���ų�, 
																	// ������ ���� �۾��� ���� ���ϵ��� ���� ���� TimeOut
	#define DEFAULT_FORCECONN_NONE_LAST_OP_CONN_TIMEOUT		500		// ������ �۾��� �۵����� �ʴ� Ŭ�����̾�Ʈ���� ���� ���� TimeOut

	#define DEFAULT_WSA_BUF_SIZE	1 * 4096				// AceeptEx, Send, Recv �۾��� WSA ������ �⺻ ũ�� (4KByte ���)

	typedef struct _BIND_INFO
	{
		addrinfo ai;
		int backlog;
	} BIND_INFO;
	typedef list<BIND_INFO>					LST_BIND_INFO;
	typedef list<BIND_INFO>::const_iterator	LST_BIND_INFO_CIT;
	typedef list<BIND_INFO>::iterator		LST_BIND_INFO_IT;


	//////////////////////////////////////////////////////////
	//			UDP ����
	//
	#define DEFAULT_MIN_PENDING_RECVS		20				// �������Ϻ� Recv ����ϴ� �⺻ �ּ� ����
	#define DEFAULT_MAX_PENDING_RECVS		100	/*~200*/	// �������Ϻ� Recv ����ϴ� �⺻ �ִ� ����

	//////////////////////////////////////////////////////////
	//				CNetServerCfg
	//
	//
	class CNetServerCfg
	{
	public:
		CNetServerCfg(
			HANDLE hServerExitEvt,				// ���� ���� �̺�Ʈ
			CNetListenCtxMan* pListenCtxMan,	// ���� context �Ŵ���
			CNetClientCtxMan* pClientCtxMan,	// Ŭ���̾�Ʈ context �Ŵ���
			CNetIoDataMan* pIoDataMan			// Overlapped I/O ������ �Ŵ���
			);
		virtual ~CNetServerCfg();

		HANDLE GetExitEvt() const;
		CNetListenCtxMan* GetListenCtxMan();
		CNetClientCtxMan* GetClientCtxMan();
		CNetIoDataMan* GetIoDataMan();
		
		// SYSTEM_INFO ����ü ��ȯ
		const SYSTEM_INFO& GetSystemInfo() const { return m_SysInfo; }

		// ���� ������ ���ε� ���� �߰�
		//	- IpAddrResolver.h ������ GetFirstAddrInfo() �Լ� ����
		// [in] addrInfo: address ����
		// [in] nBacklog: ���� ��� ť�� ���� ���� (listen()�� backlog �Ķ���� ���� ����)
		//				������ SOMAXCONN(SP ������ �����, 5) ����
		//				Bluetooth �� ���� ���� ����, 2~4 ����
		//				�ִ�� 200 (WindowsNT���), ����� ���ӿ����� ��쿡�� �ִ� 200 ���� �� ������ ����
		void AddBindInfo(const addrinfo& ai, int nBacklog = DEFAULT_BACKLOG);
		// [in] szIp: ���� IPV4 Ȥ�� IPV6
		// [in] nPort: ��Ʈ ��ȣ
		// [in] bTcp: TCP/UDP ����
		// [in] nBacklog
		//void AddBindInfo(const char* szIp, int nPort, BOOL bTcp, int nBacklog = DEFAULT_BACKLOG);
		const LST_BIND_INFO& GetBindInfo() const;

		// Completion �������� �ּ�/�ִ� ���� ����
		// [in] nMinThread: �ּ� ������ ���� (0: CPU ������ŭ)
		// [in] nMaxThread: �ִ� ������ ���� (MAX_COMPLETION_THREAD �Ѱ�)
		void SetCompletionThread(int nMinThread = DEFAULT_MIN_COMPLETION_THREAD, int nMaxThread = DEFAULT_MAX_COMPLETION_THREAD);
		int GetMinCompletionThread() const;
		int GetMaxCompletionThread() const;
		
		// NetCtx ���� ����
		// [in] nNetClientCtxFreeListSize: CNetClientCtxMan�� FreeList ũ�� (0 - ������� ����)
		// [in] nNetIoDataFreeListSize: CNetIoDataMan�� FreeList ũ�� (0 - ������� ����)
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

		// �������Ϻ�, Aceept ��� ����
		// [in] nMaxAccepts: TcpServer ������ Accept ������ �ִ� ����
		void SetPendingAccepts(int nMinAccepts = DEFAULT_MIN_PENDING_ACCEPTS, int nMaxAccepts = DEFAULT_MAX_PENDING_ACCEPTS);
		int GetMinPendingAccepts() const;
		int GetMaxPendingAccepts() const;

		// Ŭ���̾�Ʈ ���� ���� ����
		// [in] bUse: ��ƾ ��� ����
		// [in] nIntervalTimeOut: ��ƾ ���� ����(��)
		// [in] nPendingAcceptTimeOut: ������ ���ų�, ������ ���� �۾��� ���� ���ϵ��� �������� �ð�(��)
		//							AcceptEx�� ����� ������ ���ÿ� �̷�� ���µ�, ���Ḹ �ΰ� ������ ���� Ŭ���̾�Ʈ�� ���� ���� (DOS ����)
		// [in] nNoneLastOpConnTimeOut: ������ �۾�(Recv/Send) ���� �۵����� �ʴ� Ŭ���̾�Ʈ���� ���� ����(��)
		void SetForceCloseInfo(BOOL bUse = DEFAULT_FORCECONN_USE, 
								int nIntervalTimeOut = DEFAULT_FORCECONN_INTERVAL_TIMEOUT, 
								int nPendingAcceptTimeOut = DEFAULT_FORCECONN_PENDING_ACCEPT_TIMEOUT,
								int nNoneLastOpConnTimeOut = DEFAULT_FORCECONN_NONE_LAST_OP_CONN_TIMEOUT);
		void GetForceCloseInfo(BOOL* bUse, int* nIntervalTimeOut, int* nPendingAcceptTimeOut, int* nNoneLastOpConnTimeOut) const;

		// AcceptEx ���� ����, ���� ������ ���� �������� ���� ũ��
		// ���������� ��뷮 ������ ó��(hign throughput)�� ���ٴ� �뷮 ����(maximizing connections)�� ���� �����̸�,
		//	nAcceptBufSize �� 0 ���� �����Ͽ� �� ���Ằ�� locked page�� �ּ�ȭ �ϴ� ����� ����.
		//	�׷��� Ư���� ��찡 �ƴϸ�, Page ũ��(4KByte) �� ����� �����ұ� ������
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

		// �������Ϻ�, Recv ��� ����
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

// �������Ϻ�, Aceept ��� ����
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

// AcceptEx ���� ����, ���� ������ ���� �������� ���� ũ��
inline void CTcpServerCfg::SetAcceptExBufSize(int nAcceptBufSize)
{
	m_nAcceptBufSize = nAcceptBufSize;
}

inline int CTcpServerCfg::GetAcceptExBufSize() const
{
	return m_nAcceptBufSize;
}


// �������Ϻ�, Recv ��� ����
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