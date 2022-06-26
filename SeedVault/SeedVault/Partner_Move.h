#pragma once
#include "ComponentBase.h"

/// <summary>
/// ����(��Ʈ��)�� �̵�(�ൿ) Ŭ����
/// �뺴(�÷��̾�)�� �ڵ���
/// 
/// �ۼ��� : �� �� ȯ
/// </summary>

class Partner_Move : public ComponentBase
{
public:
	enum State : uint
	{
		eDead	= 1 << 0,	// ���� ����
		eWait	= 1 << 1,	// ��� ����
		eFollow = 1 << 2,	// ���󰡱� ����
		eHit	= 1 << 3,	// ���񿡰� ���ݴ��ϴ»���
		eEscape = 1 << 4,	// ������ ���ݿ��� ���������� ����
	};

public:
	Partner_Move();
	virtual ~Partner_Move();
	
	// Component base override
	virtual void Start() override;
	virtual void Update(float dTime) override;
	virtual void OnRender() override;

	// Behavior Tree Node�� �����Ǵ� �Լ�
	bool IsDead();								// �׾����� �Ǵ�
	bool IsWait() const;						// ��� ���� �Ǵ�
	bool IsFollow() const;						// ���󰡱� ���� �Ǵ�
	bool IsHit() const;							// ���񿡰� ���ݴ��ϴ� ���� �Ǵ�
	bool IsEscape() const;						// ������ ���ݿ��� ���������»����Ǵ�

	bool Wait();								// ���
	bool MoveToPlayer();						// �÷��̾������� �̵�
	bool Hit();									// ���� ���ϴ� �� ó��
	bool Escape();


public:
	class Animator* m_Animator;					// �ִϸ�����
	class PhysicsActor* m_PhysicsActor;			// ������ ����
	class NavMeshAgent* m_NavMeshAgent;			// NavMeshAgent ������Ʈ
	class Health* m_Health;						// Health ������Ʈ
	
	class PlayerController* m_PlayerController;	// �÷��̾� ��Ʈ�ѷ�
	class PhysicsActor* m_PlayerPhysicsActor;	// �÷��̾� ������ ����

	class Enemy_Move* m_EnemyWhoIsAttackingMe;	// ������ �������� ��

	// ���� ����
	uint m_State;

	// �̵� �ӵ�
	float m_StandSpeed;
	float m_CrouchSpeed;
	float m_SprintSpeed;
	float m_MoveSpeed;

	float m_SprintDistance;						// ������ �޸� �÷��̾���� ������ �Ÿ�

	// Animator Ʈ�����ǿ� ����
	bool m_bWalk;
	bool m_bSprint;
	bool m_bCrouch;

	bool m_bHitBegin;
	bool m_bHitMiddle;
	bool m_bHitEnd;

	bool m_bDie;
	bool m_bDead;
};

