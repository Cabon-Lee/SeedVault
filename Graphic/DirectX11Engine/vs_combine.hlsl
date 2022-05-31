#pragma pack_matrix( row_major )


cbuffer cbPerObject : register(b0)
{
    float4x4 wvpMatrix;
    float4x4 worldMatrix;
    float4x4 worldMatrixInverse;
};

struct VS_2D_IN
{
    float3 PosL : POSITION;
    float2 Tex : TEXCOORD;
};

struct VS_OUTPUT
{
    // 버텍스 셰이더에서 포지션을 내보내준다.
    // 이 포지션을 근거로 픽셀 셰이더의 위치를 잡아준다.
    // 선형 변환, 어파인(아핀) 변환
    
    // 문제의 원인은 픽셀 셰이더의 위치를 못잡게 한 것. w 값이 0.0이었던 것.
    float4 Position : SV_Position; // vertex position 
    float2 Tex : TEXCOORD0;
};

VS_OUTPUT main(VS_2D_IN vin)
{
    VS_OUTPUT Output;
    
    Output.Position = float4(mul(float4(vin.PosL, 1.0f), worldMatrix).xyz, 1.0f);
    Output.Tex = vin.Tex;
    
    return Output;
}