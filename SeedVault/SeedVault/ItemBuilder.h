#pragma once

/// <summary>
/// �������� �������ִ� ����(���丮) Ŭ����
/// 
/// �ۼ��� : �� �� ȯ
/// </summary>

class ItemBuilder
{
public:
	ItemBuilder();
	~ItemBuilder();

	static std::shared_ptr<class IItem> CreateItem(IItem::Type type, class Inventory* inventory = nullptr);
};

