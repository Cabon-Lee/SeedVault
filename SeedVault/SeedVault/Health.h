#pragma once
#include "DamageData.h"

/// <summary>
/// �ｺ �ý��� ���� Ŭ����
/// 
/// Hp ����, �ǰ� ��
/// 
/// �ۼ��� : YoKing
/// </summary>

class Health : public ComponentBase
{
public:
	Health();
	virtual ~Health();

	// ComponentBase Override
	virtual void Start() override;
	virtual void Update(float dTime) override;
	virtual void OnRender() override;


	virtual void OnCollisionEnter(Collision collision) override;
	virtual void OnCollisionStay(Collision collision) override;
	virtual void OnCollisionExit(Collision collision) override;
	

	float GetHp() const;
	void SetHp(const float hp);

	void IncreaseHp(const float val);
	void DecreaseHp(const float val);

	bool IsAlive() const;
	bool IsDead() const;
	bool IsInvincible() const;

	// ������ ������ ����
	void Damage(float amount);
	void Damage(float amount, Vector3 position, Vector3 direction, float forceMagnitude);
	void Damage(float amount, Vector3 position, Vector3 direction, float forceMagnitude, GameObject* attacker);
	void Damage(float amount, Vector3 position, Vector3 direction, float forceMagnitude, GameObject* attacker, PhysicsActor* hitPhysicsActor);


	// ������ ����
	void OnDamage(class DamageData damageData);

protected:
	bool m_bIsDead;			// true�� ���� ����

	float  m_MaxHp;			// Max Hp
	const float m_MinHp;	// Min Hp
	float m_Hp;				// Health Point

	bool m_bInvincible;		// true �� �������� ���� ����

	DamageData m_DamageData; // DamageData

private:


public:
	virtual void SaveData();
	virtual void LoadData();

	struct Health_Save* m_SaveData;
};

struct Health_Save
{
	bool m_bEnable;
	uint m_ComponentId;

	bool m_bIsDead;
	int m_Hp;

	bool m_bInvincible;

	//DamageData* m_DamageData;	// ���� ���� �Ұ���
};


BOOST_DESCRIBE_STRUCT(Health_Save, (), (
	m_bEnable,
	m_bIsDead,

	m_bIsDead,
	m_Hp,
	m_bInvincible//,

	//m_DamageData	// ���� ���� �Ұ���
	))