#pragma once

#include <memory>
__interface IResourceManager;

class ResourceObserver
{
public:
	ResourceObserver();
	~ResourceObserver();
	// ���ο� IResourceManager�� ������ �־���ҵ�?
	virtual void ObserverUpdate(std::shared_ptr<IResourceManager> pReosureManager) abstract;
	
};

