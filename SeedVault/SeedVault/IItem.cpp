#include "pch.h"
#include "IItem.h"

IItem::IItem(IItem::Type type, Inventory* inventory)
	: m_Type(type)
	, m_MyInventory(inventory)
	, m_Name()
{
}

IItem::~IItem()
{
}

const IItem::Type& IItem::GetType() const
{
	return m_Type;
}

const std::string& IItem::GetName() const
{
	return m_Name;
}

EquipmentItem::EquipmentItem(IItem::Type type, Inventory* inventory)
	: IItem(type, inventory)
	, m_UseCoolTimeTimer(0.0f)
{
}

EquipmentItem::~EquipmentItem()
{
}

/// <summary>
/// 현재 사용가능한가 판별
/// 쿨타임이 다 돌았나로 판단
/// </summary>
/// <returns>사용 가능 여부</returns>
bool EquipmentItem::IsFinishCoolTimeTimer() const
{
	if (m_UseCoolTimeTimer <= 0.0f)
	{
		return true;
	}

	else
	{
		return false;
	}
}

/// <summary>
/// 쿨타임 타이머 진행
/// </summary>
void EquipmentItem::CoolDown()
{
	m_UseCoolTimeTimer -= CL::Input::s_DeltaTime;

	//CA_TRACE("[EquipmentItem] CoolDown -> %f", m_RemainingCoolTime);
}
