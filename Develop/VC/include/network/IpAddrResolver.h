//					CIpAddrResolver
//				- IPv4, IPv6 �ּ��� �ؼ��� �����ϴ� ���� Ŭ����
//		�ۼ��� : dede
//		�ۼ���(dede)�� ���Ǿ��� �ش� �ҽ��� ���, �����ϴ� ���� �ҹ��Դϴ�.
//	
//	2010/5/26
//		- ���� �ۼ�
//
//	2011/10/7
//		- GetNameInfo() ���� ����, ::getnameinfo() ó�� global ������ ǥ�� ��
//		- ���� �߰�
//		
//	[�˸�����]
//		- ws2_32.lib ���� �ʿ�
//
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

// getaddrinfo() �Լ� ���� ��� �߰�
// I defined _WIN32_WINNT in the preprocessor directives and it compiled OK.
 // _WIN32_WINNT=0x0500 is for Windows 2000, and 0x0501 and on for XP and later (http://msdn.microsoft.com/en-us/library/aa383745%28VS.85%29.aspx)
#if !defined(_WIN32_WINNT) || (_WIN32_WINNT <= 0x0500)
// Window 2000 �����̸� �߰�������� ����
#include <wspiapi.h>
#endif
#include <ws2tcpip.h>


////////////////////////////////////////////////////////////////////////////////////////////////////
//				CIpAddrResolver
//
//	- None-ThreadSafe
//
////////////////////////////////////////////////////////////////////////////////////////////////////
class CIpAddrResolver
{
public:
	CIpAddrResolver(void);
	~CIpAddrResolver(void);

	enum
	{
		// �Է°��� IPV4 Ȥ�� IPV6�� IP �ּҰ����� ǥ�� ("192.168.XXX.XXX", "fe80:250:8bff:fea0:92ed%5")
		AIF_NUMERICHOST	= 0x0001,

		// �Է°��� ������ �̸����� ǥ�� ("www.microsoft.com")
		AIF_CANONNAME	= 0x0002,
		

		// ��ȯ���� TCP Ÿ�Կ� ���� ��ȯ��� ǥ��
		AIF_TCP			= 0x0010,

		// ��ȯ���� UDP Ÿ�Կ� ���� ��ȯ��� ǥ��
		AIF_UDP			= 0x0020,

		
		// ��ȯ���� IPV4 �ּҰ� ���� ��ȯ��� ǥ��
		AIF_IPV4		= 0x0100,	

		// ��ȯ���� IPV6 �ּҰ� ���� ��ȯ��� ǥ��
		AIF_IPV6		= 0x0200,
		
		// ��ȯ���� IPV4 Ȥ�� IPV6 ������� (��θ�) ��ȯ��� ǥ��
		AIF_IPUNSPEC		= 0x0400
	};
	
	typedef enum _RET_ADDR_INFO
	{
		// ������� ���ų�, �ؼ� ����
		RET_ERR_FAIL = 0,

		// ����, ������ ����� ����
		RET_SUC_NEXT_NONE,

		// ����, �������� ������� ����, GetNextAddrInfo() ȣ�� ����
		RET_SUC_NEXT_EXIST
	} RET_ADDR_INFO;
	
	// �������ݿ� ������� �Է� ������ �ؼ��Ͽ� addrinfo ����ü�� ��ȯ
	//  - inet_addr() �Լ��� �ǹ� ���, MSDN::getaddrinfo() �Լ� ����
	// [in] szNodeName: "192.168.XXX.XXX" Ȥ�� "www.microsoft.com" Ȥ�� "" (INADDR_ANY�� �ǹ�)
	// [in] szServerName: ��Ʈ��ȣ("80") Ȥ�� "ftp, telnet"��� ���� ���񽺸�
	// [out] pOut: SOCKADDR ����ü�� �����ϰ� �ִ� addrinfo ����ü�� �����Ͽ� ��ȯ
	//			NULL: ���� ���� ���ϰ��� ��ȯ
	// [in] AIFlag: ���� �÷���
	// [retVal] ���� ���� �� ������ ���� ���θ� ��ȯ
	RET_ADDR_INFO GetFirstAddrInfo(
				const char* szNodeName, 
				const char* szServerName,
				struct addrinfo* pOut = NULL, 
				int AIFlag = AIF_NUMERICHOST | AIF_TCP | AIF_IPV4
				);
	
	// GetFirstAddrInfo() �� SUC_NEXT_EXIST �� ���, ���� ��� ���� ��ȯ
	// - IPV4�� IPV6 ����� ��θ� ��ȯ�� ���� ����, �������� ������� ���� ���� ��쿡 ��� ����
	// [out] pOut: SOCKADDR ����ü�� �����ϰ� �ִ� addrinfo ����ü�� �����Ͽ� ��ȯ
	// [retVal] ���� ���� �� ������ ���� ���θ� ��ȯ
	RET_ADDR_INFO GetNextAddrInfo(struct addrinfo* pOut);
	
	
	enum
	{
		// ��ȯ���� ���ڷε� IP�� �ڵ� ��ȯ��� �� (ȣ��Ʈ���̸�, reverse DNS lookup �����Ŀ� IP �ּҷ� ��ȯ)
		NIF_NUMERICHOST			= 0x0001,
		
		// ��ȯ���� "ftp"�� ���� ���� �̸��� �ƴ� ��Ʈ ��ȣ�� ��ȯ��� ��
		NIF_NUMERICSERV			= 0x0002,
		
		// UDP�� TCP ��Ʈ�� ���� ���� ���, UDP�� ��Ʈ ��ȣ�� ��ȯ��� ��
		NIF_UDP					= 0x0004,
		
		// �ؼ��� "mist.microsoft.com"�� ���� ȣ��Ʈ ���ڿ��̸�, ���ø��� "mist" �� ��ȯ��� ��
		NIF_EXTRACT_LOCALNAME	= 0x0008
	};
	
	// �����ּ� ����ü�� �ؼ��Ͽ� ȣ��Ʈ �̸��� ���� �̸�(��Ʈ��ȣ)�� ��ȯ
	//  - inet_ntoa() �Լ��� �ǹ� ���, MSDN::getnameinfo() �Լ� ����
	// [in] pSa: �����ּ� ����ü
	// [in] nSaLen: pSa ����ü ũ��
	// [out] szHost: ȣ��Ʈ �̸� ���� ����
	// [in] nHostLen: szHost ���� ũ��
	// [out] szServName: ���� �̸� ���� ����
	// [in] nHostLen: szServName ���� ũ��
	// [in] NIFlag: ���� �÷���
	// [retVal] ���� ���
    //		FALSE: �߸��� �Է� �Ķ���� Ȥ�� ���� ����
	static BOOL GetNameInfo(const struct sockaddr* pSa, socklen_t nSaLen, 
					char* szHost, DWORD nHostLen, 
					char* szServName, DWORD nServNameLen, 
					int NIFlag = NIF_NUMERICHOST | NIF_NUMERICSERV);

private:
	struct addrinfo* m_pLastAddrInfo;
};


// �ƾ�巹���� �Ʒ� ���� ����.
//http://stackoverflow.com/questions/18046063/mac-address-using-c


/*
// IpAddrResolver_Test.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <network\IpAddrResolver.h>

int _tmain(int argc, _TCHAR* argv[])
{
	const char* QUERY_DOMAIN = "www.naver.com";
	const char* QUERY_PORT = "http";

	CIpAddrResolver::RET_ADDR_INFO retAddrInfo;
	int nFoundedIp = 0;
	char szIpA[32], szPortA[16];
	addrinfo AddrInfo;
	CIpAddrResolver Res;

	retAddrInfo = Res.GetFirstAddrInfo(QUERY_DOMAIN, QUERY_PORT, &AddrInfo, 
			CIpAddrResolver::AIF_CANONNAME | CIpAddrResolver::AIF_TCP | CIpAddrResolver::AIF_IPV4);

	while (retAddrInfo != CIpAddrResolver::RET_ERR_FAIL)
	{
		if (CIpAddrResolver::GetNameInfo(AddrInfo.ai_addr, AddrInfo.ai_addrlen, szIpA, _countof(szIpA), szPortA, _countof(szPortA)))
		{
			nFoundedIp++;
			printf("SUCC, [%d] %s (%s)\n", nFoundedIp, szIpA, szPortA);
		}
		else
		{
			printf("Fail GetNameInfo");
		}
		
		retAddrInfo = Res.GetNextAddrInfo(&AddrInfo);
	}

	return 0;
}
*/