#include "pch.h"
#include "IRenderer.h"
#include "EngineDLL.h"
#include "Transform.h"
#include "PhysicsActor.h"
#include "Managers.h"
#include "NaviMeshFace.h"
#include "NavMeshAgent.h"

// NavMeshManager를 받아서 사용한다.
NavMeshManager* NavMeshAgent::s_NavMeshManager = nullptr;
std::shared_ptr<NaviMeshSet> NavMeshAgent::s_NavMesh = nullptr;

// Render 용 identity matrix 생성
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
	// 인덱스 할당
	m_Index = m_Face->m_Index;

	// G, H 계산에 사용할 현재 노드의 위치 벡터
	SimpleMath::Vector3 _myPosition = { this->m_Face->m_CenterPosition.x, 0.0f, this->m_Face->m_CenterPosition.z };

	/// G 계산
	if (m_Parent == nullptr)
	{
		m_G = 0.0f;
	}
	else
	{
		SimpleMath::Vector3 _paretPosition = { m_Parent->m_Face->m_CenterPosition.x, 0.0f, m_Parent->m_Face->m_CenterPosition.z };
		float distance = SimpleMath::Vector3::Distance(_myPosition, _paretPosition);

		// 시작 노드부터 부모(이전)노드까지의 거리 + 부모(이전)노드와 자신(현재)노드 사이의 거리
		m_G = parent->m_G + distance;
	}

	/// H 계산
	// 현재 노드(Face)로부터 Dest까지의 거리로 한다.(y축 제외하고 2차원 x,z 평면 상의 길이로 계산)
	if (endNode == nullptr)
	{
		m_H = 0.0f;
	}
	else
	{
		SimpleMath::Vector3 _destPosition = { endNode->m_Face->m_CenterPosition.x, 0.0f, endNode->m_Face->m_CenterPosition.z };

		// 현재 노드(face)로부터 Dest 까지의 거리
		m_H = SimpleMath::Vector3::Distance(_myPosition, _destPosition);
	}

	/// F 계산
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

	//NavMeshManager에 자신을 등록	
	s_NavMeshManager->RegisterNavMeshAgent(this);

}

NavMeshAgent::~NavMeshAgent()
{
	// NavMeshManager에 자신을 제거 요청
	s_NavMeshManager->DeleteNavMeshAgent(this);
}

void NavMeshAgent::Start()
{
	IsOnNavMesh(m_Transform->m_Position);
	m_MyPhysicsActor = m_pMyObject->GetComponent<PhysicsActor>();
}

void NavMeshAgent::Update(float dTime)
{
	// 네비 매쉬 충돌 판정 갱신
	IsOnNavMesh(m_Transform->m_Position);

	// Agent가 네비 메쉬 영역 위에 위치한 경우
	if (m_bOnNavMesh == true)
	{
		// 오브젝트의 포지션 보정
		AdjustAgentPositionY();

		// 마지막으로 있었던 Face와 인접Face index 출력
#ifdef _DEBUG
		//PrintLastOnFaceInfo();
#endif
	}

	// 네비메쉬를 벗어난 경우
	else
	{
#ifdef _DEBUG
		//CA_TRACE("[NavMeshAgent] 네비메쉬를 벗어남..");
#endif
		/// 유효한 네비메쉬 영역을 이동하다가 바깥으로 빠져나가려고 할 때 충돌처리(방향(Vector)의 수정)
		MoveToNearNavFaceLineVer3();
	}

	// 설정된 목적지가 있는 경우
	//if (m_DestinationObj != nullptr)
	// 자동 이동 모드 일 경우
	if (m_bAutoMoveMode == true)
	{
		// 경로 탐색에 사용할 네비 메쉬 위의 위치를 계산
		CalcDestinationOnNavMesh();
		//CA_TRACE("[NavMeshAgent] m_Destinatnion = %f / %f / %f", (*m_DestinationPos).x, (*m_DestinationPos).y, (*m_DestinationPos).z);
		//CA_TRACE("[NavMeshAgent] Mapping 목적지 = %f / %f / %f", m_DestinationPosOnNavMesh.x, m_DestinationPosOnNavMesh.y, m_DestinationPosOnNavMesh.z);

		/// 목적지가 다른 Face로 이동했거나, Agent가 경로를 이탈한 경우에 경로 재탐색
		// 목적지가 다른 곳으로 이동한 경우
		std::shared_ptr<NaviMeshFace> _nowDestFace = IsOnNavMesh(m_DestinationPosOnNavMesh);

		if (_nowDestFace != m_DestinationFace)
		{
			m_Path.clear();
			//m_Path = PathFindAStar(m_LastOnNavMeshFace, _nowDestFace);
			m_Path = PathFindAStar2(m_LastOnNavMeshFace, _nowDestFace);
		}
		// 목적지 Face 갱신
		m_DestinationFace = _nowDestFace;

		// Agent가 경로를 이탈한 경우
		if (IsAgentOnPath() == false)
		{
			m_Path.clear();
			//m_Path = PathFindAStar(m_LastOnNavMeshFace, m_DestinationFace);
			m_Path = PathFindAStar2(m_LastOnNavMeshFace, m_DestinationFace);
		}

		// 경로를 따라 이동
		MoveToDestinationVer1();
	}


	if (DLLInput::InputKey(0x5A)) // Z 입력시 테스트
	{
		//MoveToNearNavFaceCenter();	// -> 긴급탈출 같은 느낌(Face의 중심으로 이동)
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
/// 마지막으로 위치해있던 NavMeshFace를 초기화 하는 함수
/// NavMeshManager에서 NavMesh가 교체되었을 때 이전 NavMesh의 정보를 가지고 있는 것을
/// 방지하기 위해서 초기화 해주기 위해 만듦.
/// </summary>
void NavMeshAgent::ResetLastOnNavMeshFace()
{
	m_LastOnNavMeshFace = nullptr;
}

/// <summary>
/// Agent가 네비매쉬 위에 엤는지 유효성 판단
/// </summary>
/// /// <param name="position">검사할 기준 위치</param>
/// <returns>Agent가 있는 Face</returns>
std::shared_ptr<struct NaviMeshFace> NavMeshAgent::IsOnNavMesh(SimpleMath::Vector3 position)
{
	/// 검사할 네비메쉬가 없을 때..
	if (s_NavMesh == nullptr)
	{
		return false;
	}

	/// 모든 NavMeshFace 들을 순회하면서 검사
	for (size_t i = 0; i < s_NavMesh->GetIndexSize(); i++)
	{
		// 검사할 부분 Face(Triangle) 
		std::shared_ptr<NaviMeshFace> _face = s_NavMesh->GetNeviMeshFace(i);

		// position 이 _face 위에 존재하는가?
		if (IsOnNavMeshFace(_face, position))
		{
			// 네비메쉬 위에 존재한다.
			return _face;
		}
	}

	// 네비메쉬 위에 없는 경우
	return nullptr;
}

/// <summary>
/// Agent 가 특정 Face(Triangle) 위에 존재하는지 검사
/// 
/// 2D(2차원)적으로 계산한다.. 단일 xz 평면상에서 계산..
/// </summary>
/// <param name="face">검사할 Face</param>
/// <param name="position">기준 위치</param>
/// <returns>판정 결과</returns>
bool NavMeshAgent::IsOnNavMeshFace(std::shared_ptr<NaviMeshFace>& face, SimpleMath::Vector3 position)
{
	/* 삼각형의 각 정점을 A, B, C 로 검사할 포지션 정점을 T로 칭한다.*/
	SimpleMath::Vector3 _T = position;

	/// 삼각형 정점
	SimpleMath::Vector3 _A = face->m_Vertex_Arr[0];
	SimpleMath::Vector3 _B = face->m_Vertex_Arr[1];
	SimpleMath::Vector3 _C = face->m_Vertex_Arr[2];

	/// 삼각형 사이클 벡터(변, edge)
	SimpleMath::Vector3 _TriVecAB = face->m_CycleEdgeVector_Arr[0];	// AB
	SimpleMath::Vector3 _TriVecBC = face->m_CycleEdgeVector_Arr[1];	// BC
	SimpleMath::Vector3 _TriVecCA = face->m_CycleEdgeVector_Arr[2];	// CA

	/// 삼각형 정점에서 Agent로 향한 벡터
	SimpleMath::Vector3 _HitVecAT = _T - _A;	// AT
	SimpleMath::Vector3 _HitVecBT = _T - _B;	// BT
	SimpleMath::Vector3 _HitVecCT = _T - _C;	// CT

	/// 각각의 외적(y 성분만 추출)
	// 이 식은 삼격형(페이스)가 xz 평면위에 있다는 것을 전제로한다.(마치 2D처럼..)
	// (실제로 사용하는 NavMesh는 3D(x,y,z) 성분을 모두 사용하지만 위아래로 겹치는 부분이 없어서 이렇게(간단히) 시도해봄..)
	// 
	// 각 외적의 y 성분만 계산한 것이다.
	// 외적의 y 성분 부호를 보면 두 벡터의 좌우 관계를 알 수 있다.

	/* SeedVault 의 각 맵(네비메쉬)의 구조가 위에서 봤을 때 같은 y축에 평행한 직선에 통과되는 곳이 없다는 것을 전제로 한다.*/
	/* 그러니까 위,아래로 겹치는 삼각형(페이스)가 없다는(없어야 한다는) 말!! */

	// 외적값.y -> +,- 부호로 Face의 유향벡터를 기준으로 좌측에 있는지 우측에있는지 판별 가능
	float _fCross0 = (_TriVecAB.z * _HitVecAT.x) - (_TriVecAB.x * _HitVecAT.z);
	float _fCross1 = (_TriVecBC.z * _HitVecBT.x) - (_TriVecBC.x * _HitVecBT.z);
	float _fCross2 = (_TriVecCA.z * _HitVecCT.x) - (_TriVecCA.x * _HitVecCT.z);

	/// 충돌 여부
	bool _bHit = false;
	if (position == m_Transform->m_Position)
	{
		// 검사 대상 Pos가 Agent 본인인 경우 충돌 판별 재확인
		// Dest 에 대해서 검사할 때는 안한다.
		m_bOnNavMesh = false;
	}

	/// 부호가 전부 같으면 충돌한 것이다.
	// 전부 양수인 경우
	if (_fCross0 >= 0.0f)
	{
		if ((_fCross1 >= 0.0f) && (_fCross2 >= 0.0f))
		{
			_bHit = true;

			// 검사한 position이 Agent 의 것인 경우(또는 같은 경우)
			if (position == m_Transform->m_Position)
			{
				// Agent가 네비메쉬 위에 있는걸로 설정
				m_bOnNavMesh = true;
				m_LastOnNavMeshFace = face;	// Face 갱신
			}
		}
	}

	// 전부 음수인 경우
	else
	{
		if ((_fCross1 < 0.0f) && (_fCross2 < 0.0f))
		{
			_bHit = true;

			// 검사한 position이 Agent 의 것인 경우(또는 같은 경우)
			if (position == m_Transform->m_Position)
			{
				// Agent가 네비메쉬 위에 있는걸로 설정
				m_bOnNavMesh = true;
				m_LastOnNavMeshFace = face;	// Face 갱신
			}
		}
	}

	/// 검사한 position이 Agent의 position일 때(와 같을 때)
	// 이 부분은 없어도 충돌판정 자체는 가능하다.
	// 하지만 충돌판정에 필요한 데이터를 미리 만들어두기 위해서 계산해둔다.(충돌시 벡터 재계산에 필요!!)
	if (position == m_Transform->m_Position	// 판정하는 대상이 Agent일 때에만 충돌 데이터 갱신한다.
		&& m_bOnNavMesh == true)
	{
		/// 충돌 검사 벡터 재계산에 사용하기 위해서 가장 가가운 유향 벡터를 선택한다.
		// 절대값 변환해서 +의 길이를 만듦
		float _absfCross0 = std::fabsf(_fCross0);	// 사실 외적 결과 벡터의 Length를 그대로 사용하기 떄문에
		float _absfCross1 = std::fabsf(_fCross1);	// 두 벡터로 만들어지는 평행사변형의 넓이라고 봐야함
		float _absfCross2 = std::fabsf(_fCross2);	// 넓이의 크기로도 비교 가능

		/// 가장 가까운 Edge(변)을 찾는다.
		// Edge 0(AB)에 가장 가깝다.
		if ((_absfCross0 <= _absfCross1) &&
			(_absfCross0 <= _absfCross2))
		{
			/*
			가장 가까웠던 Edge(Vector)를 기준으로

			시작점,
			벡터(시작점->끝점) Edge의 StartToEnd,
			벡터(시작점->판정점) HitVector,

			를 보관한다.
			*/
			m_LastCycleEdgeVector = _TriVecAB;
			m_StartPositionOfLastCycleEdge = _A;
		}

		else
		{
			// Edge 1(BC)에 가장 가깝다.
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

	// 결과 리턴
	return _bHit;
}

/// <summary>
/// Agent가 네비메쉬 영역을 벗어났을 때 위치를 조정하기 위한 함수
/// (일단 가장 가까운 NavFace의 CenterPosition 으로 이동시킨다.)
/// </summary>
/// <returns></returns> 
bool NavMeshAgent::MoveToNearNavFaceCenter()
{
	// 먼저 가장 가까운 Face를 찾는다.
	std::shared_ptr<NaviMeshFace> _nearFace = FindNearNavFace(m_Transform->m_Position);

	// Agent의 Position을 이동시킨다.
	m_Transform->SetPosition(_nearFace->m_CenterPosition);

	// y축 위치 보정
	AdjustAgentPositionY();

	// 충돌 데이터 계산과 m_bOnNavMesh 갱신을 위해 이동 후의 위치로 검사
	IsOnNavMeshFace(_nearFace, m_Transform->m_Position);

	// 이동한 Face로 LastOntNavMeshFace 를 설정
	m_LastOnNavMeshFace = _nearFace;


	return true;
}

/// <summary>
/// [Ver.1 함수가 호출된 시점에 실시간으로 가장 가까운 Line을 계산해서 이동] 가장 기본형..
/// Agent의 position(판정점)으로부터 가장 가까운 Face의 Edge 위의 점으로 이동한다.
/// 
/// Agent가 네비메쉬 위에서 이동하다가 제한지역으로 넘어가려고 할 때 방향을 조정하기 위해 사용

// ver.1 함수는 한계점이 있다.
// Face 밖에 있을 때 실시간으로 가까운 Vector를 계산해서 Agent가 이동하던 방향을 유지하지 못하고
// 다른 Edge(vector)와 연산이 되면서 방향이 바뀌는 현상이 있다.
// 게다가 Edge의 양끝 (Start, End) 포지션을 넘어 가는 부분에 대한 처리도 하지 못한다.
//
/// </summary>
/// <returns></returns>
bool NavMeshAgent::MoveToNearNavFaceLineVer1()
{
	// 애초에 시작 위치가 네비매쉬 영역을 벗어나면 m_LastOntNavMeshFace 가 nullptr 일 수 있다.
	// 이때는 가장 가까운 페이스로 우선 옮겨준다.
	if (!m_LastOnNavMeshFace)
	{
		MoveToNearNavFaceCenter();
	}

	/* 삼각형의 각 정점을 A, B, C 로 Agent의 포지션 정점을 T로 칭한다.*/
	SimpleMath::Vector3 _T = m_Transform->m_Position;

	/// 삼각형 정점
	SimpleMath::Vector3 _A = m_LastOnNavMeshFace->m_Vertex_Arr[0];
	SimpleMath::Vector3 _B = m_LastOnNavMeshFace->m_Vertex_Arr[1];
	SimpleMath::Vector3 _C = m_LastOnNavMeshFace->m_Vertex_Arr[2];

	/// 삼각형 사이클 벡터(변, edge)
	SimpleMath::Vector3 _TriVecAB = m_LastOnNavMeshFace->m_CycleEdgeVector_Arr[0];	// AB
	SimpleMath::Vector3 _TriVecBC = m_LastOnNavMeshFace->m_CycleEdgeVector_Arr[1];	// BC
	SimpleMath::Vector3 _TriVecCA = m_LastOnNavMeshFace->m_CycleEdgeVector_Arr[2];	// CA

	/// 삼각형 정점에서 Agent로 향한 벡터
	SimpleMath::Vector3 _HitVecAT = _T - _A;	// AT
	SimpleMath::Vector3 _HitVecBT = _T - _B;	// BT
	SimpleMath::Vector3 _HitVecCT = _T - _C;	// CT

	// 외적값.y -> +,- 부호로 Face의 유향벡터를 기준으로 좌측에 있는지 우측에있는지 판별 가능
	float _fCross0 = (_TriVecAB.z * _HitVecAT.x) - (_TriVecAB.x * _HitVecAT.z);
	float _fCross1 = (_TriVecBC.z * _HitVecBT.x) - (_TriVecBC.x * _HitVecBT.z);
	float _fCross2 = (_TriVecCA.z * _HitVecCT.x) - (_TriVecCA.x * _HitVecCT.z);
	/////////////////////////////////////////////////////////////////////////////////////////////////

	/// 이동할 position
	SimpleMath::Vector3 _newPosition = { 0.0f, _T.y, 0.0f };

	/// Face의 사이클 벡터(Edge들) 단위화(중요)
	// 방향성분만 남긴다.
	SimpleMath::Vector3 _norVecAB = { _TriVecAB.x / _TriVecAB.Length(), _TriVecAB.y / _TriVecAB.Length(), _TriVecAB.z / _TriVecAB.Length() };
	SimpleMath::Vector3 _norVecBC = { _TriVecBC.x / _TriVecBC.Length(), _TriVecBC.y / _TriVecBC.Length(), _TriVecBC.z / _TriVecBC.Length() };
	SimpleMath::Vector3 _norVecCA = { _TriVecCA.x / _TriVecCA.Length(), _TriVecCA.y / _TriVecCA.Length(), _TriVecCA.z / _TriVecCA.Length() };

	/// 절대값 변환해서 +의 길이를 만듦
	_fCross0 = std::fabsf(_fCross0);		// 사실 외적 결과 벡터의 Length를 그대로 사용하기 떄문에
	_fCross1 = std::fabsf(_fCross1);		// 두 벡터로 만들어지는 평행사변형의 넓이라고 봐야함
	_fCross2 = std::fabsf(_fCross2);		// 넓이의 크기로도 비교 가능

	/// 가장 가까운 Edge(변)을 찾는다.
	// Edge 0(AB)에 가장 가깝다.
	if ((_fCross0 <= _fCross1) &&
		(_fCross0 <= _fCross2))
	{
		float _fDot = _norVecAB.Dot(_HitVecAT);

		// 방향(normalize) * 길이(dot) + 원점(가장 가까운 Edge의 시작 점)
		_newPosition.x = _norVecAB.x * _fDot + m_LastOnNavMeshFace->m_Vertex_Arr[0].x;
		_newPosition.z = _norVecAB.z * _fDot + m_LastOnNavMeshFace->m_Vertex_Arr[0].z;
	}

	else
	{
		// Edge 1(BC)에 가장 가깝다.
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
			// Edge 2(CA)에 가장 가깝다.
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

	/// 가장 가까운 Edge(벡터)의 가장 가까운 지점으로 옮긴다.
	m_Transform->SetPosition(_newPosition);
	AdjustAgentPositionY();

	return true;
}

/// <summary>
/// [Ver.2 함수가 호출된 시점에 실시간으로 가장 가까운 Line을 계산해서 이동]
/// Agent의 position(판정점)으로부터 가장 가까운 Face의 Edge 위의 점으로 이동한다.
/// 
/// Agent가 네비메쉬 위에서 이동하다가 제한지역으로 넘어가려고 할 때 방향을 조정하기 위해 사용

// ver.2 함수는 한계점이 있다.
//
// 유효한 NavMesh 영역 안에서 Face의 정점이 겹치는 부분이 있는 경우에 
// 다음 Face로 매끄럽게 넘어가지 못하고 버벅이는 현상이 있다..
//
// Ver1 에서 개선사항
// 맵이 코너부분에서 네비매쉬 밖으로 나가지 못하게 커버하는 부분은 정상으로 동작한다!! 
// Edge(Vector)의 양끝(Start, End) 포지션을 넘어가는 경우 제한 가능.
/// </summary>
/// <returns></returns>
bool NavMeshAgent::MoveToNearNavFaceLineVer2()
{
	// 애초에 시작 위치가 네비매쉬 영역을 벗어나면 m_LastOntNavMeshFace 가 nullptr 일 수 있다.
	// 이때는 가장 가까운 페이스로 우선 옮겨준다.
	if (!m_LastOnNavMeshFace)
	{
		MoveToNearNavFaceCenter();
	}

	/* 삼각형의 각 정점을 A, B, C 로 Agent의 포지션 정점을 T로 칭한다.*/
	SimpleMath::Vector3 _T = m_Transform->m_Position;

	/// 삼각형 정점
	SimpleMath::Vector3 _A = m_LastOnNavMeshFace->m_Vertex_Arr[0];
	SimpleMath::Vector3 _B = m_LastOnNavMeshFace->m_Vertex_Arr[1];
	SimpleMath::Vector3 _C = m_LastOnNavMeshFace->m_Vertex_Arr[2];

	/// 삼각형 사이클 벡터(변, edge)
	SimpleMath::Vector3 _TriVecAB = m_LastOnNavMeshFace->m_CycleEdgeVector_Arr[0];	// AB
	SimpleMath::Vector3 _TriVecBC = m_LastOnNavMeshFace->m_CycleEdgeVector_Arr[1];	// BC
	SimpleMath::Vector3 _TriVecCA = m_LastOnNavMeshFace->m_CycleEdgeVector_Arr[2];	// CA

	/// 삼각형 정점에서 Agent로 향한 벡터
	SimpleMath::Vector3 _HitVecAT = _T - _A;	// AT
	SimpleMath::Vector3 _HitVecBT = _T - _B;	// BT
	SimpleMath::Vector3 _HitVecCT = _T - _C;	// CT

	// 외적값.y -> +,- 부호로 Face의 유향벡터를 기준으로 좌측에 있는지 우측에있는지 판별 가능
	float _fCross0 = (_TriVecAB.z * _HitVecAT.x) - (_TriVecAB.x * _HitVecAT.z);
	float _fCross1 = (_TriVecBC.z * _HitVecBT.x) - (_TriVecBC.x * _HitVecBT.z);
	float _fCross2 = (_TriVecCA.z * _HitVecCT.x) - (_TriVecCA.x * _HitVecCT.z);
	/////////////////////////////////////////////////////////////////////////////////////////////////

	/// 이동할 position
	SimpleMath::Vector3 _newPosition = { 0.0f, _T.y, 0.0f };

	/// Face의 사이클 벡터(Edge들) 단위화(중요)
	// 방향성분만 남긴다.
	SimpleMath::Vector3 _norVecAB = { _TriVecAB.x / _TriVecAB.Length(), _TriVecAB.y / _TriVecAB.Length(), _TriVecAB.z / _TriVecAB.Length() };
	SimpleMath::Vector3 _norVecBC = { _TriVecBC.x / _TriVecBC.Length(), _TriVecBC.y / _TriVecBC.Length(), _TriVecBC.z / _TriVecBC.Length() };
	SimpleMath::Vector3 _norVecCA = { _TriVecCA.x / _TriVecCA.Length(), _TriVecCA.y / _TriVecCA.Length(), _TriVecCA.z / _TriVecCA.Length() };

	/// 절대값 변환해서 +의 길이를 만듦
	_fCross0 = std::fabsf(_fCross0);		// 사실 외적 결과 벡터의 Length를 그대로 사용하기 떄문에
	_fCross1 = std::fabsf(_fCross1);		// 두 벡터로 만들어지는 평행사변형의 넓이라고 봐야함
	_fCross2 = std::fabsf(_fCross2);		// 넓이의 크기로도 비교 가능

	/// 가장 가까운 Edge(변)을 찾는다.
	// Edge 0(AB)에 가장 가깝다.
	if ((_fCross0 <= _fCross1) &&
		(_fCross0 <= _fCross2))
	{
		float _fDot = _norVecAB.Dot(_HitVecAT);

		/// Ver2 추가 사항
		/// Agent가 Edge(변) 영역을 넘어가 있는경우 Edge 안으로 들어오게끔 보정한다.
		// 연결되어 있는 유효한 Face로 이동할 때는 좀 걸리는 느낌이 남아있지만...
		// 맵의 구석에서 네비매쉬 밖으로 나가는 경우는 케어 가능하다!!
		if (_fDot < 0.0f)
		{
			_fDot = 0.0f;
		}

		if (_fDot > _TriVecAB.Length())
		{
			_fDot = _TriVecAB.Length();
		}

		// 방향(normalize) * 길이(dot) + 원점(가장 가까운 Edge의 시작 점)
		_newPosition.x = _norVecAB.x * _fDot + m_LastOnNavMeshFace->m_Vertex_Arr[0].x;
		_newPosition.z = _norVecAB.z * _fDot + m_LastOnNavMeshFace->m_Vertex_Arr[0].z;
	}

	else
	{
		// Edge 1(BC)에 가장 가깝다.
		if (_fCross1 <= _fCross2)
		{
			float _fDot = _norVecBC.Dot(_HitVecBT);

			/// Ver2 추가 사항
			/// Agent가 Edge(변) 영역을 넘어가 있는경우 Edge 안으로 들어오게끔 보정한다.
			// 연결되어 있는 유효한 Face로 이동할 때는 좀 걸리는 느낌이 남아있지만...
			// 맵의 구석에서 네비매쉬 밖으로 나가는 경우는 케어 가능하다!!
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
			// Edge 2(CA)에 가장 가깝다.
			float _fDot = _norVecCA.Dot(_HitVecCT);

			/// Ver2 추가 사항
			/// Agent가 Edge(변) 영역을 넘어가 있는경우 Edge 안으로 들어오게끔 보정한다.
			// 연결되어 있는 유효한 Face로 이동할 때는 좀 걸리는 느낌이 남아있지만...
			// 맵의 구석에서 네비매쉬 밖으로 나가는 경우는 케어 가능하다!!
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

	/// 가장 가까운 Edge(벡터)의 가장 가까운 지점으로 옮긴다.
	m_Transform->SetPosition(_newPosition);
	AdjustAgentPositionY();

	return true;
}

/// <summary>
/// [Ver.3 미리 보관했었던, 마지막으로 위치해있던 Face에서 가장 가까운 Line 으로 계산해서 이동 ]
/// [충돌 판정, 방향계산도 2중으로 한다. ver.2 에서 한 단계 개선되었다!!]
/// 
/// Agent의 position(판정점)으로부터 가장 가까운 Face의 Edge 위의 점으로 이동한다.
/// 
/// Agent가 네비메쉬 위에서 이동하다가 제한지역으로 넘어가려고 할 때 방향을 조정하기 위해 사용
/// </summary>
/// <returns></returns>
bool NavMeshAgent::MoveToNearNavFaceLineVer3()
{
	// 애초에 시작 위치가 네비매쉬 영역을 벗어나면 m_LastOntNavMeshFace 가 nullptr 일 수 있다.
	// 이때는 가장 가까운 페이스로 우선 옮겨준다.
	if (m_LastOnNavMeshFace == nullptr)
	{
		MoveToNearNavFaceCenter();
	}

	/* Agent의 포지션 정점을 T로 칭한다.*/
	SimpleMath::Vector3 _T = m_Transform->m_Position;

	/// 이동할 position
	SimpleMath::Vector3 _newPosition = { 0.0f, _T.y, 0.0f };

	/// Ver3 추가사항 m_LastCycleEdgeVector;
	// 초기값인 Vector3::zero 상태라면 이동할 수 없다.
	if (m_LastCycleEdgeVector.Length() != 0.0f)
	{
		/// 1차 벡터 계산(1자 같은 옆으로 이어진 Face간의 이동을 막힘없이 하기 위함)
		// 방향성분만 남긴다.
		SimpleMath::Vector3 _norVec = m_LastCycleEdgeVector;
		_norVec.y = 0.0f;	// x, z 축평면에 대해 계산하기 위해 y값을 날린다.(대각선 벡터(계단)에서 오류를 피하기 위함)
		_norVec.Normalize();
		/*
			SimpleMath::Vector3 _norVec = {
			m_LastCycleEdgeVector.x / m_LastCycleEdgeVector.Length(),
			m_LastCycleEdgeVector.y / m_LastCycleEdgeVector.Length(),
			m_LastCycleEdgeVector.z / m_LastCycleEdgeVector.Length()
			}
		*/

		// 거리 값을 구한다.
		SimpleMath::Vector3 _HitVec = _T - m_StartPositionOfLastCycleEdge;	// 히트벡터는 현재 Agent의 위치를 기준으로 해야함!
		_HitVec.y = 0.0f;	// x, z 축평면에 대해 계산하기 위해 y값을 날린다.(대각선 벡터(계단)에서 오류를 피하기 위함)

		float _fDot = _norVec.Dot(_HitVec);

		_newPosition.x = _norVec.x * _fDot + m_StartPositionOfLastCycleEdge.x;
		_newPosition.z = _norVec.z * _fDot + m_StartPositionOfLastCycleEdge.z;

		/// 재계산된 벡터로 이동한다.
		m_Transform->SetPosition(_newPosition);
		AdjustAgentPositionY();

		/// Ver3 추가사항. 충돌 검사 2중 처리
		/// 1차 이동 후에 위치 판정
		IsOnNavMesh(m_Transform->m_Position);
		if (m_bOnNavMesh == true)
		{
			return true;
		}

		/// 1차 이동 후에 여전히 NavMesh 밖에 있을 때

		/// Agent가 Edge(변) 영역을 넘어가 있는경우 Edge 안으로 들어오게끔 보정한다.
		// Ver2 추가 사항
		if (_fDot < 0.0f)
		{
			_fDot = 0.0f;
		}

		if (_fDot > m_LastCycleEdgeVector.Length())
		{
			_fDot = m_LastCycleEdgeVector.Length();
		}

		// 방향(normalize) * 길이(dot) + 원점(가장 가까운 Edge의 시작 점)
		_newPosition.x = _norVec.x * _fDot + m_StartPositionOfLastCycleEdge.x;
		_newPosition.z = _norVec.z * _fDot + m_StartPositionOfLastCycleEdge.z;

		/// 재계산된 벡터로 이동한다.
		m_Transform->SetPosition(_newPosition);
		AdjustAgentPositionY();

		/// 2차 이동 후에 위치 판정
		IsOnNavMesh(m_Transform->m_Position);
		if (m_bOnNavMesh == true)
		{
			return true;
		}
	}

	// 실패!
	return false;
}

/// <summary>
/// Agent의 position(판정점)으로부터 가장 가까운 정점(Vertex)로 이동한다.
/// 
/// (현재 버전의 충돌로직에서는 사용하지 않는중..)
/// </summary>
/// <returns></returns>
bool NavMeshAgent::MoveToNearVertex()
{
	/// 검사할 네비메쉬가 없을 때..
	if (s_NavMesh == nullptr)
	{
		return false;
	}

	// Agent의 포지션
	SimpleMath::Vector3 _agentPosition = m_Transform->m_Position;

	// 이동할 Vertex
	SimpleMath::Vector3 _nearVertex = s_NavMesh->GetNeviMeshFace(0)->m_Vertex_Arr[0];
	float _minDistance = SimpleMath::Vector3::Distance(_nearVertex, _agentPosition);

	/// NavMeshFace 들을 순회하면서 검사
	for (size_t i = 0; i < s_NavMesh->GetIndexSize(); i++)
	{
		// 검사할 부분 Face(Triangle) 
		std::shared_ptr<NaviMeshFace> _face = s_NavMesh->GetNeviMeshFace(i);

		// agent와 face[i]의 각 vertex의 거리를 구한다.
		float _distance0 = SimpleMath::Vector3::Distance(_face->m_Vertex_Arr[0], _agentPosition);
		float _distance1 = SimpleMath::Vector3::Distance(_face->m_Vertex_Arr[1], _agentPosition);
		float _distance2 = SimpleMath::Vector3::Distance(_face->m_Vertex_Arr[2], _agentPosition);

		/// 기존 최소값과 비교
		// 0 번 버텍스와의 거리 비교
		if (_distance0 < _minDistance)
		{
			// 이전 face보다 더 가깝다면 min 갱신
			_minDistance = _distance0;
			_nearVertex = _face->m_Vertex_Arr[0];
		}

		// 1 번 버텍스와의 거리 비교
		if (_distance1 < _minDistance)
		{
			// 이전 face보다 더 가깝다면 min 갱신
			_minDistance = _distance1;
			_nearVertex = _face->m_Vertex_Arr[1];
		}

		// 2 번 버텍스와의 거리 비교
		if (_distance2 < _minDistance)
		{
			// 이전 face보다 더 가깝다면 min 갱신
			_minDistance = _distance2;
			_nearVertex = _face->m_Vertex_Arr[2];
		}
	}

	// 가장 가까운 Vertex의 위치로 Agent 포지션 이동
	m_Transform->SetPosition(_nearVertex);
	AdjustAgentPositionY();	// y축 보정

	return true;
}

/// <summary>
/// Agent의 충돌 처리를 1차적으로 한 후에 여전히 NavMesh 영역 바깥에 있는 경우에
/// (충돌처리의 대상) LastOnNavFace를 같은 정점을 공유하는 인접 Face로 변경시키는 함수
/// 
/// (MoveToNearNavFaceLineVer3() 의 구현으로 현재 버전의 충돌로직에서는 사용하고 있지 않다..)
/// </summary>
/// <param name="vertex">기준을 잡을 가장 가까운 Face의 정점</param>
/// <returns></returns>
bool NavMeshAgent::ChangeLastOnNavMeshFaceToAdjacentNavFace()
{
	/// 기준을 잡을 가장 가까운 정점을 찾는다.
	SimpleMath::Vector3 _nearVertex = SimpleMath::Vector3::Zero;

	SimpleMath::Vector3 _vertexA = m_LastOnNavMeshFace->m_Vertex_Arr[0];
	SimpleMath::Vector3 _vertexB = m_LastOnNavMeshFace->m_Vertex_Arr[1];
	SimpleMath::Vector3 _vertexC = m_LastOnNavMeshFace->m_Vertex_Arr[2];

	// 각 정점과의 거리 계산
	unsigned int _DistanceA = SimpleMath::Vector3::Distance(m_Transform->m_Position, _vertexA);
	unsigned int _DistanceB = SimpleMath::Vector3::Distance(m_Transform->m_Position, _vertexB);
	unsigned int _DistanceC = SimpleMath::Vector3::Distance(m_Transform->m_Position, _vertexC);

	// 정점 A가 가장 가깝다
	if (_DistanceA < _DistanceB &&
		_DistanceA < _DistanceC)
	{
		_nearVertex = _vertexA;
	}

	else
	{
		// 정점 B가 가장 가깝다
		if (_DistanceB < _DistanceC)
		{
			_nearVertex = _vertexB;
		}

		// 정점 C가 가장 가깝다
		else
		{
			_nearVertex = _vertexC;
		}
	}

	// Face들을 순회하면서 찾는다.
	for (size_t i = 0; i < s_NavMesh->GetIndexSize(); i++)
	{
		std::shared_ptr<NaviMeshFace> _face = s_NavMesh->GetNeviMeshFace(i);

		// 현재 설정된 LastNavFace(기존 NavFace)면 건너 띔.
		if (_face == m_LastOnNavMeshFace)
		{
			continue;
		}

		// Face의 정점 3개를 검사
		for (size_t j = 0; j < 3; j++)
		{
			// 같은 정점을 공유하는 Face를 찾음!
			if (_face->m_Vertex_Arr[j] == _nearVertex)
			{
				// 충돌처리에 사용할 LastOnNavFace를 인접한 Face로 변경
				m_LastOnNavMeshFace = _face;

				return true;
			}
		}
	}

	return false;
}

/// <summary>
/// 오브젝트 포인터를 받아서 목적지 설정
/// </summary>
/// <param name="object">설정할 오브젝트</param>
/// <returns>성공여부</returns>
bool NavMeshAgent::SetDestinationObj(GameObject* object)
{
	// 목적지 해제하려고 nullptr 입력한 경우
	if (object == nullptr)
	{
		m_Path.clear();

		m_DestinationObj = nullptr;
		//m_DestinationPos = nullptr;

		m_bAutoMoveMode = false;
	}

	// 목적지 대상 오브젝트가 전달된 경우
	else
	{
		/// 기존 Dest와 object가 다른 경우에 Path 갱신
		if (m_DestinationObj != object)
		{
			// 기존 경로 클리어
			m_Path.clear();

			// 목표 설정
			m_DestinationObj = object;
			m_DestinationPos = object->m_Transform->m_Position;

			m_bAutoMoveMode = true;
		}

	}

	return true;
}

/// <summary>
/// Vector3 pos로 목표지점 설정
/// </summary>
/// <param name="position">목표 위치</param>
/// <returns></returns>
_DLL bool NavMeshAgent::SetDestinationPos(Vector3 position)
{
	// Agent의 현재 위치로 세팅하면 자동 이동 중지
	if (position == m_Transform->m_Position)
	{
		m_bAutoMoveMode = false;

		return true;
	}

	/// 기존 DestPos와 psition가 다른 경우에 Path 갱신
	if (m_DestinationPos != position)
	{
		// 기존 경로 클리어
		m_Path.clear();

		// 목표 설정
		m_DestinationObj = nullptr;
		m_DestinationPos = position;

		m_bAutoMoveMode = true;
	}

	return true;
}

/// <summary>
/// Agent가 마지막으로 위치해 있던 Face 리턴
/// (NavMeshManager 에서 일괄적으로 Agent들이 위치한 Face를 렌더할 때 사용)
/// </summary>
/// <returns>마지막으로 위치해 있던 Face</returns>
std::shared_ptr<struct NaviMeshFace> NavMeshAgent::GetLastOnNavMeshFace() const
{
	return m_LastOnNavMeshFace;
}

/// <summary>
/// 디버그 모드 리턴
/// </summary>
/// <returns>현재 디버그 모드 값</returns>
_DLL bool NavMeshAgent::GetDebugMode() const
{
	return m_bDebugMode;
}

/// <summary>
/// 디버그모드 값 세팅
/// </summary>
/// <param name="val">세팅할 값</param>
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

	// 남은 경로의 첫 Node가 목적지(end) Node와 같다면
	// 목적지(end) Node만 렌더하고 리턴
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

	// 외곽선 그리기
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
/// Agent 가 Face 영역 안에 있을 때 pos.y 위치를 3개의 정점들의 높이(y)의 평균치로 조정한다.
/// </summary>
bool NavMeshAgent::AdjustAgentPositionY()
{
	// 현재 위치한 Face가 없는 경우 -> (NavMesh 영역을 벗어난 경우)
	if (m_LastOnNavMeshFace == nullptr)
	{
		return false;
	}

	// 현재 Face(삼각형)의 중심 y pos 를 구한다.
	float _CenterY = m_LastOnNavMeshFace->m_CenterPosition.y;

	// 콜라이더 영역만큼 보정할 오프셋 값
	// 0으로 하니까 조금 밑으로 내려가서 보정해줌..
	float _offset = 0.23f;

	// 피직스 액터가 있으면 콜라이더의 볼륨 만큼 보정해서 posY를 위치시킨다.
	if (m_MyPhysicsActor != nullptr)
	{
		_offset += m_MyPhysicsActor->GetBoundingOffset().Bottom;
	}

	/// 중심 y 좌표에 콜라이더 영역만큼 보정할 오프셋을 더해서 y축 이동
	m_Transform->SetPosition({ m_Transform->m_Position.x, _CenterY + _offset, m_Transform->m_Position.z });

	return true;
}

/// <summary>
/// Pos에서 가장 가까운 NavMeshFace를 찾는다.
/// Face의 [중점의 좌표]를 기준으로 찾는다.
/// </summary>
/// <param name="position">기준 위치</param>
/// <returns>가장 가까운 NavFace</returns>
std::shared_ptr<struct NaviMeshFace> NavMeshAgent::FindNearNavFace(SimpleMath::Vector3 position)
{
	/// 검사할 네비메쉬가 없을 때..
	if (s_NavMesh == nullptr)
	{
		return false;
	}

	// 리턴할 Near Face
	std::shared_ptr<NaviMeshFace> _nearface = s_NavMesh->GetNeviMeshFace(0);	// 첫 번째 Face로 초기화

	// y축 빼고 길이 계산
	float _minDistance = SimpleMath::Vector2::Distance(
		{ _nearface->m_CenterPosition.x, _nearface->m_CenterPosition.z },
		{ position.x, position.z }
	);

	/// NavMeshFace 들을 순회하면서 검사
	for (size_t i = 1; i < s_NavMesh->GetIndexSize(); i++)	// 0번은 위에서 초기 값으로 세팅 했으니 1번부터 비교.
	{
		// 검사할 부분 Face(Triangle) 
		std::shared_ptr<NaviMeshFace> _face = s_NavMesh->GetNeviMeshFace(i);

		// position 과 face[i] 의 거리를 구한다.(xz 축 평면 기준으로만 한다)
		SimpleMath::Vector2 _xzFaceVec = { _face->m_CenterPosition.x, _face->m_CenterPosition.z };
		SimpleMath::Vector2 _xzPosVec = { position.x, position.z };

		float _distance = SimpleMath::Vector2::Distance(_xzFaceVec, _xzPosVec);

		// 기존 값과 비교
		if (_distance < _minDistance)
		{
			// 이전 face보다 더 가깝다면 min 갱신
			_minDistance = _distance;

			_nearface = _face;
		}
	}

	// 가장 가까운 Face 리턴
	return _nearface;
}


/// <summary>
/// 유저(클라이언트단 스크립트)가 Agent의 목적지를 설정했을 때
/// 해당 목적지의 위치로 경로탐색에 사용할 NavMesh 상에서의 실제 목적지(골 지점)을 계산한다.
/// 
/// [Path Find 할 때 사용하는 위치 계산]
/// </summary>
/// <returns>성공 여부</returns>
bool NavMeshAgent::CalcDestinationOnNavMesh()
{
	// 목적지 오브젝트 설정이 안되어  position 사용
	if (m_DestinationObj == nullptr)
	{
		m_DestinationPosOnNavMesh = m_DestinationPos;
	}

	else
	{
		// 목적지 Obj가 있을 때 에만 Object의 위치를 NavMesh 위의 영역(지점)으로 매핑한다.
		// 목적지 이동해도 동기화
		m_DestinationPosOnNavMesh = m_DestinationObj->m_Transform->m_Position;
	}

	/// 목적지가 NavMesh 위에 있는지 검사
	std::shared_ptr<NaviMeshFace> _face = IsOnNavMesh(m_DestinationPosOnNavMesh);
	if (_face != nullptr)
	{
		// y축은 제외하고 xz 평면기준으로 네비메쉬 위에 있다면 
		// 해당 y축 좌표만 해당 Face의 높의의 중간(CenterPos.y)로 보정한다.
		m_DestinationPosOnNavMesh.y = _face->m_CenterPosition.y;

		return true;
	}

	// else
	/// 목적지에서 가장 가까운 Face를 찾는다.
	std::shared_ptr<NaviMeshFace> _nearFace = FindNearNavFace(m_DestinationPosOnNavMesh);

	/// 목적지가 NavMesh 바깥에 있다면 가장 가까운 지점을 계산해서 매핑한다.
	/* 삼각형의 각 정점을 A, B, C 로 목적지의 포지션 정점을 T로 칭한다.*/
	SimpleMath::Vector3 _T = m_DestinationPosOnNavMesh;

	/// 삼각형 정점
	SimpleMath::Vector3 _A = _nearFace->m_Vertex_Arr[0];
	SimpleMath::Vector3 _B = _nearFace->m_Vertex_Arr[1];
	SimpleMath::Vector3 _C = _nearFace->m_Vertex_Arr[2];

	/// 삼각형 사이클 벡터(변, edge)
	SimpleMath::Vector3 _TriVecAB = _nearFace->m_CycleEdgeVector_Arr[0];	// AB
	SimpleMath::Vector3 _TriVecBC = _nearFace->m_CycleEdgeVector_Arr[1];	// BC
	SimpleMath::Vector3 _TriVecCA = _nearFace->m_CycleEdgeVector_Arr[2];	// CA

	/// 삼각형 정점에서 Agent로 향한 벡터
	SimpleMath::Vector3 _HitVecAT = _T - _A;	// AT
	SimpleMath::Vector3 _HitVecBT = _T - _B;	// BT
	SimpleMath::Vector3 _HitVecCT = _T - _C;	// CT

	// 외적값.y -> +,- 부호로 Face의 유향벡터를 기준으로 좌측에 있는지 우측에있는지 판별 가능
	float _fCross0 = (_TriVecAB.z * _HitVecAT.x) - (_TriVecAB.x * _HitVecAT.z);
	float _fCross1 = (_TriVecBC.z * _HitVecBT.x) - (_TriVecBC.x * _HitVecBT.z);
	float _fCross2 = (_TriVecCA.z * _HitVecCT.x) - (_TriVecCA.x * _HitVecCT.z);
	/////////////////////////////////////////////////////////////////////////////////////////////////

	/// 매핑할 position
	SimpleMath::Vector3 _newPosition = { 0.0f, _nearFace->m_CenterPosition.y, 0.0f };	// y축 좌표는 Face의 중심 높이로 설정

	/// Face의 사이클 벡터(Edge들) 단위화(중요)
	// 방향성분만 남긴다.
	SimpleMath::Vector3 _norVecAB = { _TriVecAB.x / _TriVecAB.Length(), _TriVecAB.y / _TriVecAB.Length(), _TriVecAB.z / _TriVecAB.Length() };
	SimpleMath::Vector3 _norVecBC = { _TriVecBC.x / _TriVecBC.Length(), _TriVecBC.y / _TriVecBC.Length(), _TriVecBC.z / _TriVecBC.Length() };
	SimpleMath::Vector3 _norVecCA = { _TriVecCA.x / _TriVecCA.Length(), _TriVecCA.y / _TriVecCA.Length(), _TriVecCA.z / _TriVecCA.Length() };

	/// 절대값 변환해서 +의 길이를 만듦
	_fCross0 = std::fabsf(_fCross0);
	_fCross1 = std::fabsf(_fCross1);
	_fCross2 = std::fabsf(_fCross2);

	/// 가장 가까운 Edge(변)을 찾는다.
	// Edge 0(AB)에 가장 가깝다.
	if ((_fCross0 <= _fCross1) &&
		(_fCross0 <= _fCross2))
	{
		float _fDot = _norVecAB.Dot(_HitVecAT);

		/// 목적지가 Edge(변) 영역을 넘어가 있는경우 Edge 안으로 들어오게끔 보정한다.
		if (_fDot < 0.0f)
		{
			_fDot = 0.0f;
		}

		if (_fDot > _TriVecAB.Length())
		{
			_fDot = _TriVecAB.Length();
		}

		// 방향(normalize) * 길이(dot) + 원점(가장 가까운 Edge의 시작 점)
		_newPosition.x = _norVecAB.x * _fDot + _A.x;
		_newPosition.z = _norVecAB.z * _fDot + _A.z;
	}

	else
	{
		// Edge 1(BC)에 가장 가깝다.
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
			// Edge 2(CA)에 가장 가깝다.
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

	/// 가장 가까운 Edge(벡터)의 가장 가까운 지점으로 옮긴다.
	m_DestinationPosOnNavMesh = _newPosition;

	return true;
}

/// <summary>
/// Agent가 현재 설정된 경로 위에 있는지 검사
/// </summary>
/// <returns></returns>
bool NavMeshAgent::IsAgentOnPath()
{
	// 경로의 Node 순회
	for (const auto& node : m_Path)
	{
		// Agent가 있는 Face와 같으면 경로 위에 있는 것으로 판단
		if (node->m_Face == m_LastOnNavMeshFace)
		{
			return true;
		}
	}

	// 모든 Node의 Face 중에서 Agent가 현재 있는 LastOnFace가 없으면 경로를 이탈한 것으로 간주
	return false;
}

/// <summary>
/// A* 알고리즘으로 길찾기(경로 탐색) 함수
/// 
/// (Open List 를 std::list 로 구현)
/// </summary>
/// <param name="startFace">출발지 노드</param>
/// <param name="endFace">도착지 노드</param>
/// <returns>경로</returns>
std::list<std::shared_ptr<AStarNode>> NavMeshAgent::PathFindAStar(std::shared_ptr<NaviMeshFace> startFace, std::shared_ptr<NaviMeshFace> endFace)
{
	std::list<std::shared_ptr<AStarNode>> _nodePath;			// 리턴할 경로(AStarNode list)

	if (m_LastOnNavMeshFace == nullptr
		|| m_DestinationFace == nullptr
		|| startFace == nullptr
		|| endFace == nullptr)
	{
		return _nodePath;
	}

	// Start, End Node 생성(End 부터)
	std::shared_ptr<AStarNode> _endNode = std::make_unique<AStarNode>(endFace, nullptr, nullptr);
	std::shared_ptr<AStarNode> _startNode = std::make_unique<AStarNode>(startFace, nullptr, _endNode);

	std::list<std::shared_ptr<AStarNode>> _openList;			// 열린 목록
	std::list<std::shared_ptr<AStarNode>> _closedList;			// 닫힌 목록
	std::shared_ptr<AStarNode> _curNode;						// 현재 노드

	// Open List에 Start Node 넣고 시작
	_openList.emplace_back(_startNode);

	// Open List에 검사할 Node가 남아있으면 계속 경로 탐색
	//while (_openList.size() > 0)
	while (!_openList.empty())
	{
		// CurNode 초기화
		_curNode = _openList.front();

		// Open List에서 가장 F(n) 비용이 적은 Node를 선택한다.
		// F(n) 이 같다면 목표까지의 가중치가 적은 쪽으로( H(n)이 적은 쪽 )
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

		// 선택된 비용이 가장 적은 node를 Open List에서 Closed List로 이동
		_openList.remove(_curNode);
		_closedList.emplace_back(_curNode);

		//CA_TRACE("[PathFind] Open = %d / closed = %d", _openList.size(), _closedList.size());

		// Goal Check
		if (_curNode->m_Index == _endNode->m_Index)
		{
			/// 경로 만들기
			// 역순으로 도착(end) 노드부터 시작(start)노드까지 _nodePath 에 추가
			std::shared_ptr<AStarNode> _targetCurNode = _curNode;
			while (_targetCurNode != _startNode)
			{
				_nodePath.emplace_back(_targetCurNode);
				_targetCurNode = _targetCurNode->m_Parent;
			}
			_nodePath.push_back(_startNode);

			// 역순으로 만든 path를 역으로해서 (start -> end) 경로를 만든다!
			_nodePath.reverse();

			// 경로 리턴
			return _nodePath;
		}

		// 인접 Face 들을 Open List 에 추가
		for (size_t i = 0; i < _curNode->m_Face->m_AdjoinFace_V.size(); i++)
		{
			std::shared_ptr<NaviMeshFace> _face = _curNode->m_Face->m_AdjoinFace_V.at(i);
			AddOpenList(_openList, _closedList, _face, _curNode, _endNode);
		}

	}// end of while -> 목적지 end(dest) Node 까지의 경로를 찾을 수 없음!

	// (빈) 경로 리턴
	return _nodePath;	// start와 end가 같은 face에 있는 경우 or 
						// 길이 막혔거나 등의 이유로 연결된 Path를 찾을 수 없는 경우
}

/// <summary>
/// A* 알고리즘으로 길찾기(경로 탐색) 함수
/// 
/// (Open List 를 std::priority_queue 로 구현)
/// </summary>
/// <param name="startFace"></param>
/// <param name="endFace"></param>
/// <returns></returns>
std::list<std::shared_ptr<AStarNode>> NavMeshAgent::PathFindAStar2(std::shared_ptr<struct NaviMeshFace> startFace, std::shared_ptr<struct NaviMeshFace> endFace)
{
	std::list<std::shared_ptr<AStarNode>> _nodePath;			// 리턴할 경로(AStarNode list)

	if (m_LastOnNavMeshFace == nullptr
		|| m_DestinationFace == nullptr
		|| startFace == nullptr
		|| endFace == nullptr)
	{
		return _nodePath;
	}

	// Start, End Node 생성(End 부터)
	std::shared_ptr<AStarNode> _endNode = std::make_unique<AStarNode>(endFace, nullptr, nullptr);
	std::shared_ptr<AStarNode> _startNode = std::make_unique<AStarNode>(startFace, nullptr, _endNode);

	std::priority_queue<
		std::shared_ptr<AStarNode>,
		std::vector<std::shared_ptr<AStarNode>>,
		AstarNodeCmp> _openList;			// 열린 목록

	std::list<std::shared_ptr<AStarNode>> _closedList;			// 닫힌 목록
	std::shared_ptr<AStarNode> _curNode;						// 현재 노드

	// Open List에 Start Node 넣고 시작
	_openList.push(_startNode);

	// Open List에 검사할 Node가 남아있으면 계속 경로 탐색
	while (_openList.empty() == false)
	{
		// CurNode 초기화(비용이 가장 작은 노드로)
		_curNode = _openList.top();

		// 선택된 비용이 가장 적은 node를 Open List에서 Closed List로 이동
		_openList.pop();
		_closedList.emplace_back(_curNode);

#ifdef _DEBUG
		//CA_TRACE("[PathFind] Open = %d / closed = %d", _openList.size(), _closedList.size());
#endif

		// Goal Check
		if (_curNode->m_Index == _endNode->m_Index)
		{
			/// 경로 만들기
			// 역순으로 도착(end) 노드부터 시작(start)노드까지 _nodePath 에 추가
			std::shared_ptr<AStarNode> _targetCurNode = _curNode;
			while (_targetCurNode != _startNode)
			{
				_nodePath.emplace_back(_targetCurNode);
				_targetCurNode = _targetCurNode->m_Parent;
			}
			_nodePath.push_back(_startNode);

			// 역순으로 만든 path를 역으로해서 (start -> end) 경로를 만든다!
			_nodePath.reverse();

			// 경로 리턴
			return _nodePath;
		}

		// 인접 Face 들을 Open List 에 추가
		for (size_t i = 0; i < _curNode->m_Face->m_AdjoinFace_V.size(); i++)
		{
			std::shared_ptr<NaviMeshFace> _face = _curNode->m_Face->m_AdjoinFace_V.at(i);
			//AddOpenList(_openList, _closedList, _face, _curNode, _endNode);
			AddOpenList2(_openList, _closedList, _face, _curNode, _endNode);
		}

	}// end of while -> 목적지 end(dest) Node 까지의 경로를 찾을 수 없음!

	// (빈) 경로 리턴
	return _nodePath;	// start와 end가 같은 face에 있는 경우 or 
						// 길이 막혔거나 등의 이유로 연결된 Path를 찾을 수 없는 경우
}


/// <summary>
/// Path Find 중 Open List에 Node를 추가한다.
/// </summary>
/// <param name="openList">Path Find에 사용중인 Open List</param>
/// <param name="closedList">Path Find에 사용중인 Closed List</param>
/// <param name="face">새로 추가할 Node의 Face</param>
/// <param name="parentNode">부모(이전)노드</param>
/// <param name="endNode">끝(도착)노드</param>
void NavMeshAgent::AddOpenList(std::list<std::shared_ptr<AStarNode>>& openList, std::list<std::shared_ptr<AStarNode>>& closedList, std::shared_ptr<NaviMeshFace> face, std::shared_ptr<AStarNode> parentNode, std::shared_ptr<AStarNode> endNode)
{
	// Face에 해당하는 Node가 이미 Closed List에 추가되어있는지 검사
	// (이미 검사를 한 Node는 또 검사할 필요가 없음)
	for (const auto& node : closedList)
	{
		if (node->m_Index == face->m_Index)
		{
			// Closed List 에 이미 추가되어 있으면 Open List에 또 추가하지 않음.
			return;
		}
	}

	// Closed List 에 없다면

	// 추가할 이웃(자식)노드 생성
	std::shared_ptr<AStarNode> neighborNode = std::make_shared<AStarNode>(face, parentNode, endNode);

	// Open List에 있는지 검사
	// ( Open List 중복체크를 안하면 Open List에 같은 Node가 계속 쌓이게 되면서 계속 늘어남..)
	for (const auto& node : openList)
	{
		if (node->m_Index == neighborNode->m_Index)
		{
			// Open List 에 이미 추가되어 있으면 또 추가하지 않음.
			return;
		}
	}

	// Open List 에 추가
	openList.emplace_back(neighborNode);
}

void NavMeshAgent::AddOpenList2(std::priority_queue<std::shared_ptr<AStarNode>, std::vector<std::shared_ptr<AStarNode>>, AstarNodeCmp>& openList, std::list<std::shared_ptr<AStarNode>>& closedList, std::shared_ptr<struct NaviMeshFace> face, std::shared_ptr<class AStarNode> parentNode, std::shared_ptr<AStarNode> endNode)
{
	/// Face에 해당하는 Node가 이미 Closed List에 추가되어있는지 검사
	// (이미 검사를 한 Node는 또 검사할 필요가 없음)
	for (const auto& node : closedList)
	{
		if (node->m_Index == face->m_Index)
		{
			// Closed List 에 이미 추가되어 있으면 Open List에 또 추가하지 않음.
			return;
		}
	}

	// Closed List 에 없다면

	// 추가할 이웃(자식)노드 생성
	std::shared_ptr<AStarNode> neighborNode = std::make_shared<AStarNode>(face, parentNode, endNode);

	/// Open List에 있는지 검사
	// ( Open List 중복체크를 안하면 Open List에 같은 Node가 계속 쌓이게 되면서 계속 늘어남..)

	// 순회용 임시 Open List
	std::priority_queue<std::shared_ptr<AStarNode>,
		std::vector<std::shared_ptr<AStarNode>>,
		AstarNodeCmp> _copyOpenList_L = openList;

	// 값 비교용 임시 Open List
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
			// Open List 에 이미 추가되어 있으면 또 추가하지 않음.
			return;
		}
	}

	// Open List 에 추가
	openList.push(neighborNode);
}

/// <summary>
/// [Ver 1] - 다음 Node의 Face의 중점을 향해 이동한다.
/// 
/// 목적지가 설정되어 있으면 길(경로)를 따라 자동으로 이동하는 함수
/// </summary>
void NavMeshAgent::MoveToDestinationVer1()
{
	float _dist = SimpleMath::Vector3::Distance(m_Transform->m_Position, m_DestinationPosOnNavMesh);
	if (_dist < 0.1f)
	{
		return;
	}

	// 이동, 회전(바라볼 Look 방향) 벡터 초기화
	SimpleMath::Vector3 _moveDir = SimpleMath::Vector3::Zero;
	SimpleMath::Vector3 _lookdir = SimpleMath::Vector3::Zero;

	// 아직 End Node(도착 Face)에 도착하기 전이면
	// 다음 노드의 중점을 향해 이동
	if (m_Path.size() > 0 && m_LastOnNavMeshFace != m_Path.back()->m_Face)
	{
		// 현재 Node와 다음 Node를 찾는다.
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

		// 회전방향 계산
		_lookdir = _nextWayPoint;

		// 이동방향 계산
		_moveDir = _nextWayPoint - m_Transform->m_Position;
		_moveDir.Normalize();
	}

	// End Node(도착 Face)에 도착했으면 destination 을 향해 방향 설정
	else
	{
		// 회전방향 계산
		_lookdir = m_DestinationPosOnNavMesh;

		// 이동방향 계산
		_moveDir = m_DestinationPosOnNavMesh - m_Transform->m_Position;
		_moveDir.Normalize();
	}

	// 방향에 스피드를 곱하고 deltaTime 을 보정
	_moveDir *= m_MoveSpeed * DLLTime::DeltaTime();

	/// Agent 방향 회전
	m_Transform->LookAtYaw(_lookdir);

	/// Agent 이동
	// 피직스 액터가 있는 경우 피직스로 이동
	if (m_MyPhysicsActor != nullptr)
	{
		MoveByPhysX(m_MyPhysicsActor, _moveDir);
	}

	// 피직스 액터가 없으면 트랜스폼 이동
	else
	{
		m_Transform->SetPosition(m_Transform->m_Position + _moveDir);
	}
}


/// <summary>
/// [Ver 2] - 미구현
/// 
/// 목적지가 설정되어 있으면 길(경로)를 따라 자동으로 이동하는 함수
/// </summary>
void NavMeshAgent::MoveToDestinationVer2()
{
}

/// <summary>
/// Agent가 PhysicsActor 컴포넌트가 있을 때 물리이동을 하는 함수
/// </summary>
/// <param name="physActor">PhyscisActor Component</param>
/// <param name="dir">이동 벡터</param>
void NavMeshAgent::MoveByPhysX(PhysicsActor* physActor, SimpleMath::Vector3& dir)
{
	// 키네마틱이 설정되어있으면 물리이동 불가능하므로
	// 트랜스폼 이동
	if (physActor->IsKinematic() == true)
	{
		/*
		* 일반적으로 생각하는 수치로 이동속도를 세팅하면
		* 너무 빨리 움직여서 값을 감소시켜줄 offset 값
		*/
		m_Transform->SetPosition(m_Transform->m_Position + dir);
	}

	// 키네마틱 false 일 땐 물리 이동
	else
	{
		physActor->SetVelocity(dir);
	}
}

/// <summary>
/// Debug용 함수
/// LastOnNavMeshFace 의 인덱스와 인접한 Adjoin Face의 인덱스를 직접 출력창에 출력한다.
/// </summary>
void NavMeshAgent::PrintLastOnFaceInfo() const
{
	int adjoins[3] = { -1, -1, -1 };	// -1 은 매직넘버(인접 Face가 없다는 뜻)

	for (size_t i = 0; i < m_LastOnNavMeshFace->m_AdjoinFace_V.size(); i++)
	{
		adjoins[i] = m_LastOnNavMeshFace->m_AdjoinFace_V.at(i)->m_Index;
	}
	CA_TRACE("[NavMeshAgent] now Face %d / Adjoin %d, %d, %d", m_LastOnNavMeshFace->m_Index, adjoins[0], adjoins[1], adjoins[2]);
}

