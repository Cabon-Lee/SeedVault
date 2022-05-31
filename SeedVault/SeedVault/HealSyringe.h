#pragma once
#include "IItem.h"

/// <summary>
/// �� �ֻ�� Ŭ����
/// 
/// �÷��̾� ĳ���Ͱ� ����ϸ� Hp�� ȸ���Ѵ�.
/// 
/// </summary>
class HealSyringe : public EquipmentItem
{
public:
	HealSyringe(IItem::Type type, class Inventory* inventory = nullptr);
	virtual ~HealSyringe();

	virtual bool Use() override;
	virtual void CoolDown() override;
	void Heal();

	bool IsHealing() const;

protected:
	float m_UseCoolTime;		// ���� ���� �ӵ�
	float m_HealPower;			// Hp ȸ����
private:
};

