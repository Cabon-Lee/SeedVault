#include "pch.h"
#include "AudioListener.h"
#include "EngineDLL.h"
#include "Camera.h"

_DLL AudioListener::AudioListener()
	:ComponentBase(ComponentType::Etc)
	, m_ListenerCamera(nullptr)
	, m_SaveData(new AudioListener_Save())
{

}

_DLL AudioListener::~AudioListener()
{

}

void AudioListener::Start()
{

}

void AudioListener::Update(float dTime)
{
	if (m_ListenerCamera == nullptr)
	{
		//이전에 미리 카메라를 넣어놓지 않으면 직접 카메라를 가져온다.
		m_ListenerCamera = m_pMyObject->GetComponent<Camera>();

		if (m_ListenerCamera != nullptr)
		{
			DLLEngine::SetListener(m_ListenerCamera->View(), { 0,0,0 });
		}
	}
	else
	{
		DLLEngine::SetListener(m_ListenerCamera->View(), { 0,0,0 });
	}
}
void AudioListener::OnPreRender()
{

}

void AudioListener::OnRender()
{

}

void AudioListener::SetCameraTransform(Camera* camera)
{
	m_ListenerCamera = camera;
}

void AudioListener::SaveData()
{
	m_SaveData->m_bEnable = GetIsEnabled();
	m_SaveData->m_ComponentId = m_ComponentId;
	m_SaveData->m_ListenerCamera = m_ListenerCamera->GetComponetId();

	save::WriteValue(m_Value, EraseClass(typeid(*this).name()), *m_SaveData);
}

void AudioListener::LoadData()
{
	save::ReadValue(m_Value, EraseClass(typeid(*this).name()), *m_SaveData);

	SetEnable(m_SaveData->m_bEnable);
}

void AudioListener::LoadPtrData(std::map<unsigned int, GameObject*>* gameobject, std::map<unsigned int, ComponentBase*>* component)
{
	if (m_SaveData->m_ListenerCamera != NULL)
	{
		m_ListenerCamera = dynamic_cast<Camera*>(component->at(m_SaveData->m_ListenerCamera));
	}
}
