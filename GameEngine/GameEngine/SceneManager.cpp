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

	// 자신의 인덱스를 가지고 있기 위해 추가
	unsigned int _nowIndex = m_Scenes_UM.size() - 1;
	scene->SetSceneIndex(_nowIndex);

	if (scene->IsPhysicsScene() == true)
	{
		Managers::GetInstance()->GetPhysicsEngine()->AddPhysicsScene(scene);
	}
}

void SceneManager::SetNowScene(string name)
{
	//첫번째 신은 이전신을 Release할 필요없이 Initialize만 하면 되기 때문에 이를 구분짓기 위함
	//m_pPrevScene을 쓰기로 합니다.

	////만약 맵에 등록되지 않은 신이 삽입되게 된다면
	//if (m_Scenes_UM.find(name)==m_Scenes_UM.end())
	//{

	//}

	unordered_map<string, SceneBase*>::iterator _scene = m_Scenes_UM.find(name);

	if (_scene == m_Scenes_UM.end())
	{
		Trace("Can not find the scene");
	}

	DLLEngine::GetEngine()->ResetRenderer();


	//첫번째 신이 아닐때
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
		
		//다음에는 if문에 걸리기 위함.
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
