#include "pch.h"
#include "MathHelper.h"
#include "PlayerController.h"
#include "Partner_Move.h"
#include "Health.h"
#include "NavMeshAgent.h"
#include "Zombie_Runner_Move.h"

Zombie_Runner_Move::Zombie_Runner_Move()
	: Enemy_Move()
	, m_DetectionRange(0.0f)
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
	m_WaitTime = 15.0f;
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
	m_AttackRange = _myBoundingOFfset + _playerBoundingOffset + 0.6f;

	// ���ݷ�
	m_AttackPower = 2.0f;
}

void Zombie_Runner_Move::Update(float dTime)
{
	Enemy_Move::Update(dTime);

	// ����, ���� ������ ���
	UpdateViewSight();

	//CA_TRACE("now Target %d", static_cast<int>(m_Target.type));
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
		/// <summary>
		/// �̵��߿� �ٸ� ����� �ε����� ������ ��¦ �̵����� �ش�.
		/// </summary>
		/// <param name="collision"></param>
		FreezePositionAndRotation::Position _freezePos = collision.m_Actor->GetFreezePosition();
		if (_freezePos.x == true && _freezePos.z == true)
		{
			// �������� �̵��ϱ����� �缱 ���͸� �����.
			SimpleMath::Vector3 _dir = m_Transform->m_RotationTM.Right();
			_dir.Normalize();
			_dir *= CL::Input::s_DeltaTime;

			m_Transform->SetPosition(m_Transform->m_Position + _dir);
		}
	}

	//CA_TRACE("���񹫺� ������");
}

void Zombie_Runner_Move::OnCollisionExit(Collision collision)
{
	//CA_TRACE("���񹫺� exit");
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
	float angle = UtilityFunction::CalcAngleToTarget(*m_pMyObject, target);
	UtilityFunction::NormalizeAngle(angle);

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
		m_NavMeshAgent->SetDestinationObj(nullptr);
		m_Animator->SetNoneAnimLayer("Die");

#ifdef _DEBUG
		//CA_TRACE("[EnemyMove] IsDead() -> false");
#endif
		m_State = State::eDead;

		if (m_PartnerHealth->IsDead() == false
			&& m_PartnerMove->m_EnemyWhoIsAttackingMe == this)
		{
			ReleasePartner();
		}

		return true;
	}

#ifdef _DEBUG
	//CA_TRACE("[EnemyMove] %s is Dead", m_pMyObject->GetObjectName().c_str());
#endif
	return false;
}

bool Zombie_Runner_Move::IsAttackPartner()
{
	if (m_State & State::eAttackPartner)
	{

		return true;
	}

	return false;
}

bool Zombie_Runner_Move::IsAttackPlayer()
{
	if (m_State & State::eAttackPlayer)
	{
		return true;
	}

	return false;
}

bool Zombie_Runner_Move::MoveToReturnPoint()
{
	// ���� Ž���� �ؼ� �����Ǵ� Ÿ���� ������ 
	// ���� ����
	if (FindPartner() == true || FindPlayer() == true)
	{
		return false;
	}

	return Enemy_Move::MoveToReturnPoint();
}


bool Zombie_Runner_Move::AwakenSight()
{
	if (m_State & State::eAwakenSight)
	{
		m_Animator->SetNoneAnimLayer("AwakenSight");

		// ������ �ֵ���
		m_NavMeshAgent->SetDestinationObj(nullptr);

		m_PhysicsActor->SetFreezePosition(true, true, true);
		m_PhysicsActor->SetFreezeRotation(true, true, true);

		return true;
	}

	return false;
}

bool Zombie_Runner_Move::PostAwakenSight()
{
	if (m_State & State::eAwakenSight)
	{
		// Awake ����
		m_State &= ~Zombie_Runner_Move::State::eAwakenSight;

		// Hunt ���� ����
		m_State |= Zombie_Runner_Move::State::eHunt;
		m_Animator->SetNoneAnimLayer("Hunt");

		m_NavMeshAgent->SetDestinationObj(m_Target.object);

		m_PhysicsActor->SetFreezePosition(false, true, false);
		m_PhysicsActor->SetFreezeRotation(true, false, true);

		return true;
	}

	return false;
}

/// <summary>
/// Ÿ��(�÷��̾�) ���� �Լ�
/// </summary>
bool Zombie_Runner_Move::FindPlayer()
{
	// ���α׷� ���� ����
	if (m_Player == nullptr)
	{
		return false;
	}


	// �÷��̾� ��� ���¸� ����
	if (m_PlayerHealth->IsDead() == true)
	{
		m_State &= ~State::eAttackPlayer;
		m_Target.type = Target::Type::eNone;
		m_Target.object = nullptr;
		return false;
	}

	/// Ÿ�� ������ ����
	// �÷��̾� ������ ����
	const SimpleMath::Vector3& _playerPos = m_Player->m_Transform->m_Position;
	// ���� ������ ����
	//const SimpleMath::Vector3& _partnerPos = m_Partner

	// �÷��̾ Ž���Ÿ� �ȿ� ������
	if (UtilityFunction::IsTargetInDetectionRange(*m_pMyObject, *m_Player, m_DetectionRange))
	{
		/// Ÿ���� �þ߰�(����) �ȿ� �ִ��� �˻�
		// �þ߰� ���̰� ���� �Ǵ� ���(������ ���� ���̿� 0(360)�� �� �ɸ��� ���)
		if (IsTargetInViewSight(*m_Player) == true)
		{
			/// Ÿ���� ã��!
			// ó�� �߰����� �� 
			if (m_Target.object == nullptr)
			{
				// ��ȯ��ġ ����
				m_ReturnPosition = m_Transform->m_Position;
				// ��ȯ ���� �׻� ����
				m_State &= ~State::eReturn;

				// Ž�� ���� ����
				m_State &= ~State::eExplore;

				// eAwakenSight ���� ����
				m_State |= State::eAwakenSight;

				// �÷��̾� �ٶ󺸵��� ȸ��
				m_Transform->LookAtYaw(m_Player->m_Transform->m_Position);

				// ��Ʈ�� ���� ����
				m_State &= ~State::ePatrol;
			}

			// Ÿ�� ���� ����
			m_Target = { Target::Type::ePlayer, m_Player };

			//CA_TRACE("[EnemyMove] %s : Change State <eHunt>", m_pMyObject->GetObjectName().c_str());
			//m_NavMeshAgent->SetDestinationObj(m_Player);

			return true;
		}

		/// Ÿ���� �þ� �ۿ� ������
		// �÷��̾ ������ �������̾�����
		if (m_Target.type == Target::Type::ePlayer)
		{
			// ��� �þ߿��� ���� �÷��̾ �ѵ��� ����
			m_NavMeshAgent->SetDestinationObj(m_Player);
			return true;
		}

		// ���� �����ӿ� Ž���Ÿ� �ȿ� ���ͼ� �Ǵ��ϰ� �ִ� ��쿡��
		// �����ɷ� ó��
		else
		{
			m_Target = { Target::Type::eNone, nullptr };
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
		if (m_Target.type == Target::Type::ePlayer)
		{
			// ���� ���� ����
			m_State &= ~State::eHunt;
			m_Target = { Target::Type::eNone, nullptr };

			// ��ȯ ���·� ��ȯ
			m_State |= State::eReturn;

			m_Animator->SetNoneAnimLayer("Patrol");

			return false;
		}

		// ������ ��� Ž���Ÿ� ���̾�����
		else
		{
			m_Target = { Target::Type::eNone, nullptr };

			// ���� ���� ����
			m_State &= ~State::eHunt;

			return false;
		}
	}
}

/// <summary>
/// ���� ���� �Լ�
/// </summary>
/// <returns></returns>
bool Zombie_Runner_Move::FindPartner()
{
	// ���α׷� ���� ����
	if (m_Partner == nullptr)
	{
		return false;
	}

	// ���� ��� ���¸� ����
	if (m_PartnerHealth->IsDead() == true)
	{
		return false;
	}

	/// Ÿ�� ������ ����
	// ���� ������ ����
	const SimpleMath::Vector3& _partnerPos = m_Partner->m_Transform->m_Position;

	// ������ Ž���Ÿ� �ȿ� ������
	if (UtilityFunction::IsTargetInDetectionRange(*m_pMyObject, *m_Partner, m_DetectionRange))
	{
		/// Ÿ���� �þ߰�(����) �ȿ� �ִ��� �˻�
		// �þ߰� ���̰� ���� �Ǵ� ���(������ ���� ���̿� 0(360)�� �� �ɸ��� ���)
		if (IsTargetInViewSight(*m_Partner) == true)
		{
			/// Ÿ���� ã��!
			// ó�� �߰����� �� 
			if (m_Target.type == Target::Type::eNone)
			{
				// ��ȯ��ġ ����
				m_ReturnPosition = m_Transform->m_Position;
				// ��ȯ �����׻� ����
				m_State &= ~State::eReturn;

				// Ž�� ���� ����
				m_State &= ~State::eExplore;

				// AwakenSight ���� ����
				m_State |= State::eAwakenSight;

				// ������ �ٶ󺸵��� ȸ��
				m_Transform->LookAtYaw(m_Partner->m_Transform->m_Position);

				// ������ �ְ� ������ ���� ����
				m_State &= ~State::ePatrol;
			}

			// ���� ���·� ����
			m_Target = { Target::Type::ePartner, m_Partner };

			m_State |= State::eHunt;

			//CA_TRACE("[EnemyMove] %s : Change State <eHunt>", m_pMyObject->GetObjectName().c_str());
			//m_NavMeshAgent->SetDestinationObj(m_Partner);

			return true;
		}

		/// Ÿ���� �þ� �ۿ� ������
		// ������ ������ �������̾�����
		if (m_Target.type == Target::Type::ePartner)
		{
			// ��� �þ߿��� ���� �÷��̾ �ѵ��� ����
			m_NavMeshAgent->SetDestinationObj(m_Partner);
			return true;
		}

		// ���� �����ӿ� Ž���Ÿ� �ȿ� ���ͼ� �Ǵ��ϰ� �ִ� ��쿡��
		// �����ɷ� ó��
		else
		{
			m_Target = { m_Target.type, m_Target.object };

			if (m_Target.type == Target::Type::eNone)
			{
				// ���� ���� ����
				m_State &= ~State::eHunt;
			}

			//CA_TRACE("�Ÿ��� �Ǵµ� �þ߰� �ۿ� ����!");

			return false;
		}
	}

	// ������ Ž���Ÿ��� �����
	else
	{
		// ���� ������ �������̾�����
		if (m_Target.type == Target::Type::ePartner)
		{
			// ���� ���� ����
			m_State &= ~State::eHunt;
			m_Target = { m_Target.type, m_Target.object };

			// ��ȯ ���·� ��ȯ
			m_State |= State::eReturn;

			m_Animator->SetNoneAnimLayer("Patrol");

			return false;
		}

		else
		{
			m_Target = { m_Target.type, m_Target.object };

			// ������ �������� �ʾƵ� ������ �÷��̾ �߰����̾��� �� ������
			// type�� none�� ���� �߰ݻ��¸� �����Ѵ�.
			if (m_Target.type == Target::Type::eNone)
			{
				// ���� ���� ����
				m_State &= ~State::eHunt;
			}

			return false;
		}
	}
}

/// <summary>
/// �÷��̾ ���� �̵�
/// </summary>
/// <returns></returns>
bool Zombie_Runner_Move::MoveToTarget()
{
	if (m_State & State::eHunt)
	{
		// Ÿ���� ������� ���� �����ϰ� ���ϻ��·� ����
		if (m_Target.type == Target::Type::ePartner == true)
		{
			if (m_PartnerHealth->IsDead() == true)
			{
				m_State &= ~State::eHunt;
				m_State |= State::eReturn;

				return false;
			}
		}

		else if (m_Target.type == Target::Type::ePlayer == true)
		{
			if (m_PlayerHealth->IsDead() == true)
			{
				m_State &= ~State::eHunt;
				m_State |= State::eReturn;

				return false;
			}
		}

		m_NavMeshAgent->SetDestinationObj(m_Target.object);

		/// ���� ���� �ȿ� ���Գ� ���� �˻�
		// �Ÿ� ���
		float _distance = Vector3::Distance(m_Transform->m_Position, m_Target.object->m_Transform->m_Position);
		if (_distance < m_AttackRange
			&& IsTargetInViewSight(*(m_Target.object)))
		{
			m_NavMeshAgent->SetDestinationObj(nullptr);

			// ��۹�� ���°� ����
			m_PhysicsActor->SetFreezeRotation(true, true, true);
			// �и� ����
			m_PhysicsActor->SetFreezePosition(true, true, true);

			// Ÿ�� Ÿ�Կ� ���� ���� ���� On
			switch (m_Target.type)
			{
				case Target::Type::ePartner:
				{
					m_State |= State::eAttackPartner;
					m_State &= ~State::eAttackPlayer;
					break;
				}

				case Target::Type::ePlayer:
				{
					m_State |= State::eAttackPlayer;
					m_State &= ~State::eAttackPartner;
					break;
				}

			}

			return true;
		}

		// �߰� �ִϸ��̼� ����
		m_Animator->SetNoneAnimLayer("Hunt");

		m_PhysicsActor->SetFreezePosition(false, true, false);

		return true;
	}

	return false;
}

/// <summary>
/// �÷��̾� ����
/// �ִϸ��̼� ���
/// </summary>
/// <returns></returns>
bool Zombie_Runner_Move::AttackToPlayer()
{
	m_Animator->SetNoneAnimLayer("AttackPlayer");

	m_PhysicsActor->SetFreezePosition(true, true, true);
	m_PhysicsActor->SetFreezeRotation(true, true, true);

	return true;
}

/// <summary>
/// ���� ����
/// �ִϸ��̼� ��� �� ���� �߰� 
/// </summary>
/// <returns></returns>
bool Zombie_Runner_Move::AttackToPartner()
{
	if (m_State & State::eAttackPartner)
	{
		float _damage = CL::Input::s_DeltaTime;

		// �̹� ������ �����ϰ� �ִ� �ٸ� ���� �ִٸ�
		if (m_PartnerMove->m_EnemyWhoIsAttackingMe != nullptr
			&& m_PartnerMove->m_EnemyWhoIsAttackingMe != this)
		{
			// ���ݷ� ����(����Ű�� ����)
			_damage = 10000.0f;
		}

		// ������ �ִϸ��̼� ���
		m_Transform->LookAtYaw(m_PartnerMove->m_Transform->m_Position);
		m_Animator->SetNoneAnimLayer("AttackPartner");

		// ����� ����
		m_PartnerHealth->Damage(
			_damage,
			m_PartnerHealth->m_Transform->m_Position,
			SimpleMath::Vector3::Zero,
			0.0f,
			m_pMyObject
		);

		// ���� Ż����� ����
		m_PartnerMove->m_State &= ~Partner_Move::State::eEscape;

		// ���� �ǰ� �� ���� �߰�
		m_PartnerMove->m_State |= Partner_Move::State::eHit;
		m_PartnerMove->m_bHitBegin = true;

		// ���� �������̴� ���� ������ this ���� ������ �����ϴ� ����� ����
		if (m_PartnerMove->m_EnemyWhoIsAttackingMe == nullptr)
		{
			m_PartnerMove->m_EnemyWhoIsAttackingMe = this;
		}

		// ������ ü���� �� �������� �׾��ٸ�
		if (m_PartnerHealth->IsDead() == true)
		{
			// ������ ���� ���ݻ��� ������ �ִϸ��̼��� ������ ������ �̺�Ʈ��~
			// 
			// �ִϸ��̼� ���� ����
			m_bIsAttackPartner_Middle = false;
			m_bIsAttackPartner_End = true;

			// ������ ���� ����
			m_PhysicsActor->SetFreezePosition(false, true, false);
		}

		return true;
	}

	return false;
}

/// <summary>
/// ���� �� ó��
/// -> ������� �����Ѱ�?
/// -> ���ݻ��¸� ���� �ؾ� �ϴ°�?
/// </summary>
void Zombie_Runner_Move::PostAttackPlayer()
{
	/// <summary>
	/// ������ Ÿ���� ���ݹ��� �ȿ� �ִ��� �˻�
	/// </summary>

	float distance = SimpleMath::Vector3::Distance(m_Transform->m_Position, m_PlayerHealth->m_Transform->m_Position);
	if (m_PlayerHealth->IsDead() == false
		&& distance < m_AttackRange
		&& IsTargetInViewSight(*(m_Player)) == true)
	{
		// �����
		//CA_TRACE("[Zombie_Runner_Move] �ٽ� ����!");

		m_Transform->LookAtYaw(m_Player->m_Transform->m_Position);

		m_PhysicsActor->SetFreezePosition(false, true, false);
		m_PhysicsActor->SetFreezeRotation(true, false, true);

		return;
	}

	// ���ݻ��� ����
	m_State &= ~State::eAttackPlayer;

	m_PhysicsActor->SetFreezeRotation(true, false, true);
	m_PhysicsActor->SetFreezePosition(false, true, false);

	if (m_PlayerHealth->IsDead() == true)
	{
		m_State &= ~State::eHunt;
		m_State |= State::eReturn;
	}
}

/// <summary>
/// �Ǿϻ� �� ó��
/// </summary>
void Zombie_Runner_Move::PostAssassinated()
{
	// �Ǿϻ� ���� ����
	m_State &= ~State::eAssassinated;

	// ������ ����
	m_Health->Damage(
		100.0f,		// �ϻ쵥����(�׳� ���������ϰ� ū�� �־ �ѹ��� �װԲ�)
		SimpleMath::Vector3::Zero,
		m_Transform->m_Position - m_Player->m_Transform->m_Position,
		0.0f,
		m_pMyObject
	);
}

void Zombie_Runner_Move::FinshDie()
{
	m_bIsDie = false;
	m_bIsDead = true;

	//m_Animator->SetNoneAnimLayer("Dead");
}

bool Zombie_Runner_Move::Explore()
{
	if (m_State & State::eExplore)
	{
		// Ž�� �߿��� �� ������ ��� �Ѵ�.
		if (FindPartner() == true || FindPlayer() == true)
		{
			return false;
		}
	}

	return Enemy_Move::Explore();
}




void Zombie_Runner_Move::SaveData()
{
	m_SaveData->m_bEnable = GetIsEnabled();
	m_SaveData->m_ComponentId = m_ComponentId;

	m_SaveData->m_State = m_State;

	m_SaveData->m_ViewSight = &m_ViewSight;

	m_SaveData->m_MoveSpeed = m_MoveSpeed;

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

/// <summary>
/// A.I Agent�� ��(yaw)�� �þ߰�(angle)�� ��������
/// ����, ���� ������ �ش��ϴ� ���� ����Ѵ�
/// </summary>
void Zombie_Runner_Move::UpdateViewSight()
{
	// ����, ���� ���� ���� ���
	m_ViewSight.leftSight = m_Transform->m_EulerAngles.y + (m_ViewSight.angle / 2.0f);
	m_ViewSight.rightSight = m_Transform->m_EulerAngles.y - (m_ViewSight.angle / 2.0f);

	// 0 ~ 360 ������ ����ȭ
	UtilityFunction::NormalizeAngle(m_ViewSight.leftSight);
	UtilityFunction::NormalizeAngle(m_ViewSight.rightSight);
}

/// <summary>
/// �׾��� �� ������ �������̾����� ������ �ع��Ѵ�.
/// </summary>
void Zombie_Runner_Move::ReleasePartner() const
{
	// ���� ������ �������̾�����
	if (m_PartnerMove->m_EnemyWhoIsAttackingMe == this)
	{
		// ������ �ع��Ѵ�.
		m_PartnerMove->Escape();
	}
}

