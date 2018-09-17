//						NetBuf.h
//			NetServer 에서 사용되는 버퍼 정의 추상 클래스
//
//		2010/06/xx
//		작성자: dede
//		작성자(dede)의 동의없이 해당 소스를 사용, 변경하는 것은 불법입니다.
//		
//		최종 수정일: 2010/06/xx
//
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <new>
using namespace std;

namespace Network
{
	const int DEFAULT_INIT_MEM_SIZE = 4 * 1024; 

	class CNetBuf;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	//				CNetBufMan
	//
	//	- 버퍼 관리하는 추상 클래스
	//
	class CNetBufMan
	{
	public:
		// [in] dwInitMemSize: CNetBuf 생성시에 최초 버퍼 크기
		CNetBufMan(DWORD dwInitMemSize = DEFAULT_INIT_MEM_SIZE) : m_dwInitMemSize(dwInitMemSize) {}
		virtual ~CNetBufMan() {}
		
		// 관리중인 모든 CNetBuf 들을 메모리에서 해제
		virtual void Cleanup() = 0;

		// [retVal] NULL: 실패
		virtual CNetBuf* AllocBuf() = 0;
		virtual void DeallocBuf(CNetBuf* pBuf) = 0;
	
	protected:
		DWORD m_dwInitMemSize;
	};

	////////////////////////////////////////////////////////////////////////////////////////////////////
	//				CNetBuf
	//
	//	- NetServer에서 사용되는 버퍼 추상 클래스
	//
	class CNetBuf
	{
	public:
		typedef enum _RET_CODE
		{
			E_SUCCESS,				// 성공

			E_INVALID_PARAM,		// 요청 파라미터가 올바르지 않음
			E_FAIL_ALLOC			// 버퍼 할당 실패
		} RET_CODE;

		// [in] dwInitMemSize: 최초 여유 메모리 크기
		CNetBuf(DWORD dwInitMemSize) {}
		virtual ~CNetBuf() {}
		
		// 내부 버퍼 해제
		virtual void Clear() = 0;
		
		// 데이터 추가
		// [in] pData: 추가할 데이터
		// [in] dwSize: pData 버퍼 크기
		// [retVal] E_SUCCESS 또는 E_FAIL_ALLOC 반환
		virtual RET_CODE AddData(LPVOID pData, DWORD dwSize) = 0;

		// 추가된 데이터 사이즈 반환
		// [retVal] 추가된 데이터 크기 반환
		virtual DWORD GetDataSize() = 0;

		// 내부 버퍼의 데이터 일부를 복사하여 반환
		// [in] pBuffer: 복사될 버퍼
		// [in] dwOffset: 내부 버퍼의 offset
		// [in] dwSize: 복사될 사이즈, -1: dwOffset 이후 데이터 전체 복사
		// [retVal] BOOL: 성공 여부
		virtual BOOL GetData(LPVOID pBuffer, DWORD dwOffset, DWORD dwSize) = 0;

		// 데이터 버퍼 반환
		// [retVal] LPVOID: 실패시엔 NULL 반환
		virtual LPVOID GetDataPtr() = 0;
		
		// 요청된 최소 여유 버퍼 반환
		// 필요한 버퍼를 미리 할당할 때도 사용됨
		// [in] dwMinMemSize: 여유 공간의 최소 크기 (4Kbyte 단위 권장)
		// [retVal] LPVOID: 메모리 할당 실패시엔 NULL 반환
		virtual LPVOID GetAvailMemPtr(DWORD dwMinMemSize) = 0;

		// 사용 가능한 여유 버퍼 크기
		virtual DWORD GetAvailMemSize() = 0;

		// 데이터의 유효 크기를 재설정(truncate)
		// [in] dwOffset: 내부 버프의 offset
		// [in] dwSize: 유효사이즈
		// [retVal] FAIL: 범위초과
		virtual BOOL ResetDataSize(DWORD dwOffset, DWORD dwSize) = 0;
	};



};