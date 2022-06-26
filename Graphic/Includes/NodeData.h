#pragma once
// �Ľ��� �� ��� ������ ����� �˰� �־ ���������
// cpp�� h�� ���������� ���� �ְ� ����

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
	NONE = 0,	// �ִϸ��̼� ����
	SKINNED,	// ��Ű�� �ִϸ��̼�, ���̷��� + �ִϸ��̼��� �ִ�
	MESH,		// �ܼ� �޽� �ִϸ��̼�, no ���̷��� + �ִϸ��̼�
};

// �ִϸ��̼� Ŭ���� �ٸ� ���� ���� Ű�������� �� �ִٴ� ��
// Ŭ�������� ���� � Ű�����ӿ� �ִ��� ������ ���� �ִ�
struct AnimationClipData
{
	eANIM_TYPE animType;

	std::string						m_ModelName;
	std::string						m_NodeName;
	std::string						m_AnimationName;

	// �� �������� ƽ ũ��. 
	// AnimationTable�� ������ ������ ������Ʈ���� �ٸ� �� �ִ�.
	float	m_TicksPerFrame = 0.0f;
	// �� �ִϸ��̼��� ������ �ִ� keyFrame�� ��
	unsigned int m_TotalKeyFrames = 0;

	// ������ ������ ��� ����
	unsigned int m_NowKeyFrame = 0;
	unsigned int m_LastKeyFrame = 0;
	unsigned int m_FirstKeyFrame = 0;

	// Ȯ���� �����Ϸ��� Bone ������ ������ �ɵ�
	// �� �ε����� ��Ʈ���� ���Ͱ� ���ִ� ����
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
		// �����ѹ� -1�� �־ -1�� ��쿡�� null�� PS�� �ѱ��
		memset(textureArray, -1, sizeof(textureArray));
	}

	std::string materialName;		// ��

	std::string vertexShader;
	unsigned int vertexShaderIndex;

	std::string pixelShader;
	unsigned int pixelShaderIndex;

	std::string albedoMap;			// ��
	std::string normalMap;			// ��
	std::string metalRoughMap;		// ��
	std::string emissiveMap;		// ��

	int textureArray[4];

	DirectX::SimpleMath::Vector4 albedoColor;
	DirectX::SimpleMath::Vector4 ambientColor;
	DirectX::SimpleMath::Vector3 fresnelR0;			// ���̽� �÷�(�ݻ翡 ����)

	float transparentFactor;
	float specularPower;
	float metallic;									// ��
	float smoothness;								// �� ���� �� Roughness�� �ǹ� 
	float normalFactor;								// �� 
	float emissiveFactor;							// �� 
	float reflactionFactor;

	bool isTransparency;							// �� 
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

	// BoneOffSetTM�� ��� �ֱ� ���� ���ο� ����
	DirectX::SimpleMath::Matrix nowBoneOffsetTM;

	void CalculateTransform();
	void CalculateAnimTransform();
};

/// ������ ������ ������ ���� ���̷���
/// SkinningMeshRenderer � ���Ե� ����
struct SkeletonData
{
	std::vector<BoneNodeData*> pBones_V;
	std::vector<BoneNodeData*> pTorso_V;

	std::vector<DirectX::SimpleMath::Matrix> boneOffsetTM_V;

	void SortingTorsoLeg();

	// �޽� �׸��� ������ �̰� ���� �ҷ�������
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

	// AnimationClipData�� �ִ� �Ͱ��� �޸� �̹� ��� ������ ���� PrevAnimationTM�� ����Ѵ�
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
		// ��ó�� �ڱ��ڽ��� �־��ش�
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
		// �״��� �ش� �ε����� �θ���ϴ� ������ ���ʷ� ����־��ش�
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

/// ���� ������ FBXModel�� ModelMesh�� ������ ��ȣ�ϴ�
/// �� �� �ϳ��� �����ϰ� �Ĵ���, �� ���� ������ ��Ȯ�ϰ� ������ �ʿ䰡 �ִ�
/// �ϴ��� �����ߴ�
/// ������ �׸��� �׸��µ� ���Ǵ� Ŭ����
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

	bool m_IsSkinnedAnimation;	// ���� ��Ű�� ������ ���� ��
	bool m_IsMeshAnimation;		// �ܼ� �޽� �ִϸ��̼Ǹ� ������

	// ����
	std::vector<std::shared_ptr<NodeData>>* m_pNodeData_V;
	std::vector<std::shared_ptr<GraphicMaterial>> m_pGraphicMaterial_V;

	// ���� �ִϸ��̼� ������ �־����� �׷��� �ȵ�
	// �޽��� �ִϸ��̼��� ���� �����ϴ°� ����
	// �ذ�
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
			// mixamorig:�� ������ �������� ��, �ϵ��ڵ�
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

