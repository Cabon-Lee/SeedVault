#pragma once

/// <summary>
/// �浹 ������ ��� Ŭ����
/// physx ������ �ùķ��̼� �ϸ鼭 ������Ʈ�� �浹�� �Ͼ�ԵǸ� �浹 ������ ����
/// PhysicsActor Ŭ������ OnCollisionEnter()/Stay()/Exit() �� �������ش�.
/// 
/// �ۼ��� : YoKing
/// </summary>

class ComponentBase;
class PhysicsActor;

class Collision
{
public:
	Collision(PhysicsActor* actor);
	_DLL ~Collision();

	PhysicsActor* m_Actor;
	GameObject* m_GameObject;
	Transform* m_Transform;

protected:

private:
};