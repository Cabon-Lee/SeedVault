#include "pch.h"
#include "IItem.h"
#include "Gun.h"
#include "Flask.h"
#include "ItemBuilder.h"
#include "Inventory.h"

Inventory::Inventory()
	: ComponentBase(ComponentType::GameLogic)
	, _Owner(nullptr)
	, m_Inventory()
	, m_Slot()
	, m_EquiptedSlotIndex(0)
{
	
}

Inventory::~Inventory()
{
}

void Inventory::Start()
{
	_Owner = m_pMyObject;

	m_Inventory.reserve(static_cast<uint>(IItem::Type::eMax));
	m_Slot.reserve(5);

	/// 인벤토리 아이템 추가
	{
		// 장비 먼저 추가
		m_Inventory.emplace_back(std::make_pair(ItemBuilder::CreateItem(IItem::Type::eLongGun, this), 1));
		m_Inventory.emplace_back(std::make_pair(ItemBuilder::CreateItem(IItem::Type::eHandGun, this), 1));

		// 플라스크 추가
		m_Inventory.emplace_back(std::make_pair(ItemBuilder::CreateItem(IItem::Type::eFlask, this), 3));
		m_Inventory.emplace_back(std::make_pair(ItemBuilder::CreateItem(IItem::Type::eFireFlask, this), 2));

		// 힐 주사기 추가
		m_Inventory.emplace_back(std::make_pair(ItemBuilder::CreateItem(IItem::Type::eHealSyringe, this), 0));

		// 탄 추가
		m_Inventory.emplace_back(std::make_pair(ItemBuilder::CreateItem(IItem::Type::eLongGunAmmo, this), 100));
		m_Inventory.emplace_back(std::make_pair(ItemBuilder::CreateItem(IItem::Type::eHandGunAmmo, this), 100));

		// 알코올, 허브 추가
		m_Inventory.emplace_back(std::make_pair(ItemBuilder::CreateItem(IItem::Type::eAlcohol, this), 2));
		m_Inventory.emplace_back(std::make_pair(ItemBuilder::CreateItem(IItem::Type::eHerb, this), 3));
	}

	/// 슬롯에 장비 추가
	{
		// 주무기, 보조무기 슬롯에 추가
		m_Slot.emplace_back(std::dynamic_pointer_cast<EquipmentItem>(FindItem(IItem::Type::eLongGun)));
		m_Slot.emplace_back(std::dynamic_pointer_cast<EquipmentItem>(FindItem(IItem::Type::eHandGun)));

		// 플라스크, 화염 플라스크 슬롯에 추가
		m_Slot.emplace_back(std::dynamic_pointer_cast<EquipmentItem>(FindItem(IItem::Type::eFlask)));
		m_Slot.emplace_back(std::dynamic_pointer_cast<EquipmentItem>(FindItem(IItem::Type::eFireFlask)));

		// 힐 주사기 슬롯에 추가
		m_Slot.emplace_back(std::dynamic_pointer_cast<EquipmentItem>(FindItem(IItem::Type::eHealSyringe)));
	}

	// 장총 장비
	m_EquiptedSlotIndex = 0;
	m_EquipedItem = m_Slot.at(m_EquiptedSlotIndex);
}

void Inventory::Update(float dTime)
{
	std::shared_ptr<EquipmentItem> _currentEquipment = GetCurrentEquipment();
	assert(_currentEquipment != nullptr);

	// 현재 슬롯들의 장비가 사용불가 상태면 쿨타임을 돌린다.
	for (const auto& equipment : m_Slot)
	{
		if (equipment->IsFinishCoolTimeTimer() == false)
		{
			equipment->CoolDown();
		}

		// 총이라면 장전 타이머도 돌린다.
		auto _gun = std::dynamic_pointer_cast<Gun>(equipment);
		if (_gun != nullptr)
		{
			if (_gun->IsReloading() == true)
			{
				_gun->ReloadCoolDown();
			}
		}
	}

	if (DLLInput::InputKeyDown(static_cast<int>(CL::KeyCode::KEY_I)))
	{
		PrintInfo();
	}
}

void Inventory::OnRender()
{
}

/// <summary>
/// 인벤토리에서 type에 해당하는 아이템을 찾아서 리턴
/// </summary>
/// <param name="type">찾을 아이템의 type</param>
/// <returns>아이템</returns>
std::shared_ptr<class IItem> Inventory::FindItem(IItem::Type type) const
{
	for (const auto& item : m_Inventory)
	{
		if (item.first->GetType() == type)
		{
			return item.first;
		}
	}

	return nullptr;
}

/// <summary>
/// type에 해당하는 아이템을 찾아서 
/// 인벤토리에 남아있는 갯수를 리턴
/// </summary>
/// <param name="type">찾을 아이템의 타입</param>
/// <returns>보유 갯수</returns>
uint Inventory::GetItemCount(IItem::Type type) const
{
	auto _iter = m_Inventory.begin();

	for (; _iter != m_Inventory.end(); ++_iter)
	{
		if ((*_iter).first->GetType() == type)
		{
			return (*_iter).second;
		}
	}

	return 0;
}

/// <summary>
/// type에 해당하는 아이템을 찾아서
/// 갯수를 설정
/// </summary>
/// <param name="type">갯수 설정할 아이템 타입</param>
/// <param name="count">설정할 갯수</param>
void Inventory::SetItemCount(const IItem::Type type, uint&& count)
{
	auto _iter = m_Inventory.begin();

	for (; _iter != m_Inventory.end(); ++_iter)
	{
		if ((*_iter).first->GetType() == type)
		{
			(*_iter).second = count;

			if ((*_iter).second < 0)
			{
				(*_iter).second = 0;
			}
		}
	}
}

/// <summary>
/// type 에 해당하는 이이템을 찾아서 
/// val 만큼 갯수를 증가
/// </summary>
/// <param name="type">증가시킬 아이템 타입</param>
/// <param name="val">증가시킬 양</param>
/// <returns>증가 후 갯수</returns>
uint Inventory::IncreaseItemCount(const IItem::Type type, const uint&& val)
{
	auto _iter = m_Inventory.begin();

	for (; _iter != m_Inventory.end(); ++_iter)
	{
		if ((*_iter).first->GetType() == type)
		{
			(*_iter).second += val;

			break;
		}
	}

	return (*_iter).second;
}

/// <summary>
/// type 에 해당하는 이이템을 찾아서 
/// val 만큼 갯수를 감소
/// </summary>
/// <param name="type">감소시킬 아이템 타입</param>
/// <param name="val">감소시킬 양</param>
/// <returns>감소 후 갯수</returns>
uint Inventory::DecreaseItemCount(const IItem::Type type, const uint&& val)
{
	auto _iter = m_Inventory.begin();

	for (; _iter != m_Inventory.end(); ++_iter)
	{
		if ((*_iter).first->GetType() == type)
		{
			(*_iter).second -= val;

			break;
		}
	}

	return (*_iter).second;
}

/// <summary>
/// 현재 장비중인 장비 아이템 리턴
/// </summary>
/// <returns></returns>
std::shared_ptr<class EquipmentItem> Inventory::GetCurrentEquipment() const
{
	return m_Slot.at(m_EquiptedSlotIndex);
}

/// <summary>
/// 장비 변경
/// 장비하고 싶은 번호를 인자로 받아서
/// 해당 슬롯의 장비로 현재장비를 교체
/// </summary>
/// <param name="slotNum">장비할 슬롯 넘버</param>
/// <returns>장비 교체 성공 여부</returns>
bool Inventory::ChangeWeapon(const uint& slotNum)
{
	if (slotNum == m_EquiptedSlotIndex)
	{
		CA_TRACE("[Inventory] ChangeWeapon 실패 -> (현재 장비한 아이템임)");
		return false;
	}

	// 장비 교체할 때 총인지 확인해서 장전중이면
	// 장전을 취소한다.
	std::shared_ptr<Gun> _gun = std::dynamic_pointer_cast<Gun>(m_EquipedItem);
	if (_gun != nullptr)
	{
		if (_gun->IsReloading() == true)
		{
			_gun->SetReloadCoolTimer(0.0f);
			CA_TRACE("[Inventory] ChangeWeapon() 장전 취소");
		}
	}

	m_EquiptedSlotIndex = slotNum;
	m_EquipedItem = m_Slot.at(m_EquiptedSlotIndex);

	CA_TRACE("[Inventory] ChangeWeapon -> %d, %s", m_EquiptedSlotIndex + 1, m_EquipedItem->GetName().c_str());

	return true;
}

void Inventory::PrintInfo() const
{
	CA_TRACE("[Inventory] PrintInfo -------------");

	for (size_t i = 0; i < m_Inventory.size(); i++)
	{
		CA_TRACE("[%d] %s - %d", i + 1, m_Inventory.at(i).first->GetName().c_str(), m_Inventory.at(i).second);
	}
	CA_TRACE("-----------------------------------");
}
