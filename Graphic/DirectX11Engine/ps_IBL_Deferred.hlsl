#include "PBRLibrary.hlsli"

cbuffer cbMaterial : register(b0)
{
    float4 DiffuseAlbedo;
    float3 FresnelR0;
    float smoothness;
    float metallic;
    float normalFactor;
    float emissiveFactor;
};

// 이 아래는 매 프레임별로 한번만 밀어넣으면 되는 것
cbuffer cbLightMatrix : register(b1)
{
    float4x4 lightMatrix[4];
    uint4 shadowLightType;
    uint lightCount;
    // 현재 사용 중인 라이트의 개수, 라이트 타입도 넘어와야한다
}

cbuffer Camera : register(b2)
{
    float4 EyePos;
}

cbuffer ObjectID : register(b3)
{
    uint objectID;
}

struct VS_OUTPUT
{
    float4 outPosition : SV_POSITION;
    float3 outWorldPos : POSITION;
    float3 outNormal : NORMAL;
    float2 outTexCoord : TEXCOORD;
    float3 outTangent : TANGENT;
    float4 ShadowPosH[4] : TEXCOORD1;
};

struct PS_GBUFFER_OUT
{
    float4 Albedo : SV_TARGET0;
    float4 NormalMap : SV_TARGET1;
    float4 Position : SV_TARGET2; // xyz 포지션 + depth
    float4 Material : SV_TARGET3; // FresnelR0 + roughness 
    float4 Ambient : SV_TARGET4; // Ambient + metallic + prefilter + irradiance
    float4 Shadow : SV_TARGET5;
    float4 Emissive : SV_TARGET6;
    uint4 ObjectID : SV_TARGET7;
};

Texture2D objTexture        : TEXTURE : register(t0);
Texture2D objNormal         : TEXTURE : register(t1);
Texture2D objMetalRough     : TEXTURE : register(t2);
Texture2D objEmissvie       : TEXTURE : register(t3);
Texture2D ShadowMap[4]      : TEXTURE : register(t4);
Texture2D brdfLUT           : TEXTURE : register(t8);
TextureCube irradianceMap   : TEXTURE : register(t9);
TextureCube preFilterMap    : TEXTURE : register(t10);

// 일단은 4장만 쓸 예정 기본적으로 DirectionalLight, SpotLight 두개가 될 것
// 실험 해볼것, 아마도 몇개가 들어갔는지 카운트도 포함되어야할듯

SamplerState objSamplerState : SAMPLER : register(s0);
SamplerComparisonState PCFSample : SAMPLER : register(s1);

PS_GBUFFER_OUT main(VS_OUTPUT pin) : SV_Target
{
    PS_GBUFFER_OUT deferredOut;
     
    // 알베도 맵이 없으면 Material의 알베도 맵을 가져와서 그려준다
    deferredOut.Albedo = objTexture.Sample(objSamplerState, pin.outTexCoord) * DiffuseAlbedo;
    
    // metal-rough니까 메탈이 먼저
    float4 metalRoughMap = objMetalRough.Sample(objSamplerState, pin.outTexCoord);
    
    float nowMetalFactor = saturate(metalRoughMap.r * metallic);
    float nowRoughnessFactor = saturate(metalRoughMap.g * smoothness);
    deferredOut.Material = float4(nowMetalFactor, nowRoughnessFactor, 0.0, 1.0);
    
    // 노말
    float3 normalMapSample = objNormal.Sample(objSamplerState, pin.outTexCoord).rgb;
    
    float _nowNormalFactor = (normalFactor * 0.5) + 0.5;
    
    if (length(normalMapSample) == 0.0f)
    {
        deferredOut.NormalMap = float4(((pin.outNormal * _nowNormalFactor) * 0.5f + 0.5f), 1.0f);
    }
    else
    {
        deferredOut.NormalMap = float4(((NormalSampleToWorldSpace(normalMapSample, pin.outNormal, pin.outTangent) * _nowNormalFactor) * 0.5f) + 0.5f, 1.0f);
    }
    
    deferredOut.Position = float4(pin.outWorldPos.xyz, 1 - (pin.outPosition.z / pin.outPosition.w));
    
    // 같은 노말이라도 전방위 반사를 위해 포지션 값을 더한다
    float3 _posNormal = normalize(deferredOut.NormalMap.xyz + deferredOut.Position.xyz);
    
    {
        // IBL 이미지를 엠비언트로 변경
        // 조도맵을 추출
        float3 irradianceColor = irradianceMap.Sample(objSamplerState, _posNormal).rgb;
    
        // 메탈릭으로부터 프레넬항을 구한다
        float3 viewDir = -normalize(EyePos.xyz - deferredOut.Position.xyz);
        float3 F0 = lerp(float3(0.04, 0.04, 0.04), deferredOut.Albedo.xyz, nowMetalFactor);
    
        // 러프니스를 통해 F항을 구한다
        float perceptualRoughness = SmoothnessToPerceptualRoughness(nowRoughnessFactor);
        float roughness = PerceptualRoughnessToRoughness(perceptualRoughness);
        float3 F = FresnelSchlickRoughness(max(dot(_posNormal, viewDir), 0.0), F0, roughness);
    
        float3 kS = F;
        float3 kD = 1.0 - kS;
        kD *= 1.0 - metallic;
    
        // 프리필터 맵에서 반사된 스페큘러를 가져온다
        float3 R = reflect(viewDir, _posNormal);
    
        float3 indirectDiffuse = irradianceColor * deferredOut.Albedo.xyz;
        const float MAX_REFLECTION_LOD = 4.0;
        float3 prefilteredColor = preFilterMap.SampleLevel(objSamplerState, R, roughness * MAX_REFLECTION_LOD).rgb;
        float2 envBRDF = brdfLUT.Sample(objSamplerState, float2(max(dot(_posNormal, viewDir), 0.0), roughness)).rg;
    
        float3 indirectSpecular = prefilteredColor * (F * envBRDF.x + envBRDF.y);
    
        float3 ambient = (kD * indirectDiffuse + indirectSpecular);
    
        deferredOut.Ambient.xyz = ambient;
        deferredOut.Ambient.w = 1.0;
    }
    
    
    deferredOut.Emissive = objEmissvie.Sample(objSamplerState, pin.outTexCoord);
    
    if (length(deferredOut.Emissive.rgb) == 0.0)
    {
        deferredOut.Emissive = float4(0.0, 0.0, 0.0, 1.0);
    }
    else
    {
        // 이미시브맵
        deferredOut.Emissive.x = (deferredOut.Emissive.x * deferredOut.Emissive.x) * (emissiveFactor * 100);
        deferredOut.Emissive.y = (deferredOut.Emissive.y * deferredOut.Emissive.y) * (emissiveFactor * 100);
        deferredOut.Emissive.z = (deferredOut.Emissive.z * deferredOut.Emissive.z) * (emissiveFactor * 100);
    }
    
    float shadow = 0.0f;
    float4 shadowValue = 1.0f;
    [unroll]
    for (uint i = 0; i < lightCount; i++)
    {
        uint _nowType = shadowLightType[i];
        
        float _origin = 0.0f;
        switch (_nowType)
        {
            case 0:
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
    deferredOut.ObjectID.x = objectID;
                                  
    return deferredOut;           
}                                 