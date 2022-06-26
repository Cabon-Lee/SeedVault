#pragma once
#include "align16.h"
#include <DirectXMath.h>

struct CB_VS_WVPMatrix : public AlignedAllocationPolicy<16>
{
	DirectX::XMMATRIX wvpMatrix;
};

struct CB_VS_ShadowTM : public AlignedAllocationPolicy<16>
{
	DirectX::XMMATRIX shadowTM[96];
};

struct CB_VS_vertexshader : public AlignedAllocationPolicy<16>
{
	DirectX::XMMATRIX wvpMatrix;
	DirectX::XMMATRIX worldMatrix;
};

struct CB_PS_light : public AlignedAllocationPolicy<16>
{
	DirectX::XMFLOAT3 lightDir;
	float pad;
	DirectX::XMFLOAT3 ambientLightColor;
	float ambientLightStrength;
};

// skyShpere용
struct CB_VS_WVP : public AlignedAllocationPolicy<16>
{
	DirectX::XMMATRIX worldMatrix;
	DirectX::XMMATRIX viewMatrix;
	DirectX::XMMATRIX projectionMatrix;
};



struct CB_VS_BoneOffset : public AlignedAllocationPolicy<16>
{
	DirectX::XMMATRIX boneOffset[96];

};

//2D용
struct CB_VS_vertexshader_2d : public AlignedAllocationPolicy<16>
{
	DirectX::XMMATRIX wvpMatrix;
	//DirectX::XMFLOAT4  apexColor;
	//DirectX::XMFLOAT4  centerColor;
};

struct CB_CS_Bloom 
{
	float gThreshold;
	float gIntensity;
	UINT height;
	float pad;
};


// DirectionalLight
// pragma pack 이거 한번 참고하자
// https://docs.microsoft.com/ko-kr/cpp/preprocessor/pack?view=msvc-160
#pragma pack(push, 1)

struct Light
{
	DirectX::XMFLOAT3 Strength;
	float FalloffStart;
	DirectX::XMFLOAT3 Direction;
	float FalloffEnd;
	DirectX::XMFLOAT3 Position;
	float SpotPower;
	// type, castShadow, ShadowMapIndex
	DirectX::XMUINT4 shadowInfo;
};

struct CB_PS_Lights : public AlignedAllocationPolicy<16>
{
	Light lights[24];
};

struct CB_PS_LightCount : public AlignedAllocationPolicy<16>
{
	DirectX::XMUINT4 lightType[6];
	UINT lightCount;
	UINT pad0;
	UINT pad1;
	UINT pad2;
};

struct CB_PS_LightMatrix : public AlignedAllocationPolicy<16>
{
	DirectX::XMMATRIX lightMatrix[4];
	DirectX::XMUINT4 lightType;
	UINT lightCount;
	UINT pad0;
	UINT pad1;
	UINT pad2;
};

struct CB_PS_Material : public AlignedAllocationPolicy<16>
{
	DirectX::XMFLOAT4 DiffuseAlbedo;
	DirectX::XMFLOAT3 FresnelR0;
	float Shininess;
	float metallic;
	float normalFactor;
	float emissiveFactor;
	UINT pad;
};

struct CB_PS_ObjectID : public AlignedAllocationPolicy<16>
{
	UINT ObjectID;
	UINT pad;
	UINT pad2;
	UINT pad3;
};

struct CB_PS_ParticleEmissive : public AlignedAllocationPolicy<16>
{
	DirectX::XMFLOAT4 particleEmissive;
};

struct CB_GS_PaticleProperty : public AlignedAllocationPolicy<16>
{
	DirectX::XMFLOAT3  direction;
	float randomWeight;
	float velocity;
	float size;
	float birthTime;
	float deadTime;
};

struct CB_PS_Amibent : public AlignedAllocationPolicy<16>
{
	/*
	DirectX::XMFLOAT3 LowerAmbinet;
	float pad;
	DirectX::XMFLOAT3 RangeAmbinet;
	float pad2;
	*/
	DirectX::XMFLOAT4 frustumCorner[4];
};


struct CB_VS_PER_OBJECT : public AlignedAllocationPolicy<16>
{
	DirectX::XMMATRIX WorldViewProjection;
	DirectX::XMMATRIX World;
	DirectX::XMMATRIX WorldInverse;
	DirectX::XMMATRIX TextTransform;
};

struct CB_PS_Camera : public AlignedAllocationPolicy<16>
{
	DirectX::XMFLOAT4 CameraPos;
	DirectX::XMMATRIX viewProjTex;
};

struct CB_PS_ROUGHNESS : public AlignedAllocationPolicy<16>
{
	float Roughness;
	float pad;
	float pad1;
	float pad2;
};

struct CB_GS_Paricle_PerFrame : public AlignedAllocationPolicy<16>
{
	DirectX::XMMATRIX viewProj;
	DirectX::XMFLOAT3 EyePos;
};

struct CB_GS_Particle_PerParticle : public AlignedAllocationPolicy<16>
{
	DirectX::XMFLOAT3 emitPos;
	float gameTime;
	DirectX::XMFLOAT3 emitDir;
	float timeStep;
	float randomWeight;
	float size;
	float birthTime;
	float deadTime;

	float stopTime;
	UINT particleCount;
	float pad;
	float pad2;
};

struct CB_VS_NormalR : public AlignedAllocationPolicy<16>
{
	DirectX::XMMATRIX viewProj;
	DirectX::XMMATRIX view;
};

struct CB_PS_IDpicking : public AlignedAllocationPolicy<16>
{
	float r;
	float g;
	float b;
	float a;
};

#pragma pack(pop)