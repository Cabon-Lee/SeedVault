#pragma once

/// <summary>
/// Zombie_Runner_AI 클래스 초안
/// 
/// 일단 노드 추가하고 돌아가는지 확인하자
/// 
/// 작성자 : YoKing
/// </summary>
#include "Enemy_AI.h"
#include "BehaviorTree_Base.h"
#pragma region BehaviorTree_EnemyNode
namespace BehaviorTree
{
	namespace Enemy
	{
		#pragma region Decorator
		
		/// ePatrol 상태인지 검사
		class IsPartol : public Decorator
		{
		public:
			IsPartol(class Enemy_Move* enemy) : m_Enemy(enemy) {}
			virtual ~IsPartol() { m_Enemy = nullptr; }

			virtual bool CheckCondition() override;

		private:
			class Enemy_Move* m_Enemy;
		};

		/// Wait 상태인지 검사
		class IsWait : public Decorator
		{
		public:
			IsWait(class Enemy_Move* enemy) : m_Enemy(enemy) {}
			virtual ~IsWait() { m_Enemy = nullptr; }

			virtual bool CheckCondition() override;

		private:
			class Enemy_Move* m_Enemy;
		};
		
		/// Attack 상태 검사
		class IsAttack : public Decorator
		{
		public:
			IsAttack(class Enemy_Move* enemy) : m_Enemy(enemy) {}
			virtual ~IsAttack() { m_Enemy = nullptr; }

			virtual bool CheckCondition() override;

		private:
			class Enemy_Move* m_Enemy;
		};

		/// Return 상태 검사
		class IsReturn : public Decorator
		{
		public:
			IsReturn(class Enemy_Move* enemy) : m_Enemy(enemy) {}
			virtual ~IsReturn() { m_Enemy = nullptr; }

			virtual bool CheckCondition() override;

		private:
			class Enemy_Move* m_Enemy;
		};
		#pragma endregion Decorator

		#pragma region SeqDead
		/// 죽었는지 검사
		class IsDead : public Node
		{
		public:
			IsDead(class Enemy_Move* enemy) : m_Enemy(enemy) {}
			virtual ~IsDead() { m_Enemy = nullptr; }	// 원본 컴포넌트는 해당 컴포넌트의 소멸자에서 소멸되므로 SAFE_DELETE() 하면 안된다. 
														// nullptr을 가리키는것도 아니고 이미 해재된 메모리를 가리키는 댕글링 상태에서 해제를 시도하기 떄문...
														// std::shared_ptr 을 사용할까 생각해봤지만 컴포넌트 시스템 자체가 바뀌어야 하므로 fail... 일단 Node 들은 데이터에 대한 포인터를 nullptr로 바꿔주는 것만 해주자.
			virtual bool Invoke() override;

		private:
			class Enemy_Move* m_Enemy;
		};
		#pragma endregion SeqDead

		#pragma region SeqHunt
		class MoveForPlayer : public Node
		{
		public:
			MoveForPlayer(class Enemy_Move* zombieA) : m_Enemy(zombieA) {}
			virtual ~MoveForPlayer() { m_Enemy = nullptr; }

			virtual bool Invoke() override;

		private:
			class Enemy_Move* m_Enemy;
		};
		#pragma endregion SeqHunt

		#pragma region SeqFind
		class FindPlayer : public Node
		{
		public:
			FindPlayer(class Enemy_Move* enemy) : m_Enemy(enemy) {}
			virtual ~FindPlayer() { m_Enemy = nullptr; }

			virtual bool Invoke() override;

		private:
			class Enemy_Move* m_Enemy;
		};

		#pragma endregion SeqFind

		#pragma region SeqAttack
		class AttackToPlayer : public Node
		{
		public:
			AttackToPlayer(class Enemy_Move* enemy) : m_Enemy(enemy) {}
			virtual ~AttackToPlayer() { m_Enemy = nullptr; }

			virtual bool Invoke() override;

		private:
			class Enemy_Move* m_Enemy;
		};
		#pragma region SeqAttack


		#pragma region SeqPatrol
		class UpdateTargetWayPoint : public Node
		{
		public:
			UpdateTargetWayPoint(class Enemy_Move* enemy) : m_Enemy(enemy) {}
			virtual ~UpdateTargetWayPoint() { m_Enemy = nullptr; }

			virtual bool Invoke() override;

		private:
			class Enemy_Move* m_Enemy;
		};

		class MoveForTargetWayPoint : public Node
		{
		public:
			MoveForTargetWayPoint(class Enemy_Move* enemy) : m_Enemy(enemy) {}
			virtual ~MoveForTargetWayPoint() { m_Enemy = nullptr; }

			virtual bool Invoke() override;

		private:
			class Enemy_Move* m_Enemy;
		};
		#pragma endregion SeqPatrol

		class Wait : public Node
		{
		public:
			Wait(class Enemy_Move* enemy) : m_Enemy(enemy) {}
			virtual ~Wait() { m_Enemy = nullptr; }

			virtual bool Invoke() override;

		private:
			class Enemy_Move* m_Enemy;
		};
	}
}
#pragma endregion BehaviorTree_EnemyNode

/// <summary>
/// 적의 A.I 를 구성하는 클래스
/// 
/// BehaviorTree 기반의 노드를 생성해서 A.I를 구성한다.
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
	class std::unique_ptr<BehaviorTree::Sequence> m_Root;
	class std::unique_ptr <BehaviorTree::Selector> m_Selector;
	//class BehaviorTree::Sequence* m_SeqMovingAttack;

	class std::unique_ptr<BehaviorTree::Sequence> m_SeqDead;
	class std::unique_ptr<BehaviorTree::Enemy::IsDead> m_IsDead;

	class std::unique_ptr<BehaviorTree::Selector> m_SelCombat;
	class std::unique_ptr<BehaviorTree::Enemy::IsAttack> m_IsAttack;
	class std::unique_ptr<BehaviorTree::Enemy::AttackToPlayer> m_AttackToPlayer;
	class std::unique_ptr<BehaviorTree::Enemy::IsReturn> m_IsReturn;
	class std::unique_ptr<BehaviorTree::Sequence> m_SeqSearchAndHunt;
	class std::unique_ptr<BehaviorTree::Sequence> m_SeqSearch;
	class std::unique_ptr<BehaviorTree::Enemy::FindPlayer> m_FindPlayer;
	class std::unique_ptr<BehaviorTree::Sequence> m_SeqHunt;
	class std::unique_ptr<BehaviorTree::Enemy::MoveForPlayer> m_MoveForPlayer;
	

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
