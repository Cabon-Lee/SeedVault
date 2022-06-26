#pragma once
#include "ComponentBase.h"

struct AudioListener_Save;
class Camera;
class AudioListener : public ComponentBase
{
public:
	_DLL AudioListener();
	_DLL ~AudioListener();

public:
	virtual void Start() override;
	virtual void Update(float dTime) override;

	virtual void OnPreRender() override;
	virtual void OnRender() override;

	_DLL void SetCameraTransform(Camera* camera);

private:
	Camera* m_ListenerCamera;
public:
	virtual void SaveData();
	virtual void LoadData();
	virtual void LoadPtrData(std::map<unsigned int, GameObject*>* gameobject, std::map<unsigned int, ComponentBase*>* component);

protected:
	AudioListener_Save* m_SaveData;
};

struct AudioListener_Save
{
	bool				m_bEnable;
	unsigned int		m_ComponentId;
	unsigned int		m_ListenerCamera;
};

BOOST_DESCRIBE_STRUCT(AudioListener_Save, (), (
	m_bEnable,
	m_ComponentId,
	m_ListenerCamera
	))