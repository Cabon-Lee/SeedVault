#include "pch.h"
#include "CLTestHJ.h"
#include "Audio.h"
#include "SoundEvent.h"
#include "CameraController.h"
#include "AudioListener.h"

CLTestHJ::CLTestHJ()
{

}

CLTestHJ::~CLTestHJ()
{

}

void CLTestHJ::Initialize()
{
	m_Audio = DLLEngine::CreateObject(this);
	m_Audio->AddComponent<Audio>(new Audio());

	m_Cam = DLLEngine::CreateObject(this);
	m_Cam->AddComponent<Camera>(new Camera("Listener"));
	m_Cam->AddComponent<CameraController>(new CameraController());
	m_Cam->AddComponent<AudioListener>(new AudioListener());
}

void CLTestHJ::Update(float dTime)
{
	if (DLLInput::InputKeyDown('P'))
	{
		_Event = m_Audio->GetComponent<Audio>()->PlayEvent("event:/Player_Footsteps", "WalkCWalkRun", 1);
	}
	else if (DLLInput::InputKeyUp('P'))
	{
		_Event->Stop();
	}


	if (DLLInput::InputKeyDown(VK_UP))
	{
		DLLEngine::SetBusVolume("bus:/" ,1.0f);
	}
	else if (DLLInput::InputKeyUp(VK_DOWN))
	{
		DLLEngine::SetBusVolume("bus:/", 0.5f);
	}

}

void CLTestHJ::Reset()
{

}
