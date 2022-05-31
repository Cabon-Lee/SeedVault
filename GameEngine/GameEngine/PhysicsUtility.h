#include "pch.h"
#include "Transform.h"
#include "PhysicsGeometry.h"
#include "Raycast.h"
//#include "PxPhysicsAPI.h"
//#include "PhysicsTransform.h"

/// PhysicsEngine에 쓸 utility 함수들

static physx::PxGeometry* CreatePxGeometry(GeometryBase* geom);
static physx::PxGeometryType::Enum CreatePxGeometryType(GeometryType geomType);
static physx::PxShape* CreateShape(GeometryType geomType);
static physx::PxHitFlag::Enum CreatePxHitFlag(HitFlag hitFlag);
static HitFlag CreateHitFlag(physx::PxHitFlag::Enum hitFlag);

static physx::PxRaycastHit* CreatePxRaycastHit(RaycastHit* raycastHit);

static physx::PxVec3 CreatePxVec3(const DirectX::SimpleMath::Vector3& vec);
static physx::PxQuat CreatePxQuat(const DirectX::SimpleMath::Quaternion& quat);
static DirectX::SimpleMath::Vector3 CreateVector3(const physx::PxVec3& vec);

static physx::PxTransform CreatePxTransform(const DirectX::SimpleMath::Vector3& pos, const DirectX::SimpleMath::Quaternion& rot);
static physx::PxTransform CreatePxTransform(const DirectX::SimpleMath::Vector3& pos);
static physx::PxTransform CreatePxTransform(const DirectX::SimpleMath::Quaternion& rot);
static physx::PxTransform CreatePxTransform(const Transform& transform);

static Transform CreateTransform(const physx::PxTransform& pxTransform);
static void UpdateTransformToPhysics(Transform* pTransform, physx::PxTransform& pxTransform);

static void GetPxRigidActorVector(
    physx::PxScene* pScene, physx::PxActorTypeFlags flag, physx::PxRigidActor** data);
static physx::PxRigidActor* GetPxRigidActor(const std::vector<physx::PxRigidActor*>& actors, int index);


//static Transform CreateTransform(DirectX::SimpleMath::Vector3 pos);
//static Transform CreateTransform(DirectX::SimpleMath::Quaternion rot);
//static Transform CreateTransform(Transform transform);


template<typename T> physx::PxGeometry* CreateGeometry();

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

physx::PxGeometry* CreatePxGeometry(GeometryBase* geom)
{
   physx::PxGeometry* _nowPxGeo;
   switch (geom->GetType())
   {
      case Sphere:
      {
         _nowPxGeo = CreateGeometry<physx::PxSphereGeometry>();
         break;
      }
      case Box:
      {
         _nowPxGeo = CreateGeometry<physx::PxBoxGeometry>();
         break;
      }
      case Capsule:
      {
         _nowPxGeo = CreateGeometry<physx::PxCapsuleGeometry>();
         break;
      }

      default:
      {
         _nowPxGeo = CreateGeometry<physx::PxSphereGeometry>();
         break;
      }
   }

   return _nowPxGeo;
}

physx::PxGeometryType::Enum CreatePxGeometryType(GeometryType geomType)
{
   switch (geomType)
   {
      case Sphere:
      {
         return physx::PxGeometryType::eSPHERE;
      }
      case Box:
      {
         return physx::PxGeometryType::eBOX;
      }
      case Capsule:
      {
         return physx::PxGeometryType::eCAPSULE;
      }

      default:
      {
         return physx::PxGeometryType::eSPHERE;
      }
   }
}

physx::PxShape* CreateShape(GeometryType geomType)
{

}


physx::PxHitFlag::Enum CreatePxHitFlag(HitFlag hitFlag)
{
   switch (hitFlag)
   {
      case Position:
      {
         return physx::PxHitFlag::ePOSITION;
      }
      case Normal:
      {
         return physx::PxHitFlag::eNORMAL;
      }
      case UV:
      {
         return physx::PxHitFlag::eUV;
      }      

      default:
      {
         return physx::PxHitFlag::eDEFAULT;
      }
   }
}

HitFlag CreateHitFlag(physx::PxHitFlag::Enum hitFlag)
{
   switch (hitFlag)
   {
      case physx::PxHitFlag::Enum::ePOSITION:
      {
         return HitFlag::Position;
      }
      case physx::PxHitFlag::Enum::eNORMAL:
      {
         return HitFlag::Normal;
      }
      case physx::PxHitFlag::Enum::eUV:
      {
         return HitFlag::UV;
      }
      default:
      {
         return HitFlag::DefaultFlag;
      }
   }
}

physx::PxRaycastHit* CreatePxRaycastHit(RaycastHit* raycastHit)
{
   //physx::PxRaycastHit* _rayHit = new physx::PxRaycastHit();

   //_rayHit->actor = 



}


physx::PxVec3 CreatePxVec3(const DirectX::SimpleMath::Vector3& vec)
{
   physx::PxVec3 _vec3;
   _vec3.x = vec.x;
   _vec3.y = vec.y;
   _vec3.z = vec.z;

   return _vec3;
}

physx::PxQuat CreatePxQuat(const DirectX::SimpleMath::Quaternion& quat)
{
   physx::PxQuat _quat;
   _quat.x = quat.x;
   _quat.y = quat.y;
   _quat.z = quat.z;
   _quat.w = quat.w;

   return _quat;
}

DirectX::SimpleMath::Vector3 CreateVector3(const physx::PxVec3& vec)
{
   DirectX::SimpleMath::Vector3 _vec;
   _vec.x = vec.x;
   _vec.y = vec.y;
   _vec.z = vec.z;

   return _vec;
}

physx::PxTransform CreatePxTransform(const DirectX::SimpleMath::Vector3& pos, const DirectX::SimpleMath::Quaternion& rot)
{
   physx::PxVec3 _pos;
   _pos.x = pos.x;
   _pos.y = pos.y;
   _pos.z = pos.z;

   physx::PxQuat _rot;
   _rot.x = rot.x;
   _rot.y = rot.y;
   _rot.z = rot.z;
   _rot.w = rot.w;

   physx::PxTransform _t = physx::PxTransform(_pos, _rot);

   return _t;
}

physx::PxTransform CreatePxTransform(const DirectX::SimpleMath::Vector3& pos)
{
   physx::PxVec3 _pos;
   _pos.x = pos.x;
   _pos.y = pos.y;
   _pos.z = pos.z;

   physx::PxTransform _t = physx::PxTransform(_pos);

   return _t;
}

physx::PxTransform CreatePxTransform(const DirectX::SimpleMath::Quaternion& rot)
{
   physx::PxQuat _rot;
   _rot.x = rot.x;
   _rot.y = rot.y;
   _rot.z = rot.z;
   _rot.w = rot.w;

   physx::PxTransform _t = physx::PxTransform(_rot);

   return _t;
}

physx::PxTransform CreatePxTransform(const Transform& transform)
{
   physx::PxVec3 _pos;
   _pos.x = transform.m_Position.x;
   _pos.y = transform.m_Position.y;
   _pos.z = transform.m_Position.z;

   physx::PxQuat _rot;
   _rot.x = transform.m_Rotation.x;
   _rot.y = transform.m_Rotation.y;
   _rot.z = transform.m_Rotation.z;
   _rot.w = transform.m_Rotation.w;

   physx::PxTransform _t = physx::PxTransform(_pos, _rot);

   return _t;
}

Transform CreateTransform(const physx::PxTransform& pxTransform)
{
   /// 지역변수이므로 포인터로 넘겨주면 안된다!! 
   Transform _transform;
   _transform.m_Position.x = pxTransform.p.x;
   _transform.m_Position.y = pxTransform.p.y;
   _transform.m_Position.z = pxTransform.p.z;
   // rotation
   _transform.m_Rotation.x = pxTransform.q.x;
   _transform.m_Rotation.y = pxTransform.q.y;
   _transform.m_Rotation.z = pxTransform.q.z;
   _transform.m_Rotation.w = pxTransform.q.w;

   return _transform;
}

void UpdateTransformToPhysics(Transform* pTransform, physx::PxTransform& pxTransform)
{
   pTransform->SetTransform(
      DirectX::SimpleMath::Vector3(pxTransform.p.x, pxTransform.p.y, pxTransform.p.z),
      DirectX::SimpleMath::Quaternion(pxTransform.q.x, pxTransform.q.y, pxTransform.q.z, pxTransform.q.w));
}

void GetPxRigidActorVector(
    physx::PxScene* pScene, physx::PxActorTypeFlags flag,
    physx::PxRigidActor** data)
{
   uint32_t _actorNum = pScene->getNbActors(flag);
   //std::vector<physx::PxRigidActor*> _actor_V(_actorNum);
   pScene->getActors(flag, reinterpret_cast<physx::PxActor**>(data), _actorNum);
}

physx::PxRigidActor* GetPxRigidActor(const std::vector<physx::PxRigidActor*>& actors, int index)
{
   for (auto _actor : actors)
   {
      //if ((int)_actor->userData == index)
      if (reinterpret_cast<int>(_actor->userData) == index)
      {
         return _actor;
      }
   }

   return nullptr;
}

template<typename T>
physx::PxGeometry* CreateGeometry()
{
   T* _newGeo = new T();

   return _newGeo;
}

