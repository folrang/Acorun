//								GMMF.h
//						- Function prototypes for using growable memory-mapped files
//
//		2010 / 5 / 20
//		�ۼ���: dede (��������: Jeffrey Richter)
//				http://www.microsoft.com/msj/0499/win32/win320499.aspx
//				CSparseStream Ŭ������ ����, ������å�� MMF é�� ����
//
//		- �ʱ⿡ MMF�� ũ�� ���, �ʿ��� �κ��� ���, ���ʿ��� �κ��� Zero�� �����Ͽ� ������ ũ�⸦ ���ҽ�Ű�� ��� ����
//
//		���� ����
//		- Windows 98���� �� NT4.0 ������ ���Ұ�, Windows 2000(Windows NT 5, _WIN32_WINNT 0x0500) ���� ����
//			Spare file �� Windows 2000 ���� ����
//////////////////////////////////////////////////////////////////////

#pragma once

class CGMMF 
{

	HANDLE m_hfilemap;      // File-mapping object
	PVOID  m_pvFile;        // Address to start of mapped file

public:
	// Creates a GMMF and maps it in the process's address space.
	// [in] hFile : ���� �ڵ�
	// [in] cbFileSizeMax : ����Ǵ� MMF �ִ� ũ��
	CGMMF(HANDLE hFile, UINT_PTR cbFileSizeMax);

	// Closes a GMMF
	~CGMMF() { ForceClose(); }

	// GMMF to BYTE cast operator returns address of first byte 
	// in the memory-mapped sparse file. 
	operator PBYTE() const { return((PBYTE) m_pvFile); }

	// Allows you to explicitly close the GMMF without having
	// to wait for the destructor to be called.
	// MMF ���� ����
	VOID ForceClose();

public:
	// Static method that resets a portion of a file back to
	// zeroes (frees disk clusters)
	// ���� ��ġ ���� ���͸� Zero�� �ʱ�ȭ ��Ŵ (������ �ƴ� ���� ������ ���ϰ����� ���ҽ�Ŵ)
	// [in] hfile : MMF ������ ������ ���� �ڵ�
	static BOOL SetToZero(HANDLE hfile, 
			UINT_PTR cbOffsetStart, UINT_PTR cbOffsetEnd);

	// ���� ������ ũ�⸦ ����
	// [in] hfile : MMF ������ ������ ���� �ڵ�
	// [in] liDistanceToMove : �������̸� dwMoveDirection �������� ������ �̵�
	// [in] dwMoveMothod : �̵� ���� (FILE_BEGIN, FILE_CURRENT, FILE_END)
	//			FILE_BEGIN : ���� ó������ �̵�
	//			FILE_CURRENT : ���� ���� �����ͺ��� ���
	//			FILE_END : ���� ���� ũ�⿡�� liDistanceToMove ��ƴ �� �̵�
	static BOOL SetToEndOfFile(HANDLE hfile,
			LARGE_INTEGER liDistanceToMove, DWORD dwMoveMothod = FILE_BEGIN);
};



///////////////////////////////////////////////////////////////////////////////

CGMMF::CGMMF(HANDLE hfile, UINT_PTR cbFileSizeMax) 
{
	// Initialize to NULL in case something goes wrong
    m_hfilemap = m_pvFile = NULL;

    // Make the file sparse
    DWORD dw;
    BOOL fOk = ::DeviceIoControl(hfile, FSCTL_SET_SPARSE, 
	    NULL, 0, NULL, 0, &dw, NULL);

    if (fOk) {
        // Create a file-mapping object
	    m_hfilemap = ::CreateFileMapping(hfile, NULL, 
		    PAGE_READWRITE, 0, cbFileSizeMax, NULL);

        if (m_hfilemap != NULL) {
            // Map the file into the process's address space
            m_pvFile = ::MapViewOfFile(m_hfilemap, 
                FILE_MAP_WRITE | FILE_MAP_READ, 0, 0, 0);
        } else {
            // Failed to map the file, cleanup
            ForceClose();
        }
    }
}

inline VOID CGMMF::ForceClose() 
{
    // Cleanup everything that was done sucessfully
    if (m_pvFile   != NULL) { 
        ::UnmapViewOfFile(m_pvFile); 
        m_pvFile = NULL; 
    }
    if (m_hfilemap != NULL) { 
        ::CloseHandle(m_hfilemap);   
        m_hfilemap = NULL; 
    }
}

inline BOOL CGMMF::SetToZero(HANDLE hfile, UINT_PTR cbOffsetStart, UINT_PTR cbOffsetEnd) 
{

    // NOTE: This function does not work if this file is memory-mapped.
    DWORD dw;
    FILE_ZERO_DATA_INFORMATION fzdi;
    fzdi.FileOffset.QuadPart = cbOffsetStart;
    fzdi.BeyondFinalZero.QuadPart = cbOffsetEnd + 1;
    return(::DeviceIoControl(hfile, FSCTL_SET_ZERO_DATA, 
        (LPVOID) &fzdi, sizeof(fzdi), NULL, 0, &dw, NULL));
}

// ������ ������ ũ�⸦ ����
// [in] liDistanceToMove : �������̸� dwMoveDirection �������� ������ �̵�
// [in] dwMoveMothod : �̵� ���� (FILE_BEGIN, FILE_CURRENT, FILE_END)
//			FILE_BEGIN : ���� ó������ �̵�
//			FILE_CURRENT : ���� ���� �����ͺ��� ���
//			FILE_END : ���� ���� ũ�⿡�� liDistanceToMove ��ƴ �� �̵�
inline BOOL CGMMF::SetToEndOfFile(HANDLE hfile, LARGE_INTEGER liDistanceToMove, DWORD dwMoveMothod /*= FILE_BEGIN*/)
{
	BOOL retVal = FALSE;
	retVal = SetFilePointerEx(hfile, liDistanceToMove, NULL, dwMoveMothod);
	if (retVal)
		retVal = SetEndOfFile(hfile);
	return retVal;
}


/*		��� ��
int _tmain(int argc, _TCHAR* argv[])
{
	char szPathname[] = "C:\\GMMF.";

	// Create the file
	HANDLE hfile = CreateFile(szPathname, GENERIC_READ | GENERIC_WRITE, 
		0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	// Create a GMMF using the file (set the maximum size here too)
	CGMMF gmmf(hfile, 10 * 1024 * 1024);

	int x = 0;
	// Read bytes from the file (0s are returned)
    for (x = 0; x < 10 * 1024 * 1024; x += 1024) {
        if (gmmf[x] != 0) DebugBreak();
    }

	// Write bytes to the file (clusters are allocated as necessary).
    for (x = 0; x < 100; x++) {
        gmmf[8 * 1024 * 1024 + x] = x;
    }

	// These lines just prove to us what's going on
	DWORD dw = GetFileSize(hfile, NULL);
	// This returns the logical size of the file.

	// Get the actual number of bytes allocated in the file
	dw = GetCompressedFileSize(szPathname, NULL);
	// This returns 0 because the data has not been written to the file yet.
	
	// Force the data to be written to the file
	FlushViewOfFile(gmmf, 10 * 1024 * 1024);

	// Get the actual number of bytes allocated in the file
	dw = GetCompressedFileSize(szPathname, NULL);
	// This returns the size of a cluster now

	// Normally the destructor causes the file-mapping to close.
	// But, in this case, we wish to force it so that we can reset 
	// a portion of the file back to all zeroes.
	gmmf.ForceClose();

	// We call ForceClose above because attempting to zero a portion of the 
	// file while it is mapped, causes DeviceIoControl to fail with error 
	// code 0x4C8 (ERROR_USER_MAPPED_FILE: "The requested operation cannot 
	// be performed on a file with a user-mapped section open.")
	CGMMF::SetToZero(hfile, 0, 2 * 1024 * 1024);

	// 
	dw = GetFileSize(hfile, NULL);
	dw = GetCompressedFileSize(szPathname, NULL);

	LARGE_INTEGER liDistanceToMove;
	
	liDistanceToMove.QuadPart = 9 * 1024 * 1024;
	//CGMMF::SetToEndOfFile(hfile, liDistanceToMove, FILE_BEGIN);
	CGMMF::SetToEndOfFile(hfile, liDistanceToMove, FILE_BEGIN);
	
	dw = GetFileSize(hfile, NULL);
	dw = GetCompressedFileSize(szPathname, NULL);

	// We no longer need access to the file, close it.
	CloseHandle(hfile);

	return(0);
}
*/