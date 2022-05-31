////////////////////////////////////////////////////////////////////////////////
// Filename: particle_vs.hlsl
////////////////////////////////////////////////////////////////////////////////
#pragma pack_matrix( row_major )


/////////////
// GLOBALS //
/////////////
cbuffer MatrixBuffer
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
};


//////////////
// TYPEDEFS //
//////////////
struct VertexInputType
{
    float3 position : POSITION;
    float2 tex : TEXCOORD0;
    float4 color : COLOR;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float4 color : COLOR;
};


////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
PixelInputType main(VertexInputType input)
{
    PixelInputType output;


    // 적절한 행렬 계산을 위해 위치 벡터를 4 단위로 변경합니다.
    
    // 월드, 뷰 및 투영 행렬에 대한 정점의 위치를 ??계산합니다.
    output.position = mul(float4(input.position, 1.0), worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);

    // 픽셀 쉐이더의 텍스처 좌표를 저장한다.
    output.tex = input.tex;

    // 픽셀 쉐이더의 입자 색상을 저장합니다. 
    output.color = input.color;

    return output;
}