#include "pch.h"
#include "Managers.h"
#include "PhysicsActor.h"
#include "PhysicsUtility.h"
#include "Collision.h"
#include "BaseEventCallBack.h"

extern physx::PxScene* g_pNowScene;

BaseEventCallBack::BaseEventCallBack()
	: m_ComponentSystem(nullptr)
{
	// 컴포넌트 시스템 할당
	m_ComponentSystem = Managers::GetInstance()->GetComponentSystem();
}

BaseEventCallBack::~BaseEventCallBack()
{
}

void BaseEventCallBack::onConstraintBreak(PxConstraintInfo* constraints, PxU32 count)
{
	int i = 0;
}

void BaseEventCallBack::onWake(PxActor** actors, PxU32 count)
{
	int i = 0;
}

void BaseEventCallBack::onSleep(PxActor** actors, PxU32 count)
{
	int i = 0;
}

/// <summary>
/// phsyx 씬에서 두 오브젝트가 접촉되면 이벤트가 들어온다.
/// </summary>
/// <param name="pairHeader">충도된 오브젝트들</param>
/// <param name="pairs">페어 정보</param>
/// <param name="nbPairs">페어 개수(1만 들어오는듯)</param>
void BaseEventCallBack::onContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 nbPairs)
{
	// 인덱스번호(게임엔진)로 캐스팅 
	int userIndex = reinterpret_cast<int>(pairHeader.actors[0]->userData);
	int userIndex2 = reinterpret_cast<int>(pairHeader.actors[1]->userData);

	/// 인덱스번호로 게임의 액터를 찾음
	PhysicsActor* actor = m_ComponentSystem->FindPhysicsActor(userIndex);
	PhysicsActor* actor2 = m_ComponentSystem->FindPhysicsActor(userIndex2);

	// 게임엔진의 씬 전환하고 나서도 이곳에 진입하는 경우가 있는데 이때 actor/actor2 이 nullptr이 될 수 있어서 체크해 준다.
	if (actor == nullptr || actor2 == nullptr)
	{
		return;
	}

	/// OnCollisionEnter
	// 첫 접촉시에
	if (pairs->events == PxPairFlag::eNOTIFY_TOUCH_FOUND)
	{
		// 액터가 게임로직 컴포넌트를 가지고 있는지 확인해서 있으면 
		// Enter 충돌 리스트에 충돌한 정보를 추가해준다.
		if (m_ComponentSystem->HasGameLogicScriptComponent(actor))
		{
			actor->m_CollisionEnter_V.push_back(actor2); 
		}

		if (m_ComponentSystem->HasGameLogicScriptComponent(actor2))
		{
			actor2->m_CollisionEnter_V.push_back(actor);
		}
	}

	/// OnCollisionStay
	/// 이유는 모르겠지만 SetVelocity()가 호출(갱신)되지 않으면 Stay에 안들어온다..(도대체 이놈의 구조는;;)
	/// 지속적으로 Stay 체크를하려면 오브젝트의 속도를 강제로 SetVelocity({0, 0, 0})이라도 해야할 것 같다. 
	// 접촉 유지
	if (pairs->events == PxPairFlag::eNOTIFY_TOUCH_PERSISTS)
	{
		// 액터가 게임로직 컴포넌트를 가지고 있는지 확인해서 있으면 
		// Stay 충돌 리스트에 충돌 정보를 추가해준다.
		if (m_ComponentSystem->HasGameLogicScriptComponent(actor))
		{
			/// physx 씬과 GameEngine 씬이 별도로 돌아감.
			/// physx 씬에 피직스액터가 추가된 시점부터 시뮬레이션이 계속 이루어지고 이벤트가 발생해서 게임 오브젝트의 충돌 리스트에 추가를 게속 하지만
			/// 게임 엔진의 컴포넌트들은 초기에 일괄 스타트되고 엔진Loop 에서 업데이트될 때까지 텀이 있어서 
			/// CollisionStay의 경우 충돌리스트 벡터에 push 할 때 부딪힌 대상이 이미 추가가 되어있나 확인해서 넣어줄 필요가 있다.
			
			// 무작정 Stay 리스트에 추가하지 말고 먼저 추가할 actor2가 이미 Stay 리스트에 있는지 확인
			bool notAdded = true;		// 이미 추가되어있는지 확인할 bool 변수
			for (auto collision : actor->m_CollisionStay_V)
			{
				// actor2가 이미 Stay 리스트에 들어가 있으면
				if (collision.m_Actor == actor2)
				{
					// 추가하지 않는다.
					notAdded = false;
					break;
				}
			}

			if (notAdded == true)
			{
				actor->m_CollisionStay_V.push_back(actor2);
			}
		}

		if (m_ComponentSystem->HasGameLogicScriptComponent(actor2))
		{
			// 무작정 Stay 리스트에 추가하지 말고 먼저 추가할 actor2가 이미 Stay 리스트에 있는지 확인
			bool notAdded = true;		// 이미 추가되어있는지 확인할 bool 변수
			for (auto collision : actor2->m_CollisionStay_V)
			{
				// actor가 이미 Stay 리스트에 들어가 있으면
				if (collision.m_Actor == actor)
				{
					// 추가하지 않는다.
					notAdded = false;
					break;
				}
			}

			if (notAdded == true)
			{
				actor2->m_CollisionStay_V.push_back(actor);
			}

		}

		///////////////////

		// 액터가 게임로직 컴포넌트를 가지고 있는지 확인해서 있으면 
		// Stay 충돌 리스트에 충돌 정보를 추가해준다.
		if (m_ComponentSystem->HasGameLogicScriptComponent(actor))
		{
			/// physx 씬과 GameEngine 씬이 별도로 돌아감.
			/// physx 씬에 피직스액터가 추가된 시점부터 시뮬레이션이 계속 이루어지고 이벤트가 발생해서 게임 오브젝트의 충돌 리스트에 추가를 게속 하지만
			/// 게임 엔진의 컴포넌트들은 초기에 일괄 스타트되고 엔진Loop 에서 업데이트될 때까지 텀이 있어서 
			/// CollisionStay의 경우 충돌리스트 벡터에 push 할 때 부딪힌 대상이 이미 추가가 되어있나 확인해서 넣어줄 필요가 있다.

			// 무작정 Stay 리스트에 추가하지 말고 먼저 추가할 actor2가 이미 Stay 리스트에 있는지 확인
			bool notAdded = true;		// 이미 추가되어있는지 확인할 bool 변수
			for (auto collision : actor->m_CollisionStay_V)
			{
				// actor2가 이미 Stay 리스트에 들어가 있으면
				if (collision.m_Actor == actor2)
				{
					// 추가하지 않는다.
					notAdded = false;
					break;
				}
			}

			if (notAdded == true)
			{
				actor->m_CollisionStay_V.push_back(actor2);
			}
		}

		if (m_ComponentSystem->HasGameLogicScriptComponent(actor2))
		{
			// 무작정 Stay 리스트에 추가하지 말고 먼저 추가할 actor2가 이미 Stay 리스트에 있는지 확인
			bool notAdded = true;		// 이미 추가되어있는지 확인할 bool 변수
			for (auto collision : actor2->m_CollisionStay_V)
			{
				// actor가 이미 Stay 리스트에 들어가 있으면
				if (collision.m_Actor == actor)
				{
					// 추가하지 않는다.
					notAdded = false;
					break;
				}
			}

			if (notAdded == true)
			{
				actor2->m_CollisionStay_V.push_back(actor);
			}

		}
	}

	/// OnCollisionExit
	// 접촉 해제 시에
	if (pairs->events == PxPairFlag::eNOTIFY_TOUCH_LOST)
	{
		// 액터가 게임로직 컴포넌트를 가지고 있는지 확인해서 있으면 
		// Exit 충돌 리스트에 떨어진 충돌 정보를 추가해준다.
		if (m_ComponentSystem->HasGameLogicScriptComponent(actor))
		{
			actor->m_CollisionExit_V.push_back(actor2);
		}

		if (m_ComponentSystem->HasGameLogicScriptComponent(actor2))
		{
			actor2->m_CollisionExit_V.push_back(actor);
		}
	}

}

void BaseEventCallBack::onTrigger(PxTriggerPair* pairs, PxU32 count)
{
	int i = 0;
}

void BaseEventCallBack::onAdvance(const PxRigidBody* const* bodyBuffer, const PxTransform* poseBuffer, const PxU32 count)
{
	int i = 0;
}
