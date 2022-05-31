#include "pch.h"
#include "ResourceObserver.h"
#include "EngineDLL.h"

ResourceObserver::ResourceObserver()
{
	DLLEngine::RegisterObserver(this);
}

ResourceObserver::~ResourceObserver()
{

}
