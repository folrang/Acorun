//						NetServer.h
//			Winsock Network Server(TCP/UDP) 클래스
//
//		2010/06/xx
//		작성자: dede
//		작성자(dede)의 동의없이 해당 소스를 사용, 변경하는 것은 불법입니다.
//		
//		최종 수정일: 2010/xx/xx
//
//		[히스토리]
//		2010/06/xx
//			1. 최초 개발
//
//		[알림사항]
//		- ws2_32.lib 포함
//		- 서버 종료시, 연결된 소켓 종료 및 context 등이 강제 처리됨
//			서버종료이벤트수신 -> completion 스레드들 종료 -> listen context 삭제 -> client context 삭제 -> overlapped 구조체 및 버퍼 삭제 순
//
//		[주의사항]
//		- WindowNT 이상에서만 작동, Window2000이상 권장
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

// NetServer 콜백 함수 타입 선언
//#define NSAPI

namespace NetServer
{
	//////////////////////////////////////////////////////////
	//				CNetServer
	//
	//  - 서버 중지는 CNetServerCfg::hServerExitEvt 이벤트의 시그널로 중지
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

		CNetServer(CNetServerCfg* pCfg, CNetServerStat* pStat, CNetTrace* pTrace);
		virtual ~CNetServer(void);
		
		// 서버 초기화 및 서버 실행
		RET_CODE Initialize();

		// 서버 해제
		void Uninitialize();
		
	protected:
		// Control 스레드
		static unsigned __stdcall ControlThread(LPVOID lpvThreadParam);

		// Completion 스레드
		static unsigned __stdcall CompletionThread(LPVOID lpvThreadParam);
		
		// Control 스레드의 메인 함수
		virtual unsigned ControlThreadMain(CNetServer* pNetServer) = 0;

		// MS 윈속 확장 함수들 로딩
		// [in] pCtx: 연결할 소켓의 Context
		// [retVal] FALSE: 로딩 실패
		virtual BOOL LoadWinSockExFunc(CNetListenCtx* pListenCtx) = 0;


		//////////////////////////////////////////////////////////
		//			네트웍 I/O 작업들
		//////////////////////////////////////////////////////////
		
		// 리슨 시작
		// [in] pCtx: 리스닝 소켓의 Context
		virtual RET_CODE StartListen(CNetListenCtx* pListenCtx) { return CNetServer::E_SUCCESS; }

		// 클라이언트 접속을 받을지 여부
		// [retVal] TRUE - 연결 허용
		//			FALSE - 연결 거절
		virtual BOOL IsAcceptClient(CNetServerCfg* pCfg, const string& clientIP) { return TRUE; }
		
		// Completion thread에서 성공한 Recv 작업의 처리 내용
		virtual RET_CODE CompletionSuccessRecv(CNetServer* pThis, CNetCtx* pCtx, CNetIoData* pIoData, DWORD dwBytesTransferred, int nWsaLastError);

		
		// 해당 소켓에 overlapped receive operation 실행
		// [in] pCtx: 작업 소켓 context
		// [in] pIoData: IO 작업별 데이터
		// [in] dwOpBufSize: IO 작업에 필요한 버퍼 크기
		// [retVal] RET_CODE
		//		E_SUCCESS: 성공
		//		기타: 실패, 요청 무시됨
		virtual RET_CODE PostRecvOp(CNetServer* pThis, CNetCtx* pCtx, CNetIoData* pIoData, DWORD dwOpBufSize) = 0;
		virtual RET_CODE PostSendOp(CNetServer* pThis, CNetCtx* pCtx, CNetIoData* pIoData) = 0;
		
		// 요청한 operation 작업 완료후 사용자 함수 호출
		// [in] nWsaLastError: completion 스레드에서의 처리 결과 코드
		// [retVal] E_REQUEST_CLOSECONN - 연결 종료 요청
		virtual RET_CODE CompRecvOp(CNetServer* pThis, CNetCtx* pCtx, CNetIoData* pIoData, int nWsaLastError) = 0;
		virtual RET_CODE CompSendOp(CNetServer* pThis, CNetCtx* pCtx, CNetIoData* pIoData, int nWsaLastError) = 0;

		// 성공여부에 상관없이 Overlapped IO 작업 종료 와 커넥션 종료를 요청
		// bRequestCloseConn 요청 이후에만 pIoData 객체 삭제됨
		// [in] pIoData: 완료된 IO data
		// [in] bRequestCloseConn: TCP 커넥션 종료 요청 여부
		virtual void CloseOp(CNetServer* pThis, CNetIoData* pIoData, BOOL bRequestCloseConn = FALSE) = 0;
				
	public:
		PROTOCOL m_Protocol;
		CNetServerCfg* m_pCfg;
		CNetServerStat* m_pStat;
		CNetTrace* m_pTrace;
		
		HANDLE m_hPort;			// IOCP 핸들
		HANDLE m_hControlThread;
	};



	//////////////////////////////////////////////////////////
	//				CTcpServer
	//
	//	- 설명
	//  - 
	//
	class CTcpServer : public CNetServer
	{
	public:
		CTcpServer(CTcpServerCfg* pCfg, CTcpServerStat* pStat, CNetTrace* pTrace);
		virtual ~CTcpServer();

	protected:
		// Control 스레드의 메인 함수
		unsigned ControlThreadMain(CNetServer* pNetServer);

		// TCP용 MS 윈속 확장 함수들 로딩
		BOOL LoadWinSockExFunc(CNetListenCtx* pListenCtx);

		//////////////////////////////////////////////////////////
		//			네트웍 I/O 작업들
		//////////////////////////////////////////////////////////

		// 소켓바인드 및 리슨 이후에 호출
		// [in] pCtx: 리스닝 소켓의 Context
		RET_CODE StartListen(CNetListenCtx* pListenCtx);

		// TcpClientCtx 및 NetIoData 를 생성한 후, Accept 대기 시작
		// [in] pTcpListenCtx: 리슨 객체
		// [retVal] E_SUCCESS - 성공
		//			기타	  - 실패, Accept 요청 무시됨
		RET_CODE PostAcceptOp(CTcpServer* pTcpThis, CTcpListenCtx* pTcpListenCtx);
		
		// 해당 소켓에 overlapped receive operation 실행
		// [in] pCtx: 작업 소켓 context
		// [in] pIoData: IO 작업별 데이터
		// [in] dwOpBufSize: IO 작업에 필요한 버퍼 크기
		// [retVal] RET_CODE
		//		E_SUCCESS: 성공
		//		기타: 실패, 요청 무시됨
		RET_CODE PostRecvOp(CNetServer* pThis, CNetCtx* pCtx, CNetIoData* pIoData, DWORD dwOpBufSize);
		RET_CODE PostSendOp(CNetServer* pThis, CNetCtx* pCtx, CNetIoData* pIoData);

		// 요청한 operation 작업 완료후 사용자 함수 호출
		// [retVal] E_REQUEST_CLOSECONN - 연결 종료 요청
		// [in] nWsaLastError: completion 스레드에서의 완료 결과 코드 전달
		virtual RET_CODE CompRecvOp(CNetServer* pThis, CNetCtx* pCtx, CNetIoData* pIoData, int nWsaLastError);
		virtual RET_CODE CompSendOp(CNetServer* pThis, CNetCtx* pCtx, CNetIoData* pIoData, int nWsaLastError);

		// 성공여부에 상관없이 Overlapped IO 작업 종료 와 커넥션 종료를 요청
		// bRequestCloseConn 요청 이후에만 pIoData 객체 삭제됨
		// [in] pIoData: 완료된 IO data
		// [in] bRequestCloseConn: 커넥션 종료 요청 여부
		void CloseOp(CNetServer* pThis, CNetIoData* pIoData, BOOL bRequestCloseConn = FALSE);

		// 강제 연결 종료를 요청
		// 클라이언트의 현재 IO 작업과 상관없이 강제로 연결 종료 요청
		void ForceCloseConn(CTcpClientCtx* pTcpCtx);
		
		// pNetBuf의 뒷 부분과 pFindBuf 를 nFindBufSize 만큼 memcmp 비교하여 동일한지를 반환
		virtual BOOL IsSameEndBuf(CNetBuf* pNetBuf, const BYTE* pFindBuf, DWORD dwFindBufSize);

	protected:
		// 서버 전체에서, 연결이 없거나 혹은 연결후 최초 작업이 없는 패딩중인 AceeptEx 소켓들을 nMinPendingAccepts 개수까지 강제 종료
		// [in] nMinPendingAccepts: Accept 대기 최소 개수
		// [in] nPendingAcceptTimeOut: 연결이 없거나, 연결후 최초 작업이 없는 소켓들의 강제종료 시간(초)
		//							AcceptEx는 연결과 수신이 동시에 이루어 지는데, 연결만 맺고 수신이 없는 클라이언트들 강제 종료 (DOS 방지)
		// [retVal] int: 종료된 연결 개수
		int ForceClosePendingAccept(CTcpServer* pTcpThis, int nMinPendingAccepts, int nPendingAcceptTimeOut);

		// 서버 전체에서, 마지막 작업후 작동하지 않는 클라이언트들을 강제 종료
		// [in] pTcpClientCtxMan: 클라이언트 연결 종료후 context 삭제를 위한 ClientCtxMan
		// [in] pIoDataMan: IO data 생성을 위한 CNetIoDataMan
		// [in] nNoneLastOpConnTimeOut: 마지막 작업(Recv/Send) 이후 작동하지 않는 클라이언트들의 강제 종료(초)
		// [retVal] int: 종료된 연결 개수
		int ForceCloseNoneLastOpConn(CTcpClientCtxMan* pTcpClientCtxMan, CNetIoDataMan* pIoDataMan, int nNoneLastOpConnTimeOut);

		// FD_ACCEPT 상황이며, 최대한 많은 Aceept 를 대기 시키도록 함
		// [in] pTcpListenCtx: 리슨 객체
		// [in] nMaxPendingAccepts: Accept 가능한 최대 개수
		// [retVal] Accept 대기 시킨 개수
		int BurstPendingAccepts(CTcpServer* pTcpThis, CTcpListenCtx* pTcpListenCtx, int nMaxPendingAccepts);
	};



	//////////////////////////////////////////////////////////
	//				CUdpServer
	//
	//	- 설명
	//  - 
	//
	class CUdpServer : public CNetServer
	{
	public:
		CUdpServer(CUdpServerCfg* pCfg, CUdpServerStat* pStat, CNetTrace* pTrace);
		virtual ~CUdpServer();

	protected:
		// Control 스레드의 메인 함수
		unsigned ControlThreadMain(CNetServer* pNetServer);

		// MS 윈속 확장 함수들 로딩
		// [in] pCtx: 연결할 소켓의 Context
		// [retVal] FALSE: 로딩 실패
		BOOL LoadWinSockExFunc(CNetListenCtx* pListenCtx);
		
		// 소켓바인드 및 리슨 이후에 호출
		// [in] pListenCtx: 리스닝 소켓의 Context
		RET_CODE StartListen(CNetListenCtx* pListenCtx);

		// 성공여부에 상관없이 Overlapped IO 작업 종료 와 커넥션 종료를 요청
		// bRequestCloseConn 요청 이후에만 pIoData 객체 삭제됨
		// [in] pIoData: 완료된 IO data
		// [in] bRequestCloseConn: TCP 커넥션 종료 요청 여부
		void CloseOp(CNetServer* pThis, CNetIoData* pIoData, BOOL bRequestCloseConn);
		

		// 해당 소켓에 overlapped receive operation 실행
		// [in] pCtx: 작업 소켓 context
		// [in] pIoData: IO 작업별 데이터
		// [in] dwOpBufSize: IO 작업에 필요한 버퍼 크기
		// [retVal] RET_CODE
		//		E_SUCCESS: 성공
		//		기타: 실패, 요청 무시됨
		RET_CODE PostRecvOp(CNetServer* pThis, CNetCtx* pCtx, CNetIoData* pIoData, DWORD dwOpBufSize);
		RET_CODE PostSendOp(CNetServer* pThis, CNetCtx* pCtx, CNetIoData* pIoData);
	};




};