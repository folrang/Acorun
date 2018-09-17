#pragma once

#include "NetClientStat.h"
#include <network\server\NetCtx.h>
#include <network\NetTrace.h>
#include <unordered_set>
using namespace std::tr1;

using namespace Network;
using namespace NetClient;


namespace NetClient
{
	class CIocpConnection;


	//////////////////////////////////////////////////////////
	//				CIocpClientCfg
	//
	//  - 
	//
	class CIocpClientCfg
	{
	public:
		CIocpClientCfg();
		virtual ~CIocpClientCfg();
	};

	
	//////////////////////////////////////////////////////////
	//				CIocpClient
	//
	//  - 
	//
	class CIocpClient
	{
	public:
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

		CIocpClient(CIocpClientCfg* pCfg, CNetClientStat* pStat, CNetTrace* pTrace);
		virtual ~CIocpClient(void);

		// ���� �ʱ�ȭ �� ���� ����
		RET_CODE Initialize();

		// ���� ����
		void Uninitialize();


		BOOL AddConn(CIocpConnection* pConn);
		void RemoveConn(CIocpConnection* pConn);

	protected;

	protected:
		CIocpClientCfg* m_pCfg;
		CNetClientStat* m_pStat;
		CNetTrace* m_pTrace;

		HANDLE m_hPort;			// IOCP �ڵ�
		HANDLE m_hControlThread;

		// �۾����� CIocpConn ��ü���� set
		unordered_set<CIocpConnection*> m_ConnSet;

	};


	//////////////////////////////////////////////////////////
	//				CNetServer
	//
	//  - ���� ������ CNetServerCfg::hServerExitEvt �̺�Ʈ�� �ñ׳η� ����
	//
	class CIocpConnection
	{
	public:
		CIocpConnection();
		virtual ~CIocpConnection();

		// Remote host �� ����
		// [in] szIp: ���� IPV4 Ȥ�� IPV6
		// [in] nPort: ��Ʈ ��ȣ
		// [in] dwTimeOut: ���� ��� �ð�
		virtual CIocpClient::RET_CODE PostConnect(LPCTSTR szIp, int nPort, DWORD dwTimeOut);
		virtual CIocpClient::RET_CODE PostRecvOp();
		virtual CIocpClient::RET_CODE PostSendOp();
		virtual CIocpClient::RET_CODE PostCloseOp();

		virtual CIocpClient::RET_CODE CompConnect() = 0;
		virtual CIocpClient::RET_CODE CompRecvOp() = 0;
		virtual CIocpClient::RET_CODE CompSendOp() = 0;
		virtual CIocpClient::RET_CODE CompCloseOp() = 0;
	};


};

