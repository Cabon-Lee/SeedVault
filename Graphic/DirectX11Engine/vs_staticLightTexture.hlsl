#pragma pack_matrix( row_major )

cbuffer cbPerObject : register(b0)
{
    float4x4 lightViewProjTM;   // ModelTM���� ����������
};

cbuffer ModelMatrix : register(b1)
{
    float4x4 wvpMatrix;
    float4x4 worldMatrix;
}

struct SHADOW_VS_INPUT
{
    // ���������� �� �� �ֳ� ���� �������
    float3 inPosition : POSITION;
    float3 inNormal : NORMAL0;
    float2 inTex : TEXCOORD0;
};

struct VS_OUTPUT
{
    float4 outPosition : SV_POSITION; // SV�� �����־� ���� ������ �ȵȴ�.
    float4 depthPosition : TEXTURE0; // ���� ���� ������ �߰��� ���� �����ؼ� Ÿ���� �Ѱ��ش�
    float3 outNormal : NORMAL0;
    float2 outTex : TEXCOORD1;
    float4 viewPosition : TEXCOORD2;
    float3 lightPos : TEXCOORD3;
};


VS_OUTPUT main(SHADOW_VS_INPUT vin)
{
    VS_OUTPUT vout;
    
    vout.outPosition = mul(float4(vin.inPosition, 1.0f), wvpMatrix);
    vout.depthPosition = vout.outPosition;
    
    vout.viewPosition = mul(float4(vin.inPosition, 1.0f), lightViewProjTM);
    vout.outNormal = normalize(mul(vin.inNormal, (float3x3) worldMatrix));
    
    vout.outTex = vin.inTex;
   
    
    return vout;
}