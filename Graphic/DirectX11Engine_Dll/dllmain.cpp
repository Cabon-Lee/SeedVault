// dllmain.cpp : DLL 애플리케이션의 진입점을 정의합니다.

// 진입점 함수가 호출되는 이유는 정보를 제공하기 위한 목적도 있으며,
// DLL이 프로세스별 또는 쓰레드별로 초기화를 수행하거나 정리할 목적으로 호출되기도 한다.
#include "pch.h"

BOOL APIENTRY DllMain( HMODULE hModule,             // DLL
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

