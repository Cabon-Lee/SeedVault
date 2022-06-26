#pragma once
#include "IAudioSystem.h"
#include "fmod.h"

/// <summary>
/// Audio�� SoundEvent�� ��ȣ friend class��.
/// </summary>
class SoundEvent;

namespace FMOD
{
	class System;

	namespace Studio
	{
		class System;
		class Bank;
		class Bus;
		class EventDescription;
		class EventInstance;
	}
}

/// <summary>
/// Fmod�� ����ϴ� ����� ����
/// </summary>

using PATH = std::string;
using ID = unsigned int;

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

	virtual void Set3DSetting(float dopplerScale, float gameScale, float rollOffScale = 1.0f) override;
	void SetListener(const FMOD_VECTOR& position, const FMOD_VECTOR& forward, const FMOD_VECTOR& up, const FMOD_VECTOR& velocity = { 0.f, 0.f, 0.f });	//�ӵ��� 0�̸� ���÷�ȿ���� ������� �ʴ´�.
	virtual void SetListener
	(const DirectX::SimpleMath::Matrix& veiewMatrix,
		const DirectX::SimpleMath::Vector3& velocity = DirectX::SimpleMath::Vector3{ 0.f, 0.f, 0.f }) override;	//�ӵ��� 0�̸� ���÷�ȿ���� ������� �ʴ´�.


	virtual void SetMasterVolume(float val) override;
	virtual float GetBusVolume(const PATH& name) const override;
	virtual bool GetBusPaused(const PATH& name) const override;
	virtual void SetAllBusVolume(float val) override;
	virtual void SetBusVolume(const PATH& name, float val)override;
	virtual void SetBusPaused(const PATH& name, bool val)override;

	virtual float GetEventDistance(const PATH& name) override;

public:
	SoundEvent* PlayEvent(const PATH& name);

	FMOD::Studio::EventInstance* GetEventInstance(ID id);
	FMOD::Studio::EventDescription* GetEventDescription(const PATH& name) const;

private:
	void UnloadAllBank();
	bool CheckFmodResult(const FMOD_RESULT& result) const;
	bool LoadBank(const std::string& name);


private:
	std::unique_ptr<FMOD::Studio::System> m_pSystem;			//�ַ� ����� �ý���
	std::unique_ptr<FMOD::System> m_pLowLvSystem;				//�����ؽý���

	std::unordered_map<PATH, FMOD::Studio::Bank*> m_Bank_UM;
	std::unordered_map<PATH, FMOD::Studio::Bus*> m_Bus_UM;		//������ �׷�ȭ

	std::unordered_map<PATH, FMOD::Studio::EventDescription*> m_Event_UM;
	std::unordered_map<ID, FMOD::Studio::EventInstance*> m_EventInstance_UM;


private:
	float m_MasterVolume;	//0 ~ 1.0f ������ ��

};

