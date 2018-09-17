////////////////////////////////////////////////////////////////////////////////////////////////////
//	RecvVFMBuffer.h
//
//	2010/5/31
//	작성자: dede
//	
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <VFMBuffer.h>
#include <network\tcpsrv\recvbuf.h>

////////////////////////////////////////////////////////////////////////////////////////////////////
//			CRecvVFMBuffer
//
//  - VFMBuffer를 이용한 CRecvBuf 구현 클래스
////////////////////////////////////////////////////////////////////////////////////////////////////
class CRecvVFMBuffer : public CRecvBuf
{
public:
	CRecvVFMBuffer();
	~CRecvVFMBuffer();

	// 내부 버퍼 내용을 초기화
	void Reset();
	
	// 데이터 추가
	// [retVal] BOOL: 성공 여부
	RET_CODE AddData(LPVOID pData, DWORD dwSize);
	
	// 추가된 데이터 사이즈 반환
	// [retVal] DWORD
	DWORD GetDataSize();

	// 내부 버퍼의 데이터 일부를 복사하여 반환
	// [in] pBuffer: 복사될 버퍼
	// [in] dwOffset: 내부 버퍼의 offset
	// [in] dwSize: 복사될 사이즈
	// [retVal] BOOL: 성공 여부
	BOOL GetData(LPVOID pBuffer, DWORD dwOffset, DWORD dwSize);

	// 내부 버퍼의 포인트를 반환
	// [retVal] LPVOID: 실패시엔 NULL 반환
	LPVOID GetDataPtr();
};