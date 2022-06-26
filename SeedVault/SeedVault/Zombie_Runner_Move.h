#pragma once

#include "Enemy_Move.h"

/// <summary>z
/// ���� ���� �̵� Ŭ����
/// ����, ����, ���� ���� �ൿ �ʿ�
/// 
/// �ۼ��� : �� �� ȯ
/// </summary>

struct ViewSight
{
	ViewSight()
		: angle(0)
		, leftSight(0)
		, rightSight(0) {}

	float angle;		// �þ� ��
	float leftSight;	// ���� �þ��� ������ �ش��ϴ� ��
	float rightSight;	// ���� �þ��� ������ �ش��ϴ� ��
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
	// Behavior Tree Node�� �����Ǵ� �Լ�
	virtual bool AwakenSight() override;						// �þ� �߰� ���� �Ǵ�
	virtual bool PostAwakenSight() override;					// AwakeSight �� ó��
	virtual bool MoveToTarget() override;						// Ÿ�� ����

	virtual bool AttackToPlayer() override;						// �÷��̾� ����
	virtual bool AttackToPartner() override;					// ���� ����

	virtual bool IsDead() override;								// �׾����� �Ǵ�
	virtual bool IsAttackPartner() override;					// ���� �������ΰ� �Ǵ�
	virtual bool IsAttackPlayer() override;						// �÷��̾� �������ΰ� �Ǵ�

	virtual bool MoveToReturnPoint() override;					// ��ȯ ��ġ�� �̵�

public:
	bool FindPlayer();											// �÷��̾� ã��
	bool FindPartner();											// �÷��̾� ã��

	bool IsTargetInDetectionRange(const GameObject& target);	// Ÿ���� Ž�� ���� �ȿ� �ִ°�?
	bool IsTargetInViewSight(const GameObject& target);			// Ÿ���� �þ߰� �ȿ� �ִ°�?
	
	bool DamageToPlayer();										// �÷��̾�� ������ ����

	void PostAttackPlayer();									// �÷��̾� ����(�ִϸ��̼� ����)�� ó��
	void PostAssassinated();									// �Ǿϻ�(�ִϸ��̼� ����)�� ó��
	
	virtual void FinshDie() override;							// Die �ִϸ��̼� ����� Dead�� ������ȯ �ϴ� �Լ�

	virtual bool Explore() override;							// Ž��


public:
	virtual void SaveData() override;
	virtual void LoadData() override;
	virtual void LoadPtrData(std::map<unsigned int, GameObject*>* gameobject, std::map<unsigned int, ComponentBase*>* component) override;

protected:
	struct EnemyMove_Save* m_SaveData;

public:
	float m_DetectionRange;				// Ž�� �Ÿ�
	ViewSight m_ViewSight;				// �þ� ����

private:
	// �þ߰� ������Ʈ
	void UpdateViewSight();										
	
	// �׾��� �� ������ �������̾����� ������ �ع��Ѵ�.
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
