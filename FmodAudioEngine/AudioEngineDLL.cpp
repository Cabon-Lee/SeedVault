#include "pch.h"
#include "AudioEngineDLL.h"

#include "AudioSystem.h"
#include "SoundEvent.h"

static IAudioSystem* g_AudioEngine = nullptr;

_FADLL bool DLLAudio::Initialize(const char* bankPath, int maxChanelCnt)
{
	bool val = false;
	if (g_AudioEngine == nullptr)
	{
		g_AudioEngine = new AudioSystem();
		val = g_AudioEngine->Initialize(maxChanelCnt, bankPath);
	}
	else
	{
		CA_TRACE("FMOD Audio - Already Initialized");
	}

	return val;
}

_FADLL void DLLAudio::Release()
{
	if (g_AudioEngine!= nullptr)
	{
		g_AudioEngine->Release();
	}
	else
	{
		CA_TRACE("FMOD Audio - Not thing to Release");
	}
}

_FADLL void DLLAudio::Set3DSetting(float dopplerScale, float gameScale)
{
	if (g_AudioEngine != nullptr)
	{
		g_AudioEngine->Set3DSetting(dopplerScale, gameScale, 1.0f);
	}
	else
	{
		CA_TRACE("FMOD Audio - (Set3DSetting)Not Initialized AudioEngine");
	}
}

_FADLL bool DLLAudio::LoadAllBank(const std::vector<std::string>& vec)
{
	bool val = false;
	if (g_AudioEngine != nullptr)
	{
		val = g_AudioEngine->LoadAllBank(vec);
	}
	else
	{
		CA_TRACE("FMOD Audio - (LoadAllBank)Not Initialized AudioEngine");
	}
	return val;
}

_FADLL void DLLAudio::Update()
{
	if (g_AudioEngine != nullptr)
	{
		g_AudioEngine->Update();
	}
	else
	{
		CA_TRACE("FMOD Audio - (Update)Not Initialized AudioEngine");
	}
}

_FADLL SoundEvent* DLLAudio::PlayEvent(const std::string& soundpath)
{
	if (g_AudioEngine != nullptr)
	{
		return g_AudioEngine->PlayEvent(soundpath);
	}
	else
	{
		CA_TRACE("FMOD Audio - (PlayEvent)Not Initialized AudioEngine");
		return nullptr;
	}
}

_FADLL void DLLAudio::SetListener(const DirectX::SimpleMath::Matrix& veiewMatrix, const DirectX::SimpleMath::Vector3& velocity)
{
	if (g_AudioEngine != nullptr)
	{
		g_AudioEngine->SetListener(veiewMatrix, velocity);
	}
	else
	{
		CA_TRACE("FMOD Audio - (SetListener)Not Initialized AudioEngine");
	}
}

_FADLL void DLLAudio::SetMasterVolume(float val)
{
	if (g_AudioEngine != nullptr)
	{
		g_AudioEngine->SetMasterVolume(val);
	}
	else
	{
		CA_TRACE("FMOD Audio - (SetMasterVolume)Not Initialized AudioEngine");
	}
}

_FADLL void DLLAudio::SetAllBusVolume(float val)
{
	if (g_AudioEngine != nullptr)
	{
		g_AudioEngine->SetAllBusVolume(val);
	}
	else
	{
		CA_TRACE("FMOD Audio - (SetAllBusVolume)Not Initialized AudioEngine");
	}
}

_FADLL float DLLAudio::GetBusVolume(const char* name)
{
	float val = 0;;
	if (g_AudioEngine != nullptr)
	{
		val= g_AudioEngine->GetBusVolume(name);
	}
	else
	{
		CA_TRACE("FMOD Audio - (GetBusVolume)Not Initialized AudioEngine");
	}
	return val;
}

_FADLL void DLLAudio::SetBusVolume(const char* name, float val)
{
	if (g_AudioEngine != nullptr)
	{
		g_AudioEngine->SetBusVolume(name, val);
	}
	else
	{
		CA_TRACE("FMOD Audio - (SetBusVolume)Not Initialized AudioEngine");
	}
}

_FADLL bool DLLAudio::GetBusPaused(const char* name)
{
	bool val = false;
	if (g_AudioEngine != nullptr)
	{
		val= g_AudioEngine->GetBusPaused(name);
	}
	else
	{
		CA_TRACE("FMOD Audio - (GetBusPaused)Not Initialized AudioEngine");
	}
	return val;
}

_FADLL void DLLAudio::SetBusPaused(const char* name, bool val)
{
	if (g_AudioEngine != nullptr)
	{
		g_AudioEngine->SetBusPaused(name, val);
	}
	else
	{
		CA_TRACE("FMOD Audio - (SetBusPaused)Not Initialized AudioEngine");
	}
}

_FADLL float DLLAudio::GetEventDistance(const char* name)
{
	float val = 0;
	if (g_AudioEngine != nullptr)
	{
		val = g_AudioEngine->GetEventDistance(name);
	}
	else
	{
		CA_TRACE("FMOD Audio - (GetEventDistance)Not Initialized AudioEngine");
	}
	return val;
}






