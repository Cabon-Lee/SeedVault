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
class IngameUIManager : public ComponentBase
{
public:
	IngameUIManager();
	virtual ~IngameUIManager();

	// ComponentBase Override
	virtual void Start() override;
	virtual void Update(float dTime) override;
	virtual void OnRender() override;

	void DebugStringNext();

private:

	// �÷��̾� ����
	class Health* m_health;

	// �÷��̾� �κ��丮
	class Inventory* m_Inventory;

	// ��ȭ ��� �Ŵ���
	GameObject* m_pDialogueManager;
	
	// CSVLoader
	GameObject* m_CSVLoader;

	GameObject* m_aimPoint;

	// ����Ʈ �ؽ�Ʈ
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

	GameObject* m_AssassinIcon;


	void InvenoryUISetting(GameObject* UIgameObject, IItem::Type itemType);
};

