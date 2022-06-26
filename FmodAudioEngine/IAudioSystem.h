#pragma once

#include <vector>
#include <string>
#include "SimpleMath.h"

using PATH = std::string;

class SoundEvent;
__interface IAudioSystem
{
public:
	virtual bool Initialize(int maxChanelCnt, const char* bankPath) abstract;		//마지막 슬래시 포함/미포함 상관없다.
	virtual void Release() abstract;
	virtual void Set3DSetting(float dopplerScale, float gameScale, float rollOffScale) abstract;

	virtual bool LoadAllBank(const std::vector<std::string>& vec) abstract;

	virtual void Update() abstract;

	virtual SoundEvent* PlayEvent(const PATH& name) abstract;
	virtual void SetListener(const DirectX::SimpleMath::Matrix& veiewMatrix, const DirectX::SimpleMath::Vector3& velocity = DirectX::SimpleMath::Vector3{ 0.f, 0.f, 0.f });

	virtual void SetMasterVolume(float val) abstract;
	virtual float GetBusVolume(const PATH& name)const abstract;
	virtual bool GetBusPaused(const PATH& name)const abstract;
	virtual void SetAllBusVolume(float val)abstract;
	virtual void SetBusVolume(const PATH& name, float val)abstract;
	virtual void SetBusPaused(const PATH& name, bool val)abstract;

	virtual float GetEventDistance(const PATH& name) abstract;
};