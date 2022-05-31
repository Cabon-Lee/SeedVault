#include "pch.h"
#include "Alcohol.h"

Alcohol::Alcohol(IItem::Type type, Inventory* inventory)
	: IItem(type, inventory)
{
	m_Name = "Alcohol";
}

Alcohol::~Alcohol()
{
}