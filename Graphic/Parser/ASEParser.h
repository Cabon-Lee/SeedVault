//------------------------------------------------------------
//
// ASE Parser
// Ver 2.1.20080124
// 
// 자체적으로 데이터 구조가 있으며 
// 이것을 D3D용 특정 오브젝트로 변환하는 곳까지 커버한다
//
//------------------------------------------------------------

enum eObjectType
{
	eObjectType_ObjectRoot, eObjectType_Geomobject, eObjectType_Shape,
	eObjectType_Camera, eObjectType_Light, eObjectType_HelperObject,
	eObjectType_MarkerObject, eObjectType_SkinnedMesh, eObjectType_Bone,
	eObjectType_CMeshPlane, eObjectType_CBullet_Trace,
	//
	eObjectType_BasicSphere, eObjectType_Missile,
	eObjectType_MeshBillboard, eObjectType_PointSprite,			// CMesh상속..
	eObjectType_End,
};

// 교수님이 제공해주신 Lexer
#include "ASEFile.h"

// 파서 내부에서 쓰는 데이터형
#include "ParsingDataClass.h"
//#include "Animation.h"

#include <list>

using namespace std;
using namespace ParserData;


// 파서 내부에서 특정 모드를 가지고 작동한다. (파서의 재귀구조 때문)
enum eParsingmode
{
	eNone, eGeomobject, eAnimation, eShape, eCamera, eLight, eHelperObject,
	eSkin_InitTM, eBone_List,
	eEnd,
};

/// ASE를 파싱하기 위한 클래스. 기본적으로 주어지는 Lexer클래스를 이용한다.

/// 학생 때의 ASE Parser를 예시용으로 약간 개조했다. 

class CASEParser
{
public:
	CASEParser();
	~CASEParser();


	/// 초기화, 로드
	bool Init();
	bool Load(LPSTR p_File);		// ASE파일을 읽어서, 자체 리스트에 넣는다.

	/// 1차 변환
	// CScenedata 변환
	bool TranslateToD3DFormat_scene(Scenedata* pScene);
	bool VertexOptimization(Mesh* pMesh);		// 버텍스(노말), 텍스쳐 값으로 최적화를 해 준다.
	bool ConvertAll(Mesh* pMesh);			// 모두 그냥 변환 (예시용)

	/// Node TM : worldnodeTM에서 localNodeTM 만 세팅해주고
	/// 나머지 연산(부모타고 localNode를 곱하는 재귀함수)은 계속 업데이트를 해줘야함 
	/// 그래서 meshobject에 메소드를 만들었다
	bool SetLocalTM(Mesh* pMesh);			// Mesh들을 local로 보내기 local setup
	/// 고로 아래 메소드는 여기에 없는게 자연스럽다
	//bool SetNodeTM(Mesh* pMesh);		// NodeTM 세팅하기
	//Matrix GetParentNodeTM(Mesh* pMesh);
	//bool NodeTMAssemble(Mesh* pMesh);

private:
	// 토큰을 구별해내기 위한 스트링
	char m_TokenString[256];
	int m_Token;
	int m_ParsingMode;				// 지금 모드. 재귀 내의 동일 변수때문. nodename이라던지.. row123, mapname등등..


public:
	/// 기본적인 읽어들인 내용들
	int m_data_asciiexport;			// *3DSMAX_ASCIIEXPORT	200 - 맨 처음 나오는 헤더인데 무슨 의미인지는 모르겠고, 일단 예의상 읽어서 보관해준다.
	Scenedata m_scenedata;			// 현재 Scene의 데이터가 들어갈 구조체

	/// 매트리얼 관련
	int	m_materialcount;					// Material이 총 몇개인가?
	ASEMaterial* m_materialdata;			// Material의 데이터가 들어갈 구조체
	list<ASEMaterial*> m_list_materialdata;	// Material들의 리스트

	/// 한 씬에 N개의 메시가 존재할 수 있다.
	Mesh* m_OneMesh;						// 메시 하나의 데이터가 들어갈 구조체
	vector<Mesh*> m_MeshList;				// 메시들

	/// 쉐이프 오브젝트를 파싱할 때 필요한 중간 변수
	//( 파서 내부에서 쉐이프 오브젝트는 GeomObject와 동일하게 취급된다)
	ShapeLine* m_nowshapeline;				// 현재 읽고 있는 Line
	ShapeVertex* m_nowshapevertex;			// 현재 읽고 있는 ShapeVertex

	/// 애니메이션
	//Animation* m_Animation;					// 한 오브젝트의 애니메이션 데이터(파싱용)
	//list<Animation*> m_AnimationList;		// 그 데이터의 리스트


public:
	//--------------------------------------------------
	// Export
	//--------------------------------------------------
	Mesh* GetMesh(int index);


public:
	//--------------------------------------------------
	//	Lexcer
	//--------------------------------------------------
private:
	ASE::CASELexer* m_lexer;


	//--------------------------------------------------
	// Parsing_DivergeRecursiveALL() 내부에서 쓰기 위한 함수들.
	// (parsing을 위한 단위별 함수들)
	//--------------------------------------------------
private:
	/// 파싱을 맡게 되는 거대한 재귀함수. 여기서 모든 토큰의 처리를 한다.
	void Parsing_DivergeRecursiveALL(int depth);

	int Parsing_NumberLong();			// long을 읽어서 리턴해준다.
	float Parsing_NumberFloat();		// float
	LPSTR Parsing_String();				// string
	int Parsing_NumberInt();			// int
	Vector3 Parsing_NumberVector3();	// 3개의 Float를 벡터 하나로 읽어서 리턴해준다.

	void Create_onemesh_to_list();		// 메시를 하나 동적으로 생성하고, 리스트에 넣는다.
	void Create_materialdata_to_list();	// 메트리얼 하나를
	//void Create_animationdata_to_list(); // 애니메이션 데이터(한 개의 오브젝트 대응) 하나를 동적으로 생성하고, 리스트에 넣는다.
	void Create_onevertex_to_list();	// 버텍스 하나를
};
