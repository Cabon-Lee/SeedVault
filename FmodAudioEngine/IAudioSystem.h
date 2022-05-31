#pragma once

#include <vector>
#include <string>

using PATH = std::string;

class SoundEvent;
__interface IAudioSystem
{
public:
	virtual bool Initialize(int maxChanelCnt, const char* bankPath) abstract;		//마지막 슬래시 포함/미포함 상관없다.
	virtual void Release() abstract;

	virtual bool LoadAllBank(const std::vector<std::string>& vec) abstract;
	
	virtual void Update() abstract;

	//virtual SoundEvent PlayEvent(const PATH& name) abstract;
}; 