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


	/// 초기 상태 지정
	SetWait();

	/// Health 세팅
	m_Health->SetHp(4.0f);

	/// 속도 설정
	m_WaitTime = 15.0f;
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
	m_AttackRange = _myBoundingOFfset + _playerBoundingOffset + 0.6f;

	// 공격력
	m_AttackPower = 2.0f;
}

void Zombie_Runner_Move::Update(float dTime)
{
	Enemy_Move::Update(dTime);

	// 좌측, 우측 범위각 계산
	UpdateViewSight();

	//CA_TRACE("now Target %d", static_cast<int>(m_Target.type));
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
		/// <summary>
		/// 이동중에 다른 좀비와 부딪히면 옆으로 살짝 이동시켜 준다.
		/// </summary>
		/// <param name="collision"></param>
		FreezePositionAndRotation::Position _freezePos = collision.m_Actor->GetFreezePosition();
		if (_freezePos.x == true && _freezePos.z == true)
		{
			// 우측으로 이동하기위한 사선 벡터를 만든다.
			SimpleMath::Vector3 _dir = m_Transform->m_RotationTM.Right();
			_dir.Normalize();
			_dir *= CL::Input::s_DeltaTime;

			m_Transform->SetPosition(m_Transform->m_Position + _dir);
		}
	}

	//CA_TRACE("좀비무브 스테이");
}

void Zombie_Runner_Move::OnCollisionExit(Collision collision)
{
	//CA_TRACE("좀비무브 exit");
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
	float angle = UtilityFunction::CalcAngleToTarget(*m_pMyObject, target);
	UtilityFunction::NormalizeAngle(angle);

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
	// 먼저 탐색을 해서 감지되는 타겟이 있으면 
	// 리턴 중지
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

		// 가만히 있도록
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
		// Awake 해제
		m_State &= ~Zombie_Runner_Move::State::eAwakenSight;

		// Hunt 상태 설정
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
/// 타겟(플레이어) 감지 함수
/// </summary>
bool Zombie_Runner_Move::FindPlayer()
{
	// 프로그램 터짐 방지
	if (m_Player == nullptr)
	{
		return false;
	}


	// 플레이어 사망 상태면 무시
	if (m_PlayerHealth->IsDead() == true)
	{
		m_State &= ~State::eAttackPlayer;
		m_Target.type = Target::Type::eNone;
		m_Target.object = nullptr;
		return false;
	}

	/// 타겟 포지션 갱신
	// 플레이어 포지션 갱신
	const SimpleMath::Vector3& _playerPos = m_Player->m_Transform->m_Position;
	// 조수 포지션 갱신
	//const SimpleMath::Vector3& _partnerPos = m_Partner

	// 플레이어가 탐지거리 안에 들어오면
	if (UtilityFunction::IsTargetInDetectionRange(*m_pMyObject, *m_Player, m_DetectionRange))
	{
		/// 타겟이 시야각(범위) 안에 있는지 검사
		// 시야각 사이가 연결 되는 경우(좌측과 우측 사이에 0(360)이 안 걸리는 경우)
		if (IsTargetInViewSight(*m_Player) == true)
		{
			/// 타겟을 찾음!
			// 처음 발견했을 때 
			if (m_Target.object == nullptr)
			{
				// 귀환위치 설정
				m_ReturnPosition = m_Transform->m_Position;
				// 귀환 상태 항상 해제
				m_State &= ~State::eReturn;

				// 탐색 상태 해제
				m_State &= ~State::eExplore;

				// eAwakenSight 상태 설정
				m_State |= State::eAwakenSight;

				// 플레이어 바라보도록 회전
				m_Transform->LookAtYaw(m_Player->m_Transform->m_Position);

				// 패트롤 설정 해제
				m_State &= ~State::ePatrol;
			}

			// 타겟 설정 변경
			m_Target = { Target::Type::ePlayer, m_Player };

			//CA_TRACE("[EnemyMove] %s : Change State <eHunt>", m_pMyObject->GetObjectName().c_str());
			//m_NavMeshAgent->SetDestinationObj(m_Player);

			return true;
		}

		/// 타겟이 시야 밖에 있으면
		// 플레이어를 전부터 추적중이었으면
		if (m_Target.type == Target::Type::ePlayer)
		{
			// 잠시 시야에서 벗어났어도 플레이어를 쫓도록 유지
			m_NavMeshAgent->SetDestinationObj(m_Player);
			return true;
		}

		// 현재 프레임에 탐지거리 안에 들어와서 판단하고 있는 경우에는
		// 못본걸로 처리
		else
		{
			m_Target = { Target::Type::eNone, nullptr };
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
		if (m_Target.type == Target::Type::ePlayer)
		{
			// 추적 상태 제거
			m_State &= ~State::eHunt;
			m_Target = { Target::Type::eNone, nullptr };

			// 귀환 상태로 전환
			m_State |= State::eReturn;

			m_Animator->SetNoneAnimLayer("Patrol");

			return false;
		}

		// 전부터 계속 탐지거리 밖이었으면
		else
		{
			m_Target = { Target::Type::eNone, nullptr };

			// 추적 상태 제거
			m_State &= ~State::eHunt;

			return false;
		}
	}
}

/// <summary>
/// 조수 감지 함수
/// </summary>
/// <returns></returns>
bool Zombie_Runner_Move::FindPartner()
{
	// 프로그램 터짐 방지
	if (m_Partner == nullptr)
	{
		return false;
	}

	// 조수 사망 상태면 무시
	if (m_PartnerHealth->IsDead() == true)
	{
		return false;
	}

	/// 타겟 포지션 갱신
	// 조수 포지션 갱신
	const SimpleMath::Vector3& _partnerPos = m_Partner->m_Transform->m_Position;

	// 조수가 탐지거리 안에 들어오면
	if (UtilityFunction::IsTargetInDetectionRange(*m_pMyObject, *m_Partner, m_DetectionRange))
	{
		/// 타겟이 시야각(범위) 안에 있는지 검사
		// 시야각 사이가 연결 되는 경우(좌측과 우측 사이에 0(360)이 안 걸리는 경우)
		if (IsTargetInViewSight(*m_Partner) == true)
		{
			/// 타겟을 찾음!
			// 처음 발견했을 때 
			if (m_Target.type == Target::Type::eNone)
			{
				// 귀환위치 설정
				m_ReturnPosition = m_Transform->m_Position;
				// 귀환 상태항상 해제
				m_State &= ~State::eReturn;

				// 탐색 상태 해제
				m_State &= ~State::eExplore;

				// AwakenSight 상태 설정
				m_State |= State::eAwakenSight;

				// 조수를 바라보도록 회전
				m_Transform->LookAtYaw(m_Partner->m_Transform->m_Position);

				// 가만히 있게 목적지 설정 해제
				m_State &= ~State::ePatrol;
			}

			// 추적 상태로 변경
			m_Target = { Target::Type::ePartner, m_Partner };

			m_State |= State::eHunt;

			//CA_TRACE("[EnemyMove] %s : Change State <eHunt>", m_pMyObject->GetObjectName().c_str());
			//m_NavMeshAgent->SetDestinationObj(m_Partner);

			return true;
		}

		/// 타겟이 시야 밖에 있으면
		// 조수를 전부터 추적중이었으면
		if (m_Target.type == Target::Type::ePartner)
		{
			// 잠시 시야에서 벗어났어도 플레이어를 쫓도록 유지
			m_NavMeshAgent->SetDestinationObj(m_Partner);
			return true;
		}

		// 현재 프레임에 탐지거리 안에 들어와서 판단하고 있는 경우에는
		// 못본걸로 처리
		else
		{
			m_Target = { m_Target.type, m_Target.object };

			if (m_Target.type == Target::Type::eNone)
			{
				// 추적 상태 제거
				m_State &= ~State::eHunt;
			}

			//CA_TRACE("거리는 되는데 시야각 밖에 있음!");

			return false;
		}
	}

	// 조수가 탐지거리를 벗어나면
	else
	{
		// 조수 전부터 추적중이었으면
		if (m_Target.type == Target::Type::ePartner)
		{
			// 추적 상태 제거
			m_State &= ~State::eHunt;
			m_Target = { m_Target.type, m_Target.object };

			// 귀환 상태로 전환
			m_State |= State::eReturn;

			m_Animator->SetNoneAnimLayer("Patrol");

			return false;
		}

		else
		{
			m_Target = { m_Target.type, m_Target.object };

			// 조수가 감지되지 않아도 기존에 플레이어를 추격중이었을 수 있으니
			// type이 none일 때만 추격상태를 해제한다.
			if (m_Target.type == Target::Type::eNone)
			{
				// 추적 상태 제거
				m_State &= ~State::eHunt;
			}

			return false;
		}
	}
}

/// <summary>
/// 플레이어를 향해 이동
/// </summary>
/// <returns></returns>
bool Zombie_Runner_Move::MoveToTarget()
{
	if (m_State & State::eHunt)
	{
		// 타겟이 죽은경우 추적 해제하고 리턴상태로 변경
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

		/// 공격 범위 안에 들어왔나 먼저 검사
		// 거리 계산
		float _distance = Vector3::Distance(m_Transform->m_Position, m_Target.object->m_Transform->m_Position);
		if (_distance < m_AttackRange
			&& IsTargetInViewSight(*(m_Target.object)))
		{
			m_NavMeshAgent->SetDestinationObj(nullptr);

			// 뱅글뱅글 도는거 방지
			m_PhysicsActor->SetFreezeRotation(true, true, true);
			// 밀림 방지
			m_PhysicsActor->SetFreezePosition(true, true, true);

			// 타겟 타입에 따른 공격 상태 On
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

		// 추격 애니메이션 설정
		m_Animator->SetNoneAnimLayer("Hunt");

		m_PhysicsActor->SetFreezePosition(false, true, false);

		return true;
	}

	return false;
}

/// <summary>
/// 플레이어 공격
/// 애니메이션 재생
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
/// 조수 공격
/// 애니메이션 재생 및 상태 추가 
/// </summary>
/// <returns></returns>
bool Zombie_Runner_Move::AttackToPartner()
{
	if (m_State & State::eAttackPartner)
	{
		float _damage = CL::Input::s_DeltaTime;

		// 이미 조수를 공격하고 있는 다른 좀비가 있다면
		if (m_PartnerMove->m_EnemyWhoIsAttackingMe != nullptr
			&& m_PartnerMove->m_EnemyWhoIsAttackingMe != this)
		{
			// 공격력 만땅(즉사시키기 위해)
			_damage = 10000.0f;
		}

		// 물어뜯기 애니메이션 재생
		m_Transform->LookAtYaw(m_PartnerMove->m_Transform->m_Position);
		m_Animator->SetNoneAnimLayer("AttackPartner");

		// 대미지 적용
		m_PartnerHealth->Damage(
			_damage,
			m_PartnerHealth->m_Transform->m_Position,
			SimpleMath::Vector3::Zero,
			0.0f,
			m_pMyObject
		);

		// 조수 탈출상태 해제
		m_PartnerMove->m_State &= ~Partner_Move::State::eEscape;

		// 조수 피격 중 상태 추가
		m_PartnerMove->m_State |= Partner_Move::State::eHit;
		m_PartnerMove->m_bHitBegin = true;

		// 먼저 공격중이던 좀비가 없으면 this 좀비를 조수의 공격하는 좀비로 세팅
		if (m_PartnerMove->m_EnemyWhoIsAttackingMe == nullptr)
		{
			m_PartnerMove->m_EnemyWhoIsAttackingMe = this;
		}

		// 조수의 체력이 다 떨어져서 죽었다면
		if (m_PartnerHealth->IsDead() == true)
		{
			// 좀비의 조수 공격상태 해제는 애니메이션이 끝나는 시점에 이벤트로~
			// 
			// 애니메이션 상태 변경
			m_bIsAttackPartner_Middle = false;
			m_bIsAttackPartner_End = true;

			// 포지션 고정 해제
			m_PhysicsActor->SetFreezePosition(false, true, false);
		}

		return true;
	}

	return false;
}

/// <summary>
/// 공격 후 처리
/// -> 재공격이 가능한가?
/// -> 공격상태를 해재 해야 하는가?
/// </summary>
void Zombie_Runner_Move::PostAttackPlayer()
{
	/// <summary>
	/// 여전히 타겟이 공격범위 안에 있는지 검사
	/// </summary>

	float distance = SimpleMath::Vector3::Distance(m_Transform->m_Position, m_PlayerHealth->m_Transform->m_Position);
	if (m_PlayerHealth->IsDead() == false
		&& distance < m_AttackRange
		&& IsTargetInViewSight(*(m_Player)) == true)
	{
		// 재공격
		//CA_TRACE("[Zombie_Runner_Move] 다시 공격!");

		m_Transform->LookAtYaw(m_Player->m_Transform->m_Position);

		m_PhysicsActor->SetFreezePosition(false, true, false);
		m_PhysicsActor->SetFreezeRotation(true, false, true);

		return;
	}

	// 공격상태 해제
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
/// 피암살 후 처리
/// </summary>
void Zombie_Runner_Move::PostAssassinated()
{
	// 피암살 상태 해제
	m_State &= ~State::eAssassinated;

	// 데미지 적용
	m_Health->Damage(
		100.0f,		// 암살데미지(그냥 무지막지하게 큰값 넣어서 한번에 죽게끔)
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
		// 탐색 중에도 적 감지는 계속 한다.
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
	m_SaveData->m_ViewSight = &m_ViewSight;	// 어떻게 하는지 물어볼 것
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
/// A.I Agent의 각(yaw)와 시야각(angle)을 기준으로
/// 좌측, 우측 범위에 해당하는 각을 계산한다
/// </summary>
void Zombie_Runner_Move::UpdateViewSight()
{
	// 좌측, 우측 범위 각도 계산
	m_ViewSight.leftSight = m_Transform->m_EulerAngles.y + (m_ViewSight.angle / 2.0f);
	m_ViewSight.rightSight = m_Transform->m_EulerAngles.y - (m_ViewSight.angle / 2.0f);

	// 0 ~ 360 범위로 정규화
	UtilityFunction::NormalizeAngle(m_ViewSight.leftSight);
	UtilityFunction::NormalizeAngle(m_ViewSight.rightSight);
}

/// <summary>
/// 죽었을 때 조수를 공격중이었으면 조수를 해방한다.
/// </summary>
void Zombie_Runner_Move::ReleasePartner() const
{
	// 현재 조수를 공격중이었으면
	if (m_PartnerMove->m_EnemyWhoIsAttackingMe == this)
	{
		// 조수를 해방한다.
		m_PartnerMove->Escape();
	}
}

