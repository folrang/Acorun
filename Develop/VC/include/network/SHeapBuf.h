//						SHeapBuf.h
//			new/delete 를 이용한 단순(Simple) Heap Buffer Manager
//
//		2010/06/xx
//		작성자: dede
//		작성자(dede)의 동의없이 해당 소스를 사용, 변경하는 것은 불법입니다.
//		
//		최종 수정일: 2010/06/xx
//
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <network/NetBuf.h>
#include <set>

using namespace std;
using namespace Network;

namespace Network
{

	class CSHeapBuf;

	///////////////////////////////////////////////////////////////////
	//				CSHeapBufMan
	//
	//	- CSHeapBuf 버퍼를 관리하는 클래스
	//
	class CSHeapBufMan : public CNetBufMan
	{
	public:
		
		// 재할당 버퍼 크기
		#define DEFAULT_HEAP_ALLOC_RATIO	1.3

		// 프리리스트 최대 크기
		#define DEFAULT_MAX_FREELIST_SIZE	19


		// [in] dwInitMemSize: CNetBuf 생성시에 최초 버퍼 크기
		// [in] dwMaxFreeListSize: FreeList의 최대 보유 크기
		CSHeapBufMan(DWORD dwInitMemSize = DEFAULT_INIT_MEM_SIZE, DWORD dwMaxFreeListSize = DEFAULT_MAX_FREELIST_SIZE);
		virtual ~CSHeapBufMan(void);

		// 관리중인 모든 CNetBuf 들을 메모리에서 해제
		void Cleanup();

		CNetBuf* AllocBuf();
		void DeallocBuf(CNetBuf* pBuf);

	protected:
		DWORD m_dwMaxFreeListSize;
		set<CSHeapBuf*> m_UsedList;
		set<CSHeapBuf*> m_FreeList;
		CRITICAL_SECTION m_Cs;
	};

	///////////////////////////////////////////////////////////////////
	//				CSHeapBuf
	//
	//	- Simple Heap Buffer
	//
	class CSHeapBuf : public CNetBuf
	{
	public:
		// [in] dwInitMemSize: 최초 여유 메모리 크기
		CSHeapBuf(DWORD dwInitMemSize, double dHeapAllocRatio);
		virtual ~CSHeapBuf();
		
		// 내부 버퍼 해제
		void Clear();
		
		// 데이터 추가
		// [in] pData: 추가할 데이터
		// [in] dwSize: pData 버퍼 크기
		// [retVal] E_SUCCESS 또는 E_FAIL_ALLOC 반환
		RET_CODE AddData(LPVOID pData, DWORD dwSize);

		// 추가된 데이터 사이즈 반환
		// [retVal] 추가된 데이터 크기 반환
		DWORD GetDataSize();

		// 내부 버퍼의 데이터 일부를 복사하여 반환
		// [in] pBuffer: 복사될 버퍼
		// [in] dwOffset: 내부 버퍼의 offset
		// [in] dwSize: 복사될 사이즈, -1: dwOffset 이후 데이터 전체 복사
		// [retVal] BOOL: 성공 여부
		BOOL GetData(LPVOID pBuffer, DWORD dwOffset, DWORD dwSize);

		// 데이터 버퍼 반환
		// [retVal] LPVOID: 실패시엔 NULL 반환
		LPVOID GetDataPtr();
		
		// 요청된 최소 여유 버퍼 반환
		// 필요한 버퍼를 미리 할당할 때도 사용됨
		// [in] dwMinMemSize: 여유 공간의 최소 크기 (4Kbyte 단위 권장)
		// [retVal] LPVOID: 메모리 할당 실패시엔 NULL 반환
		LPVOID GetAvailMemPtr(DWORD dwMinMemSize);

		// 사용 가능한 여유 버퍼 크기
		DWORD GetAvailMemSize();

		// 데이터의 유효 크기를 재설정(truncate)
		// [in] dwOffset: 내부 버프의 offset
		// [in] dwSize: 유효사이즈
		// [retVal] FAIL: 범위초과
		BOOL ResetDataSize(DWORD dwOffset, DWORD dwSize);
	
	protected:

		// dwSize 만큼의 공간을 준비 시킴 (초기할당 혹은 재할당등)
		// [in] dwSize: 요청 버퍼 사이즈
		// [retVal] TRUE: 준비 성공, FALSE: 준비 실패
		BOOL ReadyBuf(DWORD dwSize);

	protected:
		const DWORD m_INITBUFSIZE;
		const double m_HEAP_ALLOC_RATIO;

		BYTE* m_pBuf;			// 버퍼
		BYTE* m_pPos;			// 현재 버퍼내 Write 위치
		DWORD m_dwBufSize;		// 총 버퍼 크기
		DWORD m_dwRemainSize;	// 남은 버퍼 크기
	};

};