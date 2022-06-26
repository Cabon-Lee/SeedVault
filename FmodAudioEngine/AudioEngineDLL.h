#pragma once
#include "AudioDLLDefine.h"
class SoundEvent;
namespace DLLAudio
{
	_FADLL bool Initialize(const char* bankPath, int maxChanelCnt = 512);
	_FADLL void Release();
	_FADLL void Set3DSetting(float dopplerScale, float gameScale);
	_FADLL bool LoadAllBank(const std::vector<std::string>& vec);
	_FADLL void Update();

	_FADLL SoundEvent* PlayEvent(const std::string& soundpath);

	_FADLL void SetListener(const DirectX::SimpleMath::Matrix& veiewMatrix, const DirectX::SimpleMath::Vector3& velocity);

	_FADLL void SetMasterVolume(float val);
	_FADLL void SetAllBusVolume(float val);
	_FADLL float GetBusVolume(const char* name);
	_FADLL void SetBusVolume(const char* name, float val);
	_FADLL bool GetBusPaused(const char* name);
	_FADLL void SetBusPaused(const char* name, bool val);
	_FADLL float GetEventDistance(const char* name);
}

