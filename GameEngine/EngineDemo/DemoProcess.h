#pragma once
//#include "DXDefine.h"

#include <windows.h>

class Engine;
class TestScene1;
class SceneBase;

/// ���� ����� ���μ���
class DemoProcess
{
public:
	// ���� �ʱ�ȭ
	HRESULT Initialize(HINSTANCE hInstance);
	// ���� ����
	void Loop();
	// ������ ���� delete��
	void Finalize();


private:
	// ������ ���� 
	HWND m_hWnd;
	MSG m_Msg;
	static bool m_IsResizing;

   Engine* g_Engine;

	TestScene1* m_pTestScene;

	// �޼��� �ڵ鷯 �ݹ�
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
};