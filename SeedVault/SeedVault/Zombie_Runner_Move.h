#pragma once

#include "Enemy_Move.h"

/// <summary>
/// Enemy A.I 이동 클래스 테스트
/// 정찰, 감지, 추적 등의 행동 필요
/// 
/// 작성자 : YoKing
/// </summary>

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
	virtual bool FindPlayer() override;							// 플레이어 찾기
	virtual bool MoveForPlayer() override;						// 플레이어 추적

	virtual bool AttackToPlayer() override;						// 플레이어 공격
	virtual bool AttackToPartner() override;					// 조수 공격

	virtual bool IsDead() override;								// 죽었는지 판단
	virtual bool IsWait() override;								// 대기상태인지 판단
	virtual bool IsPatrol() override;							// 순찰 중인가 판단
	virtual bool IsAttack() override;							// 순찰 중인가 판단
	virtual bool IsReturn() override;							// 귀환 중인가 판단

	virtual bool MoveForTargetWayPoint() override;				// 타겟으로 이동
	virtual bool UpdateTargetWayPoint() override;				// 웨이 포인트 갱신

	virtual bool Wait();										// 대기


public:
	virtual void AddWayPoint(GameObject* wayPoint) override;	// 웨이포인트 추가
	virtual void SetWaitTime(const float time) override;		// 외부에서 대기시간 조절할 때 사용

	bool IsTargetInDetectionRange(const GameObject& target);	// 타겟이 탐지 범위 안에 있는가?
	bool IsTargetInViewSight(const GameObject& target);			// 타겟이 시야각 안에 있는가?
	
	bool DamageToPlayer();										// 플레이어에게 데미지 적용

	virtual void PostAttack() override;							// 공격(애니메이션 종료)후 처리
	virtual void FinshDie() override;							// Die 애니메이션 재생후 Dead로 상태전환 하는 함수


public:
	virtual void SaveData() override;
	virtual void LoadData() override;
	virtual void LoadPtrData(std::map<unsigned int, GameObject*>* gameobject, std::map<unsigned int, ComponentBase*>* component) override;

protected:
	struct EnemyMove_Save* m_SaveData;

private:

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
