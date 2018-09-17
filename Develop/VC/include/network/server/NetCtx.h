//						NetCtx.h
//			NetServer���� ���Ǵ� ���� Context Ŭ���� �� ���� Ŭ������, I/O �۾� ������ Ŭ���� ����
//
//		2011/01/xx
//		�ۼ���: dede
//		�ۼ���(dede)�� ���Ǿ��� �ش� �ҽ��� ���, �����ϴ� ���� �ҹ��Դϴ�.
//		
//		���� ������: 2011/01/xx
//
//		[�����丮]
//		2011/01/xx
//			1. InitializeCriticalSectionAndSpinCount() ���
//
//		2010/06/xx
//			1. ���� ����
//
//		[���ǻ���]
//		- 
//
//		[�������]
//		- �Ʒ����� ���Ǵ� stl list �� ������� FIFO����(push_front, pop_front)
//
//		[ToDoList]
//		- ���ҽ� ���� �ڵ�, �ٽ� Ȯ�� �ʿ�
//
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <network/IpAddrResolver.h>	// addrinfo ����
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
	//	- Overlapped I/O �۾��� ������ Ŭ����
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

		// ������ �����ϵ��� ������� �ʱ�ȭ
		void Reset();
		
	public:
		WSAOVERLAPPED m_Ol;
		OP m_Op;
		CNetBuf* m_pBuf;

		CNetCtx* m_pNetCtx;		// ���� Context ��ü
	};
	typedef list<CNetIoData*>					LST_PNETIODATA;
	typedef list<CNetIoData*>::const_iterator	LST_PNETIODATA_CIT;
	typedef list<CNetIoData*>::iterator			LST_PNETIODATA_IT;

	//////////////////////////////////////////////////////////
	//				CNetCtx
	//
	//	- ���Ϻ� Context �������� �߻� Ŭ����
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

		// ���� �Ϸ���� ���� overlapped operations
		LONG m_nOutstandingRecv;
		LONG m_nOutstandingSend;
	};

	

	//////////////////////////////////////////////////////////
	//				CNetListenCtx
	//
	//	- ���� ���Ϻ� Context �������� �߻� Ŭ����
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
	//	- TCP ���� ���Ϻ� Context �������� ���� Ŭ����
	//
	class CTcpListenCtx : public CNetListenCtx
	{
		friend class CTcpListenCtxMan;

	public:
		virtual ~CTcpListenCtx();

		//////////////////////////////////////////////////////////
		//			PendingAccepts ����Ʈ ���� �Լ�
		//////////////////////////////////////////////////////////

		// Accept ��� �۾��� ���� �߰�
		void AddPendingAccept(CNetIoData* pIoData);

		// Accept ��� �۾��� ���� ����
		void RemovePendingAccept(CNetIoData* pIoData);

		// Accept ��� �۾� ����Ʈ ��ȯ (���� ����ȭ�ʿ�)
		LST_PNETIODATA& GetPendingAcceptIoList() { return m_PendingAcceptIoList; }
		int GetPendingAcceptIoCount() const { return m_PendingAcceptIoList.size(); }

	protected:
		CTcpListenCtx();

	public:	
		HANDLE m_hFdAcceptEvt;	// FD_ACCEPT �˸� �̺�Ʈ

		// ���� Ȯ�� �Լ� ����Ʈ
		LPFN_ACCEPTEX				ACCEPTEX;
		LPFN_GETACCEPTEXSOCKADDRS	GETACCEPTEXSOCKADDRS;

	protected:
		// �������Ͽ��� AcceptEx ��� �۾����� NetIoData�� ����Ʈ
		LST_PNETIODATA m_PendingAcceptIoList;
	};


	//////////////////////////////////////////////////////////
	//				CUdpListenCtx
	//
	//	- UDP ���� ���Ϻ� Context �������� ���� Ŭ����
	//
	class CUdpListenCtx : public CNetListenCtx
	{
		friend class CUdpListenCtxMan;

	public:
		virtual ~CUdpListenCtx() {}

	protected:
		CUdpListenCtx();
	
	public:
		SOCKADDR_STORAGE m_ClientAddr;		// ���� Client�� IP �ּ�
		int m_nClientAddrLen;
	};



	//////////////////////////////////////////////////////////
	//				CNetClientCtx
	//
	//	- Ŭ���̾�Ʈ�� ���Ϻ� Context �������� �߻� Ŭ����
	//
	class CNetClientCtx : public CNetCtx
	{
	public:
		CNetClientCtx() { m_pListenCtx = NULL; }
		virtual ~CNetClientCtx() {}

		// ������ ���ؼ� ��� ������ �ʱ�ȭ
		virtual void Reset() = 0;

	public:
		CNetListenCtx* m_pListenCtx;	// ���� ListenCtx
	};
	typedef list<CNetClientCtx*>					LST_PNETCLIENTCTX;
	typedef list<CNetClientCtx*>::const_iterator	LST_PNETCLIENTCTX_CIT;
	typedef list<CNetClientCtx*>::iterator			LST_PNETCLIENTCTX_IT;


	//////////////////////////////////////////////////////////
	//				CTcpClientCtx
	//
	//	- Ŭ���̾�Ʈ�� TCP ���Ϻ� Context �������� ���� Ŭ����
	//
	class CTcpClientCtx : public CNetClientCtx
	{
		friend class CTcpClientCtxMan;

	public:
		virtual ~CTcpClientCtx();

	protected:
		CTcpClientCtx();

		// ������ ���ؼ� ��� ������ �ʱ�ȭ
		void Reset();

	public:
		string m_Ip; // Ŭ���̾�Ʈ ���� IP
		BOOL m_bRequestedClose;	// ���� ���� ��û�� �ִ��� ����
	};
	

	//////////////////////////////////////////////////////////
	//				CUdpClientCtx
	//
	//	- Ŭ���̾�Ʈ�� UDP ���Ϻ� Context �������� ���� Ŭ����
	//
	class CUdpClientCtx : public CNetClientCtx
	{
		friend class CUdpClientCtxMan;

	public:
		virtual ~CUdpClientCtx() {}

	protected:
		CUdpClientCtx() {}

		// ������ ���ؼ� ��� ������ �ʱ�ȭ
		void Reset() {}
	};

		


	//////////////////////////////////////////////////////////
	//					Management Classes
	//////////////////////////////////////////////////////////


	//////////////////////////////////////////////////////////
	//				CNetIoDataMan
	//
	//	- CNetIoData ��ü�� ����/���� �� CNetIoData ����Ʈ�� �����ϴ� �߻� Ŭ����
	//
	class CNetIoDataMan
	{
	public:
		enum 
		{
			DEFAULT_MAX_FREELIST_SIZE = 0,	// 0 - FreeList ������� ����
			NETBUF_ALLOC_RETRY_COUNT = 3	// CNetBuf::AllocBuf() ���н� ��õ� Ƚ��
		};

		CNetIoDataMan(CNetBufMan* pNetBufMan);
		virtual ~CNetIoDataMan();
		
		// FreeList ũ�� ����
		void SetMaxFreeListSize(int nMaxSize = DEFAULT_MAX_FREELIST_SIZE) { m_nMaxFreeSize = nMaxSize; }

	public:
		// CNetServer���� �������� Alloc�ߴ� CNetIoData�� �����ϱ� ���� ���
		virtual void Cleanup();

		// ���ο� CNetIoData ��ü ����
		// Free����Ʈ�� Ȯ���� �� ��������, ������ ���� ����
		// [retVal] NULL: ����
		virtual CNetIoData* AllocData();
		
		// CNetIoData ��ü ����
		// ������ ��ü�� FreeList�� �߰���
		virtual void DeallocData(CNetIoData* pData);

	protected:
		// �޸𸮿��� ��ü ����
		// [retVal] NULL: ����
		virtual CNetIoData* CreateData();

		// �޸𸮿��� ��ü ����
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
	//	- ListenCtx ��ü�� ����/���� �� ListenCtxes ����Ʈ�� �����ϴ� �߻� Ŭ����
	//
	class CNetListenCtxMan
	{
	public:
		CNetListenCtxMan() {}
		virtual ~CNetListenCtxMan() {}
	
	public:
		// �߰��� NetListen ��ü �ִ� ����
		void SetMaxNetListenListSize(int nMaxSize);

		// CNetServer���� �������� Alloc�ߴ� Ctx�� �޸𸮿��� �����ϱ� ���� ���
		virtual void Cleanup();

		// ���ο� CNetListenCtx ��ü�� �޸𸮿��� ����
		// [retVal] NULL: ����
		virtual CNetListenCtx* CreateCtx() = 0;

		// CNetListenCtx ��ü�� �޸𸮿��� ����
		virtual void DeleteCtx(CNetListenCtx* pCtx) = 0;

		// NetListenCtxes ����Ʈ�� �߰�
		void AddNetListenCtx(CNetListenCtx* pCtx);

	public:
		VCT_PNETLISTENCTX m_NetListenCtxList;
	};


	//////////////////////////////////////////////////////////
	//				CTcpListenCtxMan
	//
	//	- TCP�� ListenCtx ��ü�� ����/���� �� �����ϴ� ���� Ŭ����
	//
	class CTcpListenCtxMan : public CNetListenCtxMan
	{
	public:
		CTcpListenCtxMan() {}
		virtual ~CTcpListenCtxMan() {}

		// ���ο� CNetListenCtx ��ü�� �޸𸮿��� ����
		// [retVal] NULL: ����
		CNetListenCtx* CreateCtx();

		// CNetListenCtx ��ü�� �޸𸮿��� ����
		void DeleteCtx(CNetListenCtx* pCtx);
	};

	//////////////////////////////////////////////////////////
	//				CTcpListenCtxMan
	//
	//	- UDP�� ListenCtx ��ü�� ����/���� �� �����ϴ� ���� Ŭ����
	//
	class CUdpListenCtxMan : public CNetListenCtxMan
	{
	public:
		CUdpListenCtxMan() {}
		virtual ~CUdpListenCtxMan() {}

		// ���ο� CNetListenCtx ��ü�� �޸𸮿��� ����
		// [retVal] NULL: ����
		CNetListenCtx* CreateCtx();

		// CNetListenCtx ��ü�� �޸𸮿��� ����
		void DeleteCtx(CNetListenCtx* pCtx);
	};




	//////////////////////////////////////////////////////////
	//				CNetClientCtxMan
	//
	//	- ClientCtx ��ü�� ����/���� �� �����ϴ� �߻� Ŭ����
	//  - Dealloc�� Ctx�� ���� �޸𸮿��� �������� �ʰ� ���� ����� ���� FreeList�� ����
	//
	class CNetClientCtxMan
	{
	public:
		enum 
		{
			DEFAULT_MAX_FREELIST_SIZE = 0	// 0 - FreeList ������� ����
		};

		CNetClientCtxMan();
		virtual ~CNetClientCtxMan();

		// FreeList ũ�� ����
		void SetMaxFreeListSize(int nMaxSize = DEFAULT_MAX_FREELIST_SIZE) { m_nMaxFreeSize = nMaxSize; }

	public:
		// ����Ʈ ������ ����ȭ �ʿ�
		LST_PNETCLIENTCTX& GetUsedCtxList() { return m_UsedList; }

		// CNetServer���� �������� Alloc�ߴ� Ctx�� �����ϱ� ���� ���
		virtual void Cleanup();

		// ���ο� CNetClientCtx ��ü ����
		// Free����Ʈ�� Ȯ���� �� ��������, ������ ���� ����
		// [retVal] NULL: ����
		virtual CNetClientCtx* AllocCtx();

		// CNetClientCtx ��ü ����
		// ������ ��ü�� FreeList�� �߰���
		virtual void DeallocCtx(CNetClientCtx* pCtx);

	protected:
		// �޸𸮿��� ��ü ����
		// [retVal] NULL: ����
		virtual CNetClientCtx* CreateCtx() = 0;

		// �޸𸮿��� ��ü ����
		virtual void DeleteCtx(CNetClientCtx* pCtx) = 0;

	public:
		CRITICAL_SECTION m_Cs;

	protected:
		// ���ǰ� ���� ���� Ctx�� Free ����Ʈ
		int m_nMaxFreeSize;
		LST_PNETCLIENTCTX m_FreeList;

		// ������� Ctx ����Ʈ
		LST_PNETCLIENTCTX m_UsedList;
	};


	//////////////////////////////////////////////////////////
	//				CTcpClientCtxMan
	//
	//	- TCP�� ClientCtx ��ü�� ����/���� �� �����ϴ� ���� Ŭ����
	//
	class CTcpClientCtxMan : public CNetClientCtxMan
	{
	public:
		CTcpClientCtxMan() {}
		virtual ~CTcpClientCtxMan() {}
	
	protected:
		// �޸𸮿��� ��ü ����
		// [retVal] NULL: ����
		CNetClientCtx* CreateCtx();

		// �޸𸮿��� ��ü ����
		void DeleteCtx(CNetClientCtx* pCtx);	
	};


	//////////////////////////////////////////////////////////
	//				CUdpClientCtxMan
	//
	//	- UDP�� ClientCtx ��ü�� ����/���� �� �����ϴ� ���� Ŭ����
	//
	class CUdpClientCtxMan : public CNetClientCtxMan
	{
	public:
		CUdpClientCtxMan() {}
		virtual ~CUdpClientCtxMan() {}

		protected:
		// �޸𸮿��� ��ü ����
		// [retVal] NULL: ����
		CNetClientCtx* CreateCtx();

		// �޸𸮿��� ��ü ����
		void DeleteCtx(CNetClientCtx* pCtx);
	};


};