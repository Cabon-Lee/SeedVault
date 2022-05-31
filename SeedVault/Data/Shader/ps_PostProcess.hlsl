Texture2D<float4> gTexture00 : register(t0);
Texture2D<float4> gTexture01 : register(t1);

SamplerState LinearSampler : register(s0);
SamplerState PointSampler : register(s1);

//-----------------------------------------------------------------------------------------
// Vertex shader
//-----------------------------------------------------------------------------------------
struct VS_OUTPUT
{
    float4 Position : SV_Position; // vertex position 
    float2 UV : TEXCOORD0;
};

float4 Merge(VS_OUTPUT In) : SV_TARGET
{
    // Get the color sample
    float3 color = gTexture00.Sample(PointSampler, In.UV.xy).xyz;
    
	// Add the bloom contribution
    color += gTexture01.Sample(LinearSampler, In.UV.xy).xyz;
    
	// Output the LDR value
    return float4(color, 1.0);
}

float4 Sampling(VS_OUTPUT In) : SV_TARGET
{
    float3 color = gTexture00.Sample(LinearSampler, In.UV.xy).xyz;
    return float4(color, 1.0);
}