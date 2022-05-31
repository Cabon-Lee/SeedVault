#include "pch.h"

#include "EngineDLL.h"

#include "JsonLoader_GE.h"
#include "Camera.h"
#include "Scene.h"

_DLL Scene::Scene()
	: m_JsonLoader(nullptr)
	, m_SaveFile("")
{

}

_DLL Scene::Scene(std::string sceneName)
{
	SceneBase::m_SceneName = sceneName;
}

_DLL Scene::~Scene()
{

}

_DLL void Scene::Initialize()
{
	//�������
	///������ ���ο� ��
	if (m_JsonLoader == nullptr)
	{

	}
	/// json���Ϸ� ���� �ε� �� ��
	else
	{
		m_JsonLoader->SceneLoad(this, m_SaveFile, "Scene_" + m_SceneName);


		SetCurrentCamera();
		for (GameObject*& _go : m_pGameObjectManager->m_pGameObject_V)
		{
			_go->LoadPtrData(m_pParsedGameObject_M, m_pParsedComponent_M);
			for (ComponentBase*& _comp : _go->m_Component_V)
			{
				_comp->LoadPtrData(m_pParsedGameObject_M, m_pParsedComponent_M);
			}
		}
		/// <summary>
		/// unsigned int ������ ����Ǿ��� �����͵��� �����ͷ� �ٲ��ش�.
		/// </summary>
	}
	CA_TRACE("Scene : Initialize");
}


void Scene::Update(float dTime)
{

}

void Scene::Reset()
{

}

void Scene::SetParsingData(std::map<unsigned int, GameObject*>* pGameobject, std::map<unsigned int, ComponentBase*>* pComponent)
{
	m_pParsedGameObject_M = pGameobject;
	m_pParsedComponent_M = pComponent;
}

_DLL void Scene::SetJsonLoader(JsonLoader_GE* jsonLoader, const std::string& fullfilepath)
{
	m_JsonLoader = jsonLoader;
	m_SaveFile = fullfilepath;
}

void Scene::SetCurrentCamera()
{
	Managers::GetInstance()->GetCameraManager()->SetNowCamera(m_SaveData->m_CurrentCamreaName);
}
