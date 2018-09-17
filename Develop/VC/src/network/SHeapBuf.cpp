#include "StdAfx.h"
#include <network/SHeapBuf.h>
#include <new>

using namespace Network;

///////////////////////////////////////////////////////////////////
//				CSHeapBufMan
///////////////////////////////////////////////////////////////////
// [in] dwInitMemSize: CNetBuf �����ÿ� ���� ���� ũ��
// [in] dwMaxFreeListSize: FreeList�� �ִ� ���� ũ��
CSHeapBufMan::CSHeapBufMan(DWORD dwInitMemSize /*= DEFAULT_INIT_MEM_SIZE*/, DWORD dwMaxFreeListSize /*= DEFAULT_MAX_FREELIST_SIZE*/) : CNetBufMan(dwInitMemSize)
{
	m_dwMaxFreeListSize = dwMaxFreeListSize;

	InitializeCriticalSectionAndSpinCount(&m_Cs, 4000);
}

CSHeapBufMan::~CSHeapBufMan(void)
{
	DeleteCriticalSection(&m_Cs);
}

// �������� ��� CNetBuf ���� �޸𸮿��� ����
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
// [in] dwInitMemSize: ���� ���� �޸� ũ��
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

// ���� ���� ����
void CSHeapBuf::Clear()
{
	if (m_pBuf)
	{
		delete [] m_pBuf;
		m_pBuf = m_pPos = NULL;
		m_dwRemainSize = m_dwBufSize = 0;
	}
}

// ������ �߰�
// [in] pData: �߰��� ������
// [in] dwSize: pData ���� ũ��
// [retVal] E_SUCCESS �Ǵ� E_FAIL_ALLOC ��ȯ
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

// �߰��� ������ ������ ��ȯ
// [retVal] �߰��� ������ ũ�� ��ȯ
DWORD CSHeapBuf::GetDataSize()
{
	return (m_dwBufSize - m_dwRemainSize);
}

// ���� ������ ������ �Ϻθ� �����Ͽ� ��ȯ
// [in] pBuffer: ����� ����
// [in] dwOffset: ���� ������ offset
// [in] dwSize: ����� ������, -1: dwOffset ���� ������ ��ü ����
// [retVal] BOOL: ���� ����
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

// ������ ���� ��ȯ
// [retVal] LPVOID: ���нÿ� NULL ��ȯ
LPVOID CSHeapBuf::GetDataPtr()
{
	return m_pBuf;
}

// ��û�� �ּ� ���� ���� ��ȯ
// �ʿ��� ���۸� �̸� �Ҵ��� ���� ����
// [in] dwMinMemSize: ���� ������ �ּ� ũ�� (4Kbyte ���� ����)
// [retVal] LPVOID: �޸� �Ҵ� ���нÿ� NULL ��ȯ
LPVOID CSHeapBuf::GetAvailMemPtr(DWORD dwMinMemSize)
{
	LPVOID retVal = NULL;
	if (ReadyBuf(dwMinMemSize))
		retVal = m_pPos;

	return retVal;
}

// ��� ������ ���� ���� ũ��
DWORD CSHeapBuf::GetAvailMemSize()
{
	return m_dwRemainSize;
}

// �������� ��ȿ ũ�⸦ �缳��(truncate)
// [in] dwOffset: ���� ������ offset
// [in] dwSize: ��ȿ������
// [retVal] FAIL: �����ʰ�
BOOL CSHeapBuf::ResetDataSize(DWORD dwOffset, DWORD dwSize)
{
	DWORD dwNewDataSize = dwOffset + dwSize;
	if (dwNewDataSize > m_dwBufSize)
		return FALSE;
	
	m_pPos = m_pBuf + dwNewDataSize;
	m_dwRemainSize = m_dwBufSize - dwNewDataSize;
	return TRUE;
}


// dwSize ��ŭ�� ������ �غ� ��Ŵ (�ʱ��Ҵ� Ȥ�� ���Ҵ��)
// [in] dwSize: ��û ���� ������
// [retVal] TRUE: �غ� ����, FALSE: �غ� ����
BOOL CSHeapBuf::ReadyBuf(DWORD dwSize)
{
	BOOL retVal = FALSE;
	if (dwSize > m_dwRemainSize)
	{
		DWORD dwNewBufSize = 0, dwUsedBufSize = 0;

		if (m_pBuf)
		{
			// ���Ҵ�
			dwUsedBufSize = m_dwBufSize - m_dwRemainSize;
			dwNewBufSize = (DWORD)((dwUsedBufSize + dwSize) * m_HEAP_ALLOC_RATIO);	// ������ ũ�� ����
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
			// ���� �Ҵ�
			dwNewBufSize = m_INITBUFSIZE;
			if (dwSize > dwNewBufSize)
			{
				// �䱸����� �� Ŭ���, �ʱ� ���۸� �� ũ�� ����
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