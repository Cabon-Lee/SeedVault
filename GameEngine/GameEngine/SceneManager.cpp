#include "pch.h"
#include "Timer.h"
#include "CATrace.h"
#include "Managers.h"
#include "CameraManager.h"
#include "SceneBase.h"
#include "SceneManager.h"
#include "PhysicsEngine.h"
#include "PhysicsUtility.h"

#include "EngineDLL.h"

SceneManager::SceneManager()
	: m_pNowScene(nullptr)
	, m_pPrevScene(nullptr)
{

}

SceneManager::~SceneManager()
{

}

void SceneManager::UpdateNowScene(float dTime)
{
	m_pNowScene->Update(dTime);
}

void SceneManager::AddScene(string name, SceneBase* scene)
{
	m_Scenes_UM.insert({ name, scene });
	scene->SetName(name);

	// �ڽ��� �ε����� ������ �ֱ� ���� �߰�
	unsigned int _nowIndex = m_Scenes_UM.size() - 1;
	scene->SetSceneIndex(_nowIndex);

	if (scene->IsPhysicsScene() == true)
	{
		Managers::GetInstance()->GetPhysicsEngine()->AddPhysicsScene(scene);
	}
}

void SceneManager::SetNowScene(string name)
{
	//ù��° ���� �������� Release�� �ʿ���� Initialize�� �ϸ� �Ǳ� ������ �̸� �������� ����
	//m_pPrevScene�� ����� �մϴ�.

	////���� �ʿ� ��ϵ��� ���� ���� ���Եǰ� �ȴٸ�
	//if (m_Scenes_UM.find(name)==m_Scenes_UM.end())
	//{

	//}

	unordered_map<string, SceneBase*>::iterator _scene = m_Scenes_UM.find(name);

	if (_scene == m_Scenes_UM.end())
	{
		Trace("Can not find the scene");
	}

	DLLEngine::GetEngine()->ResetRenderer();


	//ù��° ���� �ƴҶ�
	if (m_pPrevScene!= nullptr)
	{
		if (m_pNowScene->GetSceneName() == name)
		{
			return;
		}
		m_pPrevScene = m_pNowScene;
		m_pPrevScene->ReleaseObject();
		Managers::GetInstance()->GetCameraManager()->RemoveAllCamera();

		m_pNowScene = _scene->second;
		Managers::GetInstance()->GetPhysicsEngine()->SetNowPhysicsScene(_scene->first);
		m_pNowScene->Initialize_();
		Managers::GetInstance()->GetPhysicsEngine()->GetPxRigidActors();
	}
	else
	{
		m_pNowScene = _scene->second;

		Managers::GetInstance()->GetPhysicsEngine()->SetNowPhysicsScene(_scene->first);
		m_pNowScene->Initialize_();
		Managers::GetInstance()->GetPhysicsEngine()->GetPxRigidActors();
		
		//�������� if���� �ɸ��� ����.
		m_pPrevScene = m_pNowScene;
	}

}

void SceneManager::SetNowScene(unsigned int sceneNumber)
{
	if (sceneNumber >= m_Scenes_UM.size())
	{
		Trace("Can not find the scene %d", sceneNumber);
		sceneNumber = 0;
	}

	unordered_map<string, SceneBase*>::iterator _sceneIter = m_Scenes_UM.begin();
	unsigned int _index = 0;
	for (; _sceneIter != m_Scenes_UM.end(); _sceneIter++)
	{
		if (_index == sceneNumber)
		{
			SetNowScene(_sceneIter->first);
			return;
		}
		_index++;
	}

}

SceneBase* SceneManager::GetNowScene()
{
	return m_pNowScene;
}

const unsigned int SceneManager::GetSceneCnt()
{
	return m_Scenes_UM.size();
}

SceneBase* SceneManager::GetSceneByIndex(unsigned int sceneNumber)
{
	unordered_map<string, SceneBase*>::iterator _sceneIter = m_Scenes_UM.begin();
	unsigned int _index = 0;
	for (; _sceneIter != m_Scenes_UM.end(); _sceneIter++)
	{
		if (_index == sceneNumber)
		{
			return (*_sceneIter).second;
		}
		_index++;
	}

	return nullptr;
}

const std::string SceneManager::GetSceneUMKey(unsigned int sceneNumber)
{
	unordered_map<string, SceneBase*>::iterator _sceneIter = m_Scenes_UM.begin();
	unsigned int _index = 0;
	for (; _sceneIter != m_Scenes_UM.end(); _sceneIter++)
	{
		if (_index == sceneNumber)
		{
			return (*_sceneIter).first;
		}
		_index++;
	}

	return "";
}

const std::string& SceneManager::GetNowSceneName()
{
	return m_pNowScene->GetSceneName();
}
