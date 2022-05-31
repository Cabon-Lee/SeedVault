#pragma pack_matrix( row_major )

cbuffer cbPerObject : register(b0)
{
    float4x4 lightViewProjTM;
};

struct SHADOW_VS_INPUT
{
    // ���������� �� �� �ֳ� ���� �������
    float3 PosL : POSITION;
};

struct VS_OUTPUT
{
    float4 outPosition : SV_POSITION;   // SV�� �����־� ���� ������ �ȵȴ�.
    float4 depthPosition : TEXTURE0;    // ���� ���� ������ �߰��� ���� �����ؼ� Ÿ���� �Ѱ��ش�
};


VS_OUTPUT main(SHADOW_VS_INPUT vin)
{
    VS_OUTPUT vout;
    
    vout.outPosition = mul(float4(vin.PosL, 1.0f), lightViewProjTM);
    vout.depthPosition = vout.outPosition;
    
    return vout;
}