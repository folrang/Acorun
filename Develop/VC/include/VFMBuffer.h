//					CVFMBuffer
//				Virtual-Memory, File, MMF �� ��Ÿ������ ����ϴ� ����
//
//		2010/5/22
//		�ۼ��� : dede
//		�ۼ���(dede)�� ���Ǿ��� �ش� �ҽ��� ���, �����ϴ� ���� �ҹ��Դϴ�.
//
//									VMũ���ʰ�						MMF�� ���� ��ȯ
//		�̿� ����̽�:	VM ���� ----------------> �ӽ� ����(�ʼ�)  ----------------------> MMF ����(�ɼ�)
//
//		- ���� ũ�� ���ϴ� ����޸�(VirtualAlloc)�� ���, ���� ũ�� �̻��� �ӽ� ���Ͽ� ���
//		- ����޸𸮴� ��û�� ũ�� ��ŭ ���� Ŀ�Ե��� �ʰ�, Ŀ���� ������ Page�� ������(VirtualAlloc Ư����)
//		- �ӽ� ������ �ٽ� MMF ���۷� ���� ��ȯ ����
//
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#define MEGA	1048576

// �ӽ� ���ϸ�
//		%s - �ӽ����� ���
//		%d - ��¥
//		%d - ���μ��� ���̵�
//		%d - ���� �Ϸ� ��ȣ
#define VFM_TMPFILE_NAME	_T("%s\\vfm_%d%d%d.tmp")


////////////////////////////////////////////////////////////////////////////////////////////////////
//					CVFMBuffer
//				
//	Ver 0.1	2010/5/22
//		- ���� �ۼ�
//
class CVFMBuffer
{
public:
	// �ʱ�ȭ �ɼ�
	enum Flags
	{
		NONE, 
		
		// ThreadSafe ���� ����
		USE_THREADSAFE = 0x01,

		// AddData() ȣ��� �Ѱ��� �����͸� memmove�� �̿��Ͽ� ������
		USE_MEMMOVE = 0x02, 

		// Win32 Temporary ���� ���
		//	Temporary ������ �ϸ� ��ũ ��� �޸𸮸� ����ϱ� ������, ���ɿ� ������ �� �� ����(trashing����)
		USE_WIN32_TEMPORARY_FILE = 0x08
	};

	// ���۷� ���� ����̽� Ÿ��
	typedef enum _DEVICE_TYPE
	{
		VIRUAL_MEMORY,		// ����޸�
		TMP_FILE,			// �ӽ� ����
		MMF					// MMF
	} DEVICE_TYPE;

	// ������
	// [in] dwVMemorySize: ����޸� ��û ũ��, ��û�� ũ��� ���� ����� ũ��� �ٸ� �� ������,
	//						������ ����� ũ��� GetVMemorySize() �Լ��� �̿��Ͽ� Ȯ�� ����
	// [in] szTmpFolderPath: �۾� ������ ������ ���� ���
	//				NULL: ���μ��� ������ ���� ��ġ
	// [in] dwFlags: �ʱ�ȭ ���� �÷��� �ɼ�
	CVFMBuffer(DWORD dwVMemorySize = 1 * MEGA, LPCTSTR szTmpFolderPath = NULL, DWORD dwFlags = 0);
	~CVFMBuffer(void);

	// ������ �߰�
	// [in] pData: �߰��� ���� ������
	// [in] dwDataSize: ���� ������
	// [retVal] FALSE: �߰� ����
	BOOL AddData(LPVOID pData, DWORD dwDataSize);
	//BOOL AddCompleteData(LPVOID pData, DWORD dwDataSize);

	
	// �߰��� ������ ũ��(Byte)�� ��ȯ
	DWORD GetDataSize() const;

	// ���� ������� ����̽� Ÿ���� ��ȯ
	//	- ����̽� Ÿ���� Ȯ���� ��, GetTmpFilePath() Ȥ�� GetBufferPtr() ��� ����
	//	- AddData() �Լ� ���Ŀ��� DeviceType�� ����� �� ������ ����
	DEVICE_TYPE GetDeviceType() const;

	// AddData() �Լ� ȣ���� GetDeviceType() ��ȯ���� DEVICE_TYPE::TMP_FILE �� ��쿡��, 
	//	�ӽ� ������ ��θ� ��ȯ
	// [retVal] NULL: DEVICE_TYPE::TMP_FILE �� �ƴ� ���
	LPCTSTR GetTmpFilePath();

	// AddData() �Լ� ȣ���� GetDeviceType() ��ȯ���� 
	//	DEVICE_TYPE::VIRUAL_MEMORY Ȥ�� DEVICE_TYPE::MMF �� ��쿡��, ���� ����Ʈ ��ȯ
	// [in] bForceMmf: ���� DEVICE_TYPE::TMP_FILE�� ���, MMF �� �����Ŀ� ���� ����Ʈ ��ȯ ����
	//		TRUE: �ӽ� ������ ��� MMF �� ��ȯ �� ���� ����Ʈ ��ȯ
	//		FALSE: �ӽ� ������ ��� NULL ��ȯ
	// [retVal] ���� ����Ʈ
	//		NULL: ���� ���� ����
	LPVOID GetBufferPtr(BOOL bForceMmf = FALSE);

	// ��� Device ���۵��� �ʱ�ȭ
	void Reset();


protected:
	typedef enum _RET_WRITE_VM
	{
	} RET_WRITE_VM;
	
	typedef enum _RET_WRITE_FILE
	{
	} RET_WRITE_FILE;

	typedef enum _RET_WRITE_MMF
	{
	} RET_WRITE_MMF;


	RET_WRITE_VM AddDataToVM(LPVOID pData, DWORD dwDataSize);
	RET_WRITE_FILE AddDataToFile(LPVOID pData, DWORD dwDataSize);

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	RET_WRITE_FILE DumpToFile()
	///
	/// @brief	VM ���� ������ ���Ϸ� ����ϰ�, VM ������
	///
	/// @return	RET_WRITE_FILE
	////////////////////////////////////////////////////////////////////////////////////////////////////

	RET_WRITE_FILE DumpToFile();

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	RET_WRITE_MMF DumpToMmf()
	///
	/// @brief	���� ������ MMF �� ���� ��Ŵ 
	///
	/// @return	RET_WRITE_MMF
	////////////////////////////////////////////////////////////////////////////////////////////////////

	RET_WRITE_MMF DumpToMmf();


private:
	static LONG m_nUniNum;
	
	CRITICAL_SECIONT* m_pCs;

	SYSTEM_INFO m_SysInfo;
	TCHAR m_szTmpFolder[MAX_PATH];
	TCHAR m_szTmpFile[MAX_PATH];
	DWORD m_dwFlags;
	DWORD m_dwAllocMemSize;
	DWORD m_dwDataSize;
	DWORD m_dwLastError;

	LPVOID m_pvBase;		///< VirtualAlloc ���̽� �ּ� 
	HANDLE m_hFile;
};
