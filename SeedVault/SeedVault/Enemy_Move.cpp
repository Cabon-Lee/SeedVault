#include "pch.h"
#include "MathHelper.h"
#include "PlayerController.h"
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
	, m_ViewSight()

	, m_AttackRange(1.0f)
	, m_AttackPower(1.0f)

	, m_MoveSpeed(1.0f)

	, m_Player(nullptr)
	, m_WayPoints_V()
	, m_CurrentWayPointIndex(0)
	, m_DetectionRange(0.0f)
	, m_bHasPlayer(false)
	, m_Dir(Vector3::Zero)
	, m_Timer(0.0f)
	, m_WaitTime(2.0f)

	, m_bIsDie(false)
	, m_bIsDead(false)
	, m_bIsPatrol(false)
	, m_bIsWait(false)
	, m_bIsHunt(false)
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

		/// ��ǥ Ÿ�� ��ġ ����(���� ��������Ʈ ��������)
		//m_PlayerPosition = *m_WayPoints_V.at(m_CurrentWayPointIndex);
	}

	m_Animator = m_Transform->GetChildV().at(0)->GetComponent<Animator>();

	m_Player = DLLEngine::FindGameObjectByName("Player");
}

void Enemy_Move::Update(float dTime)
{
	// �ִϸ����Ϳ��� �ִϸ��̼� ������ �� �� ������ ���� ������Ʈ
	UpdateAnimationVar();

	// �þ� �� ����� Updqte���� �Ź� �غ���.
	UpdateViewSight();

}

void Enemy_Move::OnRender()
{
}

/// <summary>
/// A.I Agent�� ��(yaw)�� �þ߰�(angle)�� ��������
/// ����, ���� ������ �ش��ϴ� ���� ����Ѵ�
/// </summary>
void Enemy_Move::UpdateViewSight()
{
	// ����, ���� ���� ���� ���
	m_ViewSight.leftSight = m_Transform->m_EulerAngles.y + (m_ViewSight.angle / 2.0f);
	m_ViewSight.rightSight = m_Transform->m_EulerAngles.y - (m_ViewSight.angle / 2.0f);

	// 0 ~ 360 ������ ����ȭ
	NormalizeAngle(m_ViewSight.leftSight);
	NormalizeAngle(m_ViewSight.rightSight);
}

/// <summary>
/// ���� 0 ~ 360 �� ���̷� �����ϱ� ���� �Լ�
/// ���� ������ �ֱ������� ���߿� ����.
/// </summary>
/// <param name="angle">����ȭ�� ��</param>
void Enemy_Move::NormalizeAngle(float& angle)
{
	if (angle > 360)
	{
		angle -= 360;
	}
	else if (angle < 0)
	{
		angle += 360;
	}
}

/// <summary>
/// target ������Ʈ���� ������ ���ؼ� �����Ѵ�
/// </summary>
/// <param name="target">Ÿ�� ������Ʈ</param>
/// <returns>Ÿ������ ���ϴ� ���Ϸ���</returns>
float Enemy_Move::CalcAngleToTarget(const GameObject& target) const
{
	// Enemy�� Player ������ ������ ���Ѵ�.
	Vector3 myPos = m_Transform->m_Position;
	Vector3 targetPos = target.m_Transform->m_Position;

	float dx = targetPos.x - myPos.x;
	float dz = targetPos.z - myPos.z;

	float rad = std::atan2f(dx, dz);
	float angle = rad * (180.0f / 3.1415926535f);

	return angle;
}



void Enemy_Move::UpdateAnimationVar()
{
	// Die / Dead ������ �� ����, ���� �����ؾ� �Ѵ�. IsDead, FinishDIe ����
	//(m_State & State::eDead) ? m_bIsDead = true : m_bIsDead = false;

	(m_State & State::ePatrol) ? m_bIsPatrol = true : m_bIsPatrol = false;
	(m_State & State::eWait) ? m_bIsWait = true : m_bIsWait = false;
	(m_State & State::eHunt) ? m_bIsHunt = true : m_bIsHunt = false;
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

	m_Animator->SetNoneAnimLayer("Wait");
}

