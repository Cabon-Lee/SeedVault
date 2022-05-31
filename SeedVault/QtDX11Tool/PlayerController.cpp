#include "pch.h"
#include "GameManager.h"
#include "Health.h"
#include "PlayerController.h"

// ���� �ʱ�ȭ
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

	// save/load �� ����
	, m_SaveData(new PlayerController_Save())

	// Debug�� 
	, m_LastHitTarget(nullptr)
{

}

PlayerController::~PlayerController()
{
	Remove();
}

void PlayerController::Start()
{
	// Ŀ�� ȭ�� �߾����� �̵�
	DLLWindow::MoveCursorToCenter();

	// Animator ����
	if (DLLEngine::FindGameObjectByName("Mesh") != nullptr)
	{
		m_Animator = DLLEngine::FindGameObjectByName("Mesh")->GetComponent<Animator>();
	}

	/// �̵� �ִϸ��̼� ���� ����
	m_PoseMode = PoseMode::Stand;
	m_MovingMode = MovingMode::Idle;

	m_MoveSpeed = m_StandSpeed;

	s_bCrouch = false;
	s_bAim = false;

	/// �������� �� ����
	s_bIsAttack = false;
	s_AttackSpeed = 0.5f;	// ���ݼӵ� �ӽ� ����
							// ���Ŀ� ������ ���⿡ ���� �����������.

	s_AttackRange = 100.0f;	// ���� ��Ÿ� ����

	// CameraParent ����
	GameObject* cameraParent = DLLEngine::FindGameObjectByName("CameraParent");
	assert(cameraParent != nullptr);

	m_CameraParent = cameraParent->m_Transform;

}

void PlayerController::Update(float dTime)
{
	// GameMager �� s_bTPSMode �� true �����Ѵ� 
	if (GameManager::IsTPSMode() == false)
	{
		return;
	}

	/// ���� Ű �Է� �ݿ�
	UpdateUserInput();

	///����
	{
		// ���� ���°� ���� �Ǹ� �����Ѵ�
		if (s_bIsAttack == true)
		{
			// ����
			Attack();

			// ��Ÿ�� ����
			s_AttackCollTimeTimer = s_AttackSpeed;
		}

		// ���� ��Ÿ�� ������(������)
		if (s_AttackCollTimeTimer >= 0.0f)
		{
			CoolDown();
		}
	}

	/// �̵�
	{
		// WASD �����¿� �̵�
		Move();


		// �׽�Ʈ�� ȸ�� ���� Ű
		if (DLLInput::InputKey(static_cast<int>(CL::KeyCode::KEY_P)))
		{
			ResetRotation();
		}
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

		m_MoveSpeed = m_SprintSpeed; // �̵��ӵ� ����
	}

	// �޸��� ��ư�̳� ���� Ű�� ����
	else if (DLLInput::InputKeyUp(CL::KeyMap::SPRINT) || DLLInput::InputKeyUp(CL::KeyMap::FORWARD))
	{
		// Stand ���·� ����
		s_bSprint = false;

		SetPoseMode(PoseMode::Stand);
	}

	/// ���� 
	if (DLLInput::InputKey(CL::KeyMap::AIM))	// ���� Ű ������ �ִ� ����
	{
		// ���� ���� ����
		s_bAim = true;

		// �޸��� �׻� off
		s_bSprint = false;

		// �������� ȸ��
		m_Transform->SetRotationFromVec({ 0.0f, m_CameraParent->m_Transform->m_EulerAngles.y, 0.0f });

		// �̵� ����, �ִϸ��̼� ����
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

		/// ���� �� ����Ű ������
		if (DLLInput::InputKeyDown(CL::KeyMap::ATTACK))
		{
			// ������ �����ϴٸ� ���� ���·� ����
			if (IsCanAttack())
			{
				s_bIsAttack = true;
			}
		}
	}

	/// ������
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

	/// ���� ��ü ����
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
	///  �����¿� �̵�
	/// </summary>

	if (s_V > 0.0f)
	{
		// ���� �̵�
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
		// �Ĺ� �̵�
		m_Transform->SetRotationFromVec({ 0.0f, m_CameraParent->m_Transform->m_EulerAngles.y, 0.0f });
		m_Transform->MoveForwardOnXZ(-m_MoveSpeed * DLLTime::DeltaTime());
	}

	if (s_H < 0.0f)
	{
		// �·� �̵�
		m_Transform->SetRotationFromVec({ 0.0f, m_CameraParent->m_Transform->m_EulerAngles.y, 0.0f });
		m_Transform->MoveSide(-m_MoveSpeed * DLLTime::DeltaTime());
	}

	if (s_H > 0.0f)
	{
		// ��� �̵�
		m_Transform->SetRotationFromVec({ 0.0f, m_CameraParent->m_Transform->m_EulerAngles.y, 0.0f });
		m_Transform->MoveSide(m_MoveSpeed * DLLTime::DeltaTime());
	}

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

		CA_TRACE("[PlayerController] Now Mode : Idle / Now MoveSpeed = %f", m_MoveSpeed);
		break;
	}


	case PoseMode::Crouch:
	{
		m_MoveSpeed = m_CrouchSpeed;		// Crouch �̵� �ӵ�

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
/// �÷��̾� ĳ���� ȸ���� �����ϱ� ���� �׽�Ʈ �Լ�
/// </summary>
void PlayerController::ResetRotation()
{
	this->m_Transform->SetRotationFromVec({ 0.0f, 0.0f });
}

/// <summary>
/// ��Ÿ���� üũ�ؼ� ���ݰ����Ѱ� �Ǵ�
/// </summary>
/// <returns></returns>
bool PlayerController::IsCanAttack() const
{
	// ��Ÿ���� �� ������ 
	if (s_AttackCollTimeTimer <= 0.0f)
	{
		// ���� ����
		return true;
	}

	// ���� �Ұ���
	return false;
}

/// <summary>
/// ���� ����� ����� �����Ѵ�.
/// �ѱ����� ���
/// ��ô����� ��ô�Ѵ�.
/// </summary>
void PlayerController::Attack()
{
	// �ӽ÷��� �ϴ� Ray�� ��ݵǴ���..
	CA_TRACE("[PlayerController] Attack!");

	/// Raycast Test
	// ���� ��ġ(ī�޶�) ã��
	GameObject* _mainCamera = DLLEngine::FindGameObjectByName("YH_MainCamera");
	assert(_mainCamera != nullptr);

	const Transform& _cameraTf = *_mainCamera->m_Transform;

	// ī�޶��� ��ġ���� ī�޶��� �������� ���̸� ����.
	Vector3 _attackPosition = _cameraTf.m_WorldPosition;
	Vector3 _attackDirection = _cameraTf.m_WorldTM.Forward();

	/// �浹 �ǻ�ü Ȯ��
	RaycastHit _hit;
	_hit = DLLEngine::CheckRaycastHit(_attackPosition, _attackDirection, s_AttackRange);

	// �浹ü�� ������
	if (_hit.actor != nullptr)
	{
		// ������ ������Ʈ ����(����׿�)
		m_LastHitTarget = _hit.actor->GetMyObject();

		Health* _health = _hit.actor->GetMyObject()->GetComponent<Health>();
		if (_health != nullptr)
		{
			_health->Damage(1.0f, _hit.position, _attackDirection, 1.0f, m_pMyObject, _hit.actor);
		}
	}

	// ���� �� ����� ���·� ����
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
	// ��ȯ�� ����
	TCHAR* wstr = StringHelper::StringToWchar(_hitObject);

	Text* text = m_pMyObject->GetComponent<Text>();
	text->PrintSpriteText((TCHAR*)L"%s", wstr);

	SAFE_DELETE(wstr);


	// �׽�Ʈ ���� ����
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
	if (m_SaveData->m_LastHitTarget != NULL)
	{
		m_LastHitTarget = dynamic_cast<GameObject*>(gameobject->at(m_SaveData->m_LastHitTarget));
	}
}
