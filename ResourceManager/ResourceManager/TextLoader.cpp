#include "pch.h"
#include "TextLoader.h"
#include "EffectUIEnum.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

enum class eQuestContent
{
	TriggerNum,
	MorS,
	Text,
	Count
};

enum class eDialogueContent
{
	TriggerNum,
	Speaker,
	Time,
	Text,
	Count
};

void TextLoader::LoadQuestText(std::string fileName, std::vector<QuestBlock>& _QuestBlock)
{
	std::locale::global(std::locale(".UTF-8"));

	std::wstring _line;
	std::wstring _cell;
	std::wifstream _file(fileName); // example.txt 파일을 연다. 없으면 생성. 
	int idx = 0;

	if (_file.is_open())
	{
		while (std::getline(_file, _line))
		{
			std::wstringstream _numchecklineStream(_line);
			std::wstring _numcheck;
			std::getline(_numchecklineStream, _numcheck, L',');
			bool _numCheck_B = true;

			//맨첫자리가 숫자일때 시작(트리거 숫자)
			for (int i = 0; i < _numcheck.size(); ++i)
			{
				if (std::isdigit(_numcheck[i]) == false)
				{
					_numCheck_B = false;
					break;
				}
			}

			if (_numCheck_B == true)
			{
				std::wstringstream _lineStream(_line);
				
				std::getline(_numchecklineStream, _numcheck, L',');
				if (_numcheck == L"M")
				{
					QuestBlock _oneQuestBlock;

					for (unsigned int i = 0; i < (unsigned int)eQuestContent::Count; ++i)
					{
						std::getline(_lineStream, _cell, L',');

						switch ((eQuestContent)i)
						{
						case eQuestContent::TriggerNum:
							_oneQuestBlock.triggerIdx = std::stoi(_cell);
							break;
						case eQuestContent::Text:
							_oneQuestBlock.MainQuest = _cell;
							break;
						}
					}
					_QuestBlock.push_back(_oneQuestBlock);

				}
				else if (_numcheck == L"S")
				{
					QuestString _oneQuestString;

					for (unsigned int i = 0; i < (unsigned int)eQuestContent::Count; ++i)
					{
						std::getline(_lineStream, _cell, L',');

						switch ((eQuestContent)i)
						{
						case eQuestContent::TriggerNum:
							_oneQuestString.triggerIdx = std::stoi(_cell);
							break;
						case eQuestContent::Text:
							_oneQuestString.SubQuest = _cell;
							break;
						}
					}
					_QuestBlock.back().SubQuest_V.push_back(_oneQuestString);

				}
			}

		}
		_file.close(); // 열었떤 파일을 닫는다. 
	}
}

void TextLoader::LoadDialogueText(std::string fileName, std::vector<struct DialogueBlock>& _DialogueBlock)
{
	std::locale::global(std::locale(".UTF-8"));

	std::wstring _line;
	std::wstring _cell;
	std::wifstream _file(fileName); // example.txt 파일을 연다. 없으면 생성. 
	int idx = 0;

	if (_file.is_open())
	{
		while (std::getline(_file, _line))
		{
			std::wstringstream _numchecklineStream(_line);
			std::wstring _numcheck;
			std::getline(_numchecklineStream, _numcheck, L',');
			bool _numCheck_B = true;

			//맨첫자리가 숫자일때 시작(트리거 숫자)
			for (int i = 0; i < _numcheck.size(); ++i)
			{
				if (std::isdigit(_numcheck[i]) == false)
				{
					_numCheck_B = false;
					break;
				}
			}

			if (_numCheck_B == true)
			{
				// 트리거 숫자가 마지막에 들어간 _DialogueBlock의 트리거와 다를떄 새로운 블럭을 만들어서 넣어준다
				int _nowTriggerNum = std::stoi(_numcheck);
				if ((_DialogueBlock.size() > 0 && _DialogueBlock.back().TriggerIdx != _nowTriggerNum) || _DialogueBlock.size() == 0)
					//if (_DialogueBlock.back().TriggerIdx != _nowTriggerNum || _DialogueBlock.size() == 0)
				{
					DialogueBlock _oneBlock;
					_oneBlock.TriggerIdx = _nowTriggerNum;

					_DialogueBlock.push_back(_oneBlock);
				}

				std::wstringstream _lineStream(_line);
				DialogueString _oneString;
				// 4가지 요소를 나눠담는다
				for (unsigned int i = 0; i < (unsigned int)eDialogueContent::Count; ++i)
				{
					std::getline(_lineStream, _cell, L',');

					//트리거 숫자가 같을때만 한블럭에 담는다
					//if (i == (unsigned int)eDialogueContent::TriggerNum &&
					//	std::wstringstream ssInt(_cell) == _DialogueBlock.back().TriggerIdx)

					switch ((eDialogueContent)i)
					{
					case eDialogueContent::TriggerNum:
						_oneString.triggerIdx = std::stoi(_cell);
						break;
					case eDialogueContent::Speaker:
						if (_cell == L"M")
						{
							_oneString.speaker = eSpeaker::M;
						}
						else if (_cell == L"A")
						{
							_oneString.speaker = eSpeaker::A;
						}
						else
						{
							_oneString.speaker = eSpeaker::None;
						}
						break;
					case eDialogueContent::Time:
						_oneString.time = std::stof(_cell);
						break;
					case eDialogueContent::Text:
						_oneString.dialogue = _cell;
						break;
					}

				}
				_DialogueBlock.back().dialogueString_V.push_back(_oneString);
			}
		}
		_file.close(); // 열었떤 파일을 닫는다. 
	}
}
