#pragma once

#ifdef _RENDERER_DLL
// DLL ����
// DLL �� ����� ����  �Լ�/Ŭ�������� "export" �� ����.
#define _RDLL  __declspec(dllexport)
//#define _TEMPLATE

#else
// DLL ���
// DLL �� ����ϱ� ���� �������. "import" �� ����.
#define _RDLL  __declspec(dllimport)
//#define _TEMPLATE extern

#ifdef _WIN64
#ifdef _DEBUG
#pragma comment(lib, "DirectX11Engine_Debug.lib")		//64bit + debug

#else
#pragma comment(lib, "DirectX11Engine_Release.lib")		//64bit.

#endif 
#endif   
#endif
