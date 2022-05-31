#pragma once

#include <windows.h>

/// ���� ����� ���μ���

class DemoProcess
{
private:

	 // ������ �ʱ� ��ġ
	 const int m_WndPosX = 100;
	 const int m_WndPosY = 100;

	 // ȭ�� ũ��
	 const int m_ScreenWidth = 1024;
	 const int m_ScreenHeight = 768;

	 // ������ ���� 
	 HWND m_hWnd;
	 MSG m_Msg;
	 static bool m_IsResized;

	 /// 3D ���� ���� �߰�
	 // 3DEngine* m_p3DEngine;

	 /// Ÿ�̸� �� �߰�

private:
	 // Initialize window
	 void ResizeWindowSize();

	 // Loop
	 void UpdateAll();
	 void RenderAll();

public:
	 // ���� �ʱ�ȭ
	 HRESULT Initialize(HINSTANCE hInstance);

	 void Loop();
	 void Finalize();

	 static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);




};