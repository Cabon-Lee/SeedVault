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
    // ���� ������ ���Ե�
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

//Texture2D textureShadow ���۵尡 �ϼ��Ǹ� ������ �ʵ� ���⼭ ����
// The output value that will be stored in a render target.
// The index indicates which of the 8 possibly bound render targets to write to.
// The value is available to all shaders.
// ���� Ÿ�ٿ� ����� ��°�, �ε����� 0~8 �� ���ε� �� ���� Ÿ���� � �������� ��Ÿ����

struct VS_OUTPUT
{
    // ���ؽ� ���̴����� �������� �������ش�.
    // �� �������� �ٰŷ� �ȼ� ���̴��� ��ġ�� ����ش�.
    // ���� ��ȯ, ������(����) ��ȯ
    
    // ������ ������ �ȼ� ���̴��� ��ġ�� ����� �� ��. w ���� 0.0�̾��� ��.
    float4 Position : SV_Position; // vertex position 
    float2 Tex : TEXCOORD0;
};

// SV_Target�� �ֺ��̴��� �𸣰ڳ�
float4 main(VS_OUTPUT pin) : SV_Target
{
    float4 color = float4(textureAlbedo.Sample(objSamplerState, pin.Tex));
    float4 normal = float4(textureNormal.Sample(objSamplerState, pin.Tex));
    float4 material = float4(textureMaterial.Sample(objSamplerState, pin.Tex));
    float4 ambient = float4(textureAmbient.Sample(objSamplerState, pin.Tex));
    float4 shadow = float4(textureShadow.Sample(pointSampler, pin.Tex));
    float4 lightTexture = float4(textureLight.Sample(pointSampler, pin.Tex));
    float4 emissive = float4(textrueEmissive.Sample(pointSampler, pin.Tex));
    
    // ��ü�� ���� �ȼ��̶�� �ش� �÷����� �Ѱ��ش�
    if (length(ambient.xyz) + length(normal.xyz) == 0.0)
        return color;
    
    // �ں��Ʈ�� �����Ѵ�
        float3 calcAmbient = float3(ambient.xyz * color.xyz);
    
    float metallic = material.r;
    float roughness = material.g;
    
    // 0~1�� ����� �븻�� �ٽ� -1~1�� ����Ѵ�
    float3 _nowNormal = normalize((normal - 0.5f) * 2.0f);
   
        // ������ ���ϱ�
    float4 position;
    {
        position = float4(texturePosition.Sample(pointSampler, pin.Tex));
    }
   
    float3 result = 0.0f;
    // ����Ʈ�� ���� lightType[0];
    uint _round = 0;
    //[unroll]
    for (uint i = 0; i < lightCount; ++i)
    {
        uint nowType = lightInfo[_round].shadowInfo.x;
        uint isCastShadow = lightInfo[_round].shadowInfo.y;
        uint isUseTexture = lightInfo[_round].shadowInfo.w; // �ؽ��ĸ� ������ �ƴ��� �Ǵ�
        
        switch (isCastShadow)
        {
                // �׸��� ����
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