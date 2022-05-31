#include "pch.h"
#include "Managers.h"
#include "ObjectManager.h"
#include "ComponentSystem.h"
#include "GameObject.h"
#include "Camera.h"
#include "IdDll.h"
#include "SceneBase.h"

#include "EngineDLL.h"

#include <json/json.h>
#include <fstream>

SceneBase::SceneBase()
	:m_SceneId(ID::GetIdByType(IDSCENE, 0))
	, m_SceneName("")
	, m_bPhysicsScene(true)
	, m_CurrentCamreaName("")
	, m_pGameObjectManager(std::make_shared<ObjectManager>())
	, m_SaveData(std::make_shared<Scene_Save>())
{

}

SceneBase::~SceneBase()
{

}

void SceneBase::Initialize_()
{
	Initialize();

	if (m_pGameObjectManager->FindGameObect("EngineCamera") == nullptr)
	{
		//기본 카메라 세팅
		GameObject* _EngineCamra = DLLEngine::CreateObject(this);
		_EngineCamra->SetObjectName("EngineCamera");
		_EngineCamra->AddComponent<Camera>(new Camera("EngineCamera"));

	}

	if (m_pGameObjectManager->FindGameObect("EngineLight") == nullptr)
	{
		//기본 라이트 세팅
		GameObject* _light = DLLEngine::CreateObject(this);
		_light->SetObjectName("EngineLight");
		Light* __light = new Light();
		_light->AddComponent<Light>(__light);
		_light->m_Transform->SetRotationFromVec({ 270.f, 0.0f, 0.0f });
		__light->m_eLightType = eLIGHT_TYPE::DIRECTIONAL;
		__light->SetIntensity(0.5f);
		__light->m_CastShadow = true;

	}

	DLLEngine::SetupParentChild(this);
	DLLEngine::StartComponent(this);
}

bool SceneBase::IsPhysicsScene() const
{
	return m_bPhysicsScene;
}

void SceneBase::SetPhysicsScene(bool val)
{
	m_bPhysicsScene = val;
}

const unsigned int SceneBase::GetSceneIndex()
{
	return m_SceneIndex;
}

void SceneBase::SetSceneIndex(const unsigned int idx)
{
	m_SceneIndex = idx;
}

void SceneBase::ReleaseObject()
{
	m_pGameObjectManager->ReleaseObject();
}

void SceneBase::ReleaseAllObject()
{
	// continual object 상관않고 모두 릴리즈 해줌
	m_pGameObjectManager->ReleaseAllObject();
}

void SceneBase::StartComponent()
{
	Managers::GetInstance()->GetComponentSystem()->Start();
}

void SceneBase::SetParentChild()
{
	m_pGameObjectManager->SetupParentChild();
}

void SceneBase::AddObject(GameObject* gameobject)
{
	m_pGameObjectManager->m_pGameObject_V.push_back(gameobject);
}

void SceneBase::SetName(std::string& name)
{
	m_SceneName = name;
}

std::string& SceneBase::GetSceneName()
{
	return m_SceneName;
}

const unsigned int& SceneBase::GetSceneId()
{
	return m_SceneId;
}

void SceneBase::Save()
{
	m_Value = new Json::Value();

	m_CurrentCamreaName = Managers::GetInstance()->GetCameraManager()->GetNowCamera()->GetName();

	float _version = 0.1f;
	(*m_Value)["Version"] = _version;

	(*m_Value)["Scene"]["m_SceneName"] = m_SceneName;
	(*m_Value)["Scene"]["m_SceneId"] = m_SceneId;
	(*m_Value)["Scene"]["m_CurrentCamreaName"] = m_CurrentCamreaName;
	(*m_Value)["Scene"]["m_bPhysicsScene"] = m_bPhysicsScene;

	for (GameObject* _go : m_pGameObjectManager->m_pGameObject_V)
	{
		_go->Save();
		(*m_Value)["Scene"]["m_GameObject_V"].append(*(_go->GetValue()));
	}

}

void SceneBase::Load()
{
	std::vector<std::string> _all_keys = (*m_Value)["Scene"].getMemberNames();

	for (auto& _key : _all_keys)
	{
		if (_key == "m_SceneName")
		{
			m_SaveData->m_SceneName = (*m_Value)["Scene"]["m_SceneName"].as<decltype(m_SaveData->m_SceneName)>();
		}
		if (_key == "m_SceneId")
		{
			m_SaveData->m_SceneId = (*m_Value)["Scene"]["m_SceneId"].as<decltype(m_SaveData->m_SceneId)>();
		}
		if (_key == "m_CurrentCamreaName")
		{
			m_SaveData->m_CurrentCamreaName = (*m_Value)["Scene"]["m_CurrentCamreaName"].as<decltype(m_SaveData->m_CurrentCamreaName)>();
		}
		if (_key == "m_bPhysicsScene")
		{
			m_SaveData->m_bPhysicsScene = (*m_Value)["Scene"]["m_bPhysicsScene"].as<decltype(m_SaveData->m_bPhysicsScene)>();
		}
	}
}


void SceneBase::WriteSceneData(const std::string& filepath)
{
	Save();
	std::string _fileName = "Scene_" + m_SceneName;//+ "(" + std::to_string(m_SceneId) + ")";

	/// <summary>
	/// file 경로가 바뀌면 함수도 수정필
	/// </summary>
	Managers::GetInstance()->GetJsonManager()->Write(m_Value, filepath, _fileName);
}

