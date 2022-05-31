#include "pch.h"
#include "SoundEvent.h"

SoundEvent::SoundEvent()
{

}

SoundEvent::SoundEvent(AudioSystem* system, unsigned int id)
{

}

SoundEvent::~SoundEvent()
{

}

bool SoundEvent::IsValid()
{
	bool _isSystemExist = false;
	bool _isIdExist = false;

	_isSystemExist = m_pAudioSystem.lock() != nullptr;
	if (_isSystemExist)
	{
		_isIdExist = m_pAudioSystem.lock()->GetEventInstance(m_Id) != nullptr;
	}

	return _isSystemExist && _isIdExist;	//모두 true여야 true를 반환한다.
}

bool SoundEvent::Is3D() const
{
	bool _is3D = false;

	auto _event =
		m_pAudioSystem.lock() == nullptr ?
		m_pAudioSystem.lock()->GetEventInstance(m_Id) : nullptr;

	if (_event != nullptr)
	{
		FMOD::Studio::EventDescription* _eventDesc = nullptr;
		_event->getDescription(&_eventDesc);

		if (_eventDesc != nullptr)
		{
			_eventDesc->is3D(&_is3D);
		}
	}

	return _is3D;
}

void SoundEvent::Set3DAttribute(const FMOD_VECTOR& position, const FMOD_VECTOR& forward, const FMOD_VECTOR& up, const FMOD_VECTOR& velocity /*= { 0.f, 0.f, 0.f }*/)
{
	auto _event =
		m_pAudioSystem.lock() == nullptr ?
		m_pAudioSystem.lock()->GetEventInstance(m_Id) : nullptr;

	if (_event != nullptr)
	{
		FMOD_3D_ATTRIBUTES _3dAtt;

		_3dAtt.position = position;
		_3dAtt.forward = forward;
		_3dAtt.up = up;
		_3dAtt.velocity = velocity;

		_event->set3DAttributes(&_3dAtt);
	}
}

void SoundEvent::SetPaused(bool value)
{
	auto _event =
		m_pAudioSystem.lock() == nullptr ?
		m_pAudioSystem.lock()->GetEventInstance(m_Id) : nullptr;

	if (_event != nullptr)
	{
		_event->setPaused(value);
	}
}

void SoundEvent::SetVolume(float value)
{
	auto _event =
		m_pAudioSystem.lock() == nullptr ?
		m_pAudioSystem.lock()->GetEventInstance(m_Id) : nullptr;

	if (_event != nullptr)
	{
		_event->setVolume(value);
	}
}

void SoundEvent::SetPitch(float value)
{
	auto _event =
		m_pAudioSystem.lock() == nullptr ?
		m_pAudioSystem.lock()->GetEventInstance(m_Id) : nullptr;

	if (_event != nullptr)
	{
		_event->setPitch(value);
	}
}

bool SoundEvent::GetPaused() const
{
	bool value = false;

	auto _event =
		m_pAudioSystem.lock() == nullptr ?
		m_pAudioSystem.lock()->GetEventInstance(m_Id) : nullptr;

	if (_event != nullptr)
	{
		_event->getPaused(&value);
	}

	return value;
}

float SoundEvent::GetVolume() const
{
	float value = 0;

	auto _event =
		m_pAudioSystem.lock() == nullptr ?
		m_pAudioSystem.lock()->GetEventInstance(m_Id) : nullptr;

	if (_event != nullptr)
	{
		_event->getVolume(&value);
	}

	return value;
}

float SoundEvent::GetPitch() const
{
	float value = 0;

	auto _event =
		m_pAudioSystem.lock() == nullptr ?
		m_pAudioSystem.lock()->GetEventInstance(m_Id) : nullptr;

	if (_event != nullptr)
	{
		_event->getPitch(&value);
	}

	return value;
}
