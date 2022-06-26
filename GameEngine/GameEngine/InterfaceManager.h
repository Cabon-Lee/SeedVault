#pragma once

#include <memory>

__interface IRenderer;
__interface IResourceManager;
//__interface IAudioSystem;

class InterfaceManager
{
public:
	InterfaceManager();
	~InterfaceManager();

	std::shared_ptr<IRenderer> CreateRenderer();
	std::shared_ptr<IResourceManager> CreateResourceManager();
	//std::shared_ptr<IAudioSystem> CreateAudioSystem();
private:

};

