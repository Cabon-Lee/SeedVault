#include "Deferred.hlsl"
#include "PBRLibrary.hlsli"

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
    float4 look;
    float4x4 ViewInv;
    float3 EyePos;
    // 아이 포스는 포함됨
}

//---------------------------------------------------------------------------------------
// MRT
//---------------------------------------------------------------------------------------
Texture2D textureAlbedo : TEXTURE : register(t0);
Texture2D textureNormal : TEXTURE : register(t1);
Texture2D texturePosition : TEXTURE : register(t2);
Texture2D textureMaterial : TEXTURE : register(t3);
Texture2D textureAmbient : TEXTURE : register(t4);
Texture2D textureShadow : TEXTURE : register(t5);
Texture2D textrueEmissive : TEXTURE : register(t6);
Texture2D textureLight : TEXTURE : register(t7);
Texture2D SSAOMap : TEXTURE : register(t8);


SamplerState pointSampler : SAMPLER : register(s0);
SamplerState objSamplerState : SAMPLER : register(s1);

//Texture2D textureShadow 디퍼드가 완성되면 쉐도우 맵도 여기서 찍어보자
// The output value that will be stored in a render target.
// The index indicates which of the 8 possibly bound render targets to write to.
// The value is available to all shaders.
// 렌더 타겟에 저장될 출력값, 인덱스는 0~8 중 바인딩 된 렌더 타겟이 어떤 것인지를 나타낸다

struct VS_OUTPUT
{
    // 버텍스 셰이더에서 포지션을 내보내준다.
    // 이 포지션을 근거로 픽셀 셰이더의 위치를 잡아준다.
    // 선형 변환, 어파인(아핀) 변환
    
    // 문제의 원인은 픽셀 셰이더의 위치를 못잡게 한 것. w 값이 0.0이었던 것.
    float4 Position : SV_Position; // vertex position 
    float2 Tex : TEXCOORD0;
};

// SV_Target은 왜붙이는지 모르겠넹
float4 main(VS_OUTPUT pin) : SV_Target
{
    float4 color = float4(textureAlbedo.Sample(objSamplerState, pin.Tex));
    float4 normal = float4(textureNormal.Sample(objSamplerState, pin.Tex));
    float4 material = float4(textureMaterial.Sample(objSamplerState, pin.Tex));
    float4 ambient = float4(textureAmbient.Sample(objSamplerState, pin.Tex));
    float4 shadow = float4(textureShadow.Sample(pointSampler, pin.Tex));
    float4 lightTexture = float4(textureLight.Sample(pointSampler, pin.Tex));
    float4 emissive = float4(textrueEmissive.Sample(pointSampler, pin.Tex));
    
    // 실체가 없는 픽셀이라면 해당 컬러값만 넘겨준다
    if (length(ambient.xyz) + length(normal.xyz) == 0.0)
        return color;
    
    // 앰비언트를 연산한다
        float3 calcAmbient = float3(ambient.xyz * color.xyz);
    
    float metallic = material.r;
    float roughness = material.g;
    
    // 0~1로 사상한 노말을 다시 -1~1로 사상한다
    float3 _nowNormal = normalize((normal - 0.5f) * 2.0f);
   
        // 포지션 구하기
    float4 position;
    {
        position = float4(texturePosition.Sample(pointSampler, pin.Tex));
    }
   
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
                            result += CalcLight(nowType, lightInfo[_round], color, position.xyz, _nowNormal.xyz, -normalize(look.xyz), roughness, metallic, ambient.xyz).xyz;
                            break;
                        }
                    case 1:
                        {
                            result += lightTexture.xyz * CalcLight(nowType, lightInfo[_round], color, position.xyz, _nowNormal.xyz, -normalize(look.xyz), roughness, metallic, ambient.xyz).xyz;
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
                            result += shadow[nowShadowMap] * CalcLight(nowType, lightInfo[_round], color, position.xyz, _nowNormal.xyz, -normalize(look.xyz), roughness, metallic, ambient.xyz).xyz;
                            break;
                        }
                    case 1:
                        {
                            result += lightTexture.xyz * shadow[nowShadowMap] * CalcLight(nowType, lightInfo[_round], color, position.xyz, _nowNormal.xyz, -normalize(look.xyz), roughness, metallic, ambient.xyz).xyz;
                            break;
                        }
                }
                break;
        }
        
        _round++;
        if (_round == lightCount)
            break;
       
    }
    
    float4 _finColor = float4(calcAmbient.xyz + result + emissive.xyz, 1.0);
        
    return _finColor;
}