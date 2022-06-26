#include "Deferred.hlsl"
#include "PBRLibrary.hlsli"
#include "Utility.hlsli"

cbuffer Light : register(b0)
{
    Light lightInfo[24];
}

cbuffer LightCount : register(b1)
{
    uint4 lightType[6];
    uint lightCount;
}

cbuffer objectPer : register(b2)
{
    float4 EyePos;
}

struct VS_OUTPUT
{
    float4 Position : SV_Position; // vertex position 
    float2 Tex : TEXCOORD0;
};

struct OUT_TARGET
{
    float4 accum : SV_Target0;
    float4 reveal : SV_Target1;
};

Texture2D textureAlbedo : TEXTURE : register(t0);
Texture2D textureNormal : TEXTURE : register(t1);
Texture2D textrueNormalDepth : TEXTURE : register(t2);
Texture2D texturePosition : TEXTURE : register(t3);
Texture2D textureMaterial : TEXTURE : register(t4);
Texture2D textureAmbient : TEXTURE : register(t5);
Texture2D textureShadow : TEXTURE : register(t6);
Texture2D textureLight : TEXTURE : register(t7);

SamplerState pointSampler : SAMPLER : register(s0);
SamplerState objSamplerState : SAMPLER : register(s1);

OUT_TARGET main(VS_OUTPUT pin) : SV_TARGET
{
    OUT_TARGET nowtarget;
    
    uint4 packedColor = uint4(textureAlbedo.Sample(objSamplerState, pin.Tex));
    float4 normal = float4(textureNormal.Sample(objSamplerState, pin.Tex));
    float4 material = float4(textureMaterial.Sample(objSamplerState, pin.Tex));
    float4 ambient = float4(textureAmbient.Sample(objSamplerState, pin.Tex));
    float4 shadow = float4(textureShadow.Sample(pointSampler, pin.Tex));
    float4 lightTexture = float4(textureLight.Sample(pointSampler, pin.Tex));
    float4 position = float4(texturePosition.Sample(pointSampler, pin.Tex));
    
    uint4 _16color = 0x0000ffff & packedColor;
    float4 emissive = f16tof32(_16color);
    
    _16color = packedColor >> 16;
    float4 color = f16tof32(_16color);
    
    float alphaStore = color.a;
    
    // 실체가 없는 픽셀이라면 해당 컬러값만 넘겨준다
    if (isApproximatelyEqual(normal.w, 0.0))
    {
        float weight = clamp(pow(min(1.0, alphaStore * 10.0) + 0.01, 3.0) * 1e8 *
                         pow(1.0 - position.z * 0.9, 3.0), 1e-2, 3e3);

        nowtarget.accum = float4(color.rgb * alphaStore, alphaStore) * weight;
        nowtarget.reveal = alphaStore;
    
        return nowtarget;
    }
    
    // 앰비언트를 연산한다
    float3 calcAmbient = float3(ambient.xyz);
    
    float metallic = material.r;
    float roughness = material.g;
    
    // 0~1로 사상한 노말을 다시 -1~1로 사상한다
    float3 _nowNormal = (normal - 0.5f) * 2.0f;
    float3 toEyeW = normalize(EyePos.xyz - position.xyz);
    
    float3 result = 0.0f;
    // 라이트의 숫자 lightType[0];
    uint _round = 0;
    //[unroll]
    for (uint i = 0; i < lightCount; ++i)
    {
        uint nowType = lightInfo[_round].shadowInfo.x;
        uint isCastShadow = lightInfo[_round].shadowInfo.y;
        uint isUseTexture = lightInfo[_round].shadowInfo.w; // 텍스쳐를 쓰는지 아닌지 판단
        
        switch (isCastShadow)
        {
                // 그림자 없음
            case 0:
                switch (isUseTexture)
                {
                    case 0:
                        {
                            //result += CalcLight(nowType, lightInfo[_round], color, position.xyz, _nowNormal.xyz, -normalize(look.xyz), roughness, metallic, ambient.xyz).xyz;
                            result += CalcLight(nowType, lightInfo[_round], color, position.xyz, _nowNormal.xyz,
                    toEyeW.xyz, roughness, metallic, ambient.xyz).xyz;
                            break;
                        }
                    case 1:
                        {
                            result += lightTexture.xyz *
                    CalcLight(nowType, lightInfo[_round], color, position.xyz, _nowNormal.xyz,
                    toEyeW.xyz, roughness, metallic, ambient.xyz).xyz;
                            break;
                        }
                        
                }
                break;
            case 1:
                uint nowShadowMap = lightInfo[_round].shadowInfo.z;
                switch (isUseTexture)
                {

                    case 0:
                        {
                            result += shadow[nowShadowMap] *
                    CalcLight(nowType, lightInfo[_round], color, position.xyz, _nowNormal.xyz,
                    toEyeW.xyz, roughness, metallic, ambient.xyz).xyz;
                            break;
                        }
                    case 1:
                        {
                            result += lightTexture.xyz * shadow[nowShadowMap] *
                    CalcLight(nowType, lightInfo[_round], color, position.xyz, _nowNormal.xyz,
                    toEyeW.xyz, roughness, metallic, ambient.xyz).xyz;
                            break;
                        }
                }
                break;
        }
        
        _round++;
        if (_round == lightCount)
            break;
       
    }
    
    // weight function
    float weight = clamp(pow(min(1.0, alphaStore * 10.0) + 0.01, 3.0) * 1e8 *
                         pow(1.0 - position.w * 0.9, 3.0), 1e-2, 3e3);

    // store pixel color accumulation
    nowtarget.accum = float4(result.rgb * alphaStore, alphaStore) * weight;

    // store pixel revealage threshold
    nowtarget.reveal.rgb = 0.0;
    nowtarget.reveal.w = alphaStore;
    
    return nowtarget;
}