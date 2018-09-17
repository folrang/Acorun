#include "StdAfx.h"
#include "VFMBuffer.h"

#include <util.h>

LONG CUsizedBuffer::m_nUniNum = 0;

#define USB_ENTER_CS	(if(m_pCS) EnterCriticalSection(m_pCs))
#define USB_LEAVE_CS	(if(m_pCS) LeaveCriticalSection(m_pCs))

CVFMBuffer::CVFMBuffer(DWORD dwVMemorySize /*= 1 * MEGA*/, LPCTSTR szTmpFolderPath /*= NULL*/, DWORD dwFlags /*= 0*/)
{
	GetSystemInfo(&m_SysInfo);
	m_szTmpFolder[0] = '\0';
	m_szTmpFile[0] = '\0';
	m_dwDataSize = 0;
	m_dwFlags = dwFlags;
	m_pvBase = NULL;
	m_dwLastError = 0;
	m_pCs = NULL;
	m_hFile = NULL;

	if (szTmpFolderPath)
		_tcscpy_s(m_szTmpFolder, MAX_PATH, szTmpFolderPath);
	else
		Util::GetCurrentDirPath(m_szTmpFolder, MAX_PATH);
	
	m_dwAllocMemSize = ROUNDUP(dwVMemorySize, m_SysInfo.dwPageSize);
	m_pvBase = VirtualAlloc(NULL, m_dwAllocMemSize, MEM_RESERVE | MEM_TOP_DOWN, PAGE_EXECUTE_READWRITE);
	if (!m_pvBase)
		m_dwLastError = ::GetLastError(); 

	if ((dwFlags & USE_THREADSAFE) == USE_THREADSAFE)
	{
		m_pCs = new CRITICAL_SECTION;
		InitializeCriticalSectionAndSpinCount(m_pCs, 4000);
	}
}

CVFMBuffer::~CVFMBuffer(void)
{
	if (m_pvBase)
		VirtualFree(m_pvBase, 0, MEM_RELEASE);

	if (m_pCs)
	{
		DeleteCriticalSection(m_pCs);
		delete m_pCs;
	}
}

DWORD CVFMBuffer::GetVMemorySize() const
{
	DWORD retVal = 0;

	USB_ENTER_CS;
	retVal = m_dwAllocMemSize;
	USB_LEAVE_CS;

	return retVal;
}

DWORD CVFMBuffer::GetDataSize() const
{
	DWORD retVal = 0;

	USB_ENTER_CS;
	retVal = m_dwDataSize;
	USB_LEAVE_CS;

	return retVal;
}

BOOL CVFMBuffer::IsFileWrite() const
{
	BOOL retVal = FALSE;

	USB_LEAVE_CS;
	if (m_hFile) retVal = TRUE;
	USB_LEAVE_CS;
	
	return retVal;
}

// 데이터 추가
// [in] pData: 추가할 버퍼 포인터
// [in] dwDataSize: 버퍼 사이즈
// [retVal] FALSE: 추가 실패
BOOL CVFMBuffer::AddData(LPVOID pData, DWORD dwDataSize)
{
	BOOL retVal = FALSE;
	
	// 1. MMF 있으면 이용
	// 2. 파일 있으면 이용
	// 3. VM 이용
	//	3-1. 용량 충분하지 않으면 파일로 이용
	return retVal;
}

CVFMBuffer::RET_WRITE_FILE 
CVFMBuffer::AddDataToFile(LPVOID pData, DWORD dwDataSize)
{
}

CVFMBuffer::RET_WRITE_FILE 
CVFMBuffer::DumpToFile()
{
	if (m_hFile)
	{
		CloseHandle(m_hFile);
		m_hFile = NULL;
	}
	
	SYSTEMTIME st;
	GetLocalTime(&st);
	InterlockedIncrement(&m_nUniNum);
	wsprintf(m_szTmpFile, USB_FILE_NAME, m_szTmpFolder, st.wDay, GetProcessId(), m_nUniNum);

	DWORD dwShareMode = FILE_SHARE_READ;	///< 파일읽기 허용
	DWORD dwFlagsAndAttributes;
	m_hFile = CreateFile(m_szTmpFile, GENERIC_READ | GENERIC_WRITE, dwShareMode, NULL, CREATE_NEW, 
}