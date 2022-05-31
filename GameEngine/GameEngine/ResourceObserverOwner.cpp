#include "pch.h"
#include "ResourceObserverOwner.h"
#include "ResourceObserver.h"
#include "ComponentBase.h"
#include "MeshFilter.h"

ResourceObserverOwner::ResourceObserverOwner()
{

}

ResourceObserverOwner::~ResourceObserverOwner()
{

}

void ResourceObserverOwner::Notify()
{
	for(auto& _nowObserver : m_ResourceObserver_V)
	{
		_nowObserver->ObserverUpdate(m_pResourceManager);
	}

	// Notify 한번 끝나면 그냥 내부를 비워버린다
	m_ResourceObserver_V.clear();
}

void ResourceObserverOwner::RegisterObserver(ComponentBase* pComponent)
{
	ResourceObserver* _nowObserver = dynamic_cast<ResourceObserver*>(pComponent);

	if (_nowObserver != nullptr)
	{
		m_ResourceObserver_V.push_back(_nowObserver);
	}
}

void ResourceObserverOwner::RegisterObserver(ResourceObserver* pObserver)
{
	if (pObserver != nullptr)
	{
		m_ResourceObserver_V.push_back(pObserver);
	}
}

void ResourceObserverOwner::SetResourceManager(std::shared_ptr<IResourceManager> pResourceManager)
{
	m_pResourceManager = pResourceManager;
}
