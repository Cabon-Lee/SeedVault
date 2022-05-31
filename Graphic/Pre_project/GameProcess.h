#pragma once
#pragma comment(lib, "DirectX11Engine.lib")
#pragma comment(lib, "DirectX11Engine_Dll.lib")

#include <string>

class RenderWindow;
class KeyboardClass;
class MouseClass;
class Timer;

/// <summary>
/// GameProcess
/// 매니저들을 관리
/// 
/// 2021. 08. 14 B.BUNNY
/// </summary>
class GameProcess
{
public:
	GameProcess();

	bool Initialize(HINSTANCE hInstance, std::string window_title, std::string window_class, int width, int height);
	void Destroy();
	void Update();

	bool ProcessMessages();
	void Draw();

	RenderWindow* m_Render_window;
	KeyboardClass* m_Keyboard;
	MouseClass* m_Mouse;

	//InputManager inputManager;
	//BoardManager boardManager;
	//SceneManager sceneManager;

	LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

private:
	Timer* m_Timer;
	int m_CameraNum = 0;
};
