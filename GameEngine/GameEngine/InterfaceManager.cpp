#include "pch.h"

#include "IRenderer.h"
#include "IAudioSystem.h"

#include "InterfaceManager.h"


static HMODULE g_hExecutiveHandle;

typedef HRESULT(*CREATE_INSTANCE_FUNC)(void** ppv);

InterfaceManager::InterfaceManager()
{

}

InterfaceManager::~InterfaceManager()
{

}

std::shared_ptr<IRenderer> InterfaceManager::CreateRenderer()
{
	IRenderer* pExecutive;
	HRESULT hr;

#ifdef _DEBUG
	g_hExecutiveHandle = ::LoadLibrary(L"../../Lib/DirectX11Engine_Debug.dll");
#else
	g_hExecutiveHandle = ::LoadLibrary(L"DirectX11Engine_Release.dll");
#endif


	CREATE_INSTANCE_FUNC        pFunc;
	pFunc = (CREATE_INSTANCE_FUNC)::GetProcAddress(g_hExecutiveHandle, "DllCreateInstance");

	hr = pFunc((void**)&pExecutive);
	if (hr != S_OK)
	{
		MessageBox(NULL, L"CreateExecutive() - Executive 积己 角菩", L"Error", MB_OK);
		return FALSE;
	}

	std::shared_ptr<IRenderer> _newPtr(pExecutive);

	return _newPtr;
}



std::shared_ptr<IResourceManager> InterfaceManager::CreateResourceManager()
{
	IResourceManager* pExecutive;
	HRESULT hr;

#ifdef _DEBUG
	g_hExecutiveHandle = ::LoadLibrary(L"../../Lib/ResourceManager_Debug.dll");
#else
	g_hExecutiveHandle = ::LoadLibrary(L"ResourceManager_Release.dll");
#endif

	CREATE_INSTANCE_FUNC        pFunc;
	pFunc = (CREATE_INSTANCE_FUNC)::GetProcAddress(g_hExecutiveHandle, "DllCreateInstance");

	hr = pFunc((void**)&pExecutive);
	if (hr != S_OK)
	{
		MessageBox(NULL, L"CreateExecutive() - Executive 积己 角菩", L"Error", MB_OK);
		return FALSE;
	}

	std::shared_ptr<IResourceManager> _newPtr(pExecutive);


	return _newPtr;
}

//std::shared_ptr<IAudioSystem> InterfaceManager::CreateAudioSystem()
//{
//	IAudioSystem* pExecutive;
//	HRESULT hr;
//
//#ifdef _DEBUG
//	g_hExecutiveHandle = ::LoadLibrary(L"../../SeedVault/x64/Debug/FmodAudioEngine.dll");
//#else
//	g_hExecutiveHandle = ::LoadLibrary(L"../../SeedVault/x64/Release/FmodAudioEngine.dll");
//#endif
//
//	CREATE_INSTANCE_FUNC        pFunc;
//	pFunc = (CREATE_INSTANCE_FUNC)::GetProcAddress(g_hExecutiveHandle, "DllCreateInstance");
//
//	hr = pFunc((void**)&pExecutive);
//	if (hr != S_OK)
//	{
//		MessageBox(NULL, L"CreateExecutive() - Executive 积己 角菩", L"Error", MB_OK);
//		return FALSE;
//	}
//
//	std::shared_ptr<IAudioSystem> _newPtr(pExecutive);
//
//
//	return _newPtr;
//}
