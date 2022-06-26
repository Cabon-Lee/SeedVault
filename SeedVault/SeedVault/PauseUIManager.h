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

	// 대화 출력 매니저
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

