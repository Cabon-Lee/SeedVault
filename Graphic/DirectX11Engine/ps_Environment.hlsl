#include "IBLLibrary.hlsl"

Texture2D equirectangularMap : TEXTURE : register(t0);
SamplerState objSamplerState : SAMPLER : register(s0);

struct VS_OUTPUT
{
    float4 outPosition : SV_POSITION;
    float3 localPosition : POSITION0;
    float2 outUV : TEXCOORD;
};

float4 main(VS_OUTPUT pin) : SV_Target0
{
    float2 uv = SampleSphericalMap(normalize(pin.localPosition)); // make sure to normalize localPos 
    float3 color = equirectangularMap.Sample(objSamplerState, float2(uv.x, -uv.y));
    return float4(color, 1.0);
}