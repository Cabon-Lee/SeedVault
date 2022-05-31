#pragma once
#include <typeinfo>
#include <type_traits>

#include <string>
#include <vector>

#include "JsonSerializer.h"
#define IDGAMEOBJECT 2

class ComponentBase;
class Transform;


class GameObject : public JsonSerializer
{
public:
	GameObject();
	GameObject(bool isContinual);
	GameObject(std::string name, std::string tag, bool isContinual);
public:
	virtual ~GameObject();

public:
	virtual void Update(float dTime);
	virtual void Release();
	virtual void Reset();

	void OnEnable();
	void OnDisable();

	// physics test
	bool GetHasPhysics() const;
	void SetHasPhysics(bool val);

	bool GetIsEnabled();
	bool GetIsContinualObject();
	const unsigned int& GetGameObjectId();

	GameObject* GetParent() const;
	void SetParent(GameObject* pObject);

private:
	bool m_bEnable;
	const bool m_bContinual;
	bool m_bPhysics;

private:
	const unsigned int m_GameObjectId;

public:
	const size_t GetComponentCnt();
	template <typename T>
	bool AddComponent(T* component);
	template <typename T>
	T* GetComponent();

	/// 숏컷을 위해서 헤더에 선언하였습니다. 봐주세요.
	Transform* m_Transform;
	Transform* GetTransform()
	{
		if (m_Transform != nullptr)
		{
			//CA_TRACE("GameObject : No Transform Component");
		}
		return m_Transform;
	}



public:
	//void DeregisterAllComponents(ComponentSystem* system);		//릴리즈 할때 해줘야 한다.
	//void RegisterComponentToSystem(ComponentBase* component);
	std::vector<ComponentBase*> m_Component_V;

public:
	void SetObjectName(const std::string& name);
	const std::string& GetObjectName();
	const std::string& GetTag();

	void SetObjectName_char(const char* name);
	const char* GetObjectName_char();
	const char* GetTag_char();

	void SetTagName(const std::string& tag);		///절대 외부에서 사용하면 안된다! 오직 오브젝트매니저에서만 사용해라.
private:
	std::string m_Name;
	std::string m_Tag;

public:
	void SetObjectFileName(const std::string& name);
	const std::string& GetObjectFileName(int idx);
private:
	std::vector<std::string> m_fileName_V;

public:
	void AddChildObject(GameObject* object);
	bool AddComponentBase(ComponentBase* comp, const type_info& typeinfo);
private:
	GameObject* m_pParent;
	std::vector <GameObject*> m_Children_V; // 추후 계층구조에 쓸것같아서 임시로 만듬

public:
	virtual void Save();
	virtual void Load();
	virtual void LoadPtrData(std::map<unsigned int, GameObject*>* gameobject, std::map<unsigned int, ComponentBase*>* component);

	/// <summary>
	/// new ObjectBase()하길 원하지 않는다. 
	/// ObjectManger에서 오브젝트를 생성하는 역할을 대신 해주기 위해 
	/// 생성자를 private로 오브젝트 매니저를 friend class로 지정해주었다.
	////이거 왜 삭제했는지 기억이 안난다.
	/// </summary>

private: 
	void RegisterComponent(ComponentBase* component);
	//void DeregisterComponent(ComponentBase* component);
};

template <typename T>
bool GameObject::AddComponent(T* component)
{
	ComponentBase* _component = dynamic_cast<ComponentBase*>(component);
	if (_component == nullptr)
	{
		return false;
	}

	//한 오브젝트 내에서 중복된 컴포넌트를 삽입을 막는다.
	const type_info& _infoT = typeid(T);
	for (ComponentBase* co : m_Component_V)
	{
		const type_info& _info = typeid(*co);

		if (typeid(*co) == typeid(T))
		{
			return false;
		}
	}

	m_Component_V.push_back(_component);
	component->SetMyObject(this);

	if (dynamic_cast<Transform*>(component) != nullptr)
	{
		this->m_Transform = dynamic_cast<Transform*>(component);
	}

	component->m_Transform = this->m_Transform;

	///오브젝트에 컴포넌트가 등록될 때 컴포넌트는 컴포넌트 시스템에 등록된다.
	this->RegisterComponent(component);
	return true;
}

template <typename T>
T* GameObject::GetComponent()
{
	const type_info& _infoT = typeid(T);
	for (ComponentBase* co : m_Component_V)
	{
		const type_info& _info = typeid(*co);

		if (typeid(*co) == typeid(T))
		{
			return dynamic_cast<T*> (co);
		}
	}
	return nullptr;
}

