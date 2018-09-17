#include "StdAfx.h"
#include <network/SHeapBuf.h>
#include <new>

using namespace Network;

///////////////////////////////////////////////////////////////////
//				CSHeapBufMan
///////////////////////////////////////////////////////////////////
// [in] dwInitMemSize: CNetBuf 생성시에 최초 버퍼 크기
// [in] dwMaxFreeListSize: FreeList의 최대 보유 크기
CSHeapBufMan::CSHeapBufMan(DWORD dwInitMemSize /*= DEFAULT_INIT_MEM_SIZE*/, DWORD dwMaxFreeListSize /*= DEFAULT_MAX_FREELIST_SIZE*/) : CNetBufMan(dwInitMemSize)
{
	m_dwMaxFreeListSize = dwMaxFreeListSize;

	InitializeCriticalSectionAndSpinCount(&m_Cs, 4000);
}

CSHeapBufMan::~CSHeapBufMan(void)
{
	DeleteCriticalSection(&m_Cs);
}

// 관리중인 모든 CNetBuf 들을 메모리에서 해제
void CSHeapBufMan::Cleanup()
{
	set<CSHeapBuf*>::iterator it;
	EnterCriticalSection(&m_Cs);
	for (it = m_UsedList.begin(); it != m_UsedList.end(); )
	{
		delete *it;
		m_UsedList.erase(it++);
	}
	
	for (it = m_FreeList.begin(); it != m_FreeList.end(); )
	{
		delete *it;
		m_FreeList.erase(it++);
	}
	LeaveCriticalSection(&m_Cs);
}

CNetBuf* CSHeapBufMan::AllocBuf()
{
	CSHeapBuf* pSHeapBuf = NULL;
	set<CSHeapBuf*>::iterator it;

	EnterCriticalSection(&m_Cs);
	
	it = m_FreeList.begin();
	if (it != m_FreeList.end())
	{
		pSHeapBuf = *it;
		m_FreeList.erase(it);
		m_UsedList.insert(pSHeapBuf);
	}
	else
	{
		pSHeapBuf = new(nothrow) CSHeapBuf(m_dwInitMemSize, DEFAULT_HEAP_ALLOC_RATIO);
		if (pSHeapBuf)
			m_UsedList.insert(pSHeapBuf);
	}

	LeaveCriticalSection(&m_Cs);
	return (CNetBuf*)pSHeapBuf;
}

void CSHeapBufMan::DeallocBuf(CNetBuf* pBuf)
{
	CSHeapBuf* pSHeapBuf = dynamic_cast<CSHeapBuf*>(pBuf);
	set<CSHeapBuf*>::iterator it;
	BOOL bDeleteHeap = TRUE;

	EnterCriticalSection(&m_Cs);
	if (m_FreeList.size() < m_dwMaxFreeListSize)
	{
		m_FreeList.insert(pSHeapBuf);
		bDeleteHeap = FALSE;
	}
	m_UsedList.erase(pSHeapBuf);
	LeaveCriticalSection(&m_Cs);

	if (bDeleteHeap)
		delete pSHeapBuf;
}

///////////////////////////////////////////////////////////////////
//				CSHeapBuf
///////////////////////////////////////////////////////////////////
// [in] dwInitMemSize: 최초 여유 메모리 크기
CSHeapBuf::CSHeapBuf(DWORD dwInitMemSize, double dHeapAllocRatio) : CNetBuf(dwInitMemSize), m_INITBUFSIZE(dwInitMemSize), m_HEAP_ALLOC_RATIO(dHeapAllocRatio)
{
	m_pBuf = m_pPos = NULL;
	m_dwRemainSize = m_dwBufSize = 0;
}

CSHeapBuf::~CSHeapBuf()
{
	if (m_pBuf)
		delete [] m_pBuf;
}

// 내부 버퍼 해제
void CSHeapBuf::Clear()
{
	if (m_pBuf)
	{
		delete [] m_pBuf;
		m_pBuf = m_pPos = NULL;
		m_dwRemainSize = m_dwBufSize = 0;
	}
}

// 데이터 추가
// [in] pData: 추가할 데이터
// [in] dwSize: pData 버퍼 크기
// [retVal] E_SUCCESS 또는 E_FAIL_ALLOC 반환
CNetBuf::RET_CODE 
CSHeapBuf::AddData(LPVOID pData, DWORD dwSize)
{
	CNetBuf::RET_CODE retVal = CNetBuf::E_FAIL_ALLOC;
	
	if (ReadyBuf(dwSize))
	{
		memcpy(m_pPos, pData, dwSize);
		m_pPos += dwSize;
		m_dwRemainSize -= dwSize;

		retVal = CNetBuf::E_SUCCESS;
	}
	return retVal;
}

// 추가된 데이터 사이즈 반환
// [retVal] 추가된 데이터 크기 반환
DWORD CSHeapBuf::GetDataSize()
{
	return (m_dwBufSize - m_dwRemainSize);
}

// 내부 버퍼의 데이터 일부를 복사하여 반환
// [in] pBuffer: 복사될 버퍼
// [in] dwOffset: 내부 버퍼의 offset
// [in] dwSize: 복사될 사이즈, -1: dwOffset 이후 데이터 전체 복사
// [retVal] BOOL: 성공 여부
BOOL CSHeapBuf::GetData(LPVOID pBuffer, DWORD dwOffset, DWORD dwSize)
{
	DWORD dwUsedSize = m_dwBufSize - m_dwRemainSize;
	if (pBuffer == NULL || m_pBuf == NULL || m_dwBufSize == 0 || 
		dwOffset + dwSize > dwUsedSize
		)
		return FALSE;

	if (dwSize == -1)
		dwSize = dwUsedSize - dwOffset;

	memcpy(pBuffer, (LPVOID)(m_pBuf + dwOffset), dwSize);

	return TRUE;
}

// 데이터 버퍼 반환
// [retVal] LPVOID: 실패시엔 NULL 반환
LPVOID CSHeapBuf::GetDataPtr()
{
	return m_pBuf;
}

// 요청된 최소 여유 버퍼 반환
// 필요한 버퍼를 미리 할당할 때도 사용됨
// [in] dwMinMemSize: 여유 공간의 최소 크기 (4Kbyte 단위 권장)
// [retVal] LPVOID: 메모리 할당 실패시엔 NULL 반환
LPVOID CSHeapBuf::GetAvailMemPtr(DWORD dwMinMemSize)
{
	LPVOID retVal = NULL;
	if (ReadyBuf(dwMinMemSize))
		retVal = m_pPos;

	return retVal;
}

// 사용 가능한 여유 버퍼 크기
DWORD CSHeapBuf::GetAvailMemSize()
{
	return m_dwRemainSize;
}

// 데이터의 유효 크기를 재설정(truncate)
// [in] dwOffset: 내부 버프의 offset
// [in] dwSize: 유효사이즈
// [retVal] FAIL: 범위초과
BOOL CSHeapBuf::ResetDataSize(DWORD dwOffset, DWORD dwSize)
{
	DWORD dwNewDataSize = dwOffset + dwSize;
	if (dwNewDataSize > m_dwBufSize)
		return FALSE;
	
	m_pPos = m_pBuf + dwNewDataSize;
	m_dwRemainSize = m_dwBufSize - dwNewDataSize;
	return TRUE;
}


// dwSize 만큼의 공간을 준비 시킴 (초기할당 혹은 재할당등)
// [in] dwSize: 요청 버퍼 사이즈
// [retVal] TRUE: 준비 성공, FALSE: 준비 실패
BOOL CSHeapBuf::ReadyBuf(DWORD dwSize)
{
	BOOL retVal = FALSE;
	if (dwSize > m_dwRemainSize)
	{
		DWORD dwNewBufSize = 0, dwUsedBufSize = 0;

		if (m_pBuf)
		{
			// 재할당
			dwUsedBufSize = m_dwBufSize - m_dwRemainSize;
			dwNewBufSize = (DWORD)((dwUsedBufSize + dwSize) * m_HEAP_ALLOC_RATIO);	// 새버퍼 크기 증가
			BYTE* pTempBuf = new(nothrow) BYTE[dwNewBufSize];
			if (pTempBuf)
			{
				// swap
				memcpy(pTempBuf, m_pBuf, dwUsedBufSize);
				delete [] m_pBuf;
				m_pBuf = pTempBuf;
				m_pPos = m_pBuf + dwUsedBufSize;

				m_dwBufSize = dwNewBufSize;
				m_dwRemainSize = m_dwBufSize - dwUsedBufSize;
				retVal = TRUE;
			}
		}
		else
		{
			// 최초 할당
			dwNewBufSize = m_INITBUFSIZE;
			if (dwSize > dwNewBufSize)
			{
				// 요구사이즈가 더 클경우, 초기 버퍼를 더 크게 설정
				dwNewBufSize = (DWORD)(dwSize * m_HEAP_ALLOC_RATIO);
			}

			m_pBuf = new(nothrow) BYTE[dwNewBufSize];
			if (m_pBuf)
			{
				m_pPos = m_pBuf;
				m_dwRemainSize = m_dwBufSize = dwNewBufSize;
				retVal = TRUE;
			}
		}
	}
	else
		retVal = TRUE;

	return retVal;
}