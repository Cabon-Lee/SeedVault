
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

// SV_Target은 왜붙이는지 모르겠넹
/// 왜붙이냐면 어떤 렌더 타겟에 그릴지 알려주기 위한 용도임
float4 main(VS_OUTPUT pin) : SV_Target
{
    return float4(targetTexture.Sample(linearSampler, pin.Tex));
}