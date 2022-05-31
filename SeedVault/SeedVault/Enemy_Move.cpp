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
	/// myObject의 컴포넌트들 가져옴
	{
		// Health 컴포넌트 할당
		m_Health = m_pMyObject->GetComponent<Health>();
		assert(m_Health != nullptr);

		// PhysicsActor 컴포넌트 할당
		m_PhysicsActor = m_pMyObject->GetComponent<PhysicsActor>();
		assert(m_PhysicsActor != nullptr);

		m_NavMeshAgent = m_pMyObject->GetComponent<NavMeshAgent>();
		assert(m_NavMeshAgent != nullptr);

		/// 웨이포인트를 인덱스 설정
		m_CurrentWayPointIndex = 0;

		/// 목표 타겟 위치 설정(현재 웨이포인트 지점으로)
		//m_PlayerPosition = *m_WayPoints_V.at(m_CurrentWayPointIndex);
	}

	m_Animator = m_Transform->GetChildV().at(0)->GetComponent<Animator>();

	m_Player = DLLEngine::FindGameObjectByName("Player");
}

void Enemy_Move::Update(float dTime)
{
	// 애니메이터에서 애니메이션 설정할 때 쓸 데이터 변수 업데이트
	UpdateAnimationVar();

	// 시야 각 계산은 Updqte에서 매번 해보자.
	UpdateViewSight();

}

void Enemy_Move::OnRender()
{
}

/// <summary>
/// A.I Agent의 각(yaw)와 시야각(angle)을 기준으로
/// 좌측, 우측 범위에 해당하는 각을 계산한다
/// </summary>
void Enemy_Move::UpdateViewSight()
{
	// 좌측, 우측 범위 각도 계산
	m_ViewSight.leftSight = m_Transform->m_EulerAngles.y + (m_ViewSight.angle / 2.0f);
	m_ViewSight.rightSight = m_Transform->m_EulerAngles.y - (m_ViewSight.angle / 2.0f);

	// 0 ~ 360 범위로 정규화
	NormalizeAngle(m_ViewSight.leftSight);
	NormalizeAngle(m_ViewSight.rightSight);
}

/// <summary>
/// 각을 0 ~ 360 도 사이로 제한하기 위한 함수
/// 아직 허점이 있긴하지만 나중에 개선.
/// </summary>
/// <param name="angle">정규화할 각</param>
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
/// target 오브젝트로의 각도를 구해서 리턴한다
/// </summary>
/// <param name="target">타게 오브젝트</param>
/// <returns>타겟으로 향하는 오일러각</returns>
float Enemy_Move::CalcAngleToTarget(const GameObject& target) const
{
	// Enemy와 Player 사이의 각도를 구한다.
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
	// Die / Dead 로직은 좀 복잡, 따로 관리해야 한다. IsDead, FinishDIe 에서
	//(m_State & State::eDead) ? m_bIsDead = true : m_bIsDead = false;

	(m_State & State::ePatrol) ? m_bIsPatrol = true : m_bIsPatrol = false;
	(m_State & State::eWait) ? m_bIsWait = true : m_bIsWait = false;
	(m_State & State::eHunt) ? m_bIsHunt = true : m_bIsHunt = false;
}

/// <summary>
/// 패트롤 상태로 세팅하는 함수
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
/// Wait 상태로 세팅하는 함수
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

