#pragma once
#include "IAudioSystem.h"
#include "SoundEvent.h"

/// <summary>
/// Audio�� SoundEvent�� ��ȣ friend class��.
/// </summary>

namespace FMOD
{
	class System;

	namespace Studio
	{
		class System;
		class Bank;
		class Bus;
		class EventDescription;
	}
}

/// <summary>
/// Fmod�� ����ϴ� ����� ����
/// </summary>

using PATH = std::string;
using ID = unsigned int;

class AudioSystem :public IAudioSystem
{
public:
	AudioSystem();
	~AudioSystem();
public:
	virtual bool Initialize(int maxChanelCnt, const char* bankPath) override;
	virtual void Release() override;
	virtual bool LoadAllBank(const std::vector<std::string>& vec) override;
	virtual void Update() override;

	void Set3DSetting(float dopplerScale, float gameScale, float rollOffScale =1.0f);
	void SetListener(const FMOD_VECTOR& position, const FMOD_VECTOR& forward, const FMOD_VECTOR& up, const FMOD_VECTOR& velocity = { 0.f, 0.f, 0.f });	//�ӵ��� 0�̸� ���÷�ȿ���� ������� �ʴ´�.


	float GetBusVolume(const PATH& name)const;
	bool GetBusPaused(const PATH& name) const;
	void SetBusVolume(const PATH& name, float val);
	void SetBusPaused(const PATH& name, bool val);

public:
	friend class SoundEvent;
	 SoundEvent PlayEvent(const PATH& name);
	
	std::shared_ptr<FMOD::Studio::EventInstance> GetEventInstance(ID id);

private:
	bool CheckFmodResult(const FMOD_RESULT& result) const;
	bool LoadBank(const std::string& name);
	//bool UnloadBank(const std::string& name);
	//bool UnloadAllBank();

private:
	std::unique_ptr<FMOD::Studio::System> m_pSystem;			//�ַ� ����� �ý���
	std::unique_ptr<FMOD::System> m_pLowLvSystem;				//�����ؽý���

	std::unordered_map<PATH, std::shared_ptr<FMOD::Studio::Bank>> m_Bank_UM;	
	std::unordered_map<PATH, std::shared_ptr<FMOD::Studio::Bus>> m_Bus_UM;		//������ �׷�ȭ

	std::unordered_map<PATH, std::shared_ptr<FMOD::Studio::EventDescription>> m_Event_UM;
	std::unordered_map<ID, std::shared_ptr<FMOD::Studio::EventInstance>> m_EventInstance_UM;


private:

};

