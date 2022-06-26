#include "pch.h"
#include <winuser.h>

#include "SceneBase.h"

#include "Timer.h"
#include "Camera.h"
#include "Input.h"

#include "SceneManager.h"
#include "ObjectManager.h"
#include "CameraManager.h"
#include "Managers.h"

#include "ComponentSystem.h"

#include "ResourceLoader.h"

#include "IRenderer.h"
#include "IResourceManager.h"

#include "AudioEngineDLL.h"
#include "InterfaceManager.h"

#include "Transform.h"
#include "MeshFilter.h"
#include "MaterialComponent.h"
#include "MeshRenderer.h"

#include "ImguiClass.h"
#include "imgui_impl_win32.h"

#include "AdapterManager.h"

#include "PhysicsEngine.h"

#include "CATrace.h"

#include "Engine.h"
#include "EngineDLL.h"


Engine::Engine()
	: m_pRenderer(nullptr)
	, m_pResourceLoader(nullptr)
	, m_hWnd(nullptr)
	, m_pComponent(nullptr)
	, m_pInput(nullptr)
	, m_PrevMousePos({ 0, 0 })
	, m_CurrMousePos({ 0, 0 })
	, m_ClientWidth(0)
	, m_ClientHeight(0)
	, m_pInterfaces(nullptr)
	, m_EngineImgui(nullptr)
	, m_bImguiActive(true)

	, m_pNavMeshManager(nullptr)
	// test���� �ʱ�ȭ ������
{
	m_RenderOption = new IRenderOption();
	ZeroMemory(m_RenderOption, sizeof(m_RenderOption));
}

Engine::~Engine()
{

}

void Engine::Initialize()
{
	// ������ �޾Ƽ� initialize�Ѵ�

	m_isClicked = false;
	m_isResize = false;

}

void Engine::Initialize(int hWND, int width, int height)
{
	bool _success = false;

	m_hWnd = (HWND)hWND;

	m_ClientWidth = width;
	m_ClientHeight = height;

	m_isResize = false;

	m_isClicked = false;

	Timer::GetInstance()->Reset();
	Timer::GetInstance()->Start();

	m_pInterfaces = new InterfaceManager();

	m_pRenderer = m_pInterfaces->CreateRenderer();
	_success = m_pRenderer->Initialize(hWND, width, height);

	m_pResourceManager = m_pInterfaces->CreateResourceManager();
	m_pResourceManager->Initialize(m_pRenderer);

	//m_pResourceLoader = new ResourceLoader();
	//m_pResourceLoader->Initialize(m_pResourceManager);
	//m_pResourceLoader->LoadResource(std::string("../Data/"));
	//CA_TRACE("Load Resource");

	bool isDataLoad = false;
	std::thread m_t1(&Engine::ShowLoadingScene, this, std::ref(isDataLoad));
	LoadResource(isDataLoad);
	m_t1.join();

	m_pRenderer->SetResourceManager(m_pResourceManager);
	m_pDeferredInfo = reinterpret_cast<DeferredInfo*>(m_pRenderer->GetDeferredInfo());
	
	m_pComponent = ComponentSystem::GetInstance();
	m_pComponent->SetResourceLoader(m_pResourceLoader);
	m_pComponent->SetResourceManager(m_pResourceManager);
	
	// �Ŵ����� ��Ƶа� �̴ϼȶ�����
	Managers::GetInstance()->Initialize();
	Managers::GetInstance()->GetCameraManager()->SetHWND(m_hWnd);
	
	m_pNavMeshManager = Managers::GetInstance()->GetNavMeshManager();
	m_pNavMeshManager->SetRenderer(m_pRenderer);
	
	m_pInput = new Input();
	m_pInput->Initialize((HWND)hWND);
	
	m_EngineImgui = new ImguiClass();
	m_EngineImgui->Initialize(m_hWnd, m_pRenderer.get());
	
	DLLAudio::Initialize("../Data/FmodBank");
	DLLAudio::LoadAllBank(*m_pResourceLoader->GetBankNames());
	DLLAudio::Set3DSetting(1.0f, 1.0f);

}

void Engine::LoadResource(bool& isLoadDone)
{
	m_pResourceLoader = new ResourceLoader();
	m_pResourceLoader->Initialize(m_pResourceManager);
	m_pResourceLoader->LoadResource(std::string("../Data/"));

	CA_TRACE("Load Resource");

	isLoadDone = true;
}

void Engine::ShowLoadingScene(bool& isLoadDone)
{
	m_pRenderer->SetLoadingSceneImage("../Data/Image/LoadingScene01.png", "../Data/Shader/vs_2d.cso", "../Data/Shader/ps_2d.cso", 12);//->�ƴ� ���ͷ� ����;
	// �ε����� �׸���.
	while(isLoadDone == false)
	{
		// png�ε��Ѵ�. wictextureFromFile
		// �ؽ������� ������ ����
		// Draw2D�� ����� ������ �Լ�

		///������ ���ο��� ������ �ε��� ����,�Լ��ϳ� ����
		m_pRenderer->DrawLoadingScene();// -> �׷��ִ� �Լ�Render
		m_pRenderer->ExecuteCommandLine();

	}
	return;
}

void Engine::Loop()
{
	Timer::GetInstance()->Tick();

	if (Timer::GetInstance()->FixFrame(60.0f) == true)
	{
		if (Managers::GetInstance()->GetSceneManager()->GetNowScene()->IsPhysicsScene() == true)
		{
			Managers::GetInstance()->GetPhysicsEngine()->PhysicsLoop();
		}

		float _dTime = Timer::GetInstance()->DeltaTime();

		PhysicsUpdateAll();	// ���� ó��

		UpdateInput(Timer::GetInstance()->DeltaTime());
		UpdateAll(Timer::GetInstance()->DeltaTime());		// �ȿ��� ���� ���ϱ� �����ӷ���Ʈ���� �޶���)
		DLLAudio::Update();
		RenderAll();

		Timer::GetInstance()->ResetDeltaTime();
	}
}

void Engine::PhysicsUpdateAll()
{
	/// �浹����
	// GameLogic type ������Ʈ �߿��� 
	// myObject�� PhysicsActor ������Ʈ�� �پ��ִ� ��쿡�� ����
	m_pComponent->OnCollisionEnter();
	m_pComponent->OnCollisionStay();
	m_pComponent->OnCollisionExit();
	m_pComponent->ClearCollisionList();
}

void Engine::UpdateInput(float dTime)
{
	InputUpdate(dTime);
}

void Engine::UpdateAll(float dTime)
{
	// F2Ű
	if (m_pInput->GetKeyDown(0x71))
	{
		if (m_RenderOption->bDebugRenderMode == true)
		{
			m_pComponent->SetDebugRenderMode(false);
			m_RenderOption->bDebugRenderMode = false;
		}
		else
		{
			m_pComponent->SetDebugRenderMode(true);
			m_RenderOption->bDebugRenderMode = true;
		}
	}

	m_RenderOption->dTime = dTime;
	Managers::GetInstance()->GetSceneManager()->UpdateNowScene(dTime);
	Camera* _nowCam = Managers::GetInstance()->GetCameraManager()->GetNowCamera();
	m_pRenderer->CameraUpdate(_nowCam->GetWorldTM(), _nowCam->View(), _nowCam->Proj(), _nowCam->GetFovY(), _nowCam->GetFarZ());

	/// ������Ʈ
	m_pComponent->Update(dTime);

	m_isClicked = false;

	m_Frequency++;
	if (m_Frequency > 5)
	{
		m_Frequency = 0;
		m_pRenderer->DeferredPickingPass();
	}

	// ���콺 ��ġ�� �׻� ������Ʈ���ش�
	m_pRenderer->Clicked(m_isClicked, DLLInput::GetMouseClientPos());

	/// ������Ʈ ��ŷ
	/// ���콺 ���� Ŭ�� Ȯ��(���������� ������Ʈ ��ŷ)
	if (DLLInput::InputKeyUp(static_cast<int>(0x01)) == true && m_isClicked == false)
	{
		m_isClicked = true;
		m_pRenderer->Clicked(m_isClicked, DLLInput::GetMouseClientPos());
		DeferredPicking();
	}

	/// ���õ� ������Ʈ ���� & ����
	if (m_isClicked == true)
	{
		unsigned int _curID = m_pRenderer->GetClickedObjectId();

		if (_curID != m_ClickedObjectID && m_ClickedObjectID > 0)
		{
			if (m_SelectedObject != nullptr)
			{
				m_SelectedObject->GetComponent<MeshRenderer>()->SetSelected(false);
			}
		}
		m_ClickedObjectID = _curID;

		if (m_ClickedObjectID > 0)
		{
			m_SelectedObject = Managers::GetInstance()->GetSceneManager()->GetNowScene()->m_pGameObjectManager->FindGameObectBinary(m_ClickedObjectID);
			if (m_SelectedObject != nullptr)
			{
				m_SelectedObject->GetComponent<MeshRenderer>()->SetSelected(true);
			}
		}
	}
}

void Engine::RenderAll()
{
	m_pRenderer->RenderOptionUpdate(*m_RenderOption);

	m_pComponent->PreRender();

	m_pRenderer->BeginDraw();

	m_pComponent->Render();

	m_pRenderer->RenderQueueProcess();

	m_pRenderer->UIPassBind();

	m_pComponent->UIRender();

	if (m_RenderOption->bDebugRenderMode)
	{
		m_pRenderer->DebugDraw(true);	// IMGUI�� �׸��� ���� ����Ÿ���� ���� ����

		m_pRenderer->HelperDraw();
	}

	m_pNavMeshManager->Render();
	
	if (m_RenderOption->bDebugRenderMode)
	{
		m_pComponent->HelperRender();

		if (m_bImguiActive == true)
		{
			m_EngineImgui->Render();
		}
	}

	m_pRenderer->EndDraw();

	m_isResize = false;

}


void Engine::ActiveImgui(bool use)
{
	m_bImguiActive = use;
}

void Engine::ActiveAudio(bool use)
{
	m_bAudioActive = use;
}

void Engine::AddScene(std::string name, SceneBase* pScene)
{
	Managers::GetInstance()->GetSceneManager()->AddScene(name, pScene);
}

void Engine::SelectScene(std::string name)
{
	if (Managers::GetInstance()->GetSceneManager()->GetNowScene() != nullptr)
	{
		if (Managers::GetInstance()->GetSceneManager()->GetNowScene()->GetSceneName() == name)
		{
			return;
		}
	}

	Managers::GetInstance()->GetSceneManager()->SetNowScene(name);
}

void Engine::Finalize()
{
	m_pRenderer->Destroy();
	m_pRenderer.reset();
	DLLAudio::Release();
}

void Engine::ResetRenderer()
{
	m_pRenderer->ResetRenderer();
	m_SelectedObject = nullptr;
	m_ClickedObjectID = 0;
}


Input* Engine::GetInput()
{
	return m_pInput;
}

void Engine::InputUpdate(float dTime)
{
	m_pInput->KeyUpdate();
	m_pInput->MouseUpdate();
}

void Engine::DeferredPicking()
{
	m_pRenderer->DeferredPickingPass();
}

void Engine::OnResize(int width, int height)
{
	m_ClientWidth = width;
	m_ClientHeight = height;

	m_isResize = true;

	if (Managers::GetInstance()->GetCameraManager() != nullptr)
	{
		Managers::GetInstance()->GetCameraManager()->SetLensOnResize(width, height);
	}

	if (m_pRenderer != nullptr)
	{
		m_pRenderer->OnResize(width, height);
	}

}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT Engine::ImGuiHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	return ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam);
}

HWND& Engine::GetWindowHandle()
{
	return m_hWnd;
}

std::shared_ptr<IRenderer> Engine::GetIRenderer() const
{
	return m_pRenderer;
}

IRenderOption* Engine::GetRenderOption()
{
	return m_RenderOption;
}

ResourceLoader* Engine::GetResoruceLoader()
{
	return m_pResourceLoader;
}

struct DeferredInfo* Engine::GetDeferredInfoFromEngine()
{
	return m_pDeferredInfo;
}

std::shared_ptr<IResourceManager> Engine::GetResourceManager() const
{
	return m_pResourceManager;
}

bool Engine::GetIsResize()
{
	return m_isResize;
}

std::string& Engine::GetPickedScene()
{
	return m_pickedSceneNmae;
}

void Engine::SetPickedScene(std::string sceneName)
{
	m_pickedSceneNmae = sceneName;
}

