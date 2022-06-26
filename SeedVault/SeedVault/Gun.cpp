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
/// 총기 사용 함수
/// 
/// 상황에 맞게 재장전 or 사격 한다
/// </summary>
/// <returns></returns>
bool Gun::Use()
{
	// 탄창이 비어있고
	if (IsMagazineEmpty() == true)
	{
		// 재장전중이 아닌데 
		if (IsReloading() == false)
		{
			// 보유탄약이 있으면
			if (IsAmmoEmpty() == false)
			{
				// 장전 시작
				StartReload();
			}

			else
			{
				// 탄약 다 떨어짐...
				CA_TRACE("[Gun] 인벤토리에 탄약 다 떨어짐...");
			}
		}

		// 재장전 중이면
		else
		{
			CA_TRACE("[Gun] Now Reloading...");
		}

		return false;
	}

	// 탄창이 남아 있으면
	else
	{
		// 사격
		return  Shoot();
	}
}

bool Gun::Shoot()
{
	/// 쿨타임이 다 돌았으면 사격
	if (IsFinishCoolTimeTimer() == true)
	{
		/*
		* (구) 레이캐스트 사격 방식
		/// Raycast Test
		// 공격 위치(카메라) 찾기
		m_CameraTf = DLLEngine::FindGameObjectByName("MainCamera")->m_Transform;
		assert(m_CameraTf != nullptr);

		/// 카메라의 위치에서 카메라의 방향으로 레이를 쏘자.
		// 카메라 충돌처리(바운딩 카메라)를 적용한 이후로 카메라가 벽에 충돌중인 상태에서 공격을 하면 벽이 충돌이 되어서
		// 카메라의 포지션에서 조금 앞으로 땡겨서 Ray의 시작 위치를 잡는다.
		Vector3 _attackPosition = m_CameraTf->m_WorldPosition + m_CameraTf->m_WorldTM.Forward() * 1.4f;
		Vector3 _attackDirection = m_CameraTf->m_WorldTM.Forward();

		/// 충돌 피사체 확인
		RaycastHit _hit;
		_hit = *DLLEngine::CheckRaycastHit(_attackPosition, _attackDirection, m_ShootRange);

		// 충돌체가 있으면
		if (_hit.actor != nullptr)
		{
			Health* _health = _hit.actor->GetMyObject()->GetComponent<Health>();
			if (_health != nullptr)
			{
				_health->Damage(m_Damage, _hit.position, _attackDirection, 1.0f, m_MyInventory->GetMyObject(), _hit.actor);
			}
		}
		*/

		/// 렌더러를 이용한 디퍼드 사격 방식
		GameObject* _Owner = m_MyInventory->GetMyObject();

		GameObject* _hitMeshObj = m_HitPoint->GetHitPoint();

		// 임시 태그 String 비교
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
		// 탄 감소
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
/// 탄창에 탄약 개수 설정
/// 
/// 최대 탄창 수보다 많거나 0보다 작으면 값 보정한다.
/// </summary>
/// <param name="ammo">설정할 갯수</param>
/// <returns>성공 여부</returns>
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
/// 현재 탄창의 남은 탄 수 리턴
/// </summary>
/// <returns></returns>
uint Gun::GetCurrentAmmoCount() const
{
	return m_CurrentAmmoCount;
}

/// <summary>
/// 총에 현재 탄 수 증가
/// </summary>
/// <param name="val">증가할 갯수</param>
/// <returns>증가 후 갯수</returns>
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
/// 총에 현재 탄 수 감소
/// </summary>
/// <param name="val">감소시킬 갯수</param>
/// <returns>감소 후 갯수</returns>
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
/// 남은 장전 시간 설정
/// (무기 교체시 장저을 취소하기위해 0.0f으로 세팅할 때 등 사용)
/// </summary>
/// <param name="time"></param>
void Gun::SetReloadCoolTimer(const float& time)
{
	m_ReloadCoolTimer = time;
}

/// <summary>
/// 장전 중인가 판단
/// </summary>
/// <returns>장전 중 여부</returns>
bool Gun::IsReloading() const
{
	// 장전완료까지 남은시간이 있으면
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
		// 재장전
		Reload();

		// 장전 애니메이션 종료
		Animator* _animator = DLLEngine::FindGameObjectByName("Player_Mesh")->GetComponent<Animator>();
		assert(_animator != nullptr);
		_animator->SetOverrideAnimLayer("");
	}
}

/// <summary>
/// 탄창에 현재 장전된 탄이 비었는지 검사
/// </summary>
/// <returns>탄창에 탄 여부</returns>
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
/// 탄창이 꽉 차있는지 검사
/// </summary>
/// <returns>결과</returns>
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
/// 인벤토리에 탄이 다 떨어졌는지 검사
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
/// 장총(1번 주무기) 공격 함수
///
/// </summary>
/// <returns>성공 여부</returns>
bool LongGun::Shoot()
{
	if (Gun::Shoot() == true)
	{
		CA_TRACE("[LongGun] Shoot - 탄 보유 <%d/%d>", m_CurrentAmmoCount, m_MyInventory->GetItemCount(IItem::Type::eLongGunAmmo));
		m_UseCoolTimeTimer = m_ShootCoolTime;	// 쿨타임 세팅

		// 손에 장착중인 아이템의 머즐플레쉬 컴포넌트 가져오기
		MuzzleFlash* _MuzzleFlash = DLLEngine::FindGameObjectByName("MuzzleFlash")->GetComponent<MuzzleFlash>();
		if (_MuzzleFlash != nullptr)
		{
			_MuzzleFlash->Fire();
		}

		// 격발 사운드 이벤트
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
/// 주무기 장전
/// 인벤토리에 보유탄수를 장전하는 만큼 감소시키고
/// 탄창에 탄 증가
/// </summary>
/// <returns>장전 후 탄창의 탄 수</returns>
uint LongGun::Reload()
{
	// 인벤토리에 보유중인 탄 수
	uint remainingAmmunitionCountInInventory = m_MyInventory->GetItemCount(IItem::Type::eLongGunAmmo);

	// 탄창에 비어있는 부분 카운트(장전할 수 있는 갯수)
	uint _requiredCount = m_MaxAmmoCount - m_CurrentAmmoCount;

	/// 장전 불가능
	// 장전할 수 있는 탄 수가 0보다 큰데
	if (_requiredCount > 0)
	{
		// 보유탄약이 0이면
		if (remainingAmmunitionCountInInventory == 0)
		{
			// 실패!
			CA_TRACE("[LongGun] Reload : 인벤토리에 탄약 다 떨어짐...");

			return false;
		}

		/// 장전
		else
		{
			// 보유 탄 >= 장전 요구 탄
			if (remainingAmmunitionCountInInventory >= _requiredCount)
			{
				/// 인벤토리에 탄약 수 감소
				// 보유 탄 수= 보유탄 수 - 요구 탄 수
				m_MyInventory->SetItemCount(IItem::Type::eLongGunAmmo, remainingAmmunitionCountInInventory - _requiredCount);

				/// 탄창에 탄약 수 증가
				// 현재 탄 수 = 현재 탄 수 + 요구 탄 수
				IncreaseCurrentAmmoCount(std::move(_requiredCount));

			}

			// 보유 탄 < 장전 요구 탄
			else
			{
				/// 인벤토리에 탄약수 0 세팅
				// 보유 탄 = 0
				m_MyInventory->SetItemCount(IItem::Type::eLongGunAmmo, 0);

				/// 탄창에 탄약 수 증가
				// 현재 탄 = 현재 탄 수 + 보유 탄
				SetCurrentAmmoCount(m_CurrentAmmoCount + remainingAmmunitionCountInInventory);
			}

			CA_TRACE("[LongGun] Reload() : <%d/%d> ", m_CurrentAmmoCount, m_MyInventory->GetItemCount(IItem::Type::eLongGunAmmo));
		}
	}

	// 장전 중인 상태 제거
	m_MyInventory->GetMyObject()->GetComponent<PlayerController>()->m_bIsReloading = false;

	// 장전 후 탄창의 탄 수 리턴
	return m_CurrentAmmoCount;
}

/// <summary>
/// 인벤토리에 탄약이 비었나 검사
/// (장총)
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
/// 권총(2번 보조무기) 공격 함수
/// </summary>
/// <returns>성공 여부</returns>
bool HandGun::Shoot()
{
	if (Gun::Shoot() == true)
	{
		CA_TRACE("[HandGun] Shoot - 탄 보유 <%d/%d>", m_CurrentAmmoCount, m_MyInventory->GetItemCount(IItem::Type::eHandGunAmmo));
		m_UseCoolTimeTimer = m_ShootCoolTime;	// 쿨타임 세팅

		// 격발 사운드 이벤트
		auto* _soundEvent = m_pAudio->PlayEvent("event:/Player_Pistol");
		float dist = m_pAudio->GetEventDistance("event:/Player_Pistol");

		return true;
	}

	//CA_TRACE("[HandGun] Shoot CoolDown ~~~ %f", m_RemainingCoolTime);
	return false;
}

uint HandGun::Reload()
{
	// 인벤토리에 보유중인 탄 수
	uint remainingAmmunitionCountInInventory = m_MyInventory->GetItemCount(IItem::Type::eHandGunAmmo);

	// 탄창에 비어있는 부분 카운트(장전할 수 있는 갯수)
	uint _requiredCount = m_MaxAmmoCount - m_CurrentAmmoCount;

	/// 장전 불가능
	// 장전할 수 있는 탄 수가 0보다 큰데
	if (_requiredCount > 0)
	{
		// 보유탄약이 0이면
		if (remainingAmmunitionCountInInventory == 0)
		{
			// 실패!
			CA_TRACE("[HandGun] Reload : 인벤토리에 탄약 다 떨어짐...");

			return false;
		}

		/// 장전
		else
		{
			// 보유 탄 >= 장전 요구 탄
			if (remainingAmmunitionCountInInventory >= _requiredCount)
			{
				/// 인벤토리에 탄약 수 감소
				// 보유 탄 수= 보유탄 수 - 요구 탄 수
				m_MyInventory->SetItemCount(IItem::Type::eHandGunAmmo, remainingAmmunitionCountInInventory - _requiredCount);

				/// 탄창에 탄약 수 증가
				// 현재 탄 수 = 현재 탄 수 + 요구 탄 수
				IncreaseCurrentAmmoCount(std::move(_requiredCount));

			}

			// 보유 탄 < 장전 요구 탄
			else
			{
				/// 인벤토리에 탄약수 0 세팅
				// 보유 탄 = 0
				m_MyInventory->SetItemCount(IItem::Type::eHandGunAmmo, 0);

				/// 탄창에 탄약 수 증가
				// 현재 탄 = 현재 탄 수 + 보유 탄
				SetCurrentAmmoCount(m_CurrentAmmoCount + remainingAmmunitionCountInInventory);
			}

			CA_TRACE("[HandGun] Reload() : <%d/%d> ", m_CurrentAmmoCount, m_MyInventory->GetItemCount(IItem::Type::eHandGunAmmo));
		}
	}

	// 장전 상태 해제
	m_MyInventory->GetMyObject()->GetComponent<PlayerController>()->m_bIsReloading = false;

	// 장전 후 탄창의 탄 수 리턴
	return m_CurrentAmmoCount;
}

/// <summary>
/// 인벤토리에 탄약이 비었나 검사
/// (권총)
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
