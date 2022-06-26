#include "pch.h"
#include "CSVLoader.h"

#include "EffectUIEnum.h"
#include "IResourceManager.h"

CSVLoader::CSVLoader()
	: ComponentBase(ComponentType::Etc)
{
}

CSVLoader::~CSVLoader()
{
}

void CSVLoader::Start()
{
	int i = 0;
}

void CSVLoader::OnRender()
{
}

void CSVLoader::ObserverUpdate(std::shared_ptr<IResourceManager> pResourceManager)
{
	m_QuestText_UM = pResourceManager->GetWholeQuestText();
	m_DialogueText_UM = pResourceManager->GetWholeDialogueText();
}
