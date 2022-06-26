#pragma once

/// <summary>
/// Behavior Tree Base
/// NPC 들의 행동 전환을 위한 기초 클래스
/// 
/// 작성자 : 최 요 환
/// 
/// </summary>

namespace BehaviorTree
{
#pragma region BasicNode
	/// 베이스 Node 클래스
	class Node abstract
	{
	public:
		virtual bool Invoke() abstract;
	};

	/// 복합 노드
	class CompositeNode : public Node
	{
	public:
		virtual bool Invoke() override abstract;

		void AddChild(Node* m_Child)
		{
			m_Children.emplace_back(m_Child);
		}

		const std::vector<Node*>& GetChildren() const
		{
			return m_Children;
		}

	private:
		// 자식 노드(행동)들, 원래는 stack으로 구현하려고 했지만 코드 작성에서 막히는 부분이 있어 일단 vector로 구현.
		std::vector<Node*> m_Children;
	};

	/// 셀렉터(선택자)
	/// 자식 노드들 중 한 개라도 true 리턴되면 true를 리턴
	class Selector : public CompositeNode
	{
	public:
		Selector() = default;
		~Selector() = default;

		virtual bool Invoke() override
		{
			for (Node* node : GetChildren())
			{
				if (node->Invoke())
				{
					return true;
				}
			}

			return false;
		}

		/*
		virtual bool Invoke() override
		{
			
			//for (auto node : GetChildren())
			for(unsigned int i = 0; i < GetChildren().size(); i++)
			{
				// 쉐어드 ptr을 사용해서 이 스코프가 끝날때 해제가 되도록함
				std::shared_ptr<Node> _shaderPtr;
				_shaderPtr.reset(GetChildren().at(i).get());

				if (_shaderPtr->Invoke())
				{
					return true;
				}
			}

			return false;
		
			
		}
		//*/
	};

	/// 모든 자식 노드들이 true를 리턴해야 true 리턴
	/// 하나라도 false 리턴되면 false 리턴
	class Sequence : public CompositeNode
	{
	public:
		Sequence() = default;
		~Sequence() = default;

		virtual bool Invoke() override
		{
			for (Node* m_Child : GetChildren())
			{
				if (m_Child->Invoke() == false)
				{
					return false;
				}
			}

			return true;
		}
	};

	/// 조건 노드
	/// 지금은 단순히 조건을 판단해서 자식노드를 실행할지 말지에 대한
	/// ConditionNode 의 역할만 하고 있음.
	/// 추후에 Decorator 들이 세분화 된다면(컨버터, 페러랠? 같은 개념들) 구조조정할 필요가 있음.
	class Decorator : public Node
	{
	public:
		Decorator() : m_Child(nullptr) {}
		~Decorator() = default;

		void SetChild(Node* newChild)
		{
			assert(m_Child == nullptr);

			m_Child = newChild;
		}

		virtual bool CheckCondition() abstract;

		virtual bool Invoke() override 
		{ 
			if (CheckCondition())
			{
				return m_Child->Invoke(); 
			}
			else
			{
				return false;
			}
		}

	protected:
		Node* GetChild() const
		{
			return m_Child;
		}

	private:
		Node* m_Child;	// Only one child allowed
	};

	// 쓸지 안쓸지 모르겠다.
	// 그냥 시퀀스로 루트 만들어 쓸지도..
	class Root : public Decorator
	{
	private:
		friend class BehaviorTree;
		
		virtual bool Invoke() override
		{
			return GetChild()->Invoke();
		}
	};
#pragma endregion BasicNode
	
};