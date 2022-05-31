#pragma once

#include <vector>

__interface IResourceManager;
class ResourceObserver;
class ComponentBase;

class ResourceObserverOwner
{
public:
	ResourceObserverOwner();
	~ResourceObserverOwner();

public:
	void Notify();
	void RegisterObserver(ComponentBase* pComponent);
	void RegisterObserver(ResourceObserver* pObserver);
	void SetResourceManager(std::shared_ptr<IResourceManager> pResourceManager);

private:
	std::shared_ptr<IResourceManager> m_pResourceManager;
	std::vector<ResourceObserver*> m_ResourceObserver_V;
};

