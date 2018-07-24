//******************************************************************/
//******************************************************************/
//! All Rights Reserved. Copyright(c)   2014 (주)한국소리마치        /
//******************************************************************/
//! File Name     : KhOpenApiTestDlg.cpp
//! Function      : 키움 오픈API 테스트 다이얼로그
//! System Name   : 키움 오픈API 테스트
//! Create        : , 2014/06/02
//! Update        : 
//! Comment       : 
//******************************************************************/
// KhOpenApiTestDlg.cpp : 구현 파일
//

#include "stdafx.h"
#include "KhOpenApiTest.h"
#include "KhOpenApiTestDlg.h"
#include "afxdialogex.h"

#include "CurrentPriceDlg.h"
#include "OrderDlg.h"
#include "KwanSimDlg.h"
#include "RealAddDlg.h"
#include "RateDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CKhOpenApiTestDlg 대화 상자
//*******************************************************************/
//! Function Name : CKhOpenApiTestDlg::CKhOpenApiTestDlg(CWnd* pParent /*=NULL*/) : CDialogEx(CKhOpenApiTestDlg::IDD, pParent)
//! Function      : 생성 초기 처리
//! Param         : HANDLE hBitmap, int bits
//! Return        : void
//! Create        : , 2014/06/02
//! Comment       : 
//******************************************************************/
CKhOpenApiTestDlg::CKhOpenApiTestDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CKhOpenApiTestDlg::IDD, pParent)
{
	m_nRet = 0;
	m_nScrN0 = 0;
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

//*******************************************************************/
// Function Name : DoDataExchange
// Function      : DDX/DDV에 관련 된 함수 호출
// Param         : CDataExchange*	pDX
// Return        : void
// Create        : , 2014/06/02
// Comment       : DDX - 컨트롤과 멤버변수사이에 정보를 교환하는 역할
//				 : DDV - 멤버변수에 저장 될 값의 유효성 체크
//				 : UapdateData()함수로 간접 호출 할 수 있음
//*******************************************************************/
void CKhOpenApiTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CKhOpenApiTestDlg)
	DDX_Control(pDX, IDC_KHOPENAPICTRL1, theApp.m_khOpenApi);
	//}}AFX_DATA_MAP
}

//*******************************************************************/
// BEGIN_MESSAGE_MAP
//*******************************************************************/
BEGIN_MESSAGE_MAP(CKhOpenApiTestDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_DESTROY()
	ON_WM_QUERYDRAGICON()
	ON_MESSAGE(UM_SCRENN_CLOSE,			OnScreenClose)
	ON_BN_CLICKED(IDC_BTN_CURRENTPRICE,	OnBtnCurrentPrice)
	ON_BN_CLICKED(IDC_BTN_ORDER,		OnBtnOrder)
	ON_BN_CLICKED(IDC_BTN_KWANSIM,		OnBtnKwanSim)
	ON_BN_CLICKED(IDC_BTN_CURRENTPRICE2, OnBnClickedBtnCurrentprice2)
	ON_BN_CLICKED(IDC_BTN_CON, OnBnClickedBtnReal)
	ON_BN_CLICKED(IDC_BUTTON1, &CKhOpenApiTestDlg::OnBnClickedButton1)
END_MESSAGE_MAP()

//*******************************************************************/
// BEGIN_EVENTSINK_MAP
//*******************************************************************/
BEGIN_EVENTSINK_MAP(CKhOpenApiTestDlg, CDialogEx)
	ON_EVENT(CKhOpenApiTestDlg, IDC_KHOPENAPICTRL1, 1, OnReceiveTrDataKhopenapictrl,	VTS_BSTR VTS_BSTR VTS_BSTR VTS_BSTR VTS_BSTR VTS_I4 VTS_BSTR VTS_BSTR VTS_BSTR)
	ON_EVENT(CKhOpenApiTestDlg, IDC_KHOPENAPICTRL1, 2, OnReceiveRealDataKhopenapictrl,	VTS_BSTR VTS_BSTR VTS_BSTR)
	ON_EVENT(CKhOpenApiTestDlg, IDC_KHOPENAPICTRL1, 3, OnReceiveMsgKhopenapictrl,		VTS_BSTR VTS_BSTR VTS_BSTR VTS_BSTR )
	ON_EVENT(CKhOpenApiTestDlg, IDC_KHOPENAPICTRL1, 4, OnReceiveChejanData,				VTS_BSTR VTS_I4 VTS_BSTR)
	ON_EVENT(CKhOpenApiTestDlg, IDC_KHOPENAPICTRL1, 5, OnEventConnect,					VTS_I4)
	ON_EVENT(CKhOpenApiTestDlg, IDC_KHOPENAPICTRL1, 7, OnReceiveRealCondition,			VTS_BSTR VTS_BSTR VTS_BSTR VTS_BSTR)
	ON_EVENT(CKhOpenApiTestDlg, IDC_KHOPENAPICTRL1, 8, OnReceiveTrCondition,			VTS_BSTR VTS_BSTR VTS_BSTR VTS_I2 VTS_I2)
	ON_EVENT(CKhOpenApiTestDlg, IDC_KHOPENAPICTRL1, 9, OnReceiveConditionVer,			VTS_I4 VTS_BSTR)
END_EVENTSINK_MAP()

//*******************************************************************/
//! Function Name : OnInitDialog
//! Function      : 생성 초기 처리
//! Param         : void
//! Return        : BOOL
//! Create        : , 2014/06/02
//! Comment       : CKhOpenApiTestDlg 메시지 처리기
//******************************************************************/
BOOL CKhOpenApiTestDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 이 대화 상자의 아이콘을 설정합니다. 응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	CString		strTitle;
	GetWindowText(strTitle);
	strTitle += SAMPLE_VERSION;
	SetWindowText(strTitle);
	// TODO: 여기에 추가 초기화 작업을 추가합니다.
	// OpenApi 접속 처리(로그인 창 실행)
	theApp.m_khOpenApi.CommConnect();

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

//*******************************************************************/
//! Function Name : OnPaint
//! Function      : 그리기 처리
//! Param         : void
//! Return        : void
//! Create        : , 2014/06/02
//! Comment       : 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//!               : 아래 코드가 필요합니다. 문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
//!               : 프레임워크에서 이 작업을 자동으로 수행합니다.
//******************************************************************/
void CKhOpenApiTestDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//*******************************************************************/
//! Function Name : OnDestroy
//! Function      : 파괴자
//! Param         : void
//! Return        : void
//! Create        : , 2014/06/02
//! Comment       : 
//******************************************************************/
void CKhOpenApiTestDlg::OnDestroy()
{
	CString strKey;
	CWnd *pWnd = NULL;
	POSITION pos = m_mapScreen.GetStartPosition();
	while (pos != NULL)
	{
		pWnd = NULL;
		m_mapScreen.GetNextAssoc(pos, strKey, (void *&)pWnd);
		if (pWnd)
		{
			delete pWnd;
			pWnd = NULL;
		}
	}
	m_mapScreen.RemoveAll();
	m_mapScreenNum.RemoveAll();

	// 통신 종료 처리
	theApp.m_khOpenApi.CommTerminate();
}

//*******************************************************************/
//! Function Name : OnQueryDragIcon
//! Function      : 드래그 아이콘 처리
//! Param         : void
//! Return        : HCURSOR
//! Create        : , 2014/06/02
//! Comment       : 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서 
//!               : 이 함수를 호출합니다.
//******************************************************************/
HCURSOR CKhOpenApiTestDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

//*******************************************************************/
//! Function Name : OnScreenClose
//! Function      : 화면이 닫힌 후 처리
//! Param         : WPARAM wParam, LPARAM lParam
//! Return        : LRESULT
//! Create        : , 2014/06/02
//! Comment       : 
//******************************************************************/
LRESULT CKhOpenApiTestDlg::OnScreenClose(WPARAM wParam, LPARAM lParam)
{
	switch (wParam)
	{
		case 0:
			{
				char *cScrNo = (char*)lParam;
				CString strKey, strScrType;
				strKey.Format("%s", cScrNo);
				delete cScrNo;

				if (m_mapScreenNum.Lookup(strKey, strScrType))
				{
					m_mapScreenNum.RemoveKey(strKey);
				}

				CWnd *pWnd = NULL;
				if (m_mapScreen.Lookup(strKey, (void *&)pWnd) && pWnd)
				{
					delete pWnd;
					pWnd = NULL;
				}
				m_mapScreen.RemoveKey(strKey);

				if (theApp.m_khOpenApi.GetSafeHwnd())
				{
					theApp.m_khOpenApi.DisconnectRealData(strKey);
				}
			}
			break;
	}

	return 0L;
}

//*******************************************************************/
//! Function Name : OnBtnCurrentPrice
//! Function      : 현재가 버튼
//! Param         : void
//! Return        : void
//! Create        : , 2014/06/02
//! Comment       : 
//******************************************************************/
void CKhOpenApiTestDlg::OnBtnCurrentPrice()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (!GetNextScreenNum(0))
	{
		return;
	}

	CCurrentPriceDlg *pCurrentPriceDlg = new CCurrentPriceDlg(this);
	pCurrentPriceDlg->m_strScrNo.Format("%04d", m_nScrN0);
	pCurrentPriceDlg->Create(IDD_CURRENTPRICE_DLG);

	m_mapScreen.SetAt(pCurrentPriceDlg->m_strScrNo, pCurrentPriceDlg);

	// 2개이사의 서비스를 조회 시 m_mapScreen에 화면을 등록하기 전에 
	// 데이터 응답이 먼저 와서 처리가 불가능 하여 여기서 조회 처리 함.
	pCurrentPriceDlg->SendSearch();		// 조회 처리
}

//*******************************************************************/
//! Function Name : OnBtnOrder
//! Function      : 주문 버튼
//! Param         : void
//! Return        : void
//! Create        : , 2014/06/02
//! Comment       : 
//******************************************************************/
void CKhOpenApiTestDlg::OnBtnOrder()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (!GetNextScreenNum(1))
	{
		return;
	}

	COrderDlg *pOrderDlg = new COrderDlg(this);
	pOrderDlg->m_strScrNo.Format("%04d", m_nScrN0);
	pOrderDlg->Create(IDD_ORDER_DLG);

	m_mapScreen.SetAt(pOrderDlg->m_strScrNo, pOrderDlg);
}

//*******************************************************************/
//! Function Name : OnBtnKwanSim
//! Function      : 관심 버튼
//! Param         : void
//! Return        : void
//! Create        : , 2014/06/02
//! Comment       : 
//******************************************************************/
void CKhOpenApiTestDlg::OnBtnKwanSim()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (!GetNextScreenNum(2))
	{
		return;
	}

	CKwanSimDlg *pKwanSimDlg = new CKwanSimDlg(this);
	pKwanSimDlg->m_strScrNo.Format("%04d", m_nScrN0);
	pKwanSimDlg->Create(IDD_KWANSIM_DLG);

	m_mapScreen.SetAt(pKwanSimDlg->m_strScrNo, pKwanSimDlg);

	// 2개이사의 서비스를 조회 시 m_mapScreen에 화면을 등록하기 전에 
	// 데이터 응답이 먼저 와서 처리가 불가능 하여 여기서 조회 처리 함.
	pKwanSimDlg->SetKwanSimJong();		// 관심 그리드에 종목 설정 처리
}

//*******************************************************************/
//! Function Name : OnReceiveTrDataKhopenapictrl
//! Function      : 조회 응답 처리
//! Param         : LPCTSTR sScrNo, LPCTSTR sRQName, LPCTSTR sTrcode, LPCTSTR sRecordName, LPCTSTR sPrevNext, long nDataLength, LPCTSTR sErrorCode, LPCTSTR sMessage, LPCTSTR sSplmMsg
//! Return        : void
//! Create        : , 2014/06/02
//! Comment       : 
//******************************************************************/
void CKhOpenApiTestDlg::OnReceiveTrDataKhopenapictrl(LPCTSTR sScrNo, LPCTSTR sRQName, LPCTSTR sTrcode, LPCTSTR sRecordName, LPCTSTR sPrevNext, long nDataLength, LPCTSTR sErrorCode, LPCTSTR sMessage, LPCTSTR sSplmMsg)
{
	if (!theApp.m_khOpenApi.GetSafeHwnd())
	{
		return;
	}

	CString strScrType, strKey = sScrNo;
	if (!m_mapScreenNum.Lookup(strKey, strScrType))
	{
		return;
	}

	CWnd *pWnd = NULL;
	if (m_mapScreen.Lookup(strKey, (void *&)pWnd) && pWnd)
	{
		switch (atoi(strScrType))
		{
			case 0:		// 현재가
				{
					((CCurrentPriceDlg *)pWnd)->OnReceiveTrDataKhopenapictrl(sScrNo, sRQName, sTrcode, sRecordName, sPrevNext, nDataLength, sErrorCode, sMessage, sSplmMsg);
				}
				break;
			case 1:		// 주문
				{
					((COrderDlg *)pWnd)->OnReceiveTrDataKhopenapictrl(sScrNo, sRQName, sTrcode, sRecordName, sPrevNext, nDataLength, sErrorCode, sMessage, sSplmMsg);
				}
				break;
			case 2:		// 관심종목
				{
					((CKwanSimDlg *)pWnd)->OnReceiveTrDataKhopenapictrl(sScrNo, sRQName, sTrcode, sRecordName, sPrevNext, nDataLength, sErrorCode, sMessage, sSplmMsg);
				}
				break;
			case 3:		// 조건검색
				{
					((CRealAddDlg *)pWnd)->OnReceiveTrDataKhopenapictrl(sScrNo, sRQName, sTrcode, sRecordName, sPrevNext, nDataLength, sErrorCode, sMessage, sSplmMsg);
				}
				break;
			case 4:		// 수익률
				{
					((CRateDlg *)pWnd)->OnReceiveTrDataKhopenapictrl(sScrNo, sRQName, sTrcode, sRecordName, sPrevNext, nDataLength, sErrorCode, sMessage, sSplmMsg);
				}
				break;
		}
	}
}

//*******************************************************************/
//! Function Name : OnReceiveMsgKhopenapictrl
//! Function      : 조회 에러 처리
//! Param         : LPCTSTR sScrNo, LPCTSTR sRQName, LPCTSTR sTrCode, LPCTSTR sMsg
//! Return        : void
//! Create        : , 2014/06/02
//! Comment       : 
//******************************************************************/
void CKhOpenApiTestDlg::OnReceiveMsgKhopenapictrl(LPCTSTR sScrNo, LPCTSTR sRQName, LPCTSTR sTrCode, LPCTSTR sMsg)
{
	if (!theApp.m_khOpenApi.GetSafeHwnd())
	{
		return;
	}

	CString strRQ = sRQName;
	CString strScrType, strKey = sScrNo;
	if (!m_mapScreenNum.Lookup(strKey, strScrType))
	{
		return;
	}

	CWnd *pWnd = NULL;
	if (m_mapScreen.Lookup(strKey, (void *&)pWnd) && pWnd)
	{
		CString strData;
		strData.Format(_T("[%s] [%s] 오류"), sRQName, sTrCode);

		CString strRQName = strRQ.Mid(4);
		switch (atoi(strScrType))
		{
			case 0:		// 현재가
				{
					((CCurrentPriceDlg *)pWnd)->OnReceiveMsgKhopenapictrl(sScrNo, strRQName, sTrCode, sMsg);
					pWnd->MessageBox(sMsg, strData, MB_ICONERROR | MB_OK);
				}
				break;
			case 1:		// 주문
				{
					((COrderDlg *)pWnd)->OnReceiveMsgKhopenapictrl(sScrNo, strRQName, sTrCode, sMsg);
				}
				break;
			case 2:		// 관심종목
				{
					((CKwanSimDlg *)pWnd)->OnReceiveMsgKhopenapictrl(sScrNo, strRQName, sTrCode, sMsg);
					pWnd->MessageBox(sMsg, strData, MB_ICONERROR | MB_OK);
				}
				break;
			case 3:		// 조건검색
				{
					((CRealAddDlg *)pWnd)->OnReceiveMsgKhopenapictrl(sScrNo, strRQName, sTrCode, sMsg);
					pWnd->MessageBox(sMsg, strData, MB_ICONERROR | MB_OK);
				}
				break;
			case 4:	// 수익률
				{
					((CRateDlg *)pWnd)->OnReceiveMsgKhopenapictrl(sScrNo, strRQName, sTrCode, sMsg);
				}
				break;
		}
	}
}

//*******************************************************************/
//! Function Name : OnReceiveRealDataKhopenapictrl
//! Function      : 실시간 처리
//! Param         : LPCTSTR sJongmokCode, LPCTSTR sRealType, LPCTSTR sRealData
//! Return        : void
//! Create        : , 2014/06/02
//! Comment       : 
//******************************************************************/
void CKhOpenApiTestDlg::OnReceiveRealDataKhopenapictrl(LPCTSTR sJongmokCode, LPCTSTR sRealType, LPCTSTR sRealData)
{
	if (!theApp.m_khOpenApi.GetSafeHwnd())
	{
		return;
	}

	CWnd *pWnd;
	CString strKey, strScrType;
	POSITION pos = m_mapScreenNum.GetStartPosition();
	while (pos != NULL)
	{
		pWnd = NULL;
		m_mapScreenNum.GetNextAssoc(pos, strKey, strScrType);
		if (m_mapScreen.Lookup(strKey, (void *&)pWnd) && pWnd)
		{
			switch (atoi(strScrType))
			{
				case 0:		// 현재가
					{
						((CCurrentPriceDlg *)pWnd)->OnReceiveRealDataKhopenapictrl(sJongmokCode, sRealType, sRealData);
					}
					break;
				case 1:		// 주문
					{
						((COrderDlg *)pWnd)->OnReceiveRealDataKhopenapictrl(sJongmokCode, sRealType, sRealData);
					}
					break;
				case 2:		// 관심종목
					{
						((CKwanSimDlg *)pWnd)->OnReceiveRealDataKhopenapictrl(sJongmokCode, sRealType, sRealData);
					}
					break;
				case 3:		// 조건검색
					{
						((CRealAddDlg *)pWnd)->OnReceiveRealDataKhopenapictrl(sJongmokCode, sRealType, sRealData);
					}
					break;
				case 4:	// 수익률
					{
						((CRateDlg *)pWnd)->OnReceiveRealDataKhopenapictrl(sJongmokCode, sRealType, sRealData);
					}
					break;
			}
		}
	}

	// 장운영 정보 처리
	if (!strcmp(sRealType, "장시작시간"))
	{
		int i;
		CString strData[3], strMsg;
		// 0: 장운영구분, 1:시간, 2:장시작 예상잔여시간
		for (i = 0; i < 3; i++)
		{
			strData[i] = theApp.m_khOpenApi.GetCommRealData(sJongmokCode, i);	strData[i].Trim();
		}
		strMsg.Format("장운영구분 : %s\r\n시간 : %s\r\n예상잔여시간 : %s", strData[0], strData[1], strData[2]);

		if (atoi(strData[2]) == 10)
		{
			ReplyMessage(S_OK);		// SendMessage 풀림
			MessageBox(strMsg, "장운영 정보", MB_ICONINFORMATION | MB_OK);
		}
	}
}

//*******************************************************************/
//! Function Name : OnReceiveChejanData
//! Function      : 체결잔고 실시간 처리
//! Param         : LPCTSTR sGubun, LONG nItemCnt, LPCTSTR sFidList
//! Return        : void
//! Create        : , 2014/06/02
//! Comment       : 
//******************************************************************/
void CKhOpenApiTestDlg::OnReceiveChejanData(LPCTSTR sGubun, LONG nItemCnt, LPCTSTR sFidList)
{
	if (!theApp.m_khOpenApi.GetSafeHwnd())
	{
		return;
	}

	CWnd *pWnd;
	CString strKey, strScrType;
	POSITION pos = m_mapScreenNum.GetStartPosition();
	while (pos != NULL)
	{
		pWnd = NULL;
		m_mapScreenNum.GetNextAssoc(pos, strKey, strScrType);
		if (m_mapScreen.Lookup(strKey, (void *&)pWnd) && pWnd)
		{
			switch (atoi(strScrType))
			{
				case 0:		// 현재가
					{
						((CCurrentPriceDlg *)pWnd)->OnReceiveChejanData(sGubun, nItemCnt, sFidList);
					}
					break;
				case 1:		// 주문
					{
						((COrderDlg *)pWnd)->OnReceiveChejanData(sGubun, nItemCnt, sFidList);
					}
					break;
				case 2:		// 관심종목
					{
						((CKwanSimDlg *)pWnd)->OnReceiveChejanData(sGubun, nItemCnt, sFidList);
					}
					break;
				case 4:		// 수익률
					{
						((CRateDlg *)pWnd)->OnReceiveChejanData(sGubun, nItemCnt, sFidList);
					}
					break;
			}
		}
	}
}

//*******************************************************************/
//! Function Name : OnReceiveRealCondition
//! Function      : 조건검색 실시간 종목 삽입/삭제
//! Param         : LPCTSTR strCode, LPCTSTR strType, LPCTSTR strConditionName, LPCTSTR strConditionIndex
//! Return        : void
//! Create        : , 2015/04/20
//! Comment       : 
//******************************************************************/
void CKhOpenApiTestDlg::OnReceiveRealCondition(LPCTSTR strCode, LPCTSTR strType, LPCTSTR strConditionName, LPCTSTR strConditionIndex)
{
	if (!theApp.m_khOpenApi.GetSafeHwnd())
	{
		return;
	}

	CWnd *pWnd;
	CString strKey, strScrType;
	POSITION pos = m_mapScreenNum.GetStartPosition();
	while (pos != NULL)
	{
		pWnd = NULL;
		m_mapScreenNum.GetNextAssoc(pos, strKey, strScrType);
		if (m_mapScreen.Lookup(strKey, (void *&)pWnd) && pWnd)
		{
			switch (atoi(strScrType))
			{
			case 3:		// 조건검색
				{
					((CRealAddDlg *)pWnd)->OnReceiveRealCondition(strCode, strType, strConditionName, strConditionIndex);
				}
				break;
			}
		}
	}
}

//*******************************************************************/
//! Function Name	: OnReceiveTrCondition
//! Function			: 조건검색 종목조회 응답
//! Param				: LPCTSTR sScrNo					- 화면번호
//!						: LPCTSTR strCodeList			- 종목리스트
//!						: LPCTSTR strConditionName	- 조건명
//!						: int nIndex								- 조건명인덱스
//!						: int nNext								- 연속조회여부(2: 연속조회, 0:연속조회없음)
//! Return        : void
//! Create        : , 2015/04/20
//! Comment       : 
//******************************************************************/
void CKhOpenApiTestDlg::OnReceiveTrCondition(LPCTSTR sScrNo, LPCTSTR strCodeList, LPCTSTR strConditionName, int nIndex, int nNext)
{
	if (!theApp.m_khOpenApi.GetSafeHwnd())
	{
		return;
	}

	CString strScrType, strKey = sScrNo;
	if (!m_mapScreenNum.Lookup(strKey, strScrType))
	{
		return;
	}

	CWnd *pWnd = NULL;
	if (m_mapScreen.Lookup(strKey, (void *&)pWnd) && pWnd)
	{
		switch (atoi(strScrType))
		{
		case 3:	//조건검색
			{
				((CRealAddDlg *)pWnd)->OnReceiveTrCondition(sScrNo, strCodeList, strConditionName, nIndex, nNext);
			}
			break;
		}
	}
}

//*******************************************************************/
//! Function Name	: OnReceiveConditionVer
//! Function			: 사용자 조건식 응답
//! Param				: BOOL bRet							- 성공(TRUE), 실패(FALSE)
//!						: LPCTSTR sMsg					- 메세지(reserved)
//! Return        : void
//! Create        : , 2015/04/20
//! Comment       : 
//******************************************************************/
void CKhOpenApiTestDlg::OnReceiveConditionVer(long lRet, LPCTSTR sMsg)
{
	if (!theApp.m_khOpenApi.GetSafeHwnd())
	{
		return;
	}

	if(lRet != 1)
	{// 사용자 조건검색식 서버요청중 에러가 발생한 경우...
		CString		strErr; strErr.Format(_T("%s [%d]"), sMsg, lRet);
		AfxMessageBox(strErr);
		return;
	}

	int nScrNo = GetNextScreenNum(3);
	CString strScrNo;
	strScrNo.Format("%04d", nScrNo);

	CString strScrType, strKey = strScrNo;
	if (!m_mapScreenNum.Lookup(strKey, strScrType))
	{
		return;
	}

	CWnd *pWnd = NULL;
	if (m_mapScreen.Lookup(strKey, (void *&)pWnd) && pWnd)
	{
		switch (atoi(strScrType))
		{
		case 3:	//조건검색
			{
				((CRealAddDlg *)pWnd)->OnReceiveConditionVer(lRet, sMsg);
			}
			break;
		}
	}
}

//*******************************************************************/
//! Function Name : OnEventConnect
//! Function      : 접속 통보 처리
//! Param         : LONG nItemCnt
//! Return        : void
//! Create        : , 2014/06/02
//! Comment       : 
//******************************************************************/
void CKhOpenApiTestDlg::OnEventConnect(LONG nItemCnt)
{
	if (nItemCnt == 0)
	{
		// 접속 정상처리
	}
	else
	{
		// 접속 비정상 처리
		EndDialog(IDCANCEL);
	}
}

//*******************************************************************/
//! Function Name : GetNextScreenNum
//! Function      : 다음 화면 번호 취득
//! Param         : int nScreenType
//! Return        : BOOL
//! Create        : , 2014/06/02
//! Comment       : 
//******************************************************************/
BOOL CKhOpenApiTestDlg::GetNextScreenNum(int nScreenType)
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	static int nRepeat = 0;
	m_nScrN0++;
	if (m_nScrN0 > 9999)
	{
		nRepeat++;
		m_nScrN0 = 1;
	}

	if (nRepeat > 1)
	{
		nRepeat = 0;
		AfxMessageBox("더 이상 화면을 열수 없습니다.다른 화면을 닫고 실행 해 주세요~!");
		return FALSE;
	}

	CString strKey, strTemp;
	strKey.Format("%04d", m_nScrN0);
	if (m_mapScreenNum.Lookup(strKey, strTemp))
	{
		return GetNextScreenNum(nScreenType);
	}

	nRepeat = 0;
	strTemp.Format("%d", nScreenType);
	m_mapScreenNum.SetAt(strKey, strTemp);
	return TRUE;
}


void CKhOpenApiTestDlg::OnBnClickedBtnCurrentprice2()
{
	CString strTemp = "";
	((CEdit*)GetDlgItem(IDC_EDT_JONGCODE2))->GetWindowText(strTemp);
	if (strTemp.GetLength() != 6)
		return;

	CString strRe = theApp.m_khOpenApi.GetMasterStockState(strTemp);
	AfxMessageBox(strRe);
}

void CKhOpenApiTestDlg::OnBnClickedBtnReal()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (!GetNextScreenNum(3))
	{
		return;
	}

	// 화면 오픈시 서버에 저장된 조건식을 받아온다.
	// 이 함수를 호출하지 않으면 이후 조건명리스트를 불러올수가 없으니 조건 검색을 할 경우
	// 무조건 이 함수를 처음에 불러와야 한다.
	// 조건검색을 시작하려면 한번은 꼭 호출해야한다.
	m_nRet = theApp.m_khOpenApi.GetConditionLoad();

	if (m_nRet > 0)
	{
		CRealAddDlg *pRealAddDlg = new CRealAddDlg(this);
		pRealAddDlg->m_strScrNo.Format("%04d", m_nScrN0);
		pRealAddDlg->Create(IDD_CON_SET);

		m_mapScreen.SetAt(pRealAddDlg->m_strScrNo, pRealAddDlg);
	}
}

void CKhOpenApiTestDlg::OnBnClickedButton1()
{
	if (!GetNextScreenNum(4))
	{
		return;
	}

	CRateDlg *pRateDlg = new CRateDlg(this);
	pRateDlg->m_strScrNo.Format(_T("%04d"), m_nScrN0);
	pRateDlg->Create(IDD_RATE);

	m_mapScreen.SetAt(pRateDlg->m_strScrNo, pRateDlg);
}