#include "StdAfx.h"
#include "Timer.h"

#pragma warning(disable : 4996)

long CTimer::m_nIdx = 0;

//////////////////////////////////////////////////////////////////////
//				CTimer
//
//////////////////////////////////////////////////////////////////////
// [in] bManualReset: 수정 리셋 여부
//		TRUE: 만료시, wait 하는 모든 쓰레드들이 알림을 받음
//		FALSE: 만료시, 오직 한개의 쓰레드만 알림을 받음
CTimer::CTimer(BOOL bManualReset)
{
	m_bManualReset = bManualReset;
	m_hTimer = NULL;
	
	long nIdx = InterlockedIncrement(&m_nIdx);
	TCHAR szName[36];
	_stprintf(szName, _T("UBiTimer%d"), nIdx);
	m_hTimer = CreateWaitableTimer(NULL, m_bManualReset, szName);
}

CTimer::~CTimer(void)
{
	if(m_hTimer!=NULL)
	{
		CloseHandle(m_hTimer);
		m_hTimer = NULL;
	}
}

// [in] nDueTimeMsconds: 대기 millisconds
// [in] nPeriodMsconds: dwDueTimeMsconds 알림 이후에, 반복적으로 대기 후에 알릴(handle singnal) 시간
BOOL CTimer::Start(long nDueTimeMsconds, long nPeriodMsconds /*= 0*/)
{
	const int nTimerUnitsPerMscond = 10000;

	BOOL retVal = FALSE; 
	LARGE_INTEGER li;

	li.QuadPart = -(nDueTimeMsconds * nTimerUnitsPerMscond);
	retVal = SetWaitableTimer(m_hTimer, &li, nPeriodMsconds, NULL, NULL, FALSE);

	return retVal;
}

void CTimer::Stop()
{
	if(m_hTimer!=NULL)
	{
		CancelWaitableTimer(m_hTimer);
	}
}

// [in] time: 알림할 시간 정보(년/월/일/시/분/초/milli)
// [in] nPeriodMsconds: dwDueTimeMsconds 알림 이후에, 반복적으로 대기 후에 알릴(handle singnal) 시간
BOOL CAlarm::SetTime(SYSTEMTIME& time, long nPeriodMsconds /*= 0*/)
{
	const int nTimerUnitsPerMscond = 10000;

	BOOL retVal = FALSE; 
	LARGE_INTEGER liUTC;

	FILETIME ftLocal, ftUTC;
	SystemTimeToFileTime(&time, &ftLocal);
	LocalFileTimeToFileTime(&ftLocal, &ftUTC);

	liUTC.LowPart = ftUTC.dwLowDateTime;
	liUTC.HighPart = ftUTC.dwHighDateTime;
	
	retVal = SetWaitableTimer(m_Timer.GetWaitForHandle(), &liUTC, nPeriodMsconds, NULL, NULL, FALSE);

	return retVal;
}