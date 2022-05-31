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


    // ������ ��� ����� ���� ��ġ ���͸� 4 ������ �����մϴ�.
    
    // ����, �� �� ���� ��Ŀ� ���� ������ ��ġ�� ??����մϴ�.
    output.position = mul(float4(input.position, 1.0), worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);

    // �ȼ� ���̴��� �ؽ�ó ��ǥ�� �����Ѵ�.
    output.tex = input.tex;

    // �ȼ� ���̴��� ���� ������ �����մϴ�. 
    output.color = input.color;

    return output;
}