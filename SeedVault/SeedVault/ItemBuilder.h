#pragma once

/// <summary>
/// 아이템을 생성해주는 빌더(팩토리) 클래스
/// 
/// 작성자 : 최 요 환
/// </summary>

class ItemBuilder
{
public:
	ItemBuilder();
	~ItemBuilder();

	static std::shared_ptr<class IItem> CreateItem(IItem::Type type, class Inventory* inventory = nullptr);
};

