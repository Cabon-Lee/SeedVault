#include "pch.h"
#include "DamageData.h"

DamageData::DamageData()
    : m_Amount(0.0f)
    , m_Position(Vector3::Zero)
    , m_Direction(Vector3::Vector3::Zero)
    , m_ForceMagnitude(0.0f)
    , m_Owner(nullptr)
    , m_HitPhysicsActor(nullptr)
{
}

DamageData::~DamageData()
{
    m_Owner = nullptr;
    m_HitPhysicsActor = nullptr;
}

float DamageData::GetAmount() const
{
    return m_Amount;
}

Vector3 DamageData::GetPosition() const
{
    return m_Position;
}

Vector3 DamageData::GetDirection() const
{
    return m_Direction;
}

float DamageData::GetForceMagnitude() const
{
    return m_ForceMagnitude;
}

GameObject* DamageData::GetOwner() const
{
    return m_Owner;
}

PhysicsActor* DamageData::GetPhysicsActor() const
{
    return m_HitPhysicsActor;
}

/// <summary>
/// ������ ���� ����
/// </summary>
/// <param name="attacker">������</param>
/// <param name="amount">���ط�</param>
/// <param name="pos">�ǰ� ��ġ</param>
/// <param name="dir">�ǰ� ����</param>
/// <param name="hitPhysicsActor">�ǰ� �ݶ��̴�</param>
void DamageData::SetDamage(float amount, Vector3 pos, Vector3 dir, float magnitude, GameObject* attacker, PhysicsActor* hitPhysicsActor)
{
    m_Amount = amount;
    m_Position = pos;
    m_Direction = dir;
    m_ForceMagnitude = magnitude;
    m_Owner = attacker;
    m_HitPhysicsActor = hitPhysicsActor;
}
