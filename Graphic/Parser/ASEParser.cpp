#include "ASEParser.h"


CASEParser::CASEParser()
{
	m_materialcount = 0;
	m_ParsingMode = eNone;
}

CASEParser::~CASEParser()
{
	delete m_lexer;
}

bool CASEParser::Init()
{
	m_lexer = new ASE::CASELexer;

	return TRUE;
}
//---------------------------------------------------------------------------------------------------
// �ε��Ѵ�.
// �̰��� �����ٸ� ������ ���������� ��� �����͸� �о �� �־�� �Ѵ�.
//
// Parsing�� ����:
// �׻� ������ �������� parsing�̶�� ���� �����Ͱ� ���� �԰ݿ� �°� �� �ִٴ� ���� ������ �Ѵ�.
// ������, ���� ���ο� ������ �ٲ�� �� �ִ°��� �����ϴٴ��� �ϴ� ��Ģ�� �����
// �˻��ϴ� ��ƾ�� ���������� ����. �ϴ��� ������� ���� �Ծ����� �� �ִٴ°��� ��������.
// -> �׷���, ��� ȣ���� �ϴ� �ϳ��� ū �Լ��� �ذ�ô�.
// -> depth�� �������� �����ۿ� ���� ���� �ڵ带 �־�߰ڴ�
//---------------------------------------------------------------------------------------------------
bool CASEParser::Load(LPSTR p_File)
{
	/// 0) ������ �ε��Ѵ�.
	if (!m_lexer->Open(p_File))
	{
		TRACE("������ ���� �߿� ������ �߻��߽��ϴ�!");
		return FALSE;
	}

	/// 1) �ٷ��̾ƺ����豳���� �Ŀ�Ǯ�� ����Լ� �Ѱ��� ó�� ��!
	Parsing_DivergeRecursiveALL(0);

	return TRUE;
}

/// 1�� ��ȯ

// CScenedata ����
bool CASEParser::TranslateToD3DFormat_scene(Scenedata* pScene)
{
	// �� ����
	*pScene = this->m_scenedata;
	return TRUE;
}

/// �޽��� ����ȭ�� �� �ش�.
/// �븻��, �ؽ��� ��ǥ�� ���� ���ؽ��� �ø���, ��ø�Ǵ°��� �����ϰ�..
bool CASEParser::VertexOptimization(Mesh* pMesh)
{
	if (pMesh->m_MeshVertices.size() > 0)
	{
		for (unsigned int _ver = 0; _ver < pMesh->m_Mesh_NumVertex; _ver++) // vertex ����
		{
			/// �ϴ� load�� vertex���� optimized vertex vector�� ����
			Vertex* _pVertex = new Vertex();
			_pVertex->m_Pos = pMesh->m_MeshVertices[_ver]->m_Pos;
			_pVertex->m_Indices = pMesh->m_MeshVertices[_ver]->m_Indices;
			/// texture ���� ����
			//_pVertex->u = pMesh->m_MeshVertices[_ver]->u;
			//_pVertex->v = pMesh->m_MeshVertices[_ver]->v;

			pMesh->m_opt_vertex.push_back(_pVertex); /// ����ȭ�� vertex�� ��ƾ� ��
		}
	}
	else
	{
		return false;
	}

	/// Face�� ��� ���鼭 ���ؽ����� �븻���� ����
	for (unsigned int i = 0; i < pMesh->m_Mesh_NumFace; i++)
	{
		/// Face���� vertex index�� 3���� (3����)
		for (unsigned int j = 0; j < 3; j++)
		{
			// ���� vertex�� �������� �񱳸� �ǽ��ؾ���
			Vertex* _nowVertex = pMesh->m_opt_vertex[pMesh->m_MeshFaces[i]->m_VertexIndex[j]];

			/// texture�� ������ u, v�� �����Ѵ�
			if (pMesh->m_Mesh_NumTVertex != 0)
			{
				_nowVertex->u = pMesh->m_Mesh_TVertices[pMesh->m_MeshFaces[i]->m_TFace[j]]->m_u;
				_nowVertex->v = pMesh->m_Mesh_TVertices[pMesh->m_MeshFaces[i]->m_TFace[j]]->m_v;
			}

			/// parser�� ���� meshvertices�� optimizedvertices�� ���ؼ� position�� ������
			/// normal, u, v �� �߿� ���� �ٸ��� ������ ���� �������Ѵ�
			if (_nowVertex->m_Normal != pMesh->m_MeshFaces[i]->m_VertexNormal[j] ||
				_nowVertex->u != pMesh->m_Mesh_TVertices[pMesh->m_MeshFaces[i]->m_TFace[j]]->m_u ||
				_nowVertex->v != pMesh->m_Mesh_TVertices[pMesh->m_MeshFaces[i]->m_TFace[j]]->m_v)
			{
				/// ���� ����� �־� ����� 
				Vertex* _newVertex = new Vertex();
				_newVertex->m_Pos = _nowVertex->m_Pos;
				_newVertex->m_Indices = _nowVertex->m_Indices;
				_newVertex->m_Normal = pMesh->m_MeshFaces[i]->m_VertexNormal[j];

				/// texture�� ������ u, v �־���
				if (pMesh->m_Mesh_NumTVertex != 0)
				{
					_newVertex->u = pMesh->m_Mesh_TVertices[pMesh->m_MeshFaces[i]->m_TFace[j]]->m_u;
					_newVertex->v = pMesh->m_Mesh_TVertices[pMesh->m_MeshFaces[i]->m_TFace[j]]->m_v;
				}

				/// �׷��� index�� �߰��ؾ��Ѵ� 
				int _newVerIndex = pMesh->m_opt_vertex.size(); // ������� 1ũ�� 

				/// index �ٲ��ְ�
				pMesh->m_MeshFaces[i]->m_VertexIndex[j] = _newVerIndex;
				// vector�� �־���
				pMesh->m_opt_vertex.push_back(_newVertex);
			}

			/*_nowVertex->m_Normal = pMesh->m_MeshFaces[i]->vertexNormal[j];

			_nowVertex->u = pMesh->m_Mesh_TVertices[pMesh->m_MeshFaces[i]->m_TFace[j]]->u;
			_nowVertex->v = pMesh->m_Mesh_TVertices[pMesh->m_MeshFaces[i]->m_TFace[j]]->v;*/
		}
	}

	/// vertex���� index�鵵 optimized index vector�� ����
	pMesh->m_opt_index = new IndexList[pMesh->m_MeshFaces.size()];

	for (unsigned int i = 0; i < pMesh->m_MeshFaces.size(); i++)
	{
		for (unsigned int j = 0; j < 3; j++)
		{
			pMesh->m_opt_index[i].Index[j] = pMesh->m_MeshFaces[i]->m_VertexIndex[j];
		}
	}

	return TRUE;
}

bool CASEParser::ConvertAll(Mesh* pMesh)
{
	// ���ؽ����� �ϴ� ��� ����
	if (pMesh->m_MeshVertices.size() > 0)
	{
		for (unsigned int i = 0; i < pMesh->m_MeshVertices.size(); i++)
		{
			Vertex* _pVertex = new Vertex;
			_pVertex->m_Pos = pMesh->m_MeshVertices[i]->m_Pos;

			pMesh->m_opt_vertex.push_back(_pVertex);
		}
	}
	else
	{
		return false;
	}

	// �������� face�� �������� �� �ε����� ã�ƾ� ��
	unsigned int _faceCount = pMesh->m_MeshFaces.size();
	TRACE("FaceCount : %d", _faceCount);

	for (unsigned int i = 0; i < pMesh->m_MeshFaces.size(); i++)
	{
		TRACE("FaceNum : %d / %d\n", i, _faceCount);

		for (int j = 0; j < 3; j++)
		{
			Vertex* _nowVertex = pMesh->m_opt_vertex[pMesh->m_MeshFaces[i]->m_VertexIndex[j]];

			// face�������� ���ؽ��� �븻�� �־��ش�.
			// (���ÿ�)
			_nowVertex->m_Normal.x = pMesh->m_MeshFaces[i]->m_VertexNormal[j].x;
			_nowVertex->m_Normal.y = pMesh->m_MeshFaces[i]->m_VertexNormal[j].y;
			_nowVertex->m_Normal.z = pMesh->m_MeshFaces[i]->m_VertexNormal[j].z;

			_nowVertex->u = pMesh->m_Mesh_TVertices[pMesh->m_MeshFaces[i]->m_TFace[j]]->m_u;
			_nowVertex->v = pMesh->m_Mesh_TVertices[pMesh->m_MeshFaces[i]->m_TFace[j]]->m_v;
		}
	}

	// �ε����� �׳� ����
	pMesh->m_opt_index = new IndexList[pMesh->m_MeshFaces.size()];

	for (unsigned int i = 0; i < pMesh->m_MeshFaces.size(); i++)
	{
		for (int j = 0; j < 3; j++)
		{
			pMesh->m_opt_index[i].Index[j] = pMesh->m_MeshFaces[i]->m_VertexIndex[j];
		}
	}

	return TRUE;
}

bool CASEParser::SetLocalTM(Mesh* pMesh)
{
	// Parsing�� ����node�� 
	Matrix _worldNodeTM = {
			   pMesh->m_TMRow0.x, pMesh->m_TMRow0.y, pMesh->m_TMRow0.z, 0.0f,
			   pMesh->m_TMRow1.x, pMesh->m_TMRow1.y, pMesh->m_TMRow1.z, 0.0f,
			   pMesh->m_TMRow2.x, pMesh->m_TMRow2.y, pMesh->m_TMRow2.z, 0.0f,
			   pMesh->m_TMRow3.x, pMesh->m_TMRow3.y, pMesh->m_TMRow3.z, 1.0f };

	// ���� node�� ������� ���ؽ����� �����ָ� ������ǥ(����)���� ������
	for (int i = 0; i < pMesh->m_opt_vertex.size(); i++)
	{
		Vector3 _nowPos = { pMesh->m_opt_vertex[i]->m_Pos.x,
								   pMesh->m_opt_vertex[i]->m_Pos.y,
								   pMesh->m_opt_vertex[i]->m_Pos.z }; // ������ 1: ����� �ϴ���...

		pMesh->m_opt_vertex[i]->m_Pos = Vector3::Transform(_nowPos, _worldNodeTM.Invert());
	}

	/// �θ��� �����ϸ�
	if (pMesh->m_IsParentExist == true)
	{
		for (UINT i = 0; i < m_MeshList.size(); i++)
		{
			// ��� �̸� ������ �н�
			if (pMesh->m_NodeName == m_MeshList[i]->m_NodeName) continue;

			// ���� �޽��� �θ����̸��� �θ����� ����̸��� ������
			if (pMesh->m_ParentNodeName == m_MeshList[i]->m_NodeName)
			{
				Matrix _parWorldNodeTM = {
					 m_MeshList[i]->m_TMRow0.x, m_MeshList[i]->m_TMRow0.y, m_MeshList[i]->m_TMRow0.z, 0.0f,
					 m_MeshList[i]->m_TMRow1.x, m_MeshList[i]->m_TMRow1.y, m_MeshList[i]->m_TMRow1.z, 0.0f,
					 m_MeshList[i]->m_TMRow2.x, m_MeshList[i]->m_TMRow2.y, m_MeshList[i]->m_TMRow2.z, 0.0f,
					 m_MeshList[i]->m_TMRow3.x, m_MeshList[i]->m_TMRow3.y, m_MeshList[i]->m_TMRow3.z, 1.0f };

				// LocalTM = WorldNodeTM * ParentWorldNodeTM^-1
				pMesh->m_LocalTM = _worldNodeTM * _parWorldNodeTM.Invert();

				// �θ�޽� �����͸� �־��ش�
				pMesh->m_pParentMesh = m_MeshList[i];
			}
		}
	}
	else // �θ��� ������
	{
		pMesh->m_LocalTM = _worldNodeTM; // ���� ��尡 �� ���� ���
	}

	return true;
}

//bool CASEParser::SetNodeTM(Mesh* pMesh)
//{	 
//	 if (pMesh->m_IsParentExist == true && pMesh->m_pParentMesh != nullptr)
//	 {
//		  pMesh->m_NodeTM = pMesh->m_LocalTM * GetParentNodeTM(pMesh->m_pParentMesh);
//	 }
//	 else
//	 {
//		  pMesh->m_NodeTM = pMesh->m_LocalTM;
//	 }
//
//	 return true;
//}
//
//DirectX::SimpleMath::Matrix CASEParser::GetParentNodeTM(Mesh* pMesh)
//{
//	 /// Parent ���� ���� �θ��尡 ���������� ��͸� ź��
//	 if (pMesh->m_IsParentExist == true && pMesh->m_pParentMesh != nullptr)
//	 {
//		  return pMesh->m_LocalTM * GetParentNodeTM(pMesh->m_pParentMesh);
//	 }
//	 else
//	 {
//		  return pMesh->m_LocalTM;
//	 }
//}
//
//bool CASEParser::NodeTMAssemble(Mesh* pMesh)
//{
//	 for (UINT i = 0; i < pMesh->m_OptimizedVertices.size(); i++)
//	 {
//		  pMesh->m_OptimizedVertices[i]->m_Pos = Vector4::Transform(pMesh->m_OptimizedVertices[i]->m_Pos, pMesh->m_NodeTM);
//	 }
//
//	 return true;
//}

ParserData::Mesh* CASEParser::GetMesh(int index)
{
	return m_MeshList[index];
}

//----------------------------------------------------------------
// ��� ȣ����� ������ �ϴ� �б� �Լ��̴�.
//
// �ϴ� �а�, ��ū�� ���� �����Ѵ�.
//
// ��͸� ���� ������ { �� ���� ����̴�.
// �����ϴ� ������ '}'�� �����ų� TOKEND_END�� ������ ���̴�.
//
// ���Ҿ�, ��͸� ���� �� ��ū�� �о�, ���� �� �Լ��� � ��������� �����Ѵ�.
//
//----------------------------------------------------------------
void CASEParser::Parsing_DivergeRecursiveALL(int depth)
{
	//----------------------------------------------------------------------
	// ���������� ����
	// ��� �����ٰ� ���� �̻� ������ ���� ���(����)�� üũ�ϱ� ���� ����
	int i = 0;
	// ����Ʈ�� �����͸� �־�� �� �� ���̴� ����Ʈ ī����.
	int listcount = 0;		// �ſ� C����� �ٸ� �ذ�å�� ������?
	// �̰��� �� ��ū�� � �����ΰ��� �Ǻ����ش�.
	// �̰��� ��� ������ �ϰ� ��͸� �����°��� �����ϴ�? -> �߸��� ��͹���̴�?
	LONG nowtoken;
	//----------------------------------------------------------------------

	Matrix TM1, TM2;
	Quaternion tempQT;
	Quaternion prevQT;
	Quaternion resultQT;

	// �̰� ���ִ°���
	static int lastFaceIndex = 0;
	static int lastVerIndex = 0;

	/// �̰��� �ϸ� �� ���� ��ū�� �а�, �� ������ ������ �� �� �ִ�. (, ������ : �տ����� �ϰ� �ڿ��� �̾�ض�)
	while (nowtoken = m_lexer->GetToken(m_TokenString), nowtoken != TOKEND_BLOCK_END)
	{
		// �ϴ� �� ���� ��ū�� �а�, �װ��� ��ȣ �ݱⰡ �ƴ϶��.
		// �Ѿ�� ��ū�� ���� ó�����ش�.

		static int iv = 0;

		switch (nowtoken)
		{
		case TOKEND_BLOCK_START: // '{' <- �̰� ������ ��ŸƮ
		{
			Parsing_DivergeRecursiveALL(depth++);
			break;
		}
		case TOKENR_HELPER_CLASS:
		{
			Parsing_String();
			break;
		}

		//--------------------
		/// 3DSMAX_ASCIIEXPORT
		//--------------------
		case TOKENR_3DSMAX_ASCIIEXPORT:
		{
			m_data_asciiexport = Parsing_NumberLong();
			break;
		}

		//--------------------
		/// COMMENT
		//--------------------
		case TOKENR_COMMENT:
		{
			Parsing_String();	// �׳� m_TokenString�� �о������ ���� ��.
			//AfxMessageBox( m_TokenString, NULL, NULL);		/// �ӽ÷� �ڸ�Ʈ�� ����غ���
			break;
		}

		//--------------------
		/// SCENE
		//--------------------
		case TOKENR_SCENE:
		{
			break;
		}
		case TOKENR_SCENE_FILENAME:
		{
			m_scenedata.m_filename = Parsing_String();		// �ϰ��� �ִ� �Լ��� ����� ���� String�� Int�� ��������.
			break;
		}
		case TOKENR_SCENE_FIRSTFRAME:
		{
			m_scenedata.m_firstframe = Parsing_NumberLong();
			break;
		}
		case TOKENR_SCENE_LASTFRAME:
		{
			m_scenedata.m_lastframe = Parsing_NumberLong();
			break;
		}
		case TOKENR_SCENE_FRAMESPEED:
		{
			m_scenedata.m_framespeed = Parsing_NumberLong();
			break;
		}
		case TOKENR_SCENE_TICKSPERFRAME:
		{
			m_scenedata.m_ticksperframe = Parsing_NumberLong();
			break;
		}
		case TOKENR_SCENE_MESHFRAMESTEP:
		{
			m_scenedata.m_meshframestep = Parsing_NumberLong();
			break;
		}
		case TOKENR_SCENE_KEYFRAMESTEP:
		{
			m_scenedata.m_keyframestep = Parsing_NumberLong();
			break;
		}
		case TOKENR_SCENE_BACKGROUND_STATIC:
		{
			m_scenedata.m_scene_background_static = Parsing_NumberVector3();
			break;
		}
		case TOKENR_SCENE_AMBIENT_STATIC:
		{
			m_scenedata.m_scene_ambient_static = Parsing_NumberVector3();
			break;
		}
		case TOKENR_SCENE_ENVMAP:
		{
			// �׳� ���� ������ �о������ }�� ���ö�����
			while (nowtoken = m_lexer->GetToken(m_TokenString), nowtoken != TOKEND_BLOCK_END)
			{
			}
		}
		break;

		//--------------------
		/// MATERIAL_LIST
		//--------------------


		//--------------------
		/// GEOMOBJECT
		//--------------------
		case TOKENR_GROUP:
			//	�� ���� �׷� ����. �� ������ �̸��� ��Ʈ������ ������� �ϴµ�.
			break;

		case TOKENR_HELPEROBJECT:
		{
			// ������Ʈ�� Ÿ�� ������. �̰Ϳ� ���� ���� �ٸ� �Ľ� ��� �ߵ�.
			m_ParsingMode = eHelperObject;
			Create_onemesh_to_list();
			break;
		}
		case TOKENR_SHAPEOBJECT:
		{
			m_ParsingMode = eShape;
			Create_onemesh_to_list();
			break;
		}
		case TOKENR_GEOMOBJECT:
		{
			/// ���ο� �޽ð� ����ٴ� ��, ���� ������ mesh�� �ϳ� ����, �� �����͸� ����Ʈ�� �ְ�, m_onemesh�� �� �����͸� ����
			m_ParsingMode = eGeomobject;
			Create_onemesh_to_list();
			// ������Ʈ�� Ÿ�� ������. �̰Ϳ� ���� ���� �ٸ� �Ľ� ��� �ߵ�.
			break;
		}
		case TOKENR_NODE_NAME:
		{
			// ��� ������ ������Ʈ���� ���� �� �� �ִ� ������ ���̴�.
			// ��忡 ���� �־�� �� ���� �ٸ���.
			if (m_ParsingMode == eAnimation)
			{
				//m_Animation->m_NodeNameAni = Parsing_String();
			}
			else
			{
				m_OneMesh->m_NodeName = Parsing_String();
			}

			break;
		}
		case TOKENR_NODE_PARENT:
		{
			// �� ����� �θ� ����� ����.
			// �ϴ� �Է��� �ϰ�, ���߿� ��������.
			m_OneMesh->m_IsParentExist = true;
			m_OneMesh->m_ParentNodeName = Parsing_String();

			break;
		}
		case TOKENR_BOUNDINGBOX_MAX:
		{
			break;
		}
		case TOKENR_BOUNDINGBOX_MIN:
		{
			break;
		}

		/// NODE_TM
		case TOKENR_NODE_TM:
		{
			// (NODE_TM���� ���� �� NODE_NAME�� �ѹ� �� ���´�.)
			// (Animation���� ������ �ؾ� �ϱ� ������ �̷��� ��带 �����ش�)

			/// �Դٰ�,
			// ī�޶�� NodeTM�� �ι� ���´�. �ι�°��� ���� �ʴ´�.
			// �ƿ� �� ��Ϳ��� ������ ��Ű�� ������. �߰��� �д°��� �����ؾ� �ϱ� ������...
			//if (m_onemesh->m_camera_isloadTarget) {
			//	'}'�� ���ö����� �����°� ������ ����! �� �̷��� �ȵǳ�..
			// ��ͱ����� �������̴�....

			/// ������ ����� �ϳ�?
			//if (m_parsingmode == eGeomobject)

			//if (m_parsingmode == eHelperObject)


			break;
		}
		case TOKENR_INHERIT_POS: // ī�޶�� NodeTM�� �ι� ���´�. �ι�°��� ���� �ʴ´�.
		{
			m_OneMesh->m_InheritPos.x = Parsing_NumberInt();
			m_OneMesh->m_InheritPos.y = Parsing_NumberInt();
			m_OneMesh->m_InheritPos.z = Parsing_NumberInt();
			break;
		}
		case TOKENR_INHERIT_ROT:
		{
			m_OneMesh->m_InheritRot.x = Parsing_NumberInt();
			m_OneMesh->m_InheritRot.y = Parsing_NumberInt();
			m_OneMesh->m_InheritRot.z = Parsing_NumberInt();
			break;
		}
		case TOKENR_INHERIT_SCL:
		{
			m_OneMesh->m_InheritScl.x = Parsing_NumberInt();
			m_OneMesh->m_InheritScl.y = Parsing_NumberInt();
			m_OneMesh->m_InheritScl.z = Parsing_NumberInt();
			break;
		}
		case TOKENR_TM_ROW0:
		{
			m_OneMesh->m_TMRow0 = Parsing_NumberVector3();
			break;
		}
		case TOKENR_TM_ROW1:
		{
			m_OneMesh->m_TMRow2 = Parsing_NumberVector3();
			break;
		}
		case TOKENR_TM_ROW2:
		{
			m_OneMesh->m_TMRow1 = Parsing_NumberVector3();
			break;
		}
		case TOKENR_TM_ROW3:
		{
			m_OneMesh->m_TMRow3 = Parsing_NumberVector3();
			break;
		}
		case TOKENR_TM_POS:
		{
			m_OneMesh->m_TMPos = Parsing_NumberVector3();
			break;
		}
		case TOKENR_TM_ROTAXIS:
		{
			m_OneMesh->m_TMRotAxis = Parsing_NumberVector3();
			break;
		}
		case TOKENR_TM_ROTANGLE:
		{
			m_OneMesh->m_TMRotAngle = Parsing_NumberFloat();
			break;
		}
		case TOKENR_TM_SCALE:
		{
			m_OneMesh->m_TMScale = Parsing_NumberVector3();
			break;
		}
		case TOKENR_TM_SCALEAXIS:
		{
			m_OneMesh->m_TMScaleAxis = Parsing_NumberVector3();
			break;
		}
		case TOKENR_TM_SCALEAXISANG: // ���� ī�޶� ���¿��ٸ� �̹� ��带 ���� ������ ǥ�����ش�.
		{
			m_OneMesh->m_TMScaleAxisAng = Parsing_NumberFloat();
			break;
		}

		/// Animation
		case TOKENR_TM_ANIMATION:
		{
			m_ParsingMode = eAnimation;
			//Create_animationdata_to_list();

			break;
		}
		case TOKENR_CONTROL_POS_TRACK:
		{
			break;
		}
		case TOKENR_CONTROL_POS_SAMPLE:
		{
			//CAnimation_pos _nowPos;
			//_nowPos.m_time = Parsing_NumberInt();
			//_nowPos.m_pos = Parsing_NumberVector3();
			//m_Animation->m_position.push_back(&_nowPos);
			break;
		}

		/// MESH
		case TOKENR_MESH:
		{
			//m_OneMesh = m_MeshList.back();
			break;
		}
		case TOKENR_TIMEVALUE:
		{
			m_OneMesh->m_timevalue = Parsing_NumberInt();
			break;
		}
		case TOKENR_MESH_NUMBONE:
			// �̰� �ִٸ� �̰��� Skinned Mesh��� ������ ���´�.
			// ���� �Է�
			break;
		case TOKENR_MESH_NUMSKINWEIGHT:
			break;
		case TOKENR_MESH_NUMVERTEX:
		{
			int _count = Parsing_NumberInt();
			m_OneMesh->m_Mesh_NumVertex = _count;

			for (int i = 0; i < _count; ++i)
			{
				m_OneMesh->m_MeshVertices.push_back(new Vertex());
			}

			break;
		}
		case TOKENR_MESH_NUMFACES:
		{
			int _count = Parsing_NumberInt();
			m_OneMesh->m_Mesh_NumFace = _count;

			for (int i = 0; i < _count; ++i)
			{
				m_OneMesh->m_MeshFaces.push_back(new Face());
			}

			break;
		}

		/// MESH_VERTEX_LIST
		case TOKENR_MESH_VERTEX_LIST:
			// ���ؽ��� ������ ����־�� �ϴµ�
			// �̹� ���ͷ� ������ �� �����Ƿ� �׳� ������ �ȴ�.
			break;
		case TOKENR_MESH_VERTEX:
		{
			// ���ؽ� ������ �Է�
			int _index = Parsing_NumberInt();
			m_OneMesh->m_MeshVertices[_index]->m_Indices = _index;
			m_OneMesh->m_MeshVertices[_index]->m_Pos.x = Parsing_NumberFloat();
			m_OneMesh->m_MeshVertices[_index]->m_Pos.z = Parsing_NumberFloat();
			m_OneMesh->m_MeshVertices[_index]->m_Pos.y = Parsing_NumberFloat();
			break;
		}

		/// Bone
		case TOKENR_SKIN_INITTM:
			break;
		case TOKENR_BONE_LIST:
			break;
		case TOKENR_BONE:
		{
			/// ��� ü���� �� �ְ�, Bone�� �����ϰ� �ִٴ� ���� �̰��� ��Ű�� ������Ʈ��� ���̴�.
			// �� �ϳ��� ���� �ӽ� ������ ����, ���Ϳ� �ְ�
			// Bone�� �ѹ��� �о� ����
			break;
		}
		//�� ������ ���� �̸��� �־�� �Ѵ�. ������ {�� �� �� �� �������Ƿ� �ӽ� ������ �����μ� �����ؾ߰���.
		case TOKENR_BONE_NAME:
			break;
		case TOKENR_BONE_PROPERTY:
			// �� ���� ABSOLUTE�� ������� �ϴµ�, �� ���� ����.
			break;
			// �������� TM_ROW0~3�� �����µ� ���� ���õ�..

		case TOKENR_MESH_WVERTEXS:
			break;

		case TOKENR_MESH_WEIGHT:
		{
			// ���ؽ� �ϳ��� ������ ���� ����Ʈ�� ����
			break;
		}
		case TOKENR_BONE_BLENGING_WEIGHT:
		{
			// ��ü �� �ܰ踦 ���°ž�...
			// ����ġ �Ѱ��� ���� ����Ʈ�� �ִ´�
			/// �� ��....
			break;
		}

		/// MESH_FACE_LIST
		case TOKENR_MESH_FACE_LIST:
			// 
			break;
		case TOKENR_MESH_NORMALS:
			break;
		case TOKENR_MESH_FACE:
		{
			// Face�� ��ȣ�ε�...
			int _index = Parsing_NumberInt();
			// ABC �б�

			for (int _abc = 0; _abc < 3; _abc++)
			{
				Parsing_String();
				m_OneMesh->m_MeshFaces[_index]->m_VertexIndex[_abc] = Parsing_NumberInt();
			}

			/// (�ڿ� ������ �� ������ default�� ���� ��ŵ�� ���̴�.)
			/// ......
			break;
		}
		case TOKENR_MESH_FACENORMAL:
		{
			int _index = Parsing_NumberInt();
			m_OneMesh->m_MeshFaces[_index]->m_Normal = Parsing_NumberVector3();
			/* m_OneMesh->m_meshface[_index]->m_normal.y = Parsing_NumberFloat();
			 m_OneMesh->m_meshface[_index]->m_normal.z = Parsing_NumberFloat();*/

			 // �̰� ���ϴ°���
			lastFaceIndex = _index;
			lastVerIndex = 0;

			break;
		}
		case TOKENR_MESH_VERTEXNORMAL:
		{
			int _index = Parsing_NumberInt();
			//m_OneMesh->m_meshface[lastFaceIndex]->m_normalvertex[lastVerIndex].x = Parsing_NumberFloat();
			//m_OneMesh->m_meshface[lastFaceIndex]->m_normalvertex[lastVerIndex].y = Parsing_NumberFloat();
			//m_OneMesh->m_meshface[lastFaceIndex]->m_normalvertex[lastVerIndex].z = Parsing_NumberFloat();

			m_OneMesh->m_MeshFaces[lastFaceIndex]->m_VertexNormal[lastVerIndex] = Parsing_NumberVector3();
			lastVerIndex++;

			break;
		}
		case TOKENR_MESH_NUMTVERTEX:
		{
			m_OneMesh->m_Mesh_NumTVertex = Parsing_NumberInt();

			int _count = m_OneMesh->m_Mesh_NumTVertex;
			if (_count > 0)
			{
				for (int i = 0; i < _count; i++)
				{
					m_OneMesh->m_Mesh_TVertices.push_back(new COneTVertex());
				}
			}

			break;
		}
		case TOKENR_MESH_NUMCVERTEX:
		{
			break;
		}

		/// MESH_TVERTLIST

		case TOKENR_MESH_TVERTLIST:
			//
			break;
		case TOKENR_MESH_TVERT:
		{
			// ���ؽ��� �ε����� �����µ� ������ ������ �����Ƿ� ������.
			// ���ο� TVertex�� ���� ���Ϳ� �ִ´�
			int _index = Parsing_NumberInt();

			m_OneMesh->m_Mesh_TVertices[_index]->m_u = Parsing_NumberFloat();
			m_OneMesh->m_Mesh_TVertices[_index]->m_v = Parsing_NumberFloat();


			break;
		}
		case TOKENR_MESH_NUMTVFACES:
			break;
		case TOKENR_MESH_TFACE:
		{
			int _index = Parsing_NumberInt();
			m_OneMesh->m_MeshFaces[_index]->m_TFace[0] = Parsing_NumberInt();
			m_OneMesh->m_MeshFaces[_index]->m_TFace[1] = Parsing_NumberInt();
			m_OneMesh->m_MeshFaces[_index]->m_TFace[2] = Parsing_NumberInt();

			break;
		}
		case TOKENR_WIREFRAME_COLOR:
		{
			m_OneMesh->m_wireframe_color = Parsing_NumberVector3();
			break;
		}

		case TOKEND_END:
			// �Ƹ��� �̰� ������ ���� ��Ÿ�������ΰ� ������. while�� Ż���ؾ� �ϴµ�?

			//AfxMessageBox("������ ���� �� �� �����ϴ�!", MB_OK, NULL);
			TRACE("TRACE: �Ľ���: ������ ���� �ý��ϴ�!\n");
			return;

			break;

			/// ���� �ƹ��͵� �ش����� ������
		default:
			// �ƹ��͵� ���� �ʴ´�.
			break;

		} // end of switch(nowtoken)

		///-----------------------------------------------
		/// ���� �ڵ�.
		i++;
		if (i > 1000000)
		{
			// ������ 1000000���̻��̳� �� ������ ����. (�����Ͱ� 100000���� �ƴ��̻�)
			// ���� 1000000�̻� ���Ҵٸ� Ȯ���� ���� ������ �ִ� ���̹Ƿ�
			TRACE("������ �鸸�� ���ҽ��ϴ�!");
			return;
		}
		///-----------------------------------------------

	} // end of while(nowtoken = m_lexer->GetToken(m_TokenString), nowtoken != TOKEND_BLOCK_END)

	// ������� �Դٸ� while()�� ����ٴ� ���̰� �� ����
	// ��ȣ�� �ݾҴٴ� ���̹Ƿ� �������� (��� ��)

	return;
}


///----------------------------------------------------------------------
/// parsing�� ���� ������ �Լ���
///----------------------------------------------------------------------

// long�� �о �������ش�.
int CASEParser::Parsing_NumberLong()
{
	LONG			token;
	LONG			tempNumber;

	token = m_lexer->GetToken(m_TokenString);	//ASSERT(token == TOKEND_NUMBER);
	tempNumber = strtoul(m_TokenString, NULL, 10);

	return			tempNumber;
}

// float
float CASEParser::Parsing_NumberFloat()
{
	LONG			token;
	float			tempNumber;

	token = m_lexer->GetToken(m_TokenString);	//ASSERT(token == TOKEND_NUMBER);
	tempNumber = (float)atof(m_TokenString);

	return			tempNumber;
}

// String
LPSTR CASEParser::Parsing_String()
{
	/// ��m_TokenString ( char[255] ) �̱� ������ CString�� ������ ���� ���Ŷ� �����ߴµ�, �������� CString�� �� ������� �� ����. �˾Ƽ� �޾Ƶ��̴µ�?
	m_lexer->GetToken(m_TokenString);

	return m_TokenString;
}

// int
int CASEParser::Parsing_NumberInt()
{
	LONG			token;
	int				tempNumber;

	token = m_lexer->GetToken(m_TokenString);	//ASSERT(token == TOKEND_NUMBER);
	tempNumber = (int)atoi(m_TokenString);

	return			tempNumber;
}

// 3���� Float�� ���� �ϳ���
Vector3 CASEParser::Parsing_NumberVector3()
{
	LONG				token;
	Vector3			tempVector3;

	token = m_lexer->GetToken(m_TokenString);
	tempVector3.x = (float)atof(m_TokenString);
	token = m_lexer->GetToken(m_TokenString);
	tempVector3.z = (float)atof(m_TokenString);
	token = m_lexer->GetToken(m_TokenString);
	tempVector3.y = (float)atof(m_TokenString);

	return			tempVector3;		// ����ƽ ������ ���۷������ٴ� �� ������ ����.
}

///--------------------------------------------------
/// ���ο��� ������ ����, ����Ʈ�� �ִ´�
///--------------------------------------------------

// �޽ø� �ϳ� �������� �����ϰ�, �� �����͸� ����Ʈ�� �ִ´�.
void CASEParser::Create_onemesh_to_list()
{
	Mesh* temp = new Mesh;
	m_OneMesh = temp;
	m_OneMesh->m_SceneData = m_scenedata;		// Ŭ������ �� ����
	m_MeshList.push_back(m_OneMesh);
}

// ��Ʈ���� �ϳ��� �������� �����ϰ�, �� �����͸� ����Ʈ�� �ִ´�.
void CASEParser::Create_materialdata_to_list()
{
	ASEMaterial* temp = new ASEMaterial;
	m_materialdata = temp;
	m_list_materialdata.push_back(m_materialdata);
}

// �ִϸ��̼ǵ����� �ϳ��� �������� �����ϰ�, �� �����͸� ����Ʈ�� �ִ´�.
//void CASEParser::Create_animationdata_to_list()
//{
//	Animation* temp = new Animation;
//	m_Animation = temp;
//	m_AnimationList.push_back(m_Animation);
//}

// ���� �ϳ���..
void CASEParser::Create_onevertex_to_list()
{
	Vertex* temp = new Vertex;
	m_OneMesh->m_MeshVertices.push_back(temp);
}

