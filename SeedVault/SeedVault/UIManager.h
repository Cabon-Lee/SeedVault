#pragma once
#include "ComponentBase.h"
#include "IItem.h"


///
/// 보통 스프라이트 인덱스는 소
/// 

/// <summary>
/// UI관리 클래스
///  : UI 이미지는 보통 있음[0]/없음[1], 
/// 
/// 2022.05.25 B.Bunny
/// </summary>
class UIManager : public ComponentBase
{
public:
	UIManager();
	virtual ~UIManager();

	// ComponentBase Override
	virtual void Start() override;
	virtual void Update(float dTime) override;
	virtual void OnRender() override;

	void InvenoryUISetting(GameObject* UIgameObject, IItem::Type itemType);

	// 플레이어 생명
	class Health* m_health;

	// 플레이어 인벤토리
	class Inventory* m_Inventory;

	// 퀘스트 텍스트

	GameObject* m_aimPoint;
	GameObject* m_questText;
	GameObject* m_questText_Sub;
	
	GameObject* m_Life;

	GameObject* m_TmpAssistLifeBar;
	GameObject* m_TmpAssistLife;

	GameObject* m_combetStat_Weapon;
	GameObject* m_combetStat_Weapon_SubText;
	
	GameObject* m_combetStat_Flask;
	GameObject* m_combetStat_Fire;
	GameObject* m_combetStat_Heal;

	GameObject* m_combetStat_Alcohol;
	GameObject* m_combetStat_Herb;

};

