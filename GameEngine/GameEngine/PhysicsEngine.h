#pragma once

#include "DLLDefine.h"


//class PxTransform;
//class PxRigidDynamic;
//class PxGeometry;
//
//class PxDefaultAllocator;
//class PxDefaultErrorCallback;
//
//class PxFoundation;
//class PxDefaultCpuDispatcher;
//class PxScene;
//class PxPhysics;
//class PxMaterial;
//enum GeometryType;

class Transform;
class PhysicsActor;
class SceneBase;
class GameObject;
struct RaycastHit;
struct CollisionHit;
class GeometryBase;

/// PhysX를 감싸고 물리 loop를 제공하기위한 클래스
/// 우리의 mesh들로 비쥬얼 디버깅 하기위해 어떻게해야하지...?
///

/// PhysX를 
class PhysicsEngine
{
public:
   PhysicsEngine();
   ~PhysicsEngine();

   void Initialize(bool isInteractive = true);
   void PhysicsLoop(bool interactive = true);
   void ReleasePhysics(bool interactive = true);
   void ReleasePhysicsActor(bool interactive = true);

   void CreateTestBox(Transform* transform, float size);
   void CreateDynamicObject(Transform* transform, int geometry, DirectX::SimpleMath::Vector3 velocity);
   void CreateStack(Transform* transform, uint32_t size, float halfExtent);

   void AddPhysicsScene(SceneBase* scene);
   void SetNowPhysicsScene(std::string name);
   void AddPhysicsActor(PhysicsActor* object); // 액터를 추가, 물리씬에 생성해주고 넣어줌

   void SetPhysicsTransform(Transform* transform);
   void UpdatePhysicsTransform(PhysicsActor* actor);
   void SetTransform(PhysicsActor* actor, const Transform& pos);
   void SetVelocity(PhysicsActor* actor, const DirectX::SimpleMath::Vector3& vel);
   void SetAngularVelocity(PhysicsActor* actor, const DirectX::SimpleMath::Vector3& vel);

   GameObject* GetGameObjectByIndex(unsigned int index);
   PhysicsActor* GetPhysicsActorByIndex(unsigned int index);
   unsigned int GetIndexFromGameObject(GameObject* object);
   unsigned int GetIndexFromPhysicsActor(PhysicsActor* actor);

   //_DLL GameObject* RaycastCheckGameObject(DirectX::SimpleMath::Vector3 rayOrigin,
   //                                        DirectX::SimpleMath::Vector3 rayDir,
   //                                        GeometryBase* geometry, Transform* transform,
   //                                        float maxDist, int hitFlag);

   _DLL GameObject* RaycastCheckGameObject(const DirectX::SimpleMath::Vector3& origin,
                                           const DirectX::SimpleMath::Vector3& dir, float dist);
   _DLL RaycastHit CheckRaycastHit(const DirectX::SimpleMath::Vector3& origin,
                                   const DirectX::SimpleMath::Vector3& dir, float dist);

   //_DLL bool MousePicking(int x, int y);


   /// how to replace to our renderer??

   // Added by 최 요 환
   _DLL physx::PxRigidActor* FindPxRigidActor(PhysicsActor* actor);
   void ChangePxRigidActor(PhysicsActor* actor);
   void ReleasePxActor(PhysicsActor* actor);
   void ReleaseAllPxActor();
   
   class BaseEventCallBack* m_BaseEvent;

   std::vector<PhysicsActor*> m_PhysicsActors_V;
   std::vector<physx::PxRigidActor*> m_PxRigidActor;
   void GetPxRigidActors();

   std::unordered_map<int, PhysicsActor*> m_PhysicsActors_UM;

   _DLL void SetRigidDynamicLockFlags(PhysicsActor* actor, struct FreezePositionAndRotation freezeValue);
   _DLL void SetKinematic(PhysicsActor* actor, bool val);
   _DLL void SetGravity(PhysicsActor* actor, bool val);

   physx::PxShape* CreateGeometryAndShape(const PhysicsActor* actor);
   physx::PxRigidStatic* CreateRigidStatic(const PhysicsActor* actor);
   physx::PxRigidDynamic* CreateRigidDynamic(const PhysicsActor* actor);

private:
   POINT m_MousePos;


};
