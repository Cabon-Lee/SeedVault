Texture2D accumulation : TEXTURE : register(t0);
SamplerState pointSamplerState : SAMPLER : register(s0);

struct VS_OUTPUT
{
    float4 Position : SV_Position; // vertex position 
    float2 Tex : TEXCOORD0;
};

float4 main(VS_OUTPUT pin) : SV_TARGET
{
    float4 accumColor = accumulation.Sample(pointSamplerState, pin.Tex);
    
    accumColor.rgb *= 0.1;
    
    return float4(accumColor.rgb, 0.0f);
}