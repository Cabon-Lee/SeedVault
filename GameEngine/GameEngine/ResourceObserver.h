#pragma once

#include <memory>
__interface IResourceManager;

class ResourceObserver
{
public:
	ResourceObserver();
	~ResourceObserver();
	// 내부에 IResourceManager를 가지고 있어야할듯?
	virtual void ObserverUpdate(std::shared_ptr<IResourceManager> pReosureManager) abstract;
	
};

