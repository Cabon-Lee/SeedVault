#include "pch.h"
#include "ComponentSystem.h"
#include "Transform.h"
#include "Camera.h"
#include "GameObject.h"
#include "Managers.h"
#include "ComponentSystem.h"
#include "ComponentBase.h"
#include "ObjectManager.h"

std::vector<GameObject*> ObjectManager::m_pContinualGameObject_V;


ObjectManager::ObjectManager()
{

}

ObjectManager::~ObjectManager()
{

}

GameObject* ObjectManager::CreateObject(bool isContinual)
{
	GameObject* _object = new GameObject(isContinual);

	if (isContinual == true)
	{
		m_pContinualGameObject_V.push_back(_object);
	}
	else
	{
		m_pGameObject_V.push_back(_object);
	}

	return _object;
}

//void ObjectManager::StartDisContinualObject()
//{
//	for (GameObject* _object : m_pGameObject_V)
//	{
//		if (_object->GetIsContinualObject() == true)
//		{
//			continue;
//		}
//
//		for (ComponentBase* _co : _object->m_Component_V)
//		{
//			ComponentSystem::GetInstance()->RegisterComponent(_co->GetComponentType(), _co);
//		}
//		/// <summary>
//		/// Camera compnent가 있는 오브젝트라면 카메라리스트에 삽입
//		/// </summary>
//		AddCameraToManager(_object);
//	}
//}

void ObjectManager::UpdateAllObject(float dTime)
{
	for (GameObject*& _object : m_pGameObject_V)
	{
		if (_object->GetIsEnabled() == true)
		{
			_object->Update(dTime);
		}
	}
}

void ObjectManager::ReleaseAllObject()
{
	//어느 오브젝트든 다 릴리즈 해준다.
	for (size_t _go = 0; _go < m_pGameObject_V.size(); ++_go)
	{
		Managers::GetInstance()->GetComponentSystem()->DeregisterComponent(m_pGameObject_V.at(_go));
		m_pGameObject_V.at(_go)->Release();
		delete(m_pGameObject_V.at(_go));
	}
	m_pGameObject_V.clear();

	for (size_t _go = 0; _go < m_pContinualGameObject_V.size(); ++_go)
	{
		Managers::GetInstance()->GetComponentSystem()->DeregisterComponent(m_pContinualGameObject_V.at(_go));
		m_pContinualGameObject_V.at(_go)->Release();
		delete(m_pGameObject_V.at(_go));
	}
	m_pContinualGameObject_V.clear();

	m_TagObject_M.clear();
}

void ObjectManager::ReleaseObject()
{
	//std::vector<GameObject*>::iterator _objectIter = m_pGameObject_V.begin();
	//for (; _objectIter != m_pGameObject_V.end();)
	//{
	//	if ((*_objectIter)->GetIsContinualObject() == false)
	//	{
	//		Managers::GetInstance()->GetComponentSystem()->DeregisterComponent(*_objectIter);
	//		(*_objectIter)->Release();
	//		delete(*_objectIter);
	//		_objectIter = m_pGameObject_V.erase(_objectIter);
	//	}
	//	else
	//	{
	//		_objectIter++;
	//	}
	//}

	for (size_t _go = 0; _go < m_pGameObject_V.size(); ++_go)
	{
		Managers::GetInstance()->GetComponentSystem()->DeregisterComponent(m_pGameObject_V.at(_go));
		m_pGameObject_V.at(_go)->Release();
		delete(m_pGameObject_V.at(_go));
	}
	m_pGameObject_V.clear();
	m_TagObject_M.clear();
}

void ObjectManager::ReleaseObject(GameObject* object)
{
	size_t _index = 0;
	for (size_t _go = 0; _go < m_pGameObject_V.size(); ++_go)
	{
		if (m_pGameObject_V.at(_go) == object)
		{
			_index = _go;
			break;
		}
	}

	Managers::GetInstance()->GetComponentSystem()->DeregisterComponent(m_pGameObject_V.at(_index));
	m_pGameObject_V.at(_index)->Release();
	delete(m_pGameObject_V.at(_index));

	m_pGameObject_V.erase(m_pGameObject_V.begin() + (_index));
}

void ObjectManager::ResetAllObject()
{
	for (GameObject*& _object : m_pGameObject_V)
	{
		_object->Reset();
	}

	for (GameObject*& _object : m_pContinualGameObject_V)
	{
		_object->Reset();
	}
}

void ObjectManager::SetupParentChild()
{
	for (GameObject*& _object : m_pGameObject_V)
	{
		// Transform을 부모자식관계 정해줌
		if (_object->GetParent() != nullptr)
		{
			_object->GetTransform()->SetParentTM(_object->GetParent());
		}
		/// Transform 말고도 더 있을 것임 추후 추가

	}
}

/// <summary>
/// 예외적인 경우에 사용
/// </summary>
void ObjectManager::AddToManager(GameObject* object)
{
	m_pGameObject_V.push_back(object);
}

void ObjectManager::AddCameraToManager(GameObject* cameraObject)
{
	Camera* _camera = cameraObject->GetComponent<Camera>();
	if (_camera != nullptr)
	{
		m_Camera_V.push_back(_camera);
	}
}

GameObject* ObjectManager::FindGameObect(const std::string& objectName)
{
	for (GameObject*& _go : m_pGameObject_V)
	{
		if (_go->GetObjectName() == objectName)
		{
			return _go;
		}
	}

	CA_TRACE("Object Manager : (name)일치하는 오브젝트가 없습니다.");
	//현재 씬에서는 존재하지 않는 오브젝트
	return nullptr;
}

GameObject* ObjectManager::FindGameObect(const unsigned int& objectId)
{
	for (GameObject*& _go : m_pGameObject_V)
	{
		if (_go->GetGameObjectId() == objectId)
		{
			return _go;
		}
	}

	CA_TRACE("Object Manager : (id)일치하는 오브젝트가 없습니다.");
	//현재 씬에서는 존재하지 않는 오브젝트
	return nullptr;
}


GameObject* ObjectManager::FindGameObectBinary(const unsigned int& objectId)
{
	int _low = 0;
	int _high = m_pGameObject_V.size() - 1;
	int _mid;

	// high가 low보다 작아진다면 찾으려는 데이터가 데이터 집합에 없다.
	while (_low <= _high)
	{
		// 중앙값은 low와 high를 더한 값을 2로 나누면 된다.
		_mid = (_low + _high) / 2;
		// 만약 찾으려는 값이 중앙값보다 작다면 high를 mid - 1로 둔다.
		if (m_pGameObject_V[_mid]->GetGameObjectId() > objectId)
		{
			_high = _mid - 1;
		}
		// 만약 찾으려는 값이 중앙값보다 크다면 low를 mid + 1로 둔다.
		else if (m_pGameObject_V[_mid]->GetGameObjectId() < objectId)
		{
			_low = _mid + 1;
		}
		// 중앙값과 찾으려는 값이 일치하면 mid를 반환한다.
		else
		{
			return m_pGameObject_V[_mid];
		}
	}

	CA_TRACE("Object Manager : 일치하는 오브젝트가 없습니다.");
	// 데이터를 찾지 못하면 -1를 반환한다.
	//현재 씬에서는 존재하지 않는 오브젝트
	return nullptr;
}

void ObjectManager::SetNowCamera()
{

}


std::vector<GameObject*>& ObjectManager::FindGameObjectVByTag(std::string tag)
{
	return m_TagObject_M.at(tag);
}

void ObjectManager::AddToTagManager(const std::string& newTag, GameObject* object)
{
	//이전 태그 리스트에서 제외하고 새 태그 리스트에 추가한다.

	if (object->GetTag() == "Untagged")
	{
		//Untagged는 예외

	}
	else
	{
		/// <summary>
		/// 다른 태그로 이미 저장되어 있던 오브젝트는 다른 태그를 삭제, 후 삽입
		/// </summary>
		std::vector<GameObject*>::iterator _objectIter = m_TagObject_M[object->GetTag()].begin();
		for (; _objectIter != m_TagObject_M[object->GetTag()].end(); _objectIter++)
		{
			if (*_objectIter == object)
			{
				m_TagObject_M[object->GetTag()].erase(_objectIter);
				break;
			}
		}
	}

	///map["존재하지 않는 키"]를 하면 새로운 값이 insert된다.
	m_TagObject_M[newTag].push_back(object);
	object->SetTagName(newTag);
}

const std::string& ObjectManager::GetTagList(unsigned int index)
{
	std::map<std::string, std::vector<GameObject*>>::iterator _iter = m_TagObject_M.begin();

	if (index >= m_TagObject_M.size())
	{
		return std::string("");
	}
	else
	{
		for (int i = 0; i <= index; i++)
		{
			//왜 이렇게 해야함..?
			_iter++;
		}
		std::string a = _iter->first;
		return _iter->first;
	}
}

