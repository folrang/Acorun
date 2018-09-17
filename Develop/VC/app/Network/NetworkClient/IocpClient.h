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
			// 성공
			E_SUCCESS,
			
			/////////////////////////////////////////
			//		초기화 결과 코드

			// WSA 초기화, 윈속 확장함수 로딩 실패, WSAEventSelect 실패, IOCP 초기화 실패
			E_FAIL_INIT_API,
			
			// 바인딩 혹은 리슨 실패
			E_CREATE_LISTEN,

			// NSAPI 함수 결과값이 FALSE 경우
			//E_NSAPI_FALSE,


			/////////////////////////////////////////
			//		작업 결과 코드
			
			// 소켓 할당 실패
			E_INVALID_SOCKET,

			// CNetClientCtxMan::AllocCtx() 실패
			E_FAIL_ALLOC_CTX,
			
			// CNetIoDataMan::AllocIoData() 실패
			E_FAIL_ALLOC_IODATA,
			
			// PostAcceptOp 작업 실패
			E_FAIL_POST_ACCEPT,
			
			// PostRecvOp 작업 실패
			E_FAIL_POST_RECV,
			
			// PostSendOp 작업 실패
			E_FAIL_POST_SEND,
			
			// CompXXXOp 오류시 처리에 관한 분기 내용
			// 현재 Operation 처리를 종료하고, 다음 operation을 처리토록 함
			E_PROCESS_NEXT_OP,

			// WSAENOBUFS 오류(locked page 오류), 발생가능성 낮음.
			// IO 작업을 줄이거나, 불필요한 연결을 닫어야 할 상황
			// operation 재시도 필요, 발생시 서버 종료도 가능
			//E_WSAENOBUFS,

			// 알수없음, 값없음
			E_UNKNOWN
		} RET_CODE;

		CIocpClient(CIocpClientCfg* pCfg, CNetClientStat* pStat, CNetTrace* pTrace);
		virtual ~CIocpClient(void);

		// 서버 초기화 및 서버 실행
		RET_CODE Initialize();

		// 서버 해제
		void Uninitialize();


		BOOL AddConn(CIocpConnection* pConn);
		void RemoveConn(CIocpConnection* pConn);

	protected;

	protected:
		CIocpClientCfg* m_pCfg;
		CNetClientStat* m_pStat;
		CNetTrace* m_pTrace;

		HANDLE m_hPort;			// IOCP 핸들
		HANDLE m_hControlThread;

		// 작업중인 CIocpConn 객체들의 set
		unordered_set<CIocpConnection*> m_ConnSet;

	};


	//////////////////////////////////////////////////////////
	//				CNetServer
	//
	//  - 서버 중지는 CNetServerCfg::hServerExitEvt 이벤트의 시그널로 중지
	//
	class CIocpConnection
	{
	public:
		CIocpConnection();
		virtual ~CIocpConnection();

		// Remote host 로 연결
		// [in] szIp: 서버 IPV4 혹은 IPV6
		// [in] nPort: 포트 번호
		// [in] dwTimeOut: 연결 대기 시간
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

