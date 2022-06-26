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
/// Hp ����
/// </summary>
/// <param name="hp">���� ������ hp</param>
void Health::SetHp(const float hp)
{
	m_Hp = hp;
}

/// <summary>
/// Hp ����
/// ���� Hp�� val��ŭ + �Ѵ�.
/// </summary>
/// <param name="val">������ ��</param>
void Health::IncreaseHp(const float val)
{
	m_Hp += val;
}

/// <summary>
/// Hp ����
/// ���� Hp�� val��ŭ - �Ѵ�.
/// </summary>
/// <param name="val">������ ��</param>
void Health::DecreaseHp(const float val)
{
	m_Hp -= val;
}

/// <summary>
/// ��ҳ�?
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
/// �����ΰ�?
/// </summary>
/// <returns></returns>
bool Health::IsInvincible() const
{
	return m_bInvincible;
}

/// <summary>
/// �������� ���� �� ����
/// </summary>
/// <param name="val">������ ��</param>
void Health::SetInvincible(const bool val)
{
	m_bInvincible = val;
}

/// <summary>
/// ������ ����
/// </summary>
/// <param name="amount">���ط�</param>
void Health::Damage(float amount)
{
	Damage(amount, m_Transform->m_Position, Vector3::Zero, 1.0f, nullptr, nullptr);
}

/// <summary>
/// ������ ����
/// </summary>
/// <param name="amount">���ط�</param>
/// <param name="position">��ġ</param>
/// <param name="direction">����</param>
/// <param name="forceMagnitude">ũ��</param>
void Health::Damage(float amount, Vector3 position, Vector3 direction, float forceMagnitude)
{
	Damage(amount, position, direction, forceMagnitude, nullptr, nullptr);
}

/// <summary>
/// ������ ����
/// </summary>
/// <param name="amount">���ط�</param>
/// <param name="position">��ġ</param>
/// <param name="direction">����</param>
/// <param name="forceMagnitude">ũ��</param>
/// <param name="attacker">������</param>
void Health::Damage(float amount, Vector3 position, Vector3 direction, float forceMagnitude, GameObject* attacker)
{
	Damage(amount, position, direction, forceMagnitude, attacker, nullptr);
}

/// <summary>
/// ������ ����
/// </summary>
/// <param name="amount">���ط�</param>
/// <param name="position">��ġ</param>
/// <param name="direction">����</param>
/// <param name="forceMagnitude">ũ��</param>
/// <param name="attacker">������</param>
/// <param name="hitPhysicsActor">��Ʈ�� �ݶ��̴�</param>
void Health::Damage(float amount, Vector3 position, Vector3 direction, float forceMagnitude, GameObject* attacker, PhysicsActor* hitPhysicsActor)
{
	m_DamageData.SetDamage(amount, position, direction, forceMagnitude, attacker, hitPhysicsActor);

	OnDamage(m_DamageData);
}

/// <summary>
/// ������ ����
/// 
/// ������ ������ �������� �����Ų��.
/// </summary>
/// <param name="damageData">������ ����</param>
void Health::OnDamage(DamageData damageData)
{
	// �����ڰ� ���� ���
	if (damageData.GetOwner() == nullptr)
	{
		return;
	}

	// ��������
	if (IsInvincible())
	{
		return;
	}

	// Hp�� ���� ������ �������ŭ ���ҽ�Ų��.
	if (m_Hp > m_MinHp)
	{
		m_Hp -= damageData.GetAmount();

		CA_TRACE("[Health] %s : ���� Hp = %f", m_pMyObject->GetObjectName().c_str(), m_Hp);
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

	/// �ν�Ʈ�� ��� ���� �ʿ� need hee jung
	//	m_SaveData->m_DamageData = &m_DamageData;	// ���� ���� �Ұ���

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
