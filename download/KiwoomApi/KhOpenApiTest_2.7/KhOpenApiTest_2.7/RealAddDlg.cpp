//******************************************************************/
//******************************************************************/
//! All Rights Reserved. Copyright(c)   2015 (주)한국소리마치                /
//******************************************************************/
//! File Name     : RealAddDlg.cpp
//! Function      : 리얼등록 다이얼로그
//! System Name   : 키움 오픈API 테스트
//! Create        : , 2015/04/01
//! Update        : 
//! Comment       : 
//******************************************************************/
// RealAddDlg.cpp : 구현 파일
//

#include "stdafx.h"
#include "KhOpenApiTest.h"
#include "RealAddDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define FIDLIST	_T("9001;302;10;11;25;12;13")

// {조회 키,		리얼 키,	행, 열, 타입,			색 변경, 정렬, 앞 문자, 뒷 문자}
const stGRID lstOPTSBFID[] = 
{
	{"종목코드",					"9001",	-1,	0,	DT_NONE,					FALSE,	DT_LEFT,	"",	""}, 
	{"종목명",						"302",		-1,	1,	DT_NONE,					FALSE,	DT_LEFT,	"",	""}, 
	{"현재가",						"10",		-1,	2,	DT_ZERO_NUMBER,	TRUE,	DT_RIGHT,	"",	""}, 
	{"전일대비기호",				"25",		-1,	3,	DT_SIGN,					TRUE,	DT_CENTER,	"",	""}, 
	{"전일대비",					"11",		-1,	4,	DT_ZERO_NUMBER,	TRUE,	DT_RIGHT,	"",	""}, 
	{"등락율",						"12",		-1,	5,	DT_ZERO_NUMBER,	TRUE,	DT_RIGHT,	"",	"%"}, 
	{"거래량",						"13",		-1,	6,	DT_ZERO_NUMBER,	FALSE,	DT_RIGHT,	"",	""}, 
};

// CRealAddDlg 대화 상자
//*******************************************************************/
//! Function Name : CRealAddDlg::CRealAddDlg(CWnd* pParent /*=NULL*/) : CDialogEx(CRealAddDlg::IDD, pParent)
//! Function      : 생성 초기 처리
//! Param         : HANDLE hBitmap, int bits
//! Return        : void
//! Create        : , 2015/04/01
//! Comment       : 
//******************************************************************/
CRealAddDlg::CRealAddDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CRealAddDlg::IDD, pParent)
{
	m_pParent = pParent;
	m_strScrNo = "9999";
	m_cellSelect.row = 0;
	m_cellSelect.col = 0;
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_strPreConditionName = "";
	m_nPreConditionIndex = -1;
	m_strConditionName = "";
	m_nConditionIndex = -1;
}

//*******************************************************************/
// Function Name : DoDataExchange
// Function      : DDX/DDV에 관련 된 함수 호출
// Param         : CDataExchange*	pDX
// Return        : void
// Create        : , 2015/04/01
// Comment       : DDX - 컨트롤과 멤버변수사이에 정보를 교환하는 역할
//				 : DDV - 멤버변수에 저장 될 값의 유효성 체크
//				 : UapdateData()함수로 간접 호출 할 수 있음
//*******************************************************************/
void CRealAddDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRealAddDlg)
	DDX_Control(pDX, IDC_GRD_REALADD,		m_grdRealAdd);
	DDX_Control(pDX, IDC_CMB_CON,			m_cmbCon);
	DDX_Control(pDX, IDC_LIST_INSERT,		m_listCtl_Insert);
	DDX_Control(pDX, IDC_LIST_DELETE,		m_listCtl_Delete);
	//}}AFX_DATA_MAP
}

//*******************************************************************/
// BEGIN_MESSAGE_MAP
//*******************************************************************/
BEGIN_MESSAGE_MAP(CRealAddDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_CLOSE()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_SEARCH,				OnBtnSearch)
	ON_BN_CLICKED(IDC_BTN_CLOSE,					OnBtnClose)
	ON_BN_CLICKED(IDC_BTN_ALLCANCEL,			OnBnClickedBtnAllcancel)
	ON_BN_CLICKED(IDC_BTN_CONDI, &CRealAddDlg::OnBnClickedBtnCondi)
	ON_BN_CLICKED(IDC_BUTTON1, &CRealAddDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CRealAddDlg::OnBnClickedButton2)
END_MESSAGE_MAP()

//*******************************************************************/
//! Function Name : OnInitDialog
//! Function      : 생성 초기 처리
//! Param         : void
//! Return        : BOOL
//! Create        : , 2015/04/01
//! Comment       : CRealAddDlg 메시지 처리기
//******************************************************************/
BOOL CRealAddDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 이 대화 상자의 아이콘을 설정합니다. 응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	InitRealAddGrid();					// 그리드 초기 처리

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

//*******************************************************************/
//! Function Name : OnPaint
//! Function      : 그리기 처리
//! Param         : void
//! Return        : void
//! Create        : , 2015/04/01
//! Comment       : 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//!               : 아래 코드가 필요합니다. 문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
//!               : 프레임워크에서 이 작업을 자동으로 수행합니다.
//******************************************************************/
void CRealAddDlg::OnPaint()
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
//! Function Name : OnClose
//! Function      : 파괴자
//! Param         : void
//! Return        : void
//! Create        : , 2015/04/01
//! Comment       : 
//******************************************************************/
void CRealAddDlg::OnClose()
{
	// 화면 닫을 때 부모윈도우에게 통보(화면 관리 위해)
	if (m_pParent)
	{
		//시세 실시간 해제
		theApp.m_khOpenApi.SetRealRemove(m_strScrNo, _T("ALL"));

		if (m_strConditionName != "" && m_nConditionIndex > -1)
		{
			theApp.m_khOpenApi.SendConditionStop(m_strScrNo, m_strConditionName, m_nConditionIndex);
		}
		int nLen = m_strScrNo.GetLength();
		char *cScrNo = new char[nLen + 1];
		memset(cScrNo, 0x00, nLen + 1);
		memcpy(cScrNo, m_strScrNo, nLen);
		m_pParent->PostMessage(UM_SCRENN_CLOSE, 0U, (LPARAM)cScrNo);
	}

	CDialogEx::OnClose();
}

//*******************************************************************/
//! Function Name : OnQueryDragIcon
//! Function      : 드래그 아이콘 처리
//! Param         : void
//! Return        : HCURSOR
//! Create        : , 2015/04/01
//! Comment       : 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서 
//!               : 이 함수를 호출합니다.
//******************************************************************/
HCURSOR CRealAddDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

//*******************************************************************/
//! Function Name : OnBtnSearch
//! Function      : 시세 실시간등록
//! Param         : void
//! Return        : void
//! Create        : , 2015/04/01
//! Comment       : 
//******************************************************************/
void CRealAddDlg::OnBtnSearch()
{
	//시세 실시간등록 함수 호출
	//종목에 대한 현재가, 대비, 등락률, 거래량등에 대한 실시간을 받기 위해 반드시 호출해야함.
	int nCount = m_grdRealAdd.GetRowCount();
	CString strCodeList;
	
	for (int i = 1; i < nCount; i++)
	{
		strCodeList += m_grdRealAdd.GetItemText(i, 0) + ";";
	}

	long lRet = theApp.m_khOpenApi.SetRealReg(m_strScrNo, strCodeList, FIDLIST, "1");
}

//*******************************************************************/
//! Function Name : OnBtnClose
//! Function      : 실시간 해제.
//! Param         : void
//! Return        : void
//! Create        : , 2015/04/01
//! Comment       : 
//******************************************************************/
void CRealAddDlg::OnBtnClose()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	theApp.m_khOpenApi.SetRealRemove(m_strScrNo, _T("ALL"));
}

//*******************************************************************/
//! Function Name : InitRealAddGrid
//! Function      : 호가 그리드 초기 처리
//! Param         : void
//! Return        : void
//! Create        : , 2015/04/01
//! Comment       : 
//******************************************************************/
void CRealAddDlg::InitRealAddGrid()
{
	m_grdRealAdd.SetEditable(false);				//cell을 에디트 못하게 함.
	m_grdRealAdd.EnableScrollBars(SB_BOTH, FALSE);

	COLORREF clr = RGB(215, 227, 241);

	// 고정 행/열 설정
	m_grdRealAdd.SetFixedRowCount(1);

	// 행/열 갯수 설정
	m_grdRealAdd.SetRowCount(1);
	m_grdRealAdd.SetColumnCount(8);

	// 열의 넓이 설정
	int i, nWidth[] = {50, 80, 80, 20, 60, 60, 100};
	CString strHeader[] = {"코드", "종목명", "현재가", "", "전일대비", "등락율", "거래량"};
	int nCnt = sizeof(nWidth) / sizeof(*nWidth);		// 전체크기 / 원소크기 = 원소개수
	for (i = 0; i < nCnt; i++)
	{
		m_grdRealAdd.SetColumnWidth(i, nWidth[i]);
		m_grdRealAdd.SetItemFormat(0, i, DT_CENTER);
		m_grdRealAdd.SetItemText(0, i, strHeader[i]);
		m_grdRealAdd.SetItemBkColour(0, i, clr);	// 지정된 셀의 배경색 설정
	}

	// 행의 높이 설정
	m_grdRealAdd.SetRowHeight(0, 24);

	m_grdRealAdd.Invalidate();
}

//*******************************************************************/
//! Function Name : OnReceiveTrDataKhopenapictrl
//! Function      : 조회 응답 처리
//! Param         : LPCTSTR sScrNo, LPCTSTR sRQName, LPCTSTR sTrcode, LPCTSTR sRecordName, LPCTSTR sPrevNext, long nDataLength, LPCTSTR sErrorCode, LPCTSTR sMessage, LPCTSTR sSplmMsg
//! Return        : void
//! Create        : , 2015/04/01
//! Comment       : 
//******************************************************************/
void CRealAddDlg::OnReceiveTrDataKhopenapictrl(LPCTSTR sScrNo, LPCTSTR sRQName, LPCTSTR sTrcode, LPCTSTR sRecordName, LPCTSTR sPrevNext, long nDataLength, LPCTSTR sErrorCode, LPCTSTR sMessage, LPCTSTR sSplmMsg)
{
	return;
}

//*******************************************************************/
//! Function Name : OnReceiveMsgKhopenapictrl
//! Function      : 조회 에러 처리
//! Param         : LPCTSTR sScrNo, LPCTSTR sRQName, LPCTSTR sTrCode, LPCTSTR sMsg
//! Return        : void//! Create        : , 2015/04/01
//! Comment       : 
//******************************************************************/
void CRealAddDlg::OnReceiveMsgKhopenapictrl(LPCTSTR sScrNo, LPCTSTR sRQName, LPCTSTR sTrCode, LPCTSTR sMsg)
{
}

//*******************************************************************/
//! Function Name : OnReceiveRealDataKhopenapictrl
//! Function      : 실시간 처리
//! Param         : LPCTSTR sJongmokCode, LPCTSTR sRealType, LPCTSTR sRealData
//! Return        : void
//! Create        : , 2015/04/01
//! Comment       : 
//******************************************************************/
void CRealAddDlg::OnReceiveRealDataKhopenapictrl(LPCTSTR sJongmokCode, LPCTSTR sRealType, LPCTSTR sRealData)
{
	CString strIndex;

	if (!m_mapJongCode.Lookup(sJongmokCode, strIndex))
	{
		return;
	}

	CString strData;
	CStringArray arrData;

	arrData.Add(sJongmokCode);
	int i, nFieldCnt = sizeof(lstOPTSBFID) / sizeof(*lstOPTSBFID);		// 전체크기 / 원소크기 = 원소개수
	for (i = 1; i < nFieldCnt; i++)
	{
		if (atoi(lstOPTSBFID[i].strRealKey) < 0)
		{
			arrData.Add("");
			continue;
		}
		strData = theApp.m_khOpenApi.GetCommRealData(sJongmokCode, atoi(lstOPTSBFID[i].strRealKey));	strData.Trim();
		arrData.Add(strData);
	}
	SetDataRealAddGrid(arrData, sRealType);
}

//*******************************************************************/
//! Function Name : OnReceiveRealCondition
//! Function      : 조건검색 실시간 종목 편입/이탈
//! Param         : LPCTSTR strCode, LPCTSTR strType, LPCTSTR strConditionName, LPCTSTR strConditionIndex
//! Return        : void
//! Create        : , 2015/04/01
//! Comment       : 
//******************************************************************/
void CRealAddDlg::OnReceiveRealCondition(LPCTSTR strCode, LPCTSTR strType, LPCTSTR strConditionName, LPCTSTR strConditionIndex)
{
	CString strName, strMsg, strIndex, sType, sCode;
	sType = strType;	//종목 편입, 이탈 구분
	sCode = strCode;	//종목코드
	m_cmbCon.GetLBText(m_cmbCon.GetCurSel(), strName);	//현재 선택된 조건명

	if (strName == strConditionName)	//현재 선택된 조건명과 실시간으로 들어온 조건명이 같은지 비교.
	{
		if (sType == "I")	//종목 편입
		{
			m_grdRealAdd.InsertRow(sCode, 1);

			// 종목명/
			CString strCodeName;
			strCodeName = theApp.m_khOpenApi.GetMasterCodeName(sCode);
			m_grdRealAdd.SetItemText(1, 1, strCodeName);

			//하단 메세지 보이기
			//메세지 5줄만 넘으로 클리어하기.
			if (m_listCtl_Insert.GetCount() > 5)
			{
				m_listCtl_Insert.ResetContent();
			}

			strMsg.Format(_T("[%s] 종목이 편입되었습니다."), sCode);
			m_listCtl_Insert.AddString(strMsg);

			//기존조회된 종목의 시세가 들어오고 있는 상황에서 추가로 편입된 종목의 시세를 보여줄때 
			// 마지막 파라메터에 "1"을 넣어주어야 기존 종목과 함께 편입된 종목의 시세가 들어온다.
			// 만약 "1" 이외의 "0"을 넣어주면 기존에 등록한 종목들에 대한 시세가 끊어진다.
			//실시간등록 함수 호출
			long lRet = theApp.m_khOpenApi.SetRealReg(m_strScrNo, strCode, FIDLIST, "1");
		} 
		else if (sType == "D")	//종목 이탈
		{
			//기존에 있던 종목인지 확인.
			if (!m_mapJongCode.Lookup(sCode, strIndex))
			{
				return;
			}

			//이탈 종목 삭제.
			m_grdRealAdd.DeleteRow(_ttoi(strIndex));

			//이탈된 종목의 시세를 받지 않을려면 사용한다.
			theApp.m_khOpenApi.SetRealRemove(m_strScrNo, sCode);

			//하단 메세지 보이기
			//메세지 5줄만 넘으로 클리어하기.
			if (m_listCtl_Delete.GetCount() > 5)
			{
				m_listCtl_Delete.ResetContent();
			}

			strMsg.Format(_T("[%s] 종목이 이탈되었습니다."), sCode);
			m_listCtl_Delete.AddString(strMsg);
		}

		m_grdRealAdd.Invalidate();

 		m_mapJongCode.RemoveAll();

		int nCount = m_grdRealAdd.GetRowCount();
		for (int i = 1; i < nCount; i++)
		{
			sCode = m_grdRealAdd.GetItemText(i, 0);
			strIndex.Format("%d", i);
			m_mapJongCode.SetAt(sCode, strIndex);
		}
	}
}

//*******************************************************************/
//! Function Name	: OnReceiveTrCondition
//! Function			: 조건검색 종목리스트 조회응답
//! Param				: LPCTSTR sScrNo					- 화면번호
//!						: PCTSTR strCodeList				- 종목리스트
//!						: LPCTSTR strConditionName	- 조건명
//!						: int nIndex								- 조건명 인덱스번호
//!						: int nNext								- 연속조회(2:연속조회, 0:연속조회없음)
//! Return        : void
//! Create        : , 2015/04/01
//! Comment       : 
//******************************************************************/
void CRealAddDlg::OnReceiveTrCondition(LPCTSTR sScrNo,PCTSTR strCodeList, LPCTSTR strConditionName, int nIndex, int nNext)
{
	CString strCndName, sConditionName;
	m_cmbCon.GetLBText(m_cmbCon.GetCurSel(), strCndName);	//선택된 조건명

	if (strConditionName == strCndName)	//현재 조건명과 조회응답으로 들어온 조건명이 같을때만 처리.
	{
		if (strCodeList != "")
		{
			int nIndex(0);
			CString strConditionCode;
			m_mapJongCode.RemoveAll();		// 종목코드 초기화
			m_grdRealAdd.SetRowCount(1);

			while ( AfxExtractSubString( strConditionCode, strCodeList, nIndex++, _T(';')) )
			{
			}

			m_grdRealAdd.SetRowCount(nIndex-1);

			nIndex = 0;
			CString strIndex;
			while ( AfxExtractSubString( strConditionCode, strCodeList, nIndex++, _T(';')) )
			{
				if (strConditionCode != "")
				{
					m_grdRealAdd.SetRowHeight(nIndex, 20);		// 행의 높이 설정
					m_grdRealAdd.SetItemText(nIndex, 0, strConditionCode);

					// 종목명
					CString strCodeName;
					strCodeName = theApp.m_khOpenApi.GetMasterCodeName(strConditionCode);
					m_grdRealAdd.SetItemText(nIndex, 1, strCodeName);

					strIndex.Format("%d", nIndex);
					m_mapJongCode.SetAt(strConditionCode, strIndex);
				}
			}
		}
	}
}

//*******************************************************************/
//! Function Name	: OnReceiveConditionVer
//! Function			: 사용자 조건식 조회응답
//! Param				: BOOL bRet							- 성공(TRUE), 실패(FALSE)
//!						: LPCTSTR sMsg					- 메세지(reserved)
//! Return        : void
//! Create        : , 2015/04/01
//! Comment       : 
//******************************************************************/
void CRealAddDlg::OnReceiveConditionVer(long lRet, LPCTSTR sMsg)
{
	CString strMsg(sMsg);
	if (lRet == 1)
	{
		//조건검색명 리스트를 불러온다.
		CString strCondition, strConditionName, strIndex;
		CString strConditionNameList = theApp.m_khOpenApi.GetConditionNameList();

		//받아온 조건검색명을 콤보에 넣는다.
		m_cmbCon.ResetContent();

		int i(0);

		while( AfxExtractSubString( strCondition, strConditionNameList, i++, _T(';')) )
		{
			if (strCondition != _T(""))
			{
				strCondition.Trim();
				int nPos = strCondition.Find("^");
				int nConditionNo = atoi(strCondition.Left(nPos));
				strConditionName = strCondition.Mid(nPos+1, strCondition.GetLength());
				strIndex.Format(_T("%d"), nConditionNo);
				m_mapNameList.SetAt(strConditionName, strIndex);
				m_cmbCon.InsertString(i-1, strConditionName);
			}
		}

		m_cmbCon.SetCurSel(0);
		BOOL bRet = theApp.m_khOpenApi.SendCondition(m_strScrNo, "새조건명1", 0, 1);
	}
	else
	{
		CString		strErr;
		strErr.Format(_T("사용자 조건읽기 실패 \r\n\r\n[%s]"), strMsg);
		AfxMessageBox(strErr);
	}
}

//*******************************************************************/
//! Function Name : SetDataRealAddGrid
//! Function      : 그리드 데이타 설정(관심종목정보 설정)
//! Param         : CStringArray &arrData, CString strRealType/* = ""*/
//! Return        : void
//! Create        : , 2015/04/01
//! Comment       : 
//******************************************************************/
void CRealAddDlg::SetDataRealAddGrid(CStringArray &arrData, CString strRealType/* = ""*/)
{
	// 종목코드에 맞는 행 찾기
	CString strData, strTemp;
	strData = arrData.GetAt(0);
	if (!m_mapJongCode.Lookup(strData, strTemp))
	{
		return;
	}

	int i, nRow = atol(strTemp), nCnt = arrData.GetSize();

	// 데이타 설정
	CString strTempData;
	for (i = 1; i < nCnt; i++)
	{
		strData = arrData.GetAt(i);
		/////////////////////////////////////////////
		// 변경 된 데이터 선택 처리↓↓↓↓↓↓↓↓↓
		if (strRealType != "" && (i == 2 || i == 4 || i == 5))
		{
			strTempData = strData;
			strTempData.Replace("+", "");	strTempData.Replace("-", "");	strTempData.Replace(".", "");

			strTemp = m_grdRealAdd.GetItemText(nRow, lstOPTSBFID[i].nCol);
			strTemp.Replace("%", "");	strTemp.Replace("+", "");	strTemp.Replace("-", "");
			strTemp.Replace(",", "");	strTemp.Replace(".", "");
			if (strTempData != strTemp)
			{
				m_cellSelect.col = lstOPTSBFID[i].nCol;
				m_grdRealAdd.SetFocusCell(m_cellSelect);
				m_grdRealAdd.SetSelectedRange(nRow, m_cellSelect.col, nRow, m_cellSelect.col);
				m_grdRealAdd.Invalidate();
			}
		}
		// 변경 된 데이터 선택 처리↑↑↑↑↑↑↑↑↑
		/////////////////////////////////////////////
		if (lstOPTSBFID[i].bTextColor)
		{
			strTemp = arrData.GetAt(3);
			if (strTemp == "1" || strTemp == "2")	// 부호에 따라 색상변경
			{
				strTemp = "1";
			}
			else if (strTemp == "4" || strTemp == "5")	// 부호에 따라 색상변경
			{
				strTemp = "-1";
			}
			else
			{
				strTemp = "0";
			}
			theApp.SetDataFgColour(&m_grdRealAdd, nRow, lstOPTSBFID[i].nCol, strTemp);
		}
		if (lstOPTSBFID[i].nDataType == DT_SIGN)
		{
			if (strData.IsEmpty())
			{
				continue;
			}
			theApp.SetSignData(&m_grdRealAdd, nRow, lstOPTSBFID[i].nCol, strData);
		}
		else
		{
			if (strData.IsEmpty())
			{
				continue;
			}
			int nCol = lstOPTSBFID[i].nCol;
			m_grdRealAdd.SetItemText(nRow, lstOPTSBFID[i].nCol, theApp.ConvDataFormat(lstOPTSBFID[i].nDataType, strData, lstOPTSBFID[i].strBeforeData, lstOPTSBFID[i].strAfterData));
		}
	}
	m_grdRealAdd.Invalidate();
}

//*******************************************************************/
//! Function Name : OnBtnClose
//! Function      : 전화면 실시간 등록 해제
//! Param         : void
//! Return        : void
//! Create        : , 2015/04/01
//! Comment       : 
//******************************************************************/
void CRealAddDlg::OnBnClickedBtnAllcancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	theApp.m_khOpenApi.SetRealRemove(_T("ALL"), _T("ALL"));
}

//*******************************************************************/
//! Function Name : OnBnClickedBtnCondi
//! Function      : 조건명 리스트를 불러와서 콤보에 저장.
//! Param         : void
//! Return        : void
//! Create        : , 2015/04/22
//! Comment       : 
//******************************************************************/
void CRealAddDlg::OnBnClickedBtnCondi()
{
	CString strConditionName = _T(""), strConditionNameList = _T(""), strCondition = _T(""), strIndex = _T("");

	//조건검색명 리스트를 불러온다.
	strConditionNameList = theApp.m_khOpenApi.GetConditionNameList();

	//받아온 조건검색명을 콤보에 넣는다.
	m_cmbCon.ResetContent();

	int i(0);

	while( AfxExtractSubString( strCondition, strConditionNameList, i++, _T(';')) )
	{
		if (strCondition != _T(""))
		{
			strCondition.Trim();
			int nPos = strCondition.Find("^");
			int nConditionNo = atoi(strCondition.Left(nPos));
			strConditionName = strCondition.Mid(nPos+1, strCondition.GetLength());
			strIndex.Format(_T("%d"), nConditionNo);
			m_mapNameList.SetAt(strConditionName, strIndex);
			m_cmbCon.InsertString(i-1, strConditionName);
		}
	}

	if(m_cmbCon.GetCount() > 0)
		m_cmbCon.SetCurSel(0);
}

//조건 검색 실시간 조회
void CRealAddDlg::OnBnClickedButton1()
{
	CString strConditionCodeList = _T(""), strIndex;

	//선택한 조건명의 검색된 종목리스트를 불러온다.
	m_cmbCon.GetLBText(m_cmbCon.GetCurSel(), m_strConditionName);
	m_mapNameList.Lookup(m_strConditionName, strIndex);
	m_nConditionIndex = _ttoi(strIndex);

	m_strPreConditionName = m_strConditionName;
	m_nPreConditionIndex = m_nConditionIndex;

	//종목편입, 이탈 로그 삭제
	m_listCtl_Insert.ResetContent();
	m_listCtl_Delete.ResetContent();

	//조검검색 실시간 조회
	//종목리스트는 OnReceiveTrCondition()을 통하여 들어온다.
	//실시간 조건 검색을 하고 싶으면 마지막 파라메터에 1로 준다.
	BOOL bRet = theApp.m_khOpenApi.SendCondition(m_strScrNo, m_strConditionName, m_nConditionIndex, 1);

	if (bRet == FALSE)
	{
		//에러
		return;
	}
}


void CRealAddDlg::OnBnClickedButton2()
{
	//조건중지
	m_strPreConditionName = "";
 	CString strConditionName, strIndex;
 	m_cmbCon.GetLBText(m_cmbCon.GetCurSel(), strConditionName);
	m_mapNameList.Lookup(m_strConditionName, strIndex);
	int nConditionIndex = _ttoi(strIndex);
 	theApp.m_khOpenApi.SendConditionStop(m_strScrNo, strConditionName, nConditionIndex);
}
