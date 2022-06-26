#include "pch.h"
#include "Zombie_Runner_Move.h"
#include "Enemy_AI.h"


bool BehaviorTree::Enemy::IsDead::Invoke()
{
	return m_Enemy->IsDead();
}

bool BehaviorTree::Enemy::IsWait::CheckCondition()
{
	return m_Enemy->IsWait();
}

bool BehaviorTree::Enemy::MoveToTarget::Invoke()
{
	return m_Enemy->MoveToTarget();
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

bool BehaviorTree::Enemy::IsAttackPlayer::CheckCondition()
{
	return m_Enemy->IsAttackPlayer();
}

bool BehaviorTree::Enemy::AttackToPlayer::Invoke()
{
	return m_Enemy->AttackToPlayer();
}

bool BehaviorTree::Enemy::IsReturn::CheckCondition()
{
	return m_Enemy->IsReturn();
}

bool BehaviorTree::Enemy::MoveToReturnPoint::Invoke()
{
	return m_Enemy->MoveToReturnPoint();
}

bool BehaviorTree::Enemy::FinishReturn::Invoke()
{
	return m_Enemy->FinishReturn();
}

bool BehaviorTree::Enemy::Explore::Invoke()
{
	return m_Enemy->Explore();
}

bool BehaviorTree::Enemy::AwakenSound::Invoke()
{
	return m_Enemy->AwakenSound();
}

bool BehaviorTree::Enemy::IsAssassinated::Invoke()
{
	return m_Enemy->IsAssassinated();
}


Enemy_AI::Enemy_AI()
	: ComponentBase(ComponentType::GameLogic)
	, m_Enemy(nullptr)
{

}
