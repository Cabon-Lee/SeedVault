#pragma once
/// <summary>
/// 캐릭터(플레이어)가 가질 인벤토리 클래스
/// 현재 가지고 있는 아이템을 알 수 있다.
/// 
/// 처음 시작 시 시작 아이템들을 생성해서 추가해놓고 이후에 얻을 때는 갯수를 카운팅한다
/// 
/// 작성자 : YoKing
/// </summary>

#include "IItem.h"
#include "ComponentBase.h"
class Inventory : public ComponentBase
{
public:
	enum class Slot
	{
		eLongGun,
		eHandGun,
		eFlask,
		eFireFlask,
		eHealSyringe,
	};

public:
	Inventory();
	virtual ~Inventory();

	// ComponentBase Override
	virtual void Start() override;
	virtual void Update(float dTime) override;
	virtual void OnRender() override;

	std::shared_ptr<class IItem> FindItem(IItem::Type type) const;
	uint GetItemCount(const IItem::Type type) const;
	void SetItemCount(const IItem::Type type, uint&& count);
	uint IncreaseItemCount(const IItem::Type type, const uint&& val);
	uint DecreaseItemCount(const IItem::Type type, const uint&& val);

	std::shared_ptr<class EquipmentItem> GetCurrentEquipment() const;

	bool ChangeWeapon(const uint& slotNum);

	void PrintInfo() const;

public:
	std::vector<std::pair<std::shared_ptr<class IItem>, int>> m_Inventory;	// [아이템, 갯수]
	std::vector<std::shared_ptr<class EquipmentItem>> m_Slot;				// 슬롯
	
	uint m_EquiptedSlotIndex;												// 현재 장비한 슬롯 idx
	std::shared_ptr<class EquipmentItem> m_EquipedItem;						// 현재 장비한 아이템
};

