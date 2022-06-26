#pragma once
// 파싱할 때 노드 데이터 헤더만 알고 있어도 상관없으니
// cpp와 h를 렌더러에서 갖고 있게 하자

#include <windows.h>
#include <d3d11.h>
#include <string>
#include <wrl/client.h>
#include <DirectXColors.h>
#include <DirectXMath.h>
#include "SimpleMath.h"
#include "DataRelease.h"

#include "VertexHelper.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"

class BoundingVolume;
class CenterCreator;
struct RenderQueueElement;
struct RenderQueueParticleElement;

enum class eANIM_TYPE
{
	NONE = 0,	// 애니메이션 없음
	SKINNED,	// 스키닝 애니메이션, 스켈레톤 + 애니메이션이 있다
	MESH,		// 단순 메쉬 애니메이션, no 스켈레톤 + 애니메이션
};

// 애니메이션 클립과 다른 점은 실제 키프레임이 들어가 있다는 점
// 클립에서는 현재 어떤 키프레임에 있는지 정도를 갖고 있다
struct AnimationClipData
{
	eANIM_TYPE animType;

	std::string						m_ModelName;
	std::string						m_NodeName;
	std::string						m_AnimationName;

	// 한 프레임의 틱 크기. 
	// AnimationTable을 가지는 각각의 오브젝트마다 다를 수 있다.
	float	m_TicksPerFrame = 0.0f;
	// 한 애니메이션이 가지고 있는 keyFrame의 양
	unsigned int m_TotalKeyFrames = 0;

	// 수종이 정의한 멤버 변수
	unsigned int m_NowKeyFrame = 0;
	unsigned int m_LastKeyFrame = 0;
	unsigned int m_FirstKeyFrame = 0;

	// 확실히 보장하려면 Bone 정보도 넣으면 될듯
	// 본 인덱스에 매트릭스 벡터가 들어가있는 형태
	std::vector<std::vector<DirectX::SimpleMath::Matrix>> keyFrames_V;
};

struct GraphicMaterial
{
	GraphicMaterial()
		: materialName()
		, vertexShader()
		, vertexShaderIndex(0)
		, pixelShader()
		, pixelShaderIndex(0)
		, albedoColor()
		, ambientColor()
		, fresnelR0()
		, transparentFactor(0.0f)
		, specularPower(0.0f)
		, smoothness(0.0f)
		, reflactionFactor(0.0f)
		, metallic(0.0f)
		, normalFactor(1.0f)
		, emissiveFactor(1.0f)
		, isTransparency(false)
	{
		// 매직넘버 -1을 넣어서 -1인 경우에는 null을 PS에 넘긴다
		memset(textureArray, -1, sizeof(textureArray));
	}

	std::string materialName;		// ★

	std::string vertexShader;
	unsigned int vertexShaderIndex;

	std::string pixelShader;
	unsigned int pixelShaderIndex;

	std::string albedoMap;			// ★
	std::string normalMap;			// ★
	std::string metalRoughMap;		// ★
	std::string emissiveMap;		// ★

	int textureArray[4];

	DirectX::SimpleMath::Vector4 albedoColor;
	DirectX::SimpleMath::Vector4 ambientColor;
	DirectX::SimpleMath::Vector3 fresnelR0;			// 베이스 컬러(반사에 사용됨)

	float transparentFactor;
	float specularPower;
	float metallic;									// ★
	float smoothness;								// ★ 블렌더 상 Roughness를 의미 
	float normalFactor;								// ★ 
	float emissiveFactor;							// ★ 
	float reflactionFactor;

	bool isTransparency;							// ★ 
};

struct NodeData
{
	std::string nodeName;

	std::shared_ptr<NodeData> pParentNode;
	std::vector<std::shared_ptr<NodeData>> pChildren_V;

	std::string materialName;
	std::shared_ptr<GraphicMaterial> pMaterial;

	Microsoft::WRL::ComPtr<ID3D11Buffer> m_VertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_IndexBuffer;

	bool isExistParent;
	int parentIndex = -1;

	DirectX::SimpleMath::Matrix meshLocalTM;
	DirectX::SimpleMath::Matrix meshWorldTM;

	DirectX::SimpleMath::Matrix nodeTM;
	DirectX::SimpleMath::Matrix worldTM;
	DirectX::SimpleMath::Matrix animationTM;

	UINT m_Stride;
	UINT m_IndexCount;

	std::unique_ptr<BoundingVolume> m_BoundingVolume;

	RenderQueueElement* m_pTempOpaqueRenderQueueElement;
	RenderQueueElement* m_pTempTrasparentRenderQueueElement;


	void FindParentNode();
	void CalculateTransform();
	void CalculateTransform(const DirectX::XMMATRIX& worldTM);
	void CalculateAnimTransform();

	ID3D11Buffer* const* GetVertexAddressOf()
	{
		return m_VertexBuffer.GetAddressOf();
	}

	Microsoft::WRL::ComPtr<ID3D11Buffer> GetVertexBuffer()
	{
		return m_VertexBuffer;
	}

	Microsoft::WRL::ComPtr<ID3D11Buffer> GetIndexBuffer()
	{
		return m_IndexBuffer;
	}

	const UINT* StridePtr() const
	{
		return &this->m_Stride;
	}
};

struct BoneNodeData
{
	std::string boneName;
	int boneIndex;

	BoneNodeData* pParentBone;
	int parentBoneIndex;
	std::vector<BoneNodeData*> pChildren_V;

	DirectX::SimpleMath::Matrix nodeTM;
	DirectX::SimpleMath::Matrix worldTM;
	DirectX::SimpleMath::Matrix bindPoseTM;
	DirectX::SimpleMath::Matrix animationTM;

	// BoneOffSetTM을 들고 있기 위한 새로운 변수
	DirectX::SimpleMath::Matrix nowBoneOffsetTM;

	void CalculateTransform();
	void CalculateAnimTransform();
};

/// 본들의 집합을 가지고 있을 스켈레톤
/// SkinningMeshRenderer 등에 포함될 예정
struct SkeletonData
{
	std::vector<BoneNodeData*> pBones_V;
	std::vector<BoneNodeData*> pTorso_V;

	std::vector<DirectX::SimpleMath::Matrix> boneOffsetTM_V;

	void SortingTorsoLeg();

	// 메쉬 그리기 이전에 이게 먼저 불려져야함
	void BindPose();
	void BindPose(const DirectX::SimpleMath::Matrix& worldTM);

	void CalculateTransform(BoneNodeData* pChild);
	void CalculateAnimTransform(BoneNodeData* pChild);
	void KeyFrameAnimate(
		const DirectX::SimpleMath::Matrix& worldTM,
		unsigned int animIndex,
		unsigned int keyFrame);

	void AnimateByAnimationClip(
		std::shared_ptr<AnimationClipData> animationClip,
		const DirectX::SimpleMath::Matrix& worldTM,
		unsigned int animIndex,
		DirectX::SimpleMath::Matrix* pPrevWorld,
		float offsetAngle = 0.0f);

	void AnimationCrossFading(
		std::shared_ptr<AnimationClipData> easeInAnimClip,
		std::shared_ptr<AnimationClipData> easeOutAnimClip,
		const DirectX::SimpleMath::Matrix& worldTM,
		DirectX::SimpleMath::Matrix* pPrevWorld,
		unsigned int easeInAnimIndex,
		unsigned int easeOutAnimIdex,
		float blending,
		float offsetAngle = 0.0f);

	// AnimationClipData에 있는 것과는 달리 이미 모든 연산이 끝난 PrevAnimationTM을 사용한다
	void CrossFadingByPrevAnimatoinTM(
		std::shared_ptr<AnimationClipData> easeInAnimClip,
		const DirectX::SimpleMath::Matrix& worldTM,
		DirectX::SimpleMath::Matrix* pPrevWorld,
		DirectX::SimpleMath::Matrix* pInterpolateTM,
		unsigned int easeInAnimIndex,
		float blending);

	void MaskingAnimation(
		std::shared_ptr<AnimationClipData> targetAnimClip,
		const DirectX::SimpleMath::Matrix& worldTM,
		unsigned int targetAnimIndex);

	void MaskingAnimation(
		std::shared_ptr<AnimationClipData> targetAnimClip,
		float alpha);

	void MaskingAnimation(
		std::shared_ptr<AnimationClipData> targetBaseAnimClip,
		std::shared_ptr<AnimationClipData> targetSubAnimClip,
		float alpha,
		unsigned int targetAnimIndex,
		const DirectX::SimpleMath::Matrix& worldTM
	);


	void SetTorsoSkeletalIndex(unsigned int parentIndex, std::vector<BoneNodeData*>& bones)
	{
		// 맨처음 자기자신을 넣어준다
		for (BoneNodeData* _nowBone : pBones_V)
		{
			if (_nowBone->boneIndex == parentIndex)
			{
				bones.push_back(_nowBone);
				break;
			}
		}

		SetParentIndex(parentIndex, bones);
	}

	void SetParentIndex(unsigned int parentIndex,
		std::vector<BoneNodeData*>& bones)
	{
		// 그다음 해당 인덱스를 부모로하는 본들을 차례로 집어넣어준다
		for (BoneNodeData* _nowBone : pBones_V)
		{
			if (_nowBone->parentBoneIndex == parentIndex)
			{
				bones.push_back(_nowBone);
				SetParentIndex(_nowBone->boneIndex, bones);
			}
		}
	}


};

struct SpriteData
{
	SpriteData() : textureName(), width(0.0f), height(0.0f), depth(0.0f), isHide(false)
	{
		m_pTempRenderQueueEffectElement = nullptr;
	}
	SpriteData(
		const std::string& name,
		float w,
		float h,
		float d,
		const Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& shaderResource)
		: textureName(name), width(w), height(h), depth(d), isHide(false), shaderResource(shaderResource)
	{
		m_pTempRenderQueueEffectElement = nullptr;
	}


	std::string textureName;

	float width;
	float height;
	float depth = 0.0f;

	bool isHide;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResource;
	RenderQueueParticleElement* m_pTempRenderQueueEffectElement;

	void SetSpriteData(
		const std::string& textureName,
		float width, float height, float depth, const Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& shaderResource)
	{
		this->shaderResource = shaderResource;
		this->textureName = textureName;
		this->width = width;
		this->height = height;
		this->depth = depth;
	}

	void SetHide(bool val)
	{
		isHide = val;
	}

	bool GetHide()
	{
		return isHide;
	}
};

/// 지금 진미의 FBXModel과 ModelMesh의 역할이 모호하다
/// 둘 중 하나를 진지하게 파던지, 두 개의 역할을 명확하게 구분할 필요가 있다
/// 일단은 진행했다
/// 실제로 그림을 그리는데 사용되는 클래스
class ModelMesh
{
public:
	ModelMesh()
		: m_pNodeData_V(nullptr)
		, m_pSkeleton(nullptr)
		, m_IsMeshAnimation(false)
		, m_IsSkinnedAnimation(false) {};
	~ModelMesh() {
		VectorDataRelese(*m_pNodeData_V);
		delete m_pNodeData_V;
		m_pSkeleton.reset();
	};

	bool m_IsSkinnedAnimation;	// 본과 스키닝 정보가 있을 때
	bool m_IsMeshAnimation;		// 단순 메쉬 애니메이션만 있을때

	// 살점
	std::vector<std::shared_ptr<NodeData>>* m_pNodeData_V;
	std::vector<std::shared_ptr<GraphicMaterial>> m_pGraphicMaterial_V;

	// 원래 애니메이션 정보가 있었으나 그러면 안됨
	// 메쉬와 애니메이션은 따로 존재하는게 맞음
	// 해골
	std::unique_ptr<SkeletonData> m_pSkeleton;
	std::vector<std::string> m_AnimationName_V;

	void MeshAnimation(
		std::shared_ptr<AnimationClipData> animationClip,
		const DirectX::SimpleMath::Matrix& worldTM,
		unsigned int animIndex);

	void CalculateAnimTransform(std::shared_ptr<NodeData> pChild);

	void CalculateBoneOffsetTM(std::vector<DirectX::SimpleMath::Matrix>& boneTM);
	void CalculateBoneOffsetTM(int boneIndex, DirectX::XMMATRIX& boneTM);
	void CalculateBoneOffsetTM(DirectX::XMMATRIX* offsetArray);
	void CalculateBoneOffsetTM();
	void SetBoneOffsetToBuffer(DirectX::XMMATRIX* offsetArray);

	DirectX::SimpleMath::Matrix* GetBoneWorldPostion(const std::string& name)
	{
		for (auto _nowBone : m_pSkeleton->pBones_V)
		{
			// mixamorig:를 제외한 나머지를 비교, 하드코딩
			if (_nowBone->boneName.compare(10, name.size(), name) == 0)
				return &_nowBone->worldTM;
		}

		return nullptr;
	}

	DirectX::SimpleMath::Matrix* GetBoneWorldPostion(unsigned int boneIndex)
	{
		return &m_pSkeleton->pBones_V.at(boneIndex)->animationTM;
	}

private:
	DirectX::SimpleMath::Matrix m_NowMat;
	DirectX::SimpleMath::Matrix m_BindPose;
	DirectX::SimpleMath::Matrix m_BindPoseInv;
	DirectX::SimpleMath::Matrix m_NowBoneWorldTM;

};

