#pragma once
#include "IItem.h"

/// <summary>
/// 플라스크 아이템 클래스
/// 투척했을 때 실제로 날아가는 (오브젝트에 붙는 컴포넌트)것이 아닌
/// 인벤토리내의 정보로써만 존재한다.
/// 
/// 공격(마우스좌클릭)으로 투척이 가능하고
/// 조합을 하면 새로운 아이템을 만들 수 있다.
/// 
/// 플라스크 + 알코올 = 화염 플라스크
/// 플라스크 + 허브   = 힐 주사기
/// 
/// 작성자 : 최 요 환
/// </summary>

class Flask : public EquipmentItem
{
public:
	Flask(IItem::Type type, class Inventory* inventory = nullptr);
	virtual ~Flask();

	virtual bool Use() override;
	virtual void CoolDown() override;
	virtual bool Throw();

	bool IsThrowing() const;

protected:
	float m_ThrowingSpeed;	// 투척 간격 속도

private:

};

/// <summary>
/// 화염 플라스크 클래스
/// 
/// 투척하면 화염플라스크를 생성해서 던진다
/// 
/// 작성자 : 최 요 환
/// </summary>
class FireFlask : public Flask
{
public:
	FireFlask(IItem::Type type, class Inventory* inventory = nullptr);
	virtual ~FireFlask();

	virtual bool Use() override;
	virtual void CoolDown() override;
	virtual bool Throw() override;

protected:

private:

};



