#include "pch.h"
#include "Inventory.h"
#include "PlayerController.h"
#include "Flask.h"

Flask::Flask(IItem::Type type, Inventory* inventory)
	: EquipmentItem(type, inventory)
	, m_ThrowingSpeed(0.0f)
{
	m_Name = "Flask";
	m_ThrowingSpeed = 1.0f;
}

Flask::~Flask()
{
}

/// <summary>
/// 플라스크 사용 함수
/// 투척 실행
/// </summary>
/// <returns></returns>
bool Flask::Use()
{
	// 쿨타임 적용
	// 던지는 모션 시작해야 함.
	if (IsThrowing() == false && m_MyInventory->GetItemCount(IItem::Type::eFlask) > 0)
	{
		m_UseCoolTimeTimer = m_ThrowingSpeed;
		m_MyInventory->GetMyObject()->GetComponent<PlayerController>()->m_bIsThrowing = true;

		CA_TRACE("[Flask] 투척 시작");

		return true;
	}

	else
	{
		CA_TRACE("[Flask] 투척 불가");
		return false;
	}
}

/// <summary>
/// 플라스크 쿨다운
/// 쿨이 다 되면 던지고 재 투척 가능하게 된다.
/// 투척 후 인벤토리에 플라스크가 남아있지 않다면 주무기로 교체한다.
/// </summary>
void Flask::CoolDown()
{
	EquipmentItem::CoolDown();

	// 쿨타임이 다 돌았으면
	// 던지는 모션(애니메이션)동작이 끝났으면 실제로 투척 플라스크 생성
	if (IsThrowing() == false)
	{
		this->Throw();

		m_MyInventory->GetMyObject()->GetComponent<PlayerController>()->m_bIsThrowing = false;


		// 투척 후 인벤토리의 Flask 갯수가 0이라면 바로 주무기로 교체한다

	}
}

/// <summary>
/// 플라스크 투척 함수
/// 인벤토리에서 플라스크 갯수를 하나 줄이고
/// 실제로 투척된 Throwing Flask 오브젝트를 만들어서 날려야 한다.
/// </summary>
/// <returns></returns>
bool Flask::Throw()
{
	m_MyInventory->DecreaseItemCount(IItem::Type::eFlask, 1);

	CA_TRACE("[Flask] Throw() -> 남은 플라스크 %d",m_MyInventory->GetItemCount(IItem::Type::eFlask));
	
	// TODO : 실제로 던질 오브젝트 생성 코드 필요

	return true;
}

/// <summary>
/// 지금 투척 중인가?
/// 투척 시퀀스 시작후 애니메이션(팔로 던지는) 동작하는 구간
/// </summary>
/// <returns>판별 결과</returns>
bool Flask::IsThrowing() const
{
	if (m_UseCoolTimeTimer > 0.0f)
	{
		return true;
	}

	else
	{
		return false;
	}
}

FireFlask::FireFlask(IItem::Type type, Inventory* inventory)
	: Flask(type, inventory)
{
	m_Name = "FireFlask";
}

FireFlask::~FireFlask()
{
}

bool FireFlask::Use()
{
	// 쿨타임 적용
	// 던지는 모션 시작해야 함.
	if (IsThrowing() == false && m_MyInventory->GetItemCount(IItem::Type::eFlask) > 0)
	{
		m_UseCoolTimeTimer = m_ThrowingSpeed;
		m_MyInventory->GetMyObject()->GetComponent<PlayerController>()->m_bIsThrowing = true;

		CA_TRACE("[FireFlask] 투척 시작");

		return true;
	}

	else
	{
		CA_TRACE("[FireFlask] 투척 불가");
		return false;
	}
}

void FireFlask::CoolDown()
{
	EquipmentItem::CoolDown();

	if (m_UseCoolTimeTimer <= 0.0f)
	{
		this->Throw();

		m_MyInventory->GetMyObject()->GetComponent<PlayerController>()->m_bIsThrowing = false;
	}
}

bool FireFlask::Throw()
{
	m_MyInventory->DecreaseItemCount(IItem::Type::eFireFlask, 1);

	CA_TRACE("[FireFlask] Throw() -> 남은 플라스크 %d", m_MyInventory->GetItemCount(IItem::Type::eFireFlask));

	// TODO : 실제로 던질 오브젝트 생성 코드 필요
	return true;
}



