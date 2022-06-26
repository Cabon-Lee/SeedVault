#pragma once
#include "ComponentBase.h"

/// <summary>
/// 조수(파트너)의 이동(행동) 클래스
/// 용병(플레이어)를 뒤따름
/// 
/// 작성자 : 최 요 환
/// </summary>

class Partner_Move : public ComponentBase
{
public:
	enum State : uint
	{
		eDead	= 1 << 0,	// 죽은 상태
		eWait	= 1 << 1,	// 대기 상태
		eFollow = 1 << 2,	// 따라가기 상태
		eHit	= 1 << 3,	// 좀비에게 공격당하는상태
		eEscape = 1 << 4,	// 좀비의 공격에서 빠져나오는 상태
	};

public:
	Partner_Move();
	virtual ~Partner_Move();
	
	// Component base override
	virtual void Start() override;
	virtual void Update(float dTime) override;
	virtual void OnRender() override;

	// Behavior Tree Node에 대응되는 함수
	bool IsDead();								// 죽었는지 판단
	bool IsWait() const;						// 대기 상태 판단
	bool IsFollow() const;						// 따라가기 상태 판단
	bool IsHit() const;							// 좀비에게 공격당하는 상태 판단
	bool IsEscape() const;						// 좀비의 공격에서 빠져나오는상태판단

	bool Wait();								// 대기
	bool MoveToPlayer();						// 플레이어쪽으로 이동
	bool Hit();									// 공격 당하는 중 처리
	bool Escape();


public:
	class Animator* m_Animator;					// 애니메이터
	class PhysicsActor* m_PhysicsActor;			// 피직스 액터
	class NavMeshAgent* m_NavMeshAgent;			// NavMeshAgent 컴포넌트
	class Health* m_Health;						// Health 컴포넌트
	
	class PlayerController* m_PlayerController;	// 플레이어 컨트롤러
	class PhysicsActor* m_PlayerPhysicsActor;	// 플레이어 피직스 액터

	class Enemy_Move* m_EnemyWhoIsAttackingMe;	// 조수를 공격중인 적

	// 상태 변수
	uint m_State;

	// 이동 속도
	float m_StandSpeed;
	float m_CrouchSpeed;
	float m_SprintSpeed;
	float m_MoveSpeed;

	float m_SprintDistance;						// 조수가 달릴 플레이어와의 떨어진 거리

	// Animator 트랜지션용 변수
	bool m_bWalk;
	bool m_bSprint;
	bool m_bCrouch;

	bool m_bHitBegin;
	bool m_bHitMiddle;
	bool m_bHitEnd;

	bool m_bDie;
	bool m_bDead;
};

