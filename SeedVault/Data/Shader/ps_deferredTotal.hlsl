Texture2D<float4> objTexture : register(t0);
Texture2D<float4> objTarget : register(t1);


SamplerState linearSampler : register(s0);

static const float2 arrBasePos[4] =
{
    float2(-1.0, 1.0),
	float2(1.0, 1.0),
	float2(-1.0, -1.0),
	float2(1.0, -1.0),
};

static const float2 arrUV[4] =
{
    float2(0.0, 0.0),
	float2(1.0, 0.0),
	float2(0.0, 1.0),
	float2(1.0, 1.0),
};

struct VS_OUTPUT
{
    float4 inPosition : SV_POSITION;
    float4 inColor : COLOR;
};

float4 DebugInfoMain(VS_OUTPUT Input) : SV_TARGET
{
    float4 color;
    
    color = Input.inColor;
    color.w = 1.0f;
   
    return color;
}

float4 AlbedoMain(VS_OUTPUT pin) : SV_Target
{
    float4 color;
    
    color = pin.inColor;
    color.w = 1.0f;
   
    return color;
}

struct VS_POSTION_UV
{
    float4 Position : SV_Position; // vertex position 
    float2 UV : TEXCOORD0;
};

VS_POSTION_UV FullScreenQuadVS(uint VertexID : SV_VertexID)
{
    VS_POSTION_UV Output;

    Output.Position = float4(arrBasePos[VertexID].xy, 0.0, 1.0);
    Output.UV = arrUV[VertexID].xy;
    
    return Output;
}

float4 DebugRender(VS_POSTION_UV In) : SV_TARGET
{
    float3 color = objTexture.Sample(linearSampler, In.UV.xy).xyz;
    
    if (length(color) == 0.0f)
    {
        return float4(color, 0.0);
    }
    else
    {
        return float4(color, 1.0);
    }
}

float4 NormalRender(VS_POSTION_UV In) : SV_TARGET
{
    float3 color = objTexture.Sample(linearSampler, In.UV.xy).xyz;
    
    return float4(color, 1.0);
}

float4 Merge(VS_POSTION_UV In) : SV_TARGET
{
    float3 _blurColor = objTexture.Sample(linearSampler, In.UV.xy).xyz;
    float4 _targetColor = objTarget.Sample(linearSampler, In.UV.xy);
    
    float3 _finColor = _targetColor.rgb + _blurColor.rgb;
    
    return float4(_finColor, 1.0);
}