
Texture2D textureColor : TEXTURE : register(t0);
Texture2D depthBuffer : TEXTURE : register(t8);

SamplerState pointSamplerState : SAMPLER : register(s0);

struct VS_OUTPUT
{
    float4 Position : SV_Position; // vertex position 
    float3 posH : POSITION;
    float2 Tex : TEXCOORD0;
};

struct OUT_TARGET
{
    float4 accum : SV_Target0;
    float4 reveal : SV_Target1;
};

OUT_TARGET main(VS_OUTPUT pin) : SV_TARGET
{
    OUT_TARGET nowtarget;
    
    float4 color = textureColor.Sample(pointSamplerState, pin.Tex);
    
    float weight = clamp(pow(min(1.0, color.a * 10.0) + 0.01, 3.0) * 1e8 *
                       pow(1.0 - pin.posH.z * 0.9, 3.0), 1e-2, 3e3);
    
    
    nowtarget.accum = float4(color.rgb * color.a, color.a) * weight;
    nowtarget.reveal.w = color.a;
    
    nowtarget.reveal.rgb = color.rgb;
    
    return nowtarget;
}