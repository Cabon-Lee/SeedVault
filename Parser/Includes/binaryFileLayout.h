#pragma once
#include <vector> 
#include <typeinfo>
#include "VertexHelper.h"
#include "MaterialData.h"
#include "simpleMath.h"

#pragma region FBXModel

struct Float4x4
{
	union
	{
		struct
		{
			float m00, m01, m02, m03;
			float m10, m11, m12, m13;
			float m20, m21, m22, m23;
			float m30, m31, m32, m33;
		};
		float m[4][4];
	};
};

struct Float4x4Func : public Float4x4
{
	Float4x4Func() {};
	Float4x4Func(const Float4x4& float4x4)
	{
		for (int i = 0; i < 4; ++i)
		{
			for (int j = 0; j < 4; ++j)
			{
				this->m[i][j] = float4x4.m[i][j];
			}
		}
	}
	Float4x4Func(float f00, float f01, float f02, float f03,
		float f10, float f11, float f12, float f13,
		float f20, float f21, float f22, float f23,
		float f30, float f31, float f32, float f33)
	{
		this->m00 = f00; this->m01 = f01; this->m02 = f02; this->m03 = f03;

		this->m10 = f10; this->m11 = f11;
		this->m12 = f12;
		this->m13 = f13;

		this->m20 = f20;
		this->m21 = f21;
		this->m22 = f22;
		this->m23 = f23;

		this->m30 = f30;
		this->m31 = f31;
		this->m32 = f32;
		this->m33 = f33;
	}
	Float4x4& operator=(const Float4x4& float4x4)
	{
		for (int i = 0; i < 4; ++i)
		{
			for (int j = 0; j < 4; ++j)
			{
				this->m[i][j] = float4x4.m[i][j];
			}
		}

		return *this;
	}
	Float4x4& operator=(const DirectX::SimpleMath::Matrix& float4x4)
	{
		for (int i = 0; i < 4; ++i)
		{
			for (int j = 0; j < 4; ++j)
			{
				this->m[i][j] = float4x4.m[i][j];
			}
		}

		return *this;
	}
	Float4x4 operator+(Float4x4& float4x4)
	{
		for (int i = 0; i < 4; ++i)
		{
			for (int j = 0; j < 4; ++j)
			{
				this->m[i][j] += float4x4.m[i][j];
			}
		}

		return *this;
	}
	Float4x4 operator-(Float4x4& float4x4)
	{
		for (int i = 0; i < 4; ++i)
		{
			for (int j = 0; j < 4; ++j)
			{
				this->m[i][j] -= float4x4.m[i][j];
			}
		}

		return *this;
	}
	bool operator==(Float4x4& float4x4)
	{
		if (this->m00 == float4x4.m00 && this->m01 == float4x4.m01 && this->m02 == float4x4.m02 && this->m03 == float4x4.m03
			&& this->m10 == float4x4.m10 && this->m11 == float4x4.m11 && this->m12 == float4x4.m12 && this->m13 == float4x4.m13
			&& this->m20 == float4x4.m20 && this->m21 == float4x4.m21 && this->m22 == float4x4.m22 && this->m23 == float4x4.m23
			&& this->m30 == float4x4.m30 && this->m31 == float4x4.m31 && this->m32 == float4x4.m32 && this->m33 == float4x4.m33)
		{
			return true;
		}
		return false;

	}
	bool operator!=(Float4x4& float4x4)
	{
		if (this->m00 != float4x4.m00 || this->m01 != float4x4.m01 || this->m02 != float4x4.m02 || this->m03 != float4x4.m03
			|| this->m10 != float4x4.m10 || this->m11 != float4x4.m11 || this->m12 != float4x4.m12 || this->m13 != float4x4.m13
			|| this->m20 != float4x4.m20 || this->m21 != float4x4.m21 || this->m22 != float4x4.m22 || this->m23 != float4x4.m23
			|| this->m30 != float4x4.m30 || this->m31 != float4x4.m31 || this->m32 != float4x4.m32 || this->m33 != float4x4.m33)
		{
			return true;
		}
		return false;
	}
};

/// ---------------------------------------------------
/// 모델 구조체
/// ---------------------------------------------------
/// 헤더-----------------------------------------------
struct FBXModelBinaryFileHeader
{
	// 파일타입 시그니처
	char mSignature[4] = { 0, };

	// 매쉬
	int meshCount = 0;

	std::vector<int> vertexCount_V;
	std::vector<int> indexCount_V;

	VertexType vertexType;

	// 머테리얼
	bool isMaterial = false;
	size_t materialCount = 0;								// 머테리얼 개수
	std::vector<int> materialNameLength_V;					// 머테리얼의 이름의 길이
	std::vector<int> textureNameCount_V;					// 머테리얼에 종속되어있는 텍스쳐들의 갯수
	std::vector<std::vector<int>> textureNameLength_V;		// 머테리얼에 종속되어있는 텍스쳐들의 이름의 길이
	std::vector<int> materialMapping_V;						// 머테리얼의 인덱스


	// 스켈레톤(Bone)
	bool isSkeleton = false;
	size_t boneCount = 0;									// 본 개수
	size_t boneOffsetCount = 0;								// 본오프셋 개수
	std::vector<int> boneNameLength_V;						// 뼈이름 길이

	// 애니메이션 정보
	bool isSkinnedAnimation = false;
	bool isMeshAnimation = false;
	size_t aniCount;										// 애니메이션 개수
	std::vector<int> aniNameLength;							// 애니 이름 길이
	std::vector<int> animationKeyFrameCount_V;				// 애니 키프레임 갯수
};

/// 데이터---------------------------------------------
struct AnimationBinaryData
{
	AnimationBinaryData() :
		animationName(""), ticksPerFrame(0), totalKeyFrames(0), animationKeyFrame(0)
	{

	}

	~AnimationBinaryData() {}

	std::string								animationName;
	float									ticksPerFrame;
	unsigned int							totalKeyFrames;
	std::vector<std::vector<Float4x4>>		animationKeyFrame;
};

struct BoneBinaryData
{
	BoneBinaryData()
		: boneName(""), boneIndex(0), parentBoneIndex(0), bindPoseTransform(Float4x4()), worldTM(Float4x4()), localTM(Float4x4())
	{}

	~BoneBinaryData() {}

	std::string		boneName;
	int				boneIndex;
	int				parentBoneIndex;

	Float4x4		bindPoseTransform;
	Float4x4		worldTM;
	Float4x4		localTM;
};

struct SkeletonBinaryData
{
	std::vector<BoneBinaryData> bone_V;
	std::vector<Float4x4> boneOffset_V;
};

struct MeshBinaryFileData
{
	std::vector<Vertex::SerializerVertex> vertex_V;
	std::vector<DWORD> index_V;

	int meshIndex;
	bool isParent;
	int parentIndex;

	Float4x4		meshWorldTM;
	Float4x4		meshLocalTM;
};

struct FBXModelBinaryFileData
{
	std::vector<MeshBinaryFileData> mesh_V;
	std::vector<MaterialDesc> material_V;

	SkeletonBinaryData skeleton;
	std::vector<AnimationBinaryData> animation_V;
};

struct FBXModelBinaryFile
{
	FBXModelBinaryFileHeader binHeader;
	FBXModelBinaryFileData binData;
};

/// ---------------------------------------------------
/// 애니메이션 구조체
/// ---------------------------------------------------
struct AnimFileHeader
{
	// 애니메이션 정보
	bool isSkinnedAnimation = false;
	bool isMeshAnimation = false;
	bool isSkeleton = false;
	size_t boneCount = 0;					// 뼈 개수
	size_t meshCount = 0;					// 머테리얼 개수
	int aniNameLength;						// 애니 이름 길이
	int animationKeyFrameCount;				// 애니 키프레임 갯수
};

struct AnimFileForWrite
{
	std::vector<AnimFileHeader> animHeader;
	std::vector <AnimationBinaryData> animData;
};

struct AnimFileForRead
{
	AnimFileHeader animHeader;
	AnimationBinaryData animData;
};

/// ---------------------------------------------------
/// 머테리얼 구조체
/// ---------------------------------------------------

struct MatFileHeader
{
	int materialNameLength;

	int albedoMapNameLength;
	int normalMapNameLength;
	int metalRoughMapNameLength;
	int emissiveMapNameLength;
};

struct MatFileData
{
	std::string materialName;						// ★

	std::string albedoMap;							// ★
	std::string normalMap;							// ★
	std::string metalRoughMap;						// ★
	std::string emissiveMap;						// ★

	float metallic;									// ★
	float shineness;								// ★ 
	float normalFactor;
	float emissiveFactor;

	bool isTransparency;							// ★ 

};

struct MatFile
{
	MatFileHeader matHeader;
	MatFileData matData;
};

#pragma endregion FBXModel
