////////////////////////////////////////////////////////////////////////////////////////////////////
//	RecvBuf.h
//
//	2010/5/
//	작성자: Dede
//	
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////
//			CRecvBuf
//
//  - 
////////////////////////////////////////////////////////////////////////////////////////////////////
class CRecvBuf
{
public:
	typedef enum _RET_CODE
	{
		E_SUCCESS,

		E_INVALID_PARAM,
		E_NOT_ENOUGH_BUFFER
	} RET_CODE;

	// 내부 버퍼 내용을 초기화
	virtual void Reset() = 0;
	
	// 데이터 추가
	// [retVal] BOOL: 성공 여부
	virtual RET_CODE AddData(LPVOID pData, DWORD dwSize) = 0;
	
	// 추가된 데이터 사이즈 반환
	// [retVal] DWORD
	virtual DWORD GetDataSize() = 0;

	// 내부 버퍼의 데이터 일부를 복사하여 반환
	// [in] pBuffer: 복사될 버퍼
	// [in] dwOffset: 내부 버퍼의 offset
	// [in] dwSize: 복사될 사이즈
	// [retVal] BOOL: 성공 여부
	virtual BOOL GetData(LPVOID pBuffer, DWORD dwOffset, DWORD dwSize) = 0;

	// 내부 버퍼의 포인트를 반환
	// [retVal] LPVOID: 실패시엔 NULL 반환
	virtual LPVOID GetDataPtr() = 0;
};