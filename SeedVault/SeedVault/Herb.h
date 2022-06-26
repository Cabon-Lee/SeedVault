#pragma once
#include "IItem.h"

/// <summary>
/// Herb 클래스
/// 인벤토리에 존재만 하며 특별한 액션은 없다.
/// 
/// 힐 주사기의 재료로 사용된다.
/// 
/// 작성자 : 최 요 환
/// </summary>

class Herb : public IItem
{
public:
	Herb(IItem::Type type, class Inventory* inventory = nullptr);
	virtual ~Herb();

protected:

private:

};
