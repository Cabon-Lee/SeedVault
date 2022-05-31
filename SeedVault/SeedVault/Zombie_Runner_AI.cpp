#include "pch.h"

#include "Zombie_Runner_Move.h"
#include "BehaviorTree_Base.h"
#include "Zombie_Runner_AI.h"

bool BehaviorTree::Enemy::IsDead::Invoke()
{
	return m_Enemy->IsDead();
}

bool BehaviorTree::Enemy::IsWait::CheckCondition()
{
	return m_Enemy->IsWait();
}

bool BehaviorTree::Enemy::FindPlayer::Invoke()
{
	return m_Enemy->FindPlayer();
}

bool BehaviorTree::Enemy::MoveForPlayer::Invoke()
{
	return m_Enemy->MoveForPlayer();
}

bool BehaviorTree::Enemy::IsPartol::CheckCondition()
{
	return m_Enemy->IsPatrol();
}

bool BehaviorTree::Enemy::UpdateTargetWayPoint::Invoke()
{
	return m_Enemy->UpdateTargetWayPoint();
}

bool BehaviorTree::Enemy::MoveForTargetWayPoint::Invoke()
{
	return m_Enemy->MoveForTargetWayPoint();
}

bool BehaviorTree::Enemy::Wait::Invoke()
{
	return m_Enemy->Wait();
}

bool BehaviorTree::Enemy::IsAttack::CheckCondition()
{
	return m_Enemy->IsAttack();
}



Zombie_Runner_AI::Zombie_Runner_AI()
	: Enemy_AI()
	, m_Root(nullptr)
	, m_Selector(nullptr)
	//, m_SeqMovingAttack(nullptr)
	, m_SeqDead(nullptr)
	, m_IsDead(nullptr)

	, m_SelCombat(nullptr)
	, m_SeqSearchAndHunt(nullptr)
	, m_SeqSearch(nullptr)
	, m_FindPlayer(nullptr)
	, m_SeqHunt(nullptr)
	, m_MoveForPlayer(nullptr)

	, m_IsPatrol(nullptr)
	, m_SeqPatrol(nullptr)
	, m_MoveForTarget(nullptr)
	, m_UpdateTarget(nullptr)

	, m_IsWait(nullptr)
	, m_Wait(nullptr)

	, m_SaveData(new Enemy_AI_Save())
{
	CA_TRACE("[Zombie_Runner_AI] Create Zombie_Runner_AI (Zombie_Runner_AI())");
}

Zombie_Runner_AI::~Zombie_Runner_AI()
{

}

void Zombie_Runner_AI::Start()
{
	CA_TRACE("[Zombie_Runner_AI] Start()");
	m_Enemy = m_pMyObject->GetComponent<Zombie_Runner_Move>();

	m_Root = std::make_unique<BehaviorTree::Sequence>();
	m_Selector = std::make_unique<BehaviorTree::Selector>();

	m_SeqDead = std::make_unique<BehaviorTree::Sequence>();
	m_IsDead = std::make_unique<BehaviorTree::Enemy::IsDead>(m_Enemy);

	m_SelCombat = std::make_unique<BehaviorTree::Selector>();
	m_IsAttack = std::make_unique<BehaviorTree::Enemy::IsAttack>(m_Enemy);
	m_AttackToPlayer = std::make_unique<BehaviorTree::Enemy::AttackToPlayer>(m_Enemy);

	m_SeqSearchAndHunt = std::make_unique<BehaviorTree::Sequence>();
	m_SeqSearch = std::make_unique <BehaviorTree::Sequence>();
	m_FindPlayer = std::make_unique<BehaviorTree::Enemy::FindPlayer>(m_Enemy);
	m_SeqHunt = std::make_unique <BehaviorTree::Sequence>();
	m_MoveForPlayer = std::make_unique<BehaviorTree::Enemy::MoveForPlayer>(m_Enemy);

	m_IsPatrol = std::make_unique<BehaviorTree::Enemy::IsPartol>(m_Enemy);
	m_SeqPatrol = std::make_unique<BehaviorTree::Sequence>();
	m_MoveForTarget = std::make_unique<BehaviorTree::Enemy::MoveForTargetWayPoint>(m_Enemy);
	m_UpdateTarget = std::make_unique<BehaviorTree::Enemy::UpdateTargetWayPoint>(m_Enemy);

	m_IsWait = std::make_unique<BehaviorTree::Enemy::IsWait>(m_Enemy);
	m_Wait = std::make_unique<BehaviorTree::Enemy::Wait>(m_Enemy);

	Initialize();
}

void Zombie_Runner_AI::Update(float dTime)
{
	m_Root->Invoke();
	//while (m_Root->Invoke())
	{
		// 유니티에서라면 아래의 코드가.. 여기선 어떻게??
		// yield return new WaitForEndOfFrame();
		// how to coroutine in C++
		// c++ 20?

		//CA_TRACE("[Zombie_Runner_AI] m_Root->Invoke() ...");
	}

	//CA_TRACE("[Zombie_Runner_AI] BehaviorProcess Exit");
}

void Zombie_Runner_AI::OnRender()
{
}

void Zombie_Runner_AI::Initialize()
{
	m_Enemy = m_pMyObject->GetComponent<Enemy_Move>();

	// Root Node
	m_Root->AddChild(m_Selector.get());

	// Dead
	m_Selector->AddChild(m_SeqDead.get());
	{
		m_SeqDead->AddChild(m_IsDead.get());
	}


	// Combat
	m_Selector->AddChild(m_SelCombat.get());
	{
		// Attack
		m_SelCombat->AddChild(m_IsAttack.get());
		{
			m_IsAttack->SetChild(m_AttackToPlayer.get());
		}

		// Return
		// 인식(어그로)끌렸던 위치로 귀환


		// Search And Hunt
		m_SelCombat->AddChild(m_SeqSearchAndHunt.get());
		{
			// Search
			m_SeqSearchAndHunt->AddChild(m_SeqSearch.get());
			{
				m_SeqSearch->AddChild(m_FindPlayer.get());
			}

			// Hunt
			m_SeqSearchAndHunt->AddChild(m_SeqHunt.get());
			{
				m_SeqHunt->AddChild(m_MoveForPlayer.get());
			}
		}
	
	}

	// Patrol
	m_Selector->AddChild(m_IsPatrol.get());
	{
		m_IsPatrol->SetChild(m_SeqPatrol.get());

		m_SeqPatrol->AddChild(m_MoveForTarget.get());
		m_SeqPatrol->AddChild(m_UpdateTarget.get());
	}

	// Wait
	m_Selector->AddChild(m_Wait.get());
	//m_Selector->AddChild(m_Wait);
	//m_Selector->AddChild(m_IsWait);
}

void Zombie_Runner_AI::SaveData()
{
	m_SaveData->m_bEnable = GetIsEnabled();
	m_SaveData->m_ComponentId = m_ComponentId;

	save::WriteValue(m_Value, EraseClass(typeid(*this).name()), *m_SaveData);
}

void Zombie_Runner_AI::LoadData()
{
	save::ReadValue(m_Value, EraseClass(typeid(*this).name()), *m_SaveData);

	SetEnable(m_SaveData->m_bEnable);
}

bool BehaviorTree::Enemy::AttackToPlayer::Invoke()
{
	return m_Enemy->AttackToPlayer();
}

bool BehaviorTree::Enemy::IsReturn::CheckCondition()
{
	return m_Enemy->IsReturn();
}
