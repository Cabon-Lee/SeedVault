#pragma once
#include "ComponentBase.h"
#include "BehaviorTree_Base.h"

/// <summary>
/// ���� A.I �� �����ϴ� Ŭ����
/// 
/// BehaviorTree ����� ��带 ����
/// �õ庼Ʈ ���ӿ� �����ϴ� ��� Enemy(Zombie)�� ����� �ൿ�鿡 ���� ��带 �ۼ�
/// 
/// �ۼ��� : �� �� ȯ
/// </summary>

namespace BehaviorTree
{
	namespace Enemy
	{
		#pragma region Decorator
		/// ePatrol �������� �˻�
		class IsPartol : public Decorator
		{
		public:
			IsPartol(class Enemy_Move* enemy) : m_Enemy(enemy) {}
			virtual ~IsPartol() { m_Enemy = nullptr; }

			virtual bool CheckCondition() override;

		private:
			class Enemy_Move* m_Enemy;
		};

		/// Wait �������� �˻�
		class IsWait : public Decorator
		{
		public:
			IsWait(class Enemy_Move* enemy) : m_Enemy(enemy) {}
			virtual ~IsWait() { m_Enemy = nullptr; }

			virtual bool CheckCondition() override;

		private:
			class Enemy_Move* m_Enemy;
		};


		/// AttackPlayer ���� �˻�
		class IsAttackPlayer : public Decorator
		{
		public:
			IsAttackPlayer(class Enemy_Move* enemy) : m_Enemy(enemy) {}
			virtual ~IsAttackPlayer() { m_Enemy = nullptr; }

			virtual bool CheckCondition() override;

		private:
			class Enemy_Move* m_Enemy;
		};

		/// MoveToReturnPoint ���� �˻�
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
		/// �׾����� �˻�
		class IsDead : public Node
		{
		public:
			IsDead(class Enemy_Move* enemy) : m_Enemy(enemy) {}
			virtual ~IsDead() { m_Enemy = nullptr; }	// ���� ������Ʈ�� �ش� ������Ʈ�� �Ҹ��ڿ��� �Ҹ�ǹǷ� SAFE_DELETE() �ϸ� �ȵȴ�. 
														// nullptr�� ����Ű�°͵� �ƴϰ� �̹� ����� �޸𸮸� ����Ű�� ��۸� ���¿��� ������ �õ��ϱ� ����...
														// std::shared_ptr �� ����ұ� �����غ����� ������Ʈ �ý��� ��ü�� �ٲ��� �ϹǷ� fail... �ϴ� Node ���� �����Ϳ� ���� �����͸� nullptr�� �ٲ��ִ� �͸� ������.
			virtual bool Invoke() override;

		private:
			class Enemy_Move* m_Enemy;
		};
		#pragma endregion SeqDead

		#pragma region SeqAssassinated
		/// �׾����� �˻�
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

