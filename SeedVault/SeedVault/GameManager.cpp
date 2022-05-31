#include "pch.h"
#include "Zombie_Runner_Move.h"
#include "CameraController.h"
#include "GameManager.h"

bool GameManager::s_bDebugMode = false;
bool GameManager::s_bIsUIVisible = true;
bool GameManager::s_bTPSMode = true;

/// <summary>
/// 현재 디버그모드 상태값 리턴
/// </summary>
/// <returns></returns>
bool GameManager::IsDebugMode()
{
	return s_bDebugMode;
}

/// <summary>
/// 디버그 모드 설정
/// (엔진의 모드도 같이 설정한다)
/// </summary>
/// <param name="val"></param>
void GameManager::SetDebugMode(const bool& val)
{
	// 클라이언트 디버그모드 설정
	s_bDebugMode = val;

	// 엔진 디버그모드 설정
	DLLEngine::SetDebugRenderMode(val);
}

/// <summary>
/// 디버깅 모드 여부를 전환(반전)시키고 결과를 리턴한다.
/// 디버그 모드가 On(true)면 디버깅 정보를 그리도록 하자
/// </summary>
bool GameManager::TogleDebugMode()
{
	s_bDebugMode = !s_bDebugMode;

	return s_bDebugMode;
}

GameManager::GameManager()
	: ComponentBase(ComponentType::Etc)
	, m_GameState(GameState::Play)
	, m_Monster_1_Name(nullptr)
	, m_Monster_1_State(nullptr)

	, m_CrossHead(nullptr)

	, m_SaveData(new GameManager_Save())
{
	CA_TRACE("[GameManager] Create!");
}

GameManager::~GameManager()
{
	CA_TRACE("[GameManager] Delete!");

	SAFE_DELETE(m_SaveData);


	// 다른 씬에서 혹시 영향을 받을까봐 초기화
	s_bDebugMode = false;
	s_bIsUIVisible = true;
	s_bTPSMode = false;
}

void GameManager::Start()
{
	// 게임 모드(아직 의미 없음)
	m_GameState = GameState::Play;

	// 디버깅모드 설정
	s_bDebugMode = false;

	// UI 렌더 여부
	s_bIsUIVisible = true;

	// TPS 플레이어 컨트롤 모드
	s_bTPSMode = true;
	ShowCursor(false);

	// UI 오브젝트 연결
	ConnectUI();
}

void GameManager::Update(float dTime)
{
	if (DLLInput::InputKeyDown(CL::KeyMap::DEBUG))
	{
		SetDebugMode(!s_bDebugMode);
	}


	if (DLLInput::InputKeyDown(static_cast<int>(CL::KeyCode::KEY_ESCAPE)))
	{
		s_bTPSMode = !s_bTPSMode;

		if (IsTPSMode() == true)
		{
			// 윈도우 기준 화면 중앙 좌표
			POINT _centerPos = { DLLWindow::GetScreenWidth() / 2, DLLWindow::GetScreenHeight() / 2 };

			HWND _hWnd = DLLWindow::GetWindowHandle();
			ClientToScreen(_hWnd, &_centerPos);
			SetCursorPos(_centerPos.x, _centerPos.y);	// 윈도우 내부 좌표가아니라 스크린(주 모니터)좌표계임...


			ShowCursor(false);

			DLLEngine::SetNowCamera("MainCamera");

			// 플레이어 고정
			GameObject* _player = DLLEngine::FindGameObjectByName("Player");
			_player->GetComponent<PhysicsActor>()->SetKinematic(false);
		}

		else
		{
			ShowCursor(true);

			// 프리캠 포지션 메인캠위치로 이동
			Transform* MainCameraTf = DLLEngine::FindGameObjectByName("MainCamera")->m_Transform;
			Transform* CameraParentTf = DLLEngine::FindGameObjectByName("CameraParent")->m_Transform;
			
			GameObject* freeCamera = DLLEngine::FindGameObjectByName("FreeCamera");
			CameraController* freeCameraController = freeCamera->GetComponent<CameraController>();

			DLLEngine::SetNowCamera("FreeCamera");
			freeCamera->m_Transform->m_Position = MainCameraTf->GetWorldPosition();				// 메인카메라의 월드 포지션으로 이동
			freeCameraController->SetRotationAngles(CameraParentTf->m_EulerAngles);	// 카메라 페어런트의 Rotation 과 같이 회전

			// 플레이어 고정 해제
			GameObject* _player = DLLEngine::FindGameObjectByName("Player");
			_player->GetComponent<PhysicsActor>()->SetKinematic(true);
		}
	}

	// 마우스 커서 가두기
	if (s_bTPSMode == true)
	{
		//LockingMouseCursor();
	}

	// UI 가시 모드일 때만 렌더 
	if (s_bIsUIVisible == true)
	{
		UIRender();
	}

}

void GameManager::OnRender()
{

}

bool GameManager::IsTPSMode()
{
	return s_bTPSMode;
}


/// <summary>
/// UI 오브젝트들 렌더
/// </summary>
void GameManager::UIRender()
{
	
	// Name Text 오브젝트 제대로 받아왔는지 확인
	if (m_Monster_1_Name != nullptr)
	{
		//m_Monster_1_Name->ResetBuffer();
		std::string strName = "Agnet : ";
		GameObject* obj = DLLEngine::FindGameObjectByName("Zombie_Runner1");
		if (obj != nullptr)
		{
			strName += obj->GetObjectName();
		}
	
		// 변환용 변수
		TCHAR* wstr = StringHelper::StringToWchar(strName);
	
		m_Monster_1_Name->PrintSpriteText((TCHAR*)L"%s", wstr);
		SAFE_DELETE(wstr);	/// new는 엔진에서 하는데 delete는 클라가?? 흐음...??
	
		// State Text 오브젝트 제대로 받아왔는지 확인
		if (m_Monster_1_State != nullptr)
		{
			std::string strState = "State : ";
	
			// Enemy의 상태를 얻어옴
			uint state = obj->GetComponent<Zombie_Runner_Move>()->m_State;
	
			/// <summary>
			/// 각 상태체크해서 UI에 보여줄 text 구성
			/// 여러개의 상태비트가 세트되어 있을 수 있으니 우선순위를 정해야 함!
			/// </summary>
			if (state & Zombie_Runner_Move::State::eHunt)			// 우선순위 1
			{
				strState += "Hunt";
			}
	
			else if (state & Zombie_Runner_Move::State::ePatrol)	// 우선순위 2
			{
				strState += "Patrol";
			}
			else if (state & Zombie_Runner_Move::State::eWait)	// 우선순위 3
			{
				strState += "Wait";
			}
	
			TCHAR* wstr2 = StringHelper::StringToWchar(strState);
			m_Monster_1_State->PrintSpriteText((TCHAR*)L"%s", wstr2);
			SAFE_DELETE(wstr2);
		}
	}
	

	if (m_CrossHead != nullptr)
	{
		// 크로스헤드 위치를 화면 가운데로 계산해서 설정
		// 현재 엔진에서 그려줄 때 이상한 곳에 그리고 있음.. (엔진에서 뭔가 보정값을 이용한다는데, 진미가 봐준다고 함)
		float _x = static_cast<float>(DLLWindow::GetScreenWidth()) / 2.0f;
		float _y = static_cast<float>(DLLWindow::GetScreenHeight()) / 2.0f;
		m_CrossHead->m_Transform->SetPosition({ _x, _y, 0.0f });
	}
	
}


/// <summary>
/// 매니져에서 UI를 관리하고 렌더할 수 있게 
/// UI 오브젝트 연결
/// </summary>
void GameManager::ConnectUI()
{
	GameObject* uiObj = nullptr;

	uiObj = DLLEngine::FindGameObjectByName("txtMonster_1_Name");
	if (uiObj != nullptr)
	{
		m_Monster_1_Name = uiObj->GetComponent<Text>();
	}

	uiObj = nullptr;
	uiObj = DLLEngine::FindGameObjectByName("txtMonster_1_State");
	if (uiObj != nullptr)
	{
		m_Monster_1_State = uiObj->GetComponent<Text>();
	}

	uiObj = nullptr;
	uiObj = DLLEngine::FindGameObjectByName("CrossHead");
	if (uiObj != nullptr)
	{
		m_CrossHead = uiObj->GetComponent<Sprite2D>();
	}
}

/// <summary>
/// 플레이어 컨트롤 중일 때 마우스 커서가 윈도우 밖으로 나가는 것을 방지하기 위해
/// 커서 위치를 윈도우의 가운데로 맞춘다.
/// </summary>
void GameManager::LockingMouseCursor()
{
	POINT _centerPos = { DLLWindow::GetScreenWidth() / 2, DLLWindow::GetScreenHeight() / 2 };

	HWND _hWnd = DLLWindow::GetWindowHandle();
	// 윈도우 좌표를 스크린 좌표로 변환
	ClientToScreen(_hWnd, &_centerPos);

	// 마우스 커서를 윈도우의 중앙으로 설정
	SetCursorPos(_centerPos.x, _centerPos.y);	// 윈도우 내부 좌표가아니라 스크린(주 모니터)좌표계임...
}


void GameManager::SaveData()
{
	m_SaveData->m_bEnable = GetIsEnabled();
	m_SaveData->m_ComponentId = m_ComponentId;

	m_SaveData->m_GameState = static_cast<int>(m_GameState);

	m_SaveData->s_bIsUIVisible = s_bIsUIVisible;

	m_SaveData->s_bDebugMode = s_bDebugMode;
	m_SaveData->s_bTPSMode = s_bTPSMode;

	save::WriteValue(m_Value, EraseClass(typeid(*this).name()), *m_SaveData);
}

void GameManager::LoadData()
{
	save::ReadValue(m_Value, EraseClass(typeid(*this).name()), *m_SaveData);
	SetEnable(m_SaveData->m_bEnable);

	m_GameState = static_cast<GameState>(m_SaveData->m_GameState);

	s_bIsUIVisible = m_SaveData->s_bIsUIVisible;

	s_bDebugMode = m_SaveData->s_bDebugMode;
	s_bTPSMode = m_SaveData->s_bTPSMode;
}