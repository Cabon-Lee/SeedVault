#pragma once

#include "Enemy_Move.h"

/// <summary>
/// Enemy A.I �̵� Ŭ���� �׽�Ʈ
/// ����, ����, ���� ���� �ൿ �ʿ�
/// 
/// �ۼ��� : YoKing
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
	// Behavior Tree Node�� �����Ǵ� �Լ�
	virtual bool FindPlayer() override;							// �÷��̾� ã��
	virtual bool MoveForPlayer() override;						// �÷��̾� ����

	virtual bool AttackToPlayer() override;						// �÷��̾� ����
	virtual bool AttackToPartner() override;					// ���� ����

	virtual bool IsDead() override;								// �׾����� �Ǵ�
	virtual bool IsWait() override;								// ���������� �Ǵ�
	virtual bool IsPatrol() override;							// ���� ���ΰ� �Ǵ�
	virtual bool IsAttack() override;							// ���� ���ΰ� �Ǵ�
	virtual bool IsReturn() override;							// ��ȯ ���ΰ� �Ǵ�

	virtual bool MoveForTargetWayPoint() override;				// Ÿ������ �̵�
	virtual bool UpdateTargetWayPoint() override;				// ���� ����Ʈ ����

	virtual bool Wait();										// ���


public:
	virtual void AddWayPoint(GameObject* wayPoint) override;	// ��������Ʈ �߰�
	virtual void SetWaitTime(const float time) override;		// �ܺο��� ���ð� ������ �� ���

	bool IsTargetInDetectionRange(const GameObject& target);	// Ÿ���� Ž�� ���� �ȿ� �ִ°�?
	bool IsTargetInViewSight(const GameObject& target);			// Ÿ���� �þ߰� �ȿ� �ִ°�?
	
	bool DamageToPlayer();										// �÷��̾�� ������ ����

	virtual void PostAttack() override;							// ����(�ִϸ��̼� ����)�� ó��
	virtual void FinshDie() override;							// Die �ִϸ��̼� ����� Dead�� ������ȯ �ϴ� �Լ�


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
