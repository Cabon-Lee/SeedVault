#pragma once
#include "IItem.h"

/// <summary>
/// Herb Ŭ����
/// �κ��丮�� ���縸 �ϸ� Ư���� �׼��� ����.
/// 
/// �� �ֻ���� ���� ���ȴ�.
/// 
/// �ۼ��� : Yoking
/// </summary>

class Herb : public IItem
{
public:
	Herb(IItem::Type type, class Inventory* inventory = nullptr);
	virtual ~Herb();

protected:

private:

};
