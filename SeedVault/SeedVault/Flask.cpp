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
/// �ö�ũ ��� �Լ�
/// ��ô ����
/// </summary>
/// <returns></returns>
bool Flask::Use()
{
	// ��Ÿ�� ����
	// ������ ��� �����ؾ� ��.
	if (IsThrowing() == false && m_MyInventory->GetItemCount(IItem::Type::eFlask) > 0)
	{
		m_UseCoolTimeTimer = m_ThrowingSpeed;
		m_MyInventory->GetMyObject()->GetComponent<PlayerController>()->m_bIsThrowing = true;

		CA_TRACE("[Flask] ��ô ����");

		return true;
	}

	else
	{
		CA_TRACE("[Flask] ��ô �Ұ�");
		return false;
	}
}

/// <summary>
/// �ö�ũ ��ٿ�
/// ���� �� �Ǹ� ������ �� ��ô �����ϰ� �ȴ�.
/// ��ô �� �κ��丮�� �ö�ũ�� �������� �ʴٸ� �ֹ���� ��ü�Ѵ�.
/// </summary>
void Flask::CoolDown()
{
	EquipmentItem::CoolDown();

	// ��Ÿ���� �� ��������
	// ������ ���(�ִϸ��̼�)������ �������� ������ ��ô �ö�ũ ����
	if (IsThrowing() == false)
	{
		this->Throw();

		m_MyInventory->GetMyObject()->GetComponent<PlayerController>()->m_bIsThrowing = false;


		// ��ô �� �κ��丮�� Flask ������ 0�̶�� �ٷ� �ֹ���� ��ü�Ѵ�

	}
}

/// <summary>
/// �ö�ũ ��ô �Լ�
/// �κ��丮���� �ö�ũ ������ �ϳ� ���̰�
/// ������ ��ô�� Throwing Flask ������Ʈ�� ���� ������ �Ѵ�.
/// </summary>
/// <returns></returns>
bool Flask::Throw()
{
	m_MyInventory->DecreaseItemCount(IItem::Type::eFlask, 1);

	CA_TRACE("[Flask] Throw() -> ���� �ö�ũ %d",m_MyInventory->GetItemCount(IItem::Type::eFlask));
	
	// TODO : ������ ���� ������Ʈ ���� �ڵ� �ʿ�

	return true;
}

/// <summary>
/// ���� ��ô ���ΰ�?
/// ��ô ������ ������ �ִϸ��̼�(�ȷ� ������) �����ϴ� ����
/// </summary>
/// <returns>�Ǻ� ���</returns>
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
	// ��Ÿ�� ����
	// ������ ��� �����ؾ� ��.
	if (IsThrowing() == false && m_MyInventory->GetItemCount(IItem::Type::eFlask) > 0)
	{
		m_UseCoolTimeTimer = m_ThrowingSpeed;
		m_MyInventory->GetMyObject()->GetComponent<PlayerController>()->m_bIsThrowing = true;

		CA_TRACE("[FireFlask] ��ô ����");

		return true;
	}

	else
	{
		CA_TRACE("[FireFlask] ��ô �Ұ�");
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

	CA_TRACE("[FireFlask] Throw() -> ���� �ö�ũ %d", m_MyInventory->GetItemCount(IItem::Type::eFireFlask));

	// TODO : ������ ���� ������Ʈ ���� �ڵ� �ʿ�
	return true;
}



