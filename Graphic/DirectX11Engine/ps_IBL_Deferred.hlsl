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

// �� �Ʒ��� �� �����Ӻ��� �ѹ��� �о������ �Ǵ� ��
cbuffer cbLightMatrix : register(b1)
{
    float4x4 lightMatrix[4];
    uint4 shadowLightType;
    uint lightCount;
    // ���� ��� ���� ����Ʈ�� ����, ����Ʈ Ÿ�Ե� �Ѿ�;��Ѵ�
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
    float4 Position : SV_TARGET2; // xyz ������ + depth
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

// �ϴ��� 4�常 �� ���� �⺻������ DirectionalLight, SpotLight �ΰ��� �� ��
// ���� �غ���, �Ƹ��� ��� ������ ī��Ʈ�� ���ԵǾ���ҵ�

SamplerState objSamplerState : SAMPLER : register(s0);
SamplerComparisonState PCFSample : SAMPLER : register(s1);

PS_GBUFFER_OUT main(VS_OUTPUT pin) : SV_Target
{
    PS_GBUFFER_OUT deferredOut;
     
    // �˺��� ���� ������ Material�� �˺��� ���� �����ͼ� �׷��ش�
    deferredOut.Albedo = objTexture.Sample(objSamplerState, pin.outTexCoord) * DiffuseAlbedo;
    
    // metal-rough�ϱ� ��Ż�� ����
    float4 metalRoughMap = objMetalRough.Sample(objSamplerState, pin.outTexCoord);
    
    float nowMetalFactor = saturate(metalRoughMap.r * metallic);
    float nowRoughnessFactor = saturate(metalRoughMap.g * smoothness);
    deferredOut.Material = float4(nowMetalFactor, nowRoughnessFactor, 0.0, 1.0);
    
    // �븻
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
    
    // ���� �븻�̶� ������ �ݻ縦 ���� ������ ���� ���Ѵ�
    float3 _posNormal = normalize(deferredOut.NormalMap.xyz + deferredOut.Position.xyz);
    
    {
        // IBL �̹����� �����Ʈ�� ����
        // �������� ����
        float3 irradianceColor = irradianceMap.Sample(objSamplerState, _posNormal).rgb;
    
        // ��Ż�����κ��� ���������� ���Ѵ�
        float3 viewDir = -normalize(EyePos.xyz - deferredOut.Position.xyz);
        float3 F0 = lerp(float3(0.04, 0.04, 0.04), deferredOut.Albedo.xyz, nowMetalFactor);
    
        // �����Ͻ��� ���� F���� ���Ѵ�
        float perceptualRoughness = SmoothnessToPerceptualRoughness(nowRoughnessFactor);
        float roughness = PerceptualRoughnessToRoughness(perceptualRoughness);
        float3 F = FresnelSchlickRoughness(max(dot(_posNormal, viewDir), 0.0), F0, roughness);
    
        float3 kS = F;
        float3 kD = 1.0 - kS;
        kD *= 1.0 - metallic;
    
        // �������� �ʿ��� �ݻ�� ����ŧ���� �����´�
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
        // �̹̽ú��
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