#include "pch.h"
#include "MathLibrary.h"

int Add_Int(const int a, const int b)
{
	return a + b;
}

int Sum_Int(const int count, va_list args)
{
	int sum = 0;   // �հ� �ʱ�ȭ

	for (int i = 0; i < count; i++)
	{
		sum += va_arg(args, int);
		
		//sum += *args;
		//args += sizeof(va_list);
		//args++;
	}

	return sum;
}

float Add_Float(const float a, const float b)
{
	return a + b;
}

float Sum_Float(const float count, va_list args)
{
	float sum = 0.0f;   // �հ� �ʱ�ȭ

	int iCount = static_cast<int>(count);
	for (int i = 0; i < iCount; i++)
	{
		// va_list
		// va_arg �� ���ڷ� char, short flaot �� ��� �Ұ���
		// char, short �� int ��
		// float �� double �� ����ؾ� ��.
		sum += static_cast<float>(va_arg(args, double));

		//sum += *args;
		//args += sizeof(va_list);
		//args++;
	}

	return sum;
}

float Sub(const float a, const float b)
{
	return a - b;
}

float Mul(const float a, const float b)
{
	return a * b;
}

float Div(const float a, const float b)
{
	return a / b;
}

