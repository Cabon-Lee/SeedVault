#include "pch.h"
#include "GameManager.h"
#include "Health.h"
#include "Inventory.h"
#include "Gun.h"
#include "MuzzleFlash.h"
#include "PlayerController.h"

// ���� �ʱ�ȭ
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

	// save/load �� ����
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
	/// �⺻ ���� ������ �ʱ�ȭ
	/// </summary>
	{
		// �̵� �ִϸ��̼� ���� ����
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

		// �������� �� ����
		s_bAim = false;


		// ��ŸƮ�� ��(����� ����) ��� �����̰� Die ������ false�� �ʱ�ȭ
		s_bIsDie = false;
		s_bIsDead = false;
	}

	// Ŀ�� ȭ�� �߾����� �̵�
	DLLWindow::MoveCursorToCenter();

	/// myObject ������Ʈ ����
	{
		// Health ����
		m_Health = m_pMyObject->GetComponent<Health>();
		assert(m_Health != nullptr);
		m_Health->SetHp(6.0f);

		// Inventory ����
		m_Inventory = m_pMyObject->GetComponent<Inventory>();
		assert(m_Inventory != nullptr);

		// Animator ����
		if (DLLEngine::FindGameObjectByName("Player_Mesh") != nullptr)
		{
			m_PlayerMesh = DLLEngine::FindGameObjectByName("Player_Mesh");
			m_Animator = m_PlayerMesh->GetComponent<Animator>();
		}
		assert(m_Animator != nullptr);
	}

	// CameraParent ����
	GameObject* cameraParent = DLLEngine::FindGameObjectByName("CameraParent");
	assert(cameraParent != nullptr);

	m_MyPhysicsActor = m_pMyObject->GetComponent<PhysicsActor>();

	m_CameraParent = cameraParent->m_Transform;
}

void PlayerController::Update(float dTime)
{
	// �׽�Ʈ �ڵ�(hp ����)
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

	// GameMager �� s_bTPSMode �� true �����Ѵ� 
	if (GameManager::IsTPSMode() == true)
	{
		/// ���� Ű �Է� �ݿ�
		UpdateUserInput();
	}

	/// �������� ������Ʈ
	UpdateAlive();

	// �׾������� �ٷ� ����
	if (IsDead())
	{
		return;
	}

	/// ��� ��ü ��Ÿ�̸�
	if (s_bIsSwaping == true)
	{
		SwapCoolDown();
	}

	/// ����
	if (DLLInput::InputKeyDown(CL::KeyMap::RELOAD))
	{
		Reload();
	}

	/// ������� ������ ���
	{
		UseEquipedItem();

	}

	/// �̵�
	{
		// WASD �����¿� �̵�
		Move();
	}

	/// ����� ���� ����
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
	/// �̵����� Axis �Է� ������Ʈ
	s_H = CL::Input::GetAxisRaw("Horizontal");
	s_V = CL::Input::GetAxisRaw("Vertical");

	/// �ִϸ��̼� ��ȯ�� Axis ������Ʈ
	/// ������ ��ȯ�� ����..(�� <-> �� , �� <-> ��)
	s_HForAnim = CL::Input::GetAxis("Horizontal");
	s_VForAnim = CL::Input::GetAxis("Vertical");

	// �ɱ� Ű ����
	if (DLLInput::InputKeyDown(CL::KeyMap::CROUCH))
	{
		// ���� <-> �ɱ� �ڼ� ����
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

	/// �޸���
	// �޸��� Ű�� ���� Ű�� ������ ������
	if (DLLInput::InputKey(CL::KeyMap::SPRINT) && DLLInput::InputKey(CL::KeyMap::FORWARD))
	{
		s_bSprint = true;
		s_bCrouch = false;	// �޸� ���� ������ ���ְ� �����

		// �̵��ӵ� ����
		if (m_pMyObject->GetComponent<PhysicsActor>())
		{
			m_MoveSpeed = m_SprintSpeed;
		}

		else
		{
			m_MoveSpeed = m_SprintSpeed; // ������ ���Ͱ� ���� Ʈ������ �̵��� �� �⺻ �޸���ӵ� ����
		}
	}

	// �޸��� ��ư�̳� ���� Ű�� ����
	else if (DLLInput::InputKeyUp(CL::KeyMap::SPRINT) || DLLInput::InputKeyUp(CL::KeyMap::FORWARD))
	{
		// �޸��� ���� ����
		s_bSprint = false;
	}

	/// ���� 
	if (CanAim() == true
		&& DLLInput::InputKey(CL::KeyMap::AIM) == true)
	{
		// ���� ���� ����
		s_bAim = true;

		// �޸��� �׻� off
		s_bSprint = false;

		//m_Animator->SetOverrideAnimLayer("");

		// �̵� ����, �ִϸ��̼� ����
		if (m_PoseMode == PoseMode::Stand)
		{
			m_MoveSpeed = m_StandSpeed;

			// Ȱ��ȭ �ϰ� ���� ���̾� �̸��� ������ �Լ� ȣ��
			m_Animator->SetOverrideAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE_AIM);
			//m_Animator->SetNoneAnimLayer("");

			if (s_H == 0.0f)
			{
				// Idle�̳� �յڷ� ������ �� ��¦ ���������� ȸ��
				m_PlayerMesh->m_Transform->SetRotationFromVec({ 0.0f, 240.0f, 0.0f });
			}

			else
			{
				// �¿�� ������ ���� �⺻ ���� �״��
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

	/// ������
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

	/// ��� ��ü (����)
	if (CanSwap() == true)
	{
		/// ��ü�� ���� Ű�� ������ ���
		// ����(1��) ��ü �õ� 
		if (DLLInput::InputKeyDown(static_cast<int>(CL::KeyMap::SLOT_1)))
		{
			// ���� ������� �������� Ȯ��
			uint _currentSlotNum = m_Inventory->m_EquiptedSlotIndex;
			if (_currentSlotNum != static_cast<uint>(Inventory::Slot::eLongGun))
			{
				// ��� ��ü ���� ���·� ����
				s_bIsSwaping = true;
				m_SwapCoolTimer = m_SwapSpeed;

				// ��ü�� ����� ���� ��ȣ ����
				s_NextSlotNum = static_cast<uint>(Inventory::Slot::eLongGun);

				// �ִϸ����� ���� ����
				s_bIsSwaping = true;

				// �ִϸ��̼� ����
				m_Animator->SetOverrideAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE_SWAP_WEAPON);

				CA_TRACE("[PlayerController] ���ü ����");
			}

			else
			{
				CA_TRACE("[PlayerController] ���ü ���� - > ���� ����� �������̴�..");
			}
		}

		// ����(2��) ��ü �õ� 
		if (DLLInput::InputKeyDown(static_cast<int>(CL::KeyMap::SLOT_2)))
		{
			// ���� ������� �������� Ȯ��
			uint _currentSlotNum = m_Inventory->m_EquiptedSlotIndex;
			if (_currentSlotNum != static_cast<uint>(Inventory::Slot::eHandGun))
			{
				// ��� ��ü ���� ���·� ����
				s_bIsSwaping = true;
				m_SwapCoolTimer = m_SwapSpeed;

				// ��ü�� ����� ���� ��ȣ ����
				s_NextSlotNum = static_cast<uint>(Inventory::Slot::eHandGun);

				// �ִϸ��̼� ����
				m_Animator->SetOverrideAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE_SWAP_WEAPON);

				CA_TRACE("[PlayerController] ���ü ����");
			}

			else
			{
				CA_TRACE("[PlayerController] ���ü ���� - > ���� ����� �������̴�..");
			}
		}

		// �ö�ũ(3��) ��ü �õ� 
		if (DLLInput::InputKeyDown(static_cast<int>(CL::KeyMap::SLOT_3)))
		{
			// ���� ������� �������� Ȯ��
			uint _currentSlotNum = m_Inventory->m_EquiptedSlotIndex;
			if (_currentSlotNum != static_cast<uint>(Inventory::Slot::eFlask))
			{
				// ��� ��ü ���� ���·� ����
				s_bIsSwaping = true;
				m_SwapCoolTimer = m_SwapSpeed;

				// ��ü�� ����� ���� ��ȣ ����
				s_NextSlotNum = static_cast<uint>(Inventory::Slot::eFlask);

				// �ִϸ��̼� ����
				m_Animator->SetOverrideAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE_SWAP_WEAPON);

				CA_TRACE("[PlayerController] ���ü ����");
			}

			else
			{
				CA_TRACE("[PlayerController] ���ü ���� - > ���� ����� �������̴�..");
			}
		}

		// ȭ�� �ö�ũ(4��) ��ü �õ� 
		if (DLLInput::InputKeyDown(static_cast<int>(CL::KeyMap::SLOT_4)))
		{
			// ���� ������� �������� Ȯ��
			uint _currentSlotNum = m_Inventory->m_EquiptedSlotIndex;
			if (_currentSlotNum != static_cast<uint>(Inventory::Slot::eFireFlask))
			{
				// ��� ��ü ���� ���·� ����
				s_bIsSwaping = true;
				m_SwapCoolTimer = m_SwapSpeed;

				// ��ü�� ����� ���� ��ȣ ����
				s_NextSlotNum = static_cast<uint>(Inventory::Slot::eFireFlask);

				// �ִϸ��̼� ����
				m_Animator->SetOverrideAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE_SWAP_WEAPON);

				CA_TRACE("[PlayerController] ���ü ����");
			}

			else
			{
				CA_TRACE("[PlayerController] ���ü ���� - > ���� ����� �������̴�..");
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
	// PhysicsActor ������Ʈ�� ������ ������ �����δ�.
	if (GetMyObject()->GetComponent<PhysicsActor>())
	{
		MoveByPhysX();
	}

	// PhysicsActor �� ���� ���¸� �⺻ Ʈ���������� �̵��Ѵ�.
	else
	{
		MoveByTransform();
	}
}

/// <summary>
/// physX ������ �ƴ� Transform�� ������ �̵��ϴ� �Լ�
/// �밢������ �̵��� �� �̵��ӵ� ������ �ʿ��ϴ�.(���� ������)
/// </summary>
void PlayerController::MoveByTransform()
{
	/// <summary>
	///  �����¿� �̵�
	/// </summary>

	if (s_V > 0.0f)
	{
		// ���� �̵�
		m_Transform->SetRotationFromVec({ 0.0f, m_CameraParent->m_Transform->m_EulerAngles.y, 0.0f });
		m_Transform->MoveForwardOnXZ(m_MoveSpeed * CL::Input::s_DeltaTime);

	}

	if (s_V < 0.0f)
	{
		// �Ĺ� �̵�
		m_Transform->SetRotationFromVec({ 0.0f, m_CameraParent->m_Transform->m_EulerAngles.y, 0.0f });
		m_Transform->MoveForwardOnXZ(-m_MoveSpeed * CL::Input::s_DeltaTime);
	}

	if (s_H < 0.0f)
	{
		// �·� �̵�
		m_Transform->SetRotationFromVec({ 0.0f, m_CameraParent->m_Transform->m_EulerAngles.y, 0.0f });
		m_Transform->MoveSide(-m_MoveSpeed * CL::Input::s_DeltaTime);
	}

	if (s_H > 0.0f)
	{
		// ��� �̵�
		m_Transform->SetRotationFromVec({ 0.0f, m_CameraParent->m_Transform->m_EulerAngles.y, 0.0f });
		m_Transform->MoveSide(m_MoveSpeed * CL::Input::s_DeltaTime);
	}
}

/// <summary>
/// physX ������ �̿��ؼ� �����̴� �Լ�
/// Object�� PhysicsActor ������Ʈ�� ���� �� ���
/// </summary>
void PlayerController::MoveByPhysX()
{
	/// Ű�׸�ƽ ���� �Ǿ������� ���ӿ��� Ʈ���������� �̵��ϰ� ���������� Ʈ�������� ����ȭ
	if (GetMyObject()->GetComponent<PhysicsActor>()->IsKinematic())
	{
		MoveByTransform();
		DLLEngine::SetPhysicsTransform(m_Transform);
		//DLLEngine::SetTransform(GetMyObject(), *m_Transform);

		return;
	}

	/// Ű�׸�ƽ false �϶� �����̵�
	// ī�޶� ���� 
	//Vector3 _cameraForwardDir = m_CameraParent->m_Transform->m_RotationTM.Forward();
	Vector3 _cameraForwardDir = m_CameraParent->m_Transform->m_WorldTM.Forward();
	Vector3 _cameraRightDir = m_CameraParent->m_Transform->m_WorldTM.Right();

	// ����� �̵� ����
	Vector3 _moveDir = Vector3::Zero;

	/// <summary>
	///  �����¿� �̵�
	/// </summary>
	if (s_V > 0.0f)
	{
		// ���� �̵�
		_moveDir += { _cameraForwardDir.x, 0.0f, _cameraForwardDir.z};
	}

	if (s_V < 0.0f)
	{
		// �Ĺ� �̵�
		_moveDir -= { _cameraForwardDir.x, 0.0f, _cameraForwardDir.z};;
	}

	if (s_H < 0.0f)
	{
		// �·� �̵�
		_moveDir -= { _cameraRightDir.x, 0.0f, _cameraRightDir.z};
	}

	if (s_H > 0.0f)
	{
		// ��� �̵�
		_moveDir += { _cameraRightDir.x, 0.0f, _cameraRightDir.z};
	}

	// �̵����� ����ȭ
	_moveDir.Normalize();

	// �̵��� ���
	_moveDir *= m_MoveSpeed * CL::Input::s_DeltaTime;

	// �̵�
	DLLEngine::SetVelocity(m_MyPhysicsActor, _moveDir);

	//float _nowMoveScale = abs(_moveDir.x) + abs(_moveDir.y) + abs(_moveDir.z);
	//CA_TRACE("Move Scale = %f", _nowMoveScale);

	// Look ���� ��ȯ
	m_Transform->SetRotationFromVec({ 0.0f, m_CameraParent->m_Transform->m_EulerAngles.y, 0.0f });
}


PoseMode PlayerController::SetPoseMode(PoseMode pose)
{
	// PoseMode ����
	m_PoseMode = pose;

	switch (m_PoseMode)
	{
	case PoseMode::Stand:
	{
		m_MoveSpeed = m_StandSpeed;		// Stand �̵� �ӵ� 

		//CA_TRACE("[PlayerController] Now Mode : Idle / Now MoveSpeed = %f", m_MoveSpeed);
		break;
	}


	case PoseMode::Crouch:
	{
		m_MoveSpeed = m_CrouchSpeed;		// Crouch �̵� �ӵ�

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
	// ������ �� ���(����, ��ô)Ű ������ ��
	if (DLLInput::InputKeyDown(CL::KeyMap::ATTACK))
	{
		// ����
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
	// ���� ������� �������� Ȯ��
	uint _currentSlotNum = m_Inventory->m_EquiptedSlotIndex;
	if (_currentSlotNum != static_cast<uint>(Inventory::Slot::eLongGun))
	{
		// ��� ��ü ���� ���·� ����
		s_bIsSwaping = true;
		m_SwapCoolTimer = m_SwapSpeed;

		// ��ü�� ����� ���� ��ȣ ����
		s_NextSlotNum = static_cast<uint>(Inventory::Slot::eLongGun);
	}

	else
	{
		CA_TRACE("[PlayerController] ���ü ���� - > ���� ����� �������̴�..");
	}
	return true;
}

void PlayerController::SwapCoolDown()
{
	m_SwapCoolTimer -= CL::Input::s_DeltaTime;

	if (m_SwapCoolTimer <= 0.0f)
	{
		s_bIsSwaping = false;
		CA_TRACE("[PlayerController] ���ü Ÿ�̸� ��");

		// ���ⱳü �ִϸ��̼� ����
		m_Animator->SetOverrideAnimLayer("");

		// �κ��丮�� ��� ���� ��ü
		// ���߿� �ִϸ��̼� �̺�Ʈ���� �ִϸ��̼��� �߰��� ������� �� �����ϵ��� �ؾ���
		m_Inventory->ChangeWeapon(s_NextSlotNum);
	}
}

/// <summary>
/// ���� �� ������̰� źâ�� ��������� �����Ѵ�.
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
	s_bIsDie = false;	// �������� �ִ� ���� ����
	s_bIsDead = true;	// �׾ �����ִ»��� true
}

/// <summary>
/// �÷��̾� ��� �Ǵ�
/// </summary>
/// <returns></returns>
bool PlayerController::IsDead()
{
	// �������� �ְų�, �̹� �׾ �����ִ� ���
	if (s_bIsDie == true || s_bIsDead == true)
	{
		return true;
	}

	// Alive
	return false;
}

/// <summary>
/// ���� ���� �����Ѱ� �Ǵ�
/// </summary>
/// <returns>�Ǵ� ����</returns>
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
/// ���� ���� ������ �������� �Ǵ�
/// </summary>
/// <returns>���� ���� ����</returns>
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
	// ��ȯ�� ����
	TCHAR* wstr = StringHelper::StringToWchar(_hitObject);

	Text* text = m_pMyObject->GetComponent<Text>();
	text->PrintSpriteText((TCHAR*)L"%s", wstr);

	SAFE_DELETE(wstr);
	*/


	// �׽�Ʈ ���� ���� (����)
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

	//m_Animator = m_SaveData->m_Animator;	// ������Ʈ �ҷ����°� Start ���� �������ش�
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
