//						NetCtx.h
//			NetServer에서 사용되는 소켓 Context 클래스 및 관리 클래스들, I/O 작업 데이터 클래스 정의
//
//		2011/01/xx
//		작성자: dede
//		작성자(dede)의 동의없이 해당 소스를 사용, 변경하는 것은 불법입니다.
//		
//		최종 수정일: 2011/01/xx
//
//		[히스토리]
//		2011/01/xx
//			1. InitializeCriticalSectionAndSpinCount() 사용
//
//		2010/06/xx
//			1. 최초 개발
//
//		[주의사항]
//		- 
//
//		[참고사항]
//		- 아래에서 사용되는 stl list 는 헤더방향 FIFO형임(push_front, pop_front)
//
//		[ToDoList]
//		- 리소스 해제 코드, 다시 확인 필요
//
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <network/IpAddrResolver.h>	// addrinfo 선언
#include <network/NetBuf.h>
#include <mswsock.h>
#include <new>
#include <list>
#include <vector>
using namespace std;
using namespace Network;

namespace NetServer
{	
	class CNetCtx;

	//////////////////////////////////////////////////////////
	//					Data Classes
	//////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////
	//				CNetIoData
	//
	//	- Overlapped I/O 작업별 데이터 클래스
	//
	class CNetIoData
	{
	public:

		// Operation
		typedef enum _OP
		{
			OP_ACCEPT,		// AcceptEx
			OP_READ,		// WSARecv/WSARecvFrom
			OP_WRITE		// WSASend/WSASendTo
		} OP;

		CNetIoData(void);
		virtual ~CNetIoData(void);

		// 재사용이 가능하도록 멤버변수 초기화
		void Reset();
		
	public:
		WSAOVERLAPPED m_Ol;
		OP m_Op;
		CNetBuf* m_pBuf;

		CNetCtx* m_pNetCtx;		// 소유 Context 객체
	};
	typedef list<CNetIoData*>					LST_PNETIODATA;
	typedef list<CNetIoData*>::const_iterator	LST_PNETIODATA_CIT;
	typedef list<CNetIoData*>::iterator			LST_PNETIODATA_IT;

	//////////////////////////////////////////////////////////
	//				CNetCtx
	//
	//	- 소켓별 Context 데이터의 추상 클래스
	//
	class CNetCtx
	{
	public:
		CNetCtx(void);
		virtual ~CNetCtx(void);

	public:
		CRITICAL_SECTION m_Cs;		
		SOCKET m_Socket;
		time_t m_LastTime;

		// 아직 완료되지 않은 overlapped operations
		LONG m_nOutstandingRecv;
		LONG m_nOutstandingSend;
	};

	

	//////////////////////////////////////////////////////////
	//				CNetListenCtx
	//
	//	- 리슨 소켓별 Context 데이터의 추상 클래스
	//
	class CNetListenCtx : public CNetCtx
	{
	public:
		CNetListenCtx();
		virtual ~CNetListenCtx();

	public:
		addrinfo m_AddrInfo;
	};
	typedef vector<CNetListenCtx*>					VCT_PNETLISTENCTX;
	typedef vector<CNetListenCtx*>::const_iterator	VCT_PNETLISTENCTX_CIT;
	typedef vector<CNetListenCtx*>::iterator		VCT_PNETLISTENCTX_IT;



	//////////////////////////////////////////////////////////
	//				CTcpListenCtx
	//
	//	- TCP 리슨 소켓별 Context 데이터의 구현 클래스
	//
	class CTcpListenCtx : public CNetListenCtx
	{
		friend class CTcpListenCtxMan;

	public:
		virtual ~CTcpListenCtx();

		//////////////////////////////////////////////////////////
		//			PendingAccepts 리스트 관련 함수
		//////////////////////////////////////////////////////////

		// Accept 대기 작업의 정보 추가
		void AddPendingAccept(CNetIoData* pIoData);

		// Accept 대기 작업의 정보 제거
		void RemovePendingAccept(CNetIoData* pIoData);

		// Accept 대기 작업 리스트 반환 (사용시 동기화필요)
		LST_PNETIODATA& GetPendingAcceptIoList() { return m_PendingAcceptIoList; }
		int GetPendingAcceptIoCount() const { return m_PendingAcceptIoList.size(); }

	protected:
		CTcpListenCtx();

	public:	
		HANDLE m_hFdAcceptEvt;	// FD_ACCEPT 알림 이벤트

		// 윈속 확장 함수 포인트
		LPFN_ACCEPTEX				ACCEPTEX;
		LPFN_GETACCEPTEXSOCKADDRS	GETACCEPTEXSOCKADDRS;

	protected:
		// 리슨소켓에서 AcceptEx 대기 작업중인 NetIoData의 리스트
		LST_PNETIODATA m_PendingAcceptIoList;
	};


	//////////////////////////////////////////////////////////
	//				CUdpListenCtx
	//
	//	- UDP 리슨 소켓별 Context 데이터의 구현 클래스
	//
	class CUdpListenCtx : public CNetListenCtx
	{
		friend class CUdpListenCtxMan;

	public:
		virtual ~CUdpListenCtx() {}

	protected:
		CUdpListenCtx();
	
	public:
		SOCKADDR_STORAGE m_ClientAddr;		// 접속 Client의 IP 주소
		int m_nClientAddrLen;
	};



	//////////////////////////////////////////////////////////
	//				CNetClientCtx
	//
	//	- 클라이언트의 소켓별 Context 데이터의 추상 클래스
	//
	class CNetClientCtx : public CNetCtx
	{
	public:
		CNetClientCtx() { m_pListenCtx = NULL; }
		virtual ~CNetClientCtx() {}

		// 재사용을 위해서 멤버 변수를 초기화
		virtual void Reset() = 0;

	public:
		CNetListenCtx* m_pListenCtx;	// 소유 ListenCtx
	};
	typedef list<CNetClientCtx*>					LST_PNETCLIENTCTX;
	typedef list<CNetClientCtx*>::const_iterator	LST_PNETCLIENTCTX_CIT;
	typedef list<CNetClientCtx*>::iterator			LST_PNETCLIENTCTX_IT;


	//////////////////////////////////////////////////////////
	//				CTcpClientCtx
	//
	//	- 클라이언트의 TCP 소켓별 Context 데이터의 구현 클래스
	//
	class CTcpClientCtx : public CNetClientCtx
	{
		friend class CTcpClientCtxMan;

	public:
		virtual ~CTcpClientCtx();

	protected:
		CTcpClientCtx();

		// 재사용을 위해서 멤버 변수를 초기화
		void Reset();

	public:
		string m_Ip; // 클라이언트 접속 IP
		BOOL m_bRequestedClose;	// 연결 종료 요청이 있는지 여부
	};
	

	//////////////////////////////////////////////////////////
	//				CUdpClientCtx
	//
	//	- 클라이언트의 UDP 소켓별 Context 데이터의 구현 클래스
	//
	class CUdpClientCtx : public CNetClientCtx
	{
		friend class CUdpClientCtxMan;

	public:
		virtual ~CUdpClientCtx() {}

	protected:
		CUdpClientCtx() {}

		// 재사용을 위해서 멤버 변수를 초기화
		void Reset() {}
	};

		


	//////////////////////////////////////////////////////////
	//					Management Classes
	//////////////////////////////////////////////////////////


	//////////////////////////////////////////////////////////
	//				CNetIoDataMan
	//
	//	- CNetIoData 객체의 생성/해제 및 CNetIoData 리스트를 관리하는 추상 클래스
	//
	class CNetIoDataMan
	{
	public:
		enum 
		{
			DEFAULT_MAX_FREELIST_SIZE = 0,	// 0 - FreeList 사용하지 않음
			NETBUF_ALLOC_RETRY_COUNT = 3	// CNetBuf::AllocBuf() 실패시 재시도 횟수
		};

		CNetIoDataMan(CNetBufMan* pNetBufMan);
		virtual ~CNetIoDataMan();
		
		// FreeList 크기 지정
		void SetMaxFreeListSize(int nMaxSize = DEFAULT_MAX_FREELIST_SIZE) { m_nMaxFreeSize = nMaxSize; }

	public:
		// CNetServer에서 종료전에 Alloc했던 CNetIoData들 해제하기 위해 사용
		virtual void Cleanup();

		// 새로운 CNetIoData 객체 생성
		// Free리스트를 확인한 후 가져오고, 없으면 새로 생성
		// [retVal] NULL: 실패
		virtual CNetIoData* AllocData();
		
		// CNetIoData 객체 해제
		// 해제된 객체는 FreeList에 추가됨
		virtual void DeallocData(CNetIoData* pData);

	protected:
		// 메모리에서 객체 생성
		// [retVal] NULL: 실패
		virtual CNetIoData* CreateData();

		// 메모리에서 객체 삭제
		virtual void DeleteData(CNetIoData* pData);

	protected:
		CRITICAL_SECTION m_Cs;
		CNetBufMan* m_pNetBufMan;

		int m_nMaxFreeSize;
		LST_PNETIODATA m_FreeList;
		LST_PNETIODATA m_UsedList;
	};

	//////////////////////////////////////////////////////////
	//				CNetListenCtxMan
	//
	//	- ListenCtx 객체의 생성/해제 및 ListenCtxes 리스트를 관리하는 추상 클래스
	//
	class CNetListenCtxMan
	{
	public:
		CNetListenCtxMan() {}
		virtual ~CNetListenCtxMan() {}
	
	public:
		// 추가될 NetListen 객체 최대 개수
		void SetMaxNetListenListSize(int nMaxSize);

		// CNetServer에서 종료전에 Alloc했던 Ctx들 메모리에서 해제하기 위해 사용
		virtual void Cleanup();

		// 새로운 CNetListenCtx 객체를 메모리에서 생성
		// [retVal] NULL: 실패
		virtual CNetListenCtx* CreateCtx() = 0;

		// CNetListenCtx 객체를 메모리에서 해제
		virtual void DeleteCtx(CNetListenCtx* pCtx) = 0;

		// NetListenCtxes 리스트에 추가
		void AddNetListenCtx(CNetListenCtx* pCtx);

	public:
		VCT_PNETLISTENCTX m_NetListenCtxList;
	};


	//////////////////////////////////////////////////////////
	//				CTcpListenCtxMan
	//
	//	- TCP용 ListenCtx 객체의 생성/해제 및 관리하는 구현 클래스
	//
	class CTcpListenCtxMan : public CNetListenCtxMan
	{
	public:
		CTcpListenCtxMan() {}
		virtual ~CTcpListenCtxMan() {}

		// 새로운 CNetListenCtx 객체를 메모리에서 생성
		// [retVal] NULL: 실패
		CNetListenCtx* CreateCtx();

		// CNetListenCtx 객체를 메모리에서 해제
		void DeleteCtx(CNetListenCtx* pCtx);
	};

	//////////////////////////////////////////////////////////
	//				CTcpListenCtxMan
	//
	//	- UDP용 ListenCtx 객체의 생성/해제 및 관리하는 구현 클래스
	//
	class CUdpListenCtxMan : public CNetListenCtxMan
	{
	public:
		CUdpListenCtxMan() {}
		virtual ~CUdpListenCtxMan() {}

		// 새로운 CNetListenCtx 객체를 메모리에서 생성
		// [retVal] NULL: 실패
		CNetListenCtx* CreateCtx();

		// CNetListenCtx 객체를 메모리에서 해제
		void DeleteCtx(CNetListenCtx* pCtx);
	};




	//////////////////////////////////////////////////////////
	//				CNetClientCtxMan
	//
	//	- ClientCtx 객체의 생성/해제 및 관리하는 추상 클래스
	//  - Dealloc된 Ctx를 실제 메모리에서 해제하지 않고 다음 사용을 위해 FreeList를 관리
	//
	class CNetClientCtxMan
	{
	public:
		enum 
		{
			DEFAULT_MAX_FREELIST_SIZE = 0	// 0 - FreeList 사용하지 않음
		};

		CNetClientCtxMan();
		virtual ~CNetClientCtxMan();

		// FreeList 크기 지정
		void SetMaxFreeListSize(int nMaxSize = DEFAULT_MAX_FREELIST_SIZE) { m_nMaxFreeSize = nMaxSize; }

	public:
		// 리스트 수정시 동기화 필요
		LST_PNETCLIENTCTX& GetUsedCtxList() { return m_UsedList; }

		// CNetServer에서 종료전에 Alloc했던 Ctx들 해제하기 위해 사용
		virtual void Cleanup();

		// 새로운 CNetClientCtx 객체 생성
		// Free리스트를 확인한 후 가져오고, 없으면 새로 생성
		// [retVal] NULL: 실패
		virtual CNetClientCtx* AllocCtx();

		// CNetClientCtx 객체 해제
		// 해제된 객체는 FreeList에 추가됨
		virtual void DeallocCtx(CNetClientCtx* pCtx);

	protected:
		// 메모리에서 객체 생성
		// [retVal] NULL: 실패
		virtual CNetClientCtx* CreateCtx() = 0;

		// 메모리에서 객체 삭제
		virtual void DeleteCtx(CNetClientCtx* pCtx) = 0;

	public:
		CRITICAL_SECTION m_Cs;

	protected:
		// 사용되고 있지 않은 Ctx의 Free 리스트
		int m_nMaxFreeSize;
		LST_PNETCLIENTCTX m_FreeList;

		// 사용중인 Ctx 리스트
		LST_PNETCLIENTCTX m_UsedList;
	};


	//////////////////////////////////////////////////////////
	//				CTcpClientCtxMan
	//
	//	- TCP용 ClientCtx 객체의 생성/해제 및 관리하는 구현 클래스
	//
	class CTcpClientCtxMan : public CNetClientCtxMan
	{
	public:
		CTcpClientCtxMan() {}
		virtual ~CTcpClientCtxMan() {}
	
	protected:
		// 메모리에서 객체 생성
		// [retVal] NULL: 실패
		CNetClientCtx* CreateCtx();

		// 메모리에서 객체 삭제
		void DeleteCtx(CNetClientCtx* pCtx);	
	};


	//////////////////////////////////////////////////////////
	//				CUdpClientCtxMan
	//
	//	- UDP용 ClientCtx 객체의 생성/해제 및 관리하는 구현 클래스
	//
	class CUdpClientCtxMan : public CNetClientCtxMan
	{
	public:
		CUdpClientCtxMan() {}
		virtual ~CUdpClientCtxMan() {}

		protected:
		// 메모리에서 객체 생성
		// [retVal] NULL: 실패
		CNetClientCtx* CreateCtx();

		// 메모리에서 객체 삭제
		void DeleteCtx(CNetClientCtx* pCtx);
	};


};