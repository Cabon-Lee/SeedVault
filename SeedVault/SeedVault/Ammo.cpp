#include "pch.h"
#include "Ammo.h"

Ammo::Ammo(IItem::Type type, Inventory* inventory, float damage)
	: IItem(type, inventory)
	, m_Damage(damage)
{
}

Ammo::~Ammo()
{
}

const float& Ammo::GetDamage() const
{
	return m_Damage;
}

void Ammo::SetDamage(const float damage)
{
	m_Damage = damage;
}

LongGunAmmo::LongGunAmmo(IItem::Type type, Inventory* inventory, float damage)
	: Ammo(type, inventory, damage)
{
	m_Name = "LongGunAmmo";
}

LongGunAmmo::~LongGunAmmo()
{
}

HandGunAmmo::HandGunAmmo(IItem::Type type, Inventory* inventory, float damage)
	: Ammo(type, inventory, damage)
{
	m_Name = "HandGunAmmo";
}

HandGunAmmo::~HandGunAmmo()
{
}
