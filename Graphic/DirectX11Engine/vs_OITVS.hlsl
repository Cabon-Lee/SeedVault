#pragma pack_matrix( row_major )

struct STD_VS_INPUT
{
    float3 pos : POSITION;
    float3 normal : NORMAL;
    float2 tex : TEXCOORD;
    //float3 tangent : TANGENT;
};

struct VS_OUTPUT
{
    float4 pos : SV_POSITION;
    float3 wPos : TEXCOORD1;
    float4 pPos : TEXCOORD2;
    float3 normal : TEXCOORD3;
    float2 tex : TEXCOORD4;
    //float3 tangent : TEXCOORD4;
};

cbuffer CB_VS_PROPERTY : register(b0)
{
    float4x4 WMat;
    float4x4 VMat;
    float4x4 PMat;
    float4x4 NMat;
    
    //not use
    float near;
    float far;
    float aspect;
    float scnRatio;
};

VS_OUTPUT main(STD_VS_INPUT input)
{
    VS_OUTPUT output;
    
    float4 wPos = mul(WMat, float4(input.pos, 1));
    float4 vPos = mul(VMat, wPos);
    float4 pPos = mul(PMat, vPos);
   
    output.wPos = wPos.xyz;
    output.pos = pPos;
    output.pPos = pPos;
    output.normal = mul((float3x3) NMat, input.normal);
    output.tex = input.tex;
    //output.tangent = mul((float3x3) WMat, input.tangent);
    
    return output;
}