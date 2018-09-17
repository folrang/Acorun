//						NetBuf.h
//			NetServer ���� ���Ǵ� ���� ���� �߻� Ŭ����
//
//		2010/06/xx
//		�ۼ���: dede
//		�ۼ���(dede)�� ���Ǿ��� �ش� �ҽ��� ���, �����ϴ� ���� �ҹ��Դϴ�.
//		
//		���� ������: 2010/06/xx
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
	//	- ���� �����ϴ� �߻� Ŭ����
	//
	class CNetBufMan
	{
	public:
		// [in] dwInitMemSize: CNetBuf �����ÿ� ���� ���� ũ��
		CNetBufMan(DWORD dwInitMemSize = DEFAULT_INIT_MEM_SIZE) : m_dwInitMemSize(dwInitMemSize) {}
		virtual ~CNetBufMan() {}
		
		// �������� ��� CNetBuf ���� �޸𸮿��� ����
		virtual void Cleanup() = 0;

		// [retVal] NULL: ����
		virtual CNetBuf* AllocBuf() = 0;
		virtual void DeallocBuf(CNetBuf* pBuf) = 0;
	
	protected:
		DWORD m_dwInitMemSize;
	};

	////////////////////////////////////////////////////////////////////////////////////////////////////
	//				CNetBuf
	//
	//	- NetServer���� ���Ǵ� ���� �߻� Ŭ����
	//
	class CNetBuf
	{
	public:
		typedef enum _RET_CODE
		{
			E_SUCCESS,				// ����

			E_INVALID_PARAM,		// ��û �Ķ���Ͱ� �ùٸ��� ����
			E_FAIL_ALLOC			// ���� �Ҵ� ����
		} RET_CODE;

		// [in] dwInitMemSize: ���� ���� �޸� ũ��
		CNetBuf(DWORD dwInitMemSize) {}
		virtual ~CNetBuf() {}
		
		// ���� ���� ����
		virtual void Clear() = 0;
		
		// ������ �߰�
		// [in] pData: �߰��� ������
		// [in] dwSize: pData ���� ũ��
		// [retVal] E_SUCCESS �Ǵ� E_FAIL_ALLOC ��ȯ
		virtual RET_CODE AddData(LPVOID pData, DWORD dwSize) = 0;

		// �߰��� ������ ������ ��ȯ
		// [retVal] �߰��� ������ ũ�� ��ȯ
		virtual DWORD GetDataSize() = 0;

		// ���� ������ ������ �Ϻθ� �����Ͽ� ��ȯ
		// [in] pBuffer: ����� ����
		// [in] dwOffset: ���� ������ offset
		// [in] dwSize: ����� ������, -1: dwOffset ���� ������ ��ü ����
		// [retVal] BOOL: ���� ����
		virtual BOOL GetData(LPVOID pBuffer, DWORD dwOffset, DWORD dwSize) = 0;

		// ������ ���� ��ȯ
		// [retVal] LPVOID: ���нÿ� NULL ��ȯ
		virtual LPVOID GetDataPtr() = 0;
		
		// ��û�� �ּ� ���� ���� ��ȯ
		// �ʿ��� ���۸� �̸� �Ҵ��� ���� ����
		// [in] dwMinMemSize: ���� ������ �ּ� ũ�� (4Kbyte ���� ����)
		// [retVal] LPVOID: �޸� �Ҵ� ���нÿ� NULL ��ȯ
		virtual LPVOID GetAvailMemPtr(DWORD dwMinMemSize) = 0;

		// ��� ������ ���� ���� ũ��
		virtual DWORD GetAvailMemSize() = 0;

		// �������� ��ȿ ũ�⸦ �缳��(truncate)
		// [in] dwOffset: ���� ������ offset
		// [in] dwSize: ��ȿ������
		// [retVal] FAIL: �����ʰ�
		virtual BOOL ResetDataSize(DWORD dwOffset, DWORD dwSize) = 0;
	};



};