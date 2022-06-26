#pragma once
#include "ComponentBase.h"

class DialogueManager : public ComponentBase
{
public:
	DialogueManager();
	virtual ~DialogueManager();

	// ComponentBase Override
	virtual void Start() override;
	virtual void Update(float dTime) override;
	virtual void OnRender() override;

	//트리거 인식하는 함수
	void RecognizeTrigger();

	std::unordered_map<std::string, std::vector<struct DialogueBlock>> m_DialogueText_UM;

private:
	unsigned int m_CurSceneIdx = 0;
	unsigned int m_CurTriggerIdx = 0;
	unsigned int m_CurStringIdx = 0;

	float m_time = 0;

	GameObject* m_pDialogueString;
};

