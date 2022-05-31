#pragma once

#include "IItem.h"

/// <summary>
/// �ѱ⿡ ���� ź ���� Ŭ����

/// �ۼ��� : Yoking
/// </summary>

class Ammo : public IItem
{
public:
	Ammo(IItem::Type type, class Inventory* inventory = nullptr, float damage = 0.0f);
	virtual ~Ammo();

	const float& GetDamage() const;
	void SetDamage(const float damage);

protected:
	float m_Damage;

private:

};

/// <summary>
/// ����(1�� �ֹ���) ź Ŭ����
/// 
/// �ۼ��� : Yoking
/// </summary>
class LongGunAmmo : public Ammo
{
public:
	LongGunAmmo(IItem::Type type, class Inventory* inventory = nullptr, float damage = 0.0f);
	virtual ~LongGunAmmo();

protected:

private:

};


/// <summary>
/// ����(2�� ��������) ź Ŭ����
///
/// �ۼ��� : Yoking
/// </summary>
class HandGunAmmo : public Ammo
{
public:
	HandGunAmmo(IItem::Type type, class Inventory* inventory = nullptr, float damage = 0.0f);
	virtual ~HandGunAmmo();

protected:

private:

};
