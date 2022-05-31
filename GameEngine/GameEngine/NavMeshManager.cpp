#include "pch.h"
#include "EngineDLL.h"
#include "IRenderer.h"
#include "NaviMeshFace.h"
#include "IResourceManager.h"
#include "NavMeshAgent.h"
#include "NavMeshManager.h"

NavMeshManager::NavMeshManager()
	: m_NavMesh_UM()
	, m_CurrentNavMeshName()
	, m_CurrentNavMesh(nullptr)
	, m_NavMeshAgent_V()
	, m_bDebugMode(true)
	, m_pIRenderer(nullptr)
	, m_WorldTM()
	, m_Scale(1.0f, 1.0f, 1.0f)
	, m_Rotation(0.0f, 0.0f, 0.0f, 1.0f)
	, m_Position(0.0f, 0.0f, 0.0f)
{
	CA_TRACE("[NavMeshManager] Create NavMeshManager !!");

	m_NavMeshAgent_V.reserve(20);

	m_pIRenderer = DLLEngine::GetEngine()->GetIRenderer();

	// 렌더할 때 필요한 WorldTM 생성
	m_WorldTM =
		DirectX::SimpleMath::Matrix::CreateScale(m_Scale) *
		DirectX::SimpleMath::Matrix::CreateFromQuaternion(m_Rotation) *
		DirectX::SimpleMath::Matrix::CreateTranslation(m_Position);
}

NavMeshManager::~NavMeshManager()
{
	CA_TRACE("[NavMeshManager] Delete NavMeshManager !!");
}

/// <summary>
/// str으로 등록할 이름과 등록할 리소스(메쉬필터) 이름을 받아서 네비메쉬 리스트(UM)에 추가한다.
/// 같은 이름으로 등록된 메시가 이미 있다면 중복해서 추가하지 않는다.
/// </summary>
/// <param name="name"></param>
/// <param name="resourceName"></param>
/// <returns></returns>
bool NavMeshManager::AddNavMesh(std::string name, std::string resourceName)
{
	// 네비메쉬를 만든다.
	std::shared_ptr<IResourceManager> _resourceManager = DLLEngine::GetEngine()->GetResourceManager();
	std::shared_ptr<NaviMeshSet> _newNavMesh = _resourceManager->MakeNeviMesh(const_cast<std::string&>(resourceName));

	// 정상적으로 만들어졌으면
	if (_newNavMesh != nullptr)
	{
		// 리스트(UM)에 등록을 시도한다.
		if (AddNavMesh(name, _newNavMesh))
		{
			CA_TRACE("[NavMeshManager] Succeeded AddNavMesh(%s, %s)", name.c_str(), resourceName.c_str());

			return true;
		}

		// 네비 메쉬는 만들었지만 등록할 수 없는 경우
		CA_TRACE("[NavMeshManager] Failed... AddNavMesh(%s, %s) -> %s 은 이미 등록되어 있다..", name.c_str(), resourceName.c_str(), name.c_str());
		return false;
	}

	// 네비 메쉬를 만드는데 실패한 경우
	CA_TRACE("[NavMeshManager] Failed AddNavMesh(%s, %s) -> %s 라는 resource(meshFilter)가 존재하지 않는다..", name.c_str(), resourceName.c_str(), resourceName.c_str());
	return false;
}

/// <summary>
/// 네비메쉬를 받아서 네비메쉬 리스트(UM)에 추가한다.
/// 같은 이름으로 등록된 메시가 이미 있다면 중복해서 추가하지 않는다.
/// </summary>
/// <param name="name">등록(접근)할 이름</param>
/// <param name="navMesh">네비메쉬 데이터</param>
/// <returns>등록 성공 여부</returns>
bool NavMeshManager::AddNavMesh(std::string name, std::shared_ptr<NaviMeshSet> navMesh)
{
	// 먼저 name 에 해당하는 네비매쉬가 있는지 찾는다.
	std::shared_ptr<NaviMeshSet> _newNavMesh = FindNavMesh(name);

	// 없으면 추가한다!
	if (_newNavMesh == nullptr)
	{
		CA_TRACE("[NavMeshManager] Succeeded AddNavMesh( %s ) !!", name.c_str());
		m_NavMesh_UM.insert(make_pair(name, navMesh));

		// 제일 처음 추가 된 네비메쉬는 자동으로 현재 네비메쉬로 설정해준다.
		if (m_NavMesh_UM.size() == 1)
		{
			m_CurrentNavMeshName = name;
			SetCurrentNavMesh(name);
		}

		return true;
	}

	CA_TRACE("[NavMeshManager] Failed AddNavMesh( %s ).. It already exists...", name.c_str());

	return false;
}

/// <summary>
/// name 으로 네비메쉬를 네비메쉬 리스트(UM)에서 제거한다.
/// </summary>
/// <param name="name">제거할 네비매쉬 이름</param>
bool NavMeshManager::DeleteNavMesh(std::string name)
{
	// 먼저 name 에 해당하는 네비매쉬가 있는지 찾는다.
	std::shared_ptr<NaviMeshSet> _newNavMesh = FindNavMesh(name);

	// 실패!
	if (_newNavMesh == nullptr)
	{
		CA_TRACE("[NavMeshManager] Failed DeleteNavMesh().. There is no such object....");

		return false;
	}

	// 제거
	m_NavMesh_UM.erase(name);
	CA_TRACE("[NavMeshManager] Succeeded DeleteNavMesh( %s ) !!", name.c_str());

	return true;
}

/// <summary>
/// name에 해당하는 네비메쉬를 찾아 현재 네비매쉬로 설정한다.
/// 없으면 교체하지 않는다.
/// </summary>
/// <param name="name">선택할 네비매쉬 이름</param>
/// <returns>교체 성공 여부</returns>
bool NavMeshManager::SetCurrentNavMesh(std::string name)
{
	// 먼저 name 에 해당하는 네비매쉬가 있는지 찾는다.
	std::shared_ptr<NaviMeshSet> _newNavMesh = FindNavMesh(name);

	// 실패!
	if (_newNavMesh == nullptr)
	{
		CA_TRACE("[NavMeshManager] Failed SetCurrentNavMesh() ...");

		return false;
	}

	// 찾은 네비메쉬로 변경
	m_CurrentNavMeshName = name;
	m_CurrentNavMesh = _newNavMesh;

	// 네비메쉬 에이전트들의 네비매쉬도 변경해준다.
	NavMeshAgent::s_NavMesh = _newNavMesh;

	CA_TRACE("[NavMeshManager] Succeese SetCurrentNavMesh() ! -> now Mesh %s", name.c_str());

	return true;
}

/// <summary>
/// index 번호로 네비 메쉬를 교체한다.
/// </summary>
/// <param name="index">번호</param>
/// <returns></returns>
bool NavMeshManager::SetCurrentNavMesh(unsigned int index)
{
	// 반복자 변수 i
	unsigned int i = 0;

	// 네비메쉬UM을 순회
	for (auto navMesh : m_NavMesh_UM)
	{
		// index번째 네비메쉬를 찾으면
		if (i == index)
		{
			// 현재 메쉬로 설정
			m_CurrentNavMeshName = navMesh.first;
			m_CurrentNavMesh = navMesh.second;

			CA_TRACE("[NavMeshManager] Succeese SetCurrentNavMesh() ! -> now Mesh %s", m_CurrentNavMeshName.c_str());

			// 네비메쉬 에이전트들의 네비매쉬도 변경해준다.
			NavMeshAgent::s_NavMesh = navMesh.second;

			// 에이전트들의 LastOnFace 를 초기화한다.
			for (auto agent : m_NavMeshAgent_V)
			{
				agent->ResetLastOnNavMeshFace();
			}

			return true;
		}

		i++;
	}

	CA_TRACE("[NavMeshManager] Failed SetCurrentNavMesh().. Out of range exception");
	return false;
}

/// <summary>
/// 디버그모드 상태 리턴
/// </summary>
/// <returns></returns>
bool NavMeshManager::GetDebugMode() const
{
	return m_bDebugMode;
}

/// <summary>
/// 디버그모드 원하는 값으로 세팅
/// </summary>
/// <param name="mode">세팅할 값</param>
void NavMeshManager::SetDebugMode(const bool mode)
{
	m_bDebugMode = mode;
}

/// <summary>
/// 디버그 모드 반전
/// </summary>
void NavMeshManager::ToggleDebugMode()
{
	m_bDebugMode = !m_bDebugMode;
}

/// <summary>
/// 현재 네비 메쉬의 이름을 리턴
/// </summary>
/// <returns></returns>
std::string& NavMeshManager::GetCurrentNavMeshName()
{
	return m_CurrentNavMeshName;
}

/// <summary>
/// 현재 네비메쉬를 리턴
/// </summary>
/// <returns></returns>
std::shared_ptr<NaviMeshSet> NavMeshManager::GetCurrentNavMesh() const
{
	return m_CurrentNavMesh;
}

/// <summary>
/// NavMeshManager의 Agent 리스트에 agent를 등록한다.
/// </summary>
/// <param name="agent">등록할 agent 포인터</param>
/// <returns>성공여부</returns>
bool NavMeshManager::RegisterNavMeshAgent(NavMeshAgent* agent)
{
	// nullptr 체크
	if (agent == nullptr)
	{
		CA_TRACE("[NavMeshManager] Failed RegisterNavMeshAgent().. agent was nullptr !");
		return false;
	}

	// agent가 등록되어 있는지 검사
	auto iter = std::find(m_NavMeshAgent_V.begin(), m_NavMeshAgent_V.end(), agent);

	// 이미 있으면 등록하지 않음.
	if (iter != m_NavMeshAgent_V.end())
	{
		CA_TRACE("[NavMeshManager] Failed RegisterNavMeshAgent.. already registered agent %p...", agent);
		return false;
	}

	// 없으면 등록한다
	CA_TRACE("[NavMeshManager] Succeeded RegisterNavMeshAgent! agent = %p", agent);
	m_NavMeshAgent_V.push_back(agent);

	return true;
}


/// <summary>
/// NavMeshManager의 Agent 리스트에서 agent를 제거한다.
/// </summary>
/// <param name="agent">제거할 agent 포인터</param>
/// <returns>성공 여부</returns>
bool NavMeshManager::DeleteNavMeshAgent(NavMeshAgent* agent)
{
	// nullptr 체크
	if (agent == nullptr)
	{
		CA_TRACE("[NavMeshManager] Failed RegisterNavMeshAgent().. agent was nullptr !");
		return false;
	}

	// agent가 등록되어 있는지 검사
	auto iter = std::find(m_NavMeshAgent_V.begin(), m_NavMeshAgent_V.end(), agent);

	// 있으면 제거한다.
	if (iter != m_NavMeshAgent_V.end())
	{
		m_NavMeshAgent_V.erase(iter);
		CA_TRACE("[NavMeshManager] Suecceeded DeleteNavMeshAgent! agent = %p...", agent);
		return true;
	}

	CA_TRACE("[NavMeshManager] Failed DeleteNavMeshAgent.. Can not find Agent... %p", agent);
	return false;

}

/// <summary>
/// 내비메쉬 리스트(UM)에서 name에 해당하는 네비매쉬를 찾아 리턴한다.
/// </summary>
/// <param name="name">찾을 내비메쉬의 이름</param>
/// <returns>탐색 성공 여부</returns>
std::shared_ptr<NaviMeshSet> NavMeshManager::FindNavMesh(std::string name)
{
	// 네비매쉬를 찾는다.
	auto _navMeshIter = m_NavMesh_UM.find(name);

	// 네비메쉬를 못 찾았으면 nullptr 리턴
	if (_navMeshIter == m_NavMesh_UM.end())
	{
		CA_TRACE("[NavMeshManager] Failed FindNavMesh()... Can not find %s", name.c_str());
		return nullptr;
	}

	// 찾은 네비메쉬 리턴
	CA_TRACE("[NavMeshManager] Succeeded FindNavMesh( %s ) !", name.c_str());
	return (*_navMeshIter).second;
}

bool NavMeshManager::SetRenderer(std::shared_ptr<IRenderer> iRenderer)
{
	m_pIRenderer = iRenderer;

	return true;
}

/// <summary>
/// 디버그 모드일 때 현재 선택된 네비메쉬를 렌더한다.
/// </summary>
void NavMeshManager::Render()
{
	// 엔진의 디버그 렌더 모드가 꺼져있으면 렌더 하지 않는다.
	if (Managers::GetInstance()->GetComponentSystem()->GetDebugRenderMode() != true)
	{
		return;
	}

	// 네비메쉬 디버그 모드일 때만 그린다.
	if (m_bDebugMode != true)
	{
		return;
	}

	// 현재 메쉬가 없다면 중지
	if (m_CurrentNavMesh == nullptr) return;

	for (size_t i = 0; i < m_CurrentNavMesh->GetIndexSize(); ++i)
	{
		auto _nowFace = m_CurrentNavMesh->GetNeviMeshFace(i);

		m_pIRenderer->RenderFace(
			WIRE,
			_nowFace->m_Vertex_Arr[0],
			_nowFace->m_Vertex_Arr[1],
			_nowFace->m_Vertex_Arr[2],
			_nowFace->m_FaceNormal,
			m_WorldTM, GREEN);
	}

	for (auto* _nowAgent : m_NavMeshAgent_V)
	{
		// Agent의 DebugMode가 false면 렌더하지 않는다.
		if (_nowAgent->GetDebugMode() == false)
		{
			continue;
		}

		_nowAgent->RenderFaceOnAgent(m_pIRenderer);
		_nowAgent->RenderPathAgentToDestination();
		
	}

}
