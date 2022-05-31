#pragma pack_matrix( row_major )

/*
행렬성분은 행위주(row- major)나 열위주(column- major)방식으로 구성된다.
열위주란 행렬의 한 열이 하나의 상수레지스터에 보관되는 방식이며, 
행위주는 행렬의 각 행이 하나의 레지스터에 보관되는방식이다.

행위주 행렬          열위주 행렬
11 12 13 14          11 21 31 41 
21 22 23 24          12 22 32 42 
31 32 33 34          13 23 33 43 
41 42 43 44          14 24 34 44 
*/

cbuffer perObjectBuffer : register(b0)
{
    float4x4 wvpMatrix;
    float4x4 worldMatrix;
};

struct VS_INPUT
{
    float3 inPos : POSITION;
    float3 inNormal : NORMAL;
    float2 inTexCoord : TEXCOORD;
};

struct VS_OUTPUT
{
    float4 outPosition : SV_POSITION;
    float3 outWorldPos : POSITION;
    float3 outNormal : NORMAL;
    float2 outTexCoord : TEXCOORD;
};

VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output;
    
    output.outPosition = mul(float4(input.inPos, 1.0f), wvpMatrix);
    output.outWorldPos = mul(float4(input.inPos, 1.0f), worldMatrix).xyz;
    output.outNormal = input.inNormal;
    output.outTexCoord = input.inTexCoord;
    return output;
}
    
