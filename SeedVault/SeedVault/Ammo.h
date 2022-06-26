#pragma once

#include "IItem.h"

/// <summary>
/// 총기에 들어가는 탄 기초 클래스

/// 작성자 : 최 요 환
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
/// 장총(1번 주무기) 탄 클래스
/// 
/// 작성자 : 최 요 환
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
/// 권총(2번 보조무기) 탄 클래스
///
/// 작성자 : 최 요 환
/// </summary>
class HandGunAmmo : public Ammo
{
public:
	HandGunAmmo(IItem::Type type, class Inventory* inventory = nullptr, float damage = 0.0f);
	virtual ~HandGunAmmo();

protected:

private:

};
