#include "pch.h"
#include "Zombie_Runner_Move.h"
#include "CameraController.h"
#include "GameManager.h"

bool GameManager::s_bDebugMode = false;
bool GameManager::s_bIsUIVisible = true;
bool GameManager::s_bTPSMode = true;

/// <summary>
/// ���� ����׸�� ���°� ����
/// </summary>
/// <returns></returns>
bool GameManager::IsDebugMode()
{
	return s_bDebugMode;
}

/// <summary>
/// ����� ��� ����
/// (������ ��嵵 ���� �����Ѵ�)
/// </summary>
/// <param name="val"></param>
void GameManager::SetDebugMode(const bool& val)
{
	// Ŭ���̾�Ʈ ����׸�� ����
	s_bDebugMode = val;

	// ���� ����׸�� ����
	DLLEngine::SetDebugRenderMode(val);
}

/// <summary>
/// ����� ��� ���θ� ��ȯ(����)��Ű�� ����� �����Ѵ�.
/// ����� ��尡 On(true)�� ����� ������ �׸����� ����
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


	// �ٸ� ������ Ȥ�� ������ ������� �ʱ�ȭ
	s_bDebugMode = false;
	s_bIsUIVisible = true;
	s_bTPSMode = false;
}

void GameManager::Start()
{
	// ���� ���(���� �ǹ� ����)
	m_GameState = GameState::Play;

	// ������� ����
	s_bDebugMode = false;

	// UI ���� ����
	s_bIsUIVisible = true;

	// TPS �÷��̾� ��Ʈ�� ���
	s_bTPSMode = true;
	ShowCursor(false);

	// UI ������Ʈ ����
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
			// ������ ���� ȭ�� �߾� ��ǥ
			POINT _centerPos = { DLLWindow::GetScreenWidth() / 2, DLLWindow::GetScreenHeight() / 2 };

			HWND _hWnd = DLLWindow::GetWindowHandle();
			ClientToScreen(_hWnd, &_centerPos);
			SetCursorPos(_centerPos.x, _centerPos.y);	// ������ ���� ��ǥ���ƴ϶� ��ũ��(�� �����)��ǥ����...


			ShowCursor(false);

			DLLEngine::SetNowCamera("MainCamera");

			// �÷��̾� ����
			GameObject* _player = DLLEngine::FindGameObjectByName("Player");
			_player->GetComponent<PhysicsActor>()->SetKinematic(false);
		}

		else
		{
			ShowCursor(true);

			// ����ķ ������ ����ķ��ġ�� �̵�
			Transform* MainCameraTf = DLLEngine::FindGameObjectByName("MainCamera")->m_Transform;
			Transform* CameraParentTf = DLLEngine::FindGameObjectByName("CameraParent")->m_Transform;
			
			GameObject* freeCamera = DLLEngine::FindGameObjectByName("FreeCamera");
			CameraController* freeCameraController = freeCamera->GetComponent<CameraController>();

			DLLEngine::SetNowCamera("FreeCamera");
			freeCamera->m_Transform->m_Position = MainCameraTf->GetWorldPosition();				// ����ī�޶��� ���� ���������� �̵�
			freeCameraController->SetRotationAngles(CameraParentTf->m_EulerAngles);	// ī�޶� ��Ʈ�� Rotation �� ���� ȸ��

			// �÷��̾� ���� ����
			GameObject* _player = DLLEngine::FindGameObjectByName("Player");
			_player->GetComponent<PhysicsActor>()->SetKinematic(true);
		}
	}

	// ���콺 Ŀ�� ���α�
	if (s_bTPSMode == true)
	{
		//LockingMouseCursor();
	}

	// UI ���� ����� ���� ���� 
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
/// UI ������Ʈ�� ����
/// </summary>
void GameManager::UIRender()
{
	
	// Name Text ������Ʈ ����� �޾ƿԴ��� Ȯ��
	if (m_Monster_1_Name != nullptr)
	{
		//m_Monster_1_Name->ResetBuffer();
		std::string strName = "Agnet : ";
		GameObject* obj = DLLEngine::FindGameObjectByName("Zombie_Runner1");
		if (obj != nullptr)
		{
			strName += obj->GetObjectName();
		}
	
		// ��ȯ�� ����
		TCHAR* wstr = StringHelper::StringToWchar(strName);
	
		m_Monster_1_Name->PrintSpriteText((TCHAR*)L"%s", wstr);
		SAFE_DELETE(wstr);	/// new�� �������� �ϴµ� delete�� Ŭ��?? ����...??
	
		// State Text ������Ʈ ����� �޾ƿԴ��� Ȯ��
		if (m_Monster_1_State != nullptr)
		{
			std::string strState = "State : ";
	
			// Enemy�� ���¸� ����
			uint state = obj->GetComponent<Zombie_Runner_Move>()->m_State;
	
			/// <summary>
			/// �� ����üũ�ؼ� UI�� ������ text ����
			/// �������� ���º�Ʈ�� ��Ʈ�Ǿ� ���� �� ������ �켱������ ���ؾ� ��!
			/// </summary>
			if (state & Zombie_Runner_Move::State::eHunt)			// �켱���� 1
			{
				strState += "Hunt";
			}
	
			else if (state & Zombie_Runner_Move::State::ePatrol)	// �켱���� 2
			{
				strState += "Patrol";
			}
			else if (state & Zombie_Runner_Move::State::eWait)	// �켱���� 3
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
		// ũ�ν���� ��ġ�� ȭ�� ����� ����ؼ� ����
		// ���� �������� �׷��� �� �̻��� ���� �׸��� ����.. (�������� ���� �������� �̿��Ѵٴµ�, ���̰� ���شٰ� ��)
		float _x = static_cast<float>(DLLWindow::GetScreenWidth()) / 2.0f;
		float _y = static_cast<float>(DLLWindow::GetScreenHeight()) / 2.0f;
		m_CrossHead->m_Transform->SetPosition({ _x, _y, 0.0f });
	}
	
}


/// <summary>
/// �Ŵ������� UI�� �����ϰ� ������ �� �ְ� 
/// UI ������Ʈ ����
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
/// �÷��̾� ��Ʈ�� ���� �� ���콺 Ŀ���� ������ ������ ������ ���� �����ϱ� ����
/// Ŀ�� ��ġ�� �������� ����� �����.
/// </summary>
void GameManager::LockingMouseCursor()
{
	POINT _centerPos = { DLLWindow::GetScreenWidth() / 2, DLLWindow::GetScreenHeight() / 2 };

	HWND _hWnd = DLLWindow::GetWindowHandle();
	// ������ ��ǥ�� ��ũ�� ��ǥ�� ��ȯ
	ClientToScreen(_hWnd, &_centerPos);

	// ���콺 Ŀ���� �������� �߾����� ����
	SetCursorPos(_centerPos.x, _centerPos.y);	// ������ ���� ��ǥ���ƴ϶� ��ũ��(�� �����)��ǥ����...
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