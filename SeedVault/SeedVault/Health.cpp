#include "pch.h"
#include "DamageData.h"
#include "Health.h"

Health::Health()
	: ComponentBase(ComponentType::GameLogic)
	, m_bIsDead(false)
	, m_MaxHp(0.0f)
	, m_MinHp(0.0f)
	, m_Hp(0.0f)
	, m_bInvincible(false)
	, m_DamageData()

	, m_SaveData(new Health_Save())
{
	m_MaxHp = 5.0f;
	m_Hp = m_MaxHp;
}

Health::~Health()
{
	SAFE_DELETE(m_SaveData);
}

void Health::Start()
{
	m_Hp = m_MaxHp;
}

void Health::Update(float dTime)
{
	if (m_Hp <= m_MinHp)
	{
		m_bIsDead = true;
	}
}

void Health::OnRender()
{
}


void Health::OnCollisionEnter(Collision collision)
{
	//CA_TRACE("[Health::OnCollisionEnter] my = %s / target = %s", m_pMyObject->GetObjectName().c_str(), collision.m_GameObject->GetObjectName().c_str());
}

void Health::OnCollisionStay(Collision collision)
{
	if (collision.m_GameObject->GetObjectName() == "Player")
	{
		//CA_TRACE("[Health::OnCollisionStay] my = %s / target = %s", m_pMyObject->GetObjectName().c_str(), collision.m_GameObject->GetObjectName().c_str());
	}
}

void Health::OnCollisionExit(Collision collision)
{
	//CA_TRACE("[Health::OnCollisionExit] my = %s / target = %s", m_pMyObject->GetObjectName().c_str(), collision.m_GameObject->GetObjectName().c_str());
}

float Health::GetMaxHp() const
{
	return m_MaxHp;
}

void Health::SetMaxHp(const float hp)
{
	m_MaxHp = hp;
}


float Health::GetHp() const
{
	return m_Hp;
}

/// <summary>
/// Hp 세팅
/// </summary>
/// <param name="hp">새로 설정할 hp</param>
void Health::SetHp(const float hp)
{
	m_Hp = hp;
}

/// <summary>
/// Hp 증가
/// 현재 Hp를 val만큼 + 한다.
/// </summary>
/// <param name="val">조절할 값</param>
void Health::IncreaseHp(const float val)
{
	m_Hp += val;
}

/// <summary>
/// Hp 감소
/// 현재 Hp를 val만큼 - 한다.
/// </summary>
/// <param name="val">조절할 값</param>
void Health::DecreaseHp(const float val)
{
	m_Hp -= val;
}

/// <summary>
/// 살았냐?
/// </summary>
/// <returns></returns>
bool Health::IsAlive() const
{
	if (m_Hp > m_MinHp)
	{
		return true;
	}

	return false;
}

bool Health::IsDead() const
{
	if (m_Hp <= m_MinHp)
	{
		return true;
	}

	return false;
}

/// <summary>
/// 무적인가?
/// </summary>
/// <returns></returns>
bool Health::IsInvincible() const
{
	return m_bInvincible;
}

/// <summary>
/// 무적상태 변수 값 설정
/// </summary>
/// <param name="val">설정할 값</param>
void Health::SetInvincible(const bool val)
{
	m_bInvincible = val;
}

/// <summary>
/// 데미지 세팅
/// </summary>
/// <param name="amount">피해량</param>
void Health::Damage(float amount)
{
	Damage(amount, m_Transform->m_Position, Vector3::Zero, 1.0f, nullptr, nullptr);
}

/// <summary>
/// 데미지 세팅
/// </summary>
/// <param name="amount">피해량</param>
/// <param name="position">위치</param>
/// <param name="direction">방향</param>
/// <param name="forceMagnitude">크기</param>
void Health::Damage(float amount, Vector3 position, Vector3 direction, float forceMagnitude)
{
	Damage(amount, position, direction, forceMagnitude, nullptr, nullptr);
}

/// <summary>
/// 데미지 세팅
/// </summary>
/// <param name="amount">피해량</param>
/// <param name="position">위치</param>
/// <param name="direction">방향</param>
/// <param name="forceMagnitude">크기</param>
/// <param name="attacker">공격자</param>
void Health::Damage(float amount, Vector3 position, Vector3 direction, float forceMagnitude, GameObject* attacker)
{
	Damage(amount, position, direction, forceMagnitude, attacker, nullptr);
}

/// <summary>
/// 데미지 세팅
/// </summary>
/// <param name="amount">피해량</param>
/// <param name="position">위치</param>
/// <param name="direction">방향</param>
/// <param name="forceMagnitude">크기</param>
/// <param name="attacker">공격자</param>
/// <param name="hitPhysicsActor">히트된 콜라이더</param>
void Health::Damage(float amount, Vector3 position, Vector3 direction, float forceMagnitude, GameObject* attacker, PhysicsActor* hitPhysicsActor)
{
	m_DamageData.SetDamage(amount, position, direction, forceMagnitude, attacker, hitPhysicsActor);

	OnDamage(m_DamageData);
}

/// <summary>
/// 데미지 적용
/// 
/// 조건이 맞으면 데미지를 적용시킨다.
/// </summary>
/// <param name="damageData">데미지 정보</param>
void Health::OnDamage(DamageData damageData)
{
	// 공격자가 없는 경우
	if (damageData.GetOwner() == nullptr)
	{
		return;
	}

	// 무적상태
	if (IsInvincible())
	{
		return;
	}

	// Hp가 남아 있으면 대미지만큼 감소시킨다.
	if (m_Hp > m_MinHp)
	{
		m_Hp -= damageData.GetAmount();

		CA_TRACE("[Health] %s : 남은 Hp = %f", m_pMyObject->GetObjectName().c_str(), m_Hp);
	}

	PhysicsActor* phyActor = m_pMyObject->GetComponent<PhysicsActor>();
	if (phyActor != nullptr)
	{
		Vector3 force = damageData.GetDirection() * damageData.GetForceMagnitude();
		if (damageData.GetForceMagnitude() > 0.0f)
		{
			phyActor->SetVelocity(force);
		}

		if (m_Hp <= m_MinHp)
		{
			phyActor->OnDisable();
		}
	}
}

void Health::SaveData()
{
	m_SaveData->m_bEnable = GetIsEnabled();
	m_SaveData->m_ComponentId = m_ComponentId;

	m_SaveData->m_bIsDead = m_bIsDead;
	m_SaveData->m_Hp = m_Hp;

	m_SaveData->m_bInvincible = m_bInvincible;

	/// 부스트에 등록 먼저 필요 need hee jung
	//	m_SaveData->m_DamageData = &m_DamageData;	// 현재 저장 불가능

	save::WriteValue(m_Value, EraseClass(typeid(*this).name()), *m_SaveData);
}

void Health::LoadData()
{
	save::ReadValue(m_Value, EraseClass(typeid(*this).name()), *m_SaveData);
	SetEnable(m_SaveData->m_bEnable);

	m_bIsDead = m_SaveData->m_bIsDead;
	m_Hp = m_SaveData->m_Hp;

	m_bInvincible = m_SaveData->m_bInvincible;

	//m_SaveData->m_DamageData = &m_DamageData;
}
