
#include "light.hlsl"

//---------------------------------------------------------------------------------------
// MRT
//---------------------------------------------------------------------------------------
Texture2D targetTexture : TEXTURE : register(t0);
SamplerState linearSampler : SAMPLER : register(s0);

struct VS_OUTPUT
{
    float4 Position : SV_Position; // vertex position 
    float2 Tex : TEXCOORD0;
};

// SV_Target�� �ֺ��̴��� �𸣰ڳ�
/// �ֺ��̳ĸ� � ���� Ÿ�ٿ� �׸��� �˷��ֱ� ���� �뵵��
float4 main(VS_OUTPUT pin) : SV_Target
{
    return float4(targetTexture.Sample(linearSampler, pin.Tex));
}