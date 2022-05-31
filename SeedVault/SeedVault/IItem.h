#pragma once

/// <summary>
/// 아이템 정보 클래스
/// 
/// 작성자 : YoKing
/// </summary>

class IItem
{
public:
	enum class Type
	{
		eLongGun,
		eHandGun,
		eFlask,
		eFireFlask,
		eAlcohol,
		eHerb,
		eHealSyringe,
		eLongGunAmmo,
		eHandGunAmmo,
		eMax,
	};

	IItem(IItem::Type type, class Inventory* inventory = nullptr);
	virtual ~IItem();

	const Type& GetType() const;
	const std::string& GetName() const;

protected:
	Type m_Type;
	std::string m_Name;

	class Inventory* m_MyInventory;

private:

};

class EquipmentItem : public IItem
{
public:
	EquipmentItem(IItem::Type type, class Inventory* inventory = nullptr);
	virtual ~EquipmentItem();

	virtual bool Use() abstract;
	virtual bool IsFinishCoolTimeTimer() const;		// 쿨타임이 다 돌았나?
	virtual void CoolDown();						// 쿨다운 진행


protected:
	float m_UseCoolTimeTimer;						// 현재 남은 쿨타임

private:

};

