#include "pch.h"
#include "ComponentBase.h"
#include "IdDll.h"

#include "GameObject.h"
//#include "Managers.h"
#include <json/json.h>
#include <fstream>

GameObject::GameObject()
	: m_bEnable(true)
	, m_bContinual(false)
	, m_bPhysics(false)
	, m_GameObjectId(ID::GetIdByType(IDGAMEOBJECT, 0))
	, m_Transform(nullptr)
	, m_Component_V()
	, m_Name("")
	, m_pParent(nullptr)
	, m_Children_V()
{

}

GameObject::GameObject(bool isContinual)
	: m_bEnable(true)
	, m_bContinual(isContinual)
	, m_bPhysics(false)
	, m_GameObjectId(ID::GetIdByType(IDGAMEOBJECT, 0))
	, m_Transform(nullptr)
	, m_Component_V()
	, m_Name("")
	, m_pParent(nullptr)
	, m_Children_V()
{


}

GameObject::~GameObject()
{

}

void GameObject::Update(float dTime)
{
	//������ ���⼭ ��������Ʈ �Ѵ�.
	/*for (IComponent* _co : m_Component_V)
	{
	   _co->Update(float dTime);
	}*/
}

void GameObject::Release()
{
	/// <summary>
	//DeregisterAllComponents(ComponentSystem::GetInstance());
	/// ������Ʈ �Ŵ������� ������Ʈ ��������� ������ ������Ʈ���� �����Ѵ�.
	/// </summary>
	std::vector<ComponentBase*>::iterator _compIter = m_Component_V.begin();
	for (; _compIter != m_Component_V.end();)
	{
		(*_compIter)->Remove();
		delete((*_compIter));
		_compIter = m_Component_V.erase(_compIter);
	}

	//for (GameObject* _child : m_Children_V)
	//{
	//	_child->Release();
	//}
}

void GameObject::Reset()
{

}

//void ObjectBase::DeregisterAllComponents(ComponentSystem* system)
//{
//	std::vector<ComponentBase*>::iterator _CompVIter = m_Component_V.begin();
//
//	for (; _CompVIter!= m_Component_V.end(); _CompVIter++)
//	{
//		//system->DeregisterComponent((*_CompVIter)->GetComponetId(), *_CompVIter);
//	}
//}

void GameObject::OnEnable()
{
	m_bEnable = true;
}

void GameObject::OnDisable()
{
	m_bEnable = false;
}

bool GameObject::GetHasPhysics() const
{
	return m_bPhysics;
}

void GameObject::SetHasPhysics(bool val)
{
	m_bPhysics = val;
}

bool GameObject::GetIsEnabled()
{
	return m_bEnable;
}

bool GameObject::GetIsContinualObject()
{
	return m_bContinual;
}

const unsigned int& GameObject::GetGameObjectId()
{
	return m_GameObjectId;
}

GameObject* GameObject::GetParent() const
{
	return m_pParent;
}

void GameObject::SetParent(GameObject* pObject)
{
	if (m_pParent == nullptr)
	{
		m_pParent = pObject;
		pObject->AddChildObject(this);
	}
	else
	{
		Trace("This already has parent!");
	}
}

const size_t GameObject::GetComponentCnt()
{
	return m_Component_V.size();
}

//void ObjectBase::RegisterComponentToSystem(ComponentBase* component)
//{
//	ComponentSystem::GetInstance()->RegisterComponent(component->GetComponetId(), component);
//}

void GameObject::SetObjectName(const std::string& name)
{
	m_Name = name;
}

const std::string& GameObject::GetObjectName()
{
	return m_Name;
}

void GameObject::SetObjectFileName(const std::string& name)
{
	m_fileName_V.push_back(name);
}

const std::string& GameObject::GetObjectFileName(int idx)
{
	return m_fileName_V[idx];
}

void GameObject::AddChildObject(GameObject* object)
{
	object->m_pParent = this;
	m_Children_V.push_back(object);
}

void GameObject::Save()
{
	m_Value = new Json::Value();

	(*m_Value)[EraseClass(typeid(*this).name()).c_str()]["m_Name"] = m_Name;
	(*m_Value)[EraseClass(typeid(*this).name()).c_str()]["m_GameObjectId"] = m_GameObjectId;
	(*m_Value)[EraseClass(typeid(*this).name()).c_str()]["m_bEnable"] = m_bEnable;
	(*m_Value)[EraseClass(typeid(*this).name()).c_str()]["m_bContinual"] = m_bContinual;
	(*m_Value)[EraseClass(typeid(*this).name()).c_str()]["m_pParent"] = m_pParent ? m_pParent->GetGameObjectId() : NULL;


	for (ComponentBase* _component : m_Component_V)
	{
		_component->Save();
		(*m_Value)[EraseClass(typeid(*this).name()).c_str()]["m_Component_V"].append(*(_component->GetValue()));
		//(*m_Value)[EraseClass(typeid(*this).name() + std::to_string(m_GameObjectId)).c_str()]["m_Component_V"].append(_component->GetValue());
	}

	///boost/json ver
	//m_Value->emplace_object()[EraseClass(typeid(*this).name())].emplace_object()["m_Name"] = m_Name;
	//m_Value->emplace_object()[EraseClass(typeid(*this).name())].emplace_object()["m_GameObjectId"] = m_GameObjectId;
	//m_Value->emplace_object()[EraseClass(typeid(*this).name())].emplace_object()["m_bEnable"] = m_bEnable;
	//m_Value->emplace_object()[EraseClass(typeid(*this).name())].emplace_object()["m_bContinual"] = m_bContinual;

	//for (ComponentBase* _component : m_Component_V)
	//{
	//	_component->Save();
	//	//(*m_Value)["GameObject"]["m_Component_V"].append(*(_component->GetValue()));
	//	m_Value->emplace_object()[EraseClass(typeid(*this).name())].emplace_object()
	//		["m_Component_V"].emplace_array().push_back(_component->GetValue()->as_object());
	//}

}

void GameObject::Load()
{
	m_Name = (*m_Value)[EraseClass(typeid(*this).name()).c_str()]["m_Name"].asString();
	m_bEnable = (*m_Value)[EraseClass(typeid(*this).name()).c_str()]["m_bEnable"].asBool();

	//std::vector<std::string> _all_keys = (*m_Value)["GameObject"].getMemberNames();
	//for (auto _key : _all_keys)
	//{
	//	//std::string a = typeid((*m_Value)["GameObject"]).name();
	//	if (_key == "m_Name")
	//	{
	//		m_Name = (*m_Value)["GameObject"][_key].asUInt();
	//	}
	//	if (_key == "m_GameObjectId")
	//	{
	//		//m_GameObjectId = (*m_Value)["GameObject"][_key].asUInt();
	//	}
	//	if (_key == "m_bEnable")
	//	{
	//		m_bEnable = (*m_Value)["GameObject"][_key].asBool();
	//	}
	//	if (_key == "m_bContinual")
	//	{
	//		m_bEnable = (*m_Value)["GameObject"][_key].asBool();
	//	}
	//	if (_key == "m_Component_V")
	//	{
	//		Json::ValueIterator _it = (*m_Value)["GameObject"]["m_Component_V"].begin();
	//		for (; _it != (*m_Value)["GameObject"]["m_Component_V"].end(); _it++)
	//		{
	//			Json::Value* _value = &(*_it);
	//			///���⼭ ������Ʈ �����ؾ� �ϴµ� ��¼����?
	//			///������Ʈ�� �������� �����ڵ� �ٸ��� �̰ͱ��� ������ © ���� �����ϴ�.
	//		}
	//	}
	//}

}

void GameObject::LoadPtrData(std::map<unsigned int, GameObject*>* gameobject, std::map<unsigned int, ComponentBase*>* component)
{
	unsigned int _tempId = (*m_Value)[EraseClass(typeid(*this).name()).c_str()]["m_pParent"].asUInt();

	if (_tempId != NULL)
	{
		m_pParent = gameobject->at(_tempId);
	}
}
