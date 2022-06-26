// 바깥에서 머트리얼을 가져온다

#include "light.hlsl"

cbuffer cbMaterial : register(b0)
{
    float4 DiffuseAlbedo;
    float3 FresnelR0;
    float Roughness;
    float3 AmbientDown;
    float metallic;
    float3 AmbientRange;
};

cbuffer cbLightMatrix : register(b1)
{
    float4x4 lightMatrix[4];
    uint4 shadowLightType;
    uint lightCount;
    // 현재 사용 중인 라이트의 개수, 라이트 타입도 넘어와야한다
}

// 렌더 타겟이 묶인다?
struct PS_GBUFFER_OUT
{
    float4 debug : SV_TARGET0;
    float4 Albedo : SV_TARGET1;
    float4 NormalMap : SV_TARGET2;
    float4 Position : SV_TARGET3; // xyz 포지션 + depth
    float4 Material : SV_TARGET4; // FresnelR0 + roughness
    float4 Ambient : SV_TARGET5; // Ambient + metallic
    float4 Shadow : SV_TARGET6;
};

struct VS_OUTPUT
{
    float4 outPosition : SV_POSITION;
    float3 outWorldPos : POSITION;
    float3 outNormal : NORMAL;
    float2 outTexCoord : TEXCOORD;
    float4 outTangent : TANGENT;
    float4 ShadowPosH[4] : TEXCOORD1;
};

Texture2D objTexture : TEXTURE : register(t0);
Texture2D objNormal : TEXTURE : register(t1);
Texture2D objMetalRough : TEXTURE : register(t2);
Texture2D objEmissvie : TEXTURE : register(t3);
Texture2D ShadowMap[4] : TEXTURE : register(t4);
TextureCube textureSkyCubeMap : TEXTURE : register(t5);

// 일단은 4장만 쓸 예정 기본적으로 DirectionalLight, SpotLight 두개가 될 것
// 실험 해볼것, 아마도 몇개가 들어갔는지 카운트도 포함되어야할듯

SamplerState objSamplerState : SAMPLER : register(s0);
SamplerComparisonState PCFSample : SAMPLER : register(s1);

PS_GBUFFER_OUT main(VS_OUTPUT pin) : SV_Target
{
    PS_GBUFFER_OUT deferredOut;
     
    // 알베도 맵이 없으면 Material의 알베도 맵을 가져와서 그려준다
    deferredOut.Albedo = objTexture.Sample(objSamplerState, pin.outTexCoord) * DiffuseAlbedo;
       
    if (length(deferredOut.Albedo.xyz) == 0.0f)
    {
        deferredOut.Albedo = DiffuseAlbedo;
    }
    
    // metal-rough니까 메탈이 먼저
    float4 metalRoughMap = objMetalRough.Sample(objSamplerState, pin.outTexCoord);
    deferredOut.Material = float4(metalRoughMap.r * metallic, metalRoughMap.g * Roughness, 0.0, 1.0);        
    
    // 노말
    //float3 normal = (pin.outNormal * 0.5f + 0.5f);
    //float3 normal = (pin.outNormal * 0.5f + 0.5f);
    float3 normalMapSample = objNormal.Sample(objSamplerState, pin.outTexCoord).rgb;
    
    if (length(normalMapSample) == 0.0f)
    {
        deferredOut.NormalMap = float4((pin.outNormal * 0.5f + 0.5f), 1.0f);
    }
    else
    {
        deferredOut.NormalMap = float4((NormalSampleToWorldSpace(normalMapSample, pin.outNormal, pin.outTangent) * 0.5f) + 0.5f, 1.0f);
    }
    
    // 앰비언트 + 메탈릭
    deferredOut.Ambient = float4(CalcAmbient(deferredOut.NormalMap.xyz, AmbientDown, AmbientRange), 0.0f);
    deferredOut.Ambient += objEmissvie.Sample(objSamplerState, pin.outTexCoord);
    deferredOut.Ambient.w = 1.0;
    
    deferredOut.Position = float4(pin.outWorldPos.xyz, 1.0f);
    
    float shadow = 0.0f;
    float4 shadowValue = 1.0f;
    

    [unroll]
    for (uint i = 0; i < lightCount; i++)
    {
        uint _nowType = shadowLightType[i];
        
        float _origin = 0.0f;
        switch (_nowType)
        {
            case 0 :
                _origin = CalcShadowFactor(PCFSample, ShadowMap[i], pin.ShadowPosH[i]);
                shadowValue[i] = saturate(_origin + 0.5);
                break;
            case 2:
                _origin = SpotShadowPCF(PCFSample, ShadowMap[i], pin.ShadowPosH[i]);
                shadowValue[i] = saturate(_origin + 0.5);
                break;
        }
    }
   
    deferredOut.Shadow = float4(shadowValue.x, shadowValue.y, shadowValue.z, shadowValue.w);
    
    return deferredOut;
}