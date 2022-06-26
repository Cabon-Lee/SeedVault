#pragma once

#include "Enemy_Move.h"

/// <summary>z
/// 러너 좀비 이동 클래스
/// 정찰, 감지, 추적 등의 행동 필요
/// 
/// 작성자 : 최 요 환
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

class Zombie_Runner_Move : public Enemy_Move
{
public:
	Zombie_Runner_Move();
	virtual ~Zombie_Runner_Move();

	virtual void Start() override;
	virtual void Update(float dTime) override;
	virtual void OnRender() override;

	virtual void OnCollisionEnter(Collision collision) override;
	virtual void OnCollisionStay(Collision collision) override;
	virtual void OnCollisionExit(Collision collision) override;

public:
	// Behavior Tree Node에 대응되는 함수
	virtual bool AwakenSight() override;						// 시야 발견 상태 판단
	virtual bool PostAwakenSight() override;					// AwakeSight 후 처리
	virtual bool MoveToTarget() override;						// 타겟 추적

	virtual bool AttackToPlayer() override;						// 플레이어 공격
	virtual bool AttackToPartner() override;					// 조수 공격

	virtual bool IsDead() override;								// 죽었는지 판단
	virtual bool IsAttackPartner() override;					// 조수 공격중인가 판단
	virtual bool IsAttackPlayer() override;						// 플레이어 공격중인가 판단

	virtual bool MoveToReturnPoint() override;					// 귀환 위치로 이동

public:
	bool FindPlayer();											// 플레이어 찾기
	bool FindPartner();											// 플레이어 찾기

	bool IsTargetInDetectionRange(const GameObject& target);	// 타겟이 탐지 범위 안에 있는가?
	bool IsTargetInViewSight(const GameObject& target);			// 타겟이 시야각 안에 있는가?
	
	bool DamageToPlayer();										// 플레이어에게 데미지 적용

	void PostAttackPlayer();									// 플레이어 공격(애니메이션 종료)후 처리
	void PostAssassinated();									// 피암살(애니메이션 종료)후 처리
	
	virtual void FinshDie() override;							// Die 애니메이션 재생후 Dead로 상태전환 하는 함수

	virtual bool Explore() override;							// 탐색


public:
	virtual void SaveData() override;
	virtual void LoadData() override;
	virtual void LoadPtrData(std::map<unsigned int, GameObject*>* gameobject, std::map<unsigned int, ComponentBase*>* component) override;

protected:
	struct EnemyMove_Save* m_SaveData;

public:
	float m_DetectionRange;				// 탐지 거리
	ViewSight m_ViewSight;				// 시야 범위

private:
	// 시야각 업데이트
	void UpdateViewSight();										
	
	// 죽었을 때 조수를 공격중이었으면 조수를 해방한다.
	void ReleasePartner() const;
};


BOOST_DESCRIBE_STRUCT(ViewSight, (), (
	angle,
	leftSight,
	rightSight
	))

struct EnemyMove_Save
{
	bool m_bEnable;
	uint m_ComponentId;

	uint m_State;

	ViewSight* m_ViewSight;

	float m_MoveSpeed;

	bool m_bHasPlayer;
	DirectX::SimpleMath::Vector3* m_PlayerPosition;

	uint m_WayPointSize;
	uint m_CurrentWayPointIndex;

	Vector3* m_Dir;

	float m_Timer;
	float m_WaitTime;
};


BOOST_DESCRIBE_STRUCT(EnemyMove_Save, (), (
	m_bEnable,
	m_ComponentId,

	m_State,

	m_ViewSight,

	m_MoveSpeed,

	m_bHasPlayer,
	m_PlayerPosition,

	m_WayPointSize,
	m_CurrentWayPointIndex,

	m_Dir,
	m_Timer,

	m_WaitTime
	))
