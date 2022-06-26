#include "pch.h"
#include "IRenderer.h"
#include "EngineDLL.h"
#include "Transform.h"
#include "PhysicsActor.h"
#include "Managers.h"
#include "NaviMeshFace.h"
#include "NavMeshAgent.h"

// NavMeshManager�� �޾Ƽ� ����Ѵ�.
NavMeshManager* NavMeshAgent::s_NavMeshManager = nullptr;
std::shared_ptr<NaviMeshSet> NavMeshAgent::s_NavMesh = nullptr;

// Render �� identity matrix ����
DirectX::SimpleMath::Vector3 g_Scale = { 1.0f, 1.0f, 1.0f };
DirectX::SimpleMath::Quaternion g_Rotation = { 0.0f, 0.0f, 0.0f, 1.0f };
DirectX::SimpleMath::Vector3 g_Position = { 0.0f, 0.0f, 0.0f };

DirectX::SimpleMath::Matrix g_WorldTM = DirectX::SimpleMath::Matrix::CreateScale(g_Scale) *
DirectX::SimpleMath::Matrix::CreateFromQuaternion(g_Rotation) *
DirectX::SimpleMath::Matrix::CreateTranslation(g_Position);

AStarNode::AStarNode(std::shared_ptr<NaviMeshFace> face, std::shared_ptr<AStarNode> parent, std::shared_ptr<AStarNode> endNode)
	: m_Index(0)
	, m_Face(face)
	, m_Parent(parent)
	, m_G(0.0f)
	, m_H(0.0f)
	, m_F(0.0f)
{
	// �ε��� �Ҵ�
	m_Index = m_Face->m_Index;

	// G, H ��꿡 ����� ���� ����� ��ġ ����
	SimpleMath::Vector3 _myPosition = { this->m_Face->m_CenterPosition.x, 0.0f, this->m_Face->m_CenterPosition.z };

	/// G ���
	if (m_Parent == nullptr)
	{
		m_G = 0.0f;
	}
	else
	{
		SimpleMath::Vector3 _paretPosition = { m_Parent->m_Face->m_CenterPosition.x, 0.0f, m_Parent->m_Face->m_CenterPosition.z };
		float distance = SimpleMath::Vector3::Distance(_myPosition, _paretPosition);

		// ���� ������ �θ�(����)�������� �Ÿ� + �θ�(����)���� �ڽ�(����)��� ������ �Ÿ�
		m_G = parent->m_G + distance;
	}

	/// H ���
	// ���� ���(Face)�κ��� Dest������ �Ÿ��� �Ѵ�.(y�� �����ϰ� 2���� x,z ��� ���� ���̷� ���)
	if (endNode == nullptr)
	{
		m_H = 0.0f;
	}
	else
	{
		SimpleMath::Vector3 _destPosition = { endNode->m_Face->m_CenterPosition.x, 0.0f, endNode->m_Face->m_CenterPosition.z };

		// ���� ���(face)�κ��� Dest ������ �Ÿ�
		m_H = SimpleMath::Vector3::Distance(_myPosition, _destPosition);
	}

	/// F ���
	m_F = m_G + m_H;
}

AStarNode::~AStarNode()
{
}

NavMeshAgent::NavMeshAgent()
	: ComponentBase(ComponentType::Etc)
	, m_LastOnNavMeshFace(nullptr)
	, m_LastCycleEdgeVector(SimpleMath::Vector3::Zero)
	, m_StartPositionOfLastCycleEdge(SimpleMath::Vector3::Zero)
	, m_bOnNavMesh(false)

	, m_MoveSpeed(1.0f)

	, m_bAutoMoveMode(false)
	, m_DestinationObj(nullptr)
	, m_DestinationPos(SimpleMath::Vector3::Zero)
	, m_DestinationPosOnNavMesh(SimpleMath::Vector3::Zero)
	, m_DestinationFace(nullptr)
	, m_Path()

	, m_bDebugMode(false)
{
	if (s_NavMeshManager == nullptr)
	{
		s_NavMeshManager = Managers::GetInstance()->GetNavMeshManager();
	}

	//NavMeshManager�� �ڽ��� ���	
	s_NavMeshManager->RegisterNavMeshAgent(this);

}

NavMeshAgent::~NavMeshAgent()
{
	// NavMeshManager�� �ڽ��� ���� ��û
	s_NavMeshManager->DeleteNavMeshAgent(this);
}

void NavMeshAgent::Start()
{
	IsOnNavMesh(m_Transform->m_Position);
	m_MyPhysicsActor = m_pMyObject->GetComponent<PhysicsActor>();
}

void NavMeshAgent::Update(float dTime)
{
	// �׺� �Ž� �浹 ���� ����
	IsOnNavMesh(m_Transform->m_Position);

	// Agent�� �׺� �޽� ���� ���� ��ġ�� ���
	if (m_bOnNavMesh == true)
	{
		// ������Ʈ�� ������ ����
		AdjustAgentPositionY();

		// ���������� �־��� Face�� ����Face index ���
#ifdef _DEBUG
		//PrintLastOnFaceInfo();
#endif
	}

	// �׺�޽��� ��� ���
	else
	{
#ifdef _DEBUG
		//CA_TRACE("[NavMeshAgent] �׺�޽��� ���..");
#endif
		/// ��ȿ�� �׺�޽� ������ �̵��ϴٰ� �ٱ����� ������������ �� �� �浹ó��(����(Vector)�� ����)
		MoveToNearNavFaceLineVer3();
	}

	// ������ �������� �ִ� ���
	//if (m_DestinationObj != nullptr)
	// �ڵ� �̵� ��� �� ���
	if (m_bAutoMoveMode == true)
	{
		// ��� Ž���� ����� �׺� �޽� ���� ��ġ�� ���
		CalcDestinationOnNavMesh();
		//CA_TRACE("[NavMeshAgent] m_Destinatnion = %f / %f / %f", (*m_DestinationPos).x, (*m_DestinationPos).y, (*m_DestinationPos).z);
		//CA_TRACE("[NavMeshAgent] Mapping ������ = %f / %f / %f", m_DestinationPosOnNavMesh.x, m_DestinationPosOnNavMesh.y, m_DestinationPosOnNavMesh.z);

		/// �������� �ٸ� Face�� �̵��߰ų�, Agent�� ��θ� ��Ż�� ��쿡 ��� ��Ž��
		// �������� �ٸ� ������ �̵��� ���
		std::shared_ptr<NaviMeshFace> _nowDestFace = IsOnNavMesh(m_DestinationPosOnNavMesh);

		if (_nowDestFace != m_DestinationFace)
		{
			m_Path.clear();
			//m_Path = PathFindAStar(m_LastOnNavMeshFace, _nowDestFace);
			m_Path = PathFindAStar2(m_LastOnNavMeshFace, _nowDestFace);
		}
		// ������ Face ����
		m_DestinationFace = _nowDestFace;

		// Agent�� ��θ� ��Ż�� ���
		if (IsAgentOnPath() == false)
		{
			m_Path.clear();
			//m_Path = PathFindAStar(m_LastOnNavMeshFace, m_DestinationFace);
			m_Path = PathFindAStar2(m_LastOnNavMeshFace, m_DestinationFace);
		}

		// ��θ� ���� �̵�
		MoveToDestinationVer1();
	}


	if (DLLInput::InputKey(0x5A)) // Z �Է½� �׽�Ʈ
	{
		//MoveToNearNavFaceCenter();	// -> ���Ż�� ���� ����(Face�� �߽����� �̵�)
		//MoveToNearNavFaceLineVer3();

		if (m_DestinationObj != nullptr)
		{
			m_Path = PathFindAStar(m_LastOnNavMeshFace, m_DestinationFace);
		}
	}
}

void NavMeshAgent::OnRender()
{
}

/// <summary>
/// ���������� ��ġ���ִ� NavMeshFace�� �ʱ�ȭ �ϴ� �Լ�
/// NavMeshManager���� NavMesh�� ��ü�Ǿ��� �� ���� NavMesh�� ������ ������ �ִ� ����
/// �����ϱ� ���ؼ� �ʱ�ȭ ���ֱ� ���� ����.
/// </summary>
void NavMeshAgent::ResetLastOnNavMeshFace()
{
	m_LastOnNavMeshFace = nullptr;
}

/// <summary>
/// Agent�� �׺�Ž� ���� �x���� ��ȿ�� �Ǵ�
/// </summary>
/// /// <param name="position">�˻��� ���� ��ġ</param>
/// <returns>Agent�� �ִ� Face</returns>
std::shared_ptr<struct NaviMeshFace> NavMeshAgent::IsOnNavMesh(SimpleMath::Vector3 position)
{
	/// �˻��� �׺�޽��� ���� ��..
	if (s_NavMesh == nullptr)
	{
		return false;
	}

	/// ��� NavMeshFace ���� ��ȸ�ϸ鼭 �˻�
	for (size_t i = 0; i < s_NavMesh->GetIndexSize(); i++)
	{
		// �˻��� �κ� Face(Triangle) 
		std::shared_ptr<NaviMeshFace> _face = s_NavMesh->GetNeviMeshFace(i);

		// position �� _face ���� �����ϴ°�?
		if (IsOnNavMeshFace(_face, position))
		{
			// �׺�޽� ���� �����Ѵ�.
			return _face;
		}
	}

	// �׺�޽� ���� ���� ���
	return nullptr;
}

/// <summary>
/// Agent �� Ư�� Face(Triangle) ���� �����ϴ��� �˻�
/// 
/// 2D(2����)������ ����Ѵ�.. ���� xz ���󿡼� ���..
/// </summary>
/// <param name="face">�˻��� Face</param>
/// <param name="position">���� ��ġ</param>
/// <returns>���� ���</returns>
bool NavMeshAgent::IsOnNavMeshFace(std::shared_ptr<NaviMeshFace>& face, SimpleMath::Vector3 position)
{
	/* �ﰢ���� �� ������ A, B, C �� �˻��� ������ ������ T�� Ī�Ѵ�.*/
	SimpleMath::Vector3 _T = position;

	/// �ﰢ�� ����
	SimpleMath::Vector3 _A = face->m_Vertex_Arr[0];
	SimpleMath::Vector3 _B = face->m_Vertex_Arr[1];
	SimpleMath::Vector3 _C = face->m_Vertex_Arr[2];

	/// �ﰢ�� ����Ŭ ����(��, edge)
	SimpleMath::Vector3 _TriVecAB = face->m_CycleEdgeVector_Arr[0];	// AB
	SimpleMath::Vector3 _TriVecBC = face->m_CycleEdgeVector_Arr[1];	// BC
	SimpleMath::Vector3 _TriVecCA = face->m_CycleEdgeVector_Arr[2];	// CA

	/// �ﰢ�� �������� Agent�� ���� ����
	SimpleMath::Vector3 _HitVecAT = _T - _A;	// AT
	SimpleMath::Vector3 _HitVecBT = _T - _B;	// BT
	SimpleMath::Vector3 _HitVecCT = _T - _C;	// CT

	/// ������ ����(y ���и� ����)
	// �� ���� �����(���̽�)�� xz ������� �ִٴ� ���� �������Ѵ�.(��ġ 2Dó��..)
	// (������ ����ϴ� NavMesh�� 3D(x,y,z) ������ ��� ��������� ���Ʒ��� ��ġ�� �κ��� ��� �̷���(������) �õ��غ�..)
	// 
	// �� ������ y ���и� ����� ���̴�.
	// ������ y ���� ��ȣ�� ���� �� ������ �¿� ���踦 �� �� �ִ�.

	/* SeedVault �� �� ��(�׺�޽�)�� ������ ������ ���� �� ���� y�࿡ ������ ������ ����Ǵ� ���� ���ٴ� ���� ������ �Ѵ�.*/
	/* �׷��ϱ� ��,�Ʒ��� ��ġ�� �ﰢ��(���̽�)�� ���ٴ�(����� �Ѵٴ�) ��!! */

	// ������.y -> +,- ��ȣ�� Face�� ���⺤�͸� �������� ������ �ִ��� �������ִ��� �Ǻ� ����
	float _fCross0 = (_TriVecAB.z * _HitVecAT.x) - (_TriVecAB.x * _HitVecAT.z);
	float _fCross1 = (_TriVecBC.z * _HitVecBT.x) - (_TriVecBC.x * _HitVecBT.z);
	float _fCross2 = (_TriVecCA.z * _HitVecCT.x) - (_TriVecCA.x * _HitVecCT.z);

	/// �浹 ����
	bool _bHit = false;
	if (position == m_Transform->m_Position)
	{
		// �˻� ��� Pos�� Agent ������ ��� �浹 �Ǻ� ��Ȯ��
		// Dest �� ���ؼ� �˻��� ���� ���Ѵ�.
		m_bOnNavMesh = false;
	}

	/// ��ȣ�� ���� ������ �浹�� ���̴�.
	// ���� ����� ���
	if (_fCross0 >= 0.0f)
	{
		if ((_fCross1 >= 0.0f) && (_fCross2 >= 0.0f))
		{
			_bHit = true;

			// �˻��� position�� Agent �� ���� ���(�Ǵ� ���� ���)
			if (position == m_Transform->m_Position)
			{
				// Agent�� �׺�޽� ���� �ִ°ɷ� ����
				m_bOnNavMesh = true;
				m_LastOnNavMeshFace = face;	// Face ����
			}
		}
	}

	// ���� ������ ���
	else
	{
		if ((_fCross1 < 0.0f) && (_fCross2 < 0.0f))
		{
			_bHit = true;

			// �˻��� position�� Agent �� ���� ���(�Ǵ� ���� ���)
			if (position == m_Transform->m_Position)
			{
				// Agent�� �׺�޽� ���� �ִ°ɷ� ����
				m_bOnNavMesh = true;
				m_LastOnNavMeshFace = face;	// Face ����
			}
		}
	}

	/// �˻��� position�� Agent�� position�� ��(�� ���� ��)
	// �� �κ��� ��� �浹���� ��ü�� �����ϴ�.
	// ������ �浹������ �ʿ��� �����͸� �̸� �����α� ���ؼ� ����صд�.(�浹�� ���� ���꿡 �ʿ�!!)
	if (position == m_Transform->m_Position	// �����ϴ� ����� Agent�� ������ �浹 ������ �����Ѵ�.
		&& m_bOnNavMesh == true)
	{
		/// �浹 �˻� ���� ���꿡 ����ϱ� ���ؼ� ���� ������ ���� ���͸� �����Ѵ�.
		// ���밪 ��ȯ�ؼ� +�� ���̸� ����
		float _absfCross0 = std::fabsf(_fCross0);	// ��� ���� ��� ������ Length�� �״�� ����ϱ� ������
		float _absfCross1 = std::fabsf(_fCross1);	// �� ���ͷ� ��������� ����纯���� ���̶�� ������
		float _absfCross2 = std::fabsf(_fCross2);	// ������ ũ��ε� �� ����

		/// ���� ����� Edge(��)�� ã�´�.
		// Edge 0(AB)�� ���� ������.
		if ((_absfCross0 <= _absfCross1) &&
			(_absfCross0 <= _absfCross2))
		{
			/*
			���� ������� Edge(Vector)�� ��������

			������,
			����(������->����) Edge�� StartToEnd,
			����(������->������) HitVector,

			�� �����Ѵ�.
			*/
			m_LastCycleEdgeVector = _TriVecAB;
			m_StartPositionOfLastCycleEdge = _A;
		}

		else
		{
			// Edge 1(BC)�� ���� ������.
			if (_absfCross1 <= _absfCross2)
			{
				m_LastCycleEdgeVector = _TriVecBC;
				m_StartPositionOfLastCycleEdge = _B;
			}

			else
			{
				m_LastCycleEdgeVector = _TriVecCA;
				m_StartPositionOfLastCycleEdge = _C;
			}
		}
	}

	// ��� ����
	return _bHit;
}

/// <summary>
/// Agent�� �׺�޽� ������ ����� �� ��ġ�� �����ϱ� ���� �Լ�
/// (�ϴ� ���� ����� NavFace�� CenterPosition ���� �̵���Ų��.)
/// </summary>
/// <returns></returns> 
bool NavMeshAgent::MoveToNearNavFaceCenter()
{
	// ���� ���� ����� Face�� ã�´�.
	std::shared_ptr<NaviMeshFace> _nearFace = FindNearNavFace(m_Transform->m_Position);

	// Agent�� Position�� �̵���Ų��.
	m_Transform->SetPosition(_nearFace->m_CenterPosition);

	// y�� ��ġ ����
	AdjustAgentPositionY();

	// �浹 ������ ���� m_bOnNavMesh ������ ���� �̵� ���� ��ġ�� �˻�
	IsOnNavMeshFace(_nearFace, m_Transform->m_Position);

	// �̵��� Face�� LastOntNavMeshFace �� ����
	m_LastOnNavMeshFace = _nearFace;


	return true;
}

/// <summary>
/// [Ver.1 �Լ��� ȣ��� ������ �ǽð����� ���� ����� Line�� ����ؼ� �̵�] ���� �⺻��..
/// Agent�� position(������)���κ��� ���� ����� Face�� Edge ���� ������ �̵��Ѵ�.
/// 
/// Agent�� �׺�޽� ������ �̵��ϴٰ� ������������ �Ѿ���� �� �� ������ �����ϱ� ���� ���

// ver.1 �Լ��� �Ѱ����� �ִ�.
// Face �ۿ� ���� �� �ǽð����� ����� Vector�� ����ؼ� Agent�� �̵��ϴ� ������ �������� ���ϰ�
// �ٸ� Edge(vector)�� ������ �Ǹ鼭 ������ �ٲ�� ������ �ִ�.
// �Դٰ� Edge�� �糡 (Start, End) �������� �Ѿ� ���� �κп� ���� ó���� ���� ���Ѵ�.
//
/// </summary>
/// <returns></returns>
bool NavMeshAgent::MoveToNearNavFaceLineVer1()
{
	// ���ʿ� ���� ��ġ�� �׺�Ž� ������ ����� m_LastOntNavMeshFace �� nullptr �� �� �ִ�.
	// �̶��� ���� ����� ���̽��� �켱 �Ű��ش�.
	if (!m_LastOnNavMeshFace)
	{
		MoveToNearNavFaceCenter();
	}

	/* �ﰢ���� �� ������ A, B, C �� Agent�� ������ ������ T�� Ī�Ѵ�.*/
	SimpleMath::Vector3 _T = m_Transform->m_Position;

	/// �ﰢ�� ����
	SimpleMath::Vector3 _A = m_LastOnNavMeshFace->m_Vertex_Arr[0];
	SimpleMath::Vector3 _B = m_LastOnNavMeshFace->m_Vertex_Arr[1];
	SimpleMath::Vector3 _C = m_LastOnNavMeshFace->m_Vertex_Arr[2];

	/// �ﰢ�� ����Ŭ ����(��, edge)
	SimpleMath::Vector3 _TriVecAB = m_LastOnNavMeshFace->m_CycleEdgeVector_Arr[0];	// AB
	SimpleMath::Vector3 _TriVecBC = m_LastOnNavMeshFace->m_CycleEdgeVector_Arr[1];	// BC
	SimpleMath::Vector3 _TriVecCA = m_LastOnNavMeshFace->m_CycleEdgeVector_Arr[2];	// CA

	/// �ﰢ�� �������� Agent�� ���� ����
	SimpleMath::Vector3 _HitVecAT = _T - _A;	// AT
	SimpleMath::Vector3 _HitVecBT = _T - _B;	// BT
	SimpleMath::Vector3 _HitVecCT = _T - _C;	// CT

	// ������.y -> +,- ��ȣ�� Face�� ���⺤�͸� �������� ������ �ִ��� �������ִ��� �Ǻ� ����
	float _fCross0 = (_TriVecAB.z * _HitVecAT.x) - (_TriVecAB.x * _HitVecAT.z);
	float _fCross1 = (_TriVecBC.z * _HitVecBT.x) - (_TriVecBC.x * _HitVecBT.z);
	float _fCross2 = (_TriVecCA.z * _HitVecCT.x) - (_TriVecCA.x * _HitVecCT.z);
	/////////////////////////////////////////////////////////////////////////////////////////////////

	/// �̵��� position
	SimpleMath::Vector3 _newPosition = { 0.0f, _T.y, 0.0f };

	/// Face�� ����Ŭ ����(Edge��) ����ȭ(�߿�)
	// ���⼺�и� �����.
	SimpleMath::Vector3 _norVecAB = { _TriVecAB.x / _TriVecAB.Length(), _TriVecAB.y / _TriVecAB.Length(), _TriVecAB.z / _TriVecAB.Length() };
	SimpleMath::Vector3 _norVecBC = { _TriVecBC.x / _TriVecBC.Length(), _TriVecBC.y / _TriVecBC.Length(), _TriVecBC.z / _TriVecBC.Length() };
	SimpleMath::Vector3 _norVecCA = { _TriVecCA.x / _TriVecCA.Length(), _TriVecCA.y / _TriVecCA.Length(), _TriVecCA.z / _TriVecCA.Length() };

	/// ���밪 ��ȯ�ؼ� +�� ���̸� ����
	_fCross0 = std::fabsf(_fCross0);		// ��� ���� ��� ������ Length�� �״�� ����ϱ� ������
	_fCross1 = std::fabsf(_fCross1);		// �� ���ͷ� ��������� ����纯���� ���̶�� ������
	_fCross2 = std::fabsf(_fCross2);		// ������ ũ��ε� �� ����

	/// ���� ����� Edge(��)�� ã�´�.
	// Edge 0(AB)�� ���� ������.
	if ((_fCross0 <= _fCross1) &&
		(_fCross0 <= _fCross2))
	{
		float _fDot = _norVecAB.Dot(_HitVecAT);

		// ����(normalize) * ����(dot) + ����(���� ����� Edge�� ���� ��)
		_newPosition.x = _norVecAB.x * _fDot + m_LastOnNavMeshFace->m_Vertex_Arr[0].x;
		_newPosition.z = _norVecAB.z * _fDot + m_LastOnNavMeshFace->m_Vertex_Arr[0].z;
	}

	else
	{
		// Edge 1(BC)�� ���� ������.
		if (_fCross1 <= _fCross2)
		{
			float _fDot = _norVecBC.Dot(_HitVecBT);

			if (_fDot < 0.0f)
			{
				_fDot = 0.0f;
			}

			if (_fDot > _TriVecBC.Length())
			{
				_fDot = _TriVecBC.Length();
			}

			_newPosition.x = _norVecBC.x * _fDot + m_LastOnNavMeshFace->m_Vertex_Arr[1].x;
			_newPosition.z = _norVecBC.z * _fDot + m_LastOnNavMeshFace->m_Vertex_Arr[1].z;
		}

		else
		{
			// Edge 2(CA)�� ���� ������.
			float _fDot = _norVecCA.Dot(_HitVecCT);

			if (_fDot < 0.0f)
			{
				_fDot = 0.0f;
			}

			if (_fDot > _TriVecCA.Length())
			{
				_fDot = _TriVecCA.Length();
			}

			_newPosition.x = _norVecCA.x * _fDot + m_LastOnNavMeshFace->m_Vertex_Arr[2].x;
			_newPosition.z = _norVecCA.z * _fDot + m_LastOnNavMeshFace->m_Vertex_Arr[2].z;
		}
	}

	/// ���� ����� Edge(����)�� ���� ����� �������� �ű��.
	m_Transform->SetPosition(_newPosition);
	AdjustAgentPositionY();

	return true;
}

/// <summary>
/// [Ver.2 �Լ��� ȣ��� ������ �ǽð����� ���� ����� Line�� ����ؼ� �̵�]
/// Agent�� position(������)���κ��� ���� ����� Face�� Edge ���� ������ �̵��Ѵ�.
/// 
/// Agent�� �׺�޽� ������ �̵��ϴٰ� ������������ �Ѿ���� �� �� ������ �����ϱ� ���� ���

// ver.2 �Լ��� �Ѱ����� �ִ�.
//
// ��ȿ�� NavMesh ���� �ȿ��� Face�� ������ ��ġ�� �κ��� �ִ� ��쿡 
// ���� Face�� �Ų����� �Ѿ�� ���ϰ� �����̴� ������ �ִ�..
//
// Ver1 ���� ��������
// ���� �ڳʺκп��� �׺�Ž� ������ ������ ���ϰ� Ŀ���ϴ� �κ��� �������� �����Ѵ�!! 
// Edge(Vector)�� �糡(Start, End) �������� �Ѿ�� ��� ���� ����.
/// </summary>
/// <returns></returns>
bool NavMeshAgent::MoveToNearNavFaceLineVer2()
{
	// ���ʿ� ���� ��ġ�� �׺�Ž� ������ ����� m_LastOntNavMeshFace �� nullptr �� �� �ִ�.
	// �̶��� ���� ����� ���̽��� �켱 �Ű��ش�.
	if (!m_LastOnNavMeshFace)
	{
		MoveToNearNavFaceCenter();
	}

	/* �ﰢ���� �� ������ A, B, C �� Agent�� ������ ������ T�� Ī�Ѵ�.*/
	SimpleMath::Vector3 _T = m_Transform->m_Position;

	/// �ﰢ�� ����
	SimpleMath::Vector3 _A = m_LastOnNavMeshFace->m_Vertex_Arr[0];
	SimpleMath::Vector3 _B = m_LastOnNavMeshFace->m_Vertex_Arr[1];
	SimpleMath::Vector3 _C = m_LastOnNavMeshFace->m_Vertex_Arr[2];

	/// �ﰢ�� ����Ŭ ����(��, edge)
	SimpleMath::Vector3 _TriVecAB = m_LastOnNavMeshFace->m_CycleEdgeVector_Arr[0];	// AB
	SimpleMath::Vector3 _TriVecBC = m_LastOnNavMeshFace->m_CycleEdgeVector_Arr[1];	// BC
	SimpleMath::Vector3 _TriVecCA = m_LastOnNavMeshFace->m_CycleEdgeVector_Arr[2];	// CA

	/// �ﰢ�� �������� Agent�� ���� ����
	SimpleMath::Vector3 _HitVecAT = _T - _A;	// AT
	SimpleMath::Vector3 _HitVecBT = _T - _B;	// BT
	SimpleMath::Vector3 _HitVecCT = _T - _C;	// CT

	// ������.y -> +,- ��ȣ�� Face�� ���⺤�͸� �������� ������ �ִ��� �������ִ��� �Ǻ� ����
	float _fCross0 = (_TriVecAB.z * _HitVecAT.x) - (_TriVecAB.x * _HitVecAT.z);
	float _fCross1 = (_TriVecBC.z * _HitVecBT.x) - (_TriVecBC.x * _HitVecBT.z);
	float _fCross2 = (_TriVecCA.z * _HitVecCT.x) - (_TriVecCA.x * _HitVecCT.z);
	/////////////////////////////////////////////////////////////////////////////////////////////////

	/// �̵��� position
	SimpleMath::Vector3 _newPosition = { 0.0f, _T.y, 0.0f };

	/// Face�� ����Ŭ ����(Edge��) ����ȭ(�߿�)
	// ���⼺�и� �����.
	SimpleMath::Vector3 _norVecAB = { _TriVecAB.x / _TriVecAB.Length(), _TriVecAB.y / _TriVecAB.Length(), _TriVecAB.z / _TriVecAB.Length() };
	SimpleMath::Vector3 _norVecBC = { _TriVecBC.x / _TriVecBC.Length(), _TriVecBC.y / _TriVecBC.Length(), _TriVecBC.z / _TriVecBC.Length() };
	SimpleMath::Vector3 _norVecCA = { _TriVecCA.x / _TriVecCA.Length(), _TriVecCA.y / _TriVecCA.Length(), _TriVecCA.z / _TriVecCA.Length() };

	/// ���밪 ��ȯ�ؼ� +�� ���̸� ����
	_fCross0 = std::fabsf(_fCross0);		// ��� ���� ��� ������ Length�� �״�� ����ϱ� ������
	_fCross1 = std::fabsf(_fCross1);		// �� ���ͷ� ��������� ����纯���� ���̶�� ������
	_fCross2 = std::fabsf(_fCross2);		// ������ ũ��ε� �� ����

	/// ���� ����� Edge(��)�� ã�´�.
	// Edge 0(AB)�� ���� ������.
	if ((_fCross0 <= _fCross1) &&
		(_fCross0 <= _fCross2))
	{
		float _fDot = _norVecAB.Dot(_HitVecAT);

		/// Ver2 �߰� ����
		/// Agent�� Edge(��) ������ �Ѿ �ִ°�� Edge ������ �����Բ� �����Ѵ�.
		// ����Ǿ� �ִ� ��ȿ�� Face�� �̵��� ���� �� �ɸ��� ������ ����������...
		// ���� �������� �׺�Ž� ������ ������ ���� �ɾ� �����ϴ�!!
		if (_fDot < 0.0f)
		{
			_fDot = 0.0f;
		}

		if (_fDot > _TriVecAB.Length())
		{
			_fDot = _TriVecAB.Length();
		}

		// ����(normalize) * ����(dot) + ����(���� ����� Edge�� ���� ��)
		_newPosition.x = _norVecAB.x * _fDot + m_LastOnNavMeshFace->m_Vertex_Arr[0].x;
		_newPosition.z = _norVecAB.z * _fDot + m_LastOnNavMeshFace->m_Vertex_Arr[0].z;
	}

	else
	{
		// Edge 1(BC)�� ���� ������.
		if (_fCross1 <= _fCross2)
		{
			float _fDot = _norVecBC.Dot(_HitVecBT);

			/// Ver2 �߰� ����
			/// Agent�� Edge(��) ������ �Ѿ �ִ°�� Edge ������ �����Բ� �����Ѵ�.
			// ����Ǿ� �ִ� ��ȿ�� Face�� �̵��� ���� �� �ɸ��� ������ ����������...
			// ���� �������� �׺�Ž� ������ ������ ���� �ɾ� �����ϴ�!!
			if (_fDot < 0.0f)
			{
				_fDot = 0.0f;
			}

			if (_fDot > _TriVecBC.Length())
			{
				_fDot = _TriVecBC.Length();
			}

			_newPosition.x = _norVecBC.x * _fDot + m_LastOnNavMeshFace->m_Vertex_Arr[1].x;
			_newPosition.z = _norVecBC.z * _fDot + m_LastOnNavMeshFace->m_Vertex_Arr[1].z;
		}

		else
		{
			// Edge 2(CA)�� ���� ������.
			float _fDot = _norVecCA.Dot(_HitVecCT);

			/// Ver2 �߰� ����
			/// Agent�� Edge(��) ������ �Ѿ �ִ°�� Edge ������ �����Բ� �����Ѵ�.
			// ����Ǿ� �ִ� ��ȿ�� Face�� �̵��� ���� �� �ɸ��� ������ ����������...
			// ���� �������� �׺�Ž� ������ ������ ���� �ɾ� �����ϴ�!!
			if (_fDot < 0.0f)
			{
				_fDot = 0.0f;
			}

			if (_fDot > _TriVecCA.Length())
			{
				_fDot = _TriVecCA.Length();
			}

			_newPosition.x = _norVecCA.x * _fDot + m_LastOnNavMeshFace->m_Vertex_Arr[2].x;
			_newPosition.z = _norVecCA.z * _fDot + m_LastOnNavMeshFace->m_Vertex_Arr[2].z;
		}
	}

	/// ���� ����� Edge(����)�� ���� ����� �������� �ű��.
	m_Transform->SetPosition(_newPosition);
	AdjustAgentPositionY();

	return true;
}

/// <summary>
/// [Ver.3 �̸� �����߾���, ���������� ��ġ���ִ� Face���� ���� ����� Line ���� ����ؼ� �̵� ]
/// [�浹 ����, �����굵 2������ �Ѵ�. ver.2 ���� �� �ܰ� �����Ǿ���!!]
/// 
/// Agent�� position(������)���κ��� ���� ����� Face�� Edge ���� ������ �̵��Ѵ�.
/// 
/// Agent�� �׺�޽� ������ �̵��ϴٰ� ������������ �Ѿ���� �� �� ������ �����ϱ� ���� ���
/// </summary>
/// <returns></returns>
bool NavMeshAgent::MoveToNearNavFaceLineVer3()
{
	// ���ʿ� ���� ��ġ�� �׺�Ž� ������ ����� m_LastOntNavMeshFace �� nullptr �� �� �ִ�.
	// �̶��� ���� ����� ���̽��� �켱 �Ű��ش�.
	if (m_LastOnNavMeshFace == nullptr)
	{
		MoveToNearNavFaceCenter();
	}

	/* Agent�� ������ ������ T�� Ī�Ѵ�.*/
	SimpleMath::Vector3 _T = m_Transform->m_Position;

	/// �̵��� position
	SimpleMath::Vector3 _newPosition = { 0.0f, _T.y, 0.0f };

	/// Ver3 �߰����� m_LastCycleEdgeVector;
	// �ʱⰪ�� Vector3::zero ���¶�� �̵��� �� ����.
	if (m_LastCycleEdgeVector.Length() != 0.0f)
	{
		/// 1�� ���� ���(1�� ���� ������ �̾��� Face���� �̵��� �������� �ϱ� ����)
		// ���⼺�и� �����.
		SimpleMath::Vector3 _norVec = m_LastCycleEdgeVector;
		_norVec.y = 0.0f;	// x, z ����鿡 ���� ����ϱ� ���� y���� ������.(�밢�� ����(���)���� ������ ���ϱ� ����)
		_norVec.Normalize();
		/*
			SimpleMath::Vector3 _norVec = {
			m_LastCycleEdgeVector.x / m_LastCycleEdgeVector.Length(),
			m_LastCycleEdgeVector.y / m_LastCycleEdgeVector.Length(),
			m_LastCycleEdgeVector.z / m_LastCycleEdgeVector.Length()
			}
		*/

		// �Ÿ� ���� ���Ѵ�.
		SimpleMath::Vector3 _HitVec = _T - m_StartPositionOfLastCycleEdge;	// ��Ʈ���ʹ� ���� Agent�� ��ġ�� �������� �ؾ���!
		_HitVec.y = 0.0f;	// x, z ����鿡 ���� ����ϱ� ���� y���� ������.(�밢�� ����(���)���� ������ ���ϱ� ����)

		float _fDot = _norVec.Dot(_HitVec);

		_newPosition.x = _norVec.x * _fDot + m_StartPositionOfLastCycleEdge.x;
		_newPosition.z = _norVec.z * _fDot + m_StartPositionOfLastCycleEdge.z;

		/// ����� ���ͷ� �̵��Ѵ�.
		m_Transform->SetPosition(_newPosition);
		AdjustAgentPositionY();

		/// Ver3 �߰�����. �浹 �˻� 2�� ó��
		/// 1�� �̵� �Ŀ� ��ġ ����
		IsOnNavMesh(m_Transform->m_Position);
		if (m_bOnNavMesh == true)
		{
			return true;
		}

		/// 1�� �̵� �Ŀ� ������ NavMesh �ۿ� ���� ��

		/// Agent�� Edge(��) ������ �Ѿ �ִ°�� Edge ������ �����Բ� �����Ѵ�.
		// Ver2 �߰� ����
		if (_fDot < 0.0f)
		{
			_fDot = 0.0f;
		}

		if (_fDot > m_LastCycleEdgeVector.Length())
		{
			_fDot = m_LastCycleEdgeVector.Length();
		}

		// ����(normalize) * ����(dot) + ����(���� ����� Edge�� ���� ��)
		_newPosition.x = _norVec.x * _fDot + m_StartPositionOfLastCycleEdge.x;
		_newPosition.z = _norVec.z * _fDot + m_StartPositionOfLastCycleEdge.z;

		/// ����� ���ͷ� �̵��Ѵ�.
		m_Transform->SetPosition(_newPosition);
		AdjustAgentPositionY();

		/// 2�� �̵� �Ŀ� ��ġ ����
		IsOnNavMesh(m_Transform->m_Position);
		if (m_bOnNavMesh == true)
		{
			return true;
		}
	}

	// ����!
	return false;
}

/// <summary>
/// Agent�� position(������)���κ��� ���� ����� ����(Vertex)�� �̵��Ѵ�.
/// 
/// (���� ������ �浹���������� ������� �ʴ���..)
/// </summary>
/// <returns></returns>
bool NavMeshAgent::MoveToNearVertex()
{
	/// �˻��� �׺�޽��� ���� ��..
	if (s_NavMesh == nullptr)
	{
		return false;
	}

	// Agent�� ������
	SimpleMath::Vector3 _agentPosition = m_Transform->m_Position;

	// �̵��� Vertex
	SimpleMath::Vector3 _nearVertex = s_NavMesh->GetNeviMeshFace(0)->m_Vertex_Arr[0];
	float _minDistance = SimpleMath::Vector3::Distance(_nearVertex, _agentPosition);

	/// NavMeshFace ���� ��ȸ�ϸ鼭 �˻�
	for (size_t i = 0; i < s_NavMesh->GetIndexSize(); i++)
	{
		// �˻��� �κ� Face(Triangle) 
		std::shared_ptr<NaviMeshFace> _face = s_NavMesh->GetNeviMeshFace(i);

		// agent�� face[i]�� �� vertex�� �Ÿ��� ���Ѵ�.
		float _distance0 = SimpleMath::Vector3::Distance(_face->m_Vertex_Arr[0], _agentPosition);
		float _distance1 = SimpleMath::Vector3::Distance(_face->m_Vertex_Arr[1], _agentPosition);
		float _distance2 = SimpleMath::Vector3::Distance(_face->m_Vertex_Arr[2], _agentPosition);

		/// ���� �ּҰ��� ��
		// 0 �� ���ؽ����� �Ÿ� ��
		if (_distance0 < _minDistance)
		{
			// ���� face���� �� �����ٸ� min ����
			_minDistance = _distance0;
			_nearVertex = _face->m_Vertex_Arr[0];
		}

		// 1 �� ���ؽ����� �Ÿ� ��
		if (_distance1 < _minDistance)
		{
			// ���� face���� �� �����ٸ� min ����
			_minDistance = _distance1;
			_nearVertex = _face->m_Vertex_Arr[1];
		}

		// 2 �� ���ؽ����� �Ÿ� ��
		if (_distance2 < _minDistance)
		{
			// ���� face���� �� �����ٸ� min ����
			_minDistance = _distance2;
			_nearVertex = _face->m_Vertex_Arr[2];
		}
	}

	// ���� ����� Vertex�� ��ġ�� Agent ������ �̵�
	m_Transform->SetPosition(_nearVertex);
	AdjustAgentPositionY();	// y�� ����

	return true;
}

/// <summary>
/// Agent�� �浹 ó���� 1�������� �� �Ŀ� ������ NavMesh ���� �ٱ��� �ִ� ��쿡
/// (�浹ó���� ���) LastOnNavFace�� ���� ������ �����ϴ� ���� Face�� �����Ű�� �Լ�
/// 
/// (MoveToNearNavFaceLineVer3() �� �������� ���� ������ �浹���������� ����ϰ� ���� �ʴ�..)
/// </summary>
/// <param name="vertex">������ ���� ���� ����� Face�� ����</param>
/// <returns></returns>
bool NavMeshAgent::ChangeLastOnNavMeshFaceToAdjacentNavFace()
{
	/// ������ ���� ���� ����� ������ ã�´�.
	SimpleMath::Vector3 _nearVertex = SimpleMath::Vector3::Zero;

	SimpleMath::Vector3 _vertexA = m_LastOnNavMeshFace->m_Vertex_Arr[0];
	SimpleMath::Vector3 _vertexB = m_LastOnNavMeshFace->m_Vertex_Arr[1];
	SimpleMath::Vector3 _vertexC = m_LastOnNavMeshFace->m_Vertex_Arr[2];

	// �� �������� �Ÿ� ���
	unsigned int _DistanceA = SimpleMath::Vector3::Distance(m_Transform->m_Position, _vertexA);
	unsigned int _DistanceB = SimpleMath::Vector3::Distance(m_Transform->m_Position, _vertexB);
	unsigned int _DistanceC = SimpleMath::Vector3::Distance(m_Transform->m_Position, _vertexC);

	// ���� A�� ���� ������
	if (_DistanceA < _DistanceB &&
		_DistanceA < _DistanceC)
	{
		_nearVertex = _vertexA;
	}

	else
	{
		// ���� B�� ���� ������
		if (_DistanceB < _DistanceC)
		{
			_nearVertex = _vertexB;
		}

		// ���� C�� ���� ������
		else
		{
			_nearVertex = _vertexC;
		}
	}

	// Face���� ��ȸ�ϸ鼭 ã�´�.
	for (size_t i = 0; i < s_NavMesh->GetIndexSize(); i++)
	{
		std::shared_ptr<NaviMeshFace> _face = s_NavMesh->GetNeviMeshFace(i);

		// ���� ������ LastNavFace(���� NavFace)�� �ǳ� ��.
		if (_face == m_LastOnNavMeshFace)
		{
			continue;
		}

		// Face�� ���� 3���� �˻�
		for (size_t j = 0; j < 3; j++)
		{
			// ���� ������ �����ϴ� Face�� ã��!
			if (_face->m_Vertex_Arr[j] == _nearVertex)
			{
				// �浹ó���� ����� LastOnNavFace�� ������ Face�� ����
				m_LastOnNavMeshFace = _face;

				return true;
			}
		}
	}

	return false;
}

/// <summary>
/// ������Ʈ �����͸� �޾Ƽ� ������ ����
/// </summary>
/// <param name="object">������ ������Ʈ</param>
/// <returns>��������</returns>
bool NavMeshAgent::SetDestinationObj(GameObject* object)
{
	// ������ �����Ϸ��� nullptr �Է��� ���
	if (object == nullptr)
	{
		m_Path.clear();

		m_DestinationObj = nullptr;
		//m_DestinationPos = nullptr;

		m_bAutoMoveMode = false;
	}

	// ������ ��� ������Ʈ�� ���޵� ���
	else
	{
		/// ���� Dest�� object�� �ٸ� ��쿡 Path ����
		if (m_DestinationObj != object)
		{
			// ���� ��� Ŭ����
			m_Path.clear();

			// ��ǥ ����
			m_DestinationObj = object;
			m_DestinationPos = object->m_Transform->m_Position;

			m_bAutoMoveMode = true;
		}

	}

	return true;
}

/// <summary>
/// Vector3 pos�� ��ǥ���� ����
/// </summary>
/// <param name="position">��ǥ ��ġ</param>
/// <returns></returns>
_DLL bool NavMeshAgent::SetDestinationPos(Vector3 position)
{
	// Agent�� ���� ��ġ�� �����ϸ� �ڵ� �̵� ����
	if (position == m_Transform->m_Position)
	{
		m_bAutoMoveMode = false;

		return true;
	}

	/// ���� DestPos�� psition�� �ٸ� ��쿡 Path ����
	if (m_DestinationPos != position)
	{
		// ���� ��� Ŭ����
		m_Path.clear();

		// ��ǥ ����
		m_DestinationObj = nullptr;
		m_DestinationPos = position;

		m_bAutoMoveMode = true;
	}

	return true;
}

/// <summary>
/// Agent�� ���������� ��ġ�� �ִ� Face ����
/// (NavMeshManager ���� �ϰ������� Agent���� ��ġ�� Face�� ������ �� ���)
/// </summary>
/// <returns>���������� ��ġ�� �ִ� Face</returns>
std::shared_ptr<struct NaviMeshFace> NavMeshAgent::GetLastOnNavMeshFace() const
{
	return m_LastOnNavMeshFace;
}

/// <summary>
/// ����� ��� ����
/// </summary>
/// <returns>���� ����� ��� ��</returns>
_DLL bool NavMeshAgent::GetDebugMode() const
{
	return m_bDebugMode;
}

/// <summary>
/// ����׸�� �� ����
/// </summary>
/// <param name="val">������ ��</param>
/// <returns></returns>
_DLL void NavMeshAgent::SetDebugMode(const bool val)
{
	m_bDebugMode = val;
}

_DLL void NavMeshAgent::RenderFaceOnAgent(std::shared_ptr<IRenderer> iRenderer) const
{
	iRenderer->RenderFace(
		SOLID,
		m_LastOnNavMeshFace->m_Vertex_Arr[0],
		m_LastOnNavMeshFace->m_Vertex_Arr[1],
		m_LastOnNavMeshFace->m_Vertex_Arr[2],
		m_LastOnNavMeshFace->m_FaceNormal,
		g_WorldTM, GREEN);
}

_DLL void NavMeshAgent::RenderPathAgentToDestination() const
{
	if (m_Path.size() == 0)
	{
		return;
	}

	std::shared_ptr<IRenderer> iRenderer = DLLEngine::GetEngine()->GetIRenderer();
	auto iter = m_Path.begin();

	// ���� ����� ù Node�� ������(end) Node�� ���ٸ�
	// ������(end) Node�� �����ϰ� ����
	if ((*iter)->m_Face == m_DestinationFace)
	{
		// end Node(Face) Render
		iRenderer->RenderFace(
			SOLID,
			m_DestinationFace->m_Vertex_Arr[0],
			m_DestinationFace->m_Vertex_Arr[1],
			m_DestinationFace->m_Vertex_Arr[2],
			m_DestinationFace->m_FaceNormal,
			g_WorldTM, BLUE
		);

		return;
	}

	// start Node(Face) Render
	iRenderer->RenderFace(
		SOLID,
		(*iter)->m_Face->m_Vertex_Arr[0],
		(*iter)->m_Face->m_Vertex_Arr[1],
		(*iter)->m_Face->m_Vertex_Arr[2],
		(*iter)->m_Face->m_FaceNormal,
		g_WorldTM, RED
	);
	++iter;

	// mid Node(Face) Render
	for (size_t i = 1; i < m_Path.size() - 1; ++i)
	{
		if (iter == m_Path.end())
		{
			break;
		}

		iRenderer->RenderFace(
			SOLID,
			(*iter)->m_Face->m_Vertex_Arr[0],
			(*iter)->m_Face->m_Vertex_Arr[1],
			(*iter)->m_Face->m_Vertex_Arr[2],
			(*iter)->m_Face->m_FaceNormal,
			g_WorldTM, WHITE
		);

		++iter;
	}

	// end Node(Face) Render
	iRenderer->RenderFace(
		SOLID,
		m_DestinationFace->m_Vertex_Arr[0],
		m_DestinationFace->m_Vertex_Arr[1],
		m_DestinationFace->m_Vertex_Arr[2],
		m_DestinationFace->m_FaceNormal,
		g_WorldTM, BLUE
	);

	// �ܰ��� �׸���
	for (const auto& node : m_Path)
	{
		iRenderer->RenderFace(
			WIRE,
			(*iter)->m_Face->m_Vertex_Arr[0],
			(*iter)->m_Face->m_Vertex_Arr[1],
			(*iter)->m_Face->m_Vertex_Arr[2],
			(*iter)->m_Face->m_FaceNormal,
			g_WorldTM, GREEN
		);
	}
}

_DLL float NavMeshAgent::GetMoveSpeed() const
{
	return m_MoveSpeed;
}

_DLL void NavMeshAgent::SetMoveSpeed(const float moveSpeed)
{
	m_MoveSpeed = moveSpeed;
}

/// <summary>
/// Agent �� Face ���� �ȿ� ���� �� pos.y ��ġ�� 3���� �������� ����(y)�� ���ġ�� �����Ѵ�.
/// </summary>
bool NavMeshAgent::AdjustAgentPositionY()
{
	// ���� ��ġ�� Face�� ���� ��� -> (NavMesh ������ ��� ���)
	if (m_LastOnNavMeshFace == nullptr)
	{
		return false;
	}

	// ���� Face(�ﰢ��)�� �߽� y pos �� ���Ѵ�.
	float _CenterY = m_LastOnNavMeshFace->m_CenterPosition.y;

	// �ݶ��̴� ������ŭ ������ ������ ��
	// 0���� �ϴϱ� ���� ������ �������� ��������..
	float _offset = 0.23f;

	// ������ ���Ͱ� ������ �ݶ��̴��� ���� ��ŭ �����ؼ� posY�� ��ġ��Ų��.
	if (m_MyPhysicsActor != nullptr)
	{
		_offset += m_MyPhysicsActor->GetBoundingOffset().Bottom;
	}

	/// �߽� y ��ǥ�� �ݶ��̴� ������ŭ ������ �������� ���ؼ� y�� �̵�
	m_Transform->SetPosition({ m_Transform->m_Position.x, _CenterY + _offset, m_Transform->m_Position.z });

	return true;
}

/// <summary>
/// Pos���� ���� ����� NavMeshFace�� ã�´�.
/// Face�� [������ ��ǥ]�� �������� ã�´�.
/// </summary>
/// <param name="position">���� ��ġ</param>
/// <returns>���� ����� NavFace</returns>
std::shared_ptr<struct NaviMeshFace> NavMeshAgent::FindNearNavFace(SimpleMath::Vector3 position)
{
	/// �˻��� �׺�޽��� ���� ��..
	if (s_NavMesh == nullptr)
	{
		return false;
	}

	// ������ Near Face
	std::shared_ptr<NaviMeshFace> _nearface = s_NavMesh->GetNeviMeshFace(0);	// ù ��° Face�� �ʱ�ȭ

	// y�� ���� ���� ���
	float _minDistance = SimpleMath::Vector2::Distance(
		{ _nearface->m_CenterPosition.x, _nearface->m_CenterPosition.z },
		{ position.x, position.z }
	);

	/// NavMeshFace ���� ��ȸ�ϸ鼭 �˻�
	for (size_t i = 1; i < s_NavMesh->GetIndexSize(); i++)	// 0���� ������ �ʱ� ������ ���� ������ 1������ ��.
	{
		// �˻��� �κ� Face(Triangle) 
		std::shared_ptr<NaviMeshFace> _face = s_NavMesh->GetNeviMeshFace(i);

		// position �� face[i] �� �Ÿ��� ���Ѵ�.(xz �� ��� �������θ� �Ѵ�)
		SimpleMath::Vector2 _xzFaceVec = { _face->m_CenterPosition.x, _face->m_CenterPosition.z };
		SimpleMath::Vector2 _xzPosVec = { position.x, position.z };

		float _distance = SimpleMath::Vector2::Distance(_xzFaceVec, _xzPosVec);

		// ���� ���� ��
		if (_distance < _minDistance)
		{
			// ���� face���� �� �����ٸ� min ����
			_minDistance = _distance;

			_nearface = _face;
		}
	}

	// ���� ����� Face ����
	return _nearface;
}


/// <summary>
/// ����(Ŭ���̾�Ʈ�� ��ũ��Ʈ)�� Agent�� �������� �������� ��
/// �ش� �������� ��ġ�� ���Ž���� ����� NavMesh �󿡼��� ���� ������(�� ����)�� ����Ѵ�.
/// 
/// [Path Find �� �� ����ϴ� ��ġ ���]
/// </summary>
/// <returns>���� ����</returns>
bool NavMeshAgent::CalcDestinationOnNavMesh()
{
	// ������ ������Ʈ ������ �ȵǾ�  position ���
	if (m_DestinationObj == nullptr)
	{
		m_DestinationPosOnNavMesh = m_DestinationPos;
	}

	else
	{
		// ������ Obj�� ���� �� ���� Object�� ��ġ�� NavMesh ���� ����(����)���� �����Ѵ�.
		// ������ �̵��ص� ����ȭ
		m_DestinationPosOnNavMesh = m_DestinationObj->m_Transform->m_Position;
	}

	/// �������� NavMesh ���� �ִ��� �˻�
	std::shared_ptr<NaviMeshFace> _face = IsOnNavMesh(m_DestinationPosOnNavMesh);
	if (_face != nullptr)
	{
		// y���� �����ϰ� xz ���������� �׺�޽� ���� �ִٸ� 
		// �ش� y�� ��ǥ�� �ش� Face�� ������ �߰�(CenterPos.y)�� �����Ѵ�.
		m_DestinationPosOnNavMesh.y = _face->m_CenterPosition.y;

		return true;
	}

	// else
	/// ���������� ���� ����� Face�� ã�´�.
	std::shared_ptr<NaviMeshFace> _nearFace = FindNearNavFace(m_DestinationPosOnNavMesh);

	/// �������� NavMesh �ٱ��� �ִٸ� ���� ����� ������ ����ؼ� �����Ѵ�.
	/* �ﰢ���� �� ������ A, B, C �� �������� ������ ������ T�� Ī�Ѵ�.*/
	SimpleMath::Vector3 _T = m_DestinationPosOnNavMesh;

	/// �ﰢ�� ����
	SimpleMath::Vector3 _A = _nearFace->m_Vertex_Arr[0];
	SimpleMath::Vector3 _B = _nearFace->m_Vertex_Arr[1];
	SimpleMath::Vector3 _C = _nearFace->m_Vertex_Arr[2];

	/// �ﰢ�� ����Ŭ ����(��, edge)
	SimpleMath::Vector3 _TriVecAB = _nearFace->m_CycleEdgeVector_Arr[0];	// AB
	SimpleMath::Vector3 _TriVecBC = _nearFace->m_CycleEdgeVector_Arr[1];	// BC
	SimpleMath::Vector3 _TriVecCA = _nearFace->m_CycleEdgeVector_Arr[2];	// CA

	/// �ﰢ�� �������� Agent�� ���� ����
	SimpleMath::Vector3 _HitVecAT = _T - _A;	// AT
	SimpleMath::Vector3 _HitVecBT = _T - _B;	// BT
	SimpleMath::Vector3 _HitVecCT = _T - _C;	// CT

	// ������.y -> +,- ��ȣ�� Face�� ���⺤�͸� �������� ������ �ִ��� �������ִ��� �Ǻ� ����
	float _fCross0 = (_TriVecAB.z * _HitVecAT.x) - (_TriVecAB.x * _HitVecAT.z);
	float _fCross1 = (_TriVecBC.z * _HitVecBT.x) - (_TriVecBC.x * _HitVecBT.z);
	float _fCross2 = (_TriVecCA.z * _HitVecCT.x) - (_TriVecCA.x * _HitVecCT.z);
	/////////////////////////////////////////////////////////////////////////////////////////////////

	/// ������ position
	SimpleMath::Vector3 _newPosition = { 0.0f, _nearFace->m_CenterPosition.y, 0.0f };	// y�� ��ǥ�� Face�� �߽� ���̷� ����

	/// Face�� ����Ŭ ����(Edge��) ����ȭ(�߿�)
	// ���⼺�и� �����.
	SimpleMath::Vector3 _norVecAB = { _TriVecAB.x / _TriVecAB.Length(), _TriVecAB.y / _TriVecAB.Length(), _TriVecAB.z / _TriVecAB.Length() };
	SimpleMath::Vector3 _norVecBC = { _TriVecBC.x / _TriVecBC.Length(), _TriVecBC.y / _TriVecBC.Length(), _TriVecBC.z / _TriVecBC.Length() };
	SimpleMath::Vector3 _norVecCA = { _TriVecCA.x / _TriVecCA.Length(), _TriVecCA.y / _TriVecCA.Length(), _TriVecCA.z / _TriVecCA.Length() };

	/// ���밪 ��ȯ�ؼ� +�� ���̸� ����
	_fCross0 = std::fabsf(_fCross0);
	_fCross1 = std::fabsf(_fCross1);
	_fCross2 = std::fabsf(_fCross2);

	/// ���� ����� Edge(��)�� ã�´�.
	// Edge 0(AB)�� ���� ������.
	if ((_fCross0 <= _fCross1) &&
		(_fCross0 <= _fCross2))
	{
		float _fDot = _norVecAB.Dot(_HitVecAT);

		/// �������� Edge(��) ������ �Ѿ �ִ°�� Edge ������ �����Բ� �����Ѵ�.
		if (_fDot < 0.0f)
		{
			_fDot = 0.0f;
		}

		if (_fDot > _TriVecAB.Length())
		{
			_fDot = _TriVecAB.Length();
		}

		// ����(normalize) * ����(dot) + ����(���� ����� Edge�� ���� ��)
		_newPosition.x = _norVecAB.x * _fDot + _A.x;
		_newPosition.z = _norVecAB.z * _fDot + _A.z;
	}

	else
	{
		// Edge 1(BC)�� ���� ������.
		if (_fCross1 <= _fCross2)
		{
			float _fDot = _norVecBC.Dot(_HitVecBT);

			if (_fDot < 0.0f)
			{
				_fDot = 0.0f;
			}

			if (_fDot > _TriVecBC.Length())
			{
				_fDot = _TriVecBC.Length();
			}

			_newPosition.x = _norVecBC.x * _fDot + _B.x;
			_newPosition.z = _norVecBC.z * _fDot + _B.z;
		}

		else
		{
			// Edge 2(CA)�� ���� ������.
			float _fDot = _norVecCA.Dot(_HitVecCT);

			if (_fDot < 0.0f)
			{
				_fDot = 0.0f;
			}

			if (_fDot > _TriVecCA.Length())
			{
				_fDot = _TriVecCA.Length();
			}

			_newPosition.x = _norVecCA.x * _fDot + _C.x;
			_newPosition.z = _norVecCA.z * _fDot + _C.z;
		}
	}

	/// ���� ����� Edge(����)�� ���� ����� �������� �ű��.
	m_DestinationPosOnNavMesh = _newPosition;

	return true;
}

/// <summary>
/// Agent�� ���� ������ ��� ���� �ִ��� �˻�
/// </summary>
/// <returns></returns>
bool NavMeshAgent::IsAgentOnPath()
{
	// ����� Node ��ȸ
	for (const auto& node : m_Path)
	{
		// Agent�� �ִ� Face�� ������ ��� ���� �ִ� ������ �Ǵ�
		if (node->m_Face == m_LastOnNavMeshFace)
		{
			return true;
		}
	}

	// ��� Node�� Face �߿��� Agent�� ���� �ִ� LastOnFace�� ������ ��θ� ��Ż�� ������ ����
	return false;
}

/// <summary>
/// A* �˰������� ��ã��(��� Ž��) �Լ�
/// 
/// (Open List �� std::list �� ����)
/// </summary>
/// <param name="startFace">����� ���</param>
/// <param name="endFace">������ ���</param>
/// <returns>���</returns>
std::list<std::shared_ptr<AStarNode>> NavMeshAgent::PathFindAStar(std::shared_ptr<NaviMeshFace> startFace, std::shared_ptr<NaviMeshFace> endFace)
{
	std::list<std::shared_ptr<AStarNode>> _nodePath;			// ������ ���(AStarNode list)

	if (m_LastOnNavMeshFace == nullptr
		|| m_DestinationFace == nullptr
		|| startFace == nullptr
		|| endFace == nullptr)
	{
		return _nodePath;
	}

	// Start, End Node ����(End ����)
	std::shared_ptr<AStarNode> _endNode = std::make_unique<AStarNode>(endFace, nullptr, nullptr);
	std::shared_ptr<AStarNode> _startNode = std::make_unique<AStarNode>(startFace, nullptr, _endNode);

	std::list<std::shared_ptr<AStarNode>> _openList;			// ���� ���
	std::list<std::shared_ptr<AStarNode>> _closedList;			// ���� ���
	std::shared_ptr<AStarNode> _curNode;						// ���� ���

	// Open List�� Start Node �ְ� ����
	_openList.emplace_back(_startNode);

	// Open List�� �˻��� Node�� ���������� ��� ��� Ž��
	//while (_openList.size() > 0)
	while (!_openList.empty())
	{
		// CurNode �ʱ�ȭ
		_curNode = _openList.front();

		// Open List���� ���� F(n) ����� ���� Node�� �����Ѵ�.
		// F(n) �� ���ٸ� ��ǥ������ ����ġ�� ���� ������( H(n)�� ���� �� )
		std::list<std::shared_ptr<AStarNode>>::iterator _iter = _openList.begin();
		_iter++;
		for (size_t i = 1; i < _openList.size(); i++)
		{
			if ((*_iter)->m_F <= _curNode->m_F &&
				(*_iter)->m_H < _curNode->m_H)
			{
				_curNode = (*_iter);
			}
		}

		// ���õ� ����� ���� ���� node�� Open List���� Closed List�� �̵�
		_openList.remove(_curNode);
		_closedList.emplace_back(_curNode);

		//CA_TRACE("[PathFind] Open = %d / closed = %d", _openList.size(), _closedList.size());

		// Goal Check
		if (_curNode->m_Index == _endNode->m_Index)
		{
			/// ��� �����
			// �������� ����(end) ������ ����(start)������ _nodePath �� �߰�
			std::shared_ptr<AStarNode> _targetCurNode = _curNode;
			while (_targetCurNode != _startNode)
			{
				_nodePath.emplace_back(_targetCurNode);
				_targetCurNode = _targetCurNode->m_Parent;
			}
			_nodePath.push_back(_startNode);

			// �������� ���� path�� �������ؼ� (start -> end) ��θ� �����!
			_nodePath.reverse();

			// ��� ����
			return _nodePath;
		}

		// ���� Face ���� Open List �� �߰�
		for (size_t i = 0; i < _curNode->m_Face->m_AdjoinFace_V.size(); i++)
		{
			std::shared_ptr<NaviMeshFace> _face = _curNode->m_Face->m_AdjoinFace_V.at(i);
			AddOpenList(_openList, _closedList, _face, _curNode, _endNode);
		}

	}// end of while -> ������ end(dest) Node ������ ��θ� ã�� �� ����!

	// (��) ��� ����
	return _nodePath;	// start�� end�� ���� face�� �ִ� ��� or 
						// ���� �����ų� ���� ������ ����� Path�� ã�� �� ���� ���
}

/// <summary>
/// A* �˰������� ��ã��(��� Ž��) �Լ�
/// 
/// (Open List �� std::priority_queue �� ����)
/// </summary>
/// <param name="startFace"></param>
/// <param name="endFace"></param>
/// <returns></returns>
std::list<std::shared_ptr<AStarNode>> NavMeshAgent::PathFindAStar2(std::shared_ptr<struct NaviMeshFace> startFace, std::shared_ptr<struct NaviMeshFace> endFace)
{
	std::list<std::shared_ptr<AStarNode>> _nodePath;			// ������ ���(AStarNode list)

	if (m_LastOnNavMeshFace == nullptr
		|| m_DestinationFace == nullptr
		|| startFace == nullptr
		|| endFace == nullptr)
	{
		return _nodePath;
	}

	// Start, End Node ����(End ����)
	std::shared_ptr<AStarNode> _endNode = std::make_unique<AStarNode>(endFace, nullptr, nullptr);
	std::shared_ptr<AStarNode> _startNode = std::make_unique<AStarNode>(startFace, nullptr, _endNode);

	std::priority_queue<
		std::shared_ptr<AStarNode>,
		std::vector<std::shared_ptr<AStarNode>>,
		AstarNodeCmp> _openList;			// ���� ���

	std::list<std::shared_ptr<AStarNode>> _closedList;			// ���� ���
	std::shared_ptr<AStarNode> _curNode;						// ���� ���

	// Open List�� Start Node �ְ� ����
	_openList.push(_startNode);

	// Open List�� �˻��� Node�� ���������� ��� ��� Ž��
	while (_openList.empty() == false)
	{
		// CurNode �ʱ�ȭ(����� ���� ���� ����)
		_curNode = _openList.top();

		// ���õ� ����� ���� ���� node�� Open List���� Closed List�� �̵�
		_openList.pop();
		_closedList.emplace_back(_curNode);

#ifdef _DEBUG
		//CA_TRACE("[PathFind] Open = %d / closed = %d", _openList.size(), _closedList.size());
#endif

		// Goal Check
		if (_curNode->m_Index == _endNode->m_Index)
		{
			/// ��� �����
			// �������� ����(end) ������ ����(start)������ _nodePath �� �߰�
			std::shared_ptr<AStarNode> _targetCurNode = _curNode;
			while (_targetCurNode != _startNode)
			{
				_nodePath.emplace_back(_targetCurNode);
				_targetCurNode = _targetCurNode->m_Parent;
			}
			_nodePath.push_back(_startNode);

			// �������� ���� path�� �������ؼ� (start -> end) ��θ� �����!
			_nodePath.reverse();

			// ��� ����
			return _nodePath;
		}

		// ���� Face ���� Open List �� �߰�
		for (size_t i = 0; i < _curNode->m_Face->m_AdjoinFace_V.size(); i++)
		{
			std::shared_ptr<NaviMeshFace> _face = _curNode->m_Face->m_AdjoinFace_V.at(i);
			//AddOpenList(_openList, _closedList, _face, _curNode, _endNode);
			AddOpenList2(_openList, _closedList, _face, _curNode, _endNode);
		}

	}// end of while -> ������ end(dest) Node ������ ��θ� ã�� �� ����!

	// (��) ��� ����
	return _nodePath;	// start�� end�� ���� face�� �ִ� ��� or 
						// ���� �����ų� ���� ������ ����� Path�� ã�� �� ���� ���
}


/// <summary>
/// Path Find �� Open List�� Node�� �߰��Ѵ�.
/// </summary>
/// <param name="openList">Path Find�� ������� Open List</param>
/// <param name="closedList">Path Find�� ������� Closed List</param>
/// <param name="face">���� �߰��� Node�� Face</param>
/// <param name="parentNode">�θ�(����)���</param>
/// <param name="endNode">��(����)���</param>
void NavMeshAgent::AddOpenList(std::list<std::shared_ptr<AStarNode>>& openList, std::list<std::shared_ptr<AStarNode>>& closedList, std::shared_ptr<NaviMeshFace> face, std::shared_ptr<AStarNode> parentNode, std::shared_ptr<AStarNode> endNode)
{
	// Face�� �ش��ϴ� Node�� �̹� Closed List�� �߰��Ǿ��ִ��� �˻�
	// (�̹� �˻縦 �� Node�� �� �˻��� �ʿ䰡 ����)
	for (const auto& node : closedList)
	{
		if (node->m_Index == face->m_Index)
		{
			// Closed List �� �̹� �߰��Ǿ� ������ Open List�� �� �߰����� ����.
			return;
		}
	}

	// Closed List �� ���ٸ�

	// �߰��� �̿�(�ڽ�)��� ����
	std::shared_ptr<AStarNode> neighborNode = std::make_shared<AStarNode>(face, parentNode, endNode);

	// Open List�� �ִ��� �˻�
	// ( Open List �ߺ�üũ�� ���ϸ� Open List�� ���� Node�� ��� ���̰� �Ǹ鼭 ��� �þ..)
	for (const auto& node : openList)
	{
		if (node->m_Index == neighborNode->m_Index)
		{
			// Open List �� �̹� �߰��Ǿ� ������ �� �߰����� ����.
			return;
		}
	}

	// Open List �� �߰�
	openList.emplace_back(neighborNode);
}

void NavMeshAgent::AddOpenList2(std::priority_queue<std::shared_ptr<AStarNode>, std::vector<std::shared_ptr<AStarNode>>, AstarNodeCmp>& openList, std::list<std::shared_ptr<AStarNode>>& closedList, std::shared_ptr<struct NaviMeshFace> face, std::shared_ptr<class AStarNode> parentNode, std::shared_ptr<AStarNode> endNode)
{
	/// Face�� �ش��ϴ� Node�� �̹� Closed List�� �߰��Ǿ��ִ��� �˻�
	// (�̹� �˻縦 �� Node�� �� �˻��� �ʿ䰡 ����)
	for (const auto& node : closedList)
	{
		if (node->m_Index == face->m_Index)
		{
			// Closed List �� �̹� �߰��Ǿ� ������ Open List�� �� �߰����� ����.
			return;
		}
	}

	// Closed List �� ���ٸ�

	// �߰��� �̿�(�ڽ�)��� ����
	std::shared_ptr<AStarNode> neighborNode = std::make_shared<AStarNode>(face, parentNode, endNode);

	/// Open List�� �ִ��� �˻�
	// ( Open List �ߺ�üũ�� ���ϸ� Open List�� ���� Node�� ��� ���̰� �Ǹ鼭 ��� �þ..)

	// ��ȸ�� �ӽ� Open List
	std::priority_queue<std::shared_ptr<AStarNode>,
		std::vector<std::shared_ptr<AStarNode>>,
		AstarNodeCmp> _copyOpenList_L = openList;

	// �� �񱳿� �ӽ� Open List
	std::vector< std::shared_ptr<AStarNode>> _copyOpenList_V;
	_copyOpenList_V.reserve(openList.size());

	while (_copyOpenList_L.empty() == false)
	{
		_copyOpenList_V.emplace_back(_copyOpenList_L.top());
		_copyOpenList_L.pop();
	}

	for (const auto& node : _copyOpenList_V)
	{
		if (node->m_Index == neighborNode->m_Index)
		{
			// Open List �� �̹� �߰��Ǿ� ������ �� �߰����� ����.
			return;
		}
	}

	// Open List �� �߰�
	openList.push(neighborNode);
}

/// <summary>
/// [Ver 1] - ���� Node�� Face�� ������ ���� �̵��Ѵ�.
/// 
/// �������� �����Ǿ� ������ ��(���)�� ���� �ڵ����� �̵��ϴ� �Լ�
/// </summary>
void NavMeshAgent::MoveToDestinationVer1()
{
	float _dist = SimpleMath::Vector3::Distance(m_Transform->m_Position, m_DestinationPosOnNavMesh);
	if (_dist < 0.1f)
	{
		return;
	}

	// �̵�, ȸ��(�ٶ� Look ����) ���� �ʱ�ȭ
	SimpleMath::Vector3 _moveDir = SimpleMath::Vector3::Zero;
	SimpleMath::Vector3 _lookdir = SimpleMath::Vector3::Zero;

	// ���� End Node(���� Face)�� �����ϱ� ���̸�
	// ���� ����� ������ ���� �̵�
	if (m_Path.size() > 0 && m_LastOnNavMeshFace != m_Path.back()->m_Face)
	{
		// ���� Node�� ���� Node�� ã�´�.
		std::shared_ptr<AStarNode> _curNode = nullptr;
		std::shared_ptr<AStarNode> _nextNode = nullptr;
		SimpleMath::Vector3 _nextWayPoint = SimpleMath::Vector3::Zero;

		std::list<std::shared_ptr<AStarNode>>::iterator iter = m_Path.begin();

		for (const auto& node : m_Path)
		{
			if (node->m_Face == m_LastOnNavMeshFace)
			{
				_curNode = node;

				_nextNode = *(++iter);
				_nextWayPoint = (*_nextNode).m_Face->m_CenterPosition;
				break;
			}
			++iter;
		}

		// ȸ������ ���
		_lookdir = _nextWayPoint;

		// �̵����� ���
		_moveDir = _nextWayPoint - m_Transform->m_Position;
		_moveDir.Normalize();
	}

	// End Node(���� Face)�� ���������� destination �� ���� ���� ����
	else
	{
		// ȸ������ ���
		_lookdir = m_DestinationPosOnNavMesh;

		// �̵����� ���
		_moveDir = m_DestinationPosOnNavMesh - m_Transform->m_Position;
		_moveDir.Normalize();
	}

	// ���⿡ ���ǵ带 ���ϰ� deltaTime �� ����
	_moveDir *= m_MoveSpeed * DLLTime::DeltaTime();

	/// Agent ���� ȸ��
	m_Transform->LookAtYaw(_lookdir);

	/// Agent �̵�
	// ������ ���Ͱ� �ִ� ��� �������� �̵�
	if (m_MyPhysicsActor != nullptr)
	{
		MoveByPhysX(m_MyPhysicsActor, _moveDir);
	}

	// ������ ���Ͱ� ������ Ʈ������ �̵�
	else
	{
		m_Transform->SetPosition(m_Transform->m_Position + _moveDir);
	}
}


/// <summary>
/// [Ver 2] - �̱���
/// 
/// �������� �����Ǿ� ������ ��(���)�� ���� �ڵ����� �̵��ϴ� �Լ�
/// </summary>
void NavMeshAgent::MoveToDestinationVer2()
{
}

/// <summary>
/// Agent�� PhysicsActor ������Ʈ�� ���� �� �����̵��� �ϴ� �Լ�
/// </summary>
/// <param name="physActor">PhyscisActor Component</param>
/// <param name="dir">�̵� ����</param>
void NavMeshAgent::MoveByPhysX(PhysicsActor* physActor, SimpleMath::Vector3& dir)
{
	// Ű�׸�ƽ�� �����Ǿ������� �����̵� �Ұ����ϹǷ�
	// Ʈ������ �̵�
	if (physActor->IsKinematic() == true)
	{
		/*
		* �Ϲ������� �����ϴ� ��ġ�� �̵��ӵ��� �����ϸ�
		* �ʹ� ���� �������� ���� ���ҽ����� offset ��
		*/
		m_Transform->SetPosition(m_Transform->m_Position + dir);
	}

	// Ű�׸�ƽ false �� �� ���� �̵�
	else
	{
		physActor->SetVelocity(dir);
	}
}

/// <summary>
/// Debug�� �Լ�
/// LastOnNavMeshFace �� �ε����� ������ Adjoin Face�� �ε����� ���� ���â�� ����Ѵ�.
/// </summary>
void NavMeshAgent::PrintLastOnFaceInfo() const
{
	int adjoins[3] = { -1, -1, -1 };	// -1 �� �����ѹ�(���� Face�� ���ٴ� ��)

	for (size_t i = 0; i < m_LastOnNavMeshFace->m_AdjoinFace_V.size(); i++)
	{
		adjoins[i] = m_LastOnNavMeshFace->m_AdjoinFace_V.at(i)->m_Index;
	}
	CA_TRACE("[NavMeshAgent] now Face %d / Adjoin %d, %d, %d", m_LastOnNavMeshFace->m_Index, adjoins[0], adjoins[1], adjoins[2]);
}

