//						Util.h
//			유틸함수들 모음
//
//		작성자: dede
//		작성자(dede)의 동의없이 해당 소스를 사용, 변경하는 것은 불법입니다.
//		소스 변경시에 작성자(dede)에게 문의해 주십시오.
//		
//		최종 수정일: 2010/02/08
//
//		[알림사항]
//
//		[주의사항]
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
	/// @brief	현재 프로세스의 폴더 경로를 반환
	///
	/// @param	[in, out] szPath	저장 버퍼
	/// @param	[in] dwSize	버퍼의 엘리먼트 크기 (_count() 매크로)
	///
	/// @return	추출된 폴더 경로를 반환
	////////////////////////////////////////////////////////////////////////////////////////////////////

	LPCTSTR GetCurrentDirPath(LPTSTR szPath, DWORD dwSize = MAX_PATH);


#pragma comment(linker, "/defaultlib:version.lib")
	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	BOOL GetFileVer(LPCTSTR szFilePath, LPTSTR szVer, DWORD dwVerSize)
	///
	/// @brief	입력 파일의 버전 문자열을 반환
	//			- 파일내의 VS_VERSION_INFO 리소스에서 "FileVersion" 항목을 추출하여 반환
	///
	/// @param	[in] szFilePath	원본 파일의 경로, LoadLibrary() 함수의 경로와 동일
	/// @param	[in, out] szVer	저장 버퍼
	/// @param	[in] dwVerSize	버퍼의 엘리먼트 크기 (_count() 매크로)
	///
	/// @return	성공여부 반환
	/// @warning: Version.lib 임포트 필요, MSDN::GetFileVersionInfoSize() 함수 참조
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
	// 최초 작성일: 2011.02.08
	// 출처: http://wearekorean.co.kr/zbxe/37881
	// 참고: http://www.microsoft.com/msj/0498/c0498.aspx, 
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