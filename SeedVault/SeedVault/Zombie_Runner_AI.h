#pragma once

/// <summary>
/// Zombie_Runner_AI �� �����ϴ� ��� Ŭ������
/// 
/// �ۼ��� : �� �� ȯ
/// </summary>
#include "Enemy_AI.h"
#include "BehaviorTree_Base.h"
#pragma region BehaviorTree_EnemyNode
namespace BehaviorTree
{
	namespace Zombie_Runner
	{
		/// AttackPartner ���� �˻�
		class IsAttackPartner : public Decorator
		{
		public:
			IsAttackPartner(class Zombie_Runner_Move* zombieRunner) : m_ZombieRunner(zombieRunner) {}
			virtual ~IsAttackPartner() { m_ZombieRunner = nullptr; }

			virtual bool CheckCondition() override;

		private:
			class Zombie_Runner_Move* m_ZombieRunner;
		};

		class AttackToPartner : public Node
		{
		public:
			AttackToPartner(class Zombie_Runner_Move* zombieRunner) : m_ZombieRunner(zombieRunner) {}
			virtual ~AttackToPartner() { m_ZombieRunner = nullptr; }

			virtual bool Invoke() override;

		private:
			class Zombie_Runner_Move* m_ZombieRunner;
		};

		class FindPlayer : public Node
		{
		public:
			FindPlayer(class Zombie_Runner_Move* zombieRunner) : m_ZombieRunner(zombieRunner) {}
			virtual ~FindPlayer() { m_ZombieRunner = nullptr; }

			virtual bool Invoke() override;

		private:
			class Zombie_Runner_Move* m_ZombieRunner;
		};

		class FindPartner : public Node
		{
		public:
			FindPartner(class Zombie_Runner_Move* zombieRunner) : m_ZombieRunner(zombieRunner) {}
			virtual ~FindPartner() { m_ZombieRunner = nullptr; }

			virtual bool Invoke() override;

		private:
			class Zombie_Runner_Move* m_ZombieRunner;
		};

		class AwakenSight : public Node
		{
		public:
			AwakenSight(class Zombie_Runner_Move* zombieRunner) : m_ZombieRunner(zombieRunner) {}
			virtual ~AwakenSight() { m_ZombieRunner = nullptr; }

			virtual bool Invoke() override;

		private:
			class Zombie_Runner_Move* m_ZombieRunner;
		};

	}
}
#pragma endregion BehaviorTree_EnemyNode

/// <summary>
/// Zombie_Runner �� A.I �� �����ϴ� Ŭ����
/// 
/// BehaviorTree ����� ��带 �����ؼ� A.I�� �����Ѵ�.
/// 
/// Zombie_Runner �� �þ�(��)�� ������ �ְ� �÷��̾(�뺴)���Դ� �ֵθ��� ������
/// �������Դ� ������ ������ �Ѵ�
/// 
/// �ۼ��� : �� �� ȯ
/// </summary>
class Zombie_Runner_AI : public Enemy_AI
{
public:
	Zombie_Runner_AI();
	virtual ~Zombie_Runner_AI();

	virtual void Start() override;
	virtual void Update(float dTime) override;
	virtual void OnRender() override;

	void Initialize();

public:
	virtual void SaveData() override;
	virtual void LoadData() override;

protected:
	struct Enemy_AI_Save* m_SaveData;

private:
	//class Zombie_Runner_Move* m_ZombieRunner;

	class std::unique_ptr<BehaviorTree::Sequence> m_Root;
	class std::unique_ptr<BehaviorTree::Selector> m_Selector;
	//class BehaviorTree::Sequence* m_SeqMovingAttack;

	class std::unique_ptr<BehaviorTree::Sequence> m_SeqDead;
	class std::unique_ptr<BehaviorTree::Enemy::IsDead> m_IsDead;

	class std::unique_ptr<BehaviorTree::Enemy::IsAssassinated> m_IsAssassinated;
	
	class std::unique_ptr<BehaviorTree::Enemy::IsReturn> m_IsReturn;
	class std::unique_ptr<BehaviorTree::Sequence> m_SeqReturn;
	class std::unique_ptr<BehaviorTree::Enemy::MoveToReturnPoint> m_MoveToReturnPoint;
	class std::unique_ptr<BehaviorTree::Enemy::FinishReturn> m_FinishReturn;

	class std::unique_ptr<BehaviorTree::Enemy::Explore> m_Explore;

	class std::unique_ptr<BehaviorTree::Zombie_Runner::AwakenSight> m_AwakenSight;
	class std::unique_ptr<BehaviorTree::Enemy::AwakenSound> m_AwakenSound;

	class std::unique_ptr<BehaviorTree::Selector> m_SelCombat;
	class std::unique_ptr<BehaviorTree::Zombie_Runner::IsAttackPartner> m_IsAttackPartner;
	class std::unique_ptr<BehaviorTree::Zombie_Runner::AttackToPartner> m_AttackToPartner;
	class std::unique_ptr<BehaviorTree::Enemy::IsAttackPlayer> m_IsAttackPlayer;
	class std::unique_ptr<BehaviorTree::Enemy::AttackToPlayer> m_AttackToPlayer;
	class std::unique_ptr<BehaviorTree::Sequence> m_SeqSearchAndHunt;
	class std::unique_ptr<BehaviorTree::Selector> m_SelSearch;
	class std::unique_ptr<BehaviorTree::Zombie_Runner::FindPartner> m_FindPartner;
	class std::unique_ptr<BehaviorTree::Zombie_Runner::FindPlayer> m_FindPlayer;
	class std::unique_ptr<BehaviorTree::Sequence> m_SeqHunt;
	class std::unique_ptr<BehaviorTree::Enemy::MoveToTarget> m_MoveToTarget;
	

	class std::unique_ptr<BehaviorTree::Enemy::IsPartol> m_IsPatrol;
	class std::unique_ptr<BehaviorTree::Sequence> m_SeqPatrol;
	class std::unique_ptr<BehaviorTree::Enemy::UpdateTargetWayPoint> m_UpdateTarget;
	class std::unique_ptr<BehaviorTree::Enemy::MoveForTargetWayPoint> m_MoveForTarget;


	
	class std::unique_ptr<BehaviorTree::Enemy::IsWait> m_IsWait;
	class std::unique_ptr<BehaviorTree::Enemy::Wait> m_Wait;
};

struct Enemy_AI_Save
{
	bool m_bEnable;
	uint m_ComponentId;
};

BOOST_DESCRIBE_STRUCT(Enemy_AI_Save, (), (
	m_bEnable,
	m_ComponentId
	))
