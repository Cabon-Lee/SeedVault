#pragma once
#include <vector>
#include <map>
#include "GameObject.h"	

#include "JsonSerializer.h"

#include <memory>

#define IDCOMPONENT 3

/// <summary>
/// Component /// 유니티에서 MonoBehavior에 해당하는 클래스로 만들어보고싶다.
/// </summary>

//다른 곳에서 캐스팅해서 상수로 이루어진 식이 많으니 함부로 바꾸지 말아주세요.
enum class ComponentType
{
	Rendering = -1,
	Light = -2,

	Physics = 1,
	Input = 2,
	GameLogic = 3,
	Etc = 4,
	Camera = 5,

	MeshFilter = 6,
	Animation,
	Particle,
};

class GameObject;
class ComponentSystem;

class Transform;
class PhysicsActor;
class Collision;

class ComponentBase : public JsonSerializer
{
private:
	ComponentBase();
public:
	ComponentBase(ComponentType type);
	virtual ~ComponentBase();

public:
	virtual void Start() abstract;							//Start는 Update 메서드가 처음 호출되기 직전, 스크립트가 활성화 되면 호출되어야 한다. 
	
	virtual void OnCollisionEnter(Collision collision);	// physX Actor 컴포넌트가 있다면 호출하자.
	virtual void OnCollisionStay(Collision collision);		// physX Actor 컴포넌트가 있다면 호출하자.
	virtual void OnCollisionExit(Collision collision);		// physX Actor 컴포넌트가 있다면 호출하자.
	
	virtual void Update(float dTime) abstract;				//fixedUpdate(고정프레임)과 구별지어야 하지 않을까?

	virtual void OnPreRender();								// 모든 컴포넌트가 들고 있을 필요가 없어서 abstract를 안붙임
	virtual void OnRender() abstract;
	virtual void OnPostRender();
	virtual void OnHelperRender();
	virtual void OnUIRender();

	void Remove();

	virtual void OnEnable();
	virtual void OnDisable();
	void SetEnable(const bool& enalble);

	bool GetIsEnabled();
private:
	bool m_bEnable;


protected:
	const ComponentType m_Type;
	const unsigned int m_ComponentId;

public:
	const unsigned int GetComponetId();
	const int GetComponentType();

	//static unsigned int s_ComponentCnt;			
public:
	void SetMyObject(GameObject* object);
	bool GetObjectIsEnabled();
	bool GetObjectIsExist();
	GameObject* GetMyObject();
protected:
	GameObject* m_pMyObject; // 자신을 들고있는 오브젝트를 알아야함 예) physics에서 transform 접근할때

public:
	template <typename T>
	T* GetComponent();
	Transform* m_Transform;
	Transform* GetTransform()
	{
		if (m_pMyObject != nullptr)
		{
			return m_pMyObject->GetTransform();
		}
		else
		{
			//CA_TRACE("Component : No Object Component", m_ComponentId);
			return nullptr;
		}
	}

	// 피직스 액터
	// -1이면 피직스액터 컴포넌트가 없음.
	// 0 이상이면 해당 userIndex(unserData)에 해당하는 피직스액터를 가지고 있음
	int m_PhysicsActorUserIndex = -1;

	///Json
	virtual void Save() final;
	virtual void Load();

	//virtual void SaveData() abstract;		///완전히 저장할 준비가 되면 추상으로 바꿀것입니다.
	virtual void SaveData();
	virtual void LoadData();
	virtual void LoadPtrData(std::map<unsigned int, GameObject*>* gameobject, std::map<unsigned int, ComponentBase*>* component);

};

template <typename T>
T* ComponentBase::GetComponent()
{
	if (m_pMyObject != nullptr)
	{
		return m_pMyObject->template GetComponent<T>();
	}

	return nullptr;
}

struct Component_Save 
{
	bool			m_bEnable;
	unsigned int	m_ComponentId;
};

BOOST_DESCRIBE_STRUCT(Component_Save, (), (
	m_bEnable,
	m_ComponentId
	))