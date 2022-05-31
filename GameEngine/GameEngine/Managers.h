#pragma once
#include "Singleton.h"

#include "CameraManager.h"
#include "SceneManager.h"
#include "ComponentSystem.h"
#include "PhysicsEngine.h"
#include "NavMeshManager.h"
#include "JsonManager.h"

class SceneManager;
class ObjectManager;
class CameraManager;
class ComponentSystem;
class PhysicsEngine;
class ResourceLoader;

/// ��� �Ŵ������� �����ϴ� Ŭ����

enum eRENDER_MODE
{
	DEFAULT = 0,		// ������ ���� ���
	REFLECTION_PROBE,	// ���÷��� ���� ���
};

class Managers : public Singleton<Managers>
{
public:
   Managers();
   ~Managers();

public:
   void Initialize();

   void GetManagersToClient(SceneManager*, ObjectManager*, CameraManager*, ComponentSystem*);

   SceneManager* GetSceneManager();
   CameraManager* GetCameraManager();
   ComponentSystem* GetComponentSystem();
   JsonManager* GetJsonManager();
   PhysicsEngine* GetPhysicsEngine();
   NavMeshManager* GetNavMeshManager();

   eRENDER_MODE GetNowRenderMode();
   void SetRenderMode(eRENDER_MODE mode);

private:
   SceneManager* m_pSceneManager;
   CameraManager* m_pCameraManager;
   ComponentSystem* m_pComponentSystem;
   JsonManager* m_pJsonManager;
   PhysicsEngine* m_pPhysicsEngine;
   NavMeshManager* m_pNavMeshManager;

   eRENDER_MODE m_NowRenderMode;
};

