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
	}

	// 현재 타겟 초기화
	m_Target = { Target::Type::eNone, nullptr };

	m_Animator = m_Transform->GetChildV().at(0)->GetComponent<Animator>();

	// 플레이어 연결
	m_Player = DLLEngine::FindGameObjectByName("Player");
	m_PlayerHealth = m_Player->GetComponent<Health>();
	assert(m_PlayerHealth != nullptr);

	// 조수 연결
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

		// 뱅글뱅글 도는거 방지
		m_PhysicsActor->SetFreezeRotation(true, true, true);

		// 밀림 방지
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
			// 귀환상태 해제
			m_State &= ~State::eReturn;

			// 대기 설정
			SetWait();
		}

		return true;
	}

	return false;
}

/// <summary>
/// 사운드 이벤트와 이벤트 발생(재생) 포지션으로 
/// 좀비가 사운드를 듣는다.
/// 사운드 범위(EventDistance)안에 있어야 들을 수 있다.
/// </summary>
/// /// <param name="obj">소리를 발생시킨 오브젝트</param>
/// <param name="playPosition">재생 위치</param>
/// <param name="sound">사운드</param>
/// <returns>듣기 성공 여부 </returns>
bool Enemy_Move::HearSound(GameObject* obj, SoundEvent* sound)
{
	// 죽어 있는 경우에는 듣지 않는다.
	if ((m_State & State::eDead) == true)
	{
		return false;
	}
	
	SimpleMath::Vector3 _playPosition = obj->m_Transform->m_Position;

	// 결과값 초기화
	bool _ret = false;

	// 이전에 전투(추격, 공격) 상태나 아니었을 때만 새로 사운드를 듣기를 시도한다.
	if ((m_State & State::eAttackPartner) == false
		&& (m_State & State::eAttackPlayer) == false
		&& (m_State & State::eHunt) == false )
	{
		/// 들리는가 판별
		// 사운드 거리안에 있는가? 
		if (UtilityFunction::IsTargetInDetectionRange(m_Transform->m_Position, _playPosition, 18.0f))
		{
			float _dist = 19.0f;

			/// 중간에 벽이 막고 있는가 체크
			// 레이캐스트가 자기 자신 오브젝트를 히트 오브젝트에서 제외 하지 못해서
			// 좀비를 기준으로 전후좌우 4군데의 지점에서 사운드 재생위치를 향해 ray를 쏜다
			std::vector <std::pair<SimpleMath::Vector3, SimpleMath::Vector3>> _OriAndDir;
			_OriAndDir.reserve(4);

			// forward / right Vector
			SimpleMath::Vector3 _forward = m_Transform->m_RotationTM.Forward();
			SimpleMath::Vector3 _right = m_Transform->m_RotationTM.Right();

			// 전방 지점에서 체크할 위치 계산
			SimpleMath::Vector3 _oriNorth = m_Transform->m_Position  + _forward * (m_PhysicsActor->GetBoundingOffset().Forward + 0.1f);
			SimpleMath::Vector3 _dirNorth = _playPosition - _oriNorth;
			_dirNorth.Normalize();
			_OriAndDir.emplace_back(_oriNorth, _dirNorth );

			// 후방 지점에서 체크할 위치 계산
			SimpleMath::Vector3 _oriSouth = m_Transform->m_Position - _forward * (m_PhysicsActor->GetBoundingOffset().Forward + 0.1f);
			SimpleMath::Vector3 _dirSouth = _playPosition - _oriSouth;
			_dirSouth.Normalize();
			_OriAndDir.emplace_back(_oriSouth, _dirSouth);

			// 좌측 지점에서 체크할 위치 계산
			SimpleMath::Vector3 _oriWest = m_Transform->m_Position - _right * (m_PhysicsActor->GetBoundingOffset().Right + 0.1f);
			SimpleMath::Vector3 _dirWest = _playPosition - _oriWest;
			_dirWest.Normalize();
			_OriAndDir.emplace_back(_oriWest, _dirWest);

			// 우측 지점에서 체크할 위치 계산
			SimpleMath::Vector3 _oriEast = m_Transform->m_Position + _right * (m_PhysicsActor->GetBoundingOffset().Right + 0.1f);
			SimpleMath::Vector3 _dirEast = _playPosition - _oriEast;
			_dirEast.Normalize();
			_OriAndDir.emplace_back(_oriEast, _dirEast);

			
			// 전후좌우 4 지점에서 레이캐스트 체크
			for (const auto& ray : _OriAndDir)
			{
				RaycastHit* _hit = DLLEngine::CheckRaycastHit(ray.first, ray.second, _dist);
				// 아무런 오브젝트도 체크되지 않은 경우
				if (_hit->actor == nullptr)
				{
					CA_TRACE("충돌체 없음", );
					_ret = true;

					break;
				}

				else
				{
					CA_TRACE("충돌체 = %s", _hit->actor->GetMyObject()->GetObjectName().c_str());
					//if (_hit->actor->GetMyObject()->GetTag() != "Wall")
					if (strcmp(_hit->actor->GetMyObject()->GetObjectName().c_str(), obj->GetObjectName().c_str()) == 0)
					{
						// 좀비와 사운드 플레이 위치 사이에 다른 오브젝트가 없으면 들은걸로 처리
						_ret = true;

						break;
					}
				}
			}
		}
	
		// 사운드를 들었다면
		if (_ret == true)
		{
			// 사운드 들은 위치 갱신
			m_HearSoundPosition = _playPosition;

			// 이전에 사운드 발견이나 탐색 상태가 아니였을 때만 사운드 발견상태로 상태를 전이하고
			// 리턴 위치를 현재 위치로 갱신한다.
			if ((m_State & State::eAwakenSound) == false
				&& (m_State & State::eExplore) == false)
			{
				// 사운드 발견 상태 설정
				m_State |= State::eAwakenSound;

				// 사운드 발견 애니메이션 설정
				m_Animator->SetNoneAnimLayer("AwakenSound");

				// 리턴 위치 설정
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

		// 가만히 있도록
		m_NavMeshAgent->SetDestinationObj(nullptr);

		// 목적지를 리셋해도 이전에 가하던 힘이 남아있어 미끄러지는 현상이 발생
		// 이를 해결하기 위해 physicsActor 의 속도를 직접 0으로 설정해준다.
		m_PhysicsActor->SetVelocity({ 0.0f, 0.0f, 0.0f });

		return true;
	}

	return false;
}

/// <summary>
/// 사운드 들은 후 처리
/// </summary>
/// <returns></returns>
bool Enemy_Move::PostAwakenSound()
{
	if (m_State & State::eAwakenSound)
	{
		// AwakeSound 해제
		m_State &= ~State::eAwakenSound;

		// 탐색 상태 설정
		m_State |= State::eExplore;
		m_Animator->SetNoneAnimLayer("Patrol");	// 애니메이션은 패트롤과 같다

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
	m_bIsReturn = false;

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
	m_bIsReturn = false;

	m_Animator->SetNoneAnimLayer("Wait");
}

/// <summary>
/// 현재 지정된 웨이포인트를 향해 이동하는 함수
/// </summary>
/// <returns></returns>
bool Enemy_Move::MoveForTargetWayPoint()
{
	if (m_State & State::ePatrol)
	{
		if (m_WayPoints_V.size() > 0)
		{
			/// 타겟 웨이포인트 체크(갱신)
			m_NavMeshAgent->SetDestinationObj(m_WayPoints_V.at(m_CurrentWayPointIndex));


			//CA_TRACE("patrol , %f", m_Dir.Length());

			m_Animator->SetNoneAnimLayer("Patrol");
		}

		return true;
	}

	return false;
}

/// <summary>
/// 웨이포인트 갱신
/// 타겟 웨이포인트에 도착하면 다음 인덱스로 타겟 웨이포인트를 새로 갱신한다
/// </summary>
/// <returns></returns>
bool Enemy_Move::UpdateTargetWayPoint()
{
	if (m_State & State::ePatrol)
	{
		// 타겟 포인트에 도착하면
		Vector3 pos = { m_Transform->m_Position.x, m_Transform->m_Position.y - m_PhysicsActor->GetBoundingOffset().Bottom, m_Transform->m_Position.z };
		float distance = Vector3::Distance(pos, m_WayPoints_V.at(m_CurrentWayPointIndex)->m_Transform->m_Position);
		if (distance < 0.4f)	// 도착 판정은 타겟과의 거리로 해보자..(아직 별도의 충돌체크 불가능)
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

			//CA_TRACE("[EnemyMove] %s : Change State <ePatrol> -> <Wait>", m_pMyObject->GetObjectName().c_str());

			return true;
		}

		return false;
	}

	return false;
}

bool Enemy_Move::Wait()
{
	// Wait 상태라면
	if (m_State & State::eWait)
	{
		if (m_WayPoints_V.size() > 0)
		{
			// 타이머 감소
			m_Timer -= CL::Input::s_DeltaTime;
			m_NavMeshAgent->SetDestinationObj(nullptr);

			// 안미끄러지게 속도 0으로
			m_PhysicsActor->SetVelocity({ 0.0f, 0.0f, 0.0f });

			// 뱅글뱅글 도는거 방지
			m_PhysicsActor->SetFreezeRotation(true, true, true);
			// 밀림 방지
			m_PhysicsActor->SetFreezePosition(true, true, true);

			// 타이머가 다 되면
			if (m_Timer < 0.0f)
			{
				// 타이머 재설정
				m_Timer = m_WaitTime;


				// 대기 상태 제거
				m_State &= ~State::eWait;

				// 패트롤 상태 추가
				m_State |= State::ePatrol;

				// 이동 할 수있게 고정 해제
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
/// 웨이포인트 오브젝트를 파라미터로 받아서
/// 해당 오브젝트의 pos의 주소를 웨이포인트들의 벡터에 추가한다.
/// (웨이포인트 오브젝트가 이동해도 포인터로 가리키니까 문제 없이 동기화 된다.)
/// </summary>
/// <param name="wayPoint">웨이포인트 오브젝트</param>
void Enemy_Move::AddWayPoint(GameObject* wayPoint)
{
	m_WayPoints_V.emplace_back(wayPoint);
	//m_WayPointsComponentId_V.push_back(tf->GetComponetId());
}

/// <summary>
/// Enemy 가 대기할 때(Idle)의 시간 설정
/// </summary>
/// <param name="time"></param>
void Enemy_Move::SetWaitTime(const float time)
{
	m_WaitTime = time;
}
