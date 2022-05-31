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


	/// 초기 상태 지정
	SetWait();

	/// Health 세팅
	m_Health->SetHp(4.0f);

	/// 속도 설정
	m_WaitTime = 5.0f;
	m_Timer = m_WaitTime;

	m_NavMeshAgent->m_MoveSpeed = m_MoveSpeed;

	/// 시야 설정
	// 거리 세팅
	//m_DetectionRange = 6.0f;
	//
	//// 각도 세팅
	//m_ViewSight.angle = 140.0f;		// 모든 Agent 들의 각도가 동기화 된다면(같은 각도로 일괄적으로 바뀔 필요가 있다면 static으로 바꾸자)

	// 좌측, 우측 범위각 계산
	UpdateViewSight();

	/// 공격력, 공격범위 설정
	float _playerBoundingOffset = 0.0f;
	if (m_Player->GetComponent<PhysicsActor>() != nullptr)
	{
		_playerBoundingOffset = m_Player->GetComponent<PhysicsActor>()->GetBoundingOffset().Forward;
	}

	float _myBoundingOFfset = m_PhysicsActor->GetBoundingOffset().Forward;

	// 공격범위
	// 바운딩 볼륨 + offset
	m_AttackRange = _myBoundingOFfset + _playerBoundingOffset + 0.5f;

	// 공격력
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
	//CA_TRACE("좀비무브 엔터");
}

void Zombie_Runner_Move::OnCollisionStay(Collision collision)
{
	if (collision.m_GameObject->GetTag() == "Zombie")
	{
	}

	//CA_TRACE("좀비무브 스테이");
}

void Zombie_Runner_Move::OnCollisionExit(Collision collision)
{
	//CA_TRACE("좀비무브 exit");
}

/// <summary>
/// 웨이포인트 오브젝트를 파라미터로 받아서
/// 해당 오브젝트의 pos의 주소를 웨이포인트들의 벡터에 추가한다.
/// (웨이포인트 오브젝트가 이동해도 포인터로 가리키니까 문제 없이 동기화 된다.)
/// </summary>
/// <param name="wayPoint">웨이포인트 오브젝트</param>
void Zombie_Runner_Move::AddWayPoint(GameObject* wayPoint)
{
	m_WayPoints_V.emplace_back(wayPoint);
	//m_WayPointsComponentId_V.push_back(tf->GetComponetId());
}

/// <summary>
/// Enemy 가 대기할 때(Idle)의 시간 설정
/// </summary>
/// <param name="time"></param>
void Zombie_Runner_Move::SetWaitTime(const float time)
{
	m_WaitTime = time;
}

/// <summary>
/// 타겟이 탐지 범위 안에 있느지 검사
/// </summary>
/// <param name="target">탐지할 오브젝트</param>
/// <returns></returns>
bool Zombie_Runner_Move::IsTargetInDetectionRange(const GameObject& target)
{
	// 거리를 구하자
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
/// 플레이어가 시야각 안에 있는지 검사
/// </summary>
/// /// <param name="target">탐지할 오브젝트</param>
/// <returns></returns>
bool Zombie_Runner_Move::IsTargetInViewSight(const GameObject& target)
{
	// 타겟으로의 각도를 구한다
	float angle = CalcAngleToTarget(target);
	NormalizeAngle(angle);

	//CA_TRACE("angle = %f", angle);

	// 시야각 사이가 연결 되는 경우(좌측과 우측 사이에 0(360)이 안 걸리는 경우)
	if (m_ViewSight.leftSight > m_ViewSight.rightSight)
	{
		if (angle <= m_ViewSight.leftSight &&
			angle >= m_ViewSight.rightSight)
		{
			return true;
		}

	}
	// 시야각 사이에 0(360)도가 끼어서 범위가 분리되는 경우
	else
	{
		if ((0.0f <= angle) && (angle <= m_ViewSight.leftSight) ||		// 좌쪽 범위 체크
			((m_ViewSight.rightSight <= angle) && (angle <= 360.0f)))	// 우측 범위 체크

		{
			return true;
		}
	}

	// 시야각 밖에 있다
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
				0.0f,			// 넉백시킬 힘
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
/// 플레이어 감지 함수
/// </summary>
bool Zombie_Runner_Move::FindPlayer()
{
	// 프로그램 터짐 방지
	if (m_Player == nullptr)
	{
		return false;
	}

	// 플레이어 포지션 갱신
	const SimpleMath::Vector3& _playerPos = m_Player->m_Transform->m_Position;


	// 플레이어가 탐지거리 안에 들어오면
	if (IsTargetInDetectionRange(*m_Player))
	{
		/// 타겟이 시야각(범위) 안에 있는지 검사
		// 시야각 사이가 연결 되는 경우(좌측과 우측 사이에 0(360)이 안 걸리는 경우)
		if (IsTargetInViewSight(*m_Player) == true)
		{
			/// 타겟을 찾음!
			// 추적 상태로 변경
			m_bHasPlayer = true;

			m_State |= State::eHunt;

			//CA_TRACE("[EnemyMove] %s : Change State <eHunt>", m_pMyObject->GetObjectName().c_str());
			m_NavMeshAgent->SetDestination(m_Player);
			return true;

		}

		/// 타겟이 시야 밖에 있으면
		// 플레이어를 전부터 추적중이었으면
		if (m_bHasPlayer == true)
		{
			// 잠시 시야에서 벗어났어도 플레이어를 쫓도록 유지
			m_NavMeshAgent->SetDestination(m_Player);
			return true;
		}

		// 현재 프레임에 탐지거리 안에 들어와서 판단하고 있는 경우에는
		// 못본걸로 처리
		else
		{
			m_bHasPlayer = false;
			// 추적 상태 제거
			m_State &= ~State::eHunt;
			//CA_TRACE("거리는 되는데 시야각 밖에 있음!");

			return false;
		}
	}

	// 플레이어가 탐지거리를 벗어나면
	else
	{
		// 플레이어를 전부터 추적중이었으면
		if (m_bHasPlayer == true)
		{
			// 귀환 상태로 전환
			m_NavMeshAgent->SetDestination(m_Player);
			return true;
		}

		m_bHasPlayer = false;

		// 추적 상태 제거
		m_State &= ~State::eHunt;

		return false;
	}
}

/// <summary>
/// 플레이어를 향해 이동
/// </summary>
/// <returns></returns>
bool Zombie_Runner_Move::MoveForPlayer()
{
	if (m_State & State::eHunt)
	{
		/// 공격 범위 안에 들어왔나 먼저 검사
		// 거리 계산
		float _distance = Vector3::Distance(m_Transform->m_Position, m_Player->m_Transform->m_Position);
		if (_distance < m_AttackRange
			&& IsTargetInViewSight(*m_Player))
		{
			m_NavMeshAgent->SetDestination(nullptr);

			// 뱅글뱅글 도는거 방지
			m_PhysicsActor->SetFreezeRotation(true, true, true);
			// 밀림 방지
			m_PhysicsActor->SetFreezePosition(true, true, true);

			// 공격 상태 On
			m_State |= State::eAttack;

			return true;
		}

		// 추격 애니메이션 설정
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
/// 현재 지정된 웨이포인트를 향해 이동하는 함수
/// </summary>
/// <returns></returns>
bool Zombie_Runner_Move::MoveForTargetWayPoint()
{
	if (m_State & State::ePatrol)
	{
		if (m_WayPoints_V.size() > 0)
		{
			/// 타겟 웨이포인트 체크(갱신)
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
/// 웨이포인트 갱신
/// 타겟 웨이포인트에 도착하면 다음 인덱스로 타겟 웨이포인트를 새로 갱신한다
/// </summary>
/// <returns></returns>
bool Zombie_Runner_Move::UpdateTargetWayPoint()
{
	if (m_State & State::ePatrol)
	{
		// 타겟 포인트에 도착하면
		Vector3 pos = { m_Transform->m_Position.x, m_Transform->m_Position.y - m_PhysicsActor->GetBoundingOffset().Bottom, m_Transform->m_Position.z };
		float distance = Vector3::Distance(pos, m_WayPoints_V.at(m_CurrentWayPointIndex)->m_Transform->m_Position);
		if (distance < 0.2f)	// 도착 판정은 타겟과의 거리로 해보자..(아직 별도의 충돌체크 불가능)
		{
			// 웨이포인트 인덱스 증가
			m_CurrentWayPointIndex++;

			// 인덱스가 마지막 웨이포인트를 넘어가면 
			if (m_CurrentWayPointIndex >= m_WayPoints_V.size())
			{
				// 다시 맨 처음 인덱스로 돌림
				m_CurrentWayPointIndex = 0;
			}

			// 패트롤 상태 제거
			m_State &= ~State::ePatrol;

			// 대기 상태 추가
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
	// Wait 상태라면
	if (m_State & State::eWait)
	{
		if (m_WayPoints_V.size() > 0)
		{
			// 타이머 감소
			m_Timer -= CL::Input::s_DeltaTime;
			m_NavMeshAgent->SetDestination(nullptr);

			// 안미끄러지게 속도 0으로
			m_PhysicsActor->SetVelocity({ 0.0f, 0.0f, 0.0f });

			// 타이머가 다 되면
			if (m_Timer < 0.0f)
			{
				// 타이머 재설정
				m_Timer = m_WaitTime;


				// 대기 상태 제거
				m_State &= ~State::eWait;

				// 패트롤 상태 추가
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
/// 공격 후 처리
/// -> 재공격이 가능한가?
/// -> 공격상태를 해재 해야 하는가?
/// </summary>
void Zombie_Runner_Move::PostAttack()
{
	/// <summary>
	/// 여전히 플레이어가 공격범위 안에 있는지 검사
	/// </summary>
	float distance = SimpleMath::Vector3::Distance(m_Transform->m_Position, m_Player->m_Transform->m_Position);
	if (distance < m_AttackRange
		&& IsTargetInViewSight(*m_Player) == true)
	{
		// 재공격
		//CA_TRACE("[Zombie_Runner_Move] 다시 공격!");

		m_Transform->LookAtYaw(m_Player->m_Transform->m_Position);
	}

	else
	{
		// 공격상태 해제
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
	m_SaveData->m_ViewSight = &m_ViewSight;	// 어떻게 하는지 물어볼 것
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

