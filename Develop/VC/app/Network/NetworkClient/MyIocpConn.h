#pragma once

#include "IocpTcpClient.h"
using namespace NetClient;

namespace NetClient
{

	class CMyIocpConn : public CIocpConn
	{
	public:		
		virtual ~CMyIocpConn() {}

		// Remote host �� ����
		// [in] szIp: ���� IPV4 Ȥ�� IPV6
		// [in] nPort: ��Ʈ ��ȣ
		// [in] dwTimeOut: ���� ��� �ð�
		virtual RET_CODE PostConnect(LPCTSTR szIp, int nPort, DWORD dwTimeOut);
		virtual RET_CODE PostRecvOp();
		virtual RET_CODE PostSendOp();
		virtual RET_CODE PostCloseOp();

		virtual RET_CODE CompConnect() = 0;
		virtual RET_CODE CompRecvOp() = 0;
		virtual RET_CODE CompSendOp() = 0;
		virtual RET_CODE CompCloseOp() = 0;

	public:
		CMyIocpConn();
	};

};
