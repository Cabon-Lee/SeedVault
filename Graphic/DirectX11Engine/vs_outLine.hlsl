#pragma pack_matrix( row_major )

struct VS_INPUT
{
    float4 Pos : POSITION;
    float3 Normal : NORMAL;
};

struct VS_OUTPUT
{
    float4 Pos : SV_POSITION;
    float2 TexCoord : TEXCOORD0;
};


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


//VS_OUTPUT main(VS_INPUT In)
VS_OUTPUT main(uint VertexID : SV_VertexID)
{

    /*
    VS_OUTPUT Out;
    // sign : 음수면 -1, 양수면 1, 0이면 0을 return
    In.Pos.xy = sign(In.Pos.xy);
    Out.Pos = float4(In.Pos.xy, 0.0, 1.0);
   
   // Image-space
    Out.TexCoord.x = 0.5 * (1 + In.Pos.x);
    Out.TexCoord.y = 0.5 * (1 - In.Pos.y);
    return Out;
    */
    
    VS_OUTPUT Output;

    Output.Pos = float4(arrBasePos[VertexID].xy, 0.0, 1.0);
    Output.TexCoord = arrUV[VertexID].xy;
    
    return Output;

}

