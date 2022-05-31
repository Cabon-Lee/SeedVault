#pragma once

// 속도를 측정하기 위해 만들어진 클래스
// 전역적인 활용을 위해 Static으로 만듦(쉬운 접근성)

#include "windows.h"
#include <ctime>

class StopWatch
{
public:
	StopWatch() {};
	~StopWatch() {};
	// 네임스페이스의 역할밖에 안하는 클래스... 이게 맞는가?
	static LARGE_INTEGER m_Frequency, m_BeginTime, m_EndTime;
	static clock_t m_Start, m_Finish;

public:
	static void StartMeasure();
	static void StopMeasure();


};

