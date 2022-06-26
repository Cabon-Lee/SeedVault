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
class PauseUIManager : public ComponentBase
{
public:
	PauseUIManager();
	virtual ~PauseUIManager();

	// ComponentBase Override
	virtual void Start() override;
	virtual void Update(float dTime) override;
	virtual void OnRender() override;

private:

	// ��ȭ ��� �Ŵ���
	GameObject* m_pBackground01;
	GameObject* m_pBackground02;

	GameObject* m_pContinue;
	GameObject* m_pChapter;
	GameObject* m_pTitle;
	GameObject* m_pExit;

	GameObject* m_pPopUp;

	GameObject* m_pYesButton;
	GameObject* m_pNoButton;

};

