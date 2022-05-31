#include "resource.h"
#include "pch.h"
#include "framework.h"
#include "GameProcess.h"

/// <summary>
/// pre_project ��������
/// 
/// 2021. 08. 14 B.BUNNY
/// </summary>


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,			// ���α׷��� �ν��Ͻ��ڵ�, ���α׷� ��ü�� ���´� ������
	_In_opt_ HINSTANCE hPrevInstance,					// �ٷ� �տ� ����� ���� ���α׷��� �ν��Ͻ� �ڵ�, ������� NULL(WIN32������ �׻�NULL)
	_In_ LPWSTR    lpCmdLine,							// ��������� �Էµ� ���α׷� �μ�
	_In_ int       nCmdShow)							// ���α׷��� ����� ����
{
	//CreateWICTextureFromFile(�ؽ���)�� �����ϱ����� �����۾�
	//HRESULT hr = CoInitialize(nullptr);
	//if (FAILED(hr))
	//{
	//	ErrorLogger::Log(hr, L"CoInitialize�� �����߽��ϴ�");
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

