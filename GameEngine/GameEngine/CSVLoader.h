#pragma once
#include <memory>
#include "DLLDefine.h"
#include "ComponentBase.h"
#include "ResourceObserver.h"

class CSVLoader : public ComponentBase, public ResourceObserver
{
public:
	_DLL CSVLoader();
	_DLL virtual ~CSVLoader();

	virtual void Start() override;
	virtual void Update(float dTime) override {};
	virtual void OnRender() override;

	virtual void ObserverUpdate(std::shared_ptr<__interface IResourceManager> pResourceManager) override;

	std::unordered_map<std::string, std::vector<struct QuestBlock>> m_QuestText_UM;
	std::unordered_map<std::string, std::vector<struct DialogueBlock>> m_DialogueText_UM;

private:

	
};

