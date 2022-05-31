#pragma pack_matrix( row_major )

cbuffer perObjectBuffer : register(b0)
{
    float4x4 wvpMatrix;
};

struct VS_OUTPUT
{
    float4 Pos : SV_POSITION;
    float4 C0 : COLOR0;
};

struct VS_INPUT
{
    float3 inPos : POSITION;
    float3 inNormal : NORMAL;
    float2 inTexCoord : TEXCOORD;
    float3 inTangent : TANGENT;
};

VS_OUTPUT main(VS_INPUT In)
{
	// Output position of the vertex, in clip space : MVP * position
    VS_OUTPUT Out = (VS_OUTPUT) 0;
    
    Out.Pos = mul(float4(In.inPos, 1.0f), wvpMatrix);
        
    Out.C0 = float4(0, 0, 0, 1);
    
    return Out;
}