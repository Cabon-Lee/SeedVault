#include "pch.h"

#include "StopWatch.h"
#include "CATrace.h"

clock_t StopWatch::m_Start = 0.0f;
clock_t StopWatch::m_Finish = 0.0f;

LARGE_INTEGER StopWatch::m_Frequency;
LARGE_INTEGER StopWatch::m_BeginTime;
LARGE_INTEGER StopWatch::m_EndTime;

void StopWatch::StartMeasure()
{
	QueryPerformanceFrequency(&m_Frequency);
	QueryPerformanceCounter(&m_BeginTime);

}

void StopWatch::StopMeasure()
{
	QueryPerformanceCounter(&m_EndTime);

	INT64 _elapsedTime = m_EndTime.QuadPart - m_BeginTime.QuadPart;
	float _duringTime = (float)_elapsedTime / (float)m_Frequency.QuadPart;


	CA_TRACE("측정 종료, 걸린 시간 = %f /ms", _duringTime * 1000);
}
