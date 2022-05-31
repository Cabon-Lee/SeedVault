
#include "Utility.hlsli"

TextureCube environmentMap : TEXTURE : register(t0);
SamplerState objSamplerState : SAMPLER : register(s0);

struct PS_INPUT
{
    float4 inPosition : SV_POSITION;
    float3 inWorldPos : POSITION;
    float3 inNormal : NORMAL;
    float2 inTexCoord : TEXCOORD;
};

// �ӽ÷� �ں��Ʈ�� �ھƳ��� �ű� ������ ������ �׸�
float4 main(PS_INPUT pin) : SV_TARGET
{
    // ������ ���κ�� �̰ɷθ� �׸��ϱ� �ʱⰪ�� ����
    float4 color;
  
    color = environmentMap.Sample(objSamplerState, pin.inNormal);
    if (isApproximatelyEqual(color.xyz, 0.0))
    {
        color = 0.03;
    }
    color.w = 1.0f;
    
    return color;
}