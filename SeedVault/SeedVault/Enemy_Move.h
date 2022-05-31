#pragma once
#include "ComponentBase.h"

/// <summary>
/// �� ĳ���͵��� �ൿ(�׼�)�� ����� Ŭ����
/// 
/// �ۼ��� : Yoking
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

	// Behavior Tree Node�� �����Ǵ� �Լ�
	virtual bool IsDead() abstract;								// �׾����� �Ǵ�
	virtual bool IsWait() abstract;								// ���������� �Ǵ�
	virtual bool IsPatrol() abstract;							// �������ΰ� �Ǵ�
	virtual bool IsAttack() abstract;							// ���ݻ��� �Ǵ�
	virtual bool IsReturn() abstract;							// ��ȯ���� �Ǵ�

	virtual bool FindPlayer() abstract;							// �÷��̾� ã��
	virtual bool MoveForPlayer() abstract;						// �÷��̾� ����

	virtual bool AttackToPlayer() abstract;
	virtual bool AttackToPartner() abstract;

	virtual bool MoveForTargetWayPoint() abstract;				// Ÿ������ �̵�
	virtual bool UpdateTargetWayPoint() abstract;				// ���� ����Ʈ ����

	virtual bool Wait() abstract;								// ���

public:
	virtual void AddWayPoint(GameObject* wayPoint) abstract;	// ��������Ʈ �߰�
	virtual void SetWaitTime(const float time) abstract;		// �ܺο��� ���ð� ������ �� ���

	virtual void PostAttack() abstract;							// ����(�ִϸ��̼� ����)�� ó��

	virtual void FinshDie() abstract;							// Die �ִϸ��̼� ����� Dead�� ������ȯ �ϴ� �Լ�

protected:
	void UpdateViewSight();										// �þ߰� ������Ʈ
	void NormalizeAngle(float& angle);							// ���� ����ȭ [0 ~ 360]
	float CalcAngleToTarget(const GameObject& target) const;

	void UpdateAnimationVar();
	void SetPatrol();
	void SetWait();

public:
	class Health* m_Health;				// Health ������Ʈ
	class PhysicsActor* m_PhysicsActor;	// PhysicsActor ������Ʈ
	class NavMeshAgent* m_NavMeshAgent;	// NavMeshAgent ������Ʈ
	class Animator* m_Animator;

	uint m_State;						// ���� ���� üũ ����
	float m_DetectionRange;				// Ž�� �Ÿ�
	ViewSight m_ViewSight;				// �þ� ����

	float m_AttackRange;				// ���� ��Ÿ�
	float m_AttackPower;				// ���ݷ�

	float m_MoveSpeed;					// �̵� �ӵ�

	bool m_bHasPlayer;					// �÷��̾� ���� ����
	GameObject* m_Player;				// �÷��̾� Obj

	std::vector<uint> m_WayPointsComponentId_V;		// ��������Ʈ Ʈ������ ������Ʈ�� ���̵�
	std::vector<GameObject*> m_WayPoints_V;			// ��������Ʈ ����Ʈ
	uint m_CurrentWayPointIndex;					// ���� ���� ����Ʈ�� �ε���

	// var for Animator
	bool m_bIsDie;
	bool m_bIsDead;
	bool m_bIsPatrol;
	bool m_bIsWait;
	bool m_bIsHunt;

protected:
	float m_WaitTime;					// ��� �ð�
	Vector3 m_Dir;						// �̵� ���� ����

	float m_Timer;						// Ÿ�̸�
};

