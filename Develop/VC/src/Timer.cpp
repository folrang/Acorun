#include "StdAfx.h"
#include "Timer.h"

#pragma warning(disable : 4996)

long CTimer::m_nIdx = 0;

//////////////////////////////////////////////////////////////////////
//				CTimer
//
//////////////////////////////////////////////////////////////////////
// [in] bManualReset: ���� ���� ����
//		TRUE: �����, wait �ϴ� ��� ��������� �˸��� ����
//		FALSE: �����, ���� �Ѱ��� �����常 �˸��� ����
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

// [in] nDueTimeMsconds: ��� millisconds
// [in] nPeriodMsconds: dwDueTimeMsconds �˸� ���Ŀ�, �ݺ������� ��� �Ŀ� �˸�(handle singnal) �ð�
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

// [in] time: �˸��� �ð� ����(��/��/��/��/��/��/milli)
// [in] nPeriodMsconds: dwDueTimeMsconds �˸� ���Ŀ�, �ݺ������� ��� �Ŀ� �˸�(handle singnal) �ð�
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