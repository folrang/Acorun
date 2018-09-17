#include "StdAfx.h"
#include <network/server/NetCtx.h>
#include <crtdbg.h> // _ASSERT
#include <algorithm>

using namespace NetServer;

/*
//////////////////////////////////////////////////////////
//			CNetCtxList
//////////////////////////////////////////////////////////
// 새 아이템을 헤더에 추가
// [in] pCtx: 새 아이템
// [retVal] 추가후 리스트
CNetCtx* CNetCtxList::AddHeader(CNetCtx* pCtxes, CNetCtx* pCtx)
{
	pCtx->m_pNext = NULL;

	if (!pCtxes)
	{
		pCtxes = pCtx;
	}
	else
	{
		// 헤더에 추가 - 순서는 상관없음
		pCtx->m_pNext = pCtxes;
		pCtxes = pCtx;
	}
	return pCtxes;
}

// 해당 아이템을 리스트에서 삭제
// [in] pCtx: 삭제할 아이템
// [retVal] 삭제후 리스트
CNetCtx* CNetCtxList::Remove(CNetCtx* pCtxes, CNetCtx* pCtx)
{
	CNetCtx *pPos = NULL, *pPrev = NULL;
		
	pPos = pCtxes;
	
	// 리스트에서 pCtx 찾기
	while (pPos && pPos != pCtx)
	{
		pPrev = pPos;
		pPos = pPos->m_pNext;
	}

	if (pPrev)
	{
		// 중간위치에서 pCtx 발견
		pPrev->m_pNext = pCtx->m_pNext;
	}
	else
	{
		// 맨 처음 위치에서 발견
		pCtxes = pCtx->m_pNext;
	}
	
	pCtx->m_pNext = NULL;

	return pCtxes;	
}

// 리스트의 헤더 아이템을 삭제 및 삭제된 헤더를 반환
// [out] pCtx: 삭제된 헤더
// [retVal] 삭제후 리스트
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

// 재사용이 가능하도록 멤버변수 초기화
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

// 재사용을 위해서 멤버 변수를 초기화
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

// Accept 대기 작업의 정보 추가
void CTcpListenCtx::AddPendingAccept(CNetIoData* pIoData)
{
	_ASSERT(pIoData);
	EnterCriticalSection(&m_Cs);

	m_PendingAcceptIoList.push_front(pIoData);
	
	LeaveCriticalSection(&m_Cs);
}

// PendingAccepts 리스트에서 제거
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

// CNetServer에서 종료전에 Alloc했던 CNetIoData들 해제하기 위해 사용
void CNetIoDataMan::Cleanup()
{
	LST_PNETIODATA_IT it;
	for (it = m_FreeList.begin(); it != m_FreeList.end(); ++it)
		DeleteData((*it));

	for (it = m_UsedList.begin(); it != m_UsedList.end(); ++it)
		DeleteData((*it));
}

// 새로운 CNetIoData 객체 생성
// Free리스트를 확인한 후 가져오고, 없으면 새로 생성
// [retVal] NULL: 실패
CNetIoData* CNetIoDataMan::AllocData()
{
	CNetIoData* retVal = NULL;
	LST_PNETIODATA_IT it;
	EnterCriticalSection(&m_Cs);
	
	it = m_FreeList.begin();
	if (it != m_FreeList.end())
	{
		// FreeList에서 반환후 FreeList에서 삭제
		retVal = (*it);
		m_FreeList.pop_front();
	}
	else
	{
		// 메모리에서 생성
		retVal = CreateData();
	}

	// UsedList에 추가
	if (retVal)
		m_UsedList.push_front(retVal);

	LeaveCriticalSection(&m_Cs);
	return retVal;
}

// CNetIoData 객체 해제
// 해제된 객체는 FreeList에 추가됨
void CNetIoDataMan::DeallocData(CNetIoData* pData)
{
	_ASSERT(pData);

	LST_PNETIODATA_IT it;
	EnterCriticalSection(&m_Cs);
	
	// UsedList에서 삭제
	it = find(m_UsedList.begin(), m_UsedList.end(), pData);
	if (it != m_UsedList.end())
	{
		m_UsedList.erase(it);
	
		if ((int)m_FreeList.size() < m_nMaxFreeSize)
		{
			// FreeList에 추가
			pData->Reset();
			m_FreeList.push_front(pData);
		}
		else
		{
			// FreeList가 꽉차서 메모리에서 삭제
			DeleteData(pData);
		}
	}

	LeaveCriticalSection(&m_Cs);
}

// 메모리에서 객체 생성
// [retVal] NULL: 실패
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
			// 버퍼 할당 성공
			break;
		}
		else
		{
			// 버퍼 할당 실패
			Sleep(5);
			i++;
		}
	}

	if (pIoData->m_pBuf == NULL)
	{
		// 재시도 이후에도 버퍼 할당 실패
		delete pIoData;
		pIoData = NULL;
	}

	return pIoData;
}

// 메모리에서 객체 삭제
void CNetIoDataMan::DeleteData(CNetIoData* pData)
{
	_ASSERT(pData);
	m_pNetBufMan->DeallocBuf(pData->m_pBuf);
	delete pData;
}

//////////////////////////////////////////////////////////
//			CNetListenCtxMan
//////////////////////////////////////////////////////////
// 추가될 NetListen 객체 최대 개수
void CNetListenCtxMan::SetMaxNetListenListSize(int nMaxSize)
{
	m_NetListenCtxList.reserve(nMaxSize);
}

// CNetServer에서 종료전에 Alloc했던 Ctx들 메모리에서 해제하기 위해 사용
void CNetListenCtxMan::Cleanup()
{
	for (VCT_PNETLISTENCTX_IT it = m_NetListenCtxList.begin(); it != m_NetListenCtxList.end(); ++it)
		DeleteCtx(*it);
}

// NetListenCtxs 리스트에 추가
void CNetListenCtxMan::AddNetListenCtx(CNetListenCtx* pCtx)
{
	m_NetListenCtxList.push_back(pCtx);
}

//////////////////////////////////////////////////////////
//			CTcpListenCtxMan
//////////////////////////////////////////////////////////
// [retVal] NULL: 실패
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
// [retVal] NULL: 실패
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

// 새로운 CNetClientCtx 객체 생성
// Free리스트를 확인한 후 가져오고, 없으면 새로 생성
// [retVal] NULL: 실패
CNetClientCtx* CNetClientCtxMan::AllocCtx()
{
	CNetClientCtx* retVal = NULL;
	LST_PNETCLIENTCTX_IT it;
	EnterCriticalSection(&m_Cs);
	
	it = m_FreeList.begin();
	if (it != m_FreeList.end())
	{
		// FreeList에서 반환후 FreeList에서 삭제
		retVal = (*it);
		m_FreeList.pop_front();
	}
	else
	{
		// 메모리에서 생성
		retVal = CreateCtx();
	}

	// UsedList에 추가
	if (retVal)
		m_UsedList.push_front(retVal);

	LeaveCriticalSection(&m_Cs);
	return retVal;
}


// CNetClientCtx 객체 해제
// 해제된 객체는 FreeList에 추가됨
void CNetClientCtxMan::DeallocCtx(CNetClientCtx* pCtx)
{
	_ASSERT(pCtx);

	LST_PNETCLIENTCTX_IT it;
	EnterCriticalSection(&m_Cs);
	
	// UsedList에서 삭제
	it = find(m_UsedList.begin(), m_UsedList.end(), pCtx);
	if (it != m_UsedList.end())
	{
		m_UsedList.erase(it);
	
		if ((int)m_FreeList.size() < m_nMaxFreeSize)
		{
			// FreeList에 추가
			pCtx->Reset();
			m_FreeList.push_front(pCtx);
		}
		else
		{
			// FreeList가 꽉차서 메모리에서 삭제
			DeleteCtx(pCtx);
		}
	}

	LeaveCriticalSection(&m_Cs);
}

//////////////////////////////////////////////////////////
//			CTcpClientCtxMan
//////////////////////////////////////////////////////////
// 메모리에서 객체 생성
// [retVal] NULL: 실패
CNetClientCtx* CTcpClientCtxMan::CreateCtx()
{
	CTcpClientCtx* pTcpCtx = new(nothrow) CTcpClientCtx();
	return pTcpCtx;
}

// 메모리에서 객체 삭제
void CTcpClientCtxMan::DeleteCtx(CNetClientCtx* pCtx)
{
	_ASSERT(pCtx);
	CTcpClientCtx* pTcpCtx = (CTcpClientCtx*)pCtx;
	delete pTcpCtx;
}

//////////////////////////////////////////////////////////
//			CUdpClientCtxMan
//////////////////////////////////////////////////////////
// 메모리에서 객체 생성
// [retVal] NULL: 실패
CNetClientCtx* CUdpClientCtxMan::CreateCtx()
{
	CUdpClientCtx* pUdpCtx = new(nothrow) CUdpClientCtx();
	return pUdpCtx;
}

// 메모리에서 객체 삭제
void CUdpClientCtxMan::DeleteCtx(CNetClientCtx* pCtx)
{
	_ASSERT(pCtx);
	CUdpClientCtx* pUdpCtx = (CUdpClientCtx*)pCtx;
	delete pUdpCtx;
}