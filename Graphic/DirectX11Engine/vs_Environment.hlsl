#pragma pack_matrix( row_major )

cbuffer cbPerObject : register(b0)
{
    float4x4 wvpMatrix;
};

struct VS_INPUT
{
    float3 inPos : POSITION;
    float2 inUV : TEXCOORD;
};

struct VS_OUTPUT
{
    float4 outPosition : SV_POSITION;
    float3 localPosition : POSITION0;
    float2 outUV : TEXCOORD;
};

VS_OUTPUT main(VS_INPUT vin)
{
    VS_OUTPUT vout;
    
    vout.outPosition = mul(float4(vin.inPos, 1.0f), wvpMatrix);
    vout.localPosition = vin.inPos;
    vout.outUV = vin.inUV;
    
    return vout;
}