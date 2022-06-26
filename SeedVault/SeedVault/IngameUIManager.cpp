#include "pch.h"
#include "PauseUIManager.h"
#include "DialogueManager.h"
#include "Inventory.h"
#include "Health.h"
#include "Gun.h"
#include "PlayerController.h"
#include "Enemy_Move.h"
#include "EffectUIEnum.h"


IngameUIManager::IngameUIManager()
	: ComponentBase(ComponentType::GameLogic)
{
}

IngameUIManager::~IngameUIManager()
{
}

void IngameUIManager::Start()
{
	m_health = DLLEngine::FindGameObjectByName("Player")->GetComponent<Health>();
	assert(m_health != nullptr);

	m_Inventory = DLLEngine::FindGameObjectByName("Player")->GetComponent<Inventory>();
	assert(m_Inventory != nullptr);

	m_pDialogueManager = DLLEngine::FindGameObjectByName("Dialogue Manager");
	assert(m_Inventory != nullptr);

 	//m_pDialogueManager->GetComponent<CSVLoader>()->m_DialogueText_UM.at("DdialogueExampleUTF8");
	/// ------------------------------------------
	/// SCV 로더
	/// ------------------------------------------
	m_CSVLoader = DLLEngine::FindGameObjectByName("CSVLoader");
	assert(m_CSVLoader != nullptr);

	m_pDialogueManager->GetComponent<DialogueManager>()->m_DialogueText_UM = m_CSVLoader->GetComponent<CSVLoader>()->m_DialogueText_UM;

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
	//m_TmpAssistLifeBar = DLLEngine::FindGameObjectByName("Assist Life Bar");
	//assert(m_TmpAssistLifeBar != nullptr);
	//
	//m_TmpAssistLife = DLLEngine::FindGameObjectByName("Assist Life");
	//assert(m_TmpAssistLife != nullptr);

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

	/// ------------------------------------------
	/// 암살
	/// ------------------------------------------
	m_AssassinIcon = DLLEngine::FindGameObjectByName("AssassinIcon");
	assert(m_AssassinIcon != nullptr);

}

void IngameUIManager::Update(float dTime)
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

	if (m_health->GetHp() > 6)
	{
		_HPIndex = 6;
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
		
	/// --------------------------------------
	/// 9 - 1. 암살 UI
	/// --------------------------------------	
	//플레이어의 암살상태가 true일떄 
	if (DLLEngine::FindGameObjectByName("Player")->GetComponent<PlayerController>()->m_bCanAssassinate == true)
	{
		//플레이어가 인식한 적의 pos를 얻어와서 billboard의 포지션을 설정해준다
		Vector3 temp = Vector3(0.0f, 0.0f, 0.0f);

		Vector3 _playerPos = DLLEngine::FindGameObjectByName("Player")->GetComponent<PlayerController>()->GetAssassinateTarget()->m_Transform->GetWorldPosition();
		//m_AssassinIcon->GetComponent<Transform>()->SetPosition(_playerPos);
		m_AssassinIcon->GetComponent<Transform>()->SetPosition(temp);
	}
}

void IngameUIManager::OnRender()
{
}

void IngameUIManager::DebugStringNext()
{
	m_pDialogueManager->GetComponent<DialogueManager>()->RecognizeTrigger();

}


/// <summary>
/// 아이템의 갯수를 세고 갯수가 0일 경우 이미지를 바꿔끼워줌
/// </summary>
/// <param name="UIgameObject"></param>
/// <param name="itemType"></param>
void IngameUIManager::InvenoryUISetting(GameObject* UIgameObject,IItem::Type itemType)
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

