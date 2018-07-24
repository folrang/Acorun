//******************************************************************/
//******************************************************************/
//! All Rights Reserved. Copyright(c)   2014 (주)한국소리마치        /
//******************************************************************/
//! File Name     : KwanSimDlg.h
//! Function      : 관심 다이얼로그
//! System Name   : 키움 오픈API 테스트
//! Create        : , 2014/06/02
//! Update        : 
//! Comment       : 
//******************************************************************/
// KwanSimDlg.h : 헤더 파일
//

#pragma once

// CKwanSimDlg 대화 상자
class CKwanSimDlg : public CDialogEx
{
// 생성입니다.
public:
	CKwanSimDlg(CWnd* pParent = NULL);	// 표준 생성자입니다.

// 대화 상자 데이터입니다.
	enum { IDD = IDD_KWANSIM_DLG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.

public:
	CWnd*					m_pParent;
	CString					m_strScrNo;
	CMapStringToString		m_mapJongCode;		// 관심종목

	CCellID					m_cellSelect;		// 관심 그리드 선택 행
	CGridCtrl				m_grdKwanSim;		// 관심 그리드
	void InitKwanSimGrid();						// 관심 그리드 초기 처리
	void SetKwanSimJong();						// 관심 그리드에 종목 설정 처리
	// 종목 조회 요청
	void SendJongSearch(int nCodeCount = 0, CString strCodeList = "", int nAddType = 0);

	void OnReceiveTrDataKhopenapictrl(LPCTSTR sScrNo, LPCTSTR sRQName, LPCTSTR sTrcode, LPCTSTR sRecordName, LPCTSTR sPrevNext, long nDataLength, LPCTSTR sErrorCode, LPCTSTR sMessage, LPCTSTR sSplmMsg);
	void OnReceiveRealDataKhopenapictrl(LPCTSTR sJongmokCode, LPCTSTR sRealType, LPCTSTR sRealData);
	void OnReceiveMsgKhopenapictrl(LPCTSTR sScrNo, LPCTSTR sRQName, LPCTSTR sTrCode, LPCTSTR sMsg);
	void OnReceiveChejanData(LPCTSTR sGubun, LONG nItemCnt, LPCTSTR sFidList);

	// 그리드 데이타 설정(주식기본정보 설정)
	void SetDataKwanSimGrid(CStringArray &arrData, CString strRealType = "");

// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg void OnClose();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnBtnJongAdd();
	afx_msg void OnBtnJongDelete();
	afx_msg void OnBtnJongAllDelete();
	afx_msg void OnBtnSearch();
	DECLARE_MESSAGE_MAP()
};
