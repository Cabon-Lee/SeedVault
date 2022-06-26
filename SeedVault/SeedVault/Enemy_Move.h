#pragma once
#include "ComponentBase.h"

/// <summary>
/// 적 캐릭터들의 행동(액션)을 만드는 클래스
/// 
/// 작성자 : 최 요 환
/// </summary>

class Enemy_Move : public ComponentBase
{
public:
	enum State : uint
	{
		eDead			= 1 <<	0,
		eWait			= 1 <<	1,
		ePatrol			= 1 <<	2,
		eHunt			= 1 <<	3,
		eAttackPartner	= 1 <<	4,
		eAttackPlayer	= 1 <<	5,
		eReturn			= 1 <<	6,
		eAwakenSight	= 1 <<	7,
		eAwakenSound	= 1 <<	8,
		eAssassinated	= 1 <<	9,
		eExplore		= 1 << 10,
	};

	struct Target
	{
		enum class Type
		{
			eNone,			// 현재 감지한 타겟 없음
			ePartner,		// 조수가 타겟으로 설정
			ePlayer,		// 플레이어가 타겟으로 설정
			eMax,
		};
		
		Type type;			// 타겟 타입
		GameObject* object;	// 타겟 오븢게트
	};

public:
	Enemy_Move();
	virtual ~Enemy_Move();

	// Component base override
	virtual void Start() override;
	virtual void Update(float dTime) override;
	virtual void OnRender() override;

	// Behavior Tree Node에 대응되는 함수
	virtual bool IsDead() abstract;								// 죽었는지 판단
	virtual bool IsWait();										// 대기상태인지 판단
	virtual bool IsPatrol();									// 순찰중인가 판단
	virtual bool IsAttackPartner() abstract;					// 조수 공격상태 판단
	virtual bool IsAttackPlayer() abstract;						// 플레이어 공격상태 판단
	virtual bool IsReturn();									// 귀환상태 판단
	virtual bool IsAssassinated();								// 암살당하는 상태 판단

	virtual bool MoveToReturnPoint();							// 귀환 위치로 이동
	bool FinishReturn();										// 귀환 종료

	virtual bool AwakenSight() abstract;						// 시야 발견 상태
	virtual bool PostAwakenSight() abstract;					// 시야 발견 후 처리
	virtual bool MoveToTarget() abstract;						// 타겟 추적

	virtual bool AttackToPlayer() abstract;
	virtual bool AttackToPartner() abstract;

	bool MoveForTargetWayPoint();								// 타겟으로 이동
	bool UpdateTargetWayPoint();								// 웨이 포인트 갱신

	bool Wait();												// 대기

public:
	void AddWayPoint(GameObject* wayPoint);						// 웨이포인트 추가
	void SetWaitTime(const float time);							// 외부에서 대기시간 조절할 때 사용

	virtual void FinshDie() abstract;							// Die 애니메이션 재생후 Dead로 상태전환 하는 함수

	bool HearSound(GameObject* obj, SoundEvent* sound);

	bool AwakenSound();											// 사운드 발견 상태
	bool PostAwakenSound();										// 사운드 발견 후 처리

	virtual bool Explore();										// 탐색


protected:
	void SetPatrol();
	void SetWait();

public:
	class Health* m_Health;				// Health 컴포넌트
	class PhysicsActor* m_PhysicsActor;	// PhysicsActor 컴포넌트
	class NavMeshAgent* m_NavMeshAgent;	// NavMeshAgent 컴포넌트
	class Animator* m_Animator;

	uint m_State;						// 현재 상태 체크 변수


	float m_AttackRange;				// 공격 사거리
	float m_AttackPower;				// 공격력

	float m_MoveSpeed;					// 이동 속도

	/// 적(플레이어, 조수) 감지
	Target m_Target;						// 현재 타겟
	
	GameObject*			m_Player;			// 플레이어 Obj
	class Health*		m_PlayerHealth;		// 플레이어 헬쓰
	GameObject*			m_Partner;			// 조수 Obj
	class Partner_Move* m_PartnerMove;		// 조수 Obj
	class Health*		m_PartnerHealth;	// 조수 헬쓰

	std::vector<uint> m_WayPointsComponentId_V;		// 웨이포인트 트랜스폼 컴포넌트의 아이디
	std::vector<GameObject*> m_WayPoints_V;			// 웨이포인트 리스트
	uint m_CurrentWayPointIndex;					// 현재 웨이 포인트의 인덱스

	// 귀환할 때 이동할 위치
	SimpleMath::Vector3 m_ReturnPosition;

	// 사운드 들린 위치(탐색할 위치)
	SimpleMath::Vector3 m_HearSoundPosition;

	// 상태 체크 변수
	bool m_bIsDie;
	bool m_bIsDead;
	bool m_bIsPatrol;
	bool m_bIsWait;
	bool m_bIsHunt;
	bool m_bIsReturn;
	bool m_bCanBeAssassinated;			// 암살 당할 수 있는 상태
	bool m_bIsAssassinated;				// 암살당하는 상태 판별
	bool m_bIsAttackPartner_Middle;		// 조수 공격 중간 애니메이션 판별
	bool m_bIsAttackPartner_End;		// 조수 공격 후 릴리즈 애니메이션 판별

protected:
	float m_WaitTime;					// 대기 시간
	Vector3 m_Dir;						// 이동 방향 벡터

	float m_Timer;						// 타이머
};

