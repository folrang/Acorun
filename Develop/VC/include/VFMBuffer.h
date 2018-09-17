//					CVFMBuffer
//				Virtual-Memory, File, MMF 를 배타적으로 사용하는 버퍼
//
//		2010/5/22
//		작성자 : dede
//		작성자(dede)의 동의없이 해당 소스를 사용, 변경하는 것은 불법입니다.
//
//									VM크기초과						MMF로 매핑 전환
//		이용 디바이스:	VM 버퍼 ----------------> 임시 파일(필수)  ----------------------> MMF 버퍼(옵션)
//
//		- 일정 크기 이하는 가상메모리(VirtualAlloc)에 기록, 일정 크기 이상은 임시 파일에 기록
//		- 가상메모리는 요청한 크기 만큼 전부 커밋되지 않고, 커밋한 만끔만 Page로 유지됨(VirtualAlloc 특성임)
//		- 임시 파일은 다시 MMF 버퍼로 수동 전환 가능
//
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#define MEGA	1048576

// 임시 파일명
//		%s - 임시폴더 경로
//		%d - 날짜
//		%d - 프로세스 아이디
//		%d - 고유 일련 번호
#define VFM_TMPFILE_NAME	_T("%s\\vfm_%d%d%d.tmp")


////////////////////////////////////////////////////////////////////////////////////////////////////
//					CVFMBuffer
//				
//	Ver 0.1	2010/5/22
//		- 최초 작성
//
class CVFMBuffer
{
public:
	// 초기화 옵션
	enum Flags
	{
		NONE, 
		
		// ThreadSafe 지원 여부
		USE_THREADSAFE = 0x01,

		// AddData() 호출시 넘겨준 데이터를 memmove를 이용하여 복사함
		USE_MEMMOVE = 0x02, 

		// Win32 Temporary 파일 사용
		//	Temporary 파일을 하면 디스크 대신 메모리를 사용하기 때문에, 성능에 지장을 줄 수 있음(trashing현상)
		USE_WIN32_TEMPORARY_FILE = 0x08
	};

	// 버퍼로 사용될 디바이스 타입
	typedef enum _DEVICE_TYPE
	{
		VIRUAL_MEMORY,		// 가상메모리
		TMP_FILE,			// 임시 파일
		MMF					// MMF
	} DEVICE_TYPE;

	// 생성자
	// [in] dwVMemorySize: 가상메모리 요청 크기, 요청한 크기와 실제 예약된 크기는 다를 수 있으며,
	//						실제로 예약된 크기는 GetVMemorySize() 함수를 이용하여 확인 가능
	// [in] szTmpFolderPath: 작업 파일을 저장할 폴더 경로
	//				NULL: 프로세스 폴더와 동일 위치
	// [in] dwFlags: 초기화 관련 플래그 옵션
	CVFMBuffer(DWORD dwVMemorySize = 1 * MEGA, LPCTSTR szTmpFolderPath = NULL, DWORD dwFlags = 0);
	~CVFMBuffer(void);

	// 데이터 추가
	// [in] pData: 추가할 버퍼 포인터
	// [in] dwDataSize: 버퍼 사이즈
	// [retVal] FALSE: 추가 실패
	BOOL AddData(LPVOID pData, DWORD dwDataSize);
	//BOOL AddCompleteData(LPVOID pData, DWORD dwDataSize);

	
	// 추가된 데이터 크기(Byte)를 반환
	DWORD GetDataSize() const;

	// 현재 사용중인 디바이스 타입을 반환
	//	- 디바이스 타입을 확인한 후, GetTmpFilePath() 혹은 GetBufferPtr() 사용 가능
	//	- AddData() 함수 이후에는 DeviceType이 변경될 수 있음에 주의
	DEVICE_TYPE GetDeviceType() const;

	// AddData() 함수 호출후 GetDeviceType() 반환값이 DEVICE_TYPE::TMP_FILE 일 경우에만, 
	//	임시 파일의 경로를 반환
	// [retVal] NULL: DEVICE_TYPE::TMP_FILE 이 아닌 경우
	LPCTSTR GetTmpFilePath();

	// AddData() 함수 호출후 GetDeviceType() 반환값이 
	//	DEVICE_TYPE::VIRUAL_MEMORY 혹은 DEVICE_TYPE::MMF 일 경우에만, 버퍼 포인트 반환
	// [in] bForceMmf: 만일 DEVICE_TYPE::TMP_FILE일 경우, MMF 로 변경후에 버퍼 포인트 반환 여부
	//		TRUE: 임시 파일일 경우 MMF 로 전환 후 버퍼 포인트 반환
	//		FALSE: 임시 파일일 경우 NULL 반환
	// [retVal] 버퍼 포인트
	//		NULL: 버퍼 존재 않음
	LPVOID GetBufferPtr(BOOL bForceMmf = FALSE);

	// 모든 Device 버퍼들을 초기화
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
	/// @brief	VM 버퍼 내용을 파일로 기록하고, VM 삭제함
	///
	/// @return	RET_WRITE_FILE
	////////////////////////////////////////////////////////////////////////////////////////////////////

	RET_WRITE_FILE DumpToFile();

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	RET_WRITE_MMF DumpToMmf()
	///
	/// @brief	파일 내용을 MMF 로 매핑 시킴 
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

	LPVOID m_pvBase;		///< VirtualAlloc 베이스 주소 
	HANDLE m_hFile;
};
