#pragma pack_matrix( row_major )

cbuffer cbPerObject : register(b0)
{
    float4x4 lightViewProjTM;
};

cbuffer cbSkinned : register(b1)
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
    float4 depthPosition : TEXTURE0;
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

    for (int i = 0; i < 4; ++i)
    {
        posL += weights[i] * mul(float4(vin.PosL, 1.0f), gBoneTransforms[boneIndex[i]]).xyz;
    }
    
    vout.outPosition = mul(float4(posL, 1.0f), lightViewProjTM);
    vout.depthPosition = vout.outPosition;
    
    return vout;
    
}