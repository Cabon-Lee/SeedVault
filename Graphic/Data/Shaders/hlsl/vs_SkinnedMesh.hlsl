#pragma pack_matrix( row_major )

#include "Matrix.hlsli"

cbuffer cbPerObject : register(b0)
{
    float4x4 wvpMatrix;
    float4x4 worldMatrix;
    float4x4 view;
    float4x4 texTransform;
};

cbuffer cbLight : register(b1)
{
    float4x4 lightMatrix[4];
    uint4 shadowLightType;
    uint lightCount;
    // 현재 사용 중인 라이트의 개수, 라이트 타입도 넘어와야한다
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
    float3 outWorldPos : POSITION0;
    float3 outNormal : NORMAL0;
    float2 outTexCoord : TEXCOORD0;
    float4 outTangent : TANGENT;
    float4 ShadowPosH[4] : TEXCOORD1;
    
    float3 outPosV : POSITION1;
    float3 outNormalDepth : NORMAL1;
    float2 outSSAOTex : TEXCOORD5;
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
    float3 tangentL = float3(0.0f, 0.0f, 0.0f);
    float3 posV = float3(0.0f, 0.0f, 0.0f);
    float3 normalDepth = float3(0.0f, 0.0f, 0.0f);
    
    for (int i = 0; i < 4; ++i)
    {
        posL += weights[i] * mul(float4(vin.PosL, 1.0f), gBoneTransforms[boneIndex[i]]).xyz;
        normalL += weights[i] * mul(vin.NormalL, (float3x3) gBoneTransforms[boneIndex[i]]);
        tangentL += weights[i] * mul(vin.Tangent.xyz, (float3x3) gBoneTransforms[boneIndex[i]]);
    }
    
    vout.outWorldPos = mul(float4(posL, 1.0f), worldMatrix).xyz;
    vout.outPosV = mul(float4(posL, 1.0f), mul(worldMatrix, view)).xyz;
    
    // 스키닝 시 정상적인 노말의 적용 방식
    //vout.outNormal = normalize(mul(float4(normalL, 0.f), worldMatrixInverse));
    //vout.outNormal = mul(normalL, (float3x3) ); <= 원래는 인버스 매트릭스를 바깥에서 가져옴
    vout.outNormal = mul(normalL, (float3x3) inverse(worldMatrix));
    vout.outNormalDepth = mul(float4(normalL, 0), view).xyz;
    
    vout.outTangent = float4(mul(tangentL, (float3x3) worldMatrix), vin.Tangent.w);
    
    vout.outPosition = mul(float4(posL, 1.0f), wvpMatrix);

    vout.outTexCoord = vin.Tex;
    
    // 이전에 안그려질때는 vin.PosL로 했다. 당연히 안된다
    //vout.ShadowPosH = mul(float4(posL, 1.0f), lightViewProj);
    [unroll]
    for (uint j = 0; j < lightCount; j++)
    {
        vout.ShadowPosH[j] = mul(float4(posL, 1.0f), lightMatrix[j]);
    }
    
    vout.outSSAOTex = float2(0, 0);
    return vout;
    
}