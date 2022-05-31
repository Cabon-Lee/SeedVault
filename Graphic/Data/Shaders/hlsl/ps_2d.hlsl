#include "Utility.hlsli"
    
struct PS_INPUT
{
    float4 inPosition : SV_POSITION;
    float2 inTexCoord : TEXCOORD;
};

Texture2D objTexture : TEXTURE : register(t0);
SamplerState objSamplerState : SAMPLER : register(s0);

float4 main(PS_INPUT input) : SV_TARGET0
{
    float4 sampleColor = objTexture.Sample(objSamplerState, input.inTexCoord);
    
    if (isApproximatelyEqual(sampleColor, 0.0))
        discard;
    
    return sampleColor;
}