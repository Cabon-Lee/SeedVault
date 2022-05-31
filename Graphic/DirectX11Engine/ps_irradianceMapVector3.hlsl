#include "IBLLibrary.hlsl"

TextureCube environmentMap : TEXTURE : register(t0);
SamplerState objSamplerState : SAMPLER : register(s0);

struct VS_OUTPUT
{
    float4 outPosition : SV_POSITION;
    float3 localPosition : POSITION0;
    float2 outUV : TEXCOORD;
};

float4 main(VS_OUTPUT pin) : SV_Target0
{
        // the sample direction equals the hemisphere's orientation 
    float3 normal = normalize(pin.localPosition);

    float3 irradiance = float3(0.0f, 0.0f, 0.0f);

    float3 up = float3(0.0f, 1.0f, 0.0f);
    float3 right = cross(up, normal);
    up = cross(normal, right);

    float sampleDelta = 0.025f;
    float nrSamples = 0.0f;
    
    for (float phi = 0.0f; phi < 2.0f * PI; phi += sampleDelta)
    {
        for (float theta = 0.0f; theta < 0.5f * PI; theta += sampleDelta)
        {
            // spherical to cartesian (in tangent space)
            float3 tangentSample = float3(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));
            // tangent space to world
            float3 sampleVec = tangentSample.x * right + tangentSample.y * up + tangentSample.z * normal;
            irradiance += environmentMap.Sample(objSamplerState, sampleVec).rgb * cos(theta) * sin(theta);
            nrSamples++;
        }
    }

    irradiance = PI * irradiance * (1.0 / float(nrSamples));

    return float4(irradiance, 1.0f);
}