#pragma pack_matrix( row_major )

cbuffer cbPerObject : register(b0)
{
    float4x4 gViewProject;
};

cbuffer cbLightMatrix : register(b1)
{
    float4x4 lightMatrix[4];
    uint4 shadowLightType;
    uint lightCount;
}

cbuffer cbSkinned : register(b2)
{
    float4x4 gBoneTransforms[96];
};

struct VS_INPUT
{
    float3 inPos : POSITION;
    float3 inNormal : NORMAL;
    float2 inTexCoord : TEXCOORD;
    float4 inTangent : TANGENT;
    
    float4x4 World : WORLD;
};

struct SKINNED_VS_INPUT
{
    float3 PosL : POSITION;
    float3 NormalL : NORMAL;
    float2 Tex : TEXCOORD;
    float3 Tangent : TANGENT;
    float4 Weights : WEIGHTS;
    uint4 BoneIndices : BONEINDICES;
    
    float4x4 World : WORLD;
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

float4x4 inverse(float4x4 m)
{
    float n11 = m[0][0], n12 = m[1][0], n13 = m[2][0], n14 = m[3][0];
    float n21 = m[0][1], n22 = m[1][1], n23 = m[2][1], n24 = m[3][1];
    float n31 = m[0][2], n32 = m[1][2], n33 = m[2][2], n34 = m[3][2];
    float n41 = m[0][3], n42 = m[1][3], n43 = m[2][3], n44 = m[3][3];

    float t11 = n23 * n34 * n42 - n24 * n33 * n42 + n24 * n32 * n43 - n22 * n34 * n43 - n23 * n32 * n44 + n22 * n33 * n44;
    float t12 = n14 * n33 * n42 - n13 * n34 * n42 - n14 * n32 * n43 + n12 * n34 * n43 + n13 * n32 * n44 - n12 * n33 * n44;
    float t13 = n13 * n24 * n42 - n14 * n23 * n42 + n14 * n22 * n43 - n12 * n24 * n43 - n13 * n22 * n44 + n12 * n23 * n44;
    float t14 = n14 * n23 * n32 - n13 * n24 * n32 - n14 * n22 * n33 + n12 * n24 * n33 + n13 * n22 * n34 - n12 * n23 * n34;

    float det = n11 * t11 + n21 * t12 + n31 * t13 + n41 * t14;
    float idet = 1.0f / det;

    float4x4 ret;

    ret[0][0] = t11 * idet;
    ret[0][1] = (n24 * n33 * n41 - n23 * n34 * n41 - n24 * n31 * n43 + n21 * n34 * n43 + n23 * n31 * n44 - n21 * n33 * n44) * idet;
    ret[0][2] = (n22 * n34 * n41 - n24 * n32 * n41 + n24 * n31 * n42 - n21 * n34 * n42 - n22 * n31 * n44 + n21 * n32 * n44) * idet;
    ret[0][3] = (n23 * n32 * n41 - n22 * n33 * n41 - n23 * n31 * n42 + n21 * n33 * n42 + n22 * n31 * n43 - n21 * n32 * n43) * idet;

    ret[1][0] = t12 * idet;
    ret[1][1] = (n13 * n34 * n41 - n14 * n33 * n41 + n14 * n31 * n43 - n11 * n34 * n43 - n13 * n31 * n44 + n11 * n33 * n44) * idet;
    ret[1][2] = (n14 * n32 * n41 - n12 * n34 * n41 - n14 * n31 * n42 + n11 * n34 * n42 + n12 * n31 * n44 - n11 * n32 * n44) * idet;
    ret[1][3] = (n12 * n33 * n41 - n13 * n32 * n41 + n13 * n31 * n42 - n11 * n33 * n42 - n12 * n31 * n43 + n11 * n32 * n43) * idet;

    ret[2][0] = t13 * idet;
    ret[2][1] = (n14 * n23 * n41 - n13 * n24 * n41 - n14 * n21 * n43 + n11 * n24 * n43 + n13 * n21 * n44 - n11 * n23 * n44) * idet;
    ret[2][2] = (n12 * n24 * n41 - n14 * n22 * n41 + n14 * n21 * n42 - n11 * n24 * n42 - n12 * n21 * n44 + n11 * n22 * n44) * idet;
    ret[2][3] = (n13 * n22 * n41 - n12 * n23 * n41 - n13 * n21 * n42 + n11 * n23 * n42 + n12 * n21 * n43 - n11 * n22 * n43) * idet;

    ret[3][0] = t14 * idet;
    ret[3][1] = (n13 * n24 * n31 - n14 * n23 * n31 + n14 * n21 * n33 - n11 * n24 * n33 - n13 * n21 * n34 + n11 * n23 * n34) * idet;
    ret[3][2] = (n14 * n22 * n31 - n12 * n24 * n31 - n14 * n21 * n32 + n11 * n24 * n32 + n12 * n21 * n34 - n11 * n22 * n34) * idet;
    ret[3][3] = (n12 * n23 * n31 - n13 * n22 * n31 + n13 * n21 * n32 - n11 * n23 * n32 - n12 * n21 * n33 + n11 * n22 * n33) * idet;

    return ret;
}


VS_OUTPUT StaticMesh(VS_INPUT vin)
{
    VS_OUTPUT vout = (VS_OUTPUT) 0.0f;
    
    // Transform to world space.
    vout.outWorldPos = mul(float4(vin.inPos, 1.0f), vin.World).xyz;
    
    vout.outNormal = normalize(mul(vin.inNormal, (float3x3) vin.World));
    //vout.outTangent = mul(vin.inTangent, (float3x3) vin.World);
    vout.outTangent = mul(vin.inTangent, vin.World);
    
    // Transform to homogeneous clip space.
    vout.outPosition = mul(float4(vout.outWorldPos, 1.0f), gViewProject);
    vout.outTexCoord = vin.inTexCoord;
    
    [unroll]
    for (uint i = 0; i < lightCount; i++)
    {
        vout.ShadowPosH[i] = mul(float4(vin.inPos, 1.0f), lightMatrix[i]);
    }
    
    return vout;
}


VS_OUTPUT SkinnedMesh(SKINNED_VS_INPUT vin)
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
        normalL += weights[i] * mul(vin.NormalL, (float3x3) gBoneTransforms[boneIndex[i]]);
    }
    
    vout.outWorldPos = mul(float4(posL, 1.0f), vin.World).xyz;
    
    // 스키닝 시 정상적인 노말의 적용 방식
    //vout.outNormal = normalize(mul(float4(normalL, 0.f), worldMatrixInverse));
    //vout.outNormal = mul(normalL, (float3x3) ); <= 원래는 인버스 매트릭스를 바깥에서 가져옴
    
    vout.outNormal = mul(normalL, (float3x3) inverse(vin.World));
    vout.outTangent = mul(vin.Tangent, (float3x3) vin.World);
    
    vout.outPosition = mul(float4(posL, 1.0f), gViewProject);

    vout.outTexCoord = vin.Tex;
    
    // 이전에 안그려질때는 vin.PosL로 했다. 당연히 안된다
    //vout.ShadowPosH = mul(float4(posL, 1.0f), lightViewProj);
    [unroll]
    for (uint j = 0; j < lightCount; j++)
    {
        vout.ShadowPosH[j] = mul(float4(posL, 1.0f), lightMatrix[j]);
    }

    return vout;
}