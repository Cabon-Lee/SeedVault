#pragma pack_matrix( row_major )

struct VertexIn
{
    float3 PosL : POSITION;
    float3 NormalL : NORMAL;
    float2 Tex : TEXCOORD;
};

struct VertexOut
{
    float4 PosH : SV_POSITION;
    float2 Tex : TEXCOORD;
};

cbuffer cbPerFrame : register(b0)
{
    float gTexelWidth;
    float gTexelHeight;
    float isHorizontal;
    float pad;
};
 
// Nonnumeric values cannot be added to a cbuffer.
Texture2D gNormalDepthMap : register(t0);
Texture2D gInputImage : register(t1);
 
SamplerState samNormalDepth
{
    Filter = MIN_MAG_LINEAR_MIP_POINT;

    AddressU = CLAMP;
    AddressV = CLAMP;
};

SamplerState samInputImage
{
    Filter = MIN_MAG_LINEAR_MIP_POINT;

    AddressU = CLAMP;
    AddressV = CLAMP;
};

float4 main(VertexOut pin) : SV_Target
{
    float gWeights[11] =
    {
        0.05f, 0.05f, 0.1f, 0.1f, 0.1f, 0.2f, 0.1f, 0.1f, 0.1f, 0.05f, 0.05f
    };
    static const int gBlurRadius = 5;
    
    float2 texOffset;
    if (isHorizontal != 0.0)
    {
        texOffset = float2(gTexelWidth, 0.0f);
    }
    else
    {
        texOffset = float2(0.0f, gTexelHeight);
    }

	// The center value always contributes to the sum.
    float4 color = gWeights[5] * gInputImage.SampleLevel(samInputImage, pin.Tex, 0.0);
    float totalWeight = gWeights[5];
	 
    float4 centerNormalDepth = gNormalDepthMap.SampleLevel(samNormalDepth, pin.Tex, 0.0f);

    for (float i = -gBlurRadius; i <= gBlurRadius; ++i)
    {
		// We already added in the center weight.
        if (i == 0)
            continue;

        float2 tex = pin.Tex + i * texOffset;

        float4 neighborNormalDepth = gNormalDepthMap.SampleLevel(
			samNormalDepth, tex, 0.0f);

		//
		// If the center value and neighbor values differ too much (either in 
		// normal or depth), then we assume we are sampling across a discontinuity.
		// We discard such samples from the blur.
		//
	
        if (dot(neighborNormalDepth.xyz, centerNormalDepth.xyz) >= 0.8f &&
		    abs(neighborNormalDepth.a - centerNormalDepth.a) <= 0.2f)
        {
            float weight = gWeights[i + gBlurRadius];

			// Add neighbor pixel to blur.
            color += weight * gInputImage.SampleLevel(
				samInputImage, tex, 0.0);
		
            totalWeight += weight;
        }
    }

	// Compensate for discarded samples by making total weights sum to 1.
    return color / totalWeight;
}