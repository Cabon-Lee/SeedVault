#include "pch.h"
#include "Inventory.h"
#include "PlayerController.h"
#include "Health.h"
#include "HealSyringe.h"



HealSyringe::HealSyringe(IItem::Type type, Inventory* inventory)
    : EquipmentItem(type, inventory)
    , m_UseCoolTime(0.0f)
    , m_HealPower(0)
{
    m_Name = "HealSyringe";
    m_UseCoolTime = 3.0f;
    m_HealPower = 5.0f;
}

HealSyringe::~HealSyringe()
{
}

bool HealSyringe::Use()
{
    if (IsFinishCoolTimeTimer() == true)
    {

    }
    return false;
}

/// <summary>
/// 힐 주사기 쿨다운
/// 쿨이 다 되면 사용(주사)하고 재사용 가능하게 된다.
/// 사용 후 인벤토리에 힐 주사기가 남아있지 않으면 주무기로 교체한다
/// </summary>
void HealSyringe::CoolDown()
{
    EquipmentItem::CoolDown();

    // 쿨타임이 다 돌았으면
    // 주사 모션(애니메이션)동작이 끝났으면 힐 적용
    if (IsHealing() == false)
    {
        this->Heal();

        PlayerController::s_bIsThrowing = false;


        // 힐 주사기 사용 후 인벤토리의 HealSyringe 갯수가 0이라면 바로 주무기로 교체한다
        PlayerController* _PlayerController = m_MyInventory->GetMyObject()->GetComponent<PlayerController>();
        assert(_PlayerController != nullptr);

        _PlayerController->EquipMainSlot();

    }
}

/// <summary>
/// 사용이 완료디고 힐이 적용하는 함수
/// 인벤토리의 주인 오브젝트의 힐을 수행한다.
/// 
/// 주인 오브젝트에 Health 컴포넌트 필요
/// </summary>
void HealSyringe::Heal()
{
    GameObject* _Owner = m_MyInventory->GetMyObject();
    Health* _Health = _Owner->GetComponent<Health>();
    assert(_Health != nullptr);

    _Health->IncreaseHp(m_HealPower);

    // 플레이어 힐 애니메이션 중지
    PlayerController::s_bIsHealing = false;
}

/// <summary>
/// 지금 힐링 중인가?
/// 힐 시퀀스 시작후 애니메이션 동작하는 구간
/// </summary>
/// <returns></returns>
bool HealSyringe::IsHealing() const
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
