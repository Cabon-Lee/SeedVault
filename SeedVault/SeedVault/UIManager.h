#pragma once
#include "ComponentBase.h"
#include "IItem.h"


///
/// ���� ��������Ʈ �ε����� ��
/// 

/// <summary>
/// UI���� Ŭ����
///  : UI �̹����� ���� ����[0]/����[1], 
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

	// �÷��̾� ����
	class Health* m_health;

	// �÷��̾� �κ��丮
	class Inventory* m_Inventory;

	// ����Ʈ �ؽ�Ʈ

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

