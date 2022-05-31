TextureCube gCubeMap : register(t0);
SamplerState samTriLinearSam : register(s0);

struct VertexOut
{
    float4 PosH : SV_POSITION;
    float3 PosL : POSITION;
    float3 Normal : NORMAL;
    float2 Tex : TEXCOORD;
};

// SV_Target1 �� ��° ����Ÿ���� ���� ������ ���� ������ �� �ִ�, �˺����� Ÿ��������
float4 main(VertexOut pin) : SV_Target1
{
    return gCubeMap.Sample(samTriLinearSam, pin.Normal);
}
