#pragma once
#include <queue>
#include "ComponentBase.h"

/// <summary>
/// A* (AStar) �� ã�⿡ ����� ��� Ŭ����
/// 
/// �ۼ��� : YoKing
/// </summary>

class AStarNode
{
public:
	AStarNode(std::shared_ptr<struct NaviMeshFace> face, std::shared_ptr<AStarNode> parent, std::shared_ptr<AStarNode> endNode);
	~AStarNode();

	int m_Index;										// ��� ��ȣ
	std::shared_ptr<struct NaviMeshFace> m_Face;		// ��忡 �ش��ϴ� NavFace
	std::shared_ptr<AStarNode> m_Parent;				// �θ�(����)���

	float m_G;	// Node������ ���� ���
	float m_H;	// �޸���ƽ ��갪(Goal ������ ���̷�)
	float m_F;	// F = G + H
};

/// <summary>
/// Open List �� �߰��� Priority Queue �� ����
/// </summary>
struct AstarNodeCmp
{
	bool operator()(std::shared_ptr<AStarNode> node1, std::shared_ptr<AStarNode> node2)
	{
		if (node1->m_F == node2->m_F)
		{
			// F ����� ���ٸ� H ����� ���� ������
			return node1->m_H > node2->m_H;
		}

		return node1->m_F > node2->m_F;
	}
};

/// <summary>
/// NavMeshAgent ���� �ൿ�� �����ϴ� Ŭ����
/// 
/// NavMesh ������ �����̴� ������Ʈ���� NavMeshAgent ������Ʈ�� �߰��Ǿ�� �Ѵ�.
/// 
/// ������Ʈ�� ��ȿ�� NavMesh ���� ���� �ִ����� ���� ó�� ���� �Ѵ�.
/// 
/// �ۼ��� : YoKing
/// </summary>

class NavMeshAgent : public ComponentBase
{
public:
	_DLL NavMeshAgent();
	_DLL virtual ~NavMeshAgent();
	
	static class NavMeshManager* s_NavMeshManager;			// �׺�޽� �Ŵ���
	static std::shared_ptr<class NaviMeshSet> s_NavMesh;	// �׺�޽� �Ŵ������� ���õ� �׺�޽�
	
	// ComponentBase override
	virtual void Start() override;
	virtual void Update(float dTime) override;
	virtual void OnRender() override;

	// ���������� ��ġ�� �ִ� NavMeshFace �ʱ�ȭ(NavMeshManager���� ȣ�����ش�)
	void ResetLastOnNavMeshFace();

	/// �׺�޽� ���� �ִ°�?
	std::shared_ptr<struct NaviMeshFace> IsOnNavMesh(SimpleMath::Vector3 position);		// ��ü �׺�޽� �˻�
	bool IsOnNavMeshFace(std::shared_ptr<NaviMeshFace>& face, SimpleMath::Vector3 position);	// �κ� Face(Triangle) �˻�

	/// ������ ���� �Լ�
	_DLL bool SetDestination(GameObject* object);
	std::shared_ptr<struct NaviMeshFace> GetLastOnNavMeshFace() const;

	/// Debug ��� ����
	_DLL bool GetDebugMode() const;
	_DLL void SetDebugMode(const bool val);

	_DLL void RenderFaceOnAgent(std::shared_ptr<IRenderer> iRenderer) const;
	_DLL void RenderPathAgentToDestination() const;

	_DLL float GetMoveSpeed() const;
	_DLL void SetMoveSpeed(const float moveSpeed);

public:
	// Agent �̵� �ӵ�
	float m_MoveSpeed;
	PhysicsActor* m_MyPhysicsActor;

protected:

private:
	// Agent�� ��ġ(y)�� ���̽� ������ �����ϴ� ���
	bool AdjustAgentPositionY();
	
	// Agent���� ���� ����� Face�� ã�´�.(Face Center �� ��������)
	std::shared_ptr<struct NaviMeshFace> FindNearNavFace(SimpleMath::Vector3 position);

	/// Agent ��ġ ������ �Լ�
	bool MoveToNearNavFaceCenter();			// ���� ����� Face�� CenterPos�� �̵� (���Ż�� ���� �Լ�)
	bool MoveToNearNavFaceLineVer1();		// ���� ����� Line ���� pos ���� �̵� Ver 1
	bool MoveToNearNavFaceLineVer2();		// ���� ����� Line ���� pos ���� �̵� Ver 2
	bool MoveToNearNavFaceLineVer3();		// ���� ����� Line ���� pos ���� �̵� Ver 3
	bool MoveToNearVertex();				// ���� ����� Vertex�� �̵� 

	// ���� ���������� ������� �ʴ´�..
	bool ChangeLastOnNavMeshFaceToAdjacentNavFace();			// m_LastOntNavMeshFace �� ���� Face���� �������ؽ��� �����ϴ� ���� Face�� �̵�


	/// Destination Path Find
	/// ������ �������� ���Ž���� ����� NavMesh ���� ��ġ�� ����(���)
	bool CalcDestinationOnNavMesh();	
	bool IsAgentOnPath();
	
	// OpenList �� List�� ���� ����
	std::list<std::shared_ptr<AStarNode>> PathFindAStar(std::shared_ptr<struct NaviMeshFace> startFace, std::shared_ptr<struct NaviMeshFace> endFace);
	void AddOpenList(std::list<std::shared_ptr<AStarNode>>& openList, std::list<std::shared_ptr<AStarNode>>& closedList, std::shared_ptr<struct NaviMeshFace> face, std::shared_ptr<class AStarNode> parentNode, std::shared_ptr<AStarNode> endNode);
	
	// OpenList �� �켱����ť�� ���� ����
	std::list<std::shared_ptr<AStarNode>> PathFindAStar2(std::shared_ptr<struct NaviMeshFace> startFace, std::shared_ptr<struct NaviMeshFace> endFace);
	void AddOpenList2(std::priority_queue<std::shared_ptr<AStarNode>, std::vector<std::shared_ptr<AStarNode>>, AstarNodeCmp>& openList, std::list<std::shared_ptr<AStarNode>>& closedList, std::shared_ptr<struct NaviMeshFace> face, std::shared_ptr<class AStarNode> parentNode, std::shared_ptr<AStarNode> endNode);

	void MoveToDestinationVer1();	// �������� ���� �ڵ� �̵�
	void MoveToDestinationVer2();	// �������� ���� �ڵ� �̵�

	void MoveByPhysX(PhysicsActor* physActor, SimpleMath::Vector3& dir);	// Agent�� physActor ������ �̵��ϴ� �Լ�

	// Debug ����
	void PrintLastOnFaceInfo() const;


private:
	/// Debug ��� ����
	bool m_bDebugMode;

	// ���������� Agnent �� ��ġ���ִ� Face(Triangle)
	std::shared_ptr<struct NaviMeshFace> m_LastOnNavMeshFace;	
	
	/// �浹������ ����� ������
	SimpleMath::Vector3 m_LastCycleEdgeVector;					// ���������� �ִ� Face���� ���� ������� Edge�� ���� ����
	SimpleMath::Vector3 m_StartPositionOfLastCycleEdge;			// ���������� �ִ� Edge�� ������
	bool m_bOnNavMesh;											// Face�� �浹�� �������� �Ǻ� ����(�� �����Ӹ��� ���ŵ�)

																
/// Path Find ����
public:
	// �������� ������ ��θ� ����(Path Find)�ϰ� �̵��Ѵ�.
	GameObject* m_DestinationObj;									// ������ Agent�� ��� ������
	SimpleMath::Vector3* m_DestinationPos;							// ������ Agent�� ��� ������
	
private:
	SimpleMath::Vector3  m_DestinationPosOnNavMesh;				// Path Find �� NavMesh ���� ��ġ
	std::shared_ptr<struct NaviMeshFace> m_DestinationFace;		// �������� �ִ� NavMeshFace
	std::list<std::shared_ptr<AStarNode>> m_Path;				// Face �̵� ���

};

