//******************************************************************/
//******************************************************************/
//! All Rights Reserved. Copyright(c)   2015 (주)한국소리마치        /
//******************************************************************/
//! File Name     : RealAddDlg.h
//! Function      : 리얼등록 다이얼로그
//! System Name   : 키움 오픈API 테스트
//! Create        : , 2015/04/01
//! Update        : 
//! Comment       : 
//******************************************************************/
// RealAddDlg.h : 헤더 파일
//

#pragma once

// CRealAddDlg 대화 상자
class CRealAddDlg : public CDialogEx
{
// 생성입니다.
public:
	CRealAddDlg(CWnd* pParent = NULL);	// 표준 생성자입니다.

// 대화 상자 데이터입니다.
	enum { IDD = IDD_CON_SET };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.

public:
	CWnd*					m_pParent;
	CString					m_strScrNo;
	CString					m_strConditionName;			//현재 조건명
	int							m_nConditionIndex;			//현재 조건명 인덱스
	CString					m_strPreConditionName;		//이전 조건명
	int							m_nPreConditionIndex;		//이전 조건명 인덱스
	CMapStringToString		m_mapJongCode;		// 리얼등록 종목
	CMapStringToString		m_mapNameList;			// 조건검색명리스트

	CCellID					m_cellSelect;		// 종목 그리드 선택 행
	CGridCtrl				m_grdRealAdd;		// 종목 그리드
	CComboBox			m_cmbCon;		// 조건검색명 콤보
	CListBox m_listCtl_Insert;
	CListBox m_listCtl_Delete;
	void InitRealAddGrid();						// 종목 그리드 초기 처리
	void OnReceiveTrDataKhopenapictrl(LPCTSTR sScrNo, LPCTSTR sRQName, LPCTSTR sTrcode, LPCTSTR sRecordName, LPCTSTR sPrevNext, long nDataLength, LPCTSTR sErrorCode, LPCTSTR sMessage, LPCTSTR sSplmMsg);
	void OnReceiveRealDataKhopenapictrl(LPCTSTR sJongmokCode, LPCTSTR sRealType, LPCTSTR sRealData);
	void OnReceiveMsgKhopenapictrl(LPCTSTR sScrNo, LPCTSTR sRQName, LPCTSTR sTrCode, LPCTSTR sMsg);
	void OnReceiveChejanData(LPCTSTR sGubun, LONG nItemCnt, LPCTSTR sFidList);
	void OnReceiveRealCondition(LPCTSTR strCode, LPCTSTR strType, LPCTSTR strConditionName, LPCTSTR strConditionIndex);				//조건검색 실시간 삽입,삭제되는 종목을 받는다
	void OnReceiveTrCondition(LPCTSTR sScrNo, LPCTSTR strCodeList, LPCTSTR strConditionName, int nIndex, int nNext);	   //조건검색 종목리스트를 받는다.
	void OnReceiveConditionVer(long bRet, LPCTSTR sMsg);

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
	afx_msg void OnBtnClose();
	afx_msg void OnBnClickedBtnAllcancel();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnCondi();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
};
