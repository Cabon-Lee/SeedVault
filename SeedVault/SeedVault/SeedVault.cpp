// SeedVault.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//

#include "pch.h"
#include "framework.h"


#include "EngineDLL.h"
#include "Managers.h"
#include "CLMainScene.h"
#include "CLTestSceneYH.h"
#include "CLTestNavMeshScene.h"
#include "CLPhysXTestSceneJH.h"
#include "CLViewerScene.h"
#include "CLTestScene2D.h"
#include "CLTestHJ.h"
#include "CLMaterialTest.h"
#include "CLAnimation.h"
#include "CLTestInGameUI.h"


#include "JsonLoader.h"
#include "JsonLoader_CL.h";

#include "SeedVault.h"


#define MAX_LOADSTRING 100

// 전역 변수:
HINSTANCE hInst;                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.

// 이 코드 모듈에 포함된 함수의 선언을 전달합니다:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
   _In_opt_ HINSTANCE hPrevInstance,
   _In_ LPWSTR    lpCmdLine,
   _In_ int       nCmdShow)
{
   UNREFERENCED_PARAMETER(hPrevInstance);
   UNREFERENCED_PARAMETER(lpCmdLine);

   // TODO: 여기에 코드를 입력합니다.


   // 전역 문자열을 초기화합니다.
   LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
   LoadStringW(hInstance, IDC_SEEDVAULT, szWindowClass, MAX_LOADSTRING);
   MyRegisterClass(hInstance);

   // 애플리케이션 초기화를 수행합니다:
   if (!InitInstance(hInstance, nCmdShow))
   {
      return FALSE;
   }

   HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_SEEDVAULT));

   MSG msg;

   // Systems
   CL::Input* _CLInput = new CL::Input();
   JsonLoader_CL* _JsonLoader = new JsonLoader_CL();
  
   SceneBase* _CLMainScene = new CLMainScene();
   DLLEngine::AddScene("MainScene", _CLMainScene);
   
   SceneBase* _CLViewerScene = new CLViewerScene();
   DLLEngine::AddScene("ViewerScene", _CLViewerScene);
   
   SceneBase* _testScene;
   _testScene = new CLPhysXTestSceneJH();
   DLLEngine::AddScene("TestSceneJH", _testScene);
   
   Scene* _CLTestSceneYH = new CLTestSceneYH();
   _CLTestSceneYH->SetJsonLoader(_JsonLoader, std::string("../Data/Scene"));
   DLLEngine::AddScene("TestSceneYH", _CLTestSceneYH);

   Scene* _TestNavMeshScene = new CLTestNavMeshScene();
   _TestNavMeshScene->SetJsonLoader(_JsonLoader, std::string("../Data/Scene"));
   DLLEngine::AddScene("TestNavMeshScene", _TestNavMeshScene);
   
   SceneBase* _CLTestScene2D = new CLTestScene2D();
   DLLEngine::AddScene("CLTestScene2D", _CLTestScene2D);
   
   SceneBase* _CLTest = new CLTestHJ();
   DLLEngine::AddScene("CLTest", _CLTest);

   SceneBase* _CLTestMaterial = new CLMaterialTest();
   DLLEngine::SetSceneName(_CLTestMaterial, "CLMaterialTest");
   DLLEngine::AddScene("CLMaterialTest", _CLTestMaterial);

   SceneBase* _CLAnim = new CLAnimation();
   DLLEngine::SetSceneName(_CLAnim, "CLAnimation");
   DLLEngine::AddScene("CLAnimation", _CLAnim);

   SceneBase* _CLUI = new CLTestInGameUI();
   DLLEngine::SetSceneName(_CLUI, "CLTestInGameUI");
   DLLEngine::AddScene("CLTestInGameUI", _CLUI);

   DLLEngine::AddScene("Gen_Room_SH_01", DLLEngine::CreateScene(_JsonLoader, "../Data/Scene", "Gen_Room_SH_01"));



   //DLLEngine::SelectScene("TestSceneYH");
   DLLEngine::SelectScene("CLMaterialTest");
   //DLLEngine::SelectScene("CLTestInGameUI");


   while (true)
   {
      if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
      {
         if (msg.message == WM_QUIT) break;

         DispatchMessage(&msg);
      }
      else
      {
          /// SceneChange 기능은 ImGui 버튼으로~ 가능하게 변경됨.

          // Client Input Update
         _CLInput->Update();
          
         DLLEngine::Loop();

         //dynamic_cast<CLTestScene*>(_newScene)->Update(CL::Input::s_DeltaTime);
      }
   }

   DLLEngine::EngineFinalize();

   return (int)msg.wParam;
}

//
//  함수: MyRegisterClass()
//
//  용도: 창 클래스를 등록합니다.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
   WNDCLASSEXW wcex;

   wcex.cbSize = sizeof(WNDCLASSEX);

   wcex.style = CS_HREDRAW | CS_VREDRAW;
   wcex.lpfnWndProc = WndProc;
   wcex.cbClsExtra = 0;
   wcex.cbWndExtra = 0;
   wcex.hInstance = hInstance;
   wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SEEDVAULT));
   wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
   wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
   wcex.lpszMenuName = 0;
   wcex.lpszClassName = szWindowClass;
   wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

   return RegisterClassExW(&wcex);
}

//
//   함수: InitInstance(HINSTANCE, int)
//
//   용도: 인스턴스 핸들을 저장하고 주 창을 만듭니다.
//
//   주석:
//
//        이 함수를 통해 인스턴스 핸들을 전역 변수에 저장하고
//        주 프로그램 창을 만든 다음 표시합니다.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }


   DLLEngine::CreateEngine();
   DLLEngine::Initialize(reinterpret_cast<int>(hWnd), 800, 600);


   //ComponentSystem::GetInstance();
   //Managers::GetInstance()->GetManagersToClient(
   //   DLLEngine::GetManagers()->GetSceneManager(), 
   //   DLLEngine::GetManagers()->GetObjectManager(),
   //   DLLEngine::GetManagers()->GetCameraManager(),
   //   DLLEngine::GetManagers()->GetComponentSystem());


   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  함수: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  용도: 주 창의 메시지를 처리합니다.
//
//  WM_COMMAND  - 애플리케이션 메뉴를 처리합니다.
//  WM_PAINT    - 주 창을 그립니다.
//  WM_DESTROY  - 종료 메시지를 게시하고 반환합니다.
//
//


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{

#ifdef _DEBUG

    if (DLLEngine::ImGuiHandler(hWnd, message, wParam, lParam))
    {
		return true;

    }
#else
	if (DLLEngine::ImGuiHandler(hWnd, message, wParam, lParam))
	{
		return true;
	}
#endif // DEBUG
	

   switch (message)
   {
      case WM_COMMAND:
      {
         int wmId = LOWORD(wParam);
         // 메뉴 선택을 구문 분석합니다:
         switch (wmId)
         {
            case IDM_EXIT:
               DestroyWindow(hWnd);
               break;
            default:
               return DefWindowProc(hWnd, message, wParam, lParam);
         }

         break;
      }
      case WM_SIZE:
      {
         DLLEngine::OnResize(LOWORD(lParam), HIWORD(lParam));   
         break;
      }

      // 커서가 윈도우 밖으로 나갈 때
      case WM_MOUSELEAVE:
      {
          //MessageBox(hWnd, _T("12"), _T("34"), MB_ICONQUESTION);

          break;
      }

      case WM_PAINT:
      {
         PAINTSTRUCT ps;
         HDC hdc = BeginPaint(hWnd, &ps);
         // TODO: 여기에 hdc를 사용하는 그리기 코드를 추가합니다...

         //Ellipse(hdc, 100, 100, 150, 150);
         EndPaint(hWnd, &ps);
         break;
      }
      case WM_DESTROY:
         PostQuitMessage(0);
         break;
      default:
         return DefWindowProc(hWnd, message, wParam, lParam);
   }
   return 0;
}
