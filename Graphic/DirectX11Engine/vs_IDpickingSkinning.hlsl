#pragma pack_matrix( row_major )

cbuffer perObjectBuffer : register(b0)
{
    float4x4 wvpMatrix;
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
    float4 C0 : COLOR0;
};

VS_OUTPUT main(SKINNED_VS_INPUT In)
{
	// Output position of the vertex, in clip space : MVP * position
    VS_OUTPUT Out = (VS_OUTPUT) 0;
    
    float weights[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
    weights[0] = In.Weights.x;
    weights[1] = In.Weights.y;
    weights[2] = In.Weights.z;
    weights[3] = 1.0f - weights[0] - weights[1] - weights[2];
    
    int boneIndex[4] = { 0, 0, 0, 0 };
    boneIndex[0] = In.BoneIndices.x;
    boneIndex[1] = In.BoneIndices.y;
    boneIndex[2] = In.BoneIndices.z;
    boneIndex[3] = In.BoneIndices.a;
    
    float3 posL = float3(0.0f, 0.0f, 0.0f);
    float3 normalL = float3(0.0f, 0.0f, 0.0f);

    for (int i = 0; i < 4; ++i)
    {
        posL += weights[i] * mul(float4(In.PosL, 1.0f), gBoneTransforms[boneIndex[i]]).xyz;
    }
    
    Out.outPosition = mul(float4(posL, 1.0f), wvpMatrix);
            
    Out.C0 = float4(0, 0, 0, 1);
    
    return Out;
}