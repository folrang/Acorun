////////////////////////////////////////////////////////////////////////////////////////////////////
/// \file	Timer.h
/// \brief	CTimer 클래스 구현
/// \author	Dede
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once


#include <Windows.h>

//////////////////////////////////////////////////////////////////////
//				CTimer
//
//////////////////////////////////////////////////////////////////////
class CTimer
{
public:
	// [in] bManualReset: 수정 리셋 여부
	//		TRUE: 만료시, wait 하는 모든 쓰레드들이 알림을 받음
	//		FALSE: 만료시, 오직 한개의 쓰레드만 알림을 받음
	CTimer(BOOL bManualReset = TRUE);
	~CTimer(void);
	
	HANDLE GetWaitForHandle() { return m_hTimer; }

	// [in] nDueTimeMsconds: 대기 millisconds
	// [in] nPeriodMsconds: dwDueTimeMsconds 알림 이후에, 반복적으로 대기 후에 알릴(handle singnal) 시간
	BOOL Start(long nDueTimeMsconds, long nPeriodMsconds = 0);
	void Stop();

	BOOL Reset(long nDueTimeMsconds, long nPeriodMsconds = 0)
	{
		return Start(nDueTimeMsconds, nPeriodMsconds);
	}

private:
	static long m_nIdx;
	BOOL m_bManualReset;
	HANDLE m_hTimer;
};

//////////////////////////////////////////////////////////////////////
//				CAlarm
//
//////////////////////////////////////////////////////////////////////
class CAlarm
{
public:
	// [in] bManualReset: 수정 리셋 여부
	//		TRUE: 만료시, wait 하는 모든 쓰레드들이 알림을 받음
	//		FALSE: 만료시, 오직 한개의 쓰레드만 알림을 받음
	CAlarm(BOOL bManualReset = TRUE) : m_Timer(bManualReset) {}
	~CAlarm(void) {}
	
	HANDLE GetWaitForHandle() { return m_Timer.GetWaitForHandle(); }

	// [in] time: 알림할 시간 정보(년/월/일/시/분/초/milli)
	// [in] nPeriodMsconds: dwDueTimeMsconds 알림 이후에, 반복적으로 대기 후에 알릴(handle singnal) 시간
	BOOL SetTime(SYSTEMTIME& time, long nPeriodMsconds = 0);
	
private:
	CTimer m_Timer;
};