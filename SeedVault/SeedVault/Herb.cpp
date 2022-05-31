#include "pch.h"
#include "Herb.h"

Herb::Herb(IItem::Type type, Inventory* inventory)
	: IItem(type, inventory)
{
	m_Name = "Herb";
}

Herb::~Herb()
{
}
