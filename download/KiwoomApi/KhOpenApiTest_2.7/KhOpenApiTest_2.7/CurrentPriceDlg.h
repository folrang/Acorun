//******************************************************************/
//******************************************************************/
//! All Rights Reserved. Copyright(c)   2014 (주)한국소리마치        /
//******************************************************************/
//! File Name     : CurrentPriceDlg.h
//! Function      : 현재가 다이얼로그
//! System Name   : 키움 오픈API 테스트
//! Create        : , 2014/06/02
//! Update        : 
//! Comment       : 
//******************************************************************/
// CurrentPriceDlg.h : 헤더 파일
//

#pragma once

#include "KhOpenApiTestDlg.h"

// CCurrentPriceDlg 대화 상자
class CCurrentPriceDlg : public CDialogEx
{
// 생성입니다.
public:
	CCurrentPriceDlg(CWnd* pParent = NULL);	// 표준 생성자입니다.

// 대화 상자 데이터입니다.
	enum { IDD = IDD_CURRENTPRICE_DLG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.

public:
	CWnd*		m_pParent;
	CString		m_strScrNo;
	CString		m_strScrNo2;
	CString		m_strScrNo3;
	CString		m_strJongCode;

	CGridCtrl	m_grdCurPrc;		// 현재가 그리드
	CGridCtrl	m_grdJongInfo;		// 종목정보 그리드
	CGridCtrl	m_grdHoga;			// 호가 그리드
	CGridCtrl	m_grdTick;			// 틱 그리드
	void InitCurPrcGrid();			// 현재가 그리드 초기 처리
	void InitJongInfoGrid();		// 종목정보 그리드 초기 처리
	void InitHogaGrid();			// 호가 그리드 초기 처리
	void InitTickGrid();			// 틱 그리드 초기 처리

	void SendSearch();				// 조회 처리

	void OnReceiveTrDataKhopenapictrl(LPCTSTR sScrNo, LPCTSTR sRQName, LPCTSTR sTrcode, LPCTSTR sRecordName, LPCTSTR sPrevNext, long nDataLength, LPCTSTR sErrorCode, LPCTSTR sMessage, LPCTSTR sSplmMsg);
	void OnReceiveRealDataKhopenapictrl(LPCTSTR sJongmokCode, LPCTSTR sRealType, LPCTSTR sRealData);
	void OnReceiveMsgKhopenapictrl(LPCTSTR sScrNo, LPCTSTR sRQName, LPCTSTR sTrCode, LPCTSTR sMsg);
	void OnReceiveChejanData(LPCTSTR sGubun, LONG nItemCnt, LPCTSTR sFidList);

	// 현재가, 종목정보 그리드 데이타 설정(주식기본정보 설정)
	void SetDataJongInfoGrid(CStringArray &arrData, CString strRealType = "");
	// 틱 그리드 데이타 설정(체결정보 설정)
	void SetDataTickGrid(int nInsertRow, int nRow, CStringArray &arrData);
	// 호가 그리드 데이타 설정(주식호가 설정)
	void SetDataHogaGrid(CStringArray &arrData, CString strRealType = "");

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
