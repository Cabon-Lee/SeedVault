#pragma once
/// <summary>
/// ĳ����(�÷��̾�)�� ���� �κ��丮 Ŭ����
/// ���� ������ �ִ� �������� �� �� �ִ�.
/// 
/// ó�� ���� �� ���� �����۵��� �����ؼ� �߰��س��� ���Ŀ� ���� ���� ������ ī�����Ѵ�
/// 
/// �ۼ��� : YoKing
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
	std::vector<std::pair<std::shared_ptr<class IItem>, int>> m_Inventory;	// [������, ����]
	std::vector<std::shared_ptr<class EquipmentItem>> m_Slot;				// ����
	
	uint m_EquiptedSlotIndex;												// ���� ����� ���� idx
	std::shared_ptr<class EquipmentItem> m_EquipedItem;						// ���� ����� ������
};

