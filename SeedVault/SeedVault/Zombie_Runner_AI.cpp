#include "pch.h"
#include "BehaviorTree_Base.h"
#include "Zombie_Runner_Move.h"
#include "Zombie_Runner_AI.h"

bool BehaviorTree::Zombie_Runner::IsAttackPartner::CheckCondition()
{
	return m_ZombieRunner->IsAttackPartner();
}

bool BehaviorTree::Zombie_Runner::AttackToPartner::Invoke()
{
	return m_ZombieRunner->AttackToPartner();
}

bool BehaviorTree::Zombie_Runner::FindPartner::Invoke()
{
	return m_ZombieRunner->FindPartner();
}

bool BehaviorTree::Zombie_Runner::FindPlayer::Invoke()
{
	return m_ZombieRunner->FindPlayer();
}

bool BehaviorTree::Zombie_Runner::AwakenSight::Invoke()
{
	return m_ZombieRunner->AwakenSight();
}

Zombie_Runner_AI::Zombie_Runner_AI()
	: Enemy_AI()
	, m_Root(nullptr)
	, m_Selector(nullptr)
	, m_SeqDead(nullptr)

	, m_IsDead(nullptr)

	, m_IsAssassinated(nullptr)

	, m_IsReturn(nullptr)
	, m_SeqReturn(nullptr)
	, m_MoveToReturnPoint(nullptr)
	, m_FinishReturn(nullptr)

	, m_Explore(nullptr)

	, m_SelCombat(nullptr)
	, m_SeqSearchAndHunt(nullptr)
	, m_SelSearch(nullptr)
	, m_FindPlayer(nullptr)
	, m_FindPartner(nullptr)
	
	, m_AwakenSight(nullptr)
	, m_AwakenSound(nullptr)

	, m_SeqHunt(nullptr)
	, m_MoveToTarget(nullptr)

	, m_IsAttackPartner(nullptr)
	, m_AttackToPartner(nullptr)
	, m_IsAttackPlayer(nullptr)
	, m_AttackToPlayer(nullptr)

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
	
	m_IsAssassinated = std::make_unique<BehaviorTree::Enemy::IsAssassinated>(m_Enemy);

	m_IsReturn = std::make_unique<BehaviorTree::Enemy::IsReturn>(m_Enemy);
	m_SeqReturn = std::make_unique<BehaviorTree::Sequence>();
	m_MoveToReturnPoint = std::make_unique<BehaviorTree::Enemy::MoveToReturnPoint>(m_Enemy);
	m_FinishReturn = std::make_unique<BehaviorTree::Enemy::FinishReturn>(m_Enemy);

	m_Explore = std::make_unique<BehaviorTree::Enemy::Explore>(m_Enemy);

	m_SelCombat = std::make_unique<BehaviorTree::Selector>();
	m_IsAttackPartner = std::make_unique<BehaviorTree::Zombie_Runner::IsAttackPartner>(dynamic_cast<Zombie_Runner_Move*>(m_Enemy));
	m_AttackToPartner = std::make_unique<BehaviorTree::Zombie_Runner::AttackToPartner>(dynamic_cast<Zombie_Runner_Move*>(m_Enemy));
	m_IsAttackPlayer = std::make_unique<BehaviorTree::Enemy::IsAttackPlayer>(m_Enemy);
	m_AttackToPlayer = std::make_unique<BehaviorTree::Enemy::AttackToPlayer>(m_Enemy);

	m_SeqSearchAndHunt = std::make_unique<BehaviorTree::Sequence>();
	m_SelSearch = std::make_unique <BehaviorTree::Selector>();
	m_FindPlayer = std::make_unique<BehaviorTree::Zombie_Runner::FindPlayer>(dynamic_cast<Zombie_Runner_Move*>(m_Enemy));
	m_FindPartner = std::make_unique<BehaviorTree::Zombie_Runner::FindPartner>(dynamic_cast<Zombie_Runner_Move*>(m_Enemy));
	
	m_AwakenSight = std::make_unique<BehaviorTree::Zombie_Runner::AwakenSight>(dynamic_cast<Zombie_Runner_Move*>(m_Enemy));
	m_AwakenSound = std::make_unique<BehaviorTree::Enemy::AwakenSound>(m_Enemy);

	m_SeqHunt = std::make_unique <BehaviorTree::Sequence>();
	m_MoveToTarget = std::make_unique<BehaviorTree::Enemy::MoveToTarget>(m_Enemy);

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

	if (m_Enemy->m_State & Enemy_Move::State::eReturn && m_Enemy->m_State & Enemy_Move::State::eAttackPlayer)
	{
		//CA_TRACE("[여기 들어오나?");
	}
}

void Zombie_Runner_AI::OnRender()
{
}

void Zombie_Runner_AI::Initialize()
{
	m_Enemy = m_pMyObject->GetComponent<Zombie_Runner_Move>();

	// Root Node
	m_Root->AddChild(m_Selector.get());

	// Dead
	m_Selector->AddChild(m_SeqDead.get());
	{
		m_SeqDead->AddChild(m_IsDead.get());
	}

	// Assassinated
	m_Selector->AddChild(m_IsAssassinated.get());
	{

	}

	// Return
	m_Selector->AddChild(m_IsReturn.get());
	{
		// SeqReturn
		m_IsReturn->SetChild(m_SeqReturn.get());
		{
			// MoveToReturnPoint
			m_SeqReturn->AddChild(m_MoveToReturnPoint.get());

			// FinishReturn
			m_SeqReturn->AddChild(m_FinishReturn.get());

		}
	}

	// Explore
	m_Selector->AddChild(m_Explore.get());

	// AwakenSight
	m_Selector->AddChild(m_AwakenSight.get());

	// AwakenSound
	m_Selector->AddChild(m_AwakenSound.get());

	// Combat
	m_Selector->AddChild(m_SelCombat.get());
	{
		/// Attack
		{
			// AttackPartner
			m_SelCombat->AddChild(m_IsAttackPartner.get());
			{
				m_IsAttackPartner->SetChild(m_AttackToPartner.get());
			}

			// AttackPlayer
			m_SelCombat->AddChild(m_IsAttackPlayer.get());
			{
				m_IsAttackPlayer->SetChild(m_AttackToPlayer.get());
			}
		}

		/// Search And Hunt
		m_SelCombat->AddChild(m_SeqSearchAndHunt.get());
		{
			// Search
			m_SeqSearchAndHunt->AddChild(m_SelSearch.get());
			{
				m_SelSearch->AddChild(m_FindPartner.get());
				m_SelSearch->AddChild(m_FindPlayer.get());
			}

			// Hunt
			m_SeqSearchAndHunt->AddChild(m_SeqHunt.get());
			{
				m_SeqHunt->AddChild(m_MoveToTarget.get());
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

