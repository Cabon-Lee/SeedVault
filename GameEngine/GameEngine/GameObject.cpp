#include "pch.h"
#include "ComponentBase.h"
#include "IdDll.h"
#include "EngineDLL.h"
#include "PhysicsActor.h"
#include "GameObject.h"
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
	, m_Tag("Untagged")
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
	, m_Tag("Untagged")
	, m_pParent(nullptr)
	, m_Children_V()
{
}

GameObject::GameObject(std::string name, std::string tag, bool isContinual)
	: m_bEnable(true)
	, m_bContinual(isContinual)
	, m_bPhysics(false)
	, m_GameObjectId(ID::GetIdByType(IDGAMEOBJECT, 0))
	, m_Transform(nullptr)
	, m_Component_V()
	, m_Name(name)
	, m_Tag(tag)
	, m_pParent(nullptr)
	, m_Children_V()
{
	DLLEngine::SetTag(tag.c_str(), this);
}

GameObject::~GameObject()
{

}

void GameObject::Update(float dTime)
{

}

void GameObject::Release()
{
	/// <summary>
	//DeregisterAllComponents(ComponentSystem::GetInstance());

	/// 오브젝트 매니저에서 컴포넌트 등록해제를 해준후 컴포넌트들을 해제한다.
	/// </summary>
	//std::vector<ComponentBase*>::iterator _compIter = m_Component_V.begin();
	//for (; _compIter != m_Component_V.end();)
	//{
	//	(*_compIter)->Remove();
	//	delete((*_compIter));
	//	_compIter = m_Component_V.erase(_compIter);
	//} 

	for (size_t _comp = 0; _comp < m_Component_V.size(); ++_comp)
	{
		m_Component_V.at(_comp)->Remove();
		delete(m_Component_V.at(_comp));
	}

	//m_Component_V.clear();
}

void GameObject::Reset()
{

}

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

void GameObject::SetTagName(const std::string& tag)
{
	m_Tag = tag;
}

const std::string& GameObject::GetTag()
{
	return m_Tag;
}

void GameObject::SetObjectName_char(const char* name)
{
	m_Name = std::string(name);
}

const char* GameObject::GetObjectName_char()
{
	return m_Name.c_str();
}

const char* GameObject::GetTag_char()
{
	return m_Tag.c_str();
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

bool GameObject::AddComponentBase(ComponentBase* comp, const type_info& typeinfo)
{
	ComponentBase* _component = comp;
	if (_component == nullptr)
	{
		return false;
	}

	//한 오브젝트 내에서 중복된 컴포넌트를 삽입을 막는다.
	const type_info& _infoT = typeinfo;
	for (ComponentBase* co : m_Component_V)
	{
		const type_info& _info = typeid(*co);

		if (typeid(*co) == typeinfo)
		{
			return false;
		}
	}

	m_Component_V.push_back(_component);
	comp->SetMyObject(this);

	if (dynamic_cast<Transform*>(comp) != nullptr)
	{
		this->m_Transform = dynamic_cast<Transform*>(comp);
	}

	comp->m_Transform = this->m_Transform;

	///오브젝트에 컴포넌트가 등록될 때 컴포넌트는 컴포넌트 시스템에 등록된다.
	this->RegisterComponent(comp);
	return true;
}

void GameObject::Save()
{
	m_Value = new Json::Value();

	(*m_Value)[EraseClass(typeid(*this).name()).c_str()]["m_Name"] = m_Name;
	(*m_Value)[EraseClass(typeid(*this).name()).c_str()]["m_GameObjectId"] = m_GameObjectId;
	(*m_Value)[EraseClass(typeid(*this).name()).c_str()]["m_Tag"] = m_Tag;
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

	std::string _tempTag = (*m_Value)[EraseClass(typeid(*this).name()).c_str()]["m_Tag"].asString();
	if (_tempTag =="")
	{
		_tempTag = "Untagged";
	}
	DLLEngine::SetTag(_tempTag.c_str(), this);
}

void GameObject::LoadPtrData(std::map<unsigned int, GameObject*>* gameobject, std::map<unsigned int, ComponentBase*>* component)
{
	unsigned int _tempId = (*m_Value)[EraseClass(typeid(*this).name()).c_str()]["m_pParent"].asUInt();

	if (_tempId != NULL)
	{
		m_pParent = gameobject->at(_tempId);
	}
}

void GameObject::RegisterComponent(ComponentBase* component)
{
	DLLEngine::RegisterComponent(component);
	//Managers::GetInstance()->GetComponentSystem()->RegisterComponent(component->GetComponentType(), component);

	// Modified by 최 요 환
	// physicsActor 컴포넌트 추가시 physx 씬에 자동으로 등록하기 위함
	PhysicsActor* physicsActor = dynamic_cast<PhysicsActor*>(component);
	if (physicsActor != nullptr)
	{
		DLLEngine::AddPhysicsActorToPhysEngine(physicsActor);
	}
}

