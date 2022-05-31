#include "pch.h"
#include "GameManager.h"
#include "Health.h"
#include "Inventory.h"
#include "Gun.h"
#include "MuzzleFlash.h"
#include "PlayerController.h"

// 변수 초기화
float PlayerController::s_HForAnim = 0.0f;
float PlayerController::s_H = 0.0f;
float PlayerController::s_VForAnim = 0.0f;
float PlayerController::s_V = 0.0f;

bool  PlayerController::s_bCrouch = false;
bool  PlayerController::s_bSprint = false;
bool  PlayerController::s_bAim = false;

bool  PlayerController::s_bIsReloading = false;
bool  PlayerController::s_bIsThrowing = false;
bool  PlayerController::s_bIsHealing = false;

bool  PlayerController::s_bIsSwaping = false;
uint  PlayerController::s_NextSlotNum = 0;

float PlayerController::s_PitchValue = 0.0f;

bool  PlayerController::s_bIsDie = false;
bool  PlayerController::s_bIsDead = false;

PlayerController::PlayerController()
	: ComponentBase(ComponentType::GameLogic)
	, m_Health(nullptr)
	, m_Inventory(nullptr)
	, m_Animator(nullptr)
	, m_MyPhysicsActor(nullptr)

	, m_PlayerMesh(nullptr)

	, m_PoseMode(PoseMode::Stand)
	, m_MovingMode(MovingMode::Idle)

	, m_StandSpeed(0.0f)
	, m_CrouchSpeed(0.0f)
	, m_SprintSpeed(0.0f)
	, m_MoveSpeed(m_StandSpeed)
	, m_SwapSpeed(0.0f)
	, m_SwapCoolTimer(0.0f)
	, m_CameraParent(nullptr)

	// save/load 용 변수
	, m_SaveData(new PlayerController_Save())

{

}

PlayerController::~PlayerController()
{
	Remove();
}

void PlayerController::Start()
{
	/// <summary>
	/// 기본 상태 변수들 초기화
	/// </summary>
	{
		// 이동 애니메이션 관련 설정
		s_HForAnim = 0.0f;
		s_H = 0.0f;
		s_VForAnim = 0.0f;
		s_V = 0.0f;

		s_bAim = false;

		s_bIsReloading = false;
		s_bIsThrowing = false;
		s_bIsHealing = false;
		s_bIsSwaping = false;

		s_PitchValue = 0.0f;

		s_bIsDie = false;
		s_bIsDead = false;

		s_bCrouch = false;
		s_bSprint = false;
		m_PoseMode = PoseMode::Stand;
		m_MovingMode = MovingMode::Idle;

		m_StandSpeed = 7.0f;
		m_CrouchSpeed = m_StandSpeed / 2.0f;
		m_SprintSpeed = m_StandSpeed * 2.0f;
		m_MoveSpeed = m_StandSpeed;
		m_SwapSpeed = 1.5f;
		m_SwapCoolTimer = 0.0f;

		// 전투관련 값 설정
		s_bAim = false;


		// 스타트될 때(재시작 포함) 살아 움직이게 Die 관련은 false로 초기화
		s_bIsDie = false;
		s_bIsDead = false;
	}

	// 커서 화면 중앙으로 이동
	DLLWindow::MoveCursorToCenter();

	/// myObject 컴포넌트 연결
	{
		// Health 연결
		m_Health = m_pMyObject->GetComponent<Health>();
		assert(m_Health != nullptr);
		m_Health->SetHp(6.0f);

		// Inventory 연결
		m_Inventory = m_pMyObject->GetComponent<Inventory>();
		assert(m_Inventory != nullptr);

		// Animator 연결
		if (DLLEngine::FindGameObjectByName("Player_Mesh") != nullptr)
		{
			m_PlayerMesh = DLLEngine::FindGameObjectByName("Player_Mesh");
			m_Animator = m_PlayerMesh->GetComponent<Animator>();
		}
		assert(m_Animator != nullptr);
	}

	// CameraParent 연결
	GameObject* cameraParent = DLLEngine::FindGameObjectByName("CameraParent");
	assert(cameraParent != nullptr);

	m_MyPhysicsActor = m_pMyObject->GetComponent<PhysicsActor>();

	m_CameraParent = cameraParent->m_Transform;
}

void PlayerController::Update(float dTime)
{
	// 테스트 코드(hp 증감)
	if (DLLInput::InputKeyDown(static_cast<int>(CL::KeyCode::KEY_9)))
	{
		int hp = m_Health->GetHp() + 1;
		m_Health->SetHp(hp);
		CA_TRACE("[PlayerController] Hp = %d", hp);
	}

	if (DLLInput::InputKeyDown(static_cast<int>(CL::KeyCode::KEY_0)))
	{
		int hp = m_Health->GetHp() - 1;
		m_Health->SetHp(hp);
		CA_TRACE("[PlayerController] Hp = %d", hp);
	}

	// GameMager 의 s_bTPSMode 가 true 조작한다 
	if (GameManager::IsTPSMode() == true)
	{
		/// 유저 키 입력 반영
		UpdateUserInput();
	}

	/// 생존여부 업데이트
	UpdateAlive();

	// 죽어있으면 바로 리턴
	if (IsDead())
	{
		return;
	}

	/// 장비 교체 쿨타이머
	if (s_bIsSwaping == true)
	{
		SwapCoolDown();
	}

	/// 장전
	if (DLLInput::InputKeyDown(CL::KeyMap::RELOAD))
	{
		Reload();
	}

	/// 장비중인 아이템 사용
	{
		UseEquipedItem();

	}

	/// 이동
	{
		// WASD 전후좌우 이동
		Move();
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


void PlayerController::OnCollisionEnter(Collision collision)
{
	//CA_TRACE("[Player::OnCollisionEnter] otherActor = %s", collision.m_GameObject->GetObjectName().c_str());
}

void PlayerController::OnCollisionStay(Collision collision)
{
	//CA_TRACE("[Player::OnCollisionStay] otherActor = %s", collision.m_GameObject->GetObjectName().c_str());
}

void PlayerController::OnCollisionExit(Collision collision)
{
	//CA_TRACE("[Player::OnCollisionExit] otherActor = %s", collision.m_GameObject->GetObjectName().c_str());
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

		// 이동속도 변경
		if (m_pMyObject->GetComponent<PhysicsActor>())
		{
			m_MoveSpeed = m_SprintSpeed;
		}

		else
		{
			m_MoveSpeed = m_SprintSpeed; // 피직스 액터가 없이 트랜스폼 이동할 때 기본 달리기속도 적용
		}
	}

	// 달리기 버튼이나 전진 키를 떼면
	else if (DLLInput::InputKeyUp(CL::KeyMap::SPRINT) || DLLInput::InputKeyUp(CL::KeyMap::FORWARD))
	{
		// 달리기 상태 해제
		s_bSprint = false;
	}

	/// 조준 
	if (CanAim() == true
		&& DLLInput::InputKey(CL::KeyMap::AIM) == true)
	{
		// 조준 상태 설정
		s_bAim = true;

		// 달리기 항상 off
		s_bSprint = false;

		//m_Animator->SetOverrideAnimLayer("");

		// 이동 상태, 애니메이션 적용
		if (m_PoseMode == PoseMode::Stand)
		{
			m_MoveSpeed = m_StandSpeed;

			// 활성화 하고 싶은 레이어 이름만 가지고 함수 호출
			m_Animator->SetOverrideAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE_AIM);
			//m_Animator->SetNoneAnimLayer("");

			if (s_H == 0.0f)
			{
				// Idle이나 앞뒤로 움직일 때 살짝 오른쪽으로 회전
				m_PlayerMesh->m_Transform->SetRotationFromVec({ 0.0f, 240.0f, 0.0f });
			}

			else
			{
				// 좌우로 움직일 때는 기본 방향 그대로
				m_PlayerMesh->m_Transform->SetRotationFromVec({ 0.0f, 190.0f, 0.0f });
			}
		}

		else if (m_PoseMode == PoseMode::Crouch)
		{
			m_MoveSpeed = m_CrouchSpeed;

			m_Animator->SetOverrideAnimLayer("");
			m_PlayerMesh->m_Transform->SetRotationFromVec({ 0.0f, 190.0f, 0.0f });
		}
	}

	/// 비조준
	else
	{
		s_bAim = false;

		if (IsNoAction() == true)
		{
			m_Animator->SetOverrideAnimLayer("");

			m_PlayerMesh->m_Transform->SetRotationFromVec({ 0.0f, 190.0f, 0.0f });
			//m_Animator->SetNoneAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE);
		}
	}

	/// 장비 교체 (로직)
	if (CanSwap() == true)
	{
		/// 교체할 슬롯 키를 눌렀을 경우
		// 장총(1번) 교체 시도 
		if (DLLInput::InputKeyDown(static_cast<int>(CL::KeyMap::SLOT_1)))
		{
			// 현재 장비중인 슬롯인지 확인
			uint _currentSlotNum = m_Inventory->m_EquiptedSlotIndex;
			if (_currentSlotNum != static_cast<uint>(Inventory::Slot::eLongGun))
			{
				// 장비 교체 중인 상태로 변경
				s_bIsSwaping = true;
				m_SwapCoolTimer = m_SwapSpeed;

				// 교체후 장비할 슬롯 번호 변경
				s_NextSlotNum = static_cast<uint>(Inventory::Slot::eLongGun);

				// 애니메이터 변수 변경
				s_bIsSwaping = true;

				// 애니메이션 변경
				m_Animator->SetOverrideAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE_SWAP_WEAPON);

				CA_TRACE("[PlayerController] 장비교체 시작");
			}

			else
			{
				CA_TRACE("[PlayerController] 장비교체 실패 - > 현재 장비한 아이템이다..");
			}
		}

		// 권총(2번) 교체 시도 
		if (DLLInput::InputKeyDown(static_cast<int>(CL::KeyMap::SLOT_2)))
		{
			// 현재 장비중인 슬롯인지 확인
			uint _currentSlotNum = m_Inventory->m_EquiptedSlotIndex;
			if (_currentSlotNum != static_cast<uint>(Inventory::Slot::eHandGun))
			{
				// 장비 교체 중인 상태로 변경
				s_bIsSwaping = true;
				m_SwapCoolTimer = m_SwapSpeed;

				// 교체후 장비할 슬롯 번호 변경
				s_NextSlotNum = static_cast<uint>(Inventory::Slot::eHandGun);

				// 애니메이션 변경
				m_Animator->SetOverrideAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE_SWAP_WEAPON);

				CA_TRACE("[PlayerController] 장비교체 시작");
			}

			else
			{
				CA_TRACE("[PlayerController] 장비교체 실패 - > 현재 장비한 아이템이다..");
			}
		}

		// 플라스크(3번) 교체 시도 
		if (DLLInput::InputKeyDown(static_cast<int>(CL::KeyMap::SLOT_3)))
		{
			// 현재 장비중인 슬롯인지 확인
			uint _currentSlotNum = m_Inventory->m_EquiptedSlotIndex;
			if (_currentSlotNum != static_cast<uint>(Inventory::Slot::eFlask))
			{
				// 장비 교체 중인 상태로 변경
				s_bIsSwaping = true;
				m_SwapCoolTimer = m_SwapSpeed;

				// 교체후 장비할 슬롯 번호 변경
				s_NextSlotNum = static_cast<uint>(Inventory::Slot::eFlask);

				// 애니메이션 변경
				m_Animator->SetOverrideAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE_SWAP_WEAPON);

				CA_TRACE("[PlayerController] 장비교체 시작");
			}

			else
			{
				CA_TRACE("[PlayerController] 장비교체 실패 - > 현재 장비한 아이템이다..");
			}
		}

		// 화염 플라스크(4번) 교체 시도 
		if (DLLInput::InputKeyDown(static_cast<int>(CL::KeyMap::SLOT_4)))
		{
			// 현재 장비중인 슬롯인지 확인
			uint _currentSlotNum = m_Inventory->m_EquiptedSlotIndex;
			if (_currentSlotNum != static_cast<uint>(Inventory::Slot::eFireFlask))
			{
				// 장비 교체 중인 상태로 변경
				s_bIsSwaping = true;
				m_SwapCoolTimer = m_SwapSpeed;

				// 교체후 장비할 슬롯 번호 변경
				s_NextSlotNum = static_cast<uint>(Inventory::Slot::eFireFlask);

				// 애니메이션 변경
				m_Animator->SetOverrideAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE_SWAP_WEAPON);

				CA_TRACE("[PlayerController] 장비교체 시작");
			}

			else
			{
				CA_TRACE("[PlayerController] 장비교체 실패 - > 현재 장비한 아이템이다..");
			}
		}
	}

}

void PlayerController::UpdateAlive()
{
	if (m_Health->IsAlive())
	{
		s_bIsDie = false;
	}

	else
	{
		s_bIsDie = true;
	}
}

void PlayerController::Move()
{
	// PhysicsActor 컴포넌트가 있으면 물리로 움직인다.
	if (GetMyObject()->GetComponent<PhysicsActor>())
	{
		MoveByPhysX();
	}

	// PhysicsActor 이 없는 상태면 기본 트랜스폼으로 이동한다.
	else
	{
		MoveByTransform();
	}
}

/// <summary>
/// physX 물리가 아닌 Transform을 적용해 이동하는 함수
/// 대각선으로 이동할 때 이동속도 보정이 필요하다.(아직 미적용)
/// </summary>
void PlayerController::MoveByTransform()
{
	/// <summary>
	///  전후좌우 이동
	/// </summary>

	if (s_V > 0.0f)
	{
		// 전방 이동
		m_Transform->SetRotationFromVec({ 0.0f, m_CameraParent->m_Transform->m_EulerAngles.y, 0.0f });
		m_Transform->MoveForwardOnXZ(m_MoveSpeed * CL::Input::s_DeltaTime);

	}

	if (s_V < 0.0f)
	{
		// 후방 이동
		m_Transform->SetRotationFromVec({ 0.0f, m_CameraParent->m_Transform->m_EulerAngles.y, 0.0f });
		m_Transform->MoveForwardOnXZ(-m_MoveSpeed * CL::Input::s_DeltaTime);
	}

	if (s_H < 0.0f)
	{
		// 좌로 이동
		m_Transform->SetRotationFromVec({ 0.0f, m_CameraParent->m_Transform->m_EulerAngles.y, 0.0f });
		m_Transform->MoveSide(-m_MoveSpeed * CL::Input::s_DeltaTime);
	}

	if (s_H > 0.0f)
	{
		// 우로 이동
		m_Transform->SetRotationFromVec({ 0.0f, m_CameraParent->m_Transform->m_EulerAngles.y, 0.0f });
		m_Transform->MoveSide(m_MoveSpeed * CL::Input::s_DeltaTime);
	}
}

/// <summary>
/// physX 물리를 이용해서 움직이는 함수
/// Object에 PhysicsActor 컴포넌트가 있을 때 사용
/// </summary>
void PlayerController::MoveByPhysX()
{
	/// 키네마틱 설정 되어있으면 게임엔진 트랜스폼으로 이동하고 피직스씬에 트랜스폼도 동기화
	if (GetMyObject()->GetComponent<PhysicsActor>()->IsKinematic())
	{
		MoveByTransform();
		DLLEngine::SetPhysicsTransform(m_Transform);
		//DLLEngine::SetTransform(GetMyObject(), *m_Transform);

		return;
	}

	/// 키네막틱 false 일때 물리이동
	// 카메라 방향 
	//Vector3 _cameraForwardDir = m_CameraParent->m_Transform->m_RotationTM.Forward();
	Vector3 _cameraForwardDir = m_CameraParent->m_Transform->m_WorldTM.Forward();
	Vector3 _cameraRightDir = m_CameraParent->m_Transform->m_WorldTM.Right();

	// 계산할 이동 방향
	Vector3 _moveDir = Vector3::Zero;

	/// <summary>
	///  전후좌우 이동
	/// </summary>
	if (s_V > 0.0f)
	{
		// 전방 이동
		_moveDir += { _cameraForwardDir.x, 0.0f, _cameraForwardDir.z};
	}

	if (s_V < 0.0f)
	{
		// 후방 이동
		_moveDir -= { _cameraForwardDir.x, 0.0f, _cameraForwardDir.z};;
	}

	if (s_H < 0.0f)
	{
		// 좌로 이동
		_moveDir -= { _cameraRightDir.x, 0.0f, _cameraRightDir.z};
	}

	if (s_H > 0.0f)
	{
		// 우로 이동
		_moveDir += { _cameraRightDir.x, 0.0f, _cameraRightDir.z};
	}

	// 이동방향 정규화
	_moveDir.Normalize();

	// 이동량 계산
	_moveDir *= m_MoveSpeed * CL::Input::s_DeltaTime;

	// 이동
	DLLEngine::SetVelocity(m_MyPhysicsActor, _moveDir);

	//float _nowMoveScale = abs(_moveDir.x) + abs(_moveDir.y) + abs(_moveDir.z);
	//CA_TRACE("Move Scale = %f", _nowMoveScale);

	// Look 방향 전환
	m_Transform->SetRotationFromVec({ 0.0f, m_CameraParent->m_Transform->m_EulerAngles.y, 0.0f });
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

		//CA_TRACE("[PlayerController] Now Mode : Idle / Now MoveSpeed = %f", m_MoveSpeed);
		break;
	}


	case PoseMode::Crouch:
	{
		m_MoveSpeed = m_CrouchSpeed;		// Crouch 이동 속도

		//CA_TRACE("[PlayerController] Now Mode : Crouch / Now MoveSpeed = %f", m_MoveSpeed);
		break;
	}

	case PoseMode::Max:
		break;
	default:
		break;
	}

	return m_PoseMode;
}

bool PlayerController::UseEquipedItem() const
{
	// 비조준 시 사용(공격, 투척)키 눌렀을 때
	if (DLLInput::InputKeyDown(CL::KeyMap::ATTACK))
	{
		// 장전
		Reload();

		if (DLLInput::InputKey(CL::KeyMap::AIM))
		{
			m_Inventory->m_EquipedItem->Use();

			return true;
		}

	}

	return false;
}

bool PlayerController::EquipMainSlot()
{
	// 현재 장비중인 슬롯인지 확인
	uint _currentSlotNum = m_Inventory->m_EquiptedSlotIndex;
	if (_currentSlotNum != static_cast<uint>(Inventory::Slot::eLongGun))
	{
		// 장비 교체 중인 상태로 변경
		s_bIsSwaping = true;
		m_SwapCoolTimer = m_SwapSpeed;

		// 교체후 장비할 슬롯 번호 변경
		s_NextSlotNum = static_cast<uint>(Inventory::Slot::eLongGun);
	}

	else
	{
		CA_TRACE("[PlayerController] 장비교체 실패 - > 현재 장비한 아이템이다..");
	}
	return true;
}

void PlayerController::SwapCoolDown()
{
	m_SwapCoolTimer -= CL::Input::s_DeltaTime;

	if (m_SwapCoolTimer <= 0.0f)
	{
		s_bIsSwaping = false;
		CA_TRACE("[PlayerController] 장비교체 타이머 끝");

		// 무기교체 애니메이션 해제
		m_Animator->SetOverrideAnimLayer("");

		// 인벤토리의 장비 슬롯 교체
		// 나중에 애니메이션 이벤트에서 애니메이션이 중간쯤 진행됐을 때 변경하도록 해야함
		m_Inventory->ChangeWeapon(s_NextSlotNum);
	}
}

/// <summary>
/// 현재 총 장비중이고 탄창이 비어있으면 장전한다.
/// </summary>
void PlayerController::Reload() const
{
	std::shared_ptr<Gun> _gun = std::dynamic_pointer_cast<Gun>(m_Inventory->GetCurrentEquipment());
	if (_gun != nullptr)
	{
		if (_gun->IsMagazineEmpty() == true
			&& s_bIsReloading == false)
		{
			_gun->StartReload();

			switch (m_PoseMode)
			{
			case PoseMode::Stand:
				m_Animator->SetOverrideAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE_STAND_RELOAD);
				break;

			case PoseMode::Crouch:
				m_Animator->SetOverrideAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE_CROUCH_RELOAD);
				break;
			}
			
		}
	}
}

void PlayerController::FinishDie()
{
	s_bIsDie = false;	// 쓰러지고 있는 상태 해제
	s_bIsDead = true;	// 죽어서 누워있는상태 true
}

/// <summary>
/// 플레이어 사망 판단
/// </summary>
/// <returns></returns>
bool PlayerController::IsDead()
{
	// 쓰러지고 있거나, 이미 죽어서 누워있는 경우
	if (s_bIsDie == true || s_bIsDead == true)
	{
		return true;
	}

	// Alive
	return false;
}

/// <summary>
/// 현재 조준 가능한가 판단
/// </summary>
/// <returns>판단 여부</returns>
bool PlayerController::CanAim() const
{
	if (s_bIsReloading == false
		&& s_bIsThrowing == false
		&& s_bIsHealing == false)
	{
		return true;
	}

	else
	{
		return false;
	}
}

/// <summary>
/// 현재 무기 스왑이 가능한지 판단
/// </summary>
/// <returns>스왑 가능 여부</returns>
bool PlayerController::CanSwap() const
{
	if (s_bIsSwaping == false && s_bSprint == false)
	{
		//CA_TRACE("[PlayerController] Now, Can Swap ~");
		return true;
	}

	else
	{
		//CA_TRACE("[PlayerController] Can't Swap, yet...");
		return false;
	}
}

bool PlayerController::IsNoAction() const
{
	if (m_Animator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE_STAND_RELOAD)->m_bEnabled == false
		&& m_Animator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE_CROUCH_RELOAD)->m_bEnabled == false
		&& m_Animator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE_SWAP_WEAPON)->m_bEnabled == false)
	{
		return true;
	}

	//else
	{
		return false;
	}
}

void PlayerController::DebugRender()
{
	std::string _hitObject = "hitObj = ";

	/*
	if (m_LastHitTarget)
	{
		_hitObject += m_LastHitTarget->GetObjectName();
		_hitObject += "\n HP = ";

		if (m_LastHitTarget->GetComponent<Health>() != nullptr)
		{
			float hp = m_LastHitTarget->GetComponent<Health>()->GetHp();
			_hitObject += std::to_string(hp);
		}

	}
	// 변환용 변수
	TCHAR* wstr = StringHelper::StringToWchar(_hitObject);

	Text* text = m_pMyObject->GetComponent<Text>();
	text->PrintSpriteText((TCHAR*)L"%s", wstr);

	SAFE_DELETE(wstr);
	*/


	// 테스트 레이 렌더 (버그)
	//DLLEngine::DrawRay(m_Transform->m_Position, m_Transform->m_RotationTM.Forward(), 100.0f);
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

}
