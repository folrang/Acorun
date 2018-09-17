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


// 프로토콜에 상관없이 입력 조건을 해석하여 addrinfo 구조체를 반환
//  - getaddrinfo()를 사용하고, 과거 inet_addr() 함수와 의미 비슷
// [in] szNodeName: "192.168.XXX.XXX" 혹은 "www.microsoft.com" 혹은 "" (INADDR_ANY를 의미)
// [in] szServerName: 포트번호("80") 혹은 "ftp, telnet"등과 같은 서비스명
// [out] pOut: SOCKADDR 구조체를 포함하고 있는 addrinfo 구조체를 복사하여 반환
//			NULL: 복사 없이 리턴값만 반환
// [in] AIFlag: 실행 플래그
// [retVal] 성공 여부 및 복수개 존재 여부를 반환
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

// GetFirstAddrInfo() 가 SUC_NEXT_EXIST 일 경우, 다음 결과 값을 반환
// - IPV4와 IPV6 결과값 모두를 반환할 때와 같이, 복수개의 결과값에 관심 있을 경우에 사용 가능
// [out] pOut: SOCKADDR 구조체를 포함하고 있는 addrinfo 구조체를 복사하여 반환
// [retVal] 성공 여부 및 복수개 존재 여부를 반환
CIpAddrResolver::RET_ADDR_INFO 
CIpAddrResolver::GetNextAddrInfo(struct addrinfo* pOut)
{
	memset(pOut, 0, sizeof(addrinfo));

	if (m_pLastAddrInfo->ai_next == NULL)
		return CIpAddrResolver::RET_ERR_FAIL;

	// linked list의 다음 노드로 이동
	m_pLastAddrInfo = m_pLastAddrInfo->ai_next;

	if (pOut) 
		memcpy(pOut, m_pLastAddrInfo, sizeof(addrinfo));

	if (m_pLastAddrInfo->ai_next)
		return CIpAddrResolver::RET_SUC_NEXT_EXIST;
	
	return CIpAddrResolver::RET_SUC_NEXT_NONE;
}

// 소켓주소 구조체에서 호스트 이름과 서비스 이름(포트번호)를 반환
//  - inet_ntoa() 함수와 의미 비슷
// [in] pSa: 소켓주소 구조체
// [in] nSaLen: pSa 구조체 크기
// [out] szHost: 호스트 이름 저장 버퍼
// [in] nHostLen: szHost 버퍼 크기
// [out] szServName: 서비스 이름 저장 버퍼
// [in] nHostLen: szServName 버퍼 크기
// [in] NIFlag: 실행 플래그
// [retVal] 실행 결과
//		FALSE: 잘못된 입력 파라미터 혹은 버퍼 부족
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