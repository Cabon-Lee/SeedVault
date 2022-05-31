#pragma once
#include <vector>
#include <fbxsdk.h>
#include "ParsingDataClass.h"
#include "BoneWeights.h"

#ifdef IOS_REF
#undef  IOS_REF
#define IOS_REF (*(pManager->GetIOSettings()))
#endif

enum Status {
	UNLOADED,			//unload file or load file failed
	MUST_BE_LOADED,		//ready for loading file
	MUST_BE_REFRESHED,	//something changed and redraw needed
	REFRESHED			//no redraw needed
};

typedef std::pair<std::string, ParserData::Bone*> BonePair;

struct FBXModel
{
	/// Material Data
	int	m_materialcount;					// Material이 총 몇개인가?
	std::vector<ParserData::CMaterial*> m_list_materialdata;	// Material들의 리스트

	/// Mesh Data
	std::vector<ParserData::Mesh*> m_MeshList;	 		// 메시들

	/// Bone Data
	std::vector<BonePair> m_AllBoneList;

	/// Animation Data
	//bool m_isAnimation;
	//AnimationData* m_OneAnimation;					// 한 오브젝트의 애니메이션 데이터(파싱용)
};


class FBXParser
{
public:
	FBXParser();
	~FBXParser();

private:
	fbxsdk::FbxManager* pManager;
	fbxsdk::FbxImporter* pImporter;
	fbxsdk::FbxScene* pScene;
	fbxsdk::FbxGeometryConverter* pConverter;

	fbxsdk::FbxMesh* pMesh;

	fbxsdk::FbxString fbxFileName;
	std::vector<fbxsdk::FbxSurfaceMaterial*> fbxMaterials;

public:
	FBXModel* m_Model;

	ParserData::CMaterial* m_materialdata;			// Material의 데이터가 들어갈 구조체
	ParserData::Mesh* m_OneMesh;						// 메시 하나의 데이터가 들어갈 구조체
	ParserData::Bone* m_OneBone;
	//AnimationData* m_OneAnimation;					// 한 오브젝트의 애니메이션 데이터(파싱용)

	bool m_Scaling = true;
	bool m_OnlyAni = false;
public:
	void Initalize();
	void SceneSetting();
	void Release();

	void LoadScene(fbxsdk::FbxString fileName, bool scaling, bool onlyAni = false);
	void LoadMaterial();
	void LoadNode(fbxsdk::FbxNode* node, fbxsdk::FbxNodeAttribute::EType attribute);
	//void LoadAnimation(fbxsdk::FbxNode* node);

	//void ProcessSkeleton(fbxsdk::FbxNode* node);
	void ProcessMesh(fbxsdk::FbxNode* node);

	//bool ProcessBoneWeights(fbxsdk::FbxNode* node, std::vector<BoneWeights>& meshBoneWeights);

	//void ProcessAnimation(fbxsdk::FbxNode* node);

	void OptimizeData();
	void OptimizeVertex(ParserData::Mesh* pMesh);
	void RecombinationTM(ParserData::Mesh* pMesh);

	Vector2 ConvertVector2(FbxVector2 v2);
	Vector2 ConvertVector2(FbxVector4 v4);
	Vector3 ConvertVector3(FbxVector4 v4);
	Vector3 NoConvertVector3(FbxVector4 v4);
	Vector4 ConvertVector4(FbxVector4 v4);
	Vector4 NoConvertVector4(FbxVector4 v4);
	XMMATRIX ConvertMatrix(FbxMatrix matrix);
	XMMATRIX NoConvertMatrix(FbxMatrix matrix);
	std::string ConvertFileRoute(const char* fileName);

	Vector3 GetPos(fbxsdk::FbxMesh* mesh, int vertexIndex);
	Vector3 GetNormal(fbxsdk::FbxMesh* mesh, int vertexIndex, int vertexCount);
	Vector2 GetUV(fbxsdk::FbxMesh* mesh, int vertexIndex, int uvIndex);
	void LinkMaterialByPolygon(fbxsdk::FbxMesh* mesh, int polygonIndex, int vertexIndex);

	void SetTransform(fbxsdk::FbxNode* node);
	XMMATRIX GetGlobalAnimationTransform(fbxsdk::FbxNode* node, fbxsdk::FbxTime time);

	int GetMaterialIndex(fbxsdk::FbxSurfaceMaterial* material);
	void SetMaterial(fbxsdk::FbxSurfaceMaterial* material);
	void SetTexture(fbxsdk::FbxSurfaceMaterial* material, const char* materialName);

	void CreateVertex(fbxsdk::FbxMesh* mesh, std::vector<BoneWeights>& boneWeights, int vertexCount);
	void CreateMesh();
	void CreateBone();

	int FindBoneIndex(std::string boneName);
	ParserData::Bone* FindBone(std::string boneName);
	ParserData::Mesh* FindMesh(std::string meshName);

	FBXModel* GetModel() { return m_Model; }
};

inline Vector2 FBXParser::ConvertVector2(FbxVector2 v2)
{
	return Vector2
	(
		static_cast<float>(v2.mData[0]),
		1.0f - static_cast<float>(v2.mData[1])
	);
}

inline Vector2 FBXParser::ConvertVector2(FbxVector4 v4)
{
	return Vector2
	(
		static_cast<float>(v4.mData[0]),
		1.0f - static_cast<float>(v4.mData[1])
	);
}

inline Vector3 FBXParser::ConvertVector3(FbxVector4 v4)
{
	// xyz -> xzy
	return Vector3
	(
		static_cast<float>(v4.mData[0]),
		static_cast<float>(v4.mData[2]),
		static_cast<float>(v4.mData[1])
	);
}

inline Vector3 FBXParser::NoConvertVector3(FbxVector4 v4)
{
	// xyz -> xzy
	return Vector3
	(
		static_cast<float>(v4.mData[0]),
		static_cast<float>(v4.mData[1]),
		static_cast<float>(v4.mData[2])
	);
}

inline Vector4 FBXParser::ConvertVector4(FbxVector4 v4)
{
	// xyzw -> xzyw
	return Vector4
	(
		static_cast<float>(v4.mData[0]),
		static_cast<float>(v4.mData[2]),
		static_cast<float>(v4.mData[1]),
		static_cast<float>(v4.mData[3])
	);
}

inline Vector4 FBXParser::NoConvertVector4(FbxVector4 v4)
{
	// xyzw -> xzyw
	return Vector4
	(
		static_cast<float>(v4.mData[0]),
		static_cast<float>(v4.mData[1]),
		static_cast<float>(v4.mData[2]),
		static_cast<float>(v4.mData[3])
	);
}

inline XMMATRIX FBXParser::ConvertMatrix(FbxMatrix matrix)
{
	FbxVector4 r1 = matrix.GetRow(0);
	FbxVector4 r2 = matrix.GetRow(1);
	FbxVector4 r3 = matrix.GetRow(2);
	FbxVector4 r4 = matrix.GetRow(3);

	// 2행, 3행 변경..
	return XMMATRIX
	(
		ConvertVector4(r1),
		ConvertVector4(r3),
		ConvertVector4(r2),
		ConvertVector4(r4)
	);
}

inline XMMATRIX FBXParser::NoConvertMatrix(FbxMatrix matrix)
{
	FbxVector4 r1 = matrix.GetRow(0);
	FbxVector4 r2 = matrix.GetRow(1);
	FbxVector4 r3 = matrix.GetRow(2);
	FbxVector4 r4 = matrix.GetRow(3);

	// 2행, 3행 변경..
	return XMMATRIX
	(
		ConvertVector4(r1),
		ConvertVector4(r3),
		ConvertVector4(r2),
		NoConvertVector4(r4)
	);
}

inline std::string FBXParser::ConvertFileRoute(const char* fileName)
{
	std::string filePath = fileName;

	size_t indexSlash = filePath.rfind("\\") + 1;
	size_t indexDot = filePath.rfind(".");

	// 파일 임시경로..
	std::string fileRoute = "../Resource/Textures/";

	std::string fileame = filePath.substr(indexSlash, indexDot - indexSlash);
	fileRoute += filePath.substr(indexSlash, filePath.size() - indexSlash);

	return fileRoute;
}
