#include "pch.h"
#include "MathHelper.h"
#include "PlayerController.h"
#include "Partner_Move.h"
#include "Health.h"
#include "NavMeshAgent.h"
#include "Enemy_Move.h"

Enemy_Move::Enemy_Move()
	: ComponentBase(ComponentType::GameLogic)
	, m_Health(nullptr)
	, m_PhysicsActor(nullptr)
	, m_NavMeshAgent(nullptr)
	, m_Animator(nullptr)

	, m_State(0)

	, m_AttackRange(1.0f)
	, m_AttackPower(1.0f)

	, m_MoveSpeed(1.0f)

	, m_Target()
	, m_Player(nullptr)
	, m_PlayerHealth(nullptr)
	, m_Partner(nullptr)
	, m_PartnerMove(nullptr)
	, m_PartnerHealth(nullptr)

	, m_WayPoints_V()
	, m_CurrentWayPointIndex(0)
	, m_Dir(Vector3::Zero)
	, m_Timer(0.0f)
	, m_WaitTime(2.0f)

	, m_ReturnPosition(SimpleMath::Vector3::Zero)

	, m_bIsDie(false)
	, m_bIsDead(false)
	, m_bIsPatrol(false)
	, m_bIsWait(false)
	, m_bIsHunt(false)
	, m_bIsReturn(false)
	, m_bCanBeAssassinated(false)
	, m_bIsAssassinated(false)
	, m_bIsAttackPartner_Middle(false)
	, m_bIsAttackPartner_End(false)
{
	m_WayPoints_V.reserve(4);
}

Enemy_Move::~Enemy_Move()
{
}

void Enemy_Move::Start()
{
	/// myObject�� ������Ʈ�� ������
	{
		// Health ������Ʈ �Ҵ�
		m_Health = m_pMyObject->GetComponent<Health>();
		assert(m_Health != nullptr);

		// PhysicsActor ������Ʈ �Ҵ�
		m_PhysicsActor = m_pMyObject->GetComponent<PhysicsActor>();
		assert(m_PhysicsActor != nullptr);

		m_NavMeshAgent = m_pMyObject->GetComponent<NavMeshAgent>();
		assert(m_NavMeshAgent != nullptr);

		/// ��������Ʈ�� �ε��� ����
		m_CurrentWayPointIndex = 0;
	}

	// ���� Ÿ�� �ʱ�ȭ
	m_Target = { Target::Type::eNone, nullptr };

	m_Animator = m_Transform->GetChildV().at(0)->GetComponent<Animator>();

	// �÷��̾� ����
	m_Player = DLLEngine::FindGameObjectByName("Player");
	m_PlayerHealth = m_Player->GetComponent<Health>();
	assert(m_PlayerHealth != nullptr);

	// ���� ����
	m_Partner = DLLEngine::FindGameObjectByName("Partner");
	m_PartnerMove = m_Partner->GetComponent<Partner_Move>();
	m_PartnerHealth = m_Partner->GetComponent<Health>();
	assert(m_PartnerMove != nullptr);
	assert(m_PartnerHealth != nullptr);
}

void Enemy_Move::Update(float dTime)
{

}

void Enemy_Move::OnRender()
{
}

bool Enemy_Move::IsWait()
{
	if (m_State & State::eWait)
	{
		return true;
	}

	return false;
}

bool Enemy_Move::IsPatrol()
{
	if (m_State & State::ePatrol)
	{
		return true;
	}

	return false;
}

bool Enemy_Move::IsReturn()
{	
	if (m_State & State::eReturn)
	{
		return true;
	}

	return false;
}

bool Enemy_Move::IsAssassinated()
{
	if (m_State & State::eAssassinated)
	{
		m_Animator->SetNoneAnimLayer("Assassinated");

		// ��۹�� ���°� ����
		m_PhysicsActor->SetFreezeRotation(true, true, true);

		// �и� ����
		m_PhysicsActor->SetFreezePosition(true, true, true);

		return true;
	}

	return false;
}

bool Enemy_Move::MoveToReturnPoint()
{
	if (m_State & State::eReturn)
	{
		m_Animator->SetNoneAnimLayer("Patrol");
		m_NavMeshAgent->SetDestinationPos(m_ReturnPosition);

		return true;
	}

	return false;
}

bool Enemy_Move::FinishReturn()
{
	if (m_State & State::eReturn)
	{
		float _dist = SimpleMath::Vector3::Distance(m_Transform->m_Position, m_ReturnPosition);
		if (_dist < 0.1f)
		{
			// ��ȯ���� ����
			m_State &= ~State::eReturn;

			// ��� ����
			SetWait();
		}

		return true;
	}

	return false;
}

/// <summary>
/// ���� �̺�Ʈ�� �̺�Ʈ �߻�(���) ���������� 
/// ���� ���带 ��´�.
/// ���� ����(EventDistance)�ȿ� �־�� ���� �� �ִ�.
/// </summary>
/// /// <param name="obj">�Ҹ��� �߻���Ų ������Ʈ</param>
/// <param name="playPosition">��� ��ġ</param>
/// <param name="sound">����</param>
/// <returns>��� ���� ���� </returns>
bool Enemy_Move::HearSound(GameObject* obj, SoundEvent* sound)
{
	// �׾� �ִ� ��쿡�� ���� �ʴ´�.
	if ((m_State & State::eDead) == true)
	{
		return false;
	}
	
	SimpleMath::Vector3 _playPosition = obj->m_Transform->m_Position;

	// ����� �ʱ�ȭ
	bool _ret = false;

	// ������ ����(�߰�, ����) ���³� �ƴϾ��� ���� ���� ���带 ��⸦ �õ��Ѵ�.
	if ((m_State & State::eAttackPartner) == false
		&& (m_State & State::eAttackPlayer) == false
		&& (m_State & State::eHunt) == false )
	{
		/// �鸮�°� �Ǻ�
		// ���� �Ÿ��ȿ� �ִ°�? 
		if (UtilityFunction::IsTargetInDetectionRange(m_Transform->m_Position, _playPosition, 18.0f))
		{
			float _dist = 19.0f;

			/// �߰��� ���� ���� �ִ°� üũ
			// ����ĳ��Ʈ�� �ڱ� �ڽ� ������Ʈ�� ��Ʈ ������Ʈ���� ���� ���� ���ؼ�
			// ���� �������� �����¿� 4������ �������� ���� �����ġ�� ���� ray�� ���
			std::vector <std::pair<SimpleMath::Vector3, SimpleMath::Vector3>> _OriAndDir;
			_OriAndDir.reserve(4);

			// forward / right Vector
			SimpleMath::Vector3 _forward = m_Transform->m_RotationTM.Forward();
			SimpleMath::Vector3 _right = m_Transform->m_RotationTM.Right();

			// ���� �������� üũ�� ��ġ ���
			SimpleMath::Vector3 _oriNorth = m_Transform->m_Position  + _forward * (m_PhysicsActor->GetBoundingOffset().Forward + 0.1f);
			SimpleMath::Vector3 _dirNorth = _playPosition - _oriNorth;
			_dirNorth.Normalize();
			_OriAndDir.emplace_back(_oriNorth, _dirNorth );

			// �Ĺ� �������� üũ�� ��ġ ���
			SimpleMath::Vector3 _oriSouth = m_Transform->m_Position - _forward * (m_PhysicsActor->GetBoundingOffset().Forward + 0.1f);
			SimpleMath::Vector3 _dirSouth = _playPosition - _oriSouth;
			_dirSouth.Normalize();
			_OriAndDir.emplace_back(_oriSouth, _dirSouth);

			// ���� �������� üũ�� ��ġ ���
			SimpleMath::Vector3 _oriWest = m_Transform->m_Position - _right * (m_PhysicsActor->GetBoundingOffset().Right + 0.1f);
			SimpleMath::Vector3 _dirWest = _playPosition - _oriWest;
			_dirWest.Normalize();
			_OriAndDir.emplace_back(_oriWest, _dirWest);

			// ���� �������� üũ�� ��ġ ���
			SimpleMath::Vector3 _oriEast = m_Transform->m_Position + _right * (m_PhysicsActor->GetBoundingOffset().Right + 0.1f);
			SimpleMath::Vector3 _dirEast = _playPosition - _oriEast;
			_dirEast.Normalize();
			_OriAndDir.emplace_back(_oriEast, _dirEast);

			
			// �����¿� 4 �������� ����ĳ��Ʈ üũ
			for (const auto& ray : _OriAndDir)
			{
				RaycastHit* _hit = DLLEngine::CheckRaycastHit(ray.first, ray.second, _dist);
				// �ƹ��� ������Ʈ�� üũ���� ���� ���
				if (_hit->actor == nullptr)
				{
					CA_TRACE("�浹ü ����", );
					_ret = true;

					break;
				}

				else
				{
					CA_TRACE("�浹ü = %s", _hit->actor->GetMyObject()->GetObjectName().c_str());
					//if (_hit->actor->GetMyObject()->GetTag() != "Wall")
					if (strcmp(_hit->actor->GetMyObject()->GetObjectName().c_str(), obj->GetObjectName().c_str()) == 0)
					{
						// ����� ���� �÷��� ��ġ ���̿� �ٸ� ������Ʈ�� ������ �����ɷ� ó��
						_ret = true;

						break;
					}
				}
			}
		}
	
		// ���带 ����ٸ�
		if (_ret == true)
		{
			// ���� ���� ��ġ ����
			m_HearSoundPosition = _playPosition;

			// ������ ���� �߰��̳� Ž�� ���°� �ƴϿ��� ���� ���� �߰߻��·� ���¸� �����ϰ�
			// ���� ��ġ�� ���� ��ġ�� �����Ѵ�.
			if ((m_State & State::eAwakenSound) == false
				&& (m_State & State::eExplore) == false)
			{
				// ���� �߰� ���� ����
				m_State |= State::eAwakenSound;

				// ���� �߰� �ִϸ��̼� ����
				m_Animator->SetNoneAnimLayer("AwakenSound");

				// ���� ��ġ ����
				m_ReturnPosition = m_Transform->m_Position;
			}
		}
	}

	return _ret;
}

bool Enemy_Move::AwakenSound()
{
	if (m_State & State::eAwakenSound)
	{
		m_Animator->SetNoneAnimLayer("AwakenSound");

		// ������ �ֵ���
		m_NavMeshAgent->SetDestinationObj(nullptr);

		// �������� �����ص� ������ ���ϴ� ���� �����־� �̲������� ������ �߻�
		// �̸� �ذ��ϱ� ���� physicsActor �� �ӵ��� ���� 0���� �������ش�.
		m_PhysicsActor->SetVelocity({ 0.0f, 0.0f, 0.0f });

		return true;
	}

	return false;
}

/// <summary>
/// ���� ���� �� ó��
/// </summary>
/// <returns></returns>
bool Enemy_Move::PostAwakenSound()
{
	if (m_State & State::eAwakenSound)
	{
		// AwakeSound ����
		m_State &= ~State::eAwakenSound;

		// Ž�� ���� ����
		m_State |= State::eExplore;
		m_Animator->SetNoneAnimLayer("Patrol");	// �ִϸ��̼��� ��Ʈ�Ѱ� ����

		m_NavMeshAgent->SetDestinationPos(m_HearSoundPosition);

		//m_PhysicsActor->SetFreezePosition(false, true, false);

		return true;
	}

	return false;
}

bool Enemy_Move::Explore()
{
	if (m_State & State::eExplore)
	{
		m_Animator->SetNoneAnimLayer("Patrol");
		m_NavMeshAgent->SetDestinationPos(m_HearSoundPosition);
		m_PhysicsActor->SetFreezePosition(false, true, false);

		return true;
	}

	return false;
}


/// <summary>
/// ��Ʈ�� ���·� �����ϴ� �Լ�
/// </summary>
void Enemy_Move::SetPatrol()
{
	m_bIsPatrol = true;
	m_State = State::ePatrol;

	m_bIsDie = false;
	m_bIsDead = false;
	m_bIsWait = false;
	m_bIsHunt = false;
	m_bIsReturn = false;

	m_Animator->SetNoneAnimLayer("Patrol");
}

/// <summary>
/// Wait ���·� �����ϴ� �Լ�
/// </summary>
void Enemy_Move::SetWait()
{
	m_bIsWait = true;
	m_State = State::eWait;

	m_bIsDie = false;
	m_bIsDead = false;
	m_bIsPatrol = false;
	m_bIsHunt = false;
	m_bIsReturn = false;

	m_Animator->SetNoneAnimLayer("Wait");
}

/// <summary>
/// ���� ������ ��������Ʈ�� ���� �̵��ϴ� �Լ�
/// </summary>
/// <returns></returns>
bool Enemy_Move::MoveForTargetWayPoint()
{
	if (m_State & State::ePatrol)
	{
		if (m_WayPoints_V.size() > 0)
		{
			/// Ÿ�� ��������Ʈ üũ(����)
			m_NavMeshAgent->SetDestinationObj(m_WayPoints_V.at(m_CurrentWayPointIndex));


			//CA_TRACE("patrol , %f", m_Dir.Length());

			m_Animator->SetNoneAnimLayer("Patrol");
		}

		return true;
	}

	return false;
}

/// <summary>
/// ��������Ʈ ����
/// Ÿ�� ��������Ʈ�� �����ϸ� ���� �ε����� Ÿ�� ��������Ʈ�� ���� �����Ѵ�
/// </summary>
/// <returns></returns>
bool Enemy_Move::UpdateTargetWayPoint()
{
	if (m_State & State::ePatrol)
	{
		// Ÿ�� ����Ʈ�� �����ϸ�
		Vector3 pos = { m_Transform->m_Position.x, m_Transform->m_Position.y - m_PhysicsActor->GetBoundingOffset().Bottom, m_Transform->m_Position.z };
		float distance = Vector3::Distance(pos, m_WayPoints_V.at(m_CurrentWayPointIndex)->m_Transform->m_Position);
		if (distance < 0.4f)	// ���� ������ Ÿ�ٰ��� �Ÿ��� �غ���..(���� ������ �浹üũ �Ұ���)
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

			//CA_TRACE("[EnemyMove] %s : Change State <ePatrol> -> <Wait>", m_pMyObject->GetObjectName().c_str());

			return true;
		}

		return false;
	}

	return false;
}

bool Enemy_Move::Wait()
{
	// Wait ���¶��
	if (m_State & State::eWait)
	{
		if (m_WayPoints_V.size() > 0)
		{
			// Ÿ�̸� ����
			m_Timer -= CL::Input::s_DeltaTime;
			m_NavMeshAgent->SetDestinationObj(nullptr);

			// �ȹ̲������� �ӵ� 0����
			m_PhysicsActor->SetVelocity({ 0.0f, 0.0f, 0.0f });

			// ��۹�� ���°� ����
			m_PhysicsActor->SetFreezeRotation(true, true, true);
			// �и� ����
			m_PhysicsActor->SetFreezePosition(true, true, true);

			// Ÿ�̸Ӱ� �� �Ǹ�
			if (m_Timer < 0.0f)
			{
				// Ÿ�̸� �缳��
				m_Timer = m_WaitTime;


				// ��� ���� ����
				m_State &= ~State::eWait;

				// ��Ʈ�� ���� �߰�
				m_State |= State::ePatrol;

				// �̵� �� ���ְ� ���� ����
				m_PhysicsActor->SetFreezeRotation(true, false, true);
				m_PhysicsActor->SetFreezePosition(false, true, false);

				m_NavMeshAgent->SetDestinationObj(m_WayPoints_V.at(m_CurrentWayPointIndex));
			}

#ifdef _DEBUG
			//CA_TRACE("[EnemyMove] %s : Change State <Wait> -> <ePatrol>", m_pMyObject->GetObjectName().c_str());
#endif
		}

		m_Animator->SetNoneAnimLayer("Wait");

		return true;
	}

	return false;
}

/// <summary>
/// ��������Ʈ ������Ʈ�� �Ķ���ͷ� �޾Ƽ�
/// �ش� ������Ʈ�� pos�� �ּҸ� ��������Ʈ���� ���Ϳ� �߰��Ѵ�.
/// (��������Ʈ ������Ʈ�� �̵��ص� �����ͷ� ����Ű�ϱ� ���� ���� ����ȭ �ȴ�.)
/// </summary>
/// <param name="wayPoint">��������Ʈ ������Ʈ</param>
void Enemy_Move::AddWayPoint(GameObject* wayPoint)
{
	m_WayPoints_V.emplace_back(wayPoint);
	//m_WayPointsComponentId_V.push_back(tf->GetComponetId());
}

/// <summary>
/// Enemy �� ����� ��(Idle)�� �ð� ����
/// </summary>
/// <param name="time"></param>
void Enemy_Move::SetWaitTime(const float time)
{
	m_WaitTime = time;
}
