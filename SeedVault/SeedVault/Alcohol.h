#pragma once

#include "IItem.h"

/// <summary>
/// Alcohol 클래스
/// 인벤토리에 존재만 하며 특별한 액션은 없다.
/// 
/// 화염 플라스크의 재료로 사용된다
/// 
/// 작성자 : 최 요 환
/// </summary>

class Alcohol : public IItem
{
public:
	Alcohol(IItem::Type type, class Inventory* inventory = nullptr);
	virtual ~Alcohol();

protected:

private:
};