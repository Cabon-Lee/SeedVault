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

	// 플레이어 컨트롤러 받아옴
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

	/// 초기 상태 지정
	m_State = State::eFollow;	// 팔로우

	/// Hp 설정
	m_Health->SetMaxHp(10.0f);
	m_Health->SetHp(m_Health->GetMaxHp());

	/// 플레이어로부터 속도를 받아온다
	m_StandSpeed = m_PlayerController->GetStandSpeed();
	m_CrouchSpeed = m_PlayerController->GetCrouchSpeed();
	m_SprintSpeed = m_PlayerController->GetSprintSpeed();

	/// 달리기 시작할 플레이어와의 떨어진 거리
	m_SprintDistance = 3.0f;

	m_NavMeshAgent->m_MoveSpeed = m_StandSpeed;
}

void Partner_Move::Update(float dTime)
{
	// 애니메이션 트랜지션용 변수 동기화
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
			// 좀비를 바라보도록 회전
			m_Transform->LookAtYaw(m_EnemyWhoIsAttackingMe->m_Transform->m_Position);
		}

		// 못움직이게 고정
		m_PhysicsActor->SetFreezePosition(true, true, true);

		// Hp 다되서 죽으면 사망처리
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
	// 탈출 상태 추가
	m_State |= State::eEscape;
	// 피격 상태 제거
	m_State &= ~State::eHit;
	m_EnemyWhoIsAttackingMe = nullptr;

	// 좀비쪽에서 풀어주면 애니메이션이 재생된다.
	m_bHitMiddle = false;
	m_bHitEnd = true;

	//CA_TRACE("[해방]");

	// Hp 초기화
	m_Health->SetHp(m_Health->GetMaxHp());

	return true;
}

bool Partner_Move::Wait()
{
	// Wait 상태라면
	if (m_State & State::eWait)
	{

		return true;
	}

	return false;
}

bool Partner_Move::MoveToPlayer()
{
	// Follow 상태라면
	if (m_State & State::eFollow)
	{
		/// 목표지점 계산
		SimpleMath::Vector3 _PlayerPos = m_PlayerController->m_Transform->m_Position;
		SimpleMath::Vector3 _backDir = -m_PlayerController->m_Transform->m_RotationTM.Forward();
		_backDir.Normalize();
		_backDir *= 0.5f;

		SimpleMath::Vector3 _destPos = _PlayerPos + _backDir;

		/// 거리 계산
		// 두 캐릭터(조수, 플레이어)의 충돌영역만큼 떨어뜨린다.
		float _playerForwardBoundingVolume = m_PlayerPhysicsActor->GetBoundingOffset().Forward;
		float _MyForwardBoundingVolume = m_PhysicsActor->GetBoundingOffset().Forward;
		float _Boundingoffset = _MyForwardBoundingVolume + _playerForwardBoundingVolume;

		//float _distance = Vector3::Distance(m_Transform->m_Position, m_PlayerController->m_Transform->m_Position);
		float _distance = Vector3::Distance(m_Transform->m_Position, _destPos);

		float _offset = 1.5f;
		if (m_PlayerController->m_bAim == true)
		{
			// 플레이어가 조준시에는 조수가 카메라 앞을 안가리도록 offset을 줄인다.
			_offset = 0.0f;
		}

		// 애니메이션 트랜지션용 변수 설정
		m_bWalk = false;
		m_bSprint = false;

		if (_distance < _Boundingoffset + _offset)
		{
			m_NavMeshAgent->SetDestinationObj(nullptr);

			// 뱅글뱅글 도는거 방지
			m_PhysicsActor->SetFreezeRotation(true, true, true);

			return true;
		}

		// 네비게이션 목적지 설정
		m_NavMeshAgent->SetDestinationPos(_destPos);

		// 피직스 고정 해제
		m_PhysicsActor->SetFreezeRotation(true, false, true);


		/// 애니메이터 설정
		// 거리가 멀어지면 달리기 상태로 변경
		if (_distance > m_SprintDistance)
		{
			m_bWalk = false;
			m_bSprint = true;

			// 달릴 떄는 선상태로 고정
			m_bCrouch = false;

			m_NavMeshAgent->m_MoveSpeed = m_SprintSpeed;
		}
		else
		{
			m_bWalk = true;
			m_bSprint = false;

			// 걸을 때는 플레이어의 자세에 따라 앉을지 설지 결정

			// 앉기 상태
			if (m_bCrouch == true)
			{
				m_NavMeshAgent->m_MoveSpeed = m_CrouchSpeed;
			}

			// 선 상태
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
