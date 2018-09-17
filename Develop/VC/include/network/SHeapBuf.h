//						SHeapBuf.h
//			new/delete �� �̿��� �ܼ�(Simple) Heap Buffer Manager
//
//		2010/06/xx
//		�ۼ���: dede
//		�ۼ���(dede)�� ���Ǿ��� �ش� �ҽ��� ���, �����ϴ� ���� �ҹ��Դϴ�.
//		
//		���� ������: 2010/06/xx
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
	//	- CSHeapBuf ���۸� �����ϴ� Ŭ����
	//
	class CSHeapBufMan : public CNetBufMan
	{
	public:
		
		// ���Ҵ� ���� ũ��
		#define DEFAULT_HEAP_ALLOC_RATIO	1.3

		// ��������Ʈ �ִ� ũ��
		#define DEFAULT_MAX_FREELIST_SIZE	19


		// [in] dwInitMemSize: CNetBuf �����ÿ� ���� ���� ũ��
		// [in] dwMaxFreeListSize: FreeList�� �ִ� ���� ũ��
		CSHeapBufMan(DWORD dwInitMemSize = DEFAULT_INIT_MEM_SIZE, DWORD dwMaxFreeListSize = DEFAULT_MAX_FREELIST_SIZE);
		virtual ~CSHeapBufMan(void);

		// �������� ��� CNetBuf ���� �޸𸮿��� ����
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
		// [in] dwInitMemSize: ���� ���� �޸� ũ��
		CSHeapBuf(DWORD dwInitMemSize, double dHeapAllocRatio);
		virtual ~CSHeapBuf();
		
		// ���� ���� ����
		void Clear();
		
		// ������ �߰�
		// [in] pData: �߰��� ������
		// [in] dwSize: pData ���� ũ��
		// [retVal] E_SUCCESS �Ǵ� E_FAIL_ALLOC ��ȯ
		RET_CODE AddData(LPVOID pData, DWORD dwSize);

		// �߰��� ������ ������ ��ȯ
		// [retVal] �߰��� ������ ũ�� ��ȯ
		DWORD GetDataSize();

		// ���� ������ ������ �Ϻθ� �����Ͽ� ��ȯ
		// [in] pBuffer: ����� ����
		// [in] dwOffset: ���� ������ offset
		// [in] dwSize: ����� ������, -1: dwOffset ���� ������ ��ü ����
		// [retVal] BOOL: ���� ����
		BOOL GetData(LPVOID pBuffer, DWORD dwOffset, DWORD dwSize);

		// ������ ���� ��ȯ
		// [retVal] LPVOID: ���нÿ� NULL ��ȯ
		LPVOID GetDataPtr();
		
		// ��û�� �ּ� ���� ���� ��ȯ
		// �ʿ��� ���۸� �̸� �Ҵ��� ���� ����
		// [in] dwMinMemSize: ���� ������ �ּ� ũ�� (4Kbyte ���� ����)
		// [retVal] LPVOID: �޸� �Ҵ� ���нÿ� NULL ��ȯ
		LPVOID GetAvailMemPtr(DWORD dwMinMemSize);

		// ��� ������ ���� ���� ũ��
		DWORD GetAvailMemSize();

		// �������� ��ȿ ũ�⸦ �缳��(truncate)
		// [in] dwOffset: ���� ������ offset
		// [in] dwSize: ��ȿ������
		// [retVal] FAIL: �����ʰ�
		BOOL ResetDataSize(DWORD dwOffset, DWORD dwSize);
	
	protected:

		// dwSize ��ŭ�� ������ �غ� ��Ŵ (�ʱ��Ҵ� Ȥ�� ���Ҵ��)
		// [in] dwSize: ��û ���� ������
		// [retVal] TRUE: �غ� ����, FALSE: �غ� ����
		BOOL ReadyBuf(DWORD dwSize);

	protected:
		const DWORD m_INITBUFSIZE;
		const double m_HEAP_ALLOC_RATIO;

		BYTE* m_pBuf;			// ����
		BYTE* m_pPos;			// ���� ���۳� Write ��ġ
		DWORD m_dwBufSize;		// �� ���� ũ��
		DWORD m_dwRemainSize;	// ���� ���� ũ��
	};

};