#include "pch.h"
#include "ComponentSystem.h"
#include "ComponentBase.h"

#include "Managers.h"
#include "Camera.h"
#include "ResourceLoader.h"
#include "MeshFilter.h"
#include "Animation.h"
#include "SkinningAnimation.h"

#include "PhysicsActor.h"

#include "ResourceObserverOwner.h"
#include "IResourceManager.h"

ComponentSystem::ComponentSystem()
	: m_IsPaused(false)
	, m_IsRunning(false)
	, m_IsDebugRenderMode(false)
{
	/// <summary>
	/// 순서제어가 필요한 타입수가 10개 이내일 것이다.
	/// </summary>
	m_Component_V.resize(10);
	m_pResourceObserverOwner = std::make_shared<ResourceObserverOwner>();
}

ComponentSystem::~ComponentSystem()
{

}

void ComponentSystem::RegisterComponent(int type, ComponentBase* component)
{
	///update중에는 등록을 지연시킨다.
	if (m_IsRunning == true)
	{
		m_TempRegisterComponent_V.push_back(std::make_pair(type, component));
		return;
	}

	RegisterComponent_NoOption(type, component);

	// Observer를 상속받은 클래스인지 내부에서 확인한다
	// Observer 생성자 내에서 하기로 했다.
	//m_pResourceObserverOwner->RegisterObserver(component);

	/// <summary>
	/// multimap ver
	/// </summary> 
	/// <param name="type"></param>
	/// <param name="component"></param>
	//m_Component_MM.insert(std::make_pair(type, component));

	//////////////////////////////////////////////////////////////////////////
}

void ComponentSystem::RegisterComponent_NoOption(int type, ComponentBase* component)
{
	///2d vector ver
	///type번째 vector에 삽입한다.
	/// render component는 따로 관리
	if (type == static_cast<int>(ComponentType::Rendering))
	{
		m_RenderComponent_V.push_back(component);
	}
	else if (type == static_cast<int>(ComponentType::Physics))
	{
		m_PhysicsActorComponent_V.push_back(component);
	}
	else if (type == static_cast<int>(ComponentType::Light))
	{
		m_LightComponent_V.push_back(component);
	}
	else if (type == static_cast<int>(ComponentType::Camera))
	{
		Managers::GetInstance()->GetCameraManager()->AddCamera(dynamic_cast<Camera*>(component), dynamic_cast<Camera*>(component)->GetName());
		m_Component_V.at(type).push_back(std::make_pair(type, component));
	}
	else
	{
		m_Component_V.at(type).push_back(std::make_pair(type, component));
	}

	CA_TRACE("Component System : Register %u", component->GetComponetId());
}

void ComponentSystem::DeregisterComponent_NoOption(int type, ComponentBase* component)
{
	if (type == static_cast<int>(ComponentType::Rendering))
	{
		std::vector<ComponentBase*>::iterator _rCompV = m_RenderComponent_V.begin();
		for (; _rCompV != m_RenderComponent_V.end(); _rCompV++)
		{
			if (*_rCompV == component)
			{
				m_RenderComponent_V.erase(_rCompV);
				CA_TRACE("Component System : Deregister %u", component->GetComponetId());
				return;
			}
		}
	}

	if (type == static_cast<int>(type == static_cast<int>(ComponentType::Physics)))
	{
		std::vector<ComponentBase*>::iterator _rCompV = m_PhysicsActorComponent_V.begin();
		for (; _rCompV != m_PhysicsActorComponent_V.end(); _rCompV++)
		{
			if (*_rCompV == component)
			{
				m_PhysicsActorComponent_V.erase(_rCompV);
				CA_TRACE("Component System : Deregister %u", component->GetComponetId());
				return;
			}
		}
	}

	if (type == static_cast<int>(ComponentType::Light))
	{
		std::vector<ComponentBase*>::iterator _rCompV = m_LightComponent_V.begin();
		for (; _rCompV != m_LightComponent_V.end(); _rCompV++)
		{
			if (*_rCompV == component)
			{
				m_LightComponent_V.erase(_rCompV);
				CA_TRACE("Component System : Deregister %u", component->GetComponetId());
				return;
			}
		}
	}

	std::vector<std::pair<int, ComponentBase*>>::iterator _pairVIter = m_Component_V.at(type).begin();

	for (; _pairVIter != m_Component_V.at(type).end(); _pairVIter++)
	{
		if (_pairVIter->second == component)
		{
			m_Component_V.at(type).erase(_pairVIter);
			CA_TRACE("Component System : Deregister %u", component->GetComponetId());
			return;
		}
	}

	CA_TRACE("Component System : %u Deregister Fail", component->GetComponetId());
}

void ComponentSystem::DeregisterComponent(GameObject* object)
{
	m_IsRunning = false;

	for (ComponentBase*& _comp : object->m_Component_V)
	{
		DeregisterComponent_NoOption(_comp->GetComponentType(), _comp);
	}
}

void ComponentSystem::DeregisterComponent(int type, ComponentBase* component)
{
	m_IsRunning = false;

	DeregisterComponent_NoOption(type, component);
}

void ComponentSystem::RegisterObserver(ResourceObserver* component)
{
	m_pResourceObserverOwner->RegisterObserver(component);
}

void ComponentSystem::SetResourceLoader(ResourceLoader* pResourceLoader)
{
	m_pResourceLoader = pResourceLoader;
}

void ComponentSystem::SetResourceManager(std::shared_ptr<IResourceManager> pResourceManager)
{
	m_pResourceManager = pResourceManager;
	m_pResourceObserverOwner->SetResourceManager(pResourceManager);
}

const std::vector<ComponentBase*>& ComponentSystem::GetRenderComponent()
{
	return m_RenderComponent_V;
}

/// <summary>
/// 등록된 컴포넌트 전체를 Start 
/// </summary>
void ComponentSystem::Start()
{
	m_pResourceObserverOwner->Notify();


	std::vector<ComponentBase*>::iterator _rCompV = m_RenderComponent_V.begin();
	for (; _rCompV != m_RenderComponent_V.end(); _rCompV++)
	{
		(*_rCompV)->Start();
	}

	_rCompV = m_PhysicsActorComponent_V.begin();
	for (; _rCompV != m_PhysicsActorComponent_V.end(); _rCompV++)
	{
		(*_rCompV)->Start();
	}

	_rCompV = m_LightComponent_V.begin();
	for (; _rCompV != m_LightComponent_V.end(); _rCompV++)
	{
		(*_rCompV)->Start();
	}

	for (auto& _componentV : m_Component_V)
	{
		std::vector<std::pair<int, ComponentBase*>>::iterator _pairVIter = _componentV.begin();

		for (; _pairVIter != _componentV.end(); _pairVIter++)
		{
			if (_pairVIter->second->GetObjectIsExist())
			{
				_pairVIter->second->Start();
			}
			else
			{
				//스타트 시점에서 자신의 오브젝트가 없는 컴포넌트는 시스템에서 제외된다.
				CA_TRACE("Component System : %u No Object", _pairVIter->second->GetComponetId());
				_componentV.erase(_pairVIter);
			}
		}
	}

}

/// <summary>
/// 지정된 컴포넌트만 Start
/// </summary>
/// <param name="component"></param>
void ComponentSystem::Start(ComponentBase* component)
{
	m_pResourceObserverOwner->Notify();

	//스타트 시점에서 자신의 오브젝트가 없는 컴포넌트는 시스템에서 제외된다.
	if (!component->GetObjectIsExist())
	{
		CA_TRACE("Component System : %u No Object", component->GetComponetId());

		/// <summary>
		/// reverse iterator
		/// 기존 이터레이터와 역순으로 동작한다.
		/// </summary>
		std::vector<std::pair<int, ComponentBase*>>::reverse_iterator _pairVIter(m_Component_V.at(component->GetComponentType()).rbegin());
		for (; _pairVIter != m_Component_V.at(component->GetComponentType()).rend(); _pairVIter++)
		{
			//삭제하려는 객체와 동일한 객체 삭제한다!
			if (component == _pairVIter->second)
			{
				m_Component_V.at(component->GetComponentType()).erase(--_pairVIter.base());
				break;
			}
		}

		return;
	}

	component->Start();
}

/// <summary>
/// 서브(워크)쓰레드
/// 컴포넌트들을 병렬로 업데이트하기 위한 함수
/// 쓰레드별로 업데이트할 컴포넌트들의 시작 iter와 끝 iter를 받아서 그 사이를 각각 업데이트
/// </summary>
/// <param name="start">시작 컴포넌트 iter</param>
/// <param name="end">끝 컴포넌트 iter</param>
/// <param name="dTime">업데이트할 때 쓸 dTime</param>
void WorkerComponentUpdate(std::vector<std::pair<int, ComponentBase*>>::iterator start,
	std::vector<std::pair<int, ComponentBase*>>::iterator end,
	const float dTime)
{
	for (auto iter = start; iter < end; ++iter)
	{
		if (iter->second->GetObjectIsEnabled() == true)
		{
			if (iter->second->GetIsEnabled() == true)
			{
				iter->second->Update(dTime);
			}
		}
	}

	CA_TRACE("쓰레드 %x", std::this_thread::get_id());
}


void ComponentSystem::Update(float dTime)
{
	m_IsRunning = true;

	/// 일시정지중이면 Update문을 돌지 않는다.
	if (m_IsPaused == false)
	{
		// 피직스액터 업데이트
		for (ComponentBase*& _rComp : m_PhysicsActorComponent_V)
		{
			if (_rComp->GetObjectIsEnabled() == true)
			{
				_rComp->Update(dTime);
			}
		}

		//컴포넌트 타입별 업데이트 순서를 제어해본다.
		for (int _type = 1; _type < m_Component_V.size(); _type++)
		{
			UpdateByType(dTime, _type);
		}

		
		/*
		// Added By Yoking
		// 컴포넌트들을 쓰레드를 사용해 병렬로 업데이트 해보려고 시도해봄..
		// 그러나 잘못 쓰고 있는지 오히려 느려짐... 
		// 아래의 코드는 해결되기 전까지 사용 금지

		// 쓰레드당 할당할 업데이트 컴포넌트 개수
		const unsigned int _assignmentCountForWorkerThread = 10;

		std::vector<std::thread> workers;
		for (size_t i = 0; i < m_Component_V.size(); i++)
		{
			// 생성할 작업 쓰레드 개수
			unsigned int _workerThreadCount = 0;
			int _compCount = m_Component_V.at(i).size();

			//while (_compCount >= _assignmentCountForWorkerThread)
			//{
			//	_compCount -= _assignmentCountForWorkerThread;
			//	_workerThreadCount++;
			//}
			//if (_compCount > 0)
			//{
			//	_workerThreadCount++;
			//}

			_workerThreadCount = m_Component_V.at(i).size() / _assignmentCountForWorkerThread;
			if (m_Component_V.at(i).size() % _assignmentCountForWorkerThread > 0)
			{
				_workerThreadCount++;
			}

			//각각의 타입별 내부 벡터를 쓰레드로 나누어 업데이트한다.			
			_compCount = m_Component_V.at(i).size();
			unsigned int _startIdx = 0;
			unsigned int _endIdx = _startIdx + _assignmentCountForWorkerThread;

			if (_endIdx >= m_Component_V.at(i).size())
			{
				_endIdx = m_Component_V.at(i).size();
			}


			workers.reserve(_assignmentCountForWorkerThread);
			for (size_t j = 0; j < _workerThreadCount; j++)
			{
				workers.push_back(
					std::thread(WorkerComponentUpdate,
						m_Component_V.at(i).begin() + _startIdx,
						m_Component_V.at(i).begin() + _endIdx,
						dTime)
				);

				_startIdx = _endIdx;
				_endIdx += _assignmentCountForWorkerThread;
				if (_endIdx >= m_Component_V.at(i).size())
				{
					_endIdx = m_Component_V.at(i).size();
				}
			}

			for (size_t k = 0; k < workers.size(); k++)
			{
				workers.at(k).join();
			}

			workers.clear();
		}
		//*/
	}


	/// <summary>
	///  update중 register된, deregister된 컴포넌트들을 모아두었다가 해당 update마지막에 Register, Start시킨다.
	/// </summary>
	/// 
	/*for (auto& _Comp : m_TempDeregisterComponent_V)
	{
		DeregisterComponent_NoOption(_Comp.first, _Comp.second);
	}*/

	for (auto& _Comp : m_TempRegisterComponent_V)
	{
		RegisterComponent_NoOption(_Comp.first, _Comp.second);
		Start(_Comp.second);
	}

	m_TempRegisterComponent_V.clear();
}

void ComponentSystem::OnCollisionEnter()
{
	// 게임로직 스크립트 컴포넌트의 경우에
	// physicsActor 컴포넌트가 myObject에 붙어있다면 OnCollisionEnter() 호출
	for (auto& _pairV : m_Component_V.at(static_cast<int>(ComponentType::GameLogic)))
	{
		// 피직스 액터컴포넌트가 붙어있으면
		PhysicsActor* physActor = _pairV.second->GetMyObject()->GetComponent<PhysicsActor>();
		if (physActor != nullptr)
		{
			// 오브젝트/게임로직 스크립트 활성화 확인
			if (_pairV.second->GetObjectIsEnabled() == true &&
				_pairV.second->GetIsEnabled() == true)
			{
				// 충돌(Enter) 된 오브젝트 수 만큼 호출
				std::vector<Collision>& collisionEnterList_V = physActor->m_CollisionEnter_V;
				for (auto& collisionEnterObj : collisionEnterList_V)
				{
					// 스크립트에서 오버라이딩 구현한 충돌 로직 호출
					_pairV.second->OnCollisionEnter(*collisionEnterList_V.begin());
				}
			}
		}
	}

}

void ComponentSystem::OnCollisionStay()
{
	// 게임로직 스크립트 컴포넌트의 경우에
	// physicsActor 컴포넌트가 myObject에 붙어있다면 OnCollisionStay() 호출
	for (auto& _pairV : m_Component_V.at(static_cast<int>(ComponentType::GameLogic)))
	{
		// 피직스 액터컴포넌트가 붙어있으면
		PhysicsActor* physActor = _pairV.second->GetMyObject()->GetComponent<PhysicsActor>();
		if (physActor != nullptr)
		{
			// 오브젝트/게임로직 스크립트 활성화 확인
			if (_pairV.second->GetObjectIsEnabled() == true &&
				_pairV.second->GetIsEnabled() == true)
			{
				// 충돌중(stay)인 오브젝트 수 만큼 호출
				std::vector<Collision>& collisionStayList_V = physActor->m_CollisionStay_V;
				for (auto& collisionStayObj : collisionStayList_V)
				{
					// 스크립트에서 오버라이딩 구현한 충돌 로직 호출
					_pairV.second->OnCollisionStay(*collisionStayList_V.begin());
				}
			}
		}
	}
}

void ComponentSystem::OnCollisionExit()
{
	// 게임로직 스크립트 컴포넌트의 경우에
	// physicsActor 컴포넌트가 myObject에 붙어있다면 OnCollisionExit() 호출
	for (auto& _pairV : m_Component_V.at(static_cast<int>(ComponentType::GameLogic)))
	{
		// 피직스 액터컴포넌트가 붙어있으면
		PhysicsActor* physActor = _pairV.second->GetMyObject()->GetComponent<PhysicsActor>();
		if (physActor != nullptr)
		{
			// 오브젝트/게임로직 스크립트 활성화 확인
			if (_pairV.second->GetObjectIsEnabled() == true &&
				_pairV.second->GetIsEnabled() == true)
			{
				// 충돌해제(Exit) 된 오브젝트 수 만큼 호출
				std::vector<Collision>& collisionExitList_V = physActor->m_CollisionExit_V;
				for (auto& collisionExitObj : collisionExitList_V)
				{
					// 스크립트에서 오버라이딩 구현한 충돌 로직 호출
					_pairV.second->OnCollisionExit(*collisionExitList_V.begin());
				}
			}
		}
	}
}

void ComponentSystem::ClearCollisionList()
{
	// 피직스 액터 컴포넌트의
	// 충돌리스트 클리어
	for (auto& _pairV : m_PhysicsActorComponent_V)
	{
		// 피직스 액터컴포넌트가 붙어있으면
		PhysicsActor* physActor = _pairV->GetMyObject()->GetComponent<PhysicsActor>();
		if (physActor != nullptr)
		{
			physActor->m_CollisionEnter_V.clear();
			physActor->m_CollisionStay_V.clear();
			physActor->m_CollisionExit_V.clear();
		}
	}
}

void ComponentSystem::LightPreRender()
{

	for (ComponentBase*& _rComp : m_RenderComponent_V)
	{
		if (_rComp->GetObjectIsEnabled() == true)
		{
			_rComp->OnPreRender();
		}
	}

}


void ComponentSystem::ReflectionProbeRender()
{
	for (ComponentBase*& _rComp : m_RenderComponent_V)
	{
		if (_rComp->GetObjectIsEnabled() == true)
		{
			_rComp->OnPreRender();
		}
	}
}

void ComponentSystem::PreRender()
{
	// 라이트의 PreRender를 돌린다
	for (ComponentBase*& _rComp : m_LightComponent_V)
	{
		if (_rComp->GetObjectIsEnabled() == true)
		{
			_rComp->OnPreRender();
		}
	}
}

void ComponentSystem::Render()
{
	/// <summary>
	/// 수종 요청 : 카메라도 Render를 돌았으면 좋겠다.
	/// </summary>
	for (auto& _camera : m_Component_V.at(static_cast<int>(ComponentType::Camera)))
	{
		if (_camera.second->GetObjectIsEnabled() == true)
		{
			_camera.second->OnRender();
		}
	}

	for (ComponentBase*& _rComp : m_RenderComponent_V)
	{
		if (_rComp->GetObjectIsEnabled() == true)
		{
			_rComp->OnRender();
		}
	}
}

void ComponentSystem::PostRender()
{
	// 라이트의 PreRender를 돌린다
	for (ComponentBase*& _rComp : m_RenderComponent_V)
	{
		if (_rComp->GetObjectIsEnabled() == true)
		{
			_rComp->OnPostRender();
		}
	}
}

void ComponentSystem::HelperRender()
{
	for (ComponentBase*& _rComp : m_RenderComponent_V)
	{
		if (_rComp->GetObjectIsEnabled() == true)
		{
			_rComp->OnHelperRender();
		}
	}

	for (ComponentBase*& _rComp : m_PhysicsActorComponent_V)
	{
		if (_rComp->GetObjectIsEnabled() == true)
		{
			_rComp->OnHelperRender();
		}
	}
}

void ComponentSystem::UIRender()
{
	for (ComponentBase*& _rComp : m_RenderComponent_V)
	{
		if (_rComp->GetObjectIsEnabled() == true)
		{
			_rComp->OnUIRender();
		}
	}
}

/// <summary>
/// Update문을 멈춘다.
/// </summary>
void ComponentSystem::Update_Pause()
{
	m_IsPaused = true;
}

void ComponentSystem::Update_Restrat()
{
	m_IsPaused = false;
}

const bool& ComponentSystem::GetIsPaused()
{
	return m_IsPaused;
}

_DLL bool ComponentSystem::GetDebugRenderMode() const
{
	return m_IsDebugRenderMode;
}

_DLL void ComponentSystem::SetDebugRenderMode(const bool& val)
{
	m_IsDebugRenderMode = val;
}

/// <summary>
/// userIndex 번호에 해당하는 피직스액터 컴포넌트를 찾아서 리턴
/// </summary>
/// <param name="userIndex">유저 인덱스</param>
/// <returns></returns>
PhysicsActor* ComponentSystem::FindPhysicsActor(int userIndex) const
{
	// 피직스 액터 벡터 순회
	for (int i = 0; i < m_PhysicsActorComponent_V.size(); i++)
	{
		PhysicsActor* physicsActor = dynamic_cast<PhysicsActor*>(m_PhysicsActorComponent_V.at(i));

		// 유저 인덱스 비교
		if (physicsActor->GetUserIndex() == userIndex)
		{
			// 인덱스가 일치하면 해당 컴포넌트 리턴
			return physicsActor;
		}
	}

	// 해당하는 인덱스를 가진 피직스 엑터가 없음.
	return nullptr;
}

bool ComponentSystem::HasGameLogicScriptComponent(PhysicsActor* actor) const
{
	GameObject* gameObject = actor->GetMyObject();

	// 게임 오브젝트의 컴포넌트를 순회하면서 게임 로직 컴포넌트가 있는지 찾는다.
	for (auto component : gameObject->m_Component_V)
	{
		if (component->GetComponentType() == static_cast<int>(ComponentType::GameLogic))
		{
			// 게임 로직 컴포넌트가 붙어있다!
			return true;
		}
	}

	// 게임 로직 컴포넌트가 없다!
	return false;
}

void ComponentSystem::UpdateByType(float dTime, int type)
{
	///2d vector ver
	
	for (auto& _pairV : m_Component_V.at(type))
	{
		if (_pairV.second->GetObjectIsEnabled() == true)
		{
			if (_pairV.second->GetIsEnabled() == true)
			{
				_pairV.second->Update(dTime);
			}
		}
	}

	/// <summary>
	/// multimap ver
	/// </summary>
	/// <param name="type"></param>
	/*std::multimap<int, ComponentBase*>::iterator _componentIter;

	for (_componentIter = m_Component_MM.equal_range(type).first;
		_componentIter != m_Component_MM.equal_range(type).second;
		_componentIter++)
	{
		if (_componentIter->second->GetObjectIsEnabled() == true)
		{
			if (_componentIter->second->GetIsEnabled() == true)
			{
				_componentIter->second->Update(dTime);
			}
		}
	}*/

	//////////////////////////////////////////////////////////////////////////
}
