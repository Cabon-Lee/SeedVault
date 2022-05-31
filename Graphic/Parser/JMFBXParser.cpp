#include "JMFBXParser.h"

JMFBXParser::JMFBXParser()
{
}

JMFBXParser::~JMFBXParser()
{
}

void JMFBXParser::Initalize()
{
	// FBX SDK Manager ����..
	m_pManager = FbxManager::Create();

	// Scene ����..
	m_pScene = FbxScene::Create(m_pManager, "My Scene");

	// IOSettings ��ü ���� �� ����
	FbxIOSettings* ios = FbxIOSettings::Create(m_pManager, IOSROOT);
	m_pManager->SetIOSettings(ios);

	// FbxImporter ��ü ����
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

	FbxAxisSystem sceneAxisSystem = m_pScene->GetGlobalSettings().GetAxisSystem(); // �� ���� ��ǥ���� �ٲ۴�. 
	//FbxAxisSystem::MayaYUp.ConvertScene(m_pScene); // �� ������ �ﰢ��ȭ �� �� �ִ� ��� ��带 �ﰢ��ȭ ��Ų��. 
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

	// �ʿ� �Ž� ����
	m_Mesh_M.insert({ fileName.Buffer() , m_pOneMesh });

	SceneSetting();
	// Scene���� RootNode ��������..
	fbxsdk::FbxNode* _pRootNode = m_pScene->GetRootNode();

	// �Ž� �ε�
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
		// �ڽ� ��尡 �ε��̵Ǿ��ִٴ� ����� �� �� �ִ�
		LoadNode(node->GetChild(i));
	}
}

void JMFBXParser::LoadMesh(fbxsdk::FbxNode* node)
{
	FbxMesh* _mesh = node->GetMesh();

	// ���ؽ� ���� ����
	ProcessControlPoints(_mesh);

	// �ε��� ���� ����
	unsigned int _triCount = _mesh->GetPolygonCount(); // �޽��� �ﰢ�� ������ �����´�. 
	unsigned int _vertexCount = 0; // ������ ���� 
	
	for(unsigned int i = 0; i < _triCount; ++i) // �ﰢ���� ���� 
	{ 
		m_pOneMesh->m_MeshFaces.push_back(new ParserData::Face);

		for(unsigned int j = 0; j < 3; ++j) // �ﰢ���� �� ���� �������� ���� 
		{ 
			int controlPointIndex = _mesh->GetPolygonVertex(i, j); // ������ �ε����� �����´�. 
			//vec3& position = positions[controlPointIndex]; // ���� ������ ���� ��ġ 
			
			// Face Vertex Index Data
			m_pOneMesh->m_MeshFaces[i]->m_VertexIndex[j] = controlPointIndex;
			
			_vertexCount++; // �ε����� ����++ 
		} 
	}

	//�ε��� ����
	m_pOneMesh->m_Mesh_NumIndex = _vertexCount;
}

// stack �޸�
// ���� �޸𸮴� ���̶� �ٸ���.
// ���� ���� : ������ ��������, ���� �����Ҵ�
// �Լ��� �ݸ��Ǵ� �������� �Ķ���ͷ� ���� ���ڴ� ����������.
// �׷� �� ���������� ���ÿ� ���δ�.
// �� �Լ��� ���� �޸𸮰� �����ִ� ������ ����� ������ �����ϴ�.

void JMFBXParser::ProcessControlPoints(fbxsdk::FbxMesh* mesh)
{
	unsigned int _count = mesh->GetControlPointsCount();

	//���ؽ��� ���� ���� ����
	m_pOneMesh->m_Mesh_NumVertex = _count;

	for (unsigned int i = 0; i < _count; ++i)
	{
		ParserData::Vertex* _position = new ParserData::Vertex;
		// �������� ���������� GetControlPointAt(int index) ��� �Լ��� �̿��Ѵ�. 

		_position->m_Pos.x = static_cast<float>(mesh->GetControlPointAt(i).mData[0]); // x��ǥ 
		_position->m_Pos.y = static_cast<float>(mesh->GetControlPointAt(i).mData[1]); // y��ǥ 
		_position->m_Pos.z = static_cast<float>(mesh->GetControlPointAt(i).mData[2]); // z��ǥ 

		m_pOneMesh->m_MeshVertices.push_back(_position);

		// ���⼭�� ���� ���� ������?
	}
}
