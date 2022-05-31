// dllmain.cpp : DLL 애플리케이션의 진입점을 정의합니다.
#include "pch.h"

#include "AudioDLLDefine.h"
#include "IAudioSystem.h"
#include "AudioSystem.h"

BOOL APIENTRY DllMain(HMODULE hModule,
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

//명시적 링크는* .lib 파일 없이 LoadLibrary(), GetProcAddress(), FreeLibrary()를 이용해
//DLL을 로딩, 함수의 주소를 구하여 실행, DLL 해제순으로 실행한다.

extern "C" _FADLL HRESULT DllCreateInstance(void** ppv)
{
	HRESULT hr;
	IAudioSystem* pExecutive = new AudioSystem();
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

