#pragma once
#include "SceneBase.h"

class CLTestInGameUI : public SceneBase
{
public:
	CLTestInGameUI();
	virtual ~CLTestInGameUI();
	CLTestInGameUI(const CLTestInGameUI& other) = default;

	virtual void Initialize() override;
	virtual void Update(float dTime) override;
	virtual void Reset() override;

private:

	GameObject* m_pDirectionalLight;
	GameObject* m_pMainCamera;

	GameObject* m_aimPoint;
	GameObject* m_questText;
	GameObject* m_questText_Sub;
	GameObject* m_combetStat;
	GameObject* m_Life;

	GameObject* m_TmpAssistLifeBar;
	GameObject* m_TmpAssistLife;

	GameObject* m_combetStat_Weapon;
	GameObject* m_combetStat_Flask;
	GameObject* m_combetStat_Fire;
	GameObject* m_combetStat_Heal;
	GameObject* m_combetStat_Alcohol;
	GameObject* m_combetStat_Herb;

};

/// 문제점
/// 1. UI가 오브젝트뒤에 그려진다
/// 2. 조준점 - 공격시(클릭시) 이미지 변경
/// 3. 
/// 
