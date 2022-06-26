#pragma once

/// <summary>
/// Gun 클래스
/// 주무기와, 보조무기로 나뉘며 각각 Ammo를 사용해서 공격한다.
/// 공격은 RayCast(히트스캔)으로 한다.
/// 
/// 작성자 : 최 요 환
/// </summary>

#include "IItem.h"
class Audio;
class Gun : public EquipmentItem
{
public:
	Gun(IItem::Type type, class Inventory* inventory = nullptr);
	virtual ~Gun();

	virtual bool Use() override;
	virtual bool Shoot();
	void StartReload();
	virtual uint Reload() abstract;

	// 인벤토리에 탄약이 비었나 검사
	virtual bool IsAmmoEmpty() const abstract;

	// 탄창에 탄이 비었나 검사
	bool IsMagazineEmpty() const;
	// 탄창에 탄이 꽉찼나 검사
	bool IsMagazineFull() const;
	
	bool SetCurrentAmmoCount(const uint ammo);
	uint GetCurrentAmmoCount() const;
	uint IncreaseCurrentAmmoCount(const uint&& val);
	uint DecreaseCurrentAmmoCount(const uint&& val);

	const float& GetReloadSpeed() const;
	void SetReloadCoolTimer(const float& time);
	bool IsReloading() const;
	void ReloadCoolDown();
	uint GetCurrentAmmoCount()
	{
		return m_CurrentAmmoCount;
	}

protected:
	Transform* m_CameraTf;				// 공격할 때 사용할 카메라
	class MuzzleFlash* m_MuzzleFlash;	// 공격할 때 사용할 머즐플래쉬
	class HitPoint* m_HitPoint;			// 공격한 지점을 알아내는 HitPoint 컴포넌트
	class Audio* m_pAudio;

	float m_Damage;						// 대미지(공격력)
	float m_ShootRange;					// 사격 거리(범위)
	float m_ShootCoolTime;				// 사격 속도(쿨타임)

	bool m_bIsReloading;				// 재장전 중인가?
	float m_ReloadSpeed;				// 재장전 속도
	float m_ReloadCoolTimer;			// 재장전 완료까지 남은 시간

	uint m_CurrentAmmoCount;			// 현재 남은 Ammo 갯수
	uint m_MaxAmmoCount;				// 최대 장전 가능한 Ammo 갯수
};


/// <summary>
/// 주무기(장총) 클래스
/// </summary>
class LongGun : public Gun
{
public:
	LongGun(IItem::Type type, class Inventory* inventory = nullptr);
	virtual ~LongGun();

	virtual bool Shoot() override;
	virtual uint Reload() override;
	virtual bool IsAmmoEmpty() const override;

protected:

private:

};

/// <summary>
/// 보조무기(권총) 클래스
/// </summary>
class HandGun : public Gun
{
public:
	HandGun(IItem::Type type, class Inventory* inventory = nullptr);
	virtual ~HandGun();

	virtual bool Shoot() override;
	virtual uint Reload() override;
	virtual bool IsAmmoEmpty() const override;

protected:

private:

};