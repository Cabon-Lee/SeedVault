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

	// ������ �� �ʿ��� WorldTM ����
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
/// str���� ����� �̸��� ����� ���ҽ�(�޽�����) �̸��� �޾Ƽ� �׺�޽� ����Ʈ(UM)�� �߰��Ѵ�.
/// ���� �̸����� ��ϵ� �޽ð� �̹� �ִٸ� �ߺ��ؼ� �߰����� �ʴ´�.
/// </summary>
/// <param name="name"></param>
/// <param name="resourceName"></param>
/// <returns></returns>
bool NavMeshManager::AddNavMesh(std::string name, std::string resourceName)
{
	// �׺�޽��� �����.
	std::shared_ptr<IResourceManager> _resourceManager = DLLEngine::GetEngine()->GetResourceManager();
	std::shared_ptr<NaviMeshSet> _newNavMesh = _resourceManager->MakeNeviMesh(const_cast<std::string&>(resourceName));

	// ���������� �����������
	if (_newNavMesh != nullptr)
	{
		// ����Ʈ(UM)�� ����� �õ��Ѵ�.
		if (AddNavMesh(name, _newNavMesh))
		{
			CA_TRACE("[NavMeshManager] Succeeded AddNavMesh(%s, %s)", name.c_str(), resourceName.c_str());

			return true;
		}

		// �׺� �޽��� ��������� ����� �� ���� ���
		CA_TRACE("[NavMeshManager] Failed... AddNavMesh(%s, %s) -> %s �� �̹� ��ϵǾ� �ִ�..", name.c_str(), resourceName.c_str(), name.c_str());
		return false;
	}

	// �׺� �޽��� ����µ� ������ ���
	CA_TRACE("[NavMeshManager] Failed AddNavMesh(%s, %s) -> %s ��� resource(meshFilter)�� �������� �ʴ´�..", name.c_str(), resourceName.c_str(), resourceName.c_str());
	return false;
}

/// <summary>
/// �׺�޽��� �޾Ƽ� �׺�޽� ����Ʈ(UM)�� �߰��Ѵ�.
/// ���� �̸����� ��ϵ� �޽ð� �̹� �ִٸ� �ߺ��ؼ� �߰����� �ʴ´�.
/// </summary>
/// <param name="name">���(����)�� �̸�</param>
/// <param name="navMesh">�׺�޽� ������</param>
/// <returns>��� ���� ����</returns>
bool NavMeshManager::AddNavMesh(std::string name, std::shared_ptr<NaviMeshSet> navMesh)
{
	// ���� name �� �ش��ϴ� �׺�Ž��� �ִ��� ã�´�.
	std::shared_ptr<NaviMeshSet> _newNavMesh = FindNavMesh(name);

	// ������ �߰��Ѵ�!
	if (_newNavMesh == nullptr)
	{
		CA_TRACE("[NavMeshManager] Succeeded AddNavMesh( %s ) !!", name.c_str());
		m_NavMesh_UM.insert(make_pair(name, navMesh));

		// ���� ó�� �߰� �� �׺�޽��� �ڵ����� ���� �׺�޽��� �������ش�.
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
/// name ���� �׺�޽��� �׺�޽� ����Ʈ(UM)���� �����Ѵ�.
/// </summary>
/// <param name="name">������ �׺�Ž� �̸�</param>
bool NavMeshManager::DeleteNavMesh(std::string name)
{
	// ���� name �� �ش��ϴ� �׺�Ž��� �ִ��� ã�´�.
	std::shared_ptr<NaviMeshSet> _newNavMesh = FindNavMesh(name);

	// ����!
	if (_newNavMesh == nullptr)
	{
		CA_TRACE("[NavMeshManager] Failed DeleteNavMesh().. There is no such object....");

		return false;
	}

	// ����
	m_NavMesh_UM.erase(name);
	CA_TRACE("[NavMeshManager] Succeeded DeleteNavMesh( %s ) !!", name.c_str());

	return true;
}

/// <summary>
/// name�� �ش��ϴ� �׺�޽��� ã�� ���� �׺�Ž��� �����Ѵ�.
/// ������ ��ü���� �ʴ´�.
/// </summary>
/// <param name="name">������ �׺�Ž� �̸�</param>
/// <returns>��ü ���� ����</returns>
bool NavMeshManager::SetCurrentNavMesh(std::string name)
{
	// ���� name �� �ش��ϴ� �׺�Ž��� �ִ��� ã�´�.
	std::shared_ptr<NaviMeshSet> _newNavMesh = FindNavMesh(name);

	// ����!
	if (_newNavMesh == nullptr)
	{
		CA_TRACE("[NavMeshManager] Failed SetCurrentNavMesh() ...");

		return false;
	}

	// ã�� �׺�޽��� ����
	m_CurrentNavMeshName = name;
	m_CurrentNavMesh = _newNavMesh;

	// �׺�޽� ������Ʈ���� �׺�Ž��� �������ش�.
	NavMeshAgent::s_NavMesh = _newNavMesh;

	CA_TRACE("[NavMeshManager] Succeese SetCurrentNavMesh() ! -> now Mesh %s", name.c_str());

	return true;
}

/// <summary>
/// index ��ȣ�� �׺� �޽��� ��ü�Ѵ�.
/// </summary>
/// <param name="index">��ȣ</param>
/// <returns></returns>
bool NavMeshManager::SetCurrentNavMesh(unsigned int index)
{
	// �ݺ��� ���� i
	unsigned int i = 0;

	// �׺�޽�UM�� ��ȸ
	for (auto navMesh : m_NavMesh_UM)
	{
		// index��° �׺�޽��� ã����
		if (i == index)
		{
			// ���� �޽��� ����
			m_CurrentNavMeshName = navMesh.first;
			m_CurrentNavMesh = navMesh.second;

			CA_TRACE("[NavMeshManager] Succeese SetCurrentNavMesh() ! -> now Mesh %s", m_CurrentNavMeshName.c_str());

			// �׺�޽� ������Ʈ���� �׺�Ž��� �������ش�.
			NavMeshAgent::s_NavMesh = navMesh.second;

			// ������Ʈ���� LastOnFace �� �ʱ�ȭ�Ѵ�.
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
/// ����׸�� ���� ����
/// </summary>
/// <returns></returns>
bool NavMeshManager::GetDebugMode() const
{
	return m_bDebugMode;
}

/// <summary>
/// ����׸�� ���ϴ� ������ ����
/// </summary>
/// <param name="mode">������ ��</param>
void NavMeshManager::SetDebugMode(const bool mode)
{
	m_bDebugMode = mode;
}

/// <summary>
/// ����� ��� ����
/// </summary>
void NavMeshManager::ToggleDebugMode()
{
	m_bDebugMode = !m_bDebugMode;
}

/// <summary>
/// ���� �׺� �޽��� �̸��� ����
/// </summary>
/// <returns></returns>
std::string& NavMeshManager::GetCurrentNavMeshName()
{
	return m_CurrentNavMeshName;
}

/// <summary>
/// ���� �׺�޽��� ����
/// </summary>
/// <returns></returns>
std::shared_ptr<NaviMeshSet> NavMeshManager::GetCurrentNavMesh() const
{
	return m_CurrentNavMesh;
}

/// <summary>
/// NavMeshManager�� Agent ����Ʈ�� agent�� ����Ѵ�.
/// </summary>
/// <param name="agent">����� agent ������</param>
/// <returns>��������</returns>
bool NavMeshManager::RegisterNavMeshAgent(NavMeshAgent* agent)
{
	// nullptr üũ
	if (agent == nullptr)
	{
		CA_TRACE("[NavMeshManager] Failed RegisterNavMeshAgent().. agent was nullptr !");
		return false;
	}

	// agent�� ��ϵǾ� �ִ��� �˻�
	auto iter = std::find(m_NavMeshAgent_V.begin(), m_NavMeshAgent_V.end(), agent);

	// �̹� ������ ������� ����.
	if (iter != m_NavMeshAgent_V.end())
	{
		CA_TRACE("[NavMeshManager] Failed RegisterNavMeshAgent.. already registered agent %p...", agent);
		return false;
	}

	// ������ ����Ѵ�
	CA_TRACE("[NavMeshManager] Succeeded RegisterNavMeshAgent! agent = %p", agent);
	m_NavMeshAgent_V.push_back(agent);

	return true;
}


/// <summary>
/// NavMeshManager�� Agent ����Ʈ���� agent�� �����Ѵ�.
/// </summary>
/// <param name="agent">������ agent ������</param>
/// <returns>���� ����</returns>
bool NavMeshManager::DeleteNavMeshAgent(NavMeshAgent* agent)
{
	// nullptr üũ
	if (agent == nullptr)
	{
		CA_TRACE("[NavMeshManager] Failed RegisterNavMeshAgent().. agent was nullptr !");
		return false;
	}

	// agent�� ��ϵǾ� �ִ��� �˻�
	auto iter = std::find(m_NavMeshAgent_V.begin(), m_NavMeshAgent_V.end(), agent);

	// ������ �����Ѵ�.
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
/// ����޽� ����Ʈ(UM)���� name�� �ش��ϴ� �׺�Ž��� ã�� �����Ѵ�.
/// </summary>
/// <param name="name">ã�� ����޽��� �̸�</param>
/// <returns>Ž�� ���� ����</returns>
std::shared_ptr<NaviMeshSet> NavMeshManager::FindNavMesh(std::string name)
{
	// �׺�Ž��� ã�´�.
	auto _navMeshIter = m_NavMesh_UM.find(name);

	// �׺�޽��� �� ã������ nullptr ����
	if (_navMeshIter == m_NavMesh_UM.end())
	{
		CA_TRACE("[NavMeshManager] Failed FindNavMesh()... Can not find %s", name.c_str());
		return nullptr;
	}

	// ã�� �׺�޽� ����
	CA_TRACE("[NavMeshManager] Succeeded FindNavMesh( %s ) !", name.c_str());
	return (*_navMeshIter).second;
}

bool NavMeshManager::SetRenderer(std::shared_ptr<IRenderer> iRenderer)
{
	m_pIRenderer = iRenderer;

	return true;
}

/// <summary>
/// ����� ����� �� ���� ���õ� �׺�޽��� �����Ѵ�.
/// </summary>
void NavMeshManager::Render()
{
	// ������ ����� ���� ��尡 ���������� ���� ���� �ʴ´�.
	if (Managers::GetInstance()->GetComponentSystem()->GetDebugRenderMode() != true)
	{
		return;
	}

	// �׺�޽� ����� ����� ���� �׸���.
	if (m_bDebugMode != true)
	{
		return;
	}

	// ���� �޽��� ���ٸ� ����
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
		// Agent�� DebugMode�� false�� �������� �ʴ´�.
		if (_nowAgent->GetDebugMode() == false)
		{
			continue;
		}

		_nowAgent->RenderFaceOnAgent(m_pIRenderer);
		_nowAgent->RenderPathAgentToDestination();
		
	}

}
