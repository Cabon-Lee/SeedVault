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
/// �� �ֻ�� ��ٿ�
/// ���� �� �Ǹ� ���(�ֻ�)�ϰ� ���� �����ϰ� �ȴ�.
/// ��� �� �κ��丮�� �� �ֻ�Ⱑ �������� ������ �ֹ���� ��ü�Ѵ�
/// </summary>
void HealSyringe::CoolDown()
{
    EquipmentItem::CoolDown();

    // ��Ÿ���� �� ��������
    // �ֻ� ���(�ִϸ��̼�)������ �������� �� ����
    if (IsHealing() == false)
    {
        this->Heal();

        PlayerController::s_bIsThrowing = false;


        // �� �ֻ�� ��� �� �κ��丮�� HealSyringe ������ 0�̶�� �ٷ� �ֹ���� ��ü�Ѵ�
        PlayerController* _PlayerController = m_MyInventory->GetMyObject()->GetComponent<PlayerController>();
        assert(_PlayerController != nullptr);

        _PlayerController->EquipMainSlot();

    }
}

/// <summary>
/// ����� �Ϸ��� ���� �����ϴ� �Լ�
/// �κ��丮�� ���� ������Ʈ�� ���� �����Ѵ�.
/// 
/// ���� ������Ʈ�� Health ������Ʈ �ʿ�
/// </summary>
void HealSyringe::Heal()
{
    GameObject* _Owner = m_MyInventory->GetMyObject();
    Health* _Health = _Owner->GetComponent<Health>();
    assert(_Health != nullptr);

    _Health->IncreaseHp(m_HealPower);

    // �÷��̾� �� �ִϸ��̼� ����
    PlayerController::s_bIsHealing = false;
}

/// <summary>
/// ���� ���� ���ΰ�?
/// �� ������ ������ �ִϸ��̼� �����ϴ� ����
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
