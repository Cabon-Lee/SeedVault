#pragma once
#include "ComponentBase.h"

/// <summary>
/// 적 캐릭터들의 행동(액션)을 만드는 클래스
/// 
/// 작성자 : Yoking
/// </summary>

struct ViewSight
{
	ViewSight()
		: angle(0)
		, leftSight(0)
		, rightSight(0) {}

	float angle;		// 시야 각
	float leftSight;	// 현재 시야의 좌측에 해당하는 각
	float rightSight;	// 현재 시야의 우측에 해당하는 각
};

class Enemy_Move : public ComponentBase
{
public:
	enum State : uint
	{
		eDead	= 1 << 0,
		eWait	= 1 << 1,
		ePatrol = 1 << 2,
		eHunt	= 1 << 3,
		eAttack = 1 << 4,
		eReturn = 1 << 5,
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
	virtual bool IsWait() abstract;								// 대기상태인지 판단
	virtual bool IsPatrol() abstract;							// 순찰중인가 판단
	virtual bool IsAttack() abstract;							// 공격상태 판단
	virtual bool IsReturn() abstract;							// 귀환상태 판단

	virtual bool FindPlayer() abstract;							// 플레이어 찾기
	virtual bool MoveForPlayer() abstract;						// 플레이어 추적

	virtual bool AttackToPlayer() abstract;
	virtual bool AttackToPartner() abstract;

	virtual bool MoveForTargetWayPoint() abstract;				// 타겟으로 이동
	virtual bool UpdateTargetWayPoint() abstract;				// 웨이 포인트 갱신

	virtual bool Wait() abstract;								// 대기

public:
	virtual void AddWayPoint(GameObject* wayPoint) abstract;	// 웨이포인트 추가
	virtual void SetWaitTime(const float time) abstract;		// 외부에서 대기시간 조절할 때 사용

	virtual void PostAttack() abstract;							// 공격(애니메이션 종료)후 처리

	virtual void FinshDie() abstract;							// Die 애니메이션 재생후 Dead로 상태전환 하는 함수

protected:
	void UpdateViewSight();										// 시야각 업데이트
	void NormalizeAngle(float& angle);							// 각도 정규화 [0 ~ 360]
	float CalcAngleToTarget(const GameObject& target) const;

	void UpdateAnimationVar();
	void SetPatrol();
	void SetWait();

public:
	class Health* m_Health;				// Health 컴포넌트
	class PhysicsActor* m_PhysicsActor;	// PhysicsActor 컴포넌트
	class NavMeshAgent* m_NavMeshAgent;	// NavMeshAgent 컴포넌트
	class Animator* m_Animator;

	uint m_State;						// 현재 상태 체크 변수
	float m_DetectionRange;				// 탐지 거리
	ViewSight m_ViewSight;				// 시야 범위

	float m_AttackRange;				// 공격 사거리
	float m_AttackPower;				// 공격력

	float m_MoveSpeed;					// 이동 속도

	bool m_bHasPlayer;					// 플레이어 감지 여부
	GameObject* m_Player;				// 플레이어 Obj

	std::vector<uint> m_WayPointsComponentId_V;		// 웨이포인트 트랜스폼 컴포넌트의 아이디
	std::vector<GameObject*> m_WayPoints_V;			// 웨이포인트 리스트
	uint m_CurrentWayPointIndex;					// 현재 웨이 포인트의 인덱스

	// var for Animator
	bool m_bIsDie;
	bool m_bIsDead;
	bool m_bIsPatrol;
	bool m_bIsWait;
	bool m_bIsHunt;

protected:
	float m_WaitTime;					// 대기 시간
	Vector3 m_Dir;						// 이동 방향 벡터

	float m_Timer;						// 타이머
};

