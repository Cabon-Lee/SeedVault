#include "PBRLibrary.hlsli"

cbuffer Light : register(b0)
{
    Light DirLight;
}

cbuffer cbMaterial : register(b1)
{
    float4 DiffuseAlbedo;
    float3 FresnelR0;
    float smoothness;
    float metallic;
    float normalFactor;
    float emissiveFactor;
};

cbuffer objectPer : register(b2)
{
    float4 EyePos;
}

Texture2D objTexture    : TEXTURE : register(t0);
Texture2D objNormal     : TEXTURE : register(t1);
Texture2D objMetalRough : TEXTURE : register(t2);
Texture2D objEmissive   : TEXTURE : register(t3);

SamplerState objSamplerState : SAMPLER : register(s0);

struct VS_OUTPUT
{
    float4 outPosition : SV_POSITION;
    float3 outWorldPos : POSITION;
    float3 outNormal : NORMAL;
    float2 outTexCoord : TEXCOORD;
    float4 outTangent : TANGENT;
};


float4 main(VS_OUTPUT pin) : SV_TARGET
{
    pin.outNormal = normalize(pin.outNormal);
    
    // Default to multiplicative identity.
    float4 texColor = float4(1, 1, 1, 1);
	// Sample texture.
    texColor = objTexture.Sample(objSamplerState, pin.outTexCoord);
    
    float3 toEyeW = normalize(EyePos.xyz - pin.outWorldPos.xyz);   // 기존과 다르게 -를 붙여야 검은 점이 사라진다... 이유는 아직 모름
    
    float4 MRMap = objMetalRough.Sample(objSamplerState, pin.outTexCoord);
    float nowMetalFactor = saturate(MRMap.r * metallic);
    float nowRoughnessFactor = saturate(MRMap.g * smoothness);
    
    float4 finalColor = 0.0;
    finalColor = texColor;
    /*
    finalColor = CalcLight(
              0,                    // 라이트 종류
              DirLight,             // 라이트 정보
              texColor,             // 베이스 컬러(알베도)
              pin.outWorldPos.xyz,  // 월드 공간에서 위치
              pin.outNormal.xyz,    // 노말
              toEyeW,               // 카메라 위치
              nowRoughnessFactor,   // 러프니스
              nowMetalFactor,       // 메탈릭
              float3(0.0, 0.0, 0.0));
    */
    finalColor.w = 1.0f;
    
    
    return finalColor;
}