#include "pch.h"
#include "AudioSystem.h"
#include "DirectoryReader.h"
#include "SoundEvent.h"
#include "SimpleMath.h"
#include "FmodHelper.h"


AudioSystem::AudioSystem()
	:m_MasterVolume(0.8f)
{

}

AudioSystem::~AudioSystem()
{

}

bool AudioSystem::Initialize(int maxChanelCnt, const char* bankPath)
{
	FMOD::Studio::System* _system = nullptr;
	FMOD::System* _lowSystem = nullptr;
	// Create the Studio System object.
	if (!CheckFmodResult(FMOD::Studio::System::create(&_system)))
	{
		exit(-1);
		return false;
	}

	_system->getCoreSystem(&_lowSystem);

	//�� �Լ������� ������ fmod sys�� �̴ϼ� ������ �ȴ�. 
	if (!CheckFmodResult(_system->initialize(maxChanelCnt, FMOD_STUDIO_INIT_NORMAL, FMOD_INIT_NORMAL, 0)))
	{
		exit(-1);
		return false;
	}

	m_pSystem.reset(_system);
	m_pLowLvSystem.reset(_lowSystem);

	//���ڿ����� �����ð� �����ִٸ� Ȯ���� �־��ش�.
	std::string _bankPath(bankPath);
	if (_bankPath.back() != '/')
	{
		_bankPath = _bankPath + "/";
	}

	if (!LoadBank(_bankPath + "Master.strings.bank"))
	{
		return false;
	}

	if (!LoadBank(_bankPath + "Master.bank"))
	{
		return false;
	}

	return true;
}

void AudioSystem::Release()
{
	UnloadAllBank();
	if (m_pSystem != nullptr)
	{
		m_pSystem->release();
	}
}

bool AudioSystem::LoadAllBank(const std::vector<std::string>& vec)
{
	for (int i = 0; i < vec.size(); i++)
	{
		if (!LoadBank(vec[i]))
		{
			//return false;
		}
	}

	return true;
}

void AudioSystem::UnloadAllBank()
{
	for (auto& iter : m_Bank_UM)
	{
		// Unload the sample data, then the bank itself
		iter.second->unloadSampleData();
		iter.second->unload();
	}
	// No banks means no events
	m_Bank_UM.clear();
	m_Event_UM.clear();
	m_Bus_UM.clear();
}

void AudioSystem::Update()
{
	//������Ʈ���� ������ �̺�Ʈ�� ã�´�.
	std::vector<ID> _doneEvent_V;

	for (auto& _iter : m_EventInstance_UM)
	{
		FMOD::Studio::EventInstance* _event(_iter.second);
		FMOD_STUDIO_PLAYBACK_STATE _state;

		_event->getPlaybackState(&_state);
		if (_state == FMOD_STUDIO_PLAYBACK_STOPPED)
		{
			//�̺�Ʈ�� �����ϰ� done id �ʿ� �߰��Ѵ�.
			_event->release();
			_doneEvent_V.emplace_back(_iter.first);
		}
	}

	//�Ϸ�� �̺�Ʈ �ν��Ͻ��� �ʿ��� ����
	for (auto _id : _doneEvent_V)
	{
		m_EventInstance_UM.erase(_id);
	}

	/// <summary>
	/// FMOD Update
	/// </summary>
	m_pSystem->update();
}

void AudioSystem::Set3DSetting(float dopplerScale, float gameScale, float rollOffScale /*=1.0f*/)
{
	m_pLowLvSystem->set3DSettings(dopplerScale,		//1=����, 1���� ũ�� ����� �Ҹ��� ����.
		gameScale,		//���� ������ ũ�� = 1m
		rollOffScale);	//���÷��� �������.
}

void AudioSystem::SetListener(const FMOD_VECTOR& position, const FMOD_VECTOR& forward, const FMOD_VECTOR& up, const FMOD_VECTOR& velocity /*= { 0.f, 0.f, 0.f }*/)
{
	FMOD_3D_ATTRIBUTES listener;

	listener.position = position;
	listener.forward = forward;
	listener.up = up;
	listener.velocity = velocity;

	//�����ʰ� �Ѹ��̸� 0���� �ִ´�.
	CheckFmodResult(m_pSystem->setListenerAttributes(0, &listener));
}

void AudioSystem::SetListener(const DirectX::SimpleMath::Matrix& veiewMatrix, const DirectX::SimpleMath::Vector3& velocity /*= { 0.f, 0.f, 0.f }*/)
{
	DirectX::SimpleMath::Matrix _invertViewMatrix = veiewMatrix.Invert();

	DirectX::SimpleMath::Vector3 a = _invertViewMatrix.Translation();
	SetListener(
		VectorToFMODVec(_invertViewMatrix.Translation()),
		VectorToFMODVec(_invertViewMatrix.Forward()),
		VectorToFMODVec(_invertViewMatrix.Up()),
		VectorToFMODVec(velocity)
	);

}

void AudioSystem::SetMasterVolume(float val)
{
	for (auto& bus : m_Bus_UM)
	{
		float _volume = 0; 
		float _masteredVolume = 0;
		bus.second->getVolume(&_masteredVolume);
		_volume = _masteredVolume / m_MasterVolume * val;
		bus.second->setVolume(_volume);
	}

	m_MasterVolume = val;
}

SoundEvent* AudioSystem::PlayEvent(const PATH& name)
{
	static ID s_ID = 0;
	ID _retId = 0;

	auto iter = m_Event_UM.find(name);
	if (iter != m_Event_UM.end())
	{
		//�̺�Ʈ �ν��Ͻ� ����
		FMOD::Studio::EventInstance* _event = nullptr;
		iter->second->createInstance(&_event);
		if (_event)
		{
			_event->start();
			s_ID++;
			_retId = s_ID;
			m_EventInstance_UM.emplace(_retId, _event);
		}
		return new SoundEvent(this, _retId, name);
	}
	else
	{
		CA_TRACE("FMOD Audio - (PlayEvent)No Data %s", name);
		return nullptr;
	}
}

float AudioSystem::GetBusVolume(const PATH& name) const
{
	float value = 0;

	auto _bus =
		m_Bus_UM.find(name) != m_Bus_UM.end() ?
		m_Bus_UM.find(name)->second : nullptr;

	if (_bus != nullptr)
	{
		_bus->getVolume(&value);
	}

	return value;
}

bool AudioSystem::GetBusPaused(const PATH& name) const
{
	bool value = false;

	auto _bus =
		m_Bus_UM.find(name) != m_Bus_UM.end() ?
		m_Bus_UM.find(name)->second : nullptr;

	if (_bus != nullptr)
	{
		_bus->getPaused(&value);
	}

	return value;
}

void AudioSystem::SetAllBusVolume(float val)
{
	for (auto& bus : m_Bus_UM)
	{
		bus.second->setVolume(val);
	}
}

void AudioSystem::SetBusVolume(const PATH& name, float val)
{
	auto _bus =
		m_Bus_UM.find(name) != m_Bus_UM.end() ?
		m_Bus_UM.find(name)->second : nullptr;

	if (_bus != nullptr)
	{
		_bus->setVolume(val);
	}
}

void AudioSystem::SetBusPaused(const PATH& name, bool val)
{
	auto _bus =
		m_Bus_UM.find(name) != m_Bus_UM.end() ?
		m_Bus_UM.find(name)->second : nullptr;

	if (_bus != nullptr)
	{
		_bus->setPaused(val);
	}
}

float AudioSystem::GetEventDistance(const PATH& name)
{
	float min = 0;
	float max = 0;

	GetEventDescription(name)->getMinMaxDistance(&min, &max);

	return max;
}

FMOD::Studio::EventInstance* AudioSystem::GetEventInstance(ID id)
{
	if (m_EventInstance_UM.find(id) != m_EventInstance_UM.end())
	{
		return m_EventInstance_UM.find(id)->second;
	}
	else
	{
		return nullptr;
	}
}

FMOD::Studio::EventDescription* AudioSystem::GetEventDescription(const PATH& name) const
{
	if (m_Event_UM.find(name) != m_Event_UM.end())
	{
		return m_Event_UM.find(name)->second;
	}
	else
	{
		return nullptr;
	}
}

bool AudioSystem::CheckFmodResult(const FMOD_RESULT& result) const
{
	if (result != FMOD_OK)
	{
#ifdef _DEBUG || _TRACE
		CA_TRACE("FMOD Audio - ERROR (%d) %s\n", result, FMOD_ErrorString(result));
#endif
		return false;
	}
	return true;
}

bool AudioSystem::LoadBank(const std::string& name)
{
	//�ι� �ε����� �ʵ��� �Ѵ�.
	if (m_Bank_UM.find(name) != m_Bank_UM.end())
	{
#ifdef _DEBUG || _TRACE
		CA_TRACE("FMOD Audio - Already Loaded(%s)", name.c_str());
#endif
		return true;
	}

	FMOD::Studio::Bank* _bank = nullptr;

	if (CheckFmodResult(m_pSystem->loadBankFile(name.c_str(), FMOD_STUDIO_LOAD_BANK_NORMAL, &_bank)))
	{
		char _pathBuffer[512] = { 0, };

		//�ε��� ��ũ�� �ʿ� �߰��Ѵ�.
		m_Bank_UM.emplace(name, _bank);
		//��Ʈ���� ������ �ƴ� ��� ���� �����͸� �ε�
		_bank->loadSampleData();
		//��ũ�� �̺�Ʈ ���� ��´�.
		int _eventCnt = 0;
		_bank->getEventCount(&_eventCnt);
		if (_eventCnt > 0)
		{
			//��ũ���� �̺�Ʈ ��ũ���� ����Ʈ�� ��´�
			std::vector<FMOD::Studio::EventDescription*> _event_V(_eventCnt);
			_bank->getEventList(_event_V.data(), _eventCnt, &_eventCnt);
			//���� ����Ʈ�� map�� ����
			for (int i = 0; i < _eventCnt; i++)
			{
				_event_V[i]->getPath(_pathBuffer, 512, nullptr);
				//�ߺ��˻� : �ߺ����� �ʴ� �͸� �����Ѵ�.
				if (m_Event_UM.find(std::string(_pathBuffer)) == m_Event_UM.end())
				{
					m_Event_UM.insert(std::make_pair(_pathBuffer, _event_V[i]));
				}
			}

		}

		//��ũ���� ���� ���� ��´�.
		int _busCnt = 0;
		_bank->getBusCount(&_busCnt);
		if (_busCnt > 0)
		{
			//��ũ���� ���� ����Ʈ�� ��´�
			std::vector<FMOD::Studio::Bus*> _bus_V(_busCnt);
			_bank->getBusList(_bus_V.data(), _busCnt, &_busCnt);
			//���� ����Ʈ�� map�� ����
			for (int i = 0; i < _busCnt; i++)
			{
				_bus_V[i]->getPath(_pathBuffer, 254, nullptr);
				m_Bus_UM.emplace(_pathBuffer, _bus_V[i]);
			}
		}
		return true;
	}

	return false;
}

