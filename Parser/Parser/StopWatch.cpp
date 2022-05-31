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

	//m_Start = 0.0f;
	//m_Start = clock();
}

void StopWatch::StopMeasure()
{
	QueryPerformanceCounter(&m_EndTime);

	INT64 _elapsedTime = m_EndTime.QuadPart - m_BeginTime.QuadPart;
	float _duringTime = (float)_elapsedTime / (float)m_Frequency.QuadPart;

	//m_Finish = clock();
	//float _elapsedTime = m_Finish - m_Start;

	CA_TRACE("���� ����, �ɸ� �ð� = %f /ms", _duringTime * 1000);
}
