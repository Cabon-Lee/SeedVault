#pragma once

#ifdef _FMODAUDIO_DLL
// DLL ����
// DLL �� ����� ����  �Լ�/Ŭ�������� "export" �� ����.
#define _FADLL  __declspec(dllexport)
//#define _TEMPLATE

#else
// DLL ���
// DLL �� ����ϱ� ���� �������. "import" �� ����.
#define _FADLL  __declspec(dllimport)
//#define _TEMPLATE extern

#endif 
