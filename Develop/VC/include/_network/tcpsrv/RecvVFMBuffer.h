////////////////////////////////////////////////////////////////////////////////////////////////////
//	RecvVFMBuffer.h
//
//	2010/5/31
//	�ۼ���: dede
//	
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <VFMBuffer.h>
#include <network\tcpsrv\recvbuf.h>

////////////////////////////////////////////////////////////////////////////////////////////////////
//			CRecvVFMBuffer
//
//  - VFMBuffer�� �̿��� CRecvBuf ���� Ŭ����
////////////////////////////////////////////////////////////////////////////////////////////////////
class CRecvVFMBuffer : public CRecvBuf
{
public:
	CRecvVFMBuffer();
	~CRecvVFMBuffer();

	// ���� ���� ������ �ʱ�ȭ
	void Reset();
	
	// ������ �߰�
	// [retVal] BOOL: ���� ����
	RET_CODE AddData(LPVOID pData, DWORD dwSize);
	
	// �߰��� ������ ������ ��ȯ
	// [retVal] DWORD
	DWORD GetDataSize();

	// ���� ������ ������ �Ϻθ� �����Ͽ� ��ȯ
	// [in] pBuffer: ����� ����
	// [in] dwOffset: ���� ������ offset
	// [in] dwSize: ����� ������
	// [retVal] BOOL: ���� ����
	BOOL GetData(LPVOID pBuffer, DWORD dwOffset, DWORD dwSize);

	// ���� ������ ����Ʈ�� ��ȯ
	// [retVal] LPVOID: ���нÿ� NULL ��ȯ
	LPVOID GetDataPtr();
};