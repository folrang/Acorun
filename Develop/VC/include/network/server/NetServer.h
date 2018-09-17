//						NetServer.h
//			Winsock Network Server(TCP/UDP) Ŭ����
//
//		2010/06/xx
//		�ۼ���: dede
//		�ۼ���(dede)�� ���Ǿ��� �ش� �ҽ��� ���, �����ϴ� ���� �ҹ��Դϴ�.
//		
//		���� ������: 2010/xx/xx
//
//		[�����丮]
//		2010/06/xx
//			1. ���� ����
//
//		[�˸�����]
//		- ws2_32.lib ����
//		- ���� �����, ����� ���� ���� �� context ���� ���� ó����
//			���������̺�Ʈ���� -> completion ������� ���� -> listen context ���� -> client context ���� -> overlapped ����ü �� ���� ���� ��
//
//		[���ǻ���]
//		- WindowNT �̻󿡼��� �۵�, Window2000�̻� ����
//
//		[ToDoList]
//		
//
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <network/server/NetServerCfg.h>
#include <network/server/NetServerstat.h>
#include <network/NetTrace.h>
#include <process.h>
#include <string>

using namespace std;
using namespace Network;
using namespace NetServer;

// NetServer �ݹ� �Լ� Ÿ�� ����
//#define NSAPI

namespace NetServer
{
	//////////////////////////////////////////////////////////
	//				CNetServer
	//
	//  - ���� ������ CNetServerCfg::hServerExitEvt �̺�Ʈ�� �ñ׳η� ����
	//
	class CNetServer
	{
	public:
		typedef enum _PROTOCOL
		{
			TCP,
			UDP
		} PROTOCOL;

		typedef enum _RET_CODE
		{
			// ����
			E_SUCCESS,
			
			/////////////////////////////////////////
			//		�ʱ�ȭ ��� �ڵ�

			// WSA �ʱ�ȭ, ���� Ȯ���Լ� �ε� ����, WSAEventSelect ����, IOCP �ʱ�ȭ ����
			E_FAIL_INIT_API,
			
			// ���ε� Ȥ�� ���� ����
			E_CREATE_LISTEN,

			// NSAPI �Լ� ������� FALSE ���
			//E_NSAPI_FALSE,


			/////////////////////////////////////////
			//		�۾� ��� �ڵ�
			
			// ���� �Ҵ� ����
			E_INVALID_SOCKET,

			// CNetClientCtxMan::AllocCtx() ����
			E_FAIL_ALLOC_CTX,
			
			// CNetIoDataMan::AllocIoData() ����
			E_FAIL_ALLOC_IODATA,
			
			// PostAcceptOp �۾� ����
			E_FAIL_POST_ACCEPT,
			
			// PostRecvOp �۾� ����
			E_FAIL_POST_RECV,
			
			// PostSendOp �۾� ����
			E_FAIL_POST_SEND,
			
			// CompXXXOp ������ ó���� ���� �б� ����
			// ���� Operation ó���� �����ϰ�, ���� operation�� ó����� ��
			E_PROCESS_NEXT_OP,

			// WSAENOBUFS ����(locked page ����), �߻����ɼ� ����.
			// IO �۾��� ���̰ų�, ���ʿ��� ������ �ݾ�� �� ��Ȳ
			// operation ��õ� �ʿ�, �߻��� ���� ���ᵵ ����
			//E_WSAENOBUFS,

			// �˼�����, ������
			E_UNKNOWN
		} RET_CODE;

		CNetServer(CNetServerCfg* pCfg, CNetServerStat* pStat, CNetTrace* pTrace);
		virtual ~CNetServer(void);
		
		// ���� �ʱ�ȭ �� ���� ����
		RET_CODE Initialize();

		// ���� ����
		void Uninitialize();
		
	protected:
		// Control ������
		static unsigned __stdcall ControlThread(LPVOID lpvThreadParam);

		// Completion ������
		static unsigned __stdcall CompletionThread(LPVOID lpvThreadParam);
		
		// Control �������� ���� �Լ�
		virtual unsigned ControlThreadMain(CNetServer* pNetServer) = 0;

		// MS ���� Ȯ�� �Լ��� �ε�
		// [in] pCtx: ������ ������ Context
		// [retVal] FALSE: �ε� ����
		virtual BOOL LoadWinSockExFunc(CNetListenCtx* pListenCtx) = 0;


		//////////////////////////////////////////////////////////
		//			��Ʈ�� I/O �۾���
		//////////////////////////////////////////////////////////
		
		// ���� ����
		// [in] pCtx: ������ ������ Context
		virtual RET_CODE StartListen(CNetListenCtx* pListenCtx) { return CNetServer::E_SUCCESS; }

		// Ŭ���̾�Ʈ ������ ������ ����
		// [retVal] TRUE - ���� ���
		//			FALSE - ���� ����
		virtual BOOL IsAcceptClient(CNetServerCfg* pCfg, const string& clientIP) { return TRUE; }
		
		// Completion thread���� ������ Recv �۾��� ó�� ����
		virtual RET_CODE CompletionSuccessRecv(CNetServer* pThis, CNetCtx* pCtx, CNetIoData* pIoData, DWORD dwBytesTransferred, int nWsaLastError);

		
		// �ش� ���Ͽ� overlapped receive operation ����
		// [in] pCtx: �۾� ���� context
		// [in] pIoData: IO �۾��� ������
		// [in] dwOpBufSize: IO �۾��� �ʿ��� ���� ũ��
		// [retVal] RET_CODE
		//		E_SUCCESS: ����
		//		��Ÿ: ����, ��û ���õ�
		virtual RET_CODE PostRecvOp(CNetServer* pThis, CNetCtx* pCtx, CNetIoData* pIoData, DWORD dwOpBufSize) = 0;
		virtual RET_CODE PostSendOp(CNetServer* pThis, CNetCtx* pCtx, CNetIoData* pIoData) = 0;
		
		// ��û�� operation �۾� �Ϸ��� ����� �Լ� ȣ��
		// [in] nWsaLastError: completion �����忡���� ó�� ��� �ڵ�
		// [retVal] E_REQUEST_CLOSECONN - ���� ���� ��û
		virtual RET_CODE CompRecvOp(CNetServer* pThis, CNetCtx* pCtx, CNetIoData* pIoData, int nWsaLastError) = 0;
		virtual RET_CODE CompSendOp(CNetServer* pThis, CNetCtx* pCtx, CNetIoData* pIoData, int nWsaLastError) = 0;

		// �������ο� ������� Overlapped IO �۾� ���� �� Ŀ�ؼ� ���Ḧ ��û
		// bRequestCloseConn ��û ���Ŀ��� pIoData ��ü ������
		// [in] pIoData: �Ϸ�� IO data
		// [in] bRequestCloseConn: TCP Ŀ�ؼ� ���� ��û ����
		virtual void CloseOp(CNetServer* pThis, CNetIoData* pIoData, BOOL bRequestCloseConn = FALSE) = 0;
				
	public:
		PROTOCOL m_Protocol;
		CNetServerCfg* m_pCfg;
		CNetServerStat* m_pStat;
		CNetTrace* m_pTrace;
		
		HANDLE m_hPort;			// IOCP �ڵ�
		HANDLE m_hControlThread;
	};



	//////////////////////////////////////////////////////////
	//				CTcpServer
	//
	//	- ����
	//  - 
	//
	class CTcpServer : public CNetServer
	{
	public:
		CTcpServer(CTcpServerCfg* pCfg, CTcpServerStat* pStat, CNetTrace* pTrace);
		virtual ~CTcpServer();

	protected:
		// Control �������� ���� �Լ�
		unsigned ControlThreadMain(CNetServer* pNetServer);

		// TCP�� MS ���� Ȯ�� �Լ��� �ε�
		BOOL LoadWinSockExFunc(CNetListenCtx* pListenCtx);

		//////////////////////////////////////////////////////////
		//			��Ʈ�� I/O �۾���
		//////////////////////////////////////////////////////////

		// ���Ϲ��ε� �� ���� ���Ŀ� ȣ��
		// [in] pCtx: ������ ������ Context
		RET_CODE StartListen(CNetListenCtx* pListenCtx);

		// TcpClientCtx �� NetIoData �� ������ ��, Accept ��� ����
		// [in] pTcpListenCtx: ���� ��ü
		// [retVal] E_SUCCESS - ����
		//			��Ÿ	  - ����, Accept ��û ���õ�
		RET_CODE PostAcceptOp(CTcpServer* pTcpThis, CTcpListenCtx* pTcpListenCtx);
		
		// �ش� ���Ͽ� overlapped receive operation ����
		// [in] pCtx: �۾� ���� context
		// [in] pIoData: IO �۾��� ������
		// [in] dwOpBufSize: IO �۾��� �ʿ��� ���� ũ��
		// [retVal] RET_CODE
		//		E_SUCCESS: ����
		//		��Ÿ: ����, ��û ���õ�
		RET_CODE PostRecvOp(CNetServer* pThis, CNetCtx* pCtx, CNetIoData* pIoData, DWORD dwOpBufSize);
		RET_CODE PostSendOp(CNetServer* pThis, CNetCtx* pCtx, CNetIoData* pIoData);

		// ��û�� operation �۾� �Ϸ��� ����� �Լ� ȣ��
		// [retVal] E_REQUEST_CLOSECONN - ���� ���� ��û
		// [in] nWsaLastError: completion �����忡���� �Ϸ� ��� �ڵ� ����
		virtual RET_CODE CompRecvOp(CNetServer* pThis, CNetCtx* pCtx, CNetIoData* pIoData, int nWsaLastError);
		virtual RET_CODE CompSendOp(CNetServer* pThis, CNetCtx* pCtx, CNetIoData* pIoData, int nWsaLastError);

		// �������ο� ������� Overlapped IO �۾� ���� �� Ŀ�ؼ� ���Ḧ ��û
		// bRequestCloseConn ��û ���Ŀ��� pIoData ��ü ������
		// [in] pIoData: �Ϸ�� IO data
		// [in] bRequestCloseConn: Ŀ�ؼ� ���� ��û ����
		void CloseOp(CNetServer* pThis, CNetIoData* pIoData, BOOL bRequestCloseConn = FALSE);

		// ���� ���� ���Ḧ ��û
		// Ŭ���̾�Ʈ�� ���� IO �۾��� ������� ������ ���� ���� ��û
		void ForceCloseConn(CTcpClientCtx* pTcpCtx);
		
		// pNetBuf�� �� �κа� pFindBuf �� nFindBufSize ��ŭ memcmp ���Ͽ� ���������� ��ȯ
		virtual BOOL IsSameEndBuf(CNetBuf* pNetBuf, const BYTE* pFindBuf, DWORD dwFindBufSize);

	protected:
		// ���� ��ü����, ������ ���ų� Ȥ�� ������ ���� �۾��� ���� �е����� AceeptEx ���ϵ��� nMinPendingAccepts �������� ���� ����
		// [in] nMinPendingAccepts: Accept ��� �ּ� ����
		// [in] nPendingAcceptTimeOut: ������ ���ų�, ������ ���� �۾��� ���� ���ϵ��� �������� �ð�(��)
		//							AcceptEx�� ����� ������ ���ÿ� �̷�� ���µ�, ���Ḹ �ΰ� ������ ���� Ŭ���̾�Ʈ�� ���� ���� (DOS ����)
		// [retVal] int: ����� ���� ����
		int ForceClosePendingAccept(CTcpServer* pTcpThis, int nMinPendingAccepts, int nPendingAcceptTimeOut);

		// ���� ��ü����, ������ �۾��� �۵����� �ʴ� Ŭ���̾�Ʈ���� ���� ����
		// [in] pTcpClientCtxMan: Ŭ���̾�Ʈ ���� ������ context ������ ���� ClientCtxMan
		// [in] pIoDataMan: IO data ������ ���� CNetIoDataMan
		// [in] nNoneLastOpConnTimeOut: ������ �۾�(Recv/Send) ���� �۵����� �ʴ� Ŭ���̾�Ʈ���� ���� ����(��)
		// [retVal] int: ����� ���� ����
		int ForceCloseNoneLastOpConn(CTcpClientCtxMan* pTcpClientCtxMan, CNetIoDataMan* pIoDataMan, int nNoneLastOpConnTimeOut);

		// FD_ACCEPT ��Ȳ�̸�, �ִ��� ���� Aceept �� ��� ��Ű���� ��
		// [in] pTcpListenCtx: ���� ��ü
		// [in] nMaxPendingAccepts: Accept ������ �ִ� ����
		// [retVal] Accept ��� ��Ų ����
		int BurstPendingAccepts(CTcpServer* pTcpThis, CTcpListenCtx* pTcpListenCtx, int nMaxPendingAccepts);
	};



	//////////////////////////////////////////////////////////
	//				CUdpServer
	//
	//	- ����
	//  - 
	//
	class CUdpServer : public CNetServer
	{
	public:
		CUdpServer(CUdpServerCfg* pCfg, CUdpServerStat* pStat, CNetTrace* pTrace);
		virtual ~CUdpServer();

	protected:
		// Control �������� ���� �Լ�
		unsigned ControlThreadMain(CNetServer* pNetServer);

		// MS ���� Ȯ�� �Լ��� �ε�
		// [in] pCtx: ������ ������ Context
		// [retVal] FALSE: �ε� ����
		BOOL LoadWinSockExFunc(CNetListenCtx* pListenCtx);
		
		// ���Ϲ��ε� �� ���� ���Ŀ� ȣ��
		// [in] pListenCtx: ������ ������ Context
		RET_CODE StartListen(CNetListenCtx* pListenCtx);

		// �������ο� ������� Overlapped IO �۾� ���� �� Ŀ�ؼ� ���Ḧ ��û
		// bRequestCloseConn ��û ���Ŀ��� pIoData ��ü ������
		// [in] pIoData: �Ϸ�� IO data
		// [in] bRequestCloseConn: TCP Ŀ�ؼ� ���� ��û ����
		void CloseOp(CNetServer* pThis, CNetIoData* pIoData, BOOL bRequestCloseConn);
		

		// �ش� ���Ͽ� overlapped receive operation ����
		// [in] pCtx: �۾� ���� context
		// [in] pIoData: IO �۾��� ������
		// [in] dwOpBufSize: IO �۾��� �ʿ��� ���� ũ��
		// [retVal] RET_CODE
		//		E_SUCCESS: ����
		//		��Ÿ: ����, ��û ���õ�
		RET_CODE PostRecvOp(CNetServer* pThis, CNetCtx* pCtx, CNetIoData* pIoData, DWORD dwOpBufSize);
		RET_CODE PostSendOp(CNetServer* pThis, CNetCtx* pCtx, CNetIoData* pIoData);
	};




};