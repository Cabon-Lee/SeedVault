#include "pch.h"
#include "IComponent.h"
#include "GameObject.h"

#include <typeinfo>

IComponent::IComponent()
	: m_bEnable(true)
	, m_Type(ComponentType::Etc)
	, m_ComponentId(0)
	, m_pMyObject(nullptr)
{
}

IComponent::IComponent(ComponentType type)
	: m_bEnable(true)
	, m_Type(type)
	, m_ComponentId(SetComponentId())
	, m_pMyObject(nullptr)
{
	///������Ʈ�� ������Ʈ�� ��ϵɶ� �ý��ۿ� ��ϵȴ�.
	//CA_TRACE("Component System : Register %d", m_ComponentId);
	//ComponentSystem::GetInstance()->RegisterComponent(static_cast<int>(type), this);
}

IComponent::~IComponent()
{
}

void IComponent::Remove()
{
	//����ߴ� �ڽ��� �����ؾ� �Ѵ�.
	//CA_TRACE("Component System : Deregister %d", m_ComponentId);
	//ComponentSystem::GetInstance()->DeregisterComponent(static_cast<int>(m_Type), this);
}

void IComponent::OnEnable()
{
	m_bEnable = true;
}

void IComponent::OnDisable()
{
	m_bEnable = false;
}

bool IComponent::GetIsEnabled()
{
	return m_bEnable;
}

unsigned int IComponent::SetComponentId()
{
	static unsigned int s_ComponentCnt = 0;

	//dll������ Ŭ���̾�Ʈ������ �����Ϳ����� �޶� ���ϴ� ��� �������� �ʴ´�. ���� ��������
	std::string _compId = "3";		//Scene 1, Object 2 Component 3

	_compId = _compId
/*		+ std::to_string(static_cast<unsigned int>(m_Type))
		+ std::to_string(++s_ComponentCnt)*/;

	return std::stoul(_compId.c_str(), nullptr, 10);
}

const unsigned int IComponent::GetComponetId()
{
	return m_ComponentId;
}

const int IComponent::GetComponentType()
{
	return static_cast<int>(m_Type);
}

void IComponent::SetMyObject(GameObject* object)
{
	m_pMyObject = object;
}

bool IComponent::GetObjectIsEnabled()
{
	return m_pMyObject->GetIsEnabled();
}

bool IComponent::GetObjectIsExist()
{
	return (m_pMyObject!=nullptr) ? true : false;
}
