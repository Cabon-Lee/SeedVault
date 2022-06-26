#pragma once
#include "ComponentBase.h"
#include "BehaviorTree_Base.h"

/// <summary>
/// 적의 A.I 를 구성하는 클래스
/// 
/// BehaviorTree 기반의 노드를 생성
/// 시드볼트 게임에 등장하는 모든 Enemy(Zombie)의 공통된 행동들에 대한 노드를 작성
/// 
/// 작성자 : 최 요 환
/// </summary>

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


		/// AttackPlayer 상태 검사
		class IsAttackPlayer : public Decorator
		{
		public:
			IsAttackPlayer(class Enemy_Move* enemy) : m_Enemy(enemy) {}
			virtual ~IsAttackPlayer() { m_Enemy = nullptr; }

			virtual bool CheckCondition() override;

		private:
			class Enemy_Move* m_Enemy;
		};

		/// MoveToReturnPoint 상태 검사
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

		#pragma region SeqAssassinated
		/// 죽었는지 검사
		class IsAssassinated : public Node
		{
		public:
			IsAssassinated(class Enemy_Move* enemy) : m_Enemy(enemy) {}
			virtual ~IsAssassinated() { m_Enemy = nullptr; }

			virtual bool Invoke() override;

		private:
			class Enemy_Move* m_Enemy;
		};
		#pragma endregion SeqAssassinated

		#pragma region SeqHunt
		class MoveToTarget : public Node
		{
		public:
			MoveToTarget(class Enemy_Move* zombieA) : m_Enemy(zombieA) {}
			virtual ~MoveToTarget() { m_Enemy = nullptr; }

			virtual bool Invoke() override;

		private:
			class Enemy_Move* m_Enemy;
		};
		#pragma endregion SeqHunt

		#pragma region SeqSearch
		class AwakenSound : public Node
		{
		public:
			AwakenSound(class Enemy_Move* enemy) : m_Enemy(enemy) {}
			virtual ~AwakenSound() { m_Enemy = nullptr; }

			virtual bool Invoke() override;

		private:
			class Enemy_Move* m_Enemy;
		};

		class Explore : public Node
		{
		public:
			Explore(class Enemy_Move* enemy) : m_Enemy(enemy) {}
			virtual ~Explore() { m_Enemy = nullptr; }

			virtual bool Invoke() override;

		private:
			class Enemy_Move* m_Enemy;
		};

		#pragma endregion SeqSearch

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

		#pragma endregion SeqReturn
		class MoveToReturnPoint : public Node
		{
		public:
			MoveToReturnPoint(class Enemy_Move* enemy) : m_Enemy(enemy) {}
			virtual ~MoveToReturnPoint() { m_Enemy = nullptr; }

			virtual bool Invoke() override;

		private:
			class Enemy_Move* m_Enemy;
		};

		class FinishReturn : public Node
		{
		public:
			FinishReturn(class Enemy_Move* enemy) : m_Enemy(enemy) {}
			virtual ~FinishReturn() { m_Enemy = nullptr; }

			virtual bool Invoke() override;

		private:
			class Enemy_Move* m_Enemy;
		};
	#pragma endregion SeqReturn
	}
}

class Enemy_AI : public ComponentBase
{
public:
	Enemy_AI();
	virtual ~Enemy_AI() = default;

	virtual void Start() abstract;
	virtual void Update(float dTime) abstract;
	virtual void OnRender() abstract;

protected:
	class Enemy_Move* m_Enemy;
};

