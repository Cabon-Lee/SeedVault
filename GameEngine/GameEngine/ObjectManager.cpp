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
//		/// Camera compnent�� �ִ� ������Ʈ��� ī�޶󸮽�Ʈ�� ����
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
	//��� ������Ʈ�� �� ������ ���ش�.
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
		// Transform�� �θ��ڽİ��� ������
		if (_object->GetParent() != nullptr)
		{
			_object->GetTransform()->SetParentTM(_object->GetParent());
		}
		/// Transform ���� �� ���� ���� ���� �߰�

	}
}

/// <summary>
/// �������� ��쿡 ���
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

	CA_TRACE("Object Manager : (name)��ġ�ϴ� ������Ʈ�� �����ϴ�.");
	//���� �������� �������� �ʴ� ������Ʈ
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

	CA_TRACE("Object Manager : (id)��ġ�ϴ� ������Ʈ�� �����ϴ�.");
	//���� �������� �������� �ʴ� ������Ʈ
	return nullptr;
}


GameObject* ObjectManager::FindGameObectBinary(const unsigned int& objectId)
{
	int _low = 0;
	int _high = m_pGameObject_V.size() - 1;
	int _mid;

	// high�� low���� �۾����ٸ� ã������ �����Ͱ� ������ ���տ� ����.
	while (_low <= _high)
	{
		// �߾Ӱ��� low�� high�� ���� ���� 2�� ������ �ȴ�.
		_mid = (_low + _high) / 2;
		// ���� ã������ ���� �߾Ӱ����� �۴ٸ� high�� mid - 1�� �д�.
		if (m_pGameObject_V[_mid]->GetGameObjectId() > objectId)
		{
			_high = _mid - 1;
		}
		// ���� ã������ ���� �߾Ӱ����� ũ�ٸ� low�� mid + 1�� �д�.
		else if (m_pGameObject_V[_mid]->GetGameObjectId() < objectId)
		{
			_low = _mid + 1;
		}
		// �߾Ӱ��� ã������ ���� ��ġ�ϸ� mid�� ��ȯ�Ѵ�.
		else
		{
			return m_pGameObject_V[_mid];
		}
	}

	CA_TRACE("Object Manager : ��ġ�ϴ� ������Ʈ�� �����ϴ�.");
	// �����͸� ã�� ���ϸ� -1�� ��ȯ�Ѵ�.
	//���� �������� �������� �ʴ� ������Ʈ
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
	//���� �±� ����Ʈ���� �����ϰ� �� �±� ����Ʈ�� �߰��Ѵ�.

	if (object->GetTag() == "Untagged")
	{
		//Untagged�� ����

	}
	else
	{
		/// <summary>
		/// �ٸ� �±׷� �̹� ����Ǿ� �ִ� ������Ʈ�� �ٸ� �±׸� ����, �� ����
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

	///map["�������� �ʴ� Ű"]�� �ϸ� ���ο� ���� insert�ȴ�.
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
			//�� �̷��� �ؾ���..?
			_iter++;
		}
		std::string a = _iter->first;
		return _iter->first;
	}
}

