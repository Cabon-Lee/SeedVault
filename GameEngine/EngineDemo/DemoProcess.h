#pragma once
//#include "DXDefine.h"

#include <windows.h>

class Engine;
class TestScene1;
class SceneBase;

/// 엔진 데모용 프로세스
class DemoProcess
{
public:
	// 게임 초기화
	HRESULT Initialize(HINSTANCE hInstance);
	// 메인 루프
	void Loop();
	// 종료전 정리 delete등
	void Finalize();


private:
	// 윈도우 관련 
	HWND m_hWnd;
	MSG m_Msg;
	static bool m_IsResizing;

   Engine* g_Engine;

	TestScene1* m_pTestScene;

	// 메세지 핸들러 콜백
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
};