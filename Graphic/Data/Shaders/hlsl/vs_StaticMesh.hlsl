//***************************************************************************************
// Default.hlsl by Frank Luna (C) 2015 All Rights Reserved.
//
// Default shader, currently supports lighting.
//***************************************************************************************
#pragma pack_matrix( row_major )

#include "Matrix.hlsli"

cbuffer cbPerObject : register(b0)
{
    float4x4 wvpMatrix;
    float4x4 worldMatrix;
};

cbuffer cbLightMatrix : register(b1)
{
    float4x4 lightMatrix[4];
    uint4 shadowLightType;
    uint lightCount;
    // 현재 사용 중인 라이트의 개수, 라이트 타입도 넘어와야한다
}

struct VS_INPUT
{
    float3 inPos : POSITION;
    float3 inNormal : NORMAL;
    float2 inTexCoord : TEXCOORD;
    float3 inTangent : TANGENT;
};

struct VS_OUTPUT
{
    float4 outPosition : SV_POSITION;
    float3 outWorldPos : POSITION;
    float3 outNormal : NORMAL;
    float2 outTexCoord : TEXCOORD;
    float3 outTangent : TANGENT;
    float4 ShadowPosH[4] : TEXCOORD1;
};

VS_OUTPUT main(VS_INPUT vin)
{
    VS_OUTPUT vout = (VS_OUTPUT) 0.0f;
    
    // Transform to world space.
    vout.outWorldPos = mul(float4(vin.inPos, 1.0f), worldMatrix).xyz;
    //vout.outWorldPos = vin.inPos;
    
    // Assumes nonuniform scaling; otherwise, need to use inverse-transpose of world matrix.
    //vout.outNormal = mul(vin.inNormal, (float3x3) worldMatrix);
    vout.outNormal = normalize(mul(float4(vin.inNormal, 0.f), worldMatrix));
    vout.outTangent = mul(vin.inTangent, (float3x3) worldMatrix);
    
    // Transform to homogeneous clip space.
    vout.outPosition = mul(float4(vin.inPos, 1.0f), wvpMatrix);
    vout.outTexCoord = vin.inTexCoord;
    
    [unroll]
    for (uint i = 0; i < lightCount; i++)
    {
        vout.ShadowPosH[i] = mul(float4(vin.inPos, 1.0f), lightMatrix[i]);
    }
    
    return vout;
}




