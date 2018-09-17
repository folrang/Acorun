////////////////////////////////////////////////////////////////////////////////////////////////////
//	RecvBuf.h
//
//	2010/5/
//	�ۼ���: Dede
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

	// ���� ���� ������ �ʱ�ȭ
	virtual void Reset() = 0;
	
	// ������ �߰�
	// [retVal] BOOL: ���� ����
	virtual RET_CODE AddData(LPVOID pData, DWORD dwSize) = 0;
	
	// �߰��� ������ ������ ��ȯ
	// [retVal] DWORD
	virtual DWORD GetDataSize() = 0;

	// ���� ������ ������ �Ϻθ� �����Ͽ� ��ȯ
	// [in] pBuffer: ����� ����
	// [in] dwOffset: ���� ������ offset
	// [in] dwSize: ����� ������
	// [retVal] BOOL: ���� ����
	virtual BOOL GetData(LPVOID pBuffer, DWORD dwOffset, DWORD dwSize) = 0;

	// ���� ������ ����Ʈ�� ��ȯ
	// [retVal] LPVOID: ���нÿ� NULL ��ȯ
	virtual LPVOID GetDataPtr() = 0;
};