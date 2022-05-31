// 바깥에서 머트리얼을 가져온다

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

// 렌더 타겟이 묶인다?
struct PS_GBUFFER_OUT
{
    float4 Albedo : SV_TARGET0;
    float4 NormalW : SV_TARGET1;
    float4 NormalMap : SV_TARGET2;
    float4 Position : SV_TARGET3; // xyz 포지션 + depth
};

struct VS_OUTPUT
{
    float4 outPosition : SV_POSITION;
    float3 outWorldPos : POSITION;
    float3 outNormal : NORMAL;
    float2 outTexCoord : TEXCOORD;
};

Texture2D objTexture : TEXTURE : register(t0);
Texture2D objNormal : TEXTURE : register(t1);

SamplerState objSamplerState : SAMPLER : register(s0);

PS_GBUFFER_OUT main(VS_OUTPUT pin) : SV_Target
{
    PS_GBUFFER_OUT deferredOut;    
    
    deferredOut.Albedo = objTexture.Sample(objSamplerState, pin.outTexCoord) * DiffuseAlbedo;
    
    if (length(deferredOut.Albedo.xyz) == 0.0f)
    {
        deferredOut.Albedo = DiffuseAlbedo;
    }
    
    deferredOut.NormalW = float4(pin.outNormal.xyz, 1.0f);
    deferredOut.NormalMap = float4(pin.outNormal.xyz, 1.0f); // 아직 노말맵 안들어감
    deferredOut.Position = float4(pin.outWorldPos.xyz, 1.0f);
    
    return deferredOut;
}