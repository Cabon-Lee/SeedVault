
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

// 임시로 앰비언트에 박아넣자 거기 넣으면 무조건 그림
float4 main(PS_INPUT pin) : SV_TARGET
{
    // 어차피 프로브는 이걸로만 그리니까 초기값을 주자
    float4 color;
  
    color = environmentMap.Sample(objSamplerState, pin.inNormal);
    if (isApproximatelyEqual(color.xyz, 0.0))
    {
        color = 0.03;
    }
    color.w = 1.0f;
    
    return color;
}