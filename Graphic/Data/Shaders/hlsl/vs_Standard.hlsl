#pragma pack_matrix( row_major )

/*
��ļ����� ������(row- major)�� ������(column- major)������� �����ȴ�.
�����ֶ� ����� �� ���� �ϳ��� ����������Ϳ� �����Ǵ� ����̸�, 
�����ִ� ����� �� ���� �ϳ��� �������Ϳ� �����Ǵ¹���̴�.

������ ���          ������ ���
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
    
