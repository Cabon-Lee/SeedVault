#pragma once

#include <vector>
#include <string>

using PATH = std::string;

class SoundEvent;
__interface IAudioSystem
{
public:
	virtual bool Initialize(int maxChanelCnt, const char* bankPath) abstract;		//������ ������ ����/������ �������.
	virtual void Release() abstract;

	virtual bool LoadAllBank(const std::vector<std::string>& vec) abstract;
	
	virtual void Update() abstract;

	//virtual SoundEvent PlayEvent(const PATH& name) abstract;
}; 