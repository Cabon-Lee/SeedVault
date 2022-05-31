#include "JMFBXParser.h"

JMFBXParser::JMFBXParser()
{
}

JMFBXParser::~JMFBXParser()
{
}

void JMFBXParser::Initalize()
{
	// FBX SDK Manager 생성..
	m_pManager = FbxManager::Create();

	// Scene 생성..
	m_pScene = FbxScene::Create(m_pManager, "My Scene");

	// IOSettings 객체 생성 및 설정
	FbxIOSettings* ios = FbxIOSettings::Create(m_pManager, IOSROOT);
	m_pManager->SetIOSettings(ios);

	// FbxImporter 객체 생성
	m_pImporter = FbxImporter::Create(m_pManager, "");
	
}

void JMFBXParser::SceneSetting()
{
	bool status = m_pImporter->Initialize(m_fbxFileName, -1, m_pManager->GetIOSettings());
	m_pImporter->Import(m_pScene);

	if (!status) 
	{ 
		printf("Call to FbxImporter::Initialize() failed.\n"); 
		printf("Error returned: %s\n\n", m_pImporter->GetStatus().GetErrorString());
		exit(-1); 
	}

	FbxAxisSystem sceneAxisSystem = m_pScene->GetGlobalSettings().GetAxisSystem(); // 씬 내의 좌표축을 바꾼다. 
	//FbxAxisSystem::MayaYUp.ConvertScene(m_pScene); // 씬 내에서 삼각형화 할 수 있는 모든 노드를 삼각형화 시킨다. 
	FbxGeometryConverter geometryConverter(m_pManager);
	geometryConverter.Triangulate(m_pScene, true);

}

void JMFBXParser::Destroy()
{
}

void JMFBXParser::LoadScene(fbxsdk::FbxString fileName)
{
	m_fbxFileName = fileName;

	m_pOneMesh = new ParserData::Mesh();

	// 맵에 매쉬 저장
	m_Mesh_M.insert({ fileName.Buffer() , m_pOneMesh });

	SceneSetting();
	// Scene에서 RootNode 가져오기..
	fbxsdk::FbxNode* _pRootNode = m_pScene->GetRootNode();

	// 매쉬 로드
	LoadNode(_pRootNode);

}

void JMFBXParser::LoadNode(fbxsdk::FbxNode* node)
{
	FbxNodeAttribute* _pNodeAtrribute = node->GetNodeAttribute();

	if (_pNodeAtrribute != nullptr)
	{
		switch (_pNodeAtrribute->GetAttributeType())
		{
		
		case FbxNodeAttribute::eMesh:
			LoadMesh(node);
			break;

		case FbxNodeAttribute::eSkeleton:
			//LoadMesh(node);
			break;
		case FbxNodeAttribute::eLight:

		
		default:
			break;
		}
	}

	const int childCount = node->GetChildCount();
	for (unsigned int i = 0; i < childCount; i++)
	{
		// 자식 노드가 인덱싱되어있다는 사실을 알 수 있다
		LoadNode(node->GetChild(i));
	}
}

void JMFBXParser::LoadMesh(fbxsdk::FbxNode* node)
{
	FbxMesh* _mesh = node->GetMesh();

	// 버텍스 정보 저장
	ProcessControlPoints(_mesh);

	// 인덱스 정보 저장
	unsigned int _triCount = _mesh->GetPolygonCount(); // 메쉬의 삼각형 개수를 가져온다. 
	unsigned int _vertexCount = 0; // 정점의 개수 
	
	for(unsigned int i = 0; i < _triCount; ++i) // 삼각형의 개수 
	{ 
		m_pOneMesh->m_MeshFaces.push_back(new ParserData::Face);

		for(unsigned int j = 0; j < 3; ++j) // 삼각형은 세 개의 정점으로 구성 
		{ 
			int controlPointIndex = _mesh->GetPolygonVertex(i, j); // 제어점 인덱스를 가져온다. 
			//vec3& position = positions[controlPointIndex]; // 현재 정점에 대한 위치 
			
			// Face Vertex Index Data
			m_pOneMesh->m_MeshFaces[i]->m_VertexIndex[j] = controlPointIndex;
			
			_vertexCount++; // 인덱스의 개수++ 
		} 
	}

	//인덱스 갯수
	m_pOneMesh->m_Mesh_NumIndex = _vertexCount;
}

// stack 메모리
// 스택 메모리는 힙이랑 다르다.
// 진미 생각 : 스택은 지역변수, 힙은 동적할당
// 함수가 콜링되는 시점에서 파라미터로 받은 인자는 지역변수다.
// 그럼 그 지역변수는 스택에 쌓인다.
// 그 함수의 스택 메모리가 열려있는 동안은 연산과 접근이 가능하다.

void JMFBXParser::ProcessControlPoints(fbxsdk::FbxMesh* mesh)
{
	unsigned int _count = mesh->GetControlPointsCount();

	//버텍스의 갯수 정보 저장
	m_pOneMesh->m_Mesh_NumVertex = _count;

	for (unsigned int i = 0; i < _count; ++i)
	{
		ParserData::Vertex* _position = new ParserData::Vertex;
		// 제어점을 가져오려면 GetControlPointAt(int index) 멤버 함수를 이용한다. 

		_position->m_Pos.x = static_cast<float>(mesh->GetControlPointAt(i).mData[0]); // x좌표 
		_position->m_Pos.y = static_cast<float>(mesh->GetControlPointAt(i).mData[1]); // y좌표 
		_position->m_Pos.z = static_cast<float>(mesh->GetControlPointAt(i).mData[2]); // z좌표 

		m_pOneMesh->m_MeshVertices.push_back(_position);

		// 여기서는 뭐가 문제 였을까?
	}
}
