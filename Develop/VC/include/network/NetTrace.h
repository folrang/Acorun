#pragma once

namespace Network
{
	//////////////////////////////////////////////////////////
	//				CNetTrace
	//
	// - ����ȭ�� �˾Ƽ�..
	class CNetTrace
	{
	public:
		enum 
		{ 
			MAX_WRITEBUF = 2048 // WriteXXX �� ������ �ִ� ũ��
		};

		CNetTrace(BOOL bSupportOutputDebugString) : m_bSupportOutputDebugString(bSupportOutputDebugString) {}
		virtual ~CNetTrace(void) {}

		virtual void WriteErrorMsg(LPCTSTR szFormat, ...) = 0;
		virtual void WriteDebugMsg(LPCTSTR szFormat, ...) = 0;

	public:
		BOOL m_bSupportOutputDebugString;	// WriteXXX �޼��� ����ÿ� OutputDebugString �� ȣ���� �� ����
	};

	//////////////////////////////////////////////////////////
	//				CConsoleTrace
	//
	class CConsoleTrace : public CNetTrace
	{
	public:
		// [in] bSupportOutputDebugString: �޽����� �ܼ� �Ӹ� �ƴ϶�, OutputDebugString ���ε� ���� ����� �� ����
		CConsoleTrace(BOOL bSupportOutputDebugString = FALSE) : CNetTrace(bSupportOutputDebugString)  {}
		virtual ~CConsoleTrace() {}

		virtual void WriteErrorMsg(LPCTSTR szFormat, ...);
		virtual void WriteDebugMsg(LPCTSTR szFormat, ...);
	};
};



using namespace Network;

//////////////////////////////////////////////////////////
//				CConsoleTrace
//////////////////////////////////////////////////////////
inline void CConsoleTrace::WriteErrorMsg(LPCTSTR szFormat, ...)
{
	if (m_bSupportOutputDebugString)
	{
		TCHAR szBuf[CNetTrace::MAX_WRITEBUF];

		va_list	pArg;
		va_start(pArg, szFormat);
		wvsprintf(szBuf, szFormat, pArg);
		va_end(pArg);

		_ftprintf(stderr, szBuf);
		OutputDebugString(szBuf);
	}
	else
	{
		va_list	pArg;
		va_start(pArg, szFormat);
		_vftprintf(stderr, szFormat, pArg);
		va_end(pArg);
	}
}

inline void CConsoleTrace::WriteDebugMsg(LPCTSTR szFormat, ...)
{
	if (m_bSupportOutputDebugString)
	{
		TCHAR szBuf[CNetTrace::MAX_WRITEBUF];

		va_list	pArg;
		va_start(pArg, szFormat);
		wvsprintf(szBuf, szFormat, pArg);
		va_end(pArg);

		_tprintf(szBuf);
		OutputDebugString(szBuf);
	}
	else
	{
		va_list	pArg;
		va_start(pArg, szFormat);
		_vtprintf(szFormat, pArg);
		va_end(pArg);
	}
}
