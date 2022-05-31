#pragma once
#include "DamageData.h"

/// <summary>
/// 헬스 시스탬 기초 클래스
/// 
/// Hp 정보, 피격 등
/// 
/// 작성자 : YoKing
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

	// 데미지 데이터 생성
	void Damage(float amount);
	void Damage(float amount, Vector3 position, Vector3 direction, float forceMagnitude);
	void Damage(float amount, Vector3 position, Vector3 direction, float forceMagnitude, GameObject* attacker);
	void Damage(float amount, Vector3 position, Vector3 direction, float forceMagnitude, GameObject* attacker, PhysicsActor* hitPhysicsActor);


	// 데미지 적용
	void OnDamage(class DamageData damageData);

protected:
	bool m_bIsDead;			// true면 죽은 상태

	float  m_MaxHp;			// Max Hp
	const float m_MinHp;	// Min Hp
	float m_Hp;				// Health Point

	bool m_bInvincible;		// true 면 데미지를 입지 않음

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

	//DamageData* m_DamageData;	// 현재 저장 불가능
};


BOOST_DESCRIBE_STRUCT(Health_Save, (), (
	m_bEnable,
	m_bIsDead,

	m_bIsDead,
	m_Hp,
	m_bInvincible//,

	//m_DamageData	// 현재 저장 불가능
	))