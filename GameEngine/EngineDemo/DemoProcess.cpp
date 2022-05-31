
//#include <DirectXMath.h>
//#include "SimpleMath.h"

//#include "EngineDLL.h"
//#include "TestScene1.h"

#include "DemoProcess.h"

//using namespace DirectX::SimpleMath;

bool DemoProcess::m_IsResizing = false;


HRESULT DemoProcess::Initialize(HINSTANCE hInstance)
{
   wchar_t szAppName[] = L"Demo for 3D Engine";

   // 윈도우 정보 입력
   WNDCLASSEXW wcex;
   wcex.cbSize = sizeof(WNDCLASSEX);
   wcex.style = CS_HREDRAW | CS_VREDRAW;
   wcex.lpfnWndProc = this->WndProc;
   wcex.cbClsExtra = 0;
   wcex.cbWndExtra = 0;
   wcex.hInstance = hInstance;
   wcex.hIcon = LoadIcon(hInstance, IDI_APPLICATION);
   wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
   wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
   wcex.lpszMenuName = nullptr;
   wcex.lpszClassName = szAppName;
   wcex.hIconSm = LoadIcon(wcex.hInstance, IDI_APPLICATION);

   // 윈도 클래스 등록
   RegisterClassExW(&wcex);

   // 윈도 생성
   m_hWnd = CreateWindowW(
      szAppName, szAppName,
      WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MAXIMIZEBOX,
      50, 50, 800, 600,
      nullptr, nullptr, hInstance, NULL);

   if (!m_hWnd) return E_FAIL;

   //g_Engine = DLLEngine::CreateEngine();
   //DLLEngine::Initialize(g_Engine, (int)m_hWnd, 800, 600);

   //m_pTestScene = dynamic_cast<TestScene1*>(DLLEngine::CreateScene(g_Engine)); // 됨

   //m_pTestScene = new TestScene1(); // 안됨
   //DLLEngine::AddScene(g_Engine, "TestScene1", m_pTestScene);
   //DLLEngine::SelectScene(g_Engine, "TestScene1");

   ShowWindow(m_hWnd, SW_SHOWNORMAL); 
   UpdateWindow(m_hWnd);

   return S_OK;
}

void DemoProcess::Loop()
{
   while (true)
   {
      if (PeekMessage(&m_Msg, NULL, 0, 0, PM_REMOVE))
      {
         if (m_Msg.message == WM_QUIT) break;
         DispatchMessage(&m_Msg);
      }
      else // 게임 루프
      {
         //DLLEngine::Loop(g_Engine);
      }
   }
}

void DemoProcess::Finalize()
{
   //DLLTestRenderer::Finalize();

}

LRESULT CALLBACK DemoProcess::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
   PAINTSTRUCT ps;
   HDC hdc;

   switch (message)
   {
   case WM_ACTIVATE:
   {
      return 0;
   }
   case WM_COMMAND:
   {
      break;
   }
   case WM_SIZE:
   {

      return 0;
   }
   case WM_ENTERSIZEMOVE:
   {
      m_IsResizing = true;
      return 0;
   }
   case WM_EXITSIZEMOVE:
   {
      m_IsResizing = false;
      //DLLTestRenderer::OnResize();
      return 0;
   }
   case WM_LBUTTONDOWN:
   case WM_MBUTTONDOWN:
   case WM_RBUTTONDOWN:
   {
      //m_pTestScene->OnMouseDown(hWnd, LOWORD(lParam), HIWORD(lParam));
      return 0;
   }
   case WM_LBUTTONUP:
   case WM_MBUTTONUP:
   case WM_RBUTTONUP:
   {
      //m_pTestScene->OnMouseUp(LOWORD(lParam), HIWORD(lParam));
      return 0;
   }
   case WM_MOUSEMOVE:
      //m_pTestScene->OnMouseMove(wParam, LOWORD(lParam), HIWORD(lParam));
   case WM_PAINT:
   {
      hdc = BeginPaint(hWnd, &ps);
      EndPaint(hWnd, &ps);
      break;
   }
   case WM_DESTROY:
   {
      PostQuitMessage(0);
      break;
   }
   }

   return  DefWindowProc(hWnd, message, wParam, lParam);
}
