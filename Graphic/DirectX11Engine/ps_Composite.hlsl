
#include "Utility.hlsli"

struct VS_OUTPUT
{
    float4 Position : SV_Position; // vertex position 
    float2 Tex : TEXCOORD0;
};

Texture2D accum : TEXTURE : register(t0);
Texture2D reveal : TEXTURE : register(t1);

SamplerState pointSampler : SAMPLER : register(s0);
SamplerState objSamplerState : SAMPLER : register(s1);

float4 main(VS_OUTPUT pin) : SV_TARGET
{
    // fragment revealage
    float revealage = reveal.Sample(pointSampler, pin.Tex).w;
    
    // save the blending and color texture fetch cost if there is not a transparent fragment
    if (isApproximatelyEqual(revealage, 1.0f))
        discard;

    // fragment color
    float4 accumulation = float4(accum.Sample(pointSampler, pin.Tex));

    // suppress overflow
    if (isinf(max3(abs(accumulation.rgb))))
        accumulation.rgb = float3(accumulation.aaa);

    // prevent floating point precision bug
    float3 average_color = accumulation.rgb / max(accumulation.a, 0.00001);
    
    float3 emissivFactor = reveal.Sample(pointSampler, pin.Tex).rgb;
    emissivFactor += 1.0;
    average_color.rgb *= emissivFactor;
    
    // blend pixels
    return float4(average_color, 1.0f - revealage);
}