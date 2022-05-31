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
/// ���� ��밡���Ѱ� �Ǻ�
/// ��Ÿ���� �� ���ҳ��� �Ǵ�
/// </summary>
/// <returns>��� ���� ����</returns>
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
/// ��Ÿ�� Ÿ�̸� ����
/// </summary>
void EquipmentItem::CoolDown()
{
	m_UseCoolTimeTimer -= CL::Input::s_DeltaTime;

	//CA_TRACE("[EquipmentItem] CoolDown -> %f", m_RemainingCoolTime);
}
