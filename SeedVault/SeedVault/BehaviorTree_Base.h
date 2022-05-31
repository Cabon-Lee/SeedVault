#pragma once

/// <summary>
/// Behavior Tree Base
/// NPC ���� �ൿ ��ȯ�� ���� ���� Ŭ����
/// 
/// �ۼ��� : YoKing
/// 
/// </summary>

namespace BehaviorTree
{
#pragma region BasicNode
	/// ���̽� Node Ŭ����
	class Node abstract
	{
	public:
		virtual bool Invoke() abstract;
	};

	/// ���� ���
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
		// �ڽ� ���(�ൿ)��, ������ stack���� �����Ϸ��� ������ �ڵ� �ۼ����� ������ �κ��� �־� �ϴ� vector�� ����.
		std::vector<Node*> m_Children;
	};

	/// ������(������)
	/// �ڽ� ���� �� �� ���� true ���ϵǸ� true�� ����
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
				// ����� ptr�� ����ؼ� �� �������� ������ ������ �ǵ�����
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

	/// ��� �ڽ� ������ true�� �����ؾ� true ����
	/// �ϳ��� false ���ϵǸ� false ����
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

	/// ���� ���
	/// ������ �ܼ��� ������ �Ǵ��ؼ� �ڽĳ�带 �������� ������ ����
	/// ConditionNode �� ���Ҹ� �ϰ� ����.
	/// ���Ŀ� Decorator ���� ����ȭ �ȴٸ�(������, �䷯��? ���� �����) ���������� �ʿ䰡 ����.
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

	// ���� �Ⱦ��� �𸣰ڴ�.
	// �׳� �������� ��Ʈ ����� ������..
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