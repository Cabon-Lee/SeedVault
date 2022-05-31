#include "pch.h"
#include "IdDll.h"
#include "GameObject.h"

#include "ComponentBase.h"
#include <typeinfo>


ComponentBase::ComponentBase()
	:
	m_bEnable(true)
	, m_Type(ComponentType::Etc)
	, m_ComponentId(0)
	, m_Transform(nullptr)
	, m_pMyObject(nullptr)
{
}

ComponentBase::ComponentBase(ComponentType type)
	:
	m_bEnable(true)
	, m_Type(type)
	, m_ComponentId(ID::GetIdByType(IDCOMPONENT, static_cast<unsigned int>(type)))
	, m_Transform(nullptr)
	, m_pMyObject(nullptr)
{
	///������Ʈ�� ������Ʈ�� ��ϵɶ� �ý��ۿ� ��ϵȴ�.
	//CA_TRACE("Component System : Register %d", m_ComponentId);
	//ComponentSystem::GetInstance()->RegisterComponent(static_cast<int>(type), this);
}

ComponentBase::~ComponentBase()
{
}

void ComponentBase::OnCollisionEnter(Collision collision)
{
}

void ComponentBase::OnCollisionStay(Collision collision)
{
}

void ComponentBase::OnCollisionExit(Collision collision)
{
}

void ComponentBase::OnPreRender()
{
	// �ƹ��͵� ���� �ʴ´�
}

void ComponentBase::OnPostRender()
{

}

void ComponentBase::Remove()
{
	//����ߴ� �ڽ��� �����ؾ� �Ѵ�.
	//CA_TRACE("Component System : Deregister %d", m_ComponentId);
	//ComponentSystem::GetInstance()->DeregisterComponent(static_cast<int>(m_Type), this);
}

void ComponentBase::OnEnable()
{
	m_bEnable = true;
}

void ComponentBase::OnDisable()
{
	m_bEnable = false;
}

void ComponentBase::SetEnable(const bool& enalble)
{
	m_bEnable = enalble;
}

bool ComponentBase::GetIsEnabled()
{
	return m_bEnable;
}

const unsigned int ComponentBase::GetComponetId()
{
	return m_ComponentId;
}

const int ComponentBase::GetComponentType()
{
	return static_cast<int>(m_Type);
}

void ComponentBase::SetMyObject(GameObject* object)
{
	m_pMyObject = object;
}

bool ComponentBase::GetObjectIsEnabled()
{
	return m_pMyObject->GetIsEnabled();
}

bool ComponentBase::GetObjectIsExist()
{
	return (m_pMyObject != nullptr) ? true : false;
}

GameObject* ComponentBase::GetMyObject()
{
	return m_pMyObject;
}

void ComponentBase::Save()
{
	m_Value = new Json::Value();
	
	SaveData();
}

void ComponentBase::Load()
{
	LoadData();
}

void ComponentBase::SaveData()
{
   ///���������� �ӽ�
}

void ComponentBase::LoadData()
{
   ///���������� �ӽ�
}

void ComponentBase::LoadPtrData(std::map<unsigned int, GameObject*>* gameobject, std::map<unsigned int, ComponentBase*>* component)
{
	///���������� �ӽ�
}
