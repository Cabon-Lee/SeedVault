#include "resource.h"
#include "pch.h"
#include "framework.h"
#include "GameProcess.h"

/// <summary>
/// pre_project 시작지점
/// 
/// 2021. 08. 14 B.BUNNY
/// </summary>


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,			// 프로그램의 인스턴스핸들, 프로그램 자체를 일컫는 정수값
	_In_opt_ HINSTANCE hPrevInstance,					// 바로 앞에 실행된 현재 프로그램의 인스턴스 핸들, 없을경우 NULL(WIN32에서는 항상NULL)
	_In_ LPWSTR    lpCmdLine,							// 명령행으로 입력된 프로그램 인수
	_In_ int       nCmdShow)							// 프로그램이 실행될 형태
{
	//CreateWICTextureFromFile(텍스쳐)를 실행하기위한 선행작업
	//HRESULT hr = CoInitialize(nullptr);
	//if (FAILED(hr))
	//{
	//	ErrorLogger::Log(hr, L"CoInitialize에 실패했습니다");
	//	return false;
	//}

	GameProcess _GameProcess;
	
	if (_GameProcess.Initialize(hInstance, "pre_project", "MyWindowClass", 1900, 1030))
	{
		while (_GameProcess.ProcessMessages() == true)
		{
			_GameProcess.Update();
			_GameProcess.Draw();
		}
		_GameProcess.Destroy();
	}
	return 0;
}

