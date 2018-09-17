#include "StdAfx.h"
#include <network/IpAddrResolver.h>

CIpAddrResolver::CIpAddrResolver(void)
{
	WSADATA wsd;
	WSAStartup(0x0202/*MAKEWORD(2,2)*/, &wsd);

	m_pLastAddrInfo = NULL;	
}

CIpAddrResolver::~CIpAddrResolver(void)
{
	freeaddrinfo(m_pLastAddrInfo);
	WSACleanup();
}


// �������ݿ� ������� �Է� ������ �ؼ��Ͽ� addrinfo ����ü�� ��ȯ
//  - getaddrinfo()�� ����ϰ�, ���� inet_addr() �Լ��� �ǹ� ���
// [in] szNodeName: "192.168.XXX.XXX" Ȥ�� "www.microsoft.com" Ȥ�� "" (INADDR_ANY�� �ǹ�)
// [in] szServerName: ��Ʈ��ȣ("80") Ȥ�� "ftp, telnet"��� ���� ���񽺸�
// [out] pOut: SOCKADDR ����ü�� �����ϰ� �ִ� addrinfo ����ü�� �����Ͽ� ��ȯ
//			NULL: ���� ���� ���ϰ��� ��ȯ
// [in] AIFlag: ���� �÷���
// [retVal] ���� ���� �� ������ ���� ���θ� ��ȯ
CIpAddrResolver::RET_ADDR_INFO 
CIpAddrResolver::GetFirstAddrInfo(const char* szNodeName, const char* szServerName, 
								  struct addrinfo* pOut /*= NULL*/,
								  int AIFlag /*=AIF_NUMERICHOST | AIF_TCP | AIF_IPV4*/
								  )
{
	freeaddrinfo(m_pLastAddrInfo);
	if (pOut) 
		memset(pOut, 0, sizeof(addrinfo));
	

	struct addrinfo hints;

	memset(&hints, 0, sizeof(addrinfo));

	if (szNodeName[0] == '\0')
		hints.ai_flags = AI_PASSIVE;
	else if (AIFlag & CIpAddrResolver::AIF_CANONNAME)
		hints.ai_flags = AI_CANONNAME;
	else
		hints.ai_flags = AI_NUMERICHOST;

	if (AIFlag & CIpAddrResolver::AIF_IPV4)
		hints.ai_family = AF_INET;
	else if (AIFlag & CIpAddrResolver::AIF_IPV6)
		hints.ai_family = AF_INET6;
	else
		hints.ai_family = AF_UNSPEC;

	if (AIFlag & CIpAddrResolver::AIF_UDP)
	{
		hints.ai_socktype = SOCK_DGRAM;
		hints.ai_protocol = IPPROTO_UDP;
	}
	else
	{
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_protocol = IPPROTO_TCP;
	}
	
	int rc = ::getaddrinfo(szNodeName, szServerName, &hints, &m_pLastAddrInfo);
	if (rc != 0)
		return CIpAddrResolver::RET_ERR_FAIL;
	
	if (pOut) 
		memcpy(pOut, m_pLastAddrInfo, sizeof(addrinfo));

	if (m_pLastAddrInfo->ai_next)
		return CIpAddrResolver::RET_SUC_NEXT_EXIST;
	
	return CIpAddrResolver::RET_SUC_NEXT_NONE;
}

// GetFirstAddrInfo() �� SUC_NEXT_EXIST �� ���, ���� ��� ���� ��ȯ
// - IPV4�� IPV6 ����� ��θ� ��ȯ�� ���� ����, �������� ������� ���� ���� ��쿡 ��� ����
// [out] pOut: SOCKADDR ����ü�� �����ϰ� �ִ� addrinfo ����ü�� �����Ͽ� ��ȯ
// [retVal] ���� ���� �� ������ ���� ���θ� ��ȯ
CIpAddrResolver::RET_ADDR_INFO 
CIpAddrResolver::GetNextAddrInfo(struct addrinfo* pOut)
{
	memset(pOut, 0, sizeof(addrinfo));

	if (m_pLastAddrInfo->ai_next == NULL)
		return CIpAddrResolver::RET_ERR_FAIL;

	// linked list�� ���� ���� �̵�
	m_pLastAddrInfo = m_pLastAddrInfo->ai_next;

	if (pOut) 
		memcpy(pOut, m_pLastAddrInfo, sizeof(addrinfo));

	if (m_pLastAddrInfo->ai_next)
		return CIpAddrResolver::RET_SUC_NEXT_EXIST;
	
	return CIpAddrResolver::RET_SUC_NEXT_NONE;
}

// �����ּ� ����ü���� ȣ��Ʈ �̸��� ���� �̸�(��Ʈ��ȣ)�� ��ȯ
//  - inet_ntoa() �Լ��� �ǹ� ���
// [in] pSa: �����ּ� ����ü
// [in] nSaLen: pSa ����ü ũ��
// [out] szHost: ȣ��Ʈ �̸� ���� ����
// [in] nHostLen: szHost ���� ũ��
// [out] szServName: ���� �̸� ���� ����
// [in] nHostLen: szServName ���� ũ��
// [in] NIFlag: ���� �÷���
// [retVal] ���� ���
//		FALSE: �߸��� �Է� �Ķ���� Ȥ�� ���� ����
BOOL CIpAddrResolver::GetNameInfo(const struct sockaddr* pSa, socklen_t nSaLen, 
				char* szHost, DWORD nHostLen, 
				char* szServName, DWORD nServNameLen, 
				int NIFlag /*= NIF_NUMERICHOST | NIF_NUMERICSERV*/)
{
	szHost[0] = szServName[0] = '\0';

	int flags = 0;
	if (NIFlag & NIF_NUMERICHOST)
		flags |= NI_NUMERICHOST;

	if (NIFlag & NIF_NUMERICSERV)
		flags |= NI_NUMERICSERV;

	if (NIFlag & NIF_UDP)
		flags |= NI_DGRAM;

	if (NIFlag & NIF_EXTRACT_LOCALNAME)
		flags |= NI_NOFQDN;
	
	int rc = ::getnameinfo(pSa, nSaLen, szHost, nHostLen, szServName, nServNameLen, flags);
	return (!rc);
}