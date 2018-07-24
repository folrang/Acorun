//******************************************************************/
//******************************************************************/
//! All Rights Reserved. Copyright(c)   2015 (주)한국소리마치        /
//******************************************************************/
//! File Name     : RateDlg.h
//! Function      : 수익률계산
//! System Name   : 키움 오픈API 테스트
//! Create        : , 2015/05/07
//! Update        : 
//! Comment       : 
//******************************************************************/
// RateDlg.h : 헤더 파일
//

#pragma once

// CRateDlg 대화 상자
class CRateDlg : public CDialogEx
{
// 생성입니다.
public:
	CRateDlg(CWnd* pParent = NULL);	// 표준 생성자입니다.

// 대화 상자 데이터입니다.
	enum { IDD = IDD_RATE };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.

public:
	CWnd*							m_pParent;
	CString							m_strScrNo;
	CMapStringToString		m_mapJongCode;		// 리얼등록 종목
	long								m_nCount;					// 그리드 Row 갯수.
	long								m_nNextRow;				// 연속조회시 시작 row 위치

	CGridCtrl						m_grdRate;				// 종목 수익률 그리드
	CEdit								m_EdtAcc;					// 계좌번호
	CEdit								m_EdtTotalEval;			// 총 평가금액
	CEdit								m_EdtTotalPurchase;	// 총 매입금액
	CEdit								m_EdtTotalProfit;			// 총 손익
	CEdit								m_EdtTotalRate;			// 총 수익률

	double							m_TotalEval;				// 총 평가금액
	double							m_TotalPurchase;		// 총 매입금액
	double							m_TotalProfit;				// 총 손익
	double							m_TotalRate;				// 총 수익률

	double							m_Eval;						// 평가금액(현금, 신용/대출)
	double							m_Profit;					// 평가손익(현금, 신용/대출)
	double							m_Rate;						// 수익률

	double							m_Purchase;				// 매입금액
	double							m_Comm;					// 매체수수료
	double							m_BuyComm;			// 매수가계산 수수료(계산)
	double							m_SellComm;			// 매도가계산 수수료(계산)
	double							m_Tax;						// 제세금(계산)
	double							m_Price;					// 현재가
	double							m_Qty;						// 수량

	BOOL							m_bNextFlag;				// 연속조회 유무 플래그.

	void InitRealAddGrid();						// 종목 그리드 초기 처리
	void OnReceiveTrDataKhopenapictrl(LPCTSTR sScrNo, LPCTSTR sRQName, LPCTSTR sTrcode, LPCTSTR sRecordName, LPCTSTR sPrevNext, long nDataLength, LPCTSTR sErrorCode, LPCTSTR sMessage, LPCTSTR sSplmMsg);
	void OnReceiveRealDataKhopenapictrl(LPCTSTR sJongmokCode, LPCTSTR sRealType, LPCTSTR sRealData);
	void OnReceiveMsgKhopenapictrl(LPCTSTR sScrNo, LPCTSTR sRQName, LPCTSTR sTrCode, LPCTSTR sMsg);
	void OnReceiveChejanData(LPCTSTR sGubun, LONG nItemCnt, LPCTSTR sFidList);
	void SetRate(int nRow, CString strCode);
	void SetTotalRate();

	// 그리드 데이타 설정(주식기본정보 설정)
	void SetDataRealAddGrid(CStringArray &arrData, CString strRealType = "");

// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg void OnClose();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnBtnSearch();
	DECLARE_MESSAGE_MAP()
};
