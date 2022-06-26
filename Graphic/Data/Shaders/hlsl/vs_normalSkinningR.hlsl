#pragma pack_matrix( row_major )

#include "Matrix.hlsli"

cbuffer cbPerObject : register(b0)
{
    float4x4 wvpMatrix;
    float4x4 worldMatrix;
};

cbuffer cbSkinned : register(b2)
{
    float4x4 gBoneTransforms[96];
};

struct SKINNED_VS_INPUT
{
    float3 PosL : POSITION;
    float3 NormalL : NORMAL;
    float2 Tex : TEXCOORD;
    float4 Tangent : TANGENT;
    float4 Weights : WEIGHTS;
    uint4 BoneIndices : BONEINDICES;
};

struct VS_OUTPUT
{
    float4 outPosition : SV_POSITION;
    float3 outNormal : NORMAL;  
};

VS_OUTPUT main(SKINNED_VS_INPUT vin)
{
    VS_OUTPUT vout;
        
    float weights[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
    weights[0] = vin.Weights.x;
    weights[1] = vin.Weights.y;
    weights[2] = vin.Weights.z;
    weights[3] = 1.0f - weights[0] - weights[1] - weights[2];

    int boneIndex[4] = { 0, 0, 0, 0 };
    boneIndex[0] = vin.BoneIndices.x;
    boneIndex[1] = vin.BoneIndices.y;
    boneIndex[2] = vin.BoneIndices.z;
    boneIndex[3] = vin.BoneIndices.a;

	// 모두가 스키닝 메쉬를 돌게될 경우, 본을 그려주는 메쉬도 함께 타기 때문에 이상한 값이 나온다.
	// 이를 구별하기 위해서 스키닝 메쉬와 아닌 것을 구분해서 그려주게 되었다.
        
    float3 posL = float3(0.0f, 0.0f, 0.0f);
    float3 normalL = float3(0.0f, 0.0f, 0.0f);

    for (int i = 0; i < 4; ++i)
    {
        posL += weights[i] * mul(float4(vin.PosL, 1.0f), gBoneTransforms[boneIndex[i]]).xyz;
        normalL += weights[i] * mul(float3(0.0f, 0.0f, 0.0f), (float3x3) gBoneTransforms[boneIndex[i]]);
        //normalL += weights[i] * mul(vin.NormalL, (float3x3) gBoneTransforms[boneIndex[i]]);
    }
   
    vout.outPosition = mul(float4(posL, 1.0f), wvpMatrix);
    vout.outNormal = mul(normalL, (float3x3) inverse(worldMatrix));
    
    return vout;
}