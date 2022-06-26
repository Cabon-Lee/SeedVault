#include "PBRLibrary.hlsli"
#include "Utility.hlsli"

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

cbuffer ibl : register(b4)
{
    float iblFactor;
}

struct VS_OUTPUT
{
    float4 outPosition : SV_POSITION;
    float3 outWorldPos : POSITION0;
    float3 outNormal : NORMAL0;
    float2 outTexCoord : TEXCOORD0;
    float4 outTangent : TANGENT;
    float4 ShadowPosH[4] : TEXCOORD1;
    
    float3 outPosV : POSITION1;
    float3 outNormalDepth : NORMAL1;
    float2 outSSAOTex : TEXCOORD5;
};

struct PS_GBUFFER_OUT
{
    float4 Albedo : SV_TARGET0;
    float4 NormalMap : SV_TARGET1;
    float4 NormalMapDepth : SV_TARGET2;
    float4 Position : SV_TARGET3; // xyz ������ + depth
    float4 Material : SV_TARGET4; // FresnelR0 + roughness 
    float4 Ambient : SV_TARGET5; // Ambient + metallic + prefilter + irradiance
    float4 Shadow : SV_TARGET6;
    uint4 ObjectID : SV_TARGET7;
};

Texture2D objTexture : TEXTURE : register(t0);
Texture2D objNormal : TEXTURE : register(t1);
Texture2D objMetalRough : TEXTURE : register(t2);
Texture2D objEmissvie : TEXTURE : register(t3);
Texture2D ShadowMap[4] : TEXTURE : register(t4);
Texture2D brdfLUT : TEXTURE : register(t8);
TextureCube irradianceMap : TEXTURE : register(t9);
TextureCube preFilterMap : TEXTURE : register(t10);

// �ϴ��� 4�常 �� ���� �⺻������ DirectionalLight, SpotLight �ΰ��� �� ��
// ���� �غ���, �Ƹ��� ��� ������ ī��Ʈ�� ���ԵǾ���ҵ�

SamplerState objSamplerState : SAMPLER : register(s0);
SamplerComparisonState PCFSample : SAMPLER : register(s1);

PS_GBUFFER_OUT main(VS_OUTPUT pin) : SV_Target
{
    PS_GBUFFER_OUT deferredOut;
     
    
    deferredOut.NormalMapDepth = float4(normalize(pin.outNormalDepth), pin.outPosV.z);
    
    
    // �˺��� ���� ������ Material�� �˺��� ���� �����ͼ� �׷��ش�
    float4 _albedo = objTexture.Sample(objSamplerState, pin.outTexCoord) * DiffuseAlbedo;
    float4 _emissive = objEmissvie.Sample(objSamplerState, pin.outTexCoord);
    
    if (isApproximatelyEqual(_emissive, 0.0))
    {
        _emissive = float4(0.0, 0.0, 0.0, 1.0);
    }
    else
    {
        // �̹̽ú��
        _emissive = (_emissive * _emissive) * (emissiveFactor * 100);
    }
    
    uint4 _16albedo = f32tof16(_albedo);
    uint4 _16emissive = f32tof16(_emissive);
    _16albedo <<= 16;
    deferredOut.Albedo = _16albedo | _16emissive;
    
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
        deferredOut.NormalMap = float4((normalize(pin.outNormal) * 0.5f + 0.5f), 1.0f);
    }
    else
    {
        pin.outNormal = NormalSampleToWorldSpace(normalMapSample, pin.outNormal, pin.outTangent);
        deferredOut.NormalMap = float4((pin.outNormal * 0.5f) + 0.5f, 1.0f);
    }
    
    deferredOut.Position = float4(pin.outWorldPos.xyz, 1 - (pin.outPosition.z / pin.outPosition.w));
    
    {
        // IBL �̹����� �����Ʈ�� ����
        // �������� ����
        float3 irradianceColor = irradianceMap.Sample(objSamplerState, pin.outNormal).rgb;
    
        float3 indirectDiffuse = irradianceColor * _albedo.xyz;
        deferredOut.Ambient.xyz = indirectDiffuse * iblFactor;
        deferredOut.Ambient.w = 1.0;
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