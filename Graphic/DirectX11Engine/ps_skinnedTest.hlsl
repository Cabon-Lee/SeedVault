#include "light.hlsl"

cbuffer objectPer : register(b0)
{
    Light Dirlight;
}

cbuffer cbMaterial : register(b1)
{
    float4 DiffuseAlbedo;
    float3 FresnelR0;
    float Roughness;
};

cbuffer objectPer : register(b2)
{
    float3 eyePos;
    float pad;
    float4 AmbientLight;
}

struct VS_OUTPUT
{
    float4 outPosition : SV_POSITION;
    float3 outWorldPos : POSITION;
    float3 outNormal : NORMAL;
    float2 outTexCoord : TEXCOORD;
    float4 outTangent : TANGENT;
};

Texture2D objTexture : TEXTURE : register(t0);
Texture2D normalMapTexture : TEXTURE : register(t1);

SamplerState objSamplerState : SAMPLER : register(s0);

float4 main(VS_OUTPUT pin) : SV_Target
{
	 // Interpolating normal can unnormalize it, so renormalize it.
    //pin.NormalW = normalize(pin.NormalW); 버텍스에서 이미 한 번 함
    float4 diffuseAlbedo = objTexture.Sample(objSamplerState, pin.outTexCoord) * DiffuseAlbedo;
    float3 normalMapSample = normalMapTexture.Sample(objSamplerState, pin.outTexCoord).rgb;
    float3 bumpedNormalW = NormalSampleToWorldSpace(normalMapSample, pin.outNormal, pin.outTangent);
    
    // Vector from point being lit to eye. 
    float3 toEyeW = normalize(eyePos - pin.outWorldPos);
  
    
	// Indirect lighting.
    // 환경광은 아무래도 전체다 적용되어서 그런가
    float4 ambient = AmbientLight * DiffuseAlbedo;

    const float shininess = 1.0f - Roughness;
    Material mat = { diffuseAlbedo, FresnelR0, shininess };
    float3 shadowFactor = 1.0f;
    //float4 directLight = ComputeLighting(Dirlight, mat, pin.outWorldPos, pin.outNormal, toEyeW, shadowFactor);
    float4 directLight = float4(ComputeDirectionalLight(Dirlight, mat, pin.outNormal, toEyeW), 1.0f);

    float4 litColor = ambient + directLight;

    // Common convention to take alpha from diffuse material.
    
    //litColor = float4(pin.outNormal, 1.0f);
    litColor.a = DiffuseAlbedo.a;

    
    return litColor;
}