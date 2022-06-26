#pragma once

/// <summary>
/// 충돌 정보를 담는 클래스
/// physx 씬에서 시뮬레이션 하면서 오브젝트간 충돌이 일어나게되면 충돌 정보를 만들어서
/// PhysicsActor 클래스의 OnCollisionEnter()/Stay()/Exit() 에 전달해준다.
/// 
/// 작성자 : 최 요 환
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