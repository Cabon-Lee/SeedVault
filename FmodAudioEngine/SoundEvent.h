#pragma once
#include "AudioDLLDefine.h"

class AudioSystem;

namespace DirectX
{
	struct XMMATRIX;
	struct XMFLOAT3;

	namespace SimpleMath
	{
		struct Vector3;
		struct Matrix;
	}
}

namespace FMOD
{
	namespace Studio
	{
		class EventInstance;
	}
}

//EventInstance의 래핑클래스
class SoundEvent
{
public:
	SoundEvent();	//금지!
	~SoundEvent();
	SoundEvent(AudioSystem* system, unsigned int id, const std::string path);

public:
	_FADLL bool IsValid();
	_FADLL bool Is3D() const;
	
	//_FADLL void Set3DAttribute(const FMOD_VECTOR& position, const FMOD_VECTOR& forward, const FMOD_VECTOR& up, const FMOD_VECTOR& velocity = { 0.f, 0.f, 0.f });
	_FADLL void Set3DAttribute(const DirectX::SimpleMath::Matrix& worldTrans, const DirectX::SimpleMath::Vector3& velocity = { 0.f, 0.f, 0.f });
	
	_FADLL void Start();
	_FADLL void Stop(bool allowFadeOut = true);
	_FADLL void SetPaused(bool value);
	_FADLL void SetVolume(float value);
	_FADLL void SetPitch(float value);
	_FADLL void SetParameter(const char* name, float value);
	
	_FADLL bool GetPaused() const;
	_FADLL float GetVolume() const;
	_FADLL float GetPitch() const;
	_FADLL float GetParameter(const char* name);
	_FADLL float GetEventDistance();
	

private:
	AudioSystem* m_pAudioSystem;
	const unsigned int m_Id;
	const std::string m_Path;
};

