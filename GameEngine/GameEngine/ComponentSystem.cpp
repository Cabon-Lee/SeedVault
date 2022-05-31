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
	/// ������� �ʿ��� Ÿ�Լ��� 10�� �̳��� ���̴�.
	/// </summary>
	m_Component_V.resize(10);
	m_pResourceObserverOwner = std::make_shared<ResourceObserverOwner>();
}

ComponentSystem::~ComponentSystem()
{

}

void ComponentSystem::RegisterComponent(int type, ComponentBase* component)
{
	///update�߿��� ����� ������Ų��.
	if (m_IsRunning == true)
	{
		m_TempRegisterComponent_V.push_back(std::make_pair(type, component));
		return;
	}

	RegisterComponent_NoOption(type, component);

	// Observer�� ��ӹ��� Ŭ�������� ���ο��� Ȯ���Ѵ�
	// Observer ������ ������ �ϱ�� �ߴ�.
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
	///type��° vector�� �����Ѵ�.
	/// render component�� ���� ����
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
/// ��ϵ� ������Ʈ ��ü�� Start 
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
				//��ŸƮ �������� �ڽ��� ������Ʈ�� ���� ������Ʈ�� �ý��ۿ��� ���ܵȴ�.
				CA_TRACE("Component System : %u No Object", _pairVIter->second->GetComponetId());
				_componentV.erase(_pairVIter);
			}
		}
	}

}

/// <summary>
/// ������ ������Ʈ�� Start
/// </summary>
/// <param name="component"></param>
void ComponentSystem::Start(ComponentBase* component)
{
	m_pResourceObserverOwner->Notify();

	//��ŸƮ �������� �ڽ��� ������Ʈ�� ���� ������Ʈ�� �ý��ۿ��� ���ܵȴ�.
	if (!component->GetObjectIsExist())
	{
		CA_TRACE("Component System : %u No Object", component->GetComponetId());

		/// <summary>
		/// reverse iterator
		/// ���� ���ͷ����Ϳ� �������� �����Ѵ�.
		/// </summary>
		std::vector<std::pair<int, ComponentBase*>>::reverse_iterator _pairVIter(m_Component_V.at(component->GetComponentType()).rbegin());
		for (; _pairVIter != m_Component_V.at(component->GetComponentType()).rend(); _pairVIter++)
		{
			//�����Ϸ��� ��ü�� ������ ��ü �����Ѵ�!
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
/// ����(��ũ)������
/// ������Ʈ���� ���ķ� ������Ʈ�ϱ� ���� �Լ�
/// �����庰�� ������Ʈ�� ������Ʈ���� ���� iter�� �� iter�� �޾Ƽ� �� ���̸� ���� ������Ʈ
/// </summary>
/// <param name="start">���� ������Ʈ iter</param>
/// <param name="end">�� ������Ʈ iter</param>
/// <param name="dTime">������Ʈ�� �� �� dTime</param>
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

	CA_TRACE("������ %x", std::this_thread::get_id());
}


void ComponentSystem::Update(float dTime)
{
	m_IsRunning = true;

	/// �Ͻ��������̸� Update���� ���� �ʴ´�.
	if (m_IsPaused == false)
	{
		// ���������� ������Ʈ
		for (ComponentBase*& _rComp : m_PhysicsActorComponent_V)
		{
			if (_rComp->GetObjectIsEnabled() == true)
			{
				_rComp->Update(dTime);
			}
		}

		//������Ʈ Ÿ�Ժ� ������Ʈ ������ �����غ���.
		for (int _type = 1; _type < m_Component_V.size(); _type++)
		{
			UpdateByType(dTime, _type);
		}

		
		/*
		// Added By Yoking
		// ������Ʈ���� �����带 ����� ���ķ� ������Ʈ �غ����� �õ��غ�..
		// �׷��� �߸� ���� �ִ��� ������ ������... 
		// �Ʒ��� �ڵ�� �ذ�Ǳ� ������ ��� ����

		// ������� �Ҵ��� ������Ʈ ������Ʈ ����
		const unsigned int _assignmentCountForWorkerThread = 10;

		std::vector<std::thread> workers;
		for (size_t i = 0; i < m_Component_V.size(); i++)
		{
			// ������ �۾� ������ ����
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

			//������ Ÿ�Ժ� ���� ���͸� ������� ������ ������Ʈ�Ѵ�.			
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
	///  update�� register��, deregister�� ������Ʈ���� ��Ƶξ��ٰ� �ش� update�������� Register, Start��Ų��.
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
	// ���ӷ��� ��ũ��Ʈ ������Ʈ�� ��쿡
	// physicsActor ������Ʈ�� myObject�� �پ��ִٸ� OnCollisionEnter() ȣ��
	for (auto& _pairV : m_Component_V.at(static_cast<int>(ComponentType::GameLogic)))
	{
		// ������ ����������Ʈ�� �پ�������
		PhysicsActor* physActor = _pairV.second->GetMyObject()->GetComponent<PhysicsActor>();
		if (physActor != nullptr)
		{
			// ������Ʈ/���ӷ��� ��ũ��Ʈ Ȱ��ȭ Ȯ��
			if (_pairV.second->GetObjectIsEnabled() == true &&
				_pairV.second->GetIsEnabled() == true)
			{
				// �浹(Enter) �� ������Ʈ �� ��ŭ ȣ��
				std::vector<Collision>& collisionEnterList_V = physActor->m_CollisionEnter_V;
				for (auto& collisionEnterObj : collisionEnterList_V)
				{
					// ��ũ��Ʈ���� �������̵� ������ �浹 ���� ȣ��
					_pairV.second->OnCollisionEnter(*collisionEnterList_V.begin());
				}
			}
		}
	}

}

void ComponentSystem::OnCollisionStay()
{
	// ���ӷ��� ��ũ��Ʈ ������Ʈ�� ��쿡
	// physicsActor ������Ʈ�� myObject�� �پ��ִٸ� OnCollisionStay() ȣ��
	for (auto& _pairV : m_Component_V.at(static_cast<int>(ComponentType::GameLogic)))
	{
		// ������ ����������Ʈ�� �پ�������
		PhysicsActor* physActor = _pairV.second->GetMyObject()->GetComponent<PhysicsActor>();
		if (physActor != nullptr)
		{
			// ������Ʈ/���ӷ��� ��ũ��Ʈ Ȱ��ȭ Ȯ��
			if (_pairV.second->GetObjectIsEnabled() == true &&
				_pairV.second->GetIsEnabled() == true)
			{
				// �浹��(stay)�� ������Ʈ �� ��ŭ ȣ��
				std::vector<Collision>& collisionStayList_V = physActor->m_CollisionStay_V;
				for (auto& collisionStayObj : collisionStayList_V)
				{
					// ��ũ��Ʈ���� �������̵� ������ �浹 ���� ȣ��
					_pairV.second->OnCollisionStay(*collisionStayList_V.begin());
				}
			}
		}
	}
}

void ComponentSystem::OnCollisionExit()
{
	// ���ӷ��� ��ũ��Ʈ ������Ʈ�� ��쿡
	// physicsActor ������Ʈ�� myObject�� �پ��ִٸ� OnCollisionExit() ȣ��
	for (auto& _pairV : m_Component_V.at(static_cast<int>(ComponentType::GameLogic)))
	{
		// ������ ����������Ʈ�� �پ�������
		PhysicsActor* physActor = _pairV.second->GetMyObject()->GetComponent<PhysicsActor>();
		if (physActor != nullptr)
		{
			// ������Ʈ/���ӷ��� ��ũ��Ʈ Ȱ��ȭ Ȯ��
			if (_pairV.second->GetObjectIsEnabled() == true &&
				_pairV.second->GetIsEnabled() == true)
			{
				// �浹����(Exit) �� ������Ʈ �� ��ŭ ȣ��
				std::vector<Collision>& collisionExitList_V = physActor->m_CollisionExit_V;
				for (auto& collisionExitObj : collisionExitList_V)
				{
					// ��ũ��Ʈ���� �������̵� ������ �浹 ���� ȣ��
					_pairV.second->OnCollisionExit(*collisionExitList_V.begin());
				}
			}
		}
	}
}

void ComponentSystem::ClearCollisionList()
{
	// ������ ���� ������Ʈ��
	// �浹����Ʈ Ŭ����
	for (auto& _pairV : m_PhysicsActorComponent_V)
	{
		// ������ ����������Ʈ�� �پ�������
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
	// ����Ʈ�� PreRender�� ������
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
	/// ���� ��û : ī�޶� Render�� �������� ���ڴ�.
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
	// ����Ʈ�� PreRender�� ������
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
/// Update���� �����.
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
/// userIndex ��ȣ�� �ش��ϴ� ���������� ������Ʈ�� ã�Ƽ� ����
/// </summary>
/// <param name="userIndex">���� �ε���</param>
/// <returns></returns>
PhysicsActor* ComponentSystem::FindPhysicsActor(int userIndex) const
{
	// ������ ���� ���� ��ȸ
	for (int i = 0; i < m_PhysicsActorComponent_V.size(); i++)
	{
		PhysicsActor* physicsActor = dynamic_cast<PhysicsActor*>(m_PhysicsActorComponent_V.at(i));

		// ���� �ε��� ��
		if (physicsActor->GetUserIndex() == userIndex)
		{
			// �ε����� ��ġ�ϸ� �ش� ������Ʈ ����
			return physicsActor;
		}
	}

	// �ش��ϴ� �ε����� ���� ������ ���Ͱ� ����.
	return nullptr;
}

bool ComponentSystem::HasGameLogicScriptComponent(PhysicsActor* actor) const
{
	GameObject* gameObject = actor->GetMyObject();

	// ���� ������Ʈ�� ������Ʈ�� ��ȸ�ϸ鼭 ���� ���� ������Ʈ�� �ִ��� ã�´�.
	for (auto component : gameObject->m_Component_V)
	{
		if (component->GetComponentType() == static_cast<int>(ComponentType::GameLogic))
		{
			// ���� ���� ������Ʈ�� �پ��ִ�!
			return true;
		}
	}

	// ���� ���� ������Ʈ�� ����!
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
