#include "StdAfx.h"
#include <network/server/NetCtx.h>
#include <crtdbg.h> // _ASSERT
#include <algorithm>

using namespace NetServer;

/*
//////////////////////////////////////////////////////////
//			CNetCtxList
//////////////////////////////////////////////////////////
// �� �������� ����� �߰�
// [in] pCtx: �� ������
// [retVal] �߰��� ����Ʈ
CNetCtx* CNetCtxList::AddHeader(CNetCtx* pCtxes, CNetCtx* pCtx)
{
	pCtx->m_pNext = NULL;

	if (!pCtxes)
	{
		pCtxes = pCtx;
	}
	else
	{
		// ����� �߰� - ������ �������
		pCtx->m_pNext = pCtxes;
		pCtxes = pCtx;
	}
	return pCtxes;
}

// �ش� �������� ����Ʈ���� ����
// [in] pCtx: ������ ������
// [retVal] ������ ����Ʈ
CNetCtx* CNetCtxList::Remove(CNetCtx* pCtxes, CNetCtx* pCtx)
{
	CNetCtx *pPos = NULL, *pPrev = NULL;
		
	pPos = pCtxes;
	
	// ����Ʈ���� pCtx ã��
	while (pPos && pPos != pCtx)
	{
		pPrev = pPos;
		pPos = pPos->m_pNext;
	}

	if (pPrev)
	{
		// �߰���ġ���� pCtx �߰�
		pPrev->m_pNext = pCtx->m_pNext;
	}
	else
	{
		// �� ó�� ��ġ���� �߰�
		pCtxes = pCtx->m_pNext;
	}
	
	pCtx->m_pNext = NULL;

	return pCtxes;	
}

// ����Ʈ�� ��� �������� ���� �� ������ ����� ��ȯ
// [out] pCtx: ������ ���
// [retVal] ������ ����Ʈ
CNetCtx* CNetCtxList::RemoveHeader(CNetCtx* pCtxes, CNetCtx* pCtx)
{
	pCtx = NULL;
	if (pCtxes)
	{
		pCtx = pCtxes->m_pNext;
		pCtxes = pCtxes->m_pNext;
		pCtx->m_pNext = NULL;

	}
	
	return pCtxes;
}
*/


//////////////////////////////////////////////////////////
//			CNetIoData
/////////////////////////////////////////////////////////
CNetIoData::CNetIoData()
{
	ZeroMemory(&m_Ol, sizeof(WSAOVERLAPPED));
	m_Op = OP_ACCEPT;
	m_pNetCtx = NULL;
	m_pBuf = NULL;
}

CNetIoData::~CNetIoData()
{
	m_pBuf = NULL;
}

// ������ �����ϵ��� ������� �ʱ�ȭ
void CNetIoData::Reset()
{
	ZeroMemory(&m_Ol, sizeof(WSAOVERLAPPED));
	m_Op = OP_ACCEPT;
	m_pNetCtx = NULL;

	if (m_pBuf)
		m_pBuf->Clear();
}

//////////////////////////////////////////////////////////
//			CNetCtx
//////////////////////////////////////////////////////////
CNetCtx::CNetCtx()
{
	InitializeCriticalSectionAndSpinCount(&m_Cs, 4000);
	m_Socket = INVALID_SOCKET;
	m_LastTime = 0;

	m_nOutstandingRecv = m_nOutstandingSend = 0;
}

CNetCtx::~CNetCtx()
{
	if (m_Socket != INVALID_SOCKET)
	{
		closesocket(m_Socket);
		m_Socket = INVALID_SOCKET;
	}

	DeleteCriticalSection(&m_Cs);
}


//////////////////////////////////////////////////////////
//			CTcpClientCtx
//////////////////////////////////////////////////////////
CTcpClientCtx::CTcpClientCtx()
{
	m_bRequestedClose = FALSE;
}

CTcpClientCtx::~CTcpClientCtx()
{
}

// ������ ���ؼ� ��� ������ �ʱ�ȭ
void CTcpClientCtx::Reset()
{
	m_LastTime = 0;
	m_nOutstandingRecv = 0;
	m_nOutstandingSend = 0;
	m_bRequestedClose = FALSE;
	m_Ip = "";
	
	m_pListenCtx = NULL;

	if (m_Socket != INVALID_SOCKET)
	{
		closesocket(m_Socket);
		m_Socket = INVALID_SOCKET;
	}
}

//////////////////////////////////////////////////////////
//			CNetListenCtx
//////////////////////////////////////////////////////////
CNetListenCtx::CNetListenCtx()
{ 
	ZeroMemory(&m_AddrInfo, sizeof(addrinfo)); 
}


CNetListenCtx::~CNetListenCtx()
{
}

//////////////////////////////////////////////////////////
//			CUdpListenCtx
//////////////////////////////////////////////////////////
CUdpListenCtx::CUdpListenCtx()
{ 
	ZeroMemory(&m_ClientAddr, sizeof(SOCKADDR_STORAGE));
	m_nClientAddrLen = 0;
}

//////////////////////////////////////////////////////////
//			CTcpListenCtx
//////////////////////////////////////////////////////////
CTcpListenCtx::CTcpListenCtx()
{
	m_hFdAcceptEvt = CreateEvent(NULL, TRUE, FALSE, NULL);

	ACCEPTEX = NULL;
	GETACCEPTEXSOCKADDRS = NULL;
}

CTcpListenCtx::~CTcpListenCtx()
{
	CloseHandle(m_hFdAcceptEvt);
}

// Accept ��� �۾��� ���� �߰�
void CTcpListenCtx::AddPendingAccept(CNetIoData* pIoData)
{
	_ASSERT(pIoData);
	EnterCriticalSection(&m_Cs);

	m_PendingAcceptIoList.push_front(pIoData);
	
	LeaveCriticalSection(&m_Cs);
}

// PendingAccepts ����Ʈ���� ����
void CTcpListenCtx::RemovePendingAccept(CNetIoData* pIoData)
{
	_ASSERT(pIoData);
	LST_PNETIODATA_IT it;
	EnterCriticalSection(&m_Cs);

	it = find(m_PendingAcceptIoList.begin(), m_PendingAcceptIoList.end(), pIoData);
	if (it != m_PendingAcceptIoList.end())
		m_PendingAcceptIoList.erase(it);

	LeaveCriticalSection(&m_Cs);
}



//////////////////////////////////////////////////////////
//			CNetIoDataMan
//////////////////////////////////////////////////////////
CNetIoDataMan::CNetIoDataMan(CNetBufMan* pNetBufMan)
{
	_ASSERT(pNetBufMan);
	InitializeCriticalSectionAndSpinCount(&m_Cs, 4000);
	m_pNetBufMan = pNetBufMan;
	m_nMaxFreeSize = DEFAULT_MAX_FREELIST_SIZE;
}

CNetIoDataMan::~CNetIoDataMan()
{
	DeleteCriticalSection(&m_Cs);
}

// CNetServer���� �������� Alloc�ߴ� CNetIoData�� �����ϱ� ���� ���
void CNetIoDataMan::Cleanup()
{
	LST_PNETIODATA_IT it;
	for (it = m_FreeList.begin(); it != m_FreeList.end(); ++it)
		DeleteData((*it));

	for (it = m_UsedList.begin(); it != m_UsedList.end(); ++it)
		DeleteData((*it));
}

// ���ο� CNetIoData ��ü ����
// Free����Ʈ�� Ȯ���� �� ��������, ������ ���� ����
// [retVal] NULL: ����
CNetIoData* CNetIoDataMan::AllocData()
{
	CNetIoData* retVal = NULL;
	LST_PNETIODATA_IT it;
	EnterCriticalSection(&m_Cs);
	
	it = m_FreeList.begin();
	if (it != m_FreeList.end())
	{
		// FreeList���� ��ȯ�� FreeList���� ����
		retVal = (*it);
		m_FreeList.pop_front();
	}
	else
	{
		// �޸𸮿��� ����
		retVal = CreateData();
	}

	// UsedList�� �߰�
	if (retVal)
		m_UsedList.push_front(retVal);

	LeaveCriticalSection(&m_Cs);
	return retVal;
}

// CNetIoData ��ü ����
// ������ ��ü�� FreeList�� �߰���
void CNetIoDataMan::DeallocData(CNetIoData* pData)
{
	_ASSERT(pData);

	LST_PNETIODATA_IT it;
	EnterCriticalSection(&m_Cs);
	
	// UsedList���� ����
	it = find(m_UsedList.begin(), m_UsedList.end(), pData);
	if (it != m_UsedList.end())
	{
		m_UsedList.erase(it);
	
		if ((int)m_FreeList.size() < m_nMaxFreeSize)
		{
			// FreeList�� �߰�
			pData->Reset();
			m_FreeList.push_front(pData);
		}
		else
		{
			// FreeList�� ������ �޸𸮿��� ����
			DeleteData(pData);
		}
	}

	LeaveCriticalSection(&m_Cs);
}

// �޸𸮿��� ��ü ����
// [retVal] NULL: ����
CNetIoData* CNetIoDataMan::CreateData()
{
	int i = 0;
	CNetIoData* pIoData = new(nothrow) CNetIoData();
	if (pIoData == NULL)
		return NULL;

	while (i < NETBUF_ALLOC_RETRY_COUNT)
	{
		pIoData->m_pBuf = m_pNetBufMan->AllocBuf();
		if (pIoData->m_pBuf)
		{
			// ���� �Ҵ� ����
			break;
		}
		else
		{
			// ���� �Ҵ� ����
			Sleep(5);
			i++;
		}
	}

	if (pIoData->m_pBuf == NULL)
	{
		// ��õ� ���Ŀ��� ���� �Ҵ� ����
		delete pIoData;
		pIoData = NULL;
	}

	return pIoData;
}

// �޸𸮿��� ��ü ����
void CNetIoDataMan::DeleteData(CNetIoData* pData)
{
	_ASSERT(pData);
	m_pNetBufMan->DeallocBuf(pData->m_pBuf);
	delete pData;
}

//////////////////////////////////////////////////////////
//			CNetListenCtxMan
//////////////////////////////////////////////////////////
// �߰��� NetListen ��ü �ִ� ����
void CNetListenCtxMan::SetMaxNetListenListSize(int nMaxSize)
{
	m_NetListenCtxList.reserve(nMaxSize);
}

// CNetServer���� �������� Alloc�ߴ� Ctx�� �޸𸮿��� �����ϱ� ���� ���
void CNetListenCtxMan::Cleanup()
{
	for (VCT_PNETLISTENCTX_IT it = m_NetListenCtxList.begin(); it != m_NetListenCtxList.end(); ++it)
		DeleteCtx(*it);
}

// NetListenCtxs ����Ʈ�� �߰�
void CNetListenCtxMan::AddNetListenCtx(CNetListenCtx* pCtx)
{
	m_NetListenCtxList.push_back(pCtx);
}

//////////////////////////////////////////////////////////
//			CTcpListenCtxMan
//////////////////////////////////////////////////////////
// [retVal] NULL: ����
CNetListenCtx* CTcpListenCtxMan::CreateCtx()
{
	CTcpListenCtx* pTcpCtx = new(nothrow) CTcpListenCtx();
	return pTcpCtx;
}

void CTcpListenCtxMan::DeleteCtx(CNetListenCtx* pCtx)
{
	_ASSERT(pCtx);

	CTcpListenCtx* pTcpCtx = (CTcpListenCtx*)pCtx;
	delete pTcpCtx;
}

//////////////////////////////////////////////////////////
//			CUdpListenCtxMan
//////////////////////////////////////////////////////////
// [retVal] NULL: ����
CNetListenCtx* CUdpListenCtxMan::CreateCtx()
{
	CUdpListenCtx* pUdpCtx = new(nothrow) CUdpListenCtx();
	return pUdpCtx;
}

void CUdpListenCtxMan::DeleteCtx(CNetListenCtx* pCtx)
{
	_ASSERT(pCtx);

	CUdpListenCtx* pUdpCtx = (CUdpListenCtx*)pCtx;
	delete pUdpCtx;
}

//////////////////////////////////////////////////////////
//			CNetClientCtxMan
//////////////////////////////////////////////////////////
CNetClientCtxMan::CNetClientCtxMan()
{
	InitializeCriticalSectionAndSpinCount(&m_Cs, 4000);
	m_nMaxFreeSize = DEFAULT_MAX_FREELIST_SIZE;
}

CNetClientCtxMan::~CNetClientCtxMan()
{
	DeleteCriticalSection(&m_Cs);
}

void CNetClientCtxMan::Cleanup()
{
	LST_PNETCLIENTCTX_IT it;
	for (it = m_FreeList.begin(); it != m_FreeList.end(); ++it)
		DeleteCtx((*it));

	for (it = m_UsedList.begin(); it != m_UsedList.end(); ++it)
		DeleteCtx((*it));
}

// ���ο� CNetClientCtx ��ü ����
// Free����Ʈ�� Ȯ���� �� ��������, ������ ���� ����
// [retVal] NULL: ����
CNetClientCtx* CNetClientCtxMan::AllocCtx()
{
	CNetClientCtx* retVal = NULL;
	LST_PNETCLIENTCTX_IT it;
	EnterCriticalSection(&m_Cs);
	
	it = m_FreeList.begin();
	if (it != m_FreeList.end())
	{
		// FreeList���� ��ȯ�� FreeList���� ����
		retVal = (*it);
		m_FreeList.pop_front();
	}
	else
	{
		// �޸𸮿��� ����
		retVal = CreateCtx();
	}

	// UsedList�� �߰�
	if (retVal)
		m_UsedList.push_front(retVal);

	LeaveCriticalSection(&m_Cs);
	return retVal;
}


// CNetClientCtx ��ü ����
// ������ ��ü�� FreeList�� �߰���
void CNetClientCtxMan::DeallocCtx(CNetClientCtx* pCtx)
{
	_ASSERT(pCtx);

	LST_PNETCLIENTCTX_IT it;
	EnterCriticalSection(&m_Cs);
	
	// UsedList���� ����
	it = find(m_UsedList.begin(), m_UsedList.end(), pCtx);
	if (it != m_UsedList.end())
	{
		m_UsedList.erase(it);
	
		if ((int)m_FreeList.size() < m_nMaxFreeSize)
		{
			// FreeList�� �߰�
			pCtx->Reset();
			m_FreeList.push_front(pCtx);
		}
		else
		{
			// FreeList�� ������ �޸𸮿��� ����
			DeleteCtx(pCtx);
		}
	}

	LeaveCriticalSection(&m_Cs);
}

//////////////////////////////////////////////////////////
//			CTcpClientCtxMan
//////////////////////////////////////////////////////////
// �޸𸮿��� ��ü ����
// [retVal] NULL: ����
CNetClientCtx* CTcpClientCtxMan::CreateCtx()
{
	CTcpClientCtx* pTcpCtx = new(nothrow) CTcpClientCtx();
	return pTcpCtx;
}

// �޸𸮿��� ��ü ����
void CTcpClientCtxMan::DeleteCtx(CNetClientCtx* pCtx)
{
	_ASSERT(pCtx);
	CTcpClientCtx* pTcpCtx = (CTcpClientCtx*)pCtx;
	delete pTcpCtx;
}

//////////////////////////////////////////////////////////
//			CUdpClientCtxMan
//////////////////////////////////////////////////////////
// �޸𸮿��� ��ü ����
// [retVal] NULL: ����
CNetClientCtx* CUdpClientCtxMan::CreateCtx()
{
	CUdpClientCtx* pUdpCtx = new(nothrow) CUdpClientCtx();
	return pUdpCtx;
}

// �޸𸮿��� ��ü ����
void CUdpClientCtxMan::DeleteCtx(CNetClientCtx* pCtx)
{
	_ASSERT(pCtx);
	CUdpClientCtx* pUdpCtx = (CUdpClientCtx*)pCtx;
	delete pUdpCtx;
}