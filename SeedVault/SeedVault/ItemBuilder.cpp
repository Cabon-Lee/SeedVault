#include "pch.h"
#include "Inventory.h"
#include "Gun.h"
#include "Ammo.h"
#include "Flask.h"
#include "Alcohol.h"
#include "Herb.h"
#include "HealSyringe.h"
#include "ItemBuilder.h"

ItemBuilder::ItemBuilder()
{
}

ItemBuilder::~ItemBuilder()
{
}

std::shared_ptr<IItem> ItemBuilder::CreateItem(IItem::Type type, Inventory* inventory)
{
	std::shared_ptr<IItem> _item = nullptr;

	switch (type)
	{
	case IItem::Type::eLongGun:
		_item = std::make_shared<LongGun>(IItem::Type::eLongGun, inventory);

		break;

	case IItem::Type::eHandGun:
		_item = std::make_shared<HandGun>(IItem::Type::eHandGun, inventory);
		break;

	case IItem::Type::eFlask:
		_item = std::make_shared<Flask>(IItem::Type::eFlask, inventory);
		break;

	case IItem::Type::eFireFlask:
		_item = std::make_shared<FireFlask>(IItem::Type::eFireFlask, inventory);
		break;

	case IItem::Type::eAlcohol:
		_item = std::make_shared<Alcohol>(IItem::Type::eAlcohol, inventory);
		break;

	case IItem::Type::eHerb:
		_item = std::make_shared<Herb>(IItem::Type::eHerb, inventory);
		break;

	case IItem::Type::eHealSyringe:
		_item = std::make_shared<HealSyringe>(IItem::Type::eHealSyringe, inventory);
		break;

	case IItem::Type::eLongGunAmmo:
		_item = std::make_shared<LongGunAmmo>(IItem::Type::eLongGunAmmo, inventory, 3.0f);
		break;

	case IItem::Type::eHandGunAmmo:
		_item = std::make_shared<HandGunAmmo>(IItem::Type::eHandGunAmmo, inventory, 1.5f);
		break;

	default:
		break;
	}

	return _item;
}

