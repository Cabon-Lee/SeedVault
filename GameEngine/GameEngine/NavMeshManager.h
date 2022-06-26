#pragma once

/// <summary>
/// NavMeshManager 클래스
/// 
/// 게임에서 이동하는데 필요한 NavMeshSet 을 등록해서 갖고 있는다.
/// 필요하면 현재 NavMeshSet 를 교체하거나 제거한다.
/// 
/// 작성자 : 최 요 환
/// 
/// </summary>

__interface IRenderer;
__interface IResourceManager;
class NaviMeshSet;

const bool WIRE = true;
const bool SOLID = false;

const unsigned int WHITE = 0;
const unsigned int RED = 1;
const unsigned int GREEN = 2;
const unsigned int BLUE = 3;
const unsigned int DARKGRAY = 4;
const unsigned int BLACK = 5;

class NavMeshManager
{
public:
	NavMeshManager();
	~NavMeshManager();

	// 네비메쉬 벡터에 네비메쉬 추가
	bool AddNavMesh(std::string name, std::string resourceName);
	bool AddNavMesh(std::string name, std::shared_ptr<NaviMeshSet> navMesh);	
	
	// string 으로 NavMesh 제거
	bool DeleteNavMesh(std::string name);

	// 네비 메쉬 교체
	bool SetCurrentNavMesh(std::string name);	// string에 해당하는 네비메쉬를 찾아서 교체
	bool SetCurrentNavMesh(unsigned int index);	// uint 인덱스로 교체

	// 현재 설정된 NavMesh 리턴
	std::string& GetCurrentNavMeshName();
	std::shared_ptr<NaviMeshSet> GetCurrentNavMesh() const;

	/// NavMeshAgent 관리
	bool RegisterNavMeshAgent(class NavMeshAgent* agent);
	bool DeleteNavMeshAgent(class NavMeshAgent* agent);

	/// Render 
	// 현재 네비메쉬를 렌더
	bool SetRenderer(std::shared_ptr<IRenderer> iRenderer);
	void Render();

	bool GetDebugMode() const;
	void SetDebugMode(const bool mode);
	void ToggleDebugMode();

private:
	std::shared_ptr<IResourceManager> m_pResourceManager;

protected:

private:
	std::shared_ptr<NaviMeshSet> FindNavMesh(std::string name);

	// 네비메쉬들의 리스트
	std::unordered_map<std::string, std::shared_ptr<NaviMeshSet>> m_NavMesh_UM;

	// 현재 선택된 네비메쉬
	std::string m_CurrentNavMeshName;
	std::shared_ptr<NaviMeshSet> m_CurrentNavMesh;

	// 현재 씬에 존재하는 NavMeshAgent들 목록
	std::vector<class NavMeshAgent*> m_NavMeshAgent_V;


	/// 렌더 하기 위한 부분
	bool m_bDebugMode;							// 디버그 모드 유무(렌더 on/off)
	std::shared_ptr<IRenderer> m_pIRenderer;	// I렌더러
	
	DirectX::SimpleMath::Matrix m_WorldTM;

	DirectX::SimpleMath::Vector3 m_Scale;
	DirectX::SimpleMath::Quaternion m_Rotation;
	DirectX::SimpleMath::Vector3 m_Position;

};

