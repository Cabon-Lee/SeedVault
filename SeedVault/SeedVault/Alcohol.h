#pragma once

#include "IItem.h"

/// <summary>
/// Alcohol Ŭ����
/// �κ��丮�� ���縸 �ϸ� Ư���� �׼��� ����.
/// 
/// ȭ�� �ö�ũ�� ���� ���ȴ�
/// 
/// �ۼ��� : �� �� ȯ
/// </summary>

class Alcohol : public IItem
{
public:
	Alcohol(IItem::Type type, class Inventory* inventory = nullptr);
	virtual ~Alcohol();

protected:

private:
};