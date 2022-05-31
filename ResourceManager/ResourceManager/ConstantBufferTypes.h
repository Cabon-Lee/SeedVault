#pragma once
#include "pch.h"

#include <DirectXMath.h>

//상수버퍼
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
struct CB_VS_wvpMatrix : public AlignedAllocationPolicy<16>
{
	DirectX::XMMATRIX worldMatrix;
	DirectX::XMMATRIX viewMatrix;
	DirectX::XMMATRIX projectionMatrix;
};

struct CB_PS_Gradient : public AlignedAllocationPolicy<16>
{
	DirectX::XMFLOAT4  apexColor;
	DirectX::XMFLOAT4  centerColor;
};

struct CB_VS_BoneOffset : public AlignedAllocationPolicy<16>
{
	DirectX::XMMATRIX boneOffset[96];
};

//2D용
struct CB_VS_vertexshader_2d : public AlignedAllocationPolicy<16>
{
	DirectX::XMFLOAT4  apexColor;
	DirectX::XMFLOAT4  centerColor;
};

// DirectionalLight
// pragma pack 이거 한번 참고하자
// https://docs.microsoft.com/ko-kr/cpp/preprocessor/pack?view=msvc-160
#pragma pack(push, 1)
struct CB_PS_DirectionalLight : public AlignedAllocationPolicy<16>
{
	DirectX::XMFLOAT3 Strength;
	float FalloffStart;
	DirectX::XMFLOAT3 Direction;
	float FalloffEnd;
	DirectX::XMFLOAT3 Position;
	float SpotPower;
};

struct CB_PS_Material : public AlignedAllocationPolicy<16>
{
	DirectX::XMFLOAT4 DiffuseAlbedo;
	DirectX::XMFLOAT3 FresnelR0;
	float Shininess;
};

struct CB_VS_PER_OBJECT : public AlignedAllocationPolicy<16>
{
	DirectX::XMMATRIX WorldViewProjection;
	DirectX::XMMATRIX World;
};

struct CB_PS_PER_OBJECT : public AlignedAllocationPolicy<16>
{
	DirectX::XMFLOAT3 m_vEyePosition;
	float pad;
	DirectX::XMFLOAT4 Ambient;
};


#pragma pack(pop)