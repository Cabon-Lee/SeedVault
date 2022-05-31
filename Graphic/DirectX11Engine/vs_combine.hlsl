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
    // ���ؽ� ���̴����� �������� �������ش�.
    // �� �������� �ٰŷ� �ȼ� ���̴��� ��ġ�� ����ش�.
    // ���� ��ȯ, ������(����) ��ȯ
    
    // ������ ������ �ȼ� ���̴��� ��ġ�� ����� �� ��. w ���� 0.0�̾��� ��.
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