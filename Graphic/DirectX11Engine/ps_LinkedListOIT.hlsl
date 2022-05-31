

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

RWByteAddressBuffer startOffsetBuffer : register(u0);               // RW이므로UAV이다
RWStructuredBuffer<FragmentData_STRUCT> FLBuffer : register(u1);

[earlydepthstencil]
float PS_StoreFragment(VS_OUTPUT pin) : SV_TARGET
{
    FragmentData_STRUCT _fragmentData;
    // _fragmentData여기에 일단 들어가야할 값을 집어넣는다 Depth와 함께
    
    uint uPixelCount = FLBuffer.IncrementCounter();
    
    // 여기서부터 다시 하자
    
    
    
    return 0;
}