
Texture2D textureColor : TEXTURE : register(t0);
SamplerState pointSamplerState : SAMPLER : register(s0);

struct VS_OUTPUT
{
    float4 outPosition : SV_POSITION;
    float3 outWorldPos : POSITION;
    float3 outNormal : NORMAL;
    float2 outTexCoord : TEXCOORD;
};

float4 main(VS_OUTPUT pin) : SV_TARGET
{
    float4 color = textureColor.Sample(pointSamplerState, pin.outTexCoord);
    return color;
}