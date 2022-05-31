#pragma once

/// Raycast�� ���� Ŭ����
/// ray�� ���� ��°��� gameobject�� ������ Ž��?�Ѵ�
/// physx�� ����� �Ẹ���ϴµ� �ʹ� ���� �������ִ�...

class GameObject;
class GeometryBase;
class PhysicsActor;
class Transform;

/// <summary>
/// �Ⱦ�����
/// </summary>
enum HitFlag
{
   Position,
   Normal,
   UV, // �Ⱦ�����

   DefaultFlag
};

/// <summary>
/// ������Ʈ�� ���� ���� ����ü
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
   float u, v; // �Ⱦ�����
};

/// <summary>
/// �浹 ���� enum
/// </summary>
enum CollisionState
{
   NonCollision, // ó�� ����Ʈ ����

   StartCollision,
   OnCollision,
   EndCollision
};


// overlapHit���� Ȯ���ϴ� collisionHit ���� ����ü
struct CollisionHit
{
   CollisionHit() : 
      actor(nullptr), 
      prevColState(CollisionState::NonCollision), 
      currColState(CollisionState::NonCollision) {}

   PhysicsActor* actor;

   CollisionState prevColState; // �����ϴ���
   CollisionState currColState;

   //GeometryBase* shape;
};