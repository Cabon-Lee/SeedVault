cbuffer perObjectBuffer : register(b0)
{
    float4 IDColor;
};

//struct PS_INPUT
//{
//    float4 Pos : SV_POSITION;
//    float4 C0 : COLOR0;
//};

struct PS_INPUT
{
    float4 outPosition : SV_POSITION;
    float3 outWorldPos : POSITION;
    float4 C0 : COLOR0;
};

float4 main(PS_INPUT input) : SV_TARGET
{
    input.C0 = IDColor;
    
    return input.C0;
}