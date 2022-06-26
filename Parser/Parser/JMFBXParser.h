#pragma once
/// 파서는 자신이 내뱉는 Mesh가 어떻게 변할지 몰라도 된다.
/// 그냥 자신이 알고있는 ParsingData에 있는대로만 전달한다.
/// 그 ParsingData를 DX 포맷에 맡게 변경하는 것은 매니저가 할일

#include <vector>
#include <queue>
#include <map>
#include <unordered_map>

#include <fbxsdk.h>

#include <memory>

#include "JMParsingDataClass.h"
#include "AnimationData.h"


#define LEGACY

class BoneWeights;
struct MaterialDesc;

enum class eANIM_TYPE
{
	NONE = 0,	// 애니메이션 없음
	SKINNED,	// 스키닝 애니메이션, 스켈레톤 + 애니메이션이 있다
	MESH,		// 단순 메쉬 애니메이션, no 스켈레톤 + 애니메이션
};

// 파서 바깥으로 내보내지는 구조체
struct FBXModel
{
	FBXModel();
	~FBXModel();

	std::string fileName;
	std::vector<std::string> materialName_V;
	std::vector<std::shared_ptr<JMParserData::Mesh>> pMesh_V;

	eANIM_TYPE animationType;

	bool isSkinnedAnimation;
	bool isMeshAnimation;

	unsigned int animationCount;
	
	// Parser에서 Temp로 만들어진 Skeleton을 깔끔하게 넘겨주기 위해 unique_ptr을 썼다
	// unique_ptr의 배열형태, 생성 시에도 전혀 다른 방법으로 생성해야 한다

	std::vector<std::shared_ptr<AnimationData>> pAnimation_V;
	std::unique_ptr<JMParserData::Skeleton> pSkeleton;
	std::vector<std::string> animationName_V;

	std::shared_ptr<JMParserData::Mesh> GetMesh(const std::string& name)
	{
		for (auto& _nowMesh : pMesh_V)
		{
			if (name == _nowMesh->nodeName)
			{
				return _nowMesh;
			}
		}
		return nullptr;
	}
};

class JMFBXParser
{
public:
	JMFBXParser();
	~JMFBXParser();

	void Initalize();
	void Destroy();

	// Fbx를 넣으면 진미의 FbxModel과 MaterialMap으로 돌려준다
	void LoadScene(
		std::string fileName, 
		std::shared_ptr<FBXModel>, 
		std::unordered_map<std::string, MaterialDesc*>& outMaterialMap);

	void LoadScene(std::string& fileName, std::string& writePath);

private:
	// 공통
	void SceneSetting();

	// 대분류
	void LoadNode(fbxsdk::FbxNode* node, FbxNodeAttribute::EType attribute);
	void LoadAnimations();
	void LoadMaterial(std::unordered_map<std::string, MaterialDesc*>& outMaterialMap);
	void LoadMaterial(std::vector<MaterialDesc>& outMaterials);

	// 실제 정보를 넣는 단계
	void ProcessMesh(fbxsdk::FbxNode* node);
	void ProcessSkeleton(fbxsdk::FbxNode* node);
	void ProcessAnimation(
		fbxsdk::FbxNode* node, 
		std::string animationName, 
		unsigned int animationIndex,
		float frameRate, float start, float stop);

	bool ProcessBoneWeights(fbxsdk::FbxNode* node, std::vector<BoneWeights>& meshBoneWeights);

	void SetTransform(fbxsdk::FbxNode* node, JMParserData::Bone* bone);

	// 버텍스 관련
	void CreateVertex(fbxsdk::FbxMesh* mesh, std::vector<BoneWeights>& boneWeights, int vertexCount);
	void ProcessMeshVertexInfo();

	// 버텍스 내부에 들어갈 Face 관련 정보
	unsigned int ProcessMeshVertexIndex();
	JMParserData::Float3 ReadNormal(const FbxMesh* mesh, int controlPointIndex, int vertexCounter);
	JMParserData::Float2 ReadUV(const FbxMesh* mesh, int controlPointIndex, int vertexCounter);
	JMParserData::Float3 ReadTangent(const FbxMesh* mesh, int controlPointIndex, int vertexCounter);
	JMParserData::Float3 RadeBinormal(const FbxMesh* mesh, int controlPointIndex, int vertexCounter);
	void MakeTangentSpace(const FbxMesh* mesh, int polygonIndex);
	void MakeBinormal(int polygonIndex);

	// Material 관련
	void GetMaterialTexture(FbxSurfaceMaterial* mat, MaterialDesc& outDesc);
	void LoadTextureProperty(FbxProperty& prop, MaterialDesc& outDesc);
	void LoadTextureToMaterial(std::string propertyName, FbxFileTexture* fileTexture, MaterialDesc& outDesc);
	
	// 버텍스 스플릿
	void SplitMesh();
	void SplitMesh2();

	// 기능 함수
	DirectX::SimpleMath::Matrix ConvertMatrix(FbxMatrix fbxMatrix);
	DirectX::SimpleMath::Vector4 ConvertVector4(FbxVector4 v4);

	DirectX::SimpleMath::Matrix AniConvertMatrix(FbxMatrix fbxMatrix);
	DirectX::SimpleMath::Vector4 AniConvertVector4(FbxVector4 v4);

	DirectX::SimpleMath::Matrix GetLocalTransformFromCurrentTake(FbxNode* node, FbxTime time);


private:
	fbxsdk::FbxManager* m_pManager;
	fbxsdk::FbxImporter* m_pImporter;
	fbxsdk::FbxScene* m_pScene;
	fbxsdk::FbxGeometryConverter* m_pGeometryConverter;
	fbxsdk::FbxMesh* m_pFBXmesh;							// FBX의 매쉬
	fbxsdk::FbxString m_fbxFileName;						// 파일이름

	std::shared_ptr<FBXModel> m_pFBXModel;					// 모델하나 단위
	std::shared_ptr<JMParserData::Mesh> m_pMesh;			// 파서의 매쉬

	std::unique_ptr<JMParserData::Skeleton> m_pSkeleton;
	std::vector<DirectX::SimpleMath::Matrix> m_BoneOffsets;

	bool m_HasSkeleton;

	int indexDebug = 0;

};

