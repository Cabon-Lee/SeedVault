#pragma once

#ifdef _RESOURCE_DLL
// DLL ����
// DLL �� ����� ����  �Լ�/Ŭ�������� "export" �� ����.
#define _RM_DLL  __declspec(dllexport)
//#define _TEMPLATE

#else
// DLL ���
// DLL �� ����ϱ� ���� �������. "import" �� ����.
#define _RM_DLL  __declspec(dllimport)
//#define _TEMPLATE extern

#ifdef _WIN64
#ifdef _DEBUG
#pragma comment(lib, "ResourceManager_Debug.lib")		//64bit + debug

#else
#pragma comment(lib, "ResourceManager_Release.lib")		//64bit.

#endif 
#endif   
#endif
