//******************************************************************/
//******************************************************************/
//! All Rights Reserved. Copyright(c)   2014 (주)한국소리마치        /
//******************************************************************/
//! File Name     : KhOpenApiTest.cpp
//! Function      : 키움 오픈API 테스트
//! System Name   : 키움 오픈API 테스트
//! Create        : , 2014/06/02
//! Update        : 
//! Comment       : 
//******************************************************************/
// KhOpenApiTest.cpp : 응용 프로그램에 대한 클래스 동작을 정의합니다.
//

#include "stdafx.h"
#include "afxctl.h"
#include "KhOpenApiTest.h"
#include "KhOpenApiTestDlg.h"
#include "KHOpenAPICtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CKhOpenApiTestApp
//*******************************************************************/
// BEGIN_MESSAGE_MAP
//*******************************************************************/
BEGIN_MESSAGE_MAP(CKhOpenApiTestApp, CWinApp)
END_MESSAGE_MAP()


// CKhOpenApiTestApp 생성
//*******************************************************************/
//! Function Name : CKhOpenApiTestApp
//! Function      : 생성 처리
//! Param         : void
//! Return        : void
//! Create        : , 2014/06/02
//! Comment       : 
//******************************************************************/
CKhOpenApiTestApp::CKhOpenApiTestApp()
{
	// 다시 시작 관리자 지원
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

	// TODO: 여기에 생성 코드를 추가합니다.
	// InitInstance에 모든 중요한 초기화 작업을 배치합니다.
}


// 유일한 CKhOpenApiTestApp 개체입니다.
CKhOpenApiTestApp theApp;

//*******************************************************************/
//! Function Name : InitInstance
//! Function      : 초기화
//! Param         : void
//! Return        : BOOL
//! Create        : , 2014/06/02
//! Comment       : CKhOpenApiTestApp 초기화
//******************************************************************/
BOOL CKhOpenApiTestApp::InitInstance()
{
	// 응용 프로그램 매니페스트가 ComCtl32.dll 버전 6 이상을 사용하여 비주얼 스타일을
	// 사용하도록 지정하는 경우, Windows XP 상에서 반드시 InitCommonControlsEx()가 필요합니다.
	// InitCommonControlsEx()를 사용하지 않으면 창을 만들 수 없습니다.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// 응용 프로그램에서 사용할 모든 공용 컨트롤 클래스를 포함하도록
	// 이 항목을 설정하십시오.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();


	AfxEnableControlContainer();

	// 대화 상자에 셸 트리 뷰 또는
	// 셸 목록 뷰 컨트롤이 포함되어 있는 경우 셸 관리자를 만듭니다.
	CShellManager *pShellManager = new CShellManager;

	// 표준 초기화
	// 이들 기능을 사용하지 않고 최종 실행 파일의 크기를 줄이려면
	// 아래에서 필요 없는 특정 초기화
	// 루틴을 제거해야 합니다.
	// 해당 설정이 저장된 레지스트리 키를 변경하십시오.
	// TODO: 이 문자열을 회사 또는 조직의 이름과 같은
	// 적절한 내용으로 수정해야 합니다.
	SetRegistryKey(_T("로컬 응용 프로그램 마법사에서 생성된 응용 프로그램"));

	//////////////////////////////////////////////////////////////////
	// 실행파일의 패스를 저장한다.
	char szPath[MAX_PATH];
	GetModuleFileName(AfxGetInstanceHandle(), szPath, MAX_PATH);
	*strrchr(szPath,'\\') = '\0';
	m_sAppPath = szPath;

	CString strFileName = m_sAppPath + "/data";
	if (!::PathIsDirectory(strFileName))		// 폴더가 없다면
	{
		::CreateDirectory(strFileName, NULL);
	}

	CKhOpenApiTestDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
//	if (nResponse == IDOK)
//	{
//		// TODO: 여기에 [확인]을 클릭하여 대화 상자가 없어질 때 처리할
//		//  코드를 배치합니다.
//	}
//	else if (nResponse == IDCANCEL)
//	{
//		// TODO: 여기에 [취소]를 클릭하여 대화 상자가 없어질 때 처리할
//		//  코드를 배치합니다.
//	}

	// 위에서 만든 셸 관리자를 삭제합니다.
	if (pShellManager != NULL)
	{
		delete pShellManager;
	}

	// 대화 상자가 닫혔으므로 응용 프로그램의 메시지 펌프를 시작하지 않고  응용 프로그램을 끝낼 수 있도록 FALSE를
	// 반환합니다.
	return FALSE;
}

//*******************************************************************/
//! Function Name : ConvDataFormat
//! Function      : 문자열 포멧 설정
//! Param         : int nType, LPCTSTR szData, LPCTSTR szBeforeData/* = ""*/, LPCTSTR szAfterData/* = ""*/
//! Return        : CString
//! Create        : , 2014/06/02
//! Comment       : 
//******************************************************************/
CString CKhOpenApiTestApp::ConvDataFormat(int nType, LPCTSTR szData, LPCTSTR szBeforeData/* = ""*/, LPCTSTR szAfterData/* = ""*/)
{
	CString strReturn, strData, strTemp = strData = szData;
	switch (nType)
	{
		case DT_DATE:			// 일자
			{
				if (strTemp.GetLength() == 6)
				{
					strData.Format("%02s/%02s/%02s", strTemp.Left(2), strTemp.Mid(2, 2), strTemp.Right(2));
				}
				else if (strTemp.GetLength() == 8)
				{
					strData.Format("%04s/%02s/%02s", strTemp.Left(4), strTemp.Mid(4, 2), strTemp.Right(2));
				}
			}
			break;
		case DT_TIME:			// 시간
			{
				if (strTemp.GetLength() == 6)
				{
					strData.Format("%02s:%02s:%02s", strTemp.Left(2), strTemp.Mid(2, 2), strTemp.Right(2));
				}
				else if (strTemp.GetLength() == 8)
				{
					strData.Format("%02s:%02s:%02s:%02s", strTemp.Left(2), strTemp.Mid(2, 2), strTemp.Mid(4, 2), strTemp.Right(2));
				}
			}
			break;
		case DT_NUMBER:			// 숫자
		case DT_ZERO_NUMBER:	// 숫자(0표시)
			{
				strTemp.Replace("+", "");
				if (atof(strTemp) == 0.00)
				{
					strData = nType == DT_ZERO_NUMBER ? strTemp : "";
					break;
				}

				int nFind = strTemp.Find(".");
				if (nFind < 0)
				{
					strData = strTemp;
				}
				else
				{
					strData = strTemp.Left(nFind);
					strTemp = strTemp.Mid(nFind);
				}

				TCHAR szFormatData[1024] = {0,};
				NUMBERFMT fmt={0, 0, 3, _T("."), _T(","), 1};
				::GetNumberFormat(NULL, 0, strData, &fmt, szFormatData, 1024);

				if (nFind < 0)
				{
					nType == DT_ZERO_NUMBER ? strData.Format("%01s", szFormatData) : strData.Format("%s", szFormatData);
				}
				else
				{
					strData.Format("%01s%s", szFormatData, strTemp);
				}
			}
			break;
	}

	strReturn.Format("%s%s%s", szBeforeData, strData, szAfterData);

	return strReturn;
}

//*******************************************************************/
//! Function Name : SetSignData
//! Function      : 그리드 셀 문자색 변경 및 기호 데이터 설정
//! Param         : CGridCtrl *pGrid, int nRow, int nCol, LPCTSTR szData
//! Return        : void
//! Create        : , 2014/06/02
//! Comment       : 
//******************************************************************/
void CKhOpenApiTestApp::SetSignData(CGridCtrl *pGrid, int nRow, int nCol, LPCTSTR szData)
{
	CString strData = szData;
	switch (atoi(strData))
	{
		case 1:
			strData = "↑";
			pGrid->SetItemFgColour(nRow, nCol, RGB(255, 0, 0));	// 지정된 셀의 텍스트 색상 설정
			break;
		case 2:
			strData = "▲";
			pGrid->SetItemFgColour(nRow, nCol, RGB(255, 0, 0));	// 지정된 셀의 텍스트 색상 설정
			break;
		case 3:	strData = "";		break;
		case 4:
			strData = "↓";
			pGrid->SetItemFgColour(nRow, nCol, RGB(0, 0, 255));	// 지정된 셀의 텍스트 색상 설정
			break;
		case 5:
			strData = "▼";	
			pGrid->SetItemFgColour(nRow, nCol, RGB(0, 0, 255));	// 지정된 셀의 텍스트 색상 설정
			break;
	}
	pGrid->SetItemText(nRow, nCol, strData);
}

//*******************************************************************/
//! Function Name : SetDataFgColour
//! Function      : 그리드 셀 문자색 변경
//! Param         : CGridCtrl *pGrid, int nRow, int nCol, LPCTSTR szData
//! Return        : void
//! Create        : , 2014/06/02
//! Comment       : 
//******************************************************************/
void CKhOpenApiTestApp::SetDataFgColour(CGridCtrl *pGrid, int nRow, int nCol, LPCTSTR szData)
{
	CString strData = szData;
	if (atof(strData) > 0)
	{
		pGrid->SetItemFgColour(nRow, nCol, RGB(255, 0, 0));	// 지정된 셀의 텍스트 색상 설정
	}
	else if (atof(strData) < 0)
	{
		pGrid->SetItemFgColour(nRow, nCol, RGB(0, 0, 255));	// 지정된 셀의 텍스트 색상 설정
	}
}

//*******************************************************************/
//! Function Name : IsError
//! Function      : 오류 여부
//! Param         : long lErrCode
//! Return        : BOOL
//! Create        : , 2014/06/02
//! Comment       : 
//******************************************************************/
BOOL CKhOpenApiTestApp::IsError(long lErrCode)
{
	CString strMsg = _T("");
	switch (lErrCode)
	{
		////////////////////////////
		// 일반 조회 오류 처리
		case OP_ERR_SISE_OVERFLOW:		// 시세조회 과부하
			strMsg = "시세조회 과부하";
			break;
		case OP_ERR_RQ_STRUCT_FAIL:		// REQUEST_INPUT_st Failed
			strMsg = "REQUEST_INPUT_st Failed";
			break;
		case OP_ERR_RQ_STRING_FAIL:		// 요청 전문 작성 실패
			strMsg = "요청 전문 작성 실패";
			break;
		case OP_ERR_NO_DATA:			// 데이터가 존재하지 않습니다.
			strMsg = "데이터가 존재하지 않습니다.";
			break;

		////////////////////////////
		// 복수 종목 조회 오류 처리
		case OP_ERR_OVER_MAX_DATA:			// 한번에 조회 가능한 종목개수는 최대 100종목 입니다.
			strMsg = "한번에 조회 가능한 종목개수는 최대 100종목 입니다.";
			break;

		////////////////////////////
		// 주문 오류 처리
		case OP_ERR_ORD_WRONG_INPUT:		// 입력값 오류
			strMsg = "입력값 오류";
			break;
		case OP_ERR_ORD_WRONG_ACCNO:		// 계좌비밀번호를 입력하십시오.
			strMsg = "계좌비밀번호를 입력하십시오.";
			break;
		case OP_ERR_OTHER_ACC_USE:			// 타인계좌는 사용할 수 없습니다.
			strMsg = "타인계좌는 사용할 수 없습니다.";
			break;
		case OP_ERR_MIS_2BILL_EXC:			// 주문가격이 20억원을 초과합니다.
			strMsg = "주문가격이 20억원을 초과합니다.";
			break;
		case OP_ERR_MIS_5BILL_EXC:			// 주문가격이 50억원을 초과합니다.
			strMsg = "주문가격이 50억원을 초과합니다.";
			break;
		case OP_ERR_MIS_1PER_EXC:			// 주문수량이 총발행주수의 1%를 초과합니다.
			strMsg = "주문수량이 총발행주수의 1%를 초과합니다.";
			break;
		case OP_ERR_MIS_3PER_EXC:			// 주문수량은 총발행주수의 3%를 초과할 수 없습니다.
			strMsg = "주문수량은 총발행주수의 3%를 초과할 수 없습니다.";
			break;
		case OP_ERR_SEND_FAIL:				// 주문전송실패
			strMsg = "주문전송에 실패하였습니다..";
			break;
		case OP_ERR_ORD_OVERFLOW:			// 주문전송 과부하
			strMsg = "주문전송 과부하 입니다. 잠시 후 다시 주문하여 주세요.";
			break;
	}

	// 에러 메세지 처리
	if (!strMsg.IsEmpty())
	{
		AfxMessageBox(strMsg);
		return FALSE;
	}

	return TRUE;
}