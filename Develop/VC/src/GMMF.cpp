#include "StdAfx.h"
#include "GMMF.h"

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

VOID CGMMF::ForceClose() 
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

BOOL CGMMF::SetToZero(HANDLE hfile, UINT_PTR cbOffsetStart, UINT_PTR cbOffsetEnd) 
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
BOOL CGMMF::SetToEndOfFile(HANDLE hfile, LARGE_INTEGER liDistanceToMove, DWORD dwMoveMothod /*= FILE_BEGIN*/)
{
	BOOL retVal = FALSE;
	retVal = SetFilePointerEx(hfile, liDistanceToMove, NULL, dwMoveMothod);
	if (retVal)
		retVal = SetEndOfFile(hfile);
	return retVal;
}