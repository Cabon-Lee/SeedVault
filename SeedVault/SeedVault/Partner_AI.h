#pragma once
#include "ComponentBase.h"
#include "BehaviorTree_Base.h"

#pragma region BehaviorTree_Partner
namespace BehaviorTree
{
	namespace Partner
	{
		#pragma region Decorator
		/// Wait �������� �˻�
		class IsWait : public Decorator
		{
		public:
			IsWait(class Partner_Move* partner) : m_Partner(partner) {}
			virtual ~IsWait() { m_Partner = nullptr; }

			virtual bool CheckCondition() override;

		private:
			class Partner_Move* m_Partner;
		};

		/// Follow �������� �˻�
		class IsFollow : public Decorator
		{
		public:
			IsFollow(class Partner_Move* partner) : m_Partner(partner) {}
			virtual ~IsFollow() { m_Partner = nullptr; }

			virtual bool CheckCondition() override;

		private:
			class Partner_Move* m_Partner;
		};

		/// Hit �������� �˻�
		class IsHit : public Decorator
		{
		public:
			IsHit(class Partner_Move* partner) : m_Partner(partner) {}
			virtual ~IsHit() { m_Partner = nullptr; }

			virtual bool CheckCondition() override;

		private:
			class Partner_Move* m_Partner;
		};

		class Hit : public Node
		{
		public:
			Hit(class Partner_Move* partner) : m_Partner(partner) {}
			virtual ~Hit() { m_Partner = nullptr; }

			virtual bool Invoke() override;

		private:
			class Partner_Move* m_Partner;
		};

		/// Escape �������� �˻�
		class IsEscape : public Decorator
		{
		public:
			IsEscape(class Partner_Move* partner) : m_Partner(partner) {}
			virtual ~IsEscape() { m_Partner = nullptr; }

			virtual bool CheckCondition() override;

		private:
			class Partner_Move* m_Partner;
		};

		class Escape : public Node
		{
		public:
			Escape(class Partner_Move* partner) : m_Partner(partner) {}
			virtual ~Escape() { m_Partner = nullptr; }

			virtual bool Invoke() override;

		private:
			class Partner_Move* m_Partner;
		};
		#pragma endregion Decorator

		/// Dead �������� �˻�
		class IsDead : public Node
		{
		public:
			IsDead(class Partner_Move* partner) : m_Partner(partner) {}
			virtual ~IsDead() { m_Partner = nullptr; }

			virtual bool Invoke() override;

		private:
			class Partner_Move* m_Partner;
		};

		

		#pragma region SeqFollow
		/// Follow �������� �˻�
		class MoveToPlayer : public Node
		{
		public:
			MoveToPlayer(class Partner_Move* partner) : m_Partner(partner) {}
			virtual ~MoveToPlayer() { m_Partner = nullptr; }

			virtual bool Invoke() override;

		private:
			class Partner_Move* m_Partner;
		};
		#pragma endregion SeqFollow
	}
}
#pragma endregion BehaviorTree_Partner



/// <summary>
/// Partner__AI Ŭ����
/// 
/// ������ �ൿ�� ��Ȳ�� ���� �����Ѵ�.
/// 
/// �ۼ��� : �� �� ȯ
/// </summary>
class Partner_AI : public ComponentBase
{
public:
	Partner_AI();
	virtual ~Partner_AI();

	virtual void Start() override;
	virtual void Update(float dTime) override;
	virtual void OnRender() override;

	void Initialize();

private:
	class Partner_Move* m_Partner;

	class std::unique_ptr<BehaviorTree::Sequence> m_Root;
	class std::unique_ptr<BehaviorTree::Selector> m_Selector;

	class std::unique_ptr<BehaviorTree::Partner::IsDead> m_IsDead;

	class std::unique_ptr<BehaviorTree::Partner::IsHit> m_IsHit;
	class std::unique_ptr<BehaviorTree::Partner::Hit> m_Hit;

	class std::unique_ptr<BehaviorTree::Partner::IsEscape> m_IsEscape;
	class std::unique_ptr<BehaviorTree::Partner::Escape> m_Escape;

	class std::unique_ptr<BehaviorTree::Partner::IsWait> m_IsWait;

	class std::unique_ptr<BehaviorTree::Partner::IsFollow> m_IsFollow;
	class std::unique_ptr<BehaviorTree::Partner::MoveToPlayer> m_MoveToPlayer;
};

