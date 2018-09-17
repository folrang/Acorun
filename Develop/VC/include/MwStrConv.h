/////////////////////////////////////////////////////////////////////
//				MwStrConv.h 
//						Programmed by DEDE (dede@ubicnt.com)
//						* 개발자의 동의 없이 수정 및 배포를 금합니다
//
// * 2009.5.12
//		- 최초개발
//
// [참고사항]
//	1. ATL 7.0이 지원되면 ATL7.0의 컨버젼 클래스를 사용하도록 해야 한다
//  2. 만일 VS 6.0 환경과 같이 ATL 7.0을 지원하지 않는다면, 아래 클래스들을 사용하길 권한다
//
/////////////////////////////////////////////////////////////////////

#ifndef __MWSTRCONV_H__
#define __MWSTRCONV_H__

#pragma once

namespace MwStrConv
{
	template< int nDefaultStackBufSize = 128 >
	class CW2WEX
	{
	public:
		CW2WEX(LPCWSTR psz ) : m_pOut(NULL)
		{
			Init(psz);
		}
		CW2WEX(LPCWSTR psz, UINT nCodePage) : m_pOut(NULL)
		{
			Init(psz);
		}
		CW2WEX( const CW2WEX& rhs) 
		{
			Copy(rhs);
		}
		~CW2WEX()
		{
			UnInit();
		}

		operator LPWSTR() const
		{
			return(m_pOut);
		}
		CW2WEX& operator = (const CW2WEX& rhs)
		{
			if (this != &rhs)
			{
				Destroy();
				Copy(rhs);
			}
			return *this;
		}

	private:
		void Init(LPCWSTR psz )
		{
			int nLength = wcslen(psz) + 1;
			if (nLength > nDefaultStackBufSize)
			{
				m_pHeapBuf = new WCHAR[nLength];
				wcscpy(m_pHeapBuf, psz);
				m_pOut = m_pHeapBuf;
			}
			else
			{
				wcscpy(m_szBuf, psz);
				m_pOut = m_szBuf;
			}
		}
		void UnInit()
		{
			wcscpy(m_szBuf, L"");
			delete [] m_pHeapBuf;
			m_pHeapBuf = NULL;
			m_pOut = NULL;
		}
		void Copy(const CW2WEX& rhs)
		{
			wcscpy(m_szBuf, rhs.m_szBuf);
			m_pOut = m_szBuf;

			if (rhs.m_pHeapBuf)
			{
				m_pHeapBuf = new WCHAR [wcslen(rhs.m_pHeapBuf) +1];
				wcscpy(m_pHeapBuf, rhs.m_pHeapBuf);
				m_pOut = m_pHeapBuf;
			}
		}
		void Destroy()
		{
			UnInit();
		}

	private:
		LPWSTR m_pOut;
		WCHAR m_szBuf[nDefaultStackBufSize];
		WCHAR* m_pHeapBuf;
	};
	typedef CW2WEX<> CW2W;

	template< int nDefaultStackBufSize = 128 >
	class CA2AEX
	{
	public:
		CA2AEX(LPCWSTR psz ) : m_pOut(NULL)
		{
			Init(psz);
		}
		CA2AEX(LPCWSTR psz, UINT nCodePage) : m_pOut(NULL)
		{
			Init(psz);
		}
		CA2AEX( const CA2AEX& rhs) 
		{
			Copy(rhs);
		}
		~CA2AEX()
		{
			UnInit();
		}
		operator LPSTR() const
		{
			return(m_pOut);
		}
		CA2AEX& operator = (const CA2AEX& rhs)
		{
			if (this != &rhs)
			{
				Destroy();
				Copy(rhs);
			}
			return *this;
		}

	private:
		void Init(LPCSTR psz)
		{
			int nLength = strlen(psz) + 1;
			if (nLength > nDefaultStackBufSize)
			{
				m_pHeapBuf = new char[nLength];
				strcpy(m_pHeapBuf, psz);
				m_pOut = m_pHeapBuf;
			}
			else
			{
				strcpy(m_szBuf, psz);
				m_pOut = m_szBuf;
			}
		}
		void UnInit()
		{
			strcpy(m_szBuf, L"");
			delete [] m_pHeapBuf;
			m_pHeapBuf = NULL;
			m_pOut = NULL;
		}
		void Copy(const CA2AEX& rhs)
		{
			strcpy(m_szBuf, rhs.m_szBuf);
			m_pOut = m_szBuf;

			if (rhs.m_pHeapBuf)
			{
				m_pHeapBuf = new char [strlen(rhs.m_pHeapBuf) +1];
				strcpy(m_pHeapBuf, rhs.m_pHeapBuf);
				m_pOut = m_pHeapBuf;
			}
		}
		void Destroy()
		{
			UnInit();
		}

	private:
		LPSTR m_pOut;
		char m_szBuf[nDefaultStackBufSize];
		char* m_pHeapBuf;
	};
	typedef CA2AEX<> CA2A;

	template< int nDefaultStackBufSize = 128 >
	class CA2CAEX
	{
	public:
		CA2CAEX(LPCSTR psz ) : m_pOut( psz )
		{
		}
		CA2CAEX(LPCSTR psz, UINT nCodePage ) : m_pOut( psz )
		{
		}
		CA2CAEX( const CA2CAEX& rhs)
		{
			m_pOut = const_cast<LPSTR> (rhs.m_pOut);
		}
		~CA2CAEX()
		{
		}

		operator LPCSTR() const
		{
			return( m_pOut );
		}
		CA2CAEX& operator = ( const CA2CAEX& rhs)
		{
			if (this != &rhs)
			{
				m_pOut = const_cast<LPSTR> (rhs.m_pOut);
			}
			return *this;
		}

	private:
		LPCSTR m_pOut;		
	};
	typedef CA2CAEX<> CA2CA;

	template< int nDefaultStackBufSize = 128 >
	class CW2CWEX
	{
	public:
		CW2CWEX(LPCWSTR psz )  : m_pOut( psz )
		{
		}
		CW2CWEX(LPCWSTR psz, UINT nCodePage ) : m_pOut(psz)
		{
		}
		CW2CWEX( const CW2CWEX& rhs)
		{
			m_pOut = const_cast<LPWSTR> (rhs.m_pOut);
		}
		~CW2CWEX()
		{
		}

		operator LPCWSTR() const
		{
			return( m_pOut );
		}

		CW2CWEX& operator = ( const CW2CWEX& rhs)
		{
			if (this != &rhs)
			{
				m_pOut = const_cast<LPWSTR> (rhs.m_pOut);
			}
			return *this;
		}

	private:
		LPCWSTR m_pOut;
	};
	typedef CW2CWEX<> CW2CW;

	template< int nDefaultStackBufSize = 128 >
	class CA2WEX
	{
	public:
		CA2WEX(LPCSTR psz ) : m_pOut(NULL)
		{
			Init( psz, CP_ACP);
		}
		CA2WEX(LPCSTR psz, UINT nCodePage) : m_pOut(NULL)
		{
			Init( psz, nCodePage);
		}
		CA2WEX( const CA2WEX& rhs)
		{
			Copy(rhs);
		}
		~CA2WEX()
		{
			UnInit();
		}

		operator LPWSTR() const
		{
			return( m_pOut);
		}
		
		CA2WEX& operator= (const CA2WEX& rhs)
		{
			if (this != &rhs)
			{
				Destroy();
				Copy(rhs);
			}
			return *this;
		}

	private:
		void Init(LPCSTR psz, UINT nCodePage)
		{
			wcscpy(m_szBuf, L"");
			m_pHeapBuf = NULL;
			
			m_pOut = m_szBuf;

			if (psz == NULL)
				return;
			
			int nLengthA = strlen(psz) + 1;
			int nLengthW = MultiByteToWideChar(nCodePage, 0, psz, nLengthA, m_pOut, 0);
			if (nLengthW > nDefaultStackBufSize)
			{
				m_pHeapBuf = new WCHAR [nLengthW];
				wcscpy(m_pHeapBuf, L"");

				m_pOut = m_pHeapBuf;
			}				
			
			MultiByteToWideChar(nCodePage, 0, psz, nLengthA, m_pOut, nLengthW);
		}

		void UnInit()
		{
			wcscpy(m_szBuf, L"");
			delete [] m_pHeapBuf;
			m_pHeapBuf = NULL;
			m_pOut = NULL;
		}

		void Copy(const CA2WEX& rhs)
		{
			wcscpy(m_szBuf, rhs.m_szBuf);
			m_pOut = m_szBuf;

			if (rhs.m_pHeapBuf)
			{
				m_pHeapBuf = new WCHAR [wcslen(rhs.m_pHeapBuf) +1];
				wcscpy(m_pHeapBuf, rhs.m_pHeapBuf);
				m_pOut = m_pHeapBuf;
			}
		}
		void Destroy()
		{
			UnInit();
		}

	private:
		LPWSTR m_pOut;
		WCHAR m_szBuf[nDefaultStackBufSize];
		WCHAR* m_pHeapBuf;		
	};
	typedef CA2WEX<> CA2W;

	template< int nDefaultStackBufSize = 128 >
	class CW2AEX
	{
	public:
		CW2AEX(LPCWSTR psz ) : m_pOut(NULL)
		{
			Init(psz, CP_ACP);
		}
		CW2AEX(LPCWSTR psz, UINT nCodePage ) : m_pOut(NULL)
		{
			Init(psz, nCodePage);
		}
		CW2AEX(const CW2AEX& rhs)
		{
			Copy(rhs);
		}
		~CW2AEX() throw()
		{		
			UnInit();
		}
	
		operator LPSTR() const
		{
			return(m_pOut);
		}

		CW2AEX& operator = (const CW2AEX& rhs)
		{
			if (this != &rhs)
			{
				Destroy();
				Copy(rhs);
			}
			return *this;
		}
	
	private:
		void Init(LPCWSTR psz, UINT nCodePage)
		{
			strcpy(m_szBuf, "");
			m_pHeapBuf = NULL;
			
			m_pOut = m_szBuf;

			if (psz == NULL)
				return;
			
			int nLengthW = lstrlenW(psz) + 1;
			int nLengthA = WideCharToMultiByte(nCodePage, 0, psz, nLengthW, m_pOut, 0, NULL, NULL);
			if (nLengthA > nDefaultStackBufSize)
			{
				m_pHeapBuf = new char [nLengthA];
				strcpy(m_pHeapBuf, "");

				m_pOut = m_pHeapBuf;
			}
			
			WideCharToMultiByte(nCodePage, 0, psz, nLengthW, m_pOut, nLengthA, NULL, NULL);
		}

		void UnInit()
		{
			strcpy(m_szBuf, "");
			delete [] m_pHeapBuf;
			m_pHeapBuf = NULL;
			m_pOut = NULL;
		}

		void Copy(const CW2AEX& rhs)
		{
			strcpy(m_szBuf, rhs.m_szBuf);
			m_pOut = m_szBuf;

			if (rhs.m_pHeapBuf)
			{
				m_pHeapBuf = new char [strlen(rhs.m_pHeapBuf) +1];
				strcpy(m_pHeapBuf, rhs.m_pHeapBuf);
				m_pOut = m_pHeapBuf;
			}
		}
		void Destroy()
		{
			UnInit();
		}

	private:
		LPSTR m_pOut;
		char m_szBuf[nDefaultStackBufSize];
		char* m_pHeapBuf;
	};
	typedef CW2AEX<> CW2A;

	
#ifdef _UNICODE

	#define CW2T CW2W
	#define CW2TEX CW2WEX
	#define CW2CT CW2CW
	#define CW2CTEX CW2CWEX
	#define CT2W CW2W
	#define CT2WEX CW2WEX
	#define CT2CW CW2CW
	#define CT2CWEX CW2CWEX

	#define CA2T CA2W
	#define CA2TEX CA2WEX
	#define CA2CT CA2W
	#define CA2CTEX CA2WEX
	#define CT2A CW2A
	#define CT2AEX CW2AEX
	#define CT2CA CW2A
	#define CT2CAEX CW2AEX

#else  // !_UNICODE

	#define CW2T CW2A
	#define CW2TEX CW2AEX
	#define CW2CT CW2A
	#define CW2CTEX CW2AEX
	#define CT2W CA2W
	#define CT2WEX CA2WEX
	#define CT2CW CA2W
	#define CT2CWEX CA2WEX

	#define CA2T CA2A
	#define CA2TEX CA2AEX
	#define CA2CT CA2CA
	#define CA2CTEX CA2CAEX
	#define CT2A CA2A
	#define CT2AEX CA2AEX
	#define CT2CA CA2CA
	#define CT2CAEX CA2CAEX

#endif  // !_UNICODE
};

#endif

/*
int _tmain(int argc, _TCHAR* argv[])
{
	WCHAR szMsgW[128];
	char szMsgA[128];
	TCHAR szMsgT[128];
	wcscpy(szMsgW, L"가나다라마바사아자차카타파하");
	strcpy(szMsgA, "가나다라마바사아자차카타파하");
	_tcscpy(szMsgT, _T("가나다라마바사아자차카타파하"));

	MwStrConv::CT2CAEX<12> test(szMsgT);
	strcpy(szMsgA, "");
	strcpy(szMsgA, MwStrConv::CT2CAEX<12>(szMsgT));
	
	MwStrConv::CT2CAEX<12> test1(test);
	strcpy(szMsgA, test1);
	//wcscpy(szMsgW, test);
	
	return 0;
}
*/