#pragma once

// �ӵ��� �����ϱ� ���� ������� Ŭ����
// �������� Ȱ���� ���� Static���� ����(���� ���ټ�)

#include "windows.h"
#include <ctime>

class StopWatch
{
public:
	StopWatch() {};
	~StopWatch() {};
	// ���ӽ����̽��� ���ҹۿ� ���ϴ� Ŭ����... �̰� �´°�?
	static LARGE_INTEGER m_Frequency, m_BeginTime, m_EndTime;
	static clock_t m_Start, m_Finish;

public:
	static void StartMeasure();
	static void StopMeasure();


};

