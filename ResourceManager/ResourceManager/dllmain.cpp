// dllmain.cpp : DLL 애플리케이션의 진입점을 정의합니다.
#include "pch.h"
#include "ResourcerDLLDefine.h"
#include "DX11ResourceManager.h"

BOOL APIENTRY DllMain( HMODULE hModule,
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

extern "C" _RM_DLL HRESULT DllCreateInstance(void** ppv)
{
	HRESULT hr;
	IResourceManager* pExecutive = new DX11ResourceManager;

	if (!pExecutive)
	{
		hr = E_OUTOFMEMORY;
		goto lb_return;
	}
	hr = S_OK;
	*ppv = pExecutive;
lb_return:
	return hr;
}
