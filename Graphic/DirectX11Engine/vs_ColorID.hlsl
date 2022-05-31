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
    float4 inColor : COLOR;
};

VS_OUTPUT main(VS_INPUT In)
{
    VS_OUTPUT Out = (VS_OUTPUT) 0;
    
    Out.Pos = mul(float4(In.inPos, 1.0f), wvpMatrix);
    Out.C0 = float4(In.inColor.r / 255.0, In.inColor.g / 255.0, In.inColor.b / 255.0, 0.0);
    
    return Out;
}