////////////////////////////////////////////////////////////////////////////////////////////////////
/// \file	Timer.h
/// \brief	CTimer Ŭ���� ����
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
	// [in] bManualReset: ���� ���� ����
	//		TRUE: �����, wait �ϴ� ��� ��������� �˸��� ����
	//		FALSE: �����, ���� �Ѱ��� �����常 �˸��� ����
	CTimer(BOOL bManualReset = TRUE);
	~CTimer(void);
	
	HANDLE GetWaitForHandle() { return m_hTimer; }

	// [in] nDueTimeMsconds: ��� millisconds
	// [in] nPeriodMsconds: dwDueTimeMsconds �˸� ���Ŀ�, �ݺ������� ��� �Ŀ� �˸�(handle singnal) �ð�
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
	// [in] bManualReset: ���� ���� ����
	//		TRUE: �����, wait �ϴ� ��� ��������� �˸��� ����
	//		FALSE: �����, ���� �Ѱ��� �����常 �˸��� ����
	CAlarm(BOOL bManualReset = TRUE) : m_Timer(bManualReset) {}
	~CAlarm(void) {}
	
	HANDLE GetWaitForHandle() { return m_Timer.GetWaitForHandle(); }

	// [in] time: �˸��� �ð� ����(��/��/��/��/��/��/milli)
	// [in] nPeriodMsconds: dwDueTimeMsconds �˸� ���Ŀ�, �ݺ������� ��� �Ŀ� �˸�(handle singnal) �ð�
	BOOL SetTime(SYSTEMTIME& time, long nPeriodMsconds = 0);
	
private:
	CTimer m_Timer;
};