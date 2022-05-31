#pragma once
/// �ļ��� �ڽ��� ����� Mesh�� ��� ������ ���� �ȴ�.
/// �׳� �ڽ��� �˰��ִ� ParsingData�� �ִ´�θ� �����Ѵ�.
/// �� ParsingData�� DX ���˿� �ð� �����ϴ� ���� �Ŵ����� ����

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
	NONE = 0,	// �ִϸ��̼� ����
	SKINNED,	// ��Ű�� �ִϸ��̼�, ���̷��� + �ִϸ��̼��� �ִ�
	MESH,		// �ܼ� �޽� �ִϸ��̼�, no ���̷��� + �ִϸ��̼�
};

// �ļ� �ٱ����� ���������� ����ü
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
	
	// Parser���� Temp�� ������� Skeleton�� ����ϰ� �Ѱ��ֱ� ���� unique_ptr�� ���
	// unique_ptr�� �迭����, ���� �ÿ��� ���� �ٸ� ������� �����ؾ� �Ѵ�

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

	// Fbx�� ������ ������ FbxModel�� MaterialMap���� �����ش�
	void LoadScene(
		std::string fileName, 
		std::shared_ptr<FBXModel>, 
		std::unordered_map<std::string, MaterialDesc*>& outMaterialMap);

	void LoadScene(std::string& fileName, std::string& writePath);

private:
	// ����
	void SceneSetting();

	// ��з�
	void LoadNode(fbxsdk::FbxNode* node, FbxNodeAttribute::EType attribute);
	void LoadAnimations();
	void LoadMaterial(std::unordered_map<std::string, MaterialDesc*>& outMaterialMap);
	void LoadMaterial(std::vector<MaterialDesc>& outMaterials);

	// ���� ������ �ִ� �ܰ�
	void ProcessMesh(fbxsdk::FbxNode* node);
	void ProcessSkeleton(fbxsdk::FbxNode* node);
	void ProcessAnimation(
		fbxsdk::FbxNode* node, 
		std::string animationName, 
		unsigned int animationIndex,
		float frameRate, float start, float stop);

	bool ProcessBoneWeights(fbxsdk::FbxNode* node, std::vector<BoneWeights>& meshBoneWeights);

	void SetTransform(fbxsdk::FbxNode* node, JMParserData::Bone* bone);

	// ���ؽ� ����
	void CreateVertex(fbxsdk::FbxMesh* mesh, std::vector<BoneWeights>& boneWeights, int vertexCount);
	void ProcessMeshVertexInfo();

	// ���ؽ� ���ο� �� Face ���� ����
	unsigned int ProcessMeshVertexIndex();
	JMParserData::Float3 ReadNormal(const FbxMesh* mesh, int controlPointIndex, int vertexCounter);
	JMParserData::Float2 ReadUV(const FbxMesh* mesh, int controlPointIndex, int vertexCounter);
	JMParserData::Float3 ReadTangent(const FbxMesh* mesh, int controlPointIndex, int vertexCounter);
	JMParserData::Float3 RadeBinormal(const FbxMesh* mesh, int controlPointIndex, int vertexCounter);
	void MakeTangentSpace(const FbxMesh* mesh, int polygonIndex);

	// Material ����
	void GetMaterialTexture(FbxSurfaceMaterial* mat, MaterialDesc& outDesc);
	void LoadTextureProperty(FbxProperty& prop, MaterialDesc& outDesc);
	void LoadTextureToMaterial(std::string propertyName, FbxFileTexture* fileTexture, MaterialDesc& outDesc);
	
	// ���ؽ� ���ø�
	void SplitMesh();
	void SplitMesh2();

	// ��� �Լ�
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
	fbxsdk::FbxMesh* m_pFBXmesh;							// FBX�� �Ž�
	fbxsdk::FbxString m_fbxFileName;						// �����̸�

	std::shared_ptr<FBXModel> m_pFBXModel;					// ���ϳ� ����
	std::shared_ptr<JMParserData::Mesh> m_pMesh;			// �ļ��� �Ž�

	std::unique_ptr<JMParserData::Skeleton> m_pSkeleton;
	std::vector<DirectX::SimpleMath::Matrix> m_BoneOffsets;

	bool m_HasSkeleton;

	int indexDebug = 0;

};

