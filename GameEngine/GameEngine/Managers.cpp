#include "pch.h"
#include "Managers.h"



Managers::Managers()
   : m_pSceneManager(nullptr)
   , m_pCameraManager(nullptr)
   , m_pComponentSystem(nullptr)
   , m_pPhysicsEngine(nullptr)
	, m_pJsonManager(nullptr)
	, m_pNavMeshManager(nullptr)
	, m_NowRenderMode(eRENDER_MODE::DEFAULT)
{
}

Managers::~Managers()
{
	Safe_Delete(m_pComponentSystem);
	Safe_Delete(m_pNavMeshManager);
	Safe_Delete(m_pPhysicsEngine);
	Safe_Delete(m_pSceneManager);
	Safe_Delete(m_pJsonManager);
	Safe_Delete(m_pCameraManager);
}

SceneManager* Managers::GetSceneManager()
{
   return m_pSceneManager;
}

CameraManager* Managers::GetCameraManager()
{
   return m_pCameraManager;
}

ComponentSystem* Managers::GetComponentSystem()
{
	return m_pComponentSystem;
}

JsonManager* Managers::GetJsonManager()
{
	return m_pJsonManager;
}

PhysicsEngine* Managers::GetPhysicsEngine()
{
   return m_pPhysicsEngine;
}

NavMeshManager* Managers::GetNavMeshManager()
{
	return m_pNavMeshManager;
}

eRENDER_MODE Managers::GetNowRenderMode()
{
	return m_NowRenderMode;
}

void Managers::SetRenderMode(eRENDER_MODE mode)
{
	m_NowRenderMode = mode;
}

void Managers::Initialize()
{
   m_pSceneManager = new SceneManager();
   m_pCameraManager = new CameraManager();
   m_pJsonManager = new JsonManager();
   m_pPhysicsEngine = new PhysicsEngine();
   m_pPhysicsEngine->Initialize();
   m_pComponentSystem = ComponentSystem::GetInstance();

   m_pNavMeshManager = new NavMeshManager();
}

void Managers::GetManagersToClient(SceneManager* pSM, ObjectManager* pOM, CameraManager* pCM, ComponentSystem* pCS)
{
   m_pSceneManager = pSM;
   m_pCameraManager = pCM;
   m_pComponentSystem = pCS;
}
