#pragma once
#include <queue>
#include "ComponentBase.h"

/// <summary>
/// A* (AStar) 길 찾기에 사용할 노드 클래스
/// 
/// 작성자 : 최 요 환
/// </summary>

class AStarNode
{
public:
	AStarNode(std::shared_ptr<struct NaviMeshFace> face, std::shared_ptr<AStarNode> parent, std::shared_ptr<AStarNode> endNode);
	~AStarNode();

	int m_Index;										// 노드 번호
	std::shared_ptr<struct NaviMeshFace> m_Face;		// 노드에 해당하는 NavFace
	std::shared_ptr<AStarNode> m_Parent;				// 부모(이전)노드

	float m_G;	// Node까지의 도달 경로
	float m_H;	// 휴리스틱 계산값(Goal 까지의 길이로)
	float m_F;	// F = G + H
};

/// <summary>
/// Open List 에 추가할 Priority Queue 의 조건
/// </summary>
struct AstarNodeCmp
{
	bool operator()(std::shared_ptr<AStarNode> node1, std::shared_ptr<AStarNode> node2)
	{
		if (node1->m_F == node2->m_F)
		{
			// F 비용이 같다면 H 비용이 적은 쪽으로
			return node1->m_H > node2->m_H;
		}

		return node1->m_F > node2->m_F;
	}
};

/// <summary>
/// NavMeshAgent 들의 행동을 관리하는 클래스
/// 
/// NavMesh 위에서 움직이는 오브젝트들은 NavMeshAgent 컴포넌트가 추가되어야 한다.
/// 
/// 오브젝트가 유효한 NavMesh 영역 위에 있는지에 대한 처리 등을 한다.
/// 
/// 작성자 : 최 요 환
/// </summary>

class NavMeshAgent : public ComponentBase
{
public:
	_DLL NavMeshAgent();
	_DLL virtual ~NavMeshAgent();
	
	static class NavMeshManager* s_NavMeshManager;			// 네비메쉬 매니져
	static std::shared_ptr<class NaviMeshSet> s_NavMesh;	// 네비메쉬 매니져에서 선택된 네비메쉬
	
	// ComponentBase override
	virtual void Start() override;
	virtual void Update(float dTime) override;
	virtual void OnRender() override;

	// 마지막으로 위치해 있던 NavMeshFace 초기화(NavMeshManager에서 호출해준다)
	void ResetLastOnNavMeshFace();

	/// 네비메쉬 위에 있는가?
	std::shared_ptr<struct NaviMeshFace> IsOnNavMesh(SimpleMath::Vector3 position);		// 전체 네비메쉬 검사
	bool IsOnNavMeshFace(std::shared_ptr<NaviMeshFace>& face, SimpleMath::Vector3 position);	// 부분 Face(Triangle) 검사

	/// 목적지 설정 함수
	_DLL bool SetDestinationObj(GameObject* object);
	_DLL bool SetDestinationPos(Vector3 position);
	std::shared_ptr<struct NaviMeshFace> GetLastOnNavMeshFace() const;

	/// Debug 출력 여부
	_DLL bool GetDebugMode() const;
	_DLL void SetDebugMode(const bool val);

	_DLL void RenderFaceOnAgent(std::shared_ptr<IRenderer> iRenderer) const;
	_DLL void RenderPathAgentToDestination() const;

	_DLL float GetMoveSpeed() const;
	_DLL void SetMoveSpeed(const float moveSpeed);

public:
	// Agent 이동 속도
	float m_MoveSpeed;
	PhysicsActor* m_MyPhysicsActor;

protected:

private:
	// Agent의 위치(y)를 페이스 위에서 보정하는 기능
	bool AdjustAgentPositionY();
	
	// Agent에서 가장 가까운 Face를 찾는다.(Face Center 를 기준으로)
	std::shared_ptr<struct NaviMeshFace> FindNearNavFace(SimpleMath::Vector3 position);

	/// Agent 위치 조정용 함수
	bool MoveToNearNavFaceCenter();			// 가장 가까운 Face의 CenterPos로 이동 (긴급탈출 같은 함수)
	bool MoveToNearNavFaceLineVer1();		// 가장 가까운 Line 위의 pos 으로 이동 Ver 1
	bool MoveToNearNavFaceLineVer2();		// 가장 가까운 Line 위의 pos 으로 이동 Ver 2
	bool MoveToNearNavFaceLineVer3();		// 가장 가까운 Line 위의 pos 으로 이동 Ver 3
	bool MoveToNearVertex();				// 가장 가까운 Vertex로 이동 

	// 현재 버전에서는 사용하지 않는다..
	bool ChangeLastOnNavMeshFaceToAdjacentNavFace();			// m_LastOntNavMeshFace 를 현재 Face에서 같은버텍스를 공유하는 인접 Face로 이동


	/// Destination Path Find
	/// 설정한 목적지를 경로탐색에 사용할 NavMesh 상의 위치로 매핑(계산)
	bool CalcDestinationOnNavMesh();	
	bool IsAgentOnPath();
	
	// OpenList 를 List로 만든 버전
	std::list<std::shared_ptr<AStarNode>> PathFindAStar(std::shared_ptr<struct NaviMeshFace> startFace, std::shared_ptr<struct NaviMeshFace> endFace);
	void AddOpenList(std::list<std::shared_ptr<AStarNode>>& openList, std::list<std::shared_ptr<AStarNode>>& closedList, std::shared_ptr<struct NaviMeshFace> face, std::shared_ptr<class AStarNode> parentNode, std::shared_ptr<AStarNode> endNode);
	
	// OpenList 를 우선순위큐로 만든 버전
	std::list<std::shared_ptr<AStarNode>> PathFindAStar2(std::shared_ptr<struct NaviMeshFace> startFace, std::shared_ptr<struct NaviMeshFace> endFace);
	void AddOpenList2(std::priority_queue<std::shared_ptr<AStarNode>, std::vector<std::shared_ptr<AStarNode>>, AstarNodeCmp>& openList, std::list<std::shared_ptr<AStarNode>>& closedList, std::shared_ptr<struct NaviMeshFace> face, std::shared_ptr<class AStarNode> parentNode, std::shared_ptr<AStarNode> endNode);

	void MoveToDestinationVer1();	// 목적지를 향해 자동 이동
	void MoveToDestinationVer2();	// 목적지를 향해 자동 이동

	void MoveByPhysX(PhysicsActor* physActor, SimpleMath::Vector3& dir);	// Agent가 physActor 있을때 이동하는 함수

	// Debug 정보
	void PrintLastOnFaceInfo() const;


private:
	/// Debug 출력 관련
	bool m_bDebugMode;

	// 마지막으로 Agnent 가 위치해있던 Face(Triangle)
	std::shared_ptr<struct NaviMeshFace> m_LastOnNavMeshFace;	
	
	/// 충돌판정에 사용할 데이터
	SimpleMath::Vector3 m_LastCycleEdgeVector;					// 마지막으로 있던 Face에서 가장 가까웠던 Edge의 유향 벡터
	SimpleMath::Vector3 m_StartPositionOfLastCycleEdge;			// 마지막으로 있던 Edge의 시작점
	bool m_bOnNavMesh;											// Face와 충돌한 상태인지 판별 변수(매 프레임마다 갱신됨)

																
/// Path Find 관련
public:
	// 목적지가 있으면 경로를 설정(Path Find)하고 이동한다.
	bool m_bAutoMoveMode;
	GameObject* m_DestinationObj;									// 설정할 Agent의 대상 목적지
	SimpleMath::Vector3 m_DestinationPos;							// 설정할 Agent의 대상 목적지
	
private:
	SimpleMath::Vector3  m_DestinationPosOnNavMesh;				// Path Find 할 NavMesh 상의 위치
	std::shared_ptr<struct NaviMeshFace> m_DestinationFace;		// 목적지가 있는 NavMeshFace
	std::list<std::shared_ptr<AStarNode>> m_Path;				// Face 이동 경로

};

