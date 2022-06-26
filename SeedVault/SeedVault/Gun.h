#pragma once

/// <summary>
/// Gun Ŭ����
/// �ֹ����, ��������� ������ ���� Ammo�� ����ؼ� �����Ѵ�.
/// ������ RayCast(��Ʈ��ĵ)���� �Ѵ�.
/// 
/// �ۼ��� : �� �� ȯ
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

	// �κ��丮�� ź���� ����� �˻�
	virtual bool IsAmmoEmpty() const abstract;

	// źâ�� ź�� ����� �˻�
	bool IsMagazineEmpty() const;
	// źâ�� ź�� ��á�� �˻�
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
	Transform* m_CameraTf;				// ������ �� ����� ī�޶�
	class MuzzleFlash* m_MuzzleFlash;	// ������ �� ����� �����÷���
	class HitPoint* m_HitPoint;			// ������ ������ �˾Ƴ��� HitPoint ������Ʈ
	class Audio* m_pAudio;

	float m_Damage;						// �����(���ݷ�)
	float m_ShootRange;					// ��� �Ÿ�(����)
	float m_ShootCoolTime;				// ��� �ӵ�(��Ÿ��)

	bool m_bIsReloading;				// ������ ���ΰ�?
	float m_ReloadSpeed;				// ������ �ӵ�
	float m_ReloadCoolTimer;			// ������ �Ϸ���� ���� �ð�

	uint m_CurrentAmmoCount;			// ���� ���� Ammo ����
	uint m_MaxAmmoCount;				// �ִ� ���� ������ Ammo ����
};


/// <summary>
/// �ֹ���(����) Ŭ����
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
/// ��������(����) Ŭ����
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