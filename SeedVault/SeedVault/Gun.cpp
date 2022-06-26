#include "pch.h"
#include "Inventory.h"
#include "Health.h"
#include "PlayerController.h"
#include "MuzzleFlash.h"
#include "HitPoint.h"
#include "MuzzleFlash.h"
#include "Zombie_Runner_Move.h"
#include "Gun.h"

Gun::Gun(EquipmentItem::Type type, Inventory* inventory)
	: EquipmentItem(type, inventory)
	, m_CameraTf(nullptr)
	, m_MuzzleFlash(nullptr)
	, m_HitPoint(nullptr)
	, m_pAudio(nullptr)

	, m_Damage(0.0f)
	, m_ShootRange(0.0f)
	, m_ShootCoolTime(0.0f)
	, m_bIsReloading(false)
	, m_ReloadSpeed(0.0f)
	, m_CurrentAmmoCount(0)
	, m_MaxAmmoCount(0)
{
	m_MuzzleFlash = m_MyInventory->GetMyObject()->GetComponent<MuzzleFlash>();
	m_HitPoint = m_MyInventory->GetMyObject()->GetComponent<HitPoint>();
	m_pAudio = m_MyInventory->GetMyObject()->GetComponent<Audio>();

	m_bIsReloading = false;
	m_ReloadCoolTimer = 0.0f;
}

Gun::~Gun()
{
}

/// <summary>
/// �ѱ� ��� �Լ�
/// 
/// ��Ȳ�� �°� ������ or ��� �Ѵ�
/// </summary>
/// <returns></returns>
bool Gun::Use()
{
	// źâ�� ����ְ�
	if (IsMagazineEmpty() == true)
	{
		// ���������� �ƴѵ� 
		if (IsReloading() == false)
		{
			// ����ź���� ������
			if (IsAmmoEmpty() == false)
			{
				// ���� ����
				StartReload();
			}

			else
			{
				// ź�� �� ������...
				CA_TRACE("[Gun] �κ��丮�� ź�� �� ������...");
			}
		}

		// ������ ���̸�
		else
		{
			CA_TRACE("[Gun] Now Reloading...");
		}

		return false;
	}

	// źâ�� ���� ������
	else
	{
		// ���
		return  Shoot();
	}
}

bool Gun::Shoot()
{
	/// ��Ÿ���� �� �������� ���
	if (IsFinishCoolTimeTimer() == true)
	{
		/*
		* (��) ����ĳ��Ʈ ��� ���
		/// Raycast Test
		// ���� ��ġ(ī�޶�) ã��
		m_CameraTf = DLLEngine::FindGameObjectByName("MainCamera")->m_Transform;
		assert(m_CameraTf != nullptr);

		/// ī�޶��� ��ġ���� ī�޶��� �������� ���̸� ����.
		// ī�޶� �浹ó��(�ٿ�� ī�޶�)�� ������ ���ķ� ī�޶� ���� �浹���� ���¿��� ������ �ϸ� ���� �浹�� �Ǿ
		// ī�޶��� �����ǿ��� ���� ������ ���ܼ� Ray�� ���� ��ġ�� ��´�.
		Vector3 _attackPosition = m_CameraTf->m_WorldPosition + m_CameraTf->m_WorldTM.Forward() * 1.4f;
		Vector3 _attackDirection = m_CameraTf->m_WorldTM.Forward();

		/// �浹 �ǻ�ü Ȯ��
		RaycastHit _hit;
		_hit = *DLLEngine::CheckRaycastHit(_attackPosition, _attackDirection, m_ShootRange);

		// �浹ü�� ������
		if (_hit.actor != nullptr)
		{
			Health* _health = _hit.actor->GetMyObject()->GetComponent<Health>();
			if (_health != nullptr)
			{
				_health->Damage(m_Damage, _hit.position, _attackDirection, 1.0f, m_MyInventory->GetMyObject(), _hit.actor);
			}
		}
		*/

		/// �������� �̿��� ���۵� ��� ���
		GameObject* _Owner = m_MyInventory->GetMyObject();

		GameObject* _hitMeshObj = m_HitPoint->GetHitPoint();

		// �ӽ� �±� String ��
		if (_hitMeshObj != nullptr)
		{
			std::string _strTag = _hitMeshObj->GetTag();
			if (_strTag == "ZombieMesh")
			{
				Health* _targetHealth = _hitMeshObj->GetParent()->GetComponent<Health>();
				_targetHealth->Damage(
					m_Damage,
					SimpleMath::Vector3::Zero,
					_targetHealth->m_Transform->m_Position - _Owner->m_Transform->m_Position,
					0.0f,
					_Owner
				);
			}
		}
		// ź ����
		DecreaseCurrentAmmoCount(1);

		return true;
	}

	return false;
}

void Gun::StartReload()
{
	m_MyInventory->GetMyObject()->GetComponent<PlayerController>()->m_bIsReloading = true;
	m_ReloadCoolTimer = m_ReloadSpeed;

	CA_TRACE("[Gun] Reload Start !!");
}

/// <summary>
/// źâ�� ź�� ���� ����
/// 
/// �ִ� źâ ������ ���ų� 0���� ������ �� �����Ѵ�.
/// </summary>
/// <param name="ammo">������ ����</param>
/// <returns>���� ����</returns>
bool Gun::SetCurrentAmmoCount(const uint ammo)
{
	m_CurrentAmmoCount = ammo;

	if (m_CurrentAmmoCount > m_MaxAmmoCount)
	{
		m_CurrentAmmoCount = m_MaxAmmoCount;
	}

	if (m_CurrentAmmoCount < 0)
	{
		m_CurrentAmmoCount = 0;
	}

	return true;
}

/// <summary>
/// ���� źâ�� ���� ź �� ����
/// </summary>
/// <returns></returns>
uint Gun::GetCurrentAmmoCount() const
{
	return m_CurrentAmmoCount;
}

/// <summary>
/// �ѿ� ���� ź �� ����
/// </summary>
/// <param name="val">������ ����</param>
/// <returns>���� �� ����</returns>
uint Gun::IncreaseCurrentAmmoCount(const uint&& val)
{
	m_CurrentAmmoCount += val;

	if (m_CurrentAmmoCount > m_MaxAmmoCount)
	{
		m_CurrentAmmoCount = m_MaxAmmoCount;
	}

	return m_CurrentAmmoCount;
}

/// <summary>
/// �ѿ� ���� ź �� ����
/// </summary>
/// <param name="val">���ҽ�ų ����</param>
/// <returns>���� �� ����</returns>
uint Gun::DecreaseCurrentAmmoCount(const uint&& val)
{
	m_CurrentAmmoCount -= val;

	if (m_CurrentAmmoCount < 0)
	{
		m_CurrentAmmoCount = 0;
	}

	return m_CurrentAmmoCount;
}

const float& Gun::GetReloadSpeed() const
{
	return m_ReloadSpeed;
}

/// <summary>
/// ���� ���� �ð� ����
/// (���� ��ü�� ������ ����ϱ����� 0.0f���� ������ �� �� ���)
/// </summary>
/// <param name="time"></param>
void Gun::SetReloadCoolTimer(const float& time)
{
	m_ReloadCoolTimer = time;
}

/// <summary>
/// ���� ���ΰ� �Ǵ�
/// </summary>
/// <returns>���� �� ����</returns>
bool Gun::IsReloading() const
{
	// �����Ϸ���� �����ð��� ������
	if (m_ReloadCoolTimer > 0.0f)
	{
		return true;
	}

	else
	{
		return false;
	}
}

void Gun::ReloadCoolDown()
{
	m_ReloadCoolTimer -= CL::Input::s_DeltaTime;

	if (m_ReloadCoolTimer <= 0.0f)
	{
		// ������
		Reload();

		// ���� �ִϸ��̼� ����
		Animator* _animator = DLLEngine::FindGameObjectByName("Player_Mesh")->GetComponent<Animator>();
		assert(_animator != nullptr);
		_animator->SetOverrideAnimLayer("");
	}
}

/// <summary>
/// źâ�� ���� ������ ź�� ������� �˻�
/// </summary>
/// <returns>źâ�� ź ����</returns>
bool Gun::IsMagazineEmpty() const
{
	if (m_CurrentAmmoCount <= 0)
	{
		return true;
	}

	else
	{
		return false;
	}
}

/// <summary>
/// źâ�� �� ���ִ��� �˻�
/// </summary>
/// <returns>���</returns>
bool Gun::IsMagazineFull() const
{
	if (m_CurrentAmmoCount >= m_MaxAmmoCount)
	{
		return true;
	}

	else
	{
		return false;
	}
}

/// <summary>
/// �κ��丮�� ź�� �� ���������� �˻�
/// </summary>
/// <returns></returns>




LongGun::LongGun(IItem::Type type, Inventory* inventory)
	: Gun(type, inventory)
{
	m_Name = "LongGun";

	m_Damage = 2.0f;
	m_ShootRange = 100.0f;
	m_ShootCoolTime = 2.0f;
	m_ReloadSpeed = 2.0f;
	m_MaxAmmoCount = 4;
	m_CurrentAmmoCount = m_MaxAmmoCount;
}

LongGun::~LongGun()
{
}

/// <summary>
/// ����(1�� �ֹ���) ���� �Լ�
///
/// </summary>
/// <returns>���� ����</returns>
bool LongGun::Shoot()
{
	if (Gun::Shoot() == true)
	{
		CA_TRACE("[LongGun] Shoot - ź ���� <%d/%d>", m_CurrentAmmoCount, m_MyInventory->GetItemCount(IItem::Type::eLongGunAmmo));
		m_UseCoolTimeTimer = m_ShootCoolTime;	// ��Ÿ�� ����

		// �տ� �������� �������� �����÷��� ������Ʈ ��������
		MuzzleFlash* _MuzzleFlash = DLLEngine::FindGameObjectByName("MuzzleFlash")->GetComponent<MuzzleFlash>();
		if (_MuzzleFlash != nullptr)
		{
			_MuzzleFlash->Fire();
		}

		// �ݹ� ���� �̺�Ʈ
		auto* _soundEvent = m_pAudio->PlayEvent("event:/Player_Rifle"); 

		// Broadcast
		auto& zombies = DLLEngine::FindGameObjectByTag("Zombie");
		for (auto& zombie : zombies)
		{
			Enemy_Move* _enemyMove = nullptr;
			_enemyMove = zombie->GetComponent<Zombie_Runner_Move>();

			if (_enemyMove != nullptr)
			{
				_enemyMove->HearSound(m_MyInventory->GetMyObject(), _soundEvent);
			}
		}

		return true;
	}

	//CA_TRACE("[LongGun] Shoot CoolDown ~~~ %f", m_RemainingCoolTime);
	return false;
}

/// <summary>
/// �ֹ��� ����
/// �κ��丮�� ����ź���� �����ϴ� ��ŭ ���ҽ�Ű��
/// źâ�� ź ����
/// </summary>
/// <returns>���� �� źâ�� ź ��</returns>
uint LongGun::Reload()
{
	// �κ��丮�� �������� ź ��
	uint remainingAmmunitionCountInInventory = m_MyInventory->GetItemCount(IItem::Type::eLongGunAmmo);

	// źâ�� ����ִ� �κ� ī��Ʈ(������ �� �ִ� ����)
	uint _requiredCount = m_MaxAmmoCount - m_CurrentAmmoCount;

	/// ���� �Ұ���
	// ������ �� �ִ� ź ���� 0���� ū��
	if (_requiredCount > 0)
	{
		// ����ź���� 0�̸�
		if (remainingAmmunitionCountInInventory == 0)
		{
			// ����!
			CA_TRACE("[LongGun] Reload : �κ��丮�� ź�� �� ������...");

			return false;
		}

		/// ����
		else
		{
			// ���� ź >= ���� �䱸 ź
			if (remainingAmmunitionCountInInventory >= _requiredCount)
			{
				/// �κ��丮�� ź�� �� ����
				// ���� ź ��= ����ź �� - �䱸 ź ��
				m_MyInventory->SetItemCount(IItem::Type::eLongGunAmmo, remainingAmmunitionCountInInventory - _requiredCount);

				/// źâ�� ź�� �� ����
				// ���� ź �� = ���� ź �� + �䱸 ź ��
				IncreaseCurrentAmmoCount(std::move(_requiredCount));

			}

			// ���� ź < ���� �䱸 ź
			else
			{
				/// �κ��丮�� ź��� 0 ����
				// ���� ź = 0
				m_MyInventory->SetItemCount(IItem::Type::eLongGunAmmo, 0);

				/// źâ�� ź�� �� ����
				// ���� ź = ���� ź �� + ���� ź
				SetCurrentAmmoCount(m_CurrentAmmoCount + remainingAmmunitionCountInInventory);
			}

			CA_TRACE("[LongGun] Reload() : <%d/%d> ", m_CurrentAmmoCount, m_MyInventory->GetItemCount(IItem::Type::eLongGunAmmo));
		}
	}

	// ���� ���� ���� ����
	m_MyInventory->GetMyObject()->GetComponent<PlayerController>()->m_bIsReloading = false;

	// ���� �� źâ�� ź �� ����
	return m_CurrentAmmoCount;
}

/// <summary>
/// �κ��丮�� ź���� ����� �˻�
/// (����)
/// </summary>
/// <returns></returns>
bool LongGun::IsAmmoEmpty() const
{
	if (m_MyInventory->GetItemCount(IItem::Type::eLongGunAmmo) <= 0)
	{
		return true;
	}

	else
	{
		return false;
	}
}


HandGun::HandGun(IItem::Type type, Inventory* inventory)
	: Gun(type, inventory)
{
	m_Name = "HandGun";

	m_Damage = 1.0f;
	m_ShootRange = 100.0f;
	m_ShootCoolTime = 1.0f;
	m_ReloadSpeed = 1.0f;
	m_MaxAmmoCount = 8;
	m_CurrentAmmoCount = m_MaxAmmoCount;
}

HandGun::~HandGun()
{
}

/// <summary>
/// ����(2�� ��������) ���� �Լ�
/// </summary>
/// <returns>���� ����</returns>
bool HandGun::Shoot()
{
	if (Gun::Shoot() == true)
	{
		CA_TRACE("[HandGun] Shoot - ź ���� <%d/%d>", m_CurrentAmmoCount, m_MyInventory->GetItemCount(IItem::Type::eHandGunAmmo));
		m_UseCoolTimeTimer = m_ShootCoolTime;	// ��Ÿ�� ����

		// �ݹ� ���� �̺�Ʈ
		auto* _soundEvent = m_pAudio->PlayEvent("event:/Player_Pistol");
		float dist = m_pAudio->GetEventDistance("event:/Player_Pistol");

		return true;
	}

	//CA_TRACE("[HandGun] Shoot CoolDown ~~~ %f", m_RemainingCoolTime);
	return false;
}

uint HandGun::Reload()
{
	// �κ��丮�� �������� ź ��
	uint remainingAmmunitionCountInInventory = m_MyInventory->GetItemCount(IItem::Type::eHandGunAmmo);

	// źâ�� ����ִ� �κ� ī��Ʈ(������ �� �ִ� ����)
	uint _requiredCount = m_MaxAmmoCount - m_CurrentAmmoCount;

	/// ���� �Ұ���
	// ������ �� �ִ� ź ���� 0���� ū��
	if (_requiredCount > 0)
	{
		// ����ź���� 0�̸�
		if (remainingAmmunitionCountInInventory == 0)
		{
			// ����!
			CA_TRACE("[HandGun] Reload : �κ��丮�� ź�� �� ������...");

			return false;
		}

		/// ����
		else
		{
			// ���� ź >= ���� �䱸 ź
			if (remainingAmmunitionCountInInventory >= _requiredCount)
			{
				/// �κ��丮�� ź�� �� ����
				// ���� ź ��= ����ź �� - �䱸 ź ��
				m_MyInventory->SetItemCount(IItem::Type::eHandGunAmmo, remainingAmmunitionCountInInventory - _requiredCount);

				/// źâ�� ź�� �� ����
				// ���� ź �� = ���� ź �� + �䱸 ź ��
				IncreaseCurrentAmmoCount(std::move(_requiredCount));

			}

			// ���� ź < ���� �䱸 ź
			else
			{
				/// �κ��丮�� ź��� 0 ����
				// ���� ź = 0
				m_MyInventory->SetItemCount(IItem::Type::eHandGunAmmo, 0);

				/// źâ�� ź�� �� ����
				// ���� ź = ���� ź �� + ���� ź
				SetCurrentAmmoCount(m_CurrentAmmoCount + remainingAmmunitionCountInInventory);
			}

			CA_TRACE("[HandGun] Reload() : <%d/%d> ", m_CurrentAmmoCount, m_MyInventory->GetItemCount(IItem::Type::eHandGunAmmo));
		}
	}

	// ���� ���� ����
	m_MyInventory->GetMyObject()->GetComponent<PlayerController>()->m_bIsReloading = false;

	// ���� �� źâ�� ź �� ����
	return m_CurrentAmmoCount;
}

/// <summary>
/// �κ��丮�� ź���� ����� �˻�
/// (����)
/// </summary>
/// <returns></returns>
bool HandGun::IsAmmoEmpty() const
{
	if (m_MyInventory->GetItemCount(IItem::Type::eHandGunAmmo) <= 0)
	{
		return true;
	}

	else
	{
		return false;
	}
}
