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
	// ������Ʈ �ý��� �Ҵ�
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
/// phsyx ������ �� ������Ʈ�� ���˵Ǹ� �̺�Ʈ�� ���´�.
/// </summary>
/// <param name="pairHeader">�浵�� ������Ʈ��</param>
/// <param name="pairs">��� ����</param>
/// <param name="nbPairs">��� ����(1�� �����µ�)</param>
void BaseEventCallBack::onContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 nbPairs)
{
	// �ε�����ȣ(���ӿ���)�� ĳ���� 
	int userIndex = reinterpret_cast<int>(pairHeader.actors[0]->userData);
	int userIndex2 = reinterpret_cast<int>(pairHeader.actors[1]->userData);

	/// �ε�����ȣ�� ������ ���͸� ã��
	PhysicsActor* actor = m_ComponentSystem->FindPhysicsActor(userIndex);
	PhysicsActor* actor2 = m_ComponentSystem->FindPhysicsActor(userIndex2);

	// ���ӿ����� �� ��ȯ�ϰ� ������ �̰��� �����ϴ� ��찡 �ִµ� �̶� actor/actor2 �� nullptr�� �� �� �־ üũ�� �ش�.
	if (actor == nullptr || actor2 == nullptr)
	{
		return;
	}

	/// OnCollisionEnter
	// ù ���˽ÿ�
	if (pairs->events == PxPairFlag::eNOTIFY_TOUCH_FOUND)
	{
		// ���Ͱ� ���ӷ��� ������Ʈ�� ������ �ִ��� Ȯ���ؼ� ������ 
		// Enter �浹 ����Ʈ�� �浹�� ������ �߰����ش�.
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
	/// ������ �𸣰����� SetVelocity()�� ȣ��(����)���� ������ Stay�� �ȵ��´�..(����ü �̳��� ������;;)
	/// ���������� Stay üũ���Ϸ��� ������Ʈ�� �ӵ��� ������ SetVelocity({0, 0, 0})�̶� �ؾ��� �� ����. 
	// ���� ����
	if (pairs->events == PxPairFlag::eNOTIFY_TOUCH_PERSISTS)
	{
		// ���Ͱ� ���ӷ��� ������Ʈ�� ������ �ִ��� Ȯ���ؼ� ������ 
		// Stay �浹 ����Ʈ�� �浹 ������ �߰����ش�.
		if (m_ComponentSystem->HasGameLogicScriptComponent(actor))
		{
			/// physx ���� GameEngine ���� ������ ���ư�.
			/// physx ���� ���������Ͱ� �߰��� �������� �ùķ��̼��� ��� �̷������ �̺�Ʈ�� �߻��ؼ� ���� ������Ʈ�� �浹 ����Ʈ�� �߰��� �Լ� ������
			/// ���� ������ ������Ʈ���� �ʱ⿡ �ϰ� ��ŸƮ�ǰ� ����Loop ���� ������Ʈ�� ������ ���� �־ 
			/// CollisionStay�� ��� �浹����Ʈ ���Ϳ� push �� �� �ε��� ����� �̹� �߰��� �Ǿ��ֳ� Ȯ���ؼ� �־��� �ʿ䰡 �ִ�.
			
			// ������ Stay ����Ʈ�� �߰����� ���� ���� �߰��� actor2�� �̹� Stay ����Ʈ�� �ִ��� Ȯ��
			bool notAdded = true;		// �̹� �߰��Ǿ��ִ��� Ȯ���� bool ����
			for (auto collision : actor->m_CollisionStay_V)
			{
				// actor2�� �̹� Stay ����Ʈ�� �� ������
				if (collision.m_Actor == actor2)
				{
					// �߰����� �ʴ´�.
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
			// ������ Stay ����Ʈ�� �߰����� ���� ���� �߰��� actor2�� �̹� Stay ����Ʈ�� �ִ��� Ȯ��
			bool notAdded = true;		// �̹� �߰��Ǿ��ִ��� Ȯ���� bool ����
			for (auto collision : actor2->m_CollisionStay_V)
			{
				// actor�� �̹� Stay ����Ʈ�� �� ������
				if (collision.m_Actor == actor)
				{
					// �߰����� �ʴ´�.
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

		// ���Ͱ� ���ӷ��� ������Ʈ�� ������ �ִ��� Ȯ���ؼ� ������ 
		// Stay �浹 ����Ʈ�� �浹 ������ �߰����ش�.
		if (m_ComponentSystem->HasGameLogicScriptComponent(actor))
		{
			/// physx ���� GameEngine ���� ������ ���ư�.
			/// physx ���� ���������Ͱ� �߰��� �������� �ùķ��̼��� ��� �̷������ �̺�Ʈ�� �߻��ؼ� ���� ������Ʈ�� �浹 ����Ʈ�� �߰��� �Լ� ������
			/// ���� ������ ������Ʈ���� �ʱ⿡ �ϰ� ��ŸƮ�ǰ� ����Loop ���� ������Ʈ�� ������ ���� �־ 
			/// CollisionStay�� ��� �浹����Ʈ ���Ϳ� push �� �� �ε��� ����� �̹� �߰��� �Ǿ��ֳ� Ȯ���ؼ� �־��� �ʿ䰡 �ִ�.

			// ������ Stay ����Ʈ�� �߰����� ���� ���� �߰��� actor2�� �̹� Stay ����Ʈ�� �ִ��� Ȯ��
			bool notAdded = true;		// �̹� �߰��Ǿ��ִ��� Ȯ���� bool ����
			for (auto collision : actor->m_CollisionStay_V)
			{
				// actor2�� �̹� Stay ����Ʈ�� �� ������
				if (collision.m_Actor == actor2)
				{
					// �߰����� �ʴ´�.
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
			// ������ Stay ����Ʈ�� �߰����� ���� ���� �߰��� actor2�� �̹� Stay ����Ʈ�� �ִ��� Ȯ��
			bool notAdded = true;		// �̹� �߰��Ǿ��ִ��� Ȯ���� bool ����
			for (auto collision : actor2->m_CollisionStay_V)
			{
				// actor�� �̹� Stay ����Ʈ�� �� ������
				if (collision.m_Actor == actor)
				{
					// �߰����� �ʴ´�.
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
	// ���� ���� �ÿ�
	if (pairs->events == PxPairFlag::eNOTIFY_TOUCH_LOST)
	{
		// ���Ͱ� ���ӷ��� ������Ʈ�� ������ �ִ��� Ȯ���ؼ� ������ 
		// Exit �浹 ����Ʈ�� ������ �浹 ������ �߰����ش�.
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
