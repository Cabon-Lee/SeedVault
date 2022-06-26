#include "pch.h"
#include "SoundEvent.h"
#include "AudioSystem.h"
#include "FmodHelper.h"
#include "SimpleMath.h"

SoundEvent::SoundEvent()
	:m_Id(0)
	, m_Path("")
{

}

SoundEvent::SoundEvent(AudioSystem* system, unsigned int id, const std::string path)
	: m_pAudioSystem(system),
	m_Id(id),
	m_Path(path)
{

}

SoundEvent::~SoundEvent()
{

}

_FADLL bool SoundEvent::IsValid()
{
	bool _isSystemExist = false;
	bool _isIdExist = false;

	_isSystemExist = m_pAudioSystem != nullptr;
	if (_isSystemExist)
	{
		_isIdExist = m_pAudioSystem->GetEventInstance(m_Id) != nullptr;
	}

	return _isSystemExist && _isIdExist;	//모두 true여야 true를 반환한다.
}

_FADLL bool SoundEvent::Is3D() const
{
	bool _is3D = false;

	auto _event =
		m_pAudioSystem != nullptr ?
		m_pAudioSystem->GetEventInstance(m_Id) : nullptr;

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

//_FADLL void SoundEvent::Set3DAttribute(const FMOD_VECTOR& position, const FMOD_VECTOR& forward, const FMOD_VECTOR& up, const FMOD_VECTOR& velocity /*= { 0.f, 0.f, 0.f }*/)
//{
//	auto _event =
//		m_pAudioSystem == nullptr ?
//		m_pAudioSystem->GetEventInstance(m_Id) : nullptr;
//
//	if (_event != nullptr)
//	{
//		FMOD_3D_ATTRIBUTES _3dAtt;
//
//		_3dAtt.position = position;
//		_3dAtt.forward = forward;
//		_3dAtt.up = up;
//		_3dAtt.velocity = velocity;
//
//		_event->set3DAttributes(&_3dAtt);
//	}
//}

_FADLL void SoundEvent::Set3DAttribute(const DirectX::SimpleMath::Matrix& worldTrans, const DirectX::SimpleMath::Vector3& velocity)
{
	auto _event =
		m_pAudioSystem != nullptr ?
		m_pAudioSystem->GetEventInstance(m_Id) : nullptr;

	if (_event != nullptr)
	{
		FMOD_3D_ATTRIBUTES _3dAtt;

		_3dAtt.position = VectorToFMODVec(worldTrans.Translation());
		_3dAtt.forward = VectorToFMODVec(worldTrans.Forward());
		_3dAtt.up = VectorToFMODVec(worldTrans.Up());
		_3dAtt.velocity = VectorToFMODVec(velocity);

		_event->set3DAttributes(&_3dAtt);
	}

}

_FADLL void SoundEvent::Start()
{
	auto _event =
		m_pAudioSystem != nullptr ?
		m_pAudioSystem->GetEventInstance(m_Id) : nullptr;


	if (_event != nullptr)
	{
		_event->start();
	}
}

_FADLL void SoundEvent::Stop(bool allowFadeOut)
{
	auto _event =
		m_pAudioSystem != nullptr ?
		m_pAudioSystem->GetEventInstance(m_Id) : nullptr;

	if (_event != nullptr)
	{
		_event->stop(allowFadeOut ? FMOD_STUDIO_STOP_MODE::FMOD_STUDIO_STOP_ALLOWFADEOUT : FMOD_STUDIO_STOP_MODE::FMOD_STUDIO_STOP_IMMEDIATE);
	}
}

_FADLL void SoundEvent::SetPaused(bool value)
{
	auto _event =
		m_pAudioSystem != nullptr ?
		m_pAudioSystem->GetEventInstance(m_Id) : nullptr;

	if (_event != nullptr)
	{
		_event->setPaused(value);
	}
}

_FADLL void SoundEvent::SetVolume(float value)
{
	auto _event =
		m_pAudioSystem != nullptr ?
		m_pAudioSystem->GetEventInstance(m_Id) : nullptr;

	if (_event != nullptr)
	{
		_event->setVolume(value);
	}
}

_FADLL void SoundEvent::SetPitch(float value)
{
	auto _event =
		m_pAudioSystem != nullptr ?
		m_pAudioSystem->GetEventInstance(m_Id) : nullptr;

	if (_event != nullptr)
	{
		_event->setPitch(value);
	}
}

_FADLL void SoundEvent::SetParameter(const char* name, float value)
{
	auto _event =
		m_pAudioSystem != nullptr ?
		m_pAudioSystem->GetEventInstance(m_Id) : nullptr;

	if (_event != nullptr)
	{
		_event->setParameterByName(name, value);
	}
}

_FADLL bool SoundEvent::GetPaused() const
{
	bool value = false;

	auto _event =
		m_pAudioSystem != nullptr ?
		m_pAudioSystem->GetEventInstance(m_Id) : nullptr;

	if (_event != nullptr)
	{
		_event->getPaused(&value);
	}

	return value;
}

_FADLL float SoundEvent::GetVolume() const
{
	float value = 0;

	auto _event =
		m_pAudioSystem != nullptr ?
		m_pAudioSystem->GetEventInstance(m_Id) : nullptr;

	if (_event != nullptr)
	{
		_event->getVolume(&value);
	}

	return value;
}

_FADLL float SoundEvent::GetPitch() const
{
	float value = 0;

	auto _event =
		m_pAudioSystem != nullptr ?
		m_pAudioSystem->GetEventInstance(m_Id) : nullptr;

	if (_event != nullptr)
	{
		_event->getPitch(&value);
	}

	return value;
}

_FADLL float SoundEvent::GetParameter(const char* name)
{
	float value = 0;

	auto _event =
		m_pAudioSystem != nullptr ?
		m_pAudioSystem->GetEventInstance(m_Id) : nullptr;

	if (_event != nullptr)
	{
		_event->getParameterByName(name, &value);
	}

	return value;
}

_FADLL float SoundEvent::GetEventDistance()
{
	float min = 0;
	float max = 0;

	auto _event =
		m_pAudioSystem != nullptr ?
		m_pAudioSystem->GetEventDescription(m_Path) : nullptr;

	if (_event != nullptr)
	{
		//_event->getProperty(FMOD_STUDIO_EVENT_PROPERTY_MAXIMUM_DISTANCE, &value);
		_event->getMinMaxDistance(&min, &max);
	}

	return max;
}

