#include "pch.h"
#include "Inventory.h"
#include "Health.h"
#include "UIManager.h"
#include "Gun.h"

UIManager::UIManager()
	: ComponentBase(ComponentType::Rendering)
{
}

UIManager::~UIManager()
{
}

void UIManager::Start()
{
	m_health = DLLEngine::FindGameObjectByName("Player")->GetComponent<Health>();
	assert(m_health != nullptr);

	m_Inventory = DLLEngine::FindGameObjectByName("Player")->GetComponent<Inventory>();
	assert(m_Inventory != nullptr);


	/// ------------------------------------------
	/// 에임 포인트
	/// ------------------------------------------
	m_aimPoint = DLLEngine::FindGameObjectByName("Aim Point");
	assert(m_aimPoint != nullptr);

	/// ------------------------------------------
	/// 퀘스트 텍스트
	/// ------------------------------------------
	m_questText = DLLEngine::FindGameObjectByName("Quest Text");
	assert(m_questText != nullptr);

	m_questText_Sub = DLLEngine::FindGameObjectByName("Quest Text Sub");
	assert(m_questText_Sub != nullptr);

	/// ------------------------------------------
	/// 주인공 생명바
	/// ------------------------------------------
	m_Life = DLLEngine::FindGameObjectByName("Player Life Stat");
	assert(m_Life != nullptr);

	/// ------------------------------------------
	/// 조수 생명바
	/// ------------------------------------------
	m_TmpAssistLifeBar = DLLEngine::FindGameObjectByName("Assist Life Bar");
	assert(m_TmpAssistLifeBar != nullptr);

	m_TmpAssistLife = DLLEngine::FindGameObjectByName("Assist Life");
	assert(m_TmpAssistLife != nullptr);

	/// ------------------------------------------
	/// 무기
	/// ------------------------------------------
	m_combetStat_Weapon = DLLEngine::FindGameObjectByName("Combet Weapon Stat");
	assert(m_combetStat_Weapon != nullptr);

	m_combetStat_Weapon_SubText = DLLEngine::FindGameObjectByName("Combet Weapon Stat Sub Text");
	assert(m_combetStat_Weapon_SubText != nullptr);

	/// ------------------------------------------
	/// 아이템
	/// ------------------------------------------
	m_combetStat_Flask = DLLEngine::FindGameObjectByName("Combet Flask Stat");
	assert(m_combetStat_Flask != nullptr);

	m_combetStat_Fire = DLLEngine::FindGameObjectByName("Combet Fire Stat");
	assert(m_combetStat_Fire != nullptr);

	m_combetStat_Heal = DLLEngine::FindGameObjectByName("Combet Heal");
	assert(m_combetStat_Heal != nullptr);

	m_combetStat_Alcohol = DLLEngine::FindGameObjectByName("Combet Alcohol");
	assert(m_combetStat_Alcohol != nullptr);

	m_combetStat_Herb = DLLEngine::FindGameObjectByName("Combet Herb");
	assert(m_combetStat_Herb != nullptr);

}

void UIManager::Update(float dTime)
{
}

void UIManager::OnRender()
{
	m_questText->GetComponent<Text>()->PrintSpriteText((TCHAR*)L"발전기 수리");
	m_questText_Sub->GetComponent<Text>()->PrintSpriteText((TCHAR*)L"발전기 수리 부품을 찾는다");

	/// --------------------------------------
	/// 1. 에임포인트 UI 이미지 전환
	/// --------------------
	// 비조준 상태 인텍스 : [0]
	// 조준 상태 인텍스 : [1]

	// 오른쪽클릭 여부에따라서 인덱스를 바꿔주면 될거같다

	/// --------------------------------------
	/// 2. HP UI 이미지 전환
	/// --------------------------------------
	unsigned int _HPIndex = static_cast<int>(m_health->GetHp());

	if (m_health->GetHp() < 0)
	{
		_HPIndex = 0;
	}

	if (_HPIndex >= 0 || _HPIndex <= 6)
	{
		m_Life->GetComponent<Sprite2D>()->SetspriteIndex(static_cast<int>(_HPIndex));
	}

	/// --------------------------------------
	/// 3 - 1. 무기 UI 이미지 전환
	/// --------------------------------------
	auto _equipment = m_Inventory->GetCurrentEquipment();

	if (_equipment->GetType() == IItem::Type::eHandGun)
	{
		m_combetStat_Weapon->GetComponent<Sprite2D>()->SetspriteIndex(0);
	}
	else if (_equipment->GetType() == IItem::Type::eLongGun)
	{
		m_combetStat_Weapon->GetComponent<Sprite2D>()->SetspriteIndex(1);
	}
	else if (_equipment->GetType() == IItem::Type::eFlask)
	{
		m_combetStat_Weapon->GetComponent<Sprite2D>()->SetspriteIndex(2);
	}
	else if (_equipment->GetType() == IItem::Type::eFireFlask)
	{
		m_combetStat_Weapon->GetComponent<Sprite2D>()->SetspriteIndex(3);
	}

	/// --------------------------------------
	/// 3 - 2. 무기 UI 수량 전환
	/// --------------------------------------
	// 현재 가지고있는 총알의 개수
	uint _CurrentGunBulletCount = 0;
	if (_equipment->GetType() == IItem::Type::eLongGun || _equipment->GetType() == IItem::Type::eHandGun)
	{
		_CurrentGunBulletCount = static_cast<Gun*>(_equipment.get())->GetCurrentAmmoCount();
	}
	m_combetStat_Weapon->GetComponent<Text>()->PrintSpriteText((TCHAR*)std::to_wstring(_CurrentGunBulletCount).c_str());

	// 인벤토리에 가지고있는 탄창의 총 개수
	auto _AmoCount = m_Inventory->GetItemCount(IItem::Type::eLongGunAmmo);
	std::wstring __AmoCountString = L"/" + std::to_wstring(_AmoCount);

	m_combetStat_Weapon_SubText->GetComponent<Text>()->PrintSpriteText((TCHAR*)__AmoCountString.c_str());

	/// --------------------------------------
	/// 4 - 1. 플라스크 UI 이미지 전환 및 수량 전환
	/// --------------------------------------
	InvenoryUISetting(m_combetStat_Flask, IItem::Type::eFlask);

	/// --------------------------------------
	/// 5 - 1. 화염병 UI 이미지 전환 및 수량 전환
	/// --------------------------------------
	InvenoryUISetting(m_combetStat_Fire, IItem::Type::eFireFlask);

	/// --------------------------------------
	/// 6 - 1. 회복킷 UI 이미지 전환 및 수량 전환
	/// --------------------------------------
	InvenoryUISetting(m_combetStat_Heal, IItem::Type::eHealSyringe);

	/// --------------------------------------
	/// 7 - 1. 알코올 UI 이미지 전환 및 수량 전환
	/// --------------------------------------
	InvenoryUISetting(m_combetStat_Alcohol, IItem::Type::eAlcohol);

	/// --------------------------------------
	/// 8 - 1. 허브 UI 이미지 전환 및 수량 전환
	/// --------------------------------------
	InvenoryUISetting(m_combetStat_Herb, IItem::Type::eHerb);
}


/// <summary>
/// 아이템의 갯수를 세고 갯수가 0일 경우 이미지를 바꿔끼워줌
/// </summary>
/// <param name="UIgameObject"></param>
/// <param name="itemType"></param>
void UIManager::InvenoryUISetting(GameObject* UIgameObject,IItem::Type itemType)
{
	auto _ItemCount = m_Inventory->GetItemCount(itemType);
	if (_ItemCount < 1)
	{
		//수량이 1 미만이면 off 이미지 출력
		UIgameObject->GetComponent<Sprite2D>()->SetspriteIndex(1);
	}
	else
	{
		UIgameObject->GetComponent<Sprite2D>()->SetspriteIndex(0);
	}

	UIgameObject->GetComponent<Text>()->PrintSpriteText((TCHAR*)std::to_wstring(_ItemCount).c_str());

}
