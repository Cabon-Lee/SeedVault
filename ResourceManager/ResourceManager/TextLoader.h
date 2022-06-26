#pragma once
class TextLoader
{
public:
	void LoadQuestText(std::string fileName, std::vector<struct QuestBlock>& _QuestBlock);
	void LoadDialogueText(std::string fileName, std::vector<struct DialogueBlock>& _DialogueBlock);
};

