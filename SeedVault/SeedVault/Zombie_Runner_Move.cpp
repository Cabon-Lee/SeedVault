#include "pch.h"
#include "MathHelper.h"
#include "PlayerController.h"
#include "Health.h"
#include "NavMeshAgent.h"
#include "Zombie_Runner_Move.h"

Zombie_Runner_Move::Zombie_Runner_Move()
	: Enemy_Move()
	, m_SaveData(new EnemyMove_Save())
{
	CA_TRACE("[Zombie_Runner_Move] Create!");
}

Zombie_Runner_Move::~Zombie_Runner_Move()
{
	CA_TRACE("[Zombie_Runner_Move] Delete!");
}

void Zombie_Runner_Move::Start()
{
	Enemy_Move::Start();

	CA_TRACE("[Zombie_Runner_Move] Start()");


	/// �ʱ� ���� ����
	SetWait();

	/// Health ����
	m_Health->SetHp(4.0f);

	/// �ӵ� ����
	m_WaitTime = 5.0f;
	m_Timer = m_WaitTime;

	m_NavMeshAgent->m_MoveSpeed = m_MoveSpeed;

	/// �þ� ����
	// �Ÿ� ����
	//m_DetectionRange = 6.0f;
	//
	//// ���� ����
	//m_ViewSight.angle = 140.0f;		// ��� Agent ���� ������ ����ȭ �ȴٸ�(���� ������ �ϰ������� �ٲ� �ʿ䰡 �ִٸ� static���� �ٲ���)

	// ����, ���� ������ ���
	UpdateViewSight();

	/// ���ݷ�, ���ݹ��� ����
	float _playerBoundingOffset = 0.0f;
	if (m_Player->GetComponent<PhysicsActor>() != nullptr)
	{
		_playerBoundingOffset = m_Player->GetComponent<PhysicsActor>()->GetBoundingOffset().Forward;
	}

	float _myBoundingOFfset = m_PhysicsActor->GetBoundingOffset().Forward;

	// ���ݹ���
	// �ٿ�� ���� + offset
	m_AttackRange = _myBoundingOFfset + _playerBoundingOffset + 0.5f;

	// ���ݷ�
	m_AttackPower = 2.0f;
}

void Zombie_Runner_Move::Update(float dTime)
{
	Enemy_Move::Update(dTime);
}

void Zombie_Runner_Move::OnRender()
{
}

void Zombie_Runner_Move::OnCollisionEnter(Collision collision)
{
	//CA_TRACE("���񹫺� ����");
}

void Zombie_Runner_Move::OnCollisionStay(Collision collision)
{
	if (collision.m_GameObject->GetTag() == "Zombie")
	{
	}

	//CA_TRACE("���񹫺� ������");
}

void Zombie_Runner_Move::OnCollisionExit(Collision collision)
{
	//CA_TRACE("���񹫺� exit");
}

/// <summary>
/// ��������Ʈ ������Ʈ�� �Ķ���ͷ� �޾Ƽ�
/// �ش� ������Ʈ�� pos�� �ּҸ� ��������Ʈ���� ���Ϳ� �߰��Ѵ�.
/// (��������Ʈ ������Ʈ�� �̵��ص� �����ͷ� ����Ű�ϱ� ���� ���� ����ȭ �ȴ�.)
/// </summary>
/// <param name="wayPoint">��������Ʈ ������Ʈ</param>
void Zombie_Runner_Move::AddWayPoint(GameObject* wayPoint)
{
	m_WayPoints_V.emplace_back(wayPoint);
	//m_WayPointsComponentId_V.push_back(tf->GetComponetId());
}

/// <summary>
/// Enemy �� ����� ��(Idle)�� �ð� ����
/// </summary>
/// <param name="time"></param>
void Zombie_Runner_Move::SetWaitTime(const float time)
{
	m_WaitTime = time;
}

/// <summary>
/// Ÿ���� Ž�� ���� �ȿ� �ִ��� �˻�
/// </summary>
/// <param name="target">Ž���� ������Ʈ</param>
/// <returns></returns>
bool Zombie_Runner_Move::IsTargetInDetectionRange(const GameObject& target)
{
	// �Ÿ��� ������
	float distance = Vector3::Distance(m_Transform->m_Position, target.m_Transform->m_Position);

	if (distance < m_DetectionRange)
	{
		return true;
	}

	else
	{
		return false;
	}
}

/// <summary>
/// �÷��̾ �þ߰� �ȿ� �ִ��� �˻�
/// </summary>
/// /// <param name="target">Ž���� ������Ʈ</param>
/// <returns></returns>
bool Zombie_Runner_Move::IsTargetInViewSight(const GameObject& target)
{
	// Ÿ�������� ������ ���Ѵ�
	float angle = CalcAngleToTarget(target);
	NormalizeAngle(angle);

	//CA_TRACE("angle = %f", angle);

	// �þ߰� ���̰� ���� �Ǵ� ���(������ ���� ���̿� 0(360)�� �� �ɸ��� ���)
	if (m_ViewSight.leftSight > m_ViewSight.rightSight)
	{
		if (angle <= m_ViewSight.leftSight &&
			angle >= m_ViewSight.rightSight)
		{
			return true;
		}

	}
	// �þ߰� ���̿� 0(360)���� ��� ������ �и��Ǵ� ���
	else
	{
		if ((0.0f <= angle) && (angle <= m_ViewSight.leftSight) ||		// ���� ���� üũ
			((m_ViewSight.rightSight <= angle) && (angle <= 360.0f)))	// ���� ���� üũ

		{
			return true;
		}
	}

	// �þ߰� �ۿ� �ִ�
	return false;
}

bool Zombie_Runner_Move::DamageToPlayer()
{
	float _distance = SimpleMath::Vector3::Distance(m_Transform->m_Position, m_Player->m_Transform->m_Position);

	if (_distance < m_AttackRange
		&& IsTargetInViewSight(*m_Player))
	{
		Health* targetHealth = m_Player->GetComponent<Health>();
		if (targetHealth != nullptr)
		{
			targetHealth->Damage(
				m_AttackPower,
				SimpleMath::Vector3::Zero,
				m_Transform->m_Position - m_Player->m_Transform->m_Position,
				0.0f,			// �˹��ų ��
				m_pMyObject
			);

			return true;
		}
	}

	return false;
}

bool Zombie_Runner_Move::IsDead()
{
	if (m_Health->IsDead())
	{
		m_NavMeshAgent->SetDestination(nullptr);
		m_Animator->SetNoneAnimLayer("Die");

#ifdef _DEBUG
		//CA_TRACE("[EnemyMove] IsDead() -> false");
#endif
		m_State = State::eDead;

		if (m_bIsDead == false)
		{
			m_bIsDie = true;
		}

		return true;
	}

#ifdef _DEBUG
	//CA_TRACE("[EnemyMove] %s is Dead", m_pMyObject->GetObjectName().c_str());
#endif
	return false;
}

bool Zombie_Runner_Move::IsWait()
{
	if (m_State & State::eWait)
	{
		return true;
	}

	return false;
}

bool Zombie_Runner_Move::IsPatrol()
{
	if (m_State & State::ePatrol)
	{
		return true;
	}

	return false;
}

bool Zombie_Runner_Move::IsAttack()
{
	if (m_State & State::eAttack)
	{
		//CA_TRACE("[Zombie_Runner_Move] Attack!");

		m_Animator->SetNoneAnimLayer("Attack");

		return true;
	}

	return false;
}

bool Zombie_Runner_Move::IsReturn()
{
	if (m_State & State::eReturn)
	{
		//CA_TRACE("[Zombie_Runner_Move] Return!");

		m_Animator->SetNoneAnimLayer("Patrol");

		return true;
	}

	return false;
}


/// <summary>
/// �÷��̾� ���� �Լ�
/// </summary>
bool Zombie_Runner_Move::FindPlayer()
{
	// ���α׷� ���� ����
	if (m_Player == nullptr)
	{
		return false;
	}

	// �÷��̾� ������ ����
	const SimpleMath::Vector3& _playerPos = m_Player->m_Transform->m_Position;


	// �÷��̾ Ž���Ÿ� �ȿ� ������
	if (IsTargetInDetectionRange(*m_Player))
	{
		/// Ÿ���� �þ߰�(����) �ȿ� �ִ��� �˻�
		// �þ߰� ���̰� ���� �Ǵ� ���(������ ���� ���̿� 0(360)�� �� �ɸ��� ���)
		if (IsTargetInViewSight(*m_Player) == true)
		{
			/// Ÿ���� ã��!
			// ���� ���·� ����
			m_bHasPlayer = true;

			m_State |= State::eHunt;

			//CA_TRACE("[EnemyMove] %s : Change State <eHunt>", m_pMyObject->GetObjectName().c_str());
			m_NavMeshAgent->SetDestination(m_Player);
			return true;

		}

		/// Ÿ���� �þ� �ۿ� ������
		// �÷��̾ ������ �������̾�����
		if (m_bHasPlayer == true)
		{
			// ��� �þ߿��� ���� �÷��̾ �ѵ��� ����
			m_NavMeshAgent->SetDestination(m_Player);
			return true;
		}

		// ���� �����ӿ� Ž���Ÿ� �ȿ� ���ͼ� �Ǵ��ϰ� �ִ� ��쿡��
		// �����ɷ� ó��
		else
		{
			m_bHasPlayer = false;
			// ���� ���� ����
			m_State &= ~State::eHunt;
			//CA_TRACE("�Ÿ��� �Ǵµ� �þ߰� �ۿ� ����!");

			return false;
		}
	}

	// �÷��̾ Ž���Ÿ��� �����
	else
	{
		// �÷��̾ ������ �������̾�����
		if (m_bHasPlayer == true)
		{
			// ��ȯ ���·� ��ȯ
			m_NavMeshAgent->SetDestination(m_Player);
			return true;
		}

		m_bHasPlayer = false;

		// ���� ���� ����
		m_State &= ~State::eHunt;

		return false;
	}
}

/// <summary>
/// �÷��̾ ���� �̵�
/// </summary>
/// <returns></returns>
bool Zombie_Runner_Move::MoveForPlayer()
{
	if (m_State & State::eHunt)
	{
		/// ���� ���� �ȿ� ���Գ� ���� �˻�
		// �Ÿ� ���
		float _distance = Vector3::Distance(m_Transform->m_Position, m_Player->m_Transform->m_Position);
		if (_distance < m_AttackRange
			&& IsTargetInViewSight(*m_Player))
		{
			m_NavMeshAgent->SetDestination(nullptr);

			// ��۹�� ���°� ����
			m_PhysicsActor->SetFreezeRotation(true, true, true);
			// �и� ����
			m_PhysicsActor->SetFreezePosition(true, true, true);

			// ���� ���� On
			m_State |= State::eAttack;

			return true;
		}

		// �߰� �ִϸ��̼� ����
		m_Animator->SetNoneAnimLayer("Hunt");

		return true;
	}

	return false;
}

bool Zombie_Runner_Move::AttackToPlayer()
{
	return true;
}

bool Zombie_Runner_Move::AttackToPartner()
{
	return false;
}

/// <summary>
/// ���� ������ ��������Ʈ�� ���� �̵��ϴ� �Լ�
/// </summary>
/// <returns></returns>
bool Zombie_Runner_Move::MoveForTargetWayPoint()
{
	if (m_State & State::ePatrol)
	{
		if (m_WayPoints_V.size() > 0)
		{
			/// Ÿ�� ��������Ʈ üũ(����)
			m_NavMeshAgent->SetDestination(m_WayPoints_V.at(m_CurrentWayPointIndex));


			//CA_TRACE("patrol , %f", m_Dir.Length());

			m_Animator->SetNoneAnimLayer("Patrol");
			return true;
		}

		return false;
	}

	return false;
}

/// <summary>
/// ��������Ʈ ����
/// Ÿ�� ��������Ʈ�� �����ϸ� ���� �ε����� Ÿ�� ��������Ʈ�� ���� �����Ѵ�
/// </summary>
/// <returns></returns>
bool Zombie_Runner_Move::UpdateTargetWayPoint()
{
	if (m_State & State::ePatrol)
	{
		// Ÿ�� ����Ʈ�� �����ϸ�
		Vector3 pos = { m_Transform->m_Position.x, m_Transform->m_Position.y - m_PhysicsActor->GetBoundingOffset().Bottom, m_Transform->m_Position.z };
		float distance = Vector3::Distance(pos, m_WayPoints_V.at(m_CurrentWayPointIndex)->m_Transform->m_Position);
		if (distance < 0.2f)	// ���� ������ Ÿ�ٰ��� �Ÿ��� �غ���..(���� ������ �浹üũ �Ұ���)
		{
			// ��������Ʈ �ε��� ����
			m_CurrentWayPointIndex++;

			// �ε����� ������ ��������Ʈ�� �Ѿ�� 
			if (m_CurrentWayPointIndex >= m_WayPoints_V.size())
			{
				// �ٽ� �� ó�� �ε����� ����
				m_CurrentWayPointIndex = 0;
			}

			// ��Ʈ�� ���� ����
			m_State &= ~State::ePatrol;

			// ��� ���� �߰�
			m_State |= State::eWait;

			CA_TRACE("[EnemyMove] %s : Change State <ePatrol> -> <Wait>", m_pMyObject->GetObjectName().c_str());

			return true;
		}

		return false;
	}

	return false;
}

bool Zombie_Runner_Move::Wait()
{
	// Wait ���¶��
	if (m_State & State::eWait)
	{
		if (m_WayPoints_V.size() > 0)
		{
			// Ÿ�̸� ����
			m_Timer -= CL::Input::s_DeltaTime;
			m_NavMeshAgent->SetDestination(nullptr);

			// �ȹ̲������� �ӵ� 0����
			m_PhysicsActor->SetVelocity({ 0.0f, 0.0f, 0.0f });

			// Ÿ�̸Ӱ� �� �Ǹ�
			if (m_Timer < 0.0f)
			{
				// Ÿ�̸� �缳��
				m_Timer = m_WaitTime;


				// ��� ���� ����
				m_State &= ~State::eWait;

				// ��Ʈ�� ���� �߰�
				m_State |= State::ePatrol;

				m_NavMeshAgent->SetDestination(m_WayPoints_V.at(m_CurrentWayPointIndex));
			}

#ifdef _DEBUG
			CA_TRACE("[EnemyMove] %s : Change State <Wait> -> <ePatrol>", m_pMyObject->GetObjectName().c_str());
#endif
		}

		m_Animator->SetNoneAnimLayer("Wait");

		return true;
	}

	return false;
}

/// <summary>
/// ���� �� ó��
/// -> ������� �����Ѱ�?
/// -> ���ݻ��¸� ���� �ؾ� �ϴ°�?
/// </summary>
void Zombie_Runner_Move::PostAttack()
{
	/// <summary>
	/// ������ �÷��̾ ���ݹ��� �ȿ� �ִ��� �˻�
	/// </summary>
	float distance = SimpleMath::Vector3::Distance(m_Transform->m_Position, m_Player->m_Transform->m_Position);
	if (distance < m_AttackRange
		&& IsTargetInViewSight(*m_Player) == true)
	{
		// �����
		//CA_TRACE("[Zombie_Runner_Move] �ٽ� ����!");

		m_Transform->LookAtYaw(m_Player->m_Transform->m_Position);
	}

	else
	{
		// ���ݻ��� ����
		m_State &= ~State::eAttack;

		m_PhysicsActor->SetFreezeRotation(true, false, true);
		m_PhysicsActor->SetFreezePosition(false, true, false);
	}
}

void Zombie_Runner_Move::FinshDie()
{
	m_bIsDie = false;
	m_bIsDead = true;

	//m_Animator->SetNoneAnimLayer("Dead");
}




void Zombie_Runner_Move::SaveData()
{
	m_SaveData->m_bEnable = GetIsEnabled();
	m_SaveData->m_ComponentId = m_ComponentId;

	m_SaveData->m_State = m_State;

	m_SaveData->m_ViewSight = &m_ViewSight;

	m_SaveData->m_MoveSpeed = m_MoveSpeed;

	m_SaveData->m_bHasPlayer = m_bHasPlayer;
	//m_SaveData->m_PlayerPosition = &m_PlayerPosition;

	m_SaveData->m_WayPointSize = m_WayPointsComponentId_V.size();
	save::WriteVectorValue(&(*m_Value)["m_WayPointsComponentId_V"], m_WayPointsComponentId_V);

	m_SaveData->m_CurrentWayPointIndex = m_CurrentWayPointIndex;

	m_SaveData->m_Dir = &m_Dir;

	m_SaveData->m_Timer = m_Timer;
	m_SaveData->m_WaitTime = m_WaitTime;

	save::WriteValue(m_Value, EraseClass(typeid(*this).name()), *m_SaveData);

}

void Zombie_Runner_Move::LoadData()
{
	//m_SaveData->m_PlayerPosition = &m_PlayerPosition;
	m_SaveData->m_ViewSight = &m_ViewSight;	// ��� �ϴ��� ��� ��
	m_SaveData->m_Dir = &m_Dir;

	save::ReadValue(m_Value, EraseClass(typeid(*this).name()), *m_SaveData);
	SetEnable(m_SaveData->m_bEnable);

	m_State = m_SaveData->m_State;

	m_MoveSpeed = m_SaveData->m_MoveSpeed;

	m_bHasPlayer = m_SaveData->m_bHasPlayer;

	save::ReadVectorValue(&(*m_Value)["m_WayPointsComponentId_V"], m_WayPointsComponentId_V);


	m_CurrentWayPointIndex = m_SaveData->m_CurrentWayPointIndex;

	m_Timer = m_SaveData->m_Timer;
	m_WaitTime = m_SaveData->m_WaitTime;


}

void Zombie_Runner_Move::LoadPtrData(std::map<unsigned int, GameObject*>* gameobject, std::map<unsigned int, ComponentBase*>* component)
{
	if (!m_WayPointsComponentId_V.empty())
	{
		for (int i = 0; i < m_WayPointsComponentId_V.size(); i++)
		{
			Transform* tf = dynamic_cast<Transform*>(component->at(m_WayPointsComponentId_V.at(i)));

			//m_WayPoints_V.push_back(&tf->m_Position);
		}
	}
}

