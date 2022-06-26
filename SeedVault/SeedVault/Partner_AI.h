#pragma once
#include "ComponentBase.h"
#include "BehaviorTree_Base.h"

#pragma region BehaviorTree_Partner
namespace BehaviorTree
{
	namespace Partner
	{
		#pragma region Decorator
		/// Wait 상태인지 검사
		class IsWait : public Decorator
		{
		public:
			IsWait(class Partner_Move* partner) : m_Partner(partner) {}
			virtual ~IsWait() { m_Partner = nullptr; }

			virtual bool CheckCondition() override;

		private:
			class Partner_Move* m_Partner;
		};

		/// Follow 상태인지 검사
		class IsFollow : public Decorator
		{
		public:
			IsFollow(class Partner_Move* partner) : m_Partner(partner) {}
			virtual ~IsFollow() { m_Partner = nullptr; }

			virtual bool CheckCondition() override;

		private:
			class Partner_Move* m_Partner;
		};

		/// Hit 상태인지 검사
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

		/// Escape 상태인지 검사
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

		/// Dead 상태인지 검사
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
		/// Follow 상태인지 검사
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
/// Partner__AI 클래스
/// 
/// 조수의 행동을 상황에 따라 결정한다.
/// 
/// 작성자 : 최 요 환
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

