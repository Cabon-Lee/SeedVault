#include "pch.h"
#include "GameManager.h"
#include "Health.h"
#include "PlayerController.h"

// 변수 초기화
float PlayerController::s_HForAnim = 0.0f;
float PlayerController::s_H = 0.0f;
float PlayerController::s_VForAnim = 0.0f;
float PlayerController::s_V = 0.0f;

bool  PlayerController::s_bCrouch = false;
bool  PlayerController::s_bSprint = false;
bool  PlayerController::s_bAim = false;

bool  PlayerController::s_bIsAttack = false;
float PlayerController::s_AttackSpeed = 1.0f;
float PlayerController::s_AttackCollTimeTimer = 0.0f;
float PlayerController::s_AttackRange = 0.0f;

float PlayerController::s_PitchValue = 0.0f;

bool  PlayerController::s_bSwaping = false;
float PlayerController::s_SwapTime = 1.0f;

PlayerController::PlayerController()
	: ComponentBase(ComponentType::Etc)
	, m_Animator(nullptr)

	, m_PoseMode(PoseMode::Stand)
	, m_MovingMode(MovingMode::Idle)

	, m_StandSpeed(5.0f)
	, m_CrouchSpeed(2.0f)
	, m_SprintSpeed(15.0f)
	, m_MoveSpeed(m_StandSpeed)

	, m_CameraParent(nullptr)

	// save/load 용 변수
	, m_SaveData(new PlayerController_Save())

	// Debug용 
	, m_LastHitTarget(nullptr)
{

}

PlayerController::~PlayerController()
{
	Remove();
}

void PlayerController::Start()
{
	// 커서 화면 중앙으로 이동
	DLLWindow::MoveCursorToCenter();

	// Animator 연결
	if (DLLEngine::FindGameObjectByName("Mesh") != nullptr)
	{
		m_Animator = DLLEngine::FindGameObjectByName("Mesh")->GetComponent<Animator>();
	}

	/// 이동 애니메이션 관련 설정
	m_PoseMode = PoseMode::Stand;
	m_MovingMode = MovingMode::Idle;

	m_MoveSpeed = m_StandSpeed;

	s_bCrouch = false;
	s_bAim = false;

	/// 전투관련 값 설정
	s_bIsAttack = false;
	s_AttackSpeed = 0.5f;	// 공격속도 임시 설정
							// 추후에 장착된 무기에 따라 설정해줘야함.

	s_AttackRange = 100.0f;	// 공격 사거리 설정

	// CameraParent 연결
	GameObject* cameraParent = DLLEngine::FindGameObjectByName("CameraParent");
	assert(cameraParent != nullptr);

	m_CameraParent = cameraParent->m_Transform;

}

void PlayerController::Update(float dTime)
{
	// GameMager 의 s_bTPSMode 가 true 조작한다 
	if (GameManager::IsTPSMode() == false)
	{
		return;
	}

	/// 유저 키 입력 반영
	UpdateUserInput();

	///전투
	{
		// 공격 상태가 설정 되면 공격한다
		if (s_bIsAttack == true)
		{
			// 공격
			Attack();

			// 쿨타임 설정
			s_AttackCollTimeTimer = s_AttackSpeed;
		}

		// 공격 쿨타임 돌리기(식히기)
		if (s_AttackCollTimeTimer >= 0.0f)
		{
			CoolDown();
		}
	}

	/// 이동
	{
		// WASD 전후좌우 이동
		Move();


		// 테스트용 회전 리셋 키
		if (DLLInput::InputKey(static_cast<int>(CL::KeyCode::KEY_P)))
		{
			ResetRotation();
		}
	}

	/// 디버그 정보 렌더
	if (GameManager::IsDebugMode())
	{
		DebugRender();
	}

}

void PlayerController::OnRender()
{
}

void PlayerController::UpdateUserInput()
{
	/// 이동관련 Axis 입력 업데이트
	s_H = CL::Input::GetAxisRaw("Horizontal");
	s_V = CL::Input::GetAxisRaw("Vertical");

	/// 애니메이션 전환용 Axis 업데이트
	/// 역방향 전환을 위해..(앞 <-> 뒤 , 좌 <-> 우)
	s_HForAnim = CL::Input::GetAxis("Horizontal");
	s_VForAnim = CL::Input::GetAxis("Vertical");

	// 앉기 키 누름
	if (DLLInput::InputKeyDown(CL::KeyMap::CROUCH))
	{
		// 서기 <-> 앉기 자세 변경
		s_bCrouch = !s_bCrouch;

		if (s_bCrouch == true)
		{
			SetPoseMode(PoseMode::Crouch);
		}

		else
		{
			SetPoseMode(PoseMode::Stand);
		}
	}

	/// 달리기
	// 달리기 키랑 전진 키를 누르고 있으면
	if (DLLInput::InputKey(CL::KeyMap::SPRINT) && DLLInput::InputKey(CL::KeyMap::FORWARD))
	{
		s_bSprint = true;
		s_bCrouch = false;	// 달릴 때는 무조건 서있게 만든다

		m_MoveSpeed = m_SprintSpeed; // 이동속도 변경
	}

	// 달리기 버튼이나 전진 키를 떼면
	else if (DLLInput::InputKeyUp(CL::KeyMap::SPRINT) || DLLInput::InputKeyUp(CL::KeyMap::FORWARD))
	{
		// Stand 상태로 변경
		s_bSprint = false;

		SetPoseMode(PoseMode::Stand);
	}

	/// 조준 
	if (DLLInput::InputKey(CL::KeyMap::AIM))	// 조준 키 누르고 있는 동안
	{
		// 조준 상태 설정
		s_bAim = true;

		// 달리기 항상 off
		s_bSprint = false;

		// 전방으로 회전
		m_Transform->SetRotationFromVec({ 0.0f, m_CameraParent->m_Transform->m_EulerAngles.y, 0.0f });

		// 이동 상태, 애니메이션 적용
		if (m_PoseMode == PoseMode::Stand)
		{
			m_MoveSpeed = m_StandSpeed;
		}

		else if (m_PoseMode == PoseMode::Crouch)
		{
			m_MoveSpeed = m_CrouchSpeed;
		}

		if (m_Animator != nullptr)
		{
			if (m_Animator->GetAnimLayer("Aiming") != nullptr)
			{
				m_Animator->GetAnimLayer("Aiming")->SetEnabled(true);
			}
		}

		/// 조준 중 공격키 눌리면
		if (DLLInput::InputKeyDown(CL::KeyMap::ATTACK))
		{
			// 공격이 가능하다면 공격 상태로 변경
			if (IsCanAttack())
			{
				s_bIsAttack = true;
			}
		}
	}

	/// 비조준
	else
	{
		s_bAim = false;

		if (m_Animator != nullptr)
		{
			if (m_Animator->GetAnimLayer("Aiming") != nullptr)
			{
				m_Animator->GetAnimLayer("Aiming")->SetEnabled(false);
			}
		}
	}

	/// 무기 교체 샘플
	if (DLLInput::InputKeyDown(static_cast<int>(CL::KeyCode::KEY_1)))
	{
		s_bSwaping = true;

		if (m_Animator != nullptr)
		{
			if (m_Animator->GetAnimLayer("FirstWeapon") != nullptr)
			{
				m_Animator->GetAnimLayer("FirstWeapon")->SetEnabled(true);
			}

			if (m_Animator->GetAnimLayer("SecondWeapon") != nullptr)
			{
				m_Animator->GetAnimLayer("SecondWeapon")->SetEnabled(false);
			}

		}
	}

	if (DLLInput::InputKeyDown(static_cast<int>(CL::KeyCode::KEY_2)))
	{
		s_bSwaping = false;

		if (m_Animator != nullptr)
		{
			if (m_Animator->GetAnimLayer("FirstWeapon") != nullptr)
			{
				m_Animator->GetAnimLayer("FirstWeapon")->SetEnabled(false);
			}

			if (m_Animator->GetAnimLayer("SecondWeapon") != nullptr)
			{
				m_Animator->GetAnimLayer("SecondWeapon")->SetEnabled(true);
			}

		}
	}
}

void PlayerController::Move()
{
	/// <summary>
	///  전후좌우 이동
	/// </summary>

	if (s_V > 0.0f)
	{
		// 전방 이동
		m_Transform->SetRotationFromVec({ 0.0f, m_CameraParent->m_Transform->m_EulerAngles.y, 0.0f });
		m_Transform->MoveForwardOnXZ(m_MoveSpeed * DLLTime::DeltaTime());

		//CA_TRACE("Pos = %f, %f, %f", m_Transform->m_Position.x, m_Transform->m_Position.y, m_Transform->m_Position.z);

		//Vector3 vec = m_Transform->m_RotationTM.Forward();
		//
		//vec = { 0.0f, 0.0f, 1.0f };
		//
		//vec *= m_MoveSpeed;

		//DLLEngine::SetVelocity(m_pMyObject, vec);

		//m_Transform->MoveForwardOnXZ(m_MoveSpeed * DLLTime::DeltaTime());
		//DLLEngine::SetPhysicsTransform(m_Transform);

	}

	if (s_V < 0.0f)
	{
		// 후방 이동
		m_Transform->SetRotationFromVec({ 0.0f, m_CameraParent->m_Transform->m_EulerAngles.y, 0.0f });
		m_Transform->MoveForwardOnXZ(-m_MoveSpeed * DLLTime::DeltaTime());
	}

	if (s_H < 0.0f)
	{
		// 좌로 이동
		m_Transform->SetRotationFromVec({ 0.0f, m_CameraParent->m_Transform->m_EulerAngles.y, 0.0f });
		m_Transform->MoveSide(-m_MoveSpeed * DLLTime::DeltaTime());
	}

	if (s_H > 0.0f)
	{
		// 우로 이동
		m_Transform->SetRotationFromVec({ 0.0f, m_CameraParent->m_Transform->m_EulerAngles.y, 0.0f });
		m_Transform->MoveSide(m_MoveSpeed * DLLTime::DeltaTime());
	}

}


PoseMode PlayerController::SetPoseMode(PoseMode pose)
{
	// PoseMode 변경
	m_PoseMode = pose;

	switch (m_PoseMode)
	{
	case PoseMode::Stand:
	{
		m_MoveSpeed = m_StandSpeed;		// Stand 이동 속도 

		CA_TRACE("[PlayerController] Now Mode : Idle / Now MoveSpeed = %f", m_MoveSpeed);
		break;
	}


	case PoseMode::Crouch:
	{
		m_MoveSpeed = m_CrouchSpeed;		// Crouch 이동 속도

		CA_TRACE("[PlayerController] Now Mode : Crouch / Now MoveSpeed = %f", m_MoveSpeed);
		break;
	}

	case PoseMode::Max:
		break;
	default:
		break;
	}

	return m_PoseMode;
}


/// <summary>
/// 플레이어 캐릭터 회전을 리셋하기 위한 테스트 함수
/// </summary>
void PlayerController::ResetRotation()
{
	this->m_Transform->SetRotationFromVec({ 0.0f, 0.0f });
}

/// <summary>
/// 쿨타임을 체크해서 공격가능한가 판단
/// </summary>
/// <returns></returns>
bool PlayerController::IsCanAttack() const
{
	// 쿨타임이 다 됐으면 
	if (s_AttackCollTimeTimer <= 0.0f)
	{
		// 공격 가능
		return true;
	}

	// 공격 불가능
	return false;
}

/// <summary>
/// 현재 장비한 무기로 공격한다.
/// 총기류라면 사격
/// 투척류라면 투척한다.
/// </summary>
void PlayerController::Attack()
{
	// 임시로직 일단 Ray로 사격되는지..
	CA_TRACE("[PlayerController] Attack!");

	/// Raycast Test
	// 공격 위치(카메라) 찾기
	GameObject* _mainCamera = DLLEngine::FindGameObjectByName("YH_MainCamera");
	assert(_mainCamera != nullptr);

	const Transform& _cameraTf = *_mainCamera->m_Transform;

	// 카메라의 위치에서 카메라의 방향으로 레이를 쏘자.
	Vector3 _attackPosition = _cameraTf.m_WorldPosition;
	Vector3 _attackDirection = _cameraTf.m_WorldTM.Forward();

	/// 충돌 피사체 확인
	RaycastHit _hit;
	_hit = DLLEngine::CheckRaycastHit(_attackPosition, _attackDirection, s_AttackRange);

	// 충돌체가 있으면
	if (_hit.actor != nullptr)
	{
		// 공격한 오브젝트 갱신(디버그용)
		m_LastHitTarget = _hit.actor->GetMyObject();

		Health* _health = _hit.actor->GetMyObject()->GetComponent<Health>();
		if (_health != nullptr)
		{
			_health->Damage(1.0f, _hit.position, _attackDirection, 1.0f, m_pMyObject, _hit.actor);
		}
	}

	// 공격 후 비공격 상태로 돌림
	s_bIsAttack = false;
}

void PlayerController::CoolDown()
{
	s_AttackCollTimeTimer -= DLLTime::DeltaTime();

	if (s_AttackCollTimeTimer <= 0.0f)
	{
		//CA_TRACE("[PlayerController] Finsih Attack CoolDown");
	}
}

void PlayerController::DebugRender()
{
	std::string _hitObject = "hitObj = ";

	if (m_LastHitTarget)
	{
		_hitObject += m_LastHitTarget->GetObjectName();
		_hitObject += "\n HP = ";

		float hp = m_LastHitTarget->GetComponent<Health>()->GetHp();
		_hitObject += std::to_string(hp);

	}
	// 변환용 변수
	TCHAR* wstr = StringHelper::StringToWchar(_hitObject);

	Text* text = m_pMyObject->GetComponent<Text>();
	text->PrintSpriteText((TCHAR*)L"%s", wstr);

	SAFE_DELETE(wstr);


	// 테스트 레이 렌더
	DLLEngine::DrawRay(m_Transform->m_Position, m_Transform->m_RotationTM.Forward(), 100.0f);
}

void PlayerController::SaveData()
{
	m_SaveData->m_bEnable = GetIsEnabled();
	m_SaveData->m_ComponentId = m_ComponentId;

	m_SaveData->s_HForAnim = s_HForAnim;
	m_SaveData->s_VForAnim = s_VForAnim;
	m_SaveData->s_H = s_H;
	m_SaveData->s_V = s_V;

	m_SaveData->s_bCrouch = s_bCrouch;
	m_SaveData->s_bSprint = s_bSprint;
	m_SaveData->s_bAim = s_bAim;

	//m_SaveData->m_Animator = m_Animator->GetComponetId();

	m_SaveData->m_PoseMode = static_cast<int>(m_PoseMode);
	m_SaveData->m_MovingMode = static_cast<int>(m_MovingMode);

	m_SaveData->m_StandSpeed = m_StandSpeed;
	m_SaveData->m_CrouchSpeed = m_CrouchSpeed;
	m_SaveData->m_SprintSpeed = m_SprintSpeed;
	m_SaveData->m_MoveSpeed = m_MoveSpeed;

	m_SaveData->m_CameraParent = m_CameraParent->GetComponetId();

	if (m_LastHitTarget != nullptr)
	{
		m_SaveData->m_LastHitTarget = m_LastHitTarget->GetGameObjectId();
	}

	save::WriteValue(m_Value, EraseClass(typeid(*this).name()), *m_SaveData);
}

void PlayerController::LoadData()
{
	save::ReadValue(m_Value, EraseClass(typeid(*this).name()), *m_SaveData);
	SetEnable(m_SaveData->m_bEnable);

	s_HForAnim = m_SaveData->s_HForAnim;
	s_VForAnim = m_SaveData->s_VForAnim;
	s_H = m_SaveData->s_H;
	s_V = m_SaveData->s_V;
	s_bCrouch = m_SaveData->s_bCrouch;
	s_bSprint = m_SaveData->s_bSprint;
	s_bAim = m_SaveData->s_bAim;

	//m_Animator = m_SaveData->m_Animator;	// 컴포넌트 불러오는건 Start 에서 연결해준다
	m_PoseMode = static_cast<PoseMode>(m_SaveData->m_PoseMode);
	m_MovingMode = static_cast<MovingMode>(m_SaveData->m_MovingMode);

	m_StandSpeed = m_SaveData->m_StandSpeed;
	m_CrouchSpeed = m_SaveData->m_CrouchSpeed;
	m_SprintSpeed = m_SaveData->m_SprintSpeed;
	m_MoveSpeed = m_SaveData->m_MoveSpeed;
}

void PlayerController::LoadPtrData(std::map<unsigned int, GameObject*>* gameobject, std::map<unsigned int, ComponentBase*>* component)
{
	if (m_SaveData->m_LastHitTarget != NULL)
	{
		m_LastHitTarget = dynamic_cast<GameObject*>(gameobject->at(m_SaveData->m_LastHitTarget));
	}
}
