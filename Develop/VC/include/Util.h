//						Util.h
//			��ƿ�Լ��� ����
//
//		�ۼ���: dede
//		�ۼ���(dede)�� ���Ǿ��� �ش� �ҽ��� ���, �����ϴ� ���� �ҹ��Դϴ�.
//		�ҽ� ����ÿ� �ۼ���(dede)���� ������ �ֽʽÿ�.
//		
//		���� ������: 2010/02/08
//
//		[�˸�����]
//
//		[���ǻ���]
//
//		[ToDoList]
//
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

namespace Util
{
	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	template <class TV, class TM> inline TV ROUNDDOWN(TV Value, TM Multiple)
	///
	/// @brief	This inline function rounds a value down to the nearest multiple
	////////////////////////////////////////////////////////////////////////////////////////////////////

	template <class TV, class TM>
	inline TV ROUNDDOWN(TV Value, TM Multiple) {
	   return((Value / Multiple) * Multiple);
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	template <class TV, class TM> inline TV ROUNDUP(TV Value, TM Multiple)
	///
	/// @brief	This inline function rounds a value down to the nearest multiple
	////////////////////////////////////////////////////////////////////////////////////////////////////

	template <class TV, class TM>
	inline TV ROUNDUP(TV Value, TM Multiple) {
	   return(ROUNDDOWN(Value, Multiple) + 
		  (((Value % Multiple) > 0) ? Multiple : 0));
	}
};

namespace Util
{
	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	LPCTSTR GetCurrentDirPath(LPTSTR szPath, DWORD dwSize = MAX_PATH)
	///
	/// @brief	���� ���μ����� ���� ��θ� ��ȯ
	///
	/// @param	[in, out] szPath	���� ����
	/// @param	[in] dwSize	������ ������Ʈ ũ�� (_count() ��ũ��)
	///
	/// @return	����� ���� ��θ� ��ȯ
	////////////////////////////////////////////////////////////////////////////////////////////////////

	LPCTSTR GetCurrentDirPath(LPTSTR szPath, DWORD dwSize = MAX_PATH);


#pragma comment(linker, "/defaultlib:version.lib")
	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	BOOL GetFileVer(LPCTSTR szFilePath, LPTSTR szVer, DWORD dwVerSize)
	///
	/// @brief	�Է� ������ ���� ���ڿ��� ��ȯ
	//			- ���ϳ��� VS_VERSION_INFO ���ҽ����� "FileVersion" �׸��� �����Ͽ� ��ȯ
	///
	/// @param	[in] szFilePath	���� ������ ���, LoadLibrary() �Լ��� ��ο� ����
	/// @param	[in, out] szVer	���� ����
	/// @param	[in] dwVerSize	������ ������Ʈ ũ�� (_count() ��ũ��)
	///
	/// @return	�������� ��ȯ
	/// @warning: Version.lib ����Ʈ �ʿ�, MSDN::GetFileVersionInfoSize() �Լ� ����
	///
	////////////////////////////////////////////////////////////////////////////////////////////////////
	BOOL GetFileVersion(LPCTSTR szFilePath, LPTSTR szVer, DWORD dwVerSize);
};


inline LPCTSTR Util::GetCurrentDirPath(LPTSTR szPath, DWORD dwSize)
{
	GetModuleFileName(NULL, szPath, dwSize);
	TCHAR* pEnd = _tcsrchr(szPath, '\\');
	*pEnd = '\0';
	return szPath;
}

inline BOOL Util::GetFileVersion(LPCTSTR szFilePath, LPTSTR szVer, DWORD dwVerSize)
{
	// ���� �ۼ���: 2011.02.08
	// ��ó: http://wearekorean.co.kr/zbxe/37881
	// ����: http://www.microsoft.com/msj/0498/c0498.aspx, 
	//			http://crowback.tistory.com/116
	
	if (szVer == NULL || dwVerSize < 1)
		return FALSE;

	szVer[0] = '\0';

	BOOL retVal = FALSE;
	DWORD dwHandle = 0;
	DWORD dwDestSize = 0;

	dwDestSize = GetFileVersionInfoSize(szFilePath, &dwHandle);
	
	if(dwDestSize > 0)
	{
		BYTE* pbDestData = new BYTE[dwDestSize+1];

		if(GetFileVersionInfo(szFilePath, dwHandle, dwDestSize, pbDestData))
		{
			VS_FIXEDFILEINFO *pvsInfo = NULL;
			UINT uLen = 0;

			if(VerQueryValue(pbDestData, _T("\\"), (void**)&pvsInfo, &uLen))
			{
				int nRet = -1;
				nRet = _stprintf_s(szVer, dwVerSize, _T("%d.%d.%d.%d"), 
							(pvsInfo->dwProductVersionMS / 65536), (pvsInfo->dwFileVersionMS % 65536), 
							(pvsInfo->dwFileVersionLS / 65536), (pvsInfo->dwFileVersionLS % 65536)
							);

				if (nRet != -1)
					retVal = TRUE;
			}
		}
		delete [] pbDestData;
	}

	return retVal;
}