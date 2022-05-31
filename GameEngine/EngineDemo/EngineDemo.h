#pragma once

#include <windows.h>

/// 엔진 데모용 프로세스

class DemoProcess
{
private:

	 // 윈도우 초기 위치
	 const int m_WndPosX = 100;
	 const int m_WndPosY = 100;

	 // 화면 크기
	 const int m_ScreenWidth = 1024;
	 const int m_ScreenHeight = 768;

	 // 윈도우 관련 
	 HWND m_hWnd;
	 MSG m_Msg;
	 static bool m_IsResized;

	 /// 3D 엔진 추후 추가
	 // 3DEngine* m_p3DEngine;

	 /// 타이머 등 추가

private:
	 // Initialize window
	 void ResizeWindowSize();

	 // Loop
	 void UpdateAll();
	 void RenderAll();

public:
	 // 게임 초기화
	 HRESULT Initialize(HINSTANCE hInstance);

	 void Loop();
	 void Finalize();

	 static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);




};