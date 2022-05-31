#pragma once
#include "AudioSystem.h"

class AudioSystem;

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
	SoundEvent();
	~SoundEvent();
protected:
	friend class AudioSystem;
	SoundEvent(AudioSystem* system, unsigned int id);

public:
	bool IsValid();
	bool Is3D() const;

	void Set3DAttribute(const FMOD_VECTOR& position, const FMOD_VECTOR& forward, const FMOD_VECTOR& up, const FMOD_VECTOR& velocity = { 0.f, 0.f, 0.f });

	void Stop(bool allowFadeOut = true);
	void SetPaused(bool value);
	void SetVolume(float value);
	void SetPitch(float value);

	bool GetPaused() const;
	float GetVolume() const;
	float GetPitch() const;

private:
	std::weak_ptr<AudioSystem> m_pAudioSystem;
	unsigned int m_Id;
};

