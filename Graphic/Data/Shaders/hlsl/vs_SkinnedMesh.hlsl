#pragma pack_matrix( row_major )

#include "Matrix.hlsli"

cbuffer cbPerObject : register(b0)
{
    float4x4 wvpMatrix;
    float4x4 worldMatrix;
    //float4x4 worldMatrixInverse;
};

cbuffer cbLight : register(b1)
{
    float4x4 lightMatrix[4];
    uint4 shadowLightType;
    uint lightCount;
    // ���� ��� ���� ����Ʈ�� ����, ����Ʈ Ÿ�Ե� �Ѿ�;��Ѵ�
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
    float3 Tangent : TANGENT;
    float4 Weights : WEIGHTS;
    uint4 BoneIndices : BONEINDICES;
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

	// ��ΰ� ��Ű�� �޽��� ���Ե� ���, ���� �׷��ִ� �޽��� �Բ� Ÿ�� ������ �̻��� ���� ���´�.
	// �̸� �����ϱ� ���ؼ� ��Ű�� �޽��� �ƴ� ���� �����ؼ� �׷��ְ� �Ǿ���.
        
    float3 posL = float3(0.0f, 0.0f, 0.0f);
    float3 normalL = float3(0.0f, 0.0f, 0.0f);

    for (int i = 0; i < 4; ++i)
    {
        posL += weights[i] * mul(float4(vin.PosL, 1.0f), gBoneTransforms[boneIndex[i]]).xyz;
        normalL += weights[i] * mul(vin.NormalL, (float3x3) gBoneTransforms[boneIndex[i]]);
    }
    
    vout.outWorldPos = mul(float4(posL, 1.0f), worldMatrix).xyz;
    
    // ��Ű�� �� �������� �븻�� ���� ���
    //vout.outNormal = normalize(mul(float4(normalL, 0.f), worldMatrixInverse));
    //vout.outNormal = mul(normalL, (float3x3) ); <= ������ �ι��� ��Ʈ������ �ٱ����� ������
    
    vout.outNormal = mul(normalL, (float3x3) inverse(worldMatrix));
    vout.outTangent = mul(vin.Tangent, (float3x3)worldMatrix);
    
    vout.outPosition = mul(float4(posL, 1.0f), wvpMatrix);

    vout.outTexCoord = vin.Tex;
    
    // ������ �ȱ׷������� vin.PosL�� �ߴ�. �翬�� �ȵȴ�
    //vout.ShadowPosH = mul(float4(posL, 1.0f), lightViewProj);
    [unroll]
    for (uint j = 0; j < lightCount; j++)
    {
        vout.ShadowPosH[j] = mul(float4(posL, 1.0f), lightMatrix[j]);
    }

    
    return vout;
    
}