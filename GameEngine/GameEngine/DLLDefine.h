#pragma once

#ifdef _LIB_DLL
// DLL ����
// DLL �� ����� ����  �Լ�/Ŭ�������� "export" �� ����.
   #define _DLL     __declspec(dllexport)

   //#define _TEMPLATE

#else
// DLL ���
// DLL �� ����ϱ� ���� �������. "import" �� ����.
   #define _DLL  __declspec(dllimport)
   //#define _TEMPLATE extern
   
      #ifdef _WIN64
         #ifdef _DEBUG
            #pragma comment(lib, "GameEngine_Debug.lib")		//64bit + debug

         #else
            #pragma comment(lib, "GameEngine_Release.lib")		//64bit.

         #endif 
      #endif   
#endif
