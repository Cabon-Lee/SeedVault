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
// 로딩한다.
// 이것이 끝났다면 정해진 데이터형에 모든 데이터를 읽어서 들어가 있어야 한다.
//
// Parsing에 대해:
// 항상 느끼는 것이지만 parsing이라는 것은 데이터가 일정 규격에 맞게 들어가 있다는 것을 전제로 한다.
// 하지만, 파일 내부에 순서가 바뀌어 들어가 있는것이 가능하다던지 하는 규칙이 생기면
// 검색하는 루틴이 복잡해지기 마련. 일단은 순서대로 일정 규약으로 들어가 있다는것을 가정하자.
// -> 그래서, 재귀 호출을 하는 하나의 큰 함수로 해결봤다.
// -> depth를 기준으로 오동작에 대한 안전 코드를 넣어야겠다
//---------------------------------------------------------------------------------------------------
bool CASEParser::Load(LPSTR p_File)
{
	/// 0) 파일을 로드한다.
	if (!m_lexer->Open(p_File))
	{
		TRACE("파일을 여는 중에 문제가 발생했습니다!");
		return FALSE;
	}

	/// 1) 다롱이아부지김교수의 파워풀한 재귀함수 한개로 처리 끝!
	Parsing_DivergeRecursiveALL(0);

	return TRUE;
}

/// 1차 변환

// CScenedata 복사
bool CASEParser::TranslateToD3DFormat_scene(Scenedata* pScene)
{
	// 값 복사
	*pScene = this->m_scenedata;
	return TRUE;
}

/// 메시의 최적화를 해 준다.
/// 노말값, 텍스쳐 좌표에 따라 버텍스를 늘리고, 중첩되는것은 제거하고..
bool CASEParser::VertexOptimization(Mesh* pMesh)
{
	if (pMesh->m_MeshVertices.size() > 0)
	{
		for (unsigned int _ver = 0; _ver < pMesh->m_Mesh_NumVertex; _ver++) // vertex 복사
		{
			/// 일단 load된 vertex들을 optimized vertex vector에 넣음
			Vertex* _pVertex = new Vertex();
			_pVertex->m_Pos = pMesh->m_MeshVertices[_ver]->m_Pos;
			_pVertex->m_Indices = pMesh->m_MeshVertices[_ver]->m_Indices;
			/// texture 값도 넣음
			//_pVertex->u = pMesh->m_MeshVertices[_ver]->u;
			//_pVertex->v = pMesh->m_MeshVertices[_ver]->v;

			pMesh->m_opt_vertex.push_back(_pVertex); /// 최적화된 vertex를 모아야 함
		}
	}
	else
	{
		return false;
	}

	/// Face를 모두 돌면서 버텍스들의 노말들을 조사
	for (unsigned int i = 0; i < pMesh->m_Mesh_NumFace; i++)
	{
		/// Face마다 vertex index가 3개씩 (3각형)
		for (unsigned int j = 0; j < 3; j++)
		{
			// 현재 vertex를 가져오고 비교를 실시해야함
			Vertex* _nowVertex = pMesh->m_opt_vertex[pMesh->m_MeshFaces[i]->m_VertexIndex[j]];

			/// texture가 있으면 u, v값 설정한다
			if (pMesh->m_Mesh_NumTVertex != 0)
			{
				_nowVertex->u = pMesh->m_Mesh_TVertices[pMesh->m_MeshFaces[i]->m_TFace[j]]->m_u;
				_nowVertex->v = pMesh->m_Mesh_TVertices[pMesh->m_MeshFaces[i]->m_TFace[j]]->m_v;
			}

			/// parser로 넣은 meshvertices와 optimizedvertices를 비교해서 position은 같은데
			/// normal, u, v 값 중에 서로 다른게 있으면 새로 만들어야한다
			if (_nowVertex->m_Normal != pMesh->m_MeshFaces[i]->m_VertexNormal[j] ||
				_nowVertex->u != pMesh->m_Mesh_TVertices[pMesh->m_MeshFaces[i]->m_TFace[j]]->m_u ||
				_nowVertex->v != pMesh->m_Mesh_TVertices[pMesh->m_MeshFaces[i]->m_TFace[j]]->m_v)
			{
				/// 새로 만들어 넣어 줘야함 
				Vertex* _newVertex = new Vertex();
				_newVertex->m_Pos = _nowVertex->m_Pos;
				_newVertex->m_Indices = _nowVertex->m_Indices;
				_newVertex->m_Normal = pMesh->m_MeshFaces[i]->m_VertexNormal[j];

				/// texture가 있으면 u, v 넣어줌
				if (pMesh->m_Mesh_NumTVertex != 0)
				{
					_newVertex->u = pMesh->m_Mesh_TVertices[pMesh->m_MeshFaces[i]->m_TFace[j]]->m_u;
					_newVertex->v = pMesh->m_Mesh_TVertices[pMesh->m_MeshFaces[i]->m_TFace[j]]->m_v;
				}

				/// 그러면 index도 추가해야한다 
				int _newVerIndex = pMesh->m_opt_vertex.size(); // 사이즈보단 1크게 

				/// index 바꿔주고
				pMesh->m_MeshFaces[i]->m_VertexIndex[j] = _newVerIndex;
				// vector에 넣어줌
				pMesh->m_opt_vertex.push_back(_newVertex);
			}

			/*_nowVertex->m_Normal = pMesh->m_MeshFaces[i]->vertexNormal[j];

			_nowVertex->u = pMesh->m_Mesh_TVertices[pMesh->m_MeshFaces[i]->m_TFace[j]]->u;
			_nowVertex->v = pMesh->m_Mesh_TVertices[pMesh->m_MeshFaces[i]->m_TFace[j]]->v;*/
		}
	}

	/// vertex들의 index들도 optimized index vector에 넣음
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
	// 버텍스들은 일단 모두 복사
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

	// 나머지는 face를 기준으로 한 인덱스로 찾아야 함
	unsigned int _faceCount = pMesh->m_MeshFaces.size();
	TRACE("FaceCount : %d", _faceCount);

	for (unsigned int i = 0; i < pMesh->m_MeshFaces.size(); i++)
	{
		TRACE("FaceNum : %d / %d\n", i, _faceCount);

		for (int j = 0; j < 3; j++)
		{
			Vertex* _nowVertex = pMesh->m_opt_vertex[pMesh->m_MeshFaces[i]->m_VertexIndex[j]];

			// face기준으로 버텍스의 노말을 넣어준다.
			// (예시용)
			_nowVertex->m_Normal.x = pMesh->m_MeshFaces[i]->m_VertexNormal[j].x;
			_nowVertex->m_Normal.y = pMesh->m_MeshFaces[i]->m_VertexNormal[j].y;
			_nowVertex->m_Normal.z = pMesh->m_MeshFaces[i]->m_VertexNormal[j].z;

			_nowVertex->u = pMesh->m_Mesh_TVertices[pMesh->m_MeshFaces[i]->m_TFace[j]]->m_u;
			_nowVertex->v = pMesh->m_Mesh_TVertices[pMesh->m_MeshFaces[i]->m_TFace[j]]->m_v;
		}
	}

	// 인덱스는 그냥 복사
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
	// Parsing한 월드node들 
	Matrix _worldNodeTM = {
			   pMesh->m_TMRow0.x, pMesh->m_TMRow0.y, pMesh->m_TMRow0.z, 0.0f,
			   pMesh->m_TMRow1.x, pMesh->m_TMRow1.y, pMesh->m_TMRow1.z, 0.0f,
			   pMesh->m_TMRow2.x, pMesh->m_TMRow2.y, pMesh->m_TMRow2.z, 0.0f,
			   pMesh->m_TMRow3.x, pMesh->m_TMRow3.y, pMesh->m_TMRow3.z, 1.0f };

	// 월드 node의 역행렬을 버텍스한테 곱해주면 로컬좌표(원점)으로 보내짐
	for (int i = 0; i < pMesh->m_opt_vertex.size(); i++)
	{
		Vector3 _nowPos = { pMesh->m_opt_vertex[i]->m_Pos.x,
								   pMesh->m_opt_vertex[i]->m_Pos.y,
								   pMesh->m_opt_vertex[i]->m_Pos.z }; // 마지막 1: 뭐라고 하더라...

		pMesh->m_opt_vertex[i]->m_Pos = Vector3::Transform(_nowPos, _worldNodeTM.Invert());
	}

	/// 부모노드 존재하면
	if (pMesh->m_IsParentExist == true)
	{
		for (UINT i = 0; i < m_MeshList.size(); i++)
		{
			// 노드 이름 같으면 패스
			if (pMesh->m_NodeName == m_MeshList[i]->m_NodeName) continue;

			// 현재 메시의 부모노드이름과 부모노드의 노드이름과 같으면
			if (pMesh->m_ParentNodeName == m_MeshList[i]->m_NodeName)
			{
				Matrix _parWorldNodeTM = {
					 m_MeshList[i]->m_TMRow0.x, m_MeshList[i]->m_TMRow0.y, m_MeshList[i]->m_TMRow0.z, 0.0f,
					 m_MeshList[i]->m_TMRow1.x, m_MeshList[i]->m_TMRow1.y, m_MeshList[i]->m_TMRow1.z, 0.0f,
					 m_MeshList[i]->m_TMRow2.x, m_MeshList[i]->m_TMRow2.y, m_MeshList[i]->m_TMRow2.z, 0.0f,
					 m_MeshList[i]->m_TMRow3.x, m_MeshList[i]->m_TMRow3.y, m_MeshList[i]->m_TMRow3.z, 1.0f };

				// LocalTM = WorldNodeTM * ParentWorldNodeTM^-1
				pMesh->m_LocalTM = _worldNodeTM * _parWorldNodeTM.Invert();

				// 부모메시 포인터를 넣어준다
				pMesh->m_pParentMesh = m_MeshList[i];
			}
		}
	}
	else // 부모노드 없으면
	{
		pMesh->m_LocalTM = _worldNodeTM; // 로컬 노드가 곧 로컬 노드
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
//	 /// Parent 노드들 들어가서 부모노드가 없을때까지 재귀를 탄다
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
// 재귀 호출됨을 전제로 하는 분기 함수이다.
//
// 일단 읽고, 토큰에 따라 동작한다.
//
// 재귀를 들어가는 조건은 { 을 만날 경우이다.
// 리턴하는 조건은 '}'를 만나거나 TOKEND_END를 만났을 때이다.
//
// 더불어, 재귀를 들어가기 전 토큰을 읽어, 지금 이 함수가 어떤 모드인지를 결정한다.
//
//----------------------------------------------------------------
void CASEParser::Parsing_DivergeRecursiveALL(int depth)
{
	//----------------------------------------------------------------------
	// 지역변수들 선언
	// 재귀 돌리다가 일정 이상 루프를 도는 경우(오류)를 체크하기 위한 변수
	int i = 0;
	// 리스트에 데이터를 넣어야 할 때 쓰이는 리스트 카운터.
	int listcount = 0;		// 매우 C스러운데 다른 해결책이 없을까?
	// 이것은 현 토큰이 어떤 종류인가를 판별해준다.
	// 이것을 멤버 변수로 하고 재귀를 돌리는것은 위험하다? -> 잘못된 재귀방식이다?
	LONG nowtoken;
	//----------------------------------------------------------------------

	Matrix TM1, TM2;
	Quaternion tempQT;
	Quaternion prevQT;
	Quaternion resultQT;

	// 이건 왜있는거지
	static int lastFaceIndex = 0;
	static int lastVerIndex = 0;

	/// 이것을 하면 한 개의 토큰을 읽고, 그 종류와 내용을 알 수 있다. (, 연산자 : 앞에것을 하고 뒤에것 이어서해라)
	while (nowtoken = m_lexer->GetToken(m_TokenString), nowtoken != TOKEND_BLOCK_END)
	{
		// 일단 한 개의 토큰을 읽고, 그것이 괄호 닫기가 아니라면.
		// 넘어온 토큰에 따라 처리해준다.

		static int iv = 0;

		switch (nowtoken)
		{
		case TOKEND_BLOCK_START: // '{' <- 이게 나오면 스타트
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
			Parsing_String();	// 그냥 m_TokenString에 읽어버리는 역할 뿐.
			//AfxMessageBox( m_TokenString, NULL, NULL);		/// 임시로 코멘트를 출력해본다
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
			m_scenedata.m_filename = Parsing_String();		// 일관성 있는 함수의 사용을 위해 String과 Int도 만들어줬다.
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
			// 그냥 안의 내용을 읽어버린다 }가 나올때까지
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
			//	한 개의 그룹 시작. 이 다음에 이름이 스트링으로 나오기는 하는데.
			break;

		case TOKENR_HELPEROBJECT:
		{
			// 오브젝트의 타입 정해줌. 이것에 따라 서로 다른 파싱 모드 발동.
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
			/// 새로운 메시가 생겼다는 것, 지역 변수로 mesh를 하나 선언, 그 포인터를 리스트에 넣고, m_onemesh에 그 포인터를 복사
			m_ParsingMode = eGeomobject;
			Create_onemesh_to_list();
			// 오브젝트의 타입 정해줌. 이것에 따라 서로 다른 파싱 모드 발동.
			break;
		}
		case TOKENR_NODE_NAME:
		{
			// 어쩄든 지금은 오브젝트들을 구별 할 수 있는 유일한 값이다.
			// 모드에 따라 넣어야 할 곳이 다르다.
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
			// 현 노드의 부모 노드의 정보.
			// 일단 입력을 하고, 나중에 정리하자.
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
			// (NODE_TM으로 진입 후 NODE_NAME이 한번 더 나온다.)
			// (Animation과도 구별을 해야 하기 때문에 이렇게 모드를 적어준다)

			/// 게다가,
			// 카메라는 NodeTM이 두번 나온다. 두번째라면 넣지 않는다.
			// 아예 이 재귀에서 리턴을 시키고 싶지만. 중간에 읽는것을 무시해야 하기 때문에...
			//if (m_onemesh->m_camera_isloadTarget) {
			//	'}'이 나올때까지 나오는건 무조건 무시! 뭐 이런거 안되나..
			// 재귀구조의 문제점이다....

			/// 뭔가를 해줘야 하나?
			//if (m_parsingmode == eGeomobject)

			//if (m_parsingmode == eHelperObject)


			break;
		}
		case TOKENR_INHERIT_POS: // 카메라는 NodeTM이 두번 나온다. 두번째라면 넣지 않는다.
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
		case TOKENR_TM_SCALEAXISANG: // 현재 카메라 상태였다면 이미 노드를 읽은 것으로 표시해준다.
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
			// 이게 있다면 이것은 Skinned Mesh라고 단정을 짓는다.
			// 내용 입력
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
			// 버텍스의 값들을 집어넣어야 하는데
			// 이미 벡터로 선언이 돼 있으므로 그냥 넣으면 된다.
			break;
		case TOKENR_MESH_VERTEX:
		{
			// 버텍스 데이터 입력
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
			/// 모드 체인지 해 주고, Bone을 소유하고 있다는 것은 이것은 스키닝 오브젝트라는 것이다.
			// 본 하나를 만들어서 임시 포인터 보관, 벡터에 넣고
			// Bone의 넘버를 읽어 주자
			break;
		}
		//이 다음에 본의 이름을 넣어야 한다. 하지만 {를 한 개 더 열었으므로 임시 포인터 변수로서 보관해야겠지.
		case TOKENR_BONE_NAME:
			break;
		case TOKENR_BONE_PROPERTY:
			// 이 다음 ABSOLUTE가 나오기는 하는데, 쓸 일이 없다.
			break;
			// 다음에는 TM_ROW0~3이 나오는데 역시 무시됨..

		case TOKENR_MESH_WVERTEXS:
			break;

		case TOKENR_MESH_WEIGHT:
		{
			// 버텍스 하나의 정보를 만들어서 리스트에 넣음
			break;
		}
		case TOKENR_BONE_BLENGING_WEIGHT:
		{
			// 대체 몇 단계를 들어가는거야...
			// 가중치 한개를 만들어서 리스트에 넣는다
			/// 헥 헥....
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
			// Face의 번호인데...
			int _index = Parsing_NumberInt();
			// ABC 읽기

			for (int _abc = 0; _abc < 3; _abc++)
			{
				Parsing_String();
				m_OneMesh->m_MeshFaces[_index]->m_VertexIndex[_abc] = Parsing_NumberInt();
			}

			/// (뒤에 정보가 더 있지만 default에 의해 스킵될 것이다.)
			/// ......
			break;
		}
		case TOKENR_MESH_FACENORMAL:
		{
			int _index = Parsing_NumberInt();
			m_OneMesh->m_MeshFaces[_index]->m_Normal = Parsing_NumberVector3();
			/* m_OneMesh->m_meshface[_index]->m_normal.y = Parsing_NumberFloat();
			 m_OneMesh->m_meshface[_index]->m_normal.z = Parsing_NumberFloat();*/

			 // 이건 왜하는거지
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
			// 버텍스의 인덱스가 나오는데 어차피 순서와 같으므로 버린다.
			// 새로운 TVertex를 만들어서 벡터에 넣는다
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
			// 아마도 이건 파일의 끝이 나타났을때인것 같은데. while을 탈출해야 하는데?

			//AfxMessageBox("파일의 끝을 본 것 같습니다!", MB_OK, NULL);
			TRACE("TRACE: 파싱중: 파일의 끝을 봤습니다!\n");
			return;

			break;

			/// 위의 아무것도 해당하지 않을때
		default:
			// 아무것도 하지 않는다.
			break;

		} // end of switch(nowtoken)

		///-----------------------------------------------
		/// 안전 코드.
		i++;
		if (i > 1000000)
		{
			// 루프를 1000000번이상이나 돌 이유가 없다. (데이터가 100000개가 아닌이상)
			// 만약 1000000이상 돌았다면 확실히 뭔가 문제가 있는 것이므로
			TRACE("루프를 백만번 돌았습니다!");
			return;
		}
		///-----------------------------------------------

	} // end of while(nowtoken = m_lexer->GetToken(m_TokenString), nowtoken != TOKEND_BLOCK_END)

	// 여기까지 왔다면 while()을 벗어났다는 것이고 그 말은
	// 괄호를 닫았다는 것이므로 리턴하자 (재귀 끝)

	return;
}


///----------------------------------------------------------------------
/// parsing을 위한 단위별 함수들
///----------------------------------------------------------------------

// long을 읽어서 리턴해준다.
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
	/// ※m_TokenString ( char[255] ) 이기 때문에 CString에 넣으면 에러 날거라 생각했는데, 생각보다 CString은 잘 만들어진 것 같다. 알아서 받아들이는데?
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

// 3개의 Float를 벡터 하나로
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

	return			tempVector3;		// 스태틱 변수의 레퍼런스보다는 값 전달을 하자.
}

///--------------------------------------------------
/// 내부에서 뭔가를 생성, 리스트에 넣는다
///--------------------------------------------------

// 메시를 하나 동적으로 생성하고, 그 포인터를 리스트에 넣는다.
void CASEParser::Create_onemesh_to_list()
{
	Mesh* temp = new Mesh;
	m_OneMesh = temp;
	m_OneMesh->m_SceneData = m_scenedata;		// 클래스간 값 복사
	m_MeshList.push_back(m_OneMesh);
}

// 메트리얼 하나를 동적으로 생성하고, 그 포인터를 리스트에 넣는다.
void CASEParser::Create_materialdata_to_list()
{
	ASEMaterial* temp = new ASEMaterial;
	m_materialdata = temp;
	m_list_materialdata.push_back(m_materialdata);
}

// 애니메이션데이터 하나를 동적으로 생성하고, 그 포인터를 리스트에 넣는다.
//void CASEParser::Create_animationdata_to_list()
//{
//	Animation* temp = new Animation;
//	m_Animation = temp;
//	m_AnimationList.push_back(m_Animation);
//}

// 정점 하나를..
void CASEParser::Create_onevertex_to_list()
{
	Vertex* temp = new Vertex;
	m_OneMesh->m_MeshVertices.push_back(temp);
}

