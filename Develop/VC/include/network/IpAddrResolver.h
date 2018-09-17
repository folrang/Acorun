//					CIpAddrResolver
//				- IPv4, IPv6 주소의 해석을 지원하는 헬퍼 클래스
//		작성자 : dede
//		작성자(dede)의 동의없이 해당 소스를 사용, 변경하는 것은 불법입니다.
//	
//	2010/5/26
//		- 최초 작성
//
//	2011/10/7
//		- GetNameInfo() 구현 수정, ::getnameinfo() 처럼 global 스코프 표시 함
//		- 예제 추가
//		
//	[알림사항]
//		- ws2_32.lib 포함 필요
//
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

// getaddrinfo() 함수 정의 헤더 추가
// I defined _WIN32_WINNT in the preprocessor directives and it compiled OK.
 // _WIN32_WINNT=0x0500 is for Windows 2000, and 0x0501 and on for XP and later (http://msdn.microsoft.com/en-us/library/aa383745%28VS.85%29.aspx)
#if !defined(_WIN32_WINNT) || (_WIN32_WINNT <= 0x0500)
// Window 2000 이하이면 추가헤더파일 선언
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
		// 입력값이 IPV4 혹은 IPV6의 IP 주소값임을 표시 ("192.168.XXX.XXX", "fe80:250:8bff:fea0:92ed%5")
		AIF_NUMERICHOST	= 0x0001,

		// 입력값이 도메인 이름임을 표시 ("www.microsoft.com")
		AIF_CANONNAME	= 0x0002,
		

		// 반환값이 TCP 타입용 만을 반환토록 표시
		AIF_TCP			= 0x0010,

		// 반환값이 UDP 타입용 만을 반환토록 표시
		AIF_UDP			= 0x0020,

		
		// 반환값이 IPV4 주소값 만을 반환토록 표시
		AIF_IPV4		= 0x0100,	

		// 반환값이 IPV6 주소값 만을 반환토록 표시
		AIF_IPV6		= 0x0200,
		
		// 반환값이 IPV4 혹은 IPV6 상관없이 (모두를) 반환토록 표시
		AIF_IPUNSPEC		= 0x0400
	};
	
	typedef enum _RET_ADDR_INFO
	{
		// 결과값이 없거나, 해석 실패
		RET_ERR_FAIL = 0,

		// 성공, 더많은 결과값 없음
		RET_SUC_NEXT_NONE,

		// 성공, 복수개의 결과값이 존재, GetNextAddrInfo() 호출 가능
		RET_SUC_NEXT_EXIST
	} RET_ADDR_INFO;
	
	// 프로토콜에 상관없이 입력 조건을 해석하여 addrinfo 구조체를 반환
	//  - inet_addr() 함수와 의미 비슷, MSDN::getaddrinfo() 함수 참조
	// [in] szNodeName: "192.168.XXX.XXX" 혹은 "www.microsoft.com" 혹은 "" (INADDR_ANY를 의미)
	// [in] szServerName: 포트번호("80") 혹은 "ftp, telnet"등과 같은 서비스명
	// [out] pOut: SOCKADDR 구조체를 포함하고 있는 addrinfo 구조체를 복사하여 반환
	//			NULL: 복사 없이 리턴값만 반환
	// [in] AIFlag: 실행 플래그
	// [retVal] 성공 여부 및 복수개 존재 여부를 반환
	RET_ADDR_INFO GetFirstAddrInfo(
				const char* szNodeName, 
				const char* szServerName,
				struct addrinfo* pOut = NULL, 
				int AIFlag = AIF_NUMERICHOST | AIF_TCP | AIF_IPV4
				);
	
	// GetFirstAddrInfo() 가 SUC_NEXT_EXIST 일 경우, 다음 결과 값을 반환
	// - IPV4와 IPV6 결과값 모두를 반환할 때와 같이, 복수개의 결과값에 관심 있을 경우에 사용 가능
	// [out] pOut: SOCKADDR 구조체를 포함하고 있는 addrinfo 구조체를 복사하여 반환
	// [retVal] 성공 여부 및 복수개 존재 여부를 반환
	RET_ADDR_INFO GetNextAddrInfo(struct addrinfo* pOut);
	
	
	enum
	{
		// 반환값이 숫자로된 IP를 자동 반환토록 함 (호스트명이면, reverse DNS lookup 실행후에 IP 주소로 반환)
		NIF_NUMERICHOST			= 0x0001,
		
		// 반환값이 "ftp"와 같은 서비스 이름이 아닌 포트 번호를 반환토록 함
		NIF_NUMERICSERV			= 0x0002,
		
		// UDP와 TCP 포트가 같이 있을 경우, UDP용 포트 번호를 반환토록 함
		NIF_UDP					= 0x0004,
		
		// 해석후 "mist.microsoft.com"와 같이 호스트 문자열이면, 로컬명인 "mist" 만 반환토록 함
		NIF_EXTRACT_LOCALNAME	= 0x0008
	};
	
	// 소켓주소 구조체를 해석하여 호스트 이름과 서비스 이름(포트번호)를 반환
	//  - inet_ntoa() 함수와 의미 비슷, MSDN::getnameinfo() 함수 참조
	// [in] pSa: 소켓주소 구조체
	// [in] nSaLen: pSa 구조체 크기
	// [out] szHost: 호스트 이름 저장 버퍼
	// [in] nHostLen: szHost 버퍼 크기
	// [out] szServName: 서비스 이름 저장 버퍼
	// [in] nHostLen: szServName 버퍼 크기
	// [in] NIFlag: 실행 플래그
	// [retVal] 실행 결과
    //		FALSE: 잘못된 입력 파라미터 혹은 버퍼 부족
	static BOOL GetNameInfo(const struct sockaddr* pSa, socklen_t nSaLen, 
					char* szHost, DWORD nHostLen, 
					char* szServName, DWORD nServNameLen, 
					int NIFlag = NIF_NUMERICHOST | NIF_NUMERICSERV);

private:
	struct addrinfo* m_pLastAddrInfo;
};


// 맥어드레스는 아래 내용 참고.
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