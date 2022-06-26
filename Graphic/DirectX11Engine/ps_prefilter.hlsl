#include "IBLLibrary.hlsl"

TextureCube shaderTexture : register(t0);
SamplerState textureSampler : register(s0);

cbuffer cbRoughness : register(b0)
{
    float roughness;
    float pad;
    float pad1;
    float pad2;
};

struct VS_OUTPUT
{
    float4 outPosition : SV_POSITION;
    float3 localPosition : POSITION0;
    float2 outUV : TEXCOORD;
};


float4 main(VS_OUTPUT pin) : SV_Target0
{
    float3 N = normalize(pin.localPosition);
    float3 R = N;
    float3 V = R;

    const uint SAMPLE_COUNT = 1024u;
    float totalWeight = 0.0;
    float3 prefilteredColor = float3(0.0, 0.0, 0.0);
    for (uint i = 0u; i < SAMPLE_COUNT; ++i)
    {
        float2 Xi = Hammersley(i, SAMPLE_COUNT);
        float3 H = ImportanceSampleGGX(Xi, N, roughness);
        float3 L = normalize(2.0 * dot(V, H) * H - V);

        float NdotL = max(dot(N, L), 0.0);
        if (NdotL > 0.0)
        {
            prefilteredColor += shaderTexture.Sample(textureSampler, L).rgb * NdotL;
            totalWeight += NdotL;
        }
    }
    prefilteredColor = prefilteredColor / totalWeight;

    return float4(prefilteredColor, 1.0);
    
}