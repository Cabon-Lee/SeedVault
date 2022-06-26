#pragma once
#include "ComponentBase.h"

/// <summary>
/// �� ĳ���͵��� �ൿ(�׼�)�� ����� Ŭ����
/// 
/// �ۼ��� : �� �� ȯ
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
			eNone,			// ���� ������ Ÿ�� ����
			ePartner,		// ������ Ÿ������ ����
			ePlayer,		// �÷��̾ Ÿ������ ����
			eMax,
		};
		
		Type type;			// Ÿ�� Ÿ��
		GameObject* object;	// Ÿ�� ������Ʈ
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
	virtual bool IsWait();										// ���������� �Ǵ�
	virtual bool IsPatrol();									// �������ΰ� �Ǵ�
	virtual bool IsAttackPartner() abstract;					// ���� ���ݻ��� �Ǵ�
	virtual bool IsAttackPlayer() abstract;						// �÷��̾� ���ݻ��� �Ǵ�
	virtual bool IsReturn();									// ��ȯ���� �Ǵ�
	virtual bool IsAssassinated();								// �ϻ���ϴ� ���� �Ǵ�

	virtual bool MoveToReturnPoint();							// ��ȯ ��ġ�� �̵�
	bool FinishReturn();										// ��ȯ ����

	virtual bool AwakenSight() abstract;						// �þ� �߰� ����
	virtual bool PostAwakenSight() abstract;					// �þ� �߰� �� ó��
	virtual bool MoveToTarget() abstract;						// Ÿ�� ����

	virtual bool AttackToPlayer() abstract;
	virtual bool AttackToPartner() abstract;

	bool MoveForTargetWayPoint();								// Ÿ������ �̵�
	bool UpdateTargetWayPoint();								// ���� ����Ʈ ����

	bool Wait();												// ���

public:
	void AddWayPoint(GameObject* wayPoint);						// ��������Ʈ �߰�
	void SetWaitTime(const float time);							// �ܺο��� ���ð� ������ �� ���

	virtual void FinshDie() abstract;							// Die �ִϸ��̼� ����� Dead�� ������ȯ �ϴ� �Լ�

	bool HearSound(GameObject* obj, SoundEvent* sound);

	bool AwakenSound();											// ���� �߰� ����
	bool PostAwakenSound();										// ���� �߰� �� ó��

	virtual bool Explore();										// Ž��


protected:
	void SetPatrol();
	void SetWait();

public:
	class Health* m_Health;				// Health ������Ʈ
	class PhysicsActor* m_PhysicsActor;	// PhysicsActor ������Ʈ
	class NavMeshAgent* m_NavMeshAgent;	// NavMeshAgent ������Ʈ
	class Animator* m_Animator;

	uint m_State;						// ���� ���� üũ ����


	float m_AttackRange;				// ���� ��Ÿ�
	float m_AttackPower;				// ���ݷ�

	float m_MoveSpeed;					// �̵� �ӵ�

	/// ��(�÷��̾�, ����) ����
	Target m_Target;						// ���� Ÿ��
	
	GameObject*			m_Player;			// �÷��̾� Obj
	class Health*		m_PlayerHealth;		// �÷��̾� �ﾲ
	GameObject*			m_Partner;			// ���� Obj
	class Partner_Move* m_PartnerMove;		// ���� Obj
	class Health*		m_PartnerHealth;	// ���� �ﾲ

	std::vector<uint> m_WayPointsComponentId_V;		// ��������Ʈ Ʈ������ ������Ʈ�� ���̵�
	std::vector<GameObject*> m_WayPoints_V;			// ��������Ʈ ����Ʈ
	uint m_CurrentWayPointIndex;					// ���� ���� ����Ʈ�� �ε���

	// ��ȯ�� �� �̵��� ��ġ
	SimpleMath::Vector3 m_ReturnPosition;

	// ���� �鸰 ��ġ(Ž���� ��ġ)
	SimpleMath::Vector3 m_HearSoundPosition;

	// ���� üũ ����
	bool m_bIsDie;
	bool m_bIsDead;
	bool m_bIsPatrol;
	bool m_bIsWait;
	bool m_bIsHunt;
	bool m_bIsReturn;
	bool m_bCanBeAssassinated;			// �ϻ� ���� �� �ִ� ����
	bool m_bIsAssassinated;				// �ϻ���ϴ� ���� �Ǻ�
	bool m_bIsAttackPartner_Middle;		// ���� ���� �߰� �ִϸ��̼� �Ǻ�
	bool m_bIsAttackPartner_End;		// ���� ���� �� ������ �ִϸ��̼� �Ǻ�

protected:
	float m_WaitTime;					// ��� �ð�
	Vector3 m_Dir;						// �̵� ���� ����

	float m_Timer;						// Ÿ�̸�
};

