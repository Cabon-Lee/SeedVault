// �ٱ����� ��Ʈ������ �����´�

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
    // ���� ��� ���� ����Ʈ�� ����, ����Ʈ Ÿ�Ե� �Ѿ�;��Ѵ�
}

// ���� Ÿ���� ���δ�?
struct PS_GBUFFER_OUT
{
    float4 debug : SV_TARGET0;
    float4 Albedo : SV_TARGET1;
    float4 NormalMap : SV_TARGET2;
    float4 Position : SV_TARGET3; // xyz ������ + depth
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
    float3 outTangent : TANGENT;
    float4 ShadowPosH[4] : TEXCOORD1;
};

Texture2D objTexture : TEXTURE : register(t0);
Texture2D objNormal : TEXTURE : register(t1);
Texture2D objMetalRough : TEXTURE : register(t2);
Texture2D objEmissvie : TEXTURE : register(t3);
Texture2D ShadowMap[4] : TEXTURE : register(t4);
TextureCube textureSkyCubeMap : TEXTURE : register(t5);

// �ϴ��� 4�常 �� ���� �⺻������ DirectionalLight, SpotLight �ΰ��� �� ��
// ���� �غ���, �Ƹ��� ��� ������ ī��Ʈ�� ���ԵǾ���ҵ�

SamplerState objSamplerState : SAMPLER : register(s0);
SamplerComparisonState PCFSample : SAMPLER : register(s1);

PS_GBUFFER_OUT main(VS_OUTPUT pin) : SV_Target
{
    PS_GBUFFER_OUT deferredOut;
     
    // �˺��� ���� ������ Material�� �˺��� ���� �����ͼ� �׷��ش�
    deferredOut.Albedo = objTexture.Sample(objSamplerState, pin.outTexCoord) * DiffuseAlbedo;
       
    if (length(deferredOut.Albedo.xyz) == 0.0f)
    {
        deferredOut.Albedo = DiffuseAlbedo;
    }
    
    // metal-rough�ϱ� ��Ż�� ����
    float4 metalRoughMap = objMetalRough.Sample(objSamplerState, pin.outTexCoord);
    deferredOut.Material = float4(metalRoughMap.r * metallic, metalRoughMap.g * Roughness, 0.0, 1.0);        
    
    // �븻
    //float3 normal = (pin.outNormal * 0.5f + 0.5f);
    //float3 normal = (pin.outNormal * 0.5f + 0.5f);
    float3 normalMapSample = objNormal.Sample(objSamplerState, pin.outTexCoord).rgb;
    
    if (length(normalMapSample) == 0.0f)
    {
        deferredOut.NormalMap = float4((pin.outNormal * 0.5f + 0.5f), 1.0f);
    }
    else
    {
        //deferredOut.NormalMap = float4((NormalSampleToWorldSpace(normalMapSample, normal, pin.outTangent) * 0.5f) + 0.5f, 1.0f);
        deferredOut.NormalMap = float4((NormalSampleToWorldSpace(normalMapSample, pin.outNormal, pin.outTangent) * 0.5f) + 0.5f, 1.0f);
    }
    
    // �ں��Ʈ + ��Ż��
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