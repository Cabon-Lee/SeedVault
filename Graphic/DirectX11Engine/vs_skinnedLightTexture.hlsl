#pragma pack_matrix( row_major )

cbuffer cbPerObject : register(b0)
{
    float4x4 lightViewProjTM; // ModelTM까지 곱해져야함
};

cbuffer ModelMatrix : register(b1)
{
    float4x4 wvpMatrix;
    float4x4 worldMatrix;
}

cbuffer cbSkinned : register(b2)
{
    float4x4 gBoneTransforms[96];
};

struct SHADOW_VS_INPUT
{
    // 범용적으로 쓸 수 있나 각을 재봐야함
    float3 inPosition : POSITION;
    float3 inNormal : NORMAL0;
    float2 inTex : TEXCOORD0;
    float3 inTangent : TANGENT;
    float4 inWeights : WEIGHTS;
    uint4 inBoneIndices : BONEINDICES;
};

struct VS_OUTPUT
{
    float4 outPosition : SV_POSITION; // SV로 묶여있어 직접 접근이 안된다.
    float4 depthPosition : TEXTURE0; // 위의 이유 때문에 추가로 같은 복사해서 타입을 넘겨준다
    float3 outNormal : NORMAL0;
    float2 outTex : TEXCOORD1;
    float4 viewPosition : TEXCOORD2;
    float3 lightPos : TEXCOORD3;
};

VS_OUTPUT main(SHADOW_VS_INPUT vin)
{
    VS_OUTPUT vout;
    
    float weights[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
    weights[0] = vin.inWeights.x;
    weights[1] = vin.inWeights.y;
    weights[2] = vin.inWeights.z;
    weights[3] = 1.0f - weights[0] - weights[1] - weights[2];


    int boneIndex[4] = { 0, 0, 0, 0 };
    boneIndex[0] = vin.inBoneIndices.x;
    boneIndex[1] = vin.inBoneIndices.y;
    boneIndex[2] = vin.inBoneIndices.z;
    boneIndex[3] = vin.inBoneIndices.a;

    float3 posL = float3(0.0f, 0.0f, 0.0f);

    for (int i = 0; i < 4; ++i)
    {
        posL += weights[i] * mul(float4(vin.inPosition, 1.0f), gBoneTransforms[boneIndex[i]]).xyz;
    }
    
    vout.outPosition = mul(float4(posL, 1.0f), wvpMatrix);
    vout.depthPosition = vout.outPosition;
    
    vout.viewPosition = mul(float4(posL, 1.0f), lightViewProjTM);
    vout.outNormal = normalize(mul(vin.inNormal, (float3x3) worldMatrix));
    
    vout.outTex = vin.inTex;
   
    
    return vout;
}