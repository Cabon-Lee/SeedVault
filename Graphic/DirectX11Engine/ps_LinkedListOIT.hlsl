

struct FragmentData_STRUCT
{
    uint uPixelColor;
    uint uDepth;
    uint uNext;
};

struct VS_OUTPUT
{
    float4 outPosition : SV_POSITION;
    float3 outWorldPos : POSITION;
    float3 outNormal : NORMAL;
    float2 outTexCoord : TEXCOORD;
    float3 outTangent : TANGENT;
    float4 ShadowPosH[4] : TEXCOORD1;
};

RWByteAddressBuffer startOffsetBuffer : register(u0);               // RW�̹Ƿ�UAV�̴�
RWStructuredBuffer<FragmentData_STRUCT> FLBuffer : register(u1);

[earlydepthstencil]
float PS_StoreFragment(VS_OUTPUT pin) : SV_TARGET
{
    FragmentData_STRUCT _fragmentData;
    // _fragmentData���⿡ �ϴ� ������ ���� ����ִ´� Depth�� �Բ�
    
    uint uPixelCount = FLBuffer.IncrementCounter();
    
    // ���⼭���� �ٽ� ����
    
    
    
    return 0;
}