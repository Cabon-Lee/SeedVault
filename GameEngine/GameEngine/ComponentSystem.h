#pragma once
/// <summary>
/// 컴포넌트의 관리를 맡는 Class 
/// </summary>
/// 
#include "Singleton.h"
#include <memory>
#include <map>
#include <vector>
#include <functional>

//#define PHYSICS 0
//#define ETC 1
#define RENDERING -1


class ComponentBase;
class GameObject;
class ResourceLoader;
class ResourceObserverOwner;
class ResourceObserver;
__interface IResourceManager;

class ComponentSystem: public Singleton<ComponentSystem>
{
public:
	ComponentSystem();
	~ComponentSystem();

public:
	void RegisterComponent(int type, ComponentBase* component);
	void DeregisterComponent(GameObject* object);
	void DeregisterComponent(int type, ComponentBase* component);

	void RegisterObserver(ResourceObserver* component);
	void SetResourceLoader(ResourceLoader* pResourceLoader);
	void SetResourceManager(std::shared_ptr<IResourceManager> pResourceManager);

	const std::vector<ComponentBase*>& GetRenderComponent();

private:
	void DeregisterComponent_NoOption(int type, ComponentBase* component);
	void RegisterComponent_NoOption(int type, ComponentBase* component);
private:
	std::vector<std::vector<std::pair<int , ComponentBase*>>> m_Component_V;		//업데이트를 원하는 순서대로 삽입해야 한다, std::pair<type, ComponentBase*>
	std::vector<ComponentBase*> m_RenderComponent_V;
	std::vector<ComponentBase*> m_LightComponent_V;
	std::vector<ComponentBase*> m_PhysicsActorComponent_V;

	std::shared_ptr<ResourceObserverOwner> m_pResourceObserverOwner;

	std::shared_ptr<IResourceManager> m_pResourceManager;
	ResourceLoader* m_pResourceLoader;
	//std::multimap<int, ComponentBase*> m_Component_MM;

	bool m_IsPaused;
	bool m_IsRunning;		
	bool m_IsDebugRenderMode;	// 디버그 정보 렌더할것인가?(피직스 액터, 네비메쉬 등)

public:
	void Start();
	void Update(float dTime);
	void OnCollisionEnter();
	void OnCollisionStay();
	void OnCollisionExit();
	void ClearCollisionList();
	void LightPreRender();
	void ReflectionProbeRender();
	void PreRender();
	void Render();
	void PostRender();
	void HelperRender();
	void UIRender();

	void Update_Pause();		//완전 일시정지
	void Update_Restrat();	
	const bool& GetIsPaused();

	// Added By 최 요 환
	_DLL bool GetDebugRenderMode() const;
	_DLL void SetDebugRenderMode(const bool& val);

	_DLL class PhysicsActor* FindPhysicsActor(int userIndex) const;
	bool HasGameLogicScriptComponent(class PhysicsActor* actor) const;

private:
	void UpdateByType(float dTime, int type);
	void Start(ComponentBase* component);

private:
	std::vector<std::pair<int, ComponentBase*>> m_TempRegisterComponent_V;		//업데이트중 생성/삽입된 컴포넌트를 임시로 보관하는 곳

};

