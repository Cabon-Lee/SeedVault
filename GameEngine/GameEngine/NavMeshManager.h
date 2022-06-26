#pragma once

/// <summary>
/// NavMeshManager Ŭ����
/// 
/// ���ӿ��� �̵��ϴµ� �ʿ��� NavMeshSet �� ����ؼ� ���� �ִ´�.
/// �ʿ��ϸ� ���� NavMeshSet �� ��ü�ϰų� �����Ѵ�.
/// 
/// �ۼ��� : �� �� ȯ
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

	// �׺�޽� ���Ϳ� �׺�޽� �߰�
	bool AddNavMesh(std::string name, std::string resourceName);
	bool AddNavMesh(std::string name, std::shared_ptr<NaviMeshSet> navMesh);	
	
	// string ���� NavMesh ����
	bool DeleteNavMesh(std::string name);

	// �׺� �޽� ��ü
	bool SetCurrentNavMesh(std::string name);	// string�� �ش��ϴ� �׺�޽��� ã�Ƽ� ��ü
	bool SetCurrentNavMesh(unsigned int index);	// uint �ε����� ��ü

	// ���� ������ NavMesh ����
	std::string& GetCurrentNavMeshName();
	std::shared_ptr<NaviMeshSet> GetCurrentNavMesh() const;

	/// NavMeshAgent ����
	bool RegisterNavMeshAgent(class NavMeshAgent* agent);
	bool DeleteNavMeshAgent(class NavMeshAgent* agent);

	/// Render 
	// ���� �׺�޽��� ����
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

	// �׺�޽����� ����Ʈ
	std::unordered_map<std::string, std::shared_ptr<NaviMeshSet>> m_NavMesh_UM;

	// ���� ���õ� �׺�޽�
	std::string m_CurrentNavMeshName;
	std::shared_ptr<NaviMeshSet> m_CurrentNavMesh;

	// ���� ���� �����ϴ� NavMeshAgent�� ���
	std::vector<class NavMeshAgent*> m_NavMeshAgent_V;


	/// ���� �ϱ� ���� �κ�
	bool m_bDebugMode;							// ����� ��� ����(���� on/off)
	std::shared_ptr<IRenderer> m_pIRenderer;	// I������
	
	DirectX::SimpleMath::Matrix m_WorldTM;

	DirectX::SimpleMath::Vector3 m_Scale;
	DirectX::SimpleMath::Quaternion m_Rotation;
	DirectX::SimpleMath::Vector3 m_Position;

};

