#pragma once

/// Raycast에 관한 클래스
/// ray를 쏴서 닿는곳의 gameobject의 정보를 탐색?한다
/// physx의 기능을 써보려하는데 너무 많은 내용이있다...

class GameObject;
class GeometryBase;
class PhysicsActor;
class Transform;

/// <summary>
/// 안쓸듯함
/// </summary>
enum HitFlag
{
   Position,
   Normal,
   UV, // 안쓸듯함

   DefaultFlag
};

/// <summary>
/// 레이히트에 대한 정보 구조체
/// </summary>
struct RaycastHit
{
   RaycastHit() : actor(nullptr), faceIndex(0), flag(HitFlag::DefaultFlag),
                  position(), normal(), distance(0.0f), u(0.0f), v(0.0f) {}

   PhysicsActor* actor;
   //GeometryBase* shape;
   unsigned int faceIndex;
   HitFlag flag;
   DirectX::SimpleMath::Vector3 position;
   DirectX::SimpleMath::Vector3 normal;
   float distance;
   float u, v; // 안쓸듯함
};

/// <summary>
/// 충돌 상태 enum
/// </summary>
enum CollisionState
{
   NonCollision, // 처음 디폴트 상태

   StartCollision,
   OnCollision,
   EndCollision
};


// overlapHit으로 확인하는 collisionHit 정보 구조체
struct CollisionHit
{
   CollisionHit() : 
      actor(nullptr), 
      prevColState(CollisionState::NonCollision), 
      currColState(CollisionState::NonCollision) {}

   PhysicsActor* actor;

   CollisionState prevColState; // 사용안하는중
   CollisionState currColState;

   //GeometryBase* shape;
};