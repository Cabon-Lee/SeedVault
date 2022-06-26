#include "pch.h"
#include "PlayerController.h"
#include "NavMeshAgent.h"
#include "Health.h"
#include "Enemy_Move.h"
#include "Partner_Move.h"

Partner_Move::Partner_Move()
	: ComponentBase(ComponentType::GameLogic)
	, m_Animator(nullptr)
	, m_PhysicsActor(nullptr)
	, m_NavMeshAgent(nullptr)
	, m_Health(nullptr)

	, m_PlayerController(nullptr)
	, m_PlayerPhysicsActor(nullptr)

	, m_EnemyWhoIsAttackingMe(nullptr)

	, m_State(0)

	, m_StandSpeed(0.0f)
	, m_CrouchSpeed(0.0f)
	, m_SprintSpeed(0.0f)
	, m_MoveSpeed(0.0f)

	, m_bWalk(false)
	, m_bSprint(false)
	, m_bCrouch(false)

	, m_bHitBegin(false)
	, m_bHitMiddle(false)
	, m_bHitEnd(false)

	, m_bDie(false)
	, m_bDead(false)

	, m_SprintDistance(0.0f)

{
	CA_TRACE("[Partner_Move] Create!");
}

Partner_Move::~Partner_Move()
{
	CA_TRACE("[Partner_Move] Delete!");
}

void Partner_Move::Start()
{
	CA_TRACE("[Partner_Move] Start()");

	// �÷��̾� ��Ʈ�ѷ� �޾ƿ�
	GameObject* _Player = DLLEngine::FindGameObjectByName("Player");
	if (_Player != nullptr)
	{
		m_PlayerController = _Player->GetComponent<PlayerController>();
		m_PlayerPhysicsActor = _Player->GetComponent<PhysicsActor>();
	}
	assert(m_PlayerController != nullptr);
	assert(m_PlayerPhysicsActor != nullptr);
	///////////////////////////////////////////////////////////////////


	m_Animator = m_Transform->GetChildV().at(0)->GetComponent<Animator>();
	assert(m_Animator != nullptr);

	m_PhysicsActor = m_pMyObject->GetComponent<PhysicsActor>();
	assert(m_PhysicsActor != nullptr);

	m_NavMeshAgent = m_pMyObject->GetComponent<NavMeshAgent>();
	assert(m_NavMeshAgent != nullptr);

	m_Health = m_pMyObject->GetComponent<Health>();
	assert(m_Health != nullptr);

	m_EnemyWhoIsAttackingMe = nullptr;

	/// �ʱ� ���� ����
	m_State = State::eFollow;	// �ȷο�

	/// Hp ����
	m_Health->SetMaxHp(10.0f);
	m_Health->SetHp(m_Health->GetMaxHp());

	/// �÷��̾�κ��� �ӵ��� �޾ƿ´�
	m_StandSpeed = m_PlayerController->GetStandSpeed();
	m_CrouchSpeed = m_PlayerController->GetCrouchSpeed();
	m_SprintSpeed = m_PlayerController->GetSprintSpeed();

	/// �޸��� ������ �÷��̾���� ������ �Ÿ�
	m_SprintDistance = 3.0f;

	m_NavMeshAgent->m_MoveSpeed = m_StandSpeed;
}

void Partner_Move::Update(float dTime)
{
	// �ִϸ��̼� Ʈ�����ǿ� ���� ����ȭ
	m_bCrouch = m_PlayerController->m_bCrouch;
}

void Partner_Move::OnRender()
{
}

bool Partner_Move::IsDead()
{
	if (m_State & State::eDead)
	{
		m_NavMeshAgent->SetDestinationObj(nullptr);


		m_State = State::eDead;

		return true;
	}


	return false;
}

bool Partner_Move::IsWait() const
{
	if (m_State & State::eWait)
	{
		return true;
	}

	return false;
}

bool Partner_Move::IsFollow() const
{
	if (m_State & State::eFollow)
	{
		return true;
	}

	return false;
}

bool Partner_Move::IsHit() const
{
	if (m_State & State::eHit)
	{
		return true;
	}

	return false;
}

bool Partner_Move::IsEscape() const
{
	if (m_State & State::eEscape)
	{
		return true;
	}

	return false;
}

bool Partner_Move::Hit()
{
	if (m_State & State::eHit)
	{
		if (m_EnemyWhoIsAttackingMe != nullptr)
		{
			// ���� �ٶ󺸵��� ȸ��
			m_Transform->LookAtYaw(m_EnemyWhoIsAttackingMe->m_Transform->m_Position);
		}

		// �������̰� ����
		m_PhysicsActor->SetFreezePosition(true, true, true);

		// Hp �ٵǼ� ������ ���ó��
		if (m_Health->IsDead() == true)
		{
			m_State = State::eDead;
			m_bDie = true;

			m_EnemyWhoIsAttackingMe = nullptr;
		}


		return true;
	}

	return false;
}

bool Partner_Move::Escape()
{
	// Ż�� ���� �߰�
	m_State |= State::eEscape;
	// �ǰ� ���� ����
	m_State &= ~State::eHit;
	m_EnemyWhoIsAttackingMe = nullptr;

	// �����ʿ��� Ǯ���ָ� �ִϸ��̼��� ����ȴ�.
	m_bHitMiddle = false;
	m_bHitEnd = true;

	//CA_TRACE("[�ع�]");

	// Hp �ʱ�ȭ
	m_Health->SetHp(m_Health->GetMaxHp());

	return true;
}

bool Partner_Move::Wait()
{
	// Wait ���¶��
	if (m_State & State::eWait)
	{

		return true;
	}

	return false;
}

bool Partner_Move::MoveToPlayer()
{
	// Follow ���¶��
	if (m_State & State::eFollow)
	{
		/// ��ǥ���� ���
		SimpleMath::Vector3 _PlayerPos = m_PlayerController->m_Transform->m_Position;
		SimpleMath::Vector3 _backDir = -m_PlayerController->m_Transform->m_RotationTM.Forward();
		_backDir.Normalize();
		_backDir *= 0.5f;

		SimpleMath::Vector3 _destPos = _PlayerPos + _backDir;

		/// �Ÿ� ���
		// �� ĳ����(����, �÷��̾�)�� �浹������ŭ ����߸���.
		float _playerForwardBoundingVolume = m_PlayerPhysicsActor->GetBoundingOffset().Forward;
		float _MyForwardBoundingVolume = m_PhysicsActor->GetBoundingOffset().Forward;
		float _Boundingoffset = _MyForwardBoundingVolume + _playerForwardBoundingVolume;

		//float _distance = Vector3::Distance(m_Transform->m_Position, m_PlayerController->m_Transform->m_Position);
		float _distance = Vector3::Distance(m_Transform->m_Position, _destPos);

		float _offset = 1.5f;
		if (m_PlayerController->m_bAim == true)
		{
			// �÷��̾ ���ؽÿ��� ������ ī�޶� ���� �Ȱ������� offset�� ���δ�.
			_offset = 0.0f;
		}

		// �ִϸ��̼� Ʈ�����ǿ� ���� ����
		m_bWalk = false;
		m_bSprint = false;

		if (_distance < _Boundingoffset + _offset)
		{
			m_NavMeshAgent->SetDestinationObj(nullptr);

			// ��۹�� ���°� ����
			m_PhysicsActor->SetFreezeRotation(true, true, true);

			return true;
		}

		// �׺���̼� ������ ����
		m_NavMeshAgent->SetDestinationPos(_destPos);

		// ������ ���� ����
		m_PhysicsActor->SetFreezeRotation(true, false, true);


		/// �ִϸ����� ����
		// �Ÿ��� �־����� �޸��� ���·� ����
		if (_distance > m_SprintDistance)
		{
			m_bWalk = false;
			m_bSprint = true;

			// �޸� ���� �����·� ����
			m_bCrouch = false;

			m_NavMeshAgent->m_MoveSpeed = m_SprintSpeed;
		}
		else
		{
			m_bWalk = true;
			m_bSprint = false;

			// ���� ���� �÷��̾��� �ڼ��� ���� ������ ���� ����

			// �ɱ� ����
			if (m_bCrouch == true)
			{
				m_NavMeshAgent->m_MoveSpeed = m_CrouchSpeed;
			}

			// �� ����
			else
			{
				m_NavMeshAgent->m_MoveSpeed = m_StandSpeed;
			}
		}

		//CA_TRACE("%f", m_NavMeshAgent->m_MoveSpeed);
		return true;
	}

	return false;
}
