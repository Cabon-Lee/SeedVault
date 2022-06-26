#include "pch.h"
#include "DialogueManager.h"
#include <string>

#include "EffectUIEnum.h"

DialogueManager::DialogueManager()
	: ComponentBase(ComponentType::GameLogic)
{
}

DialogueManager::~DialogueManager()
{
}

void DialogueManager::Start()
{
	m_pDialogueString = DLLEngine::FindGameObjectByName("Dialogue Text");
	assert(m_pDialogueString != nullptr);
}

void DialogueManager::Update(float dTime)
{
	m_time += dTime;
	
	if (m_time > m_DialogueText_UM.at("DdialogueExampleUTF8")[m_CurTriggerIdx].dialogueString_V[m_CurStringIdx].time)
	{
		++m_CurStringIdx;
		unsigned int _Idx = m_DialogueText_UM.at("DdialogueExampleUTF8")[m_CurTriggerIdx].dialogueString_V.size();

		if (m_CurStringIdx >= _Idx)
		{
			++m_CurTriggerIdx;
			m_CurStringIdx = 0;

			if (m_CurTriggerIdx >= m_DialogueText_UM.at("DdialogueExampleUTF8").size())
			{
				m_CurTriggerIdx = 0;
			}
		}

		m_time = 0;
	}
	
	//m_pDialogueString->GetComponent<Text>()->PrintSpriteText((TCHAR*)m_DialogueText_UM.at("DdialogueExampleUTF8")[m_CurTriggerIdx].dialogueString_V[m_CurStringIdx].dialogue.c_str());
}

void DialogueManager::OnRender()
{

}

void DialogueManager::RecognizeTrigger()
{

	if (m_time > m_DialogueText_UM.at("DdialogueExampleUTF8")[m_CurTriggerIdx].dialogueString_V[m_CurStringIdx].time)
	{
		++m_CurStringIdx;
		unsigned int _Idx = m_DialogueText_UM.at("DdialogueExampleUTF8")[m_CurTriggerIdx].dialogueString_V.size();

		if (m_CurStringIdx >= _Idx)
		{
			++m_CurTriggerIdx;
			m_CurStringIdx = 0;

			if (m_CurTriggerIdx >= m_DialogueText_UM.at("DdialogueExampleUTF8").size())
			{
				m_CurTriggerIdx = 0;
			}
		}

		m_time = 0;
	}
}

