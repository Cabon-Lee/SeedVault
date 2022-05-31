TextureCube gCubeMap : register(t0);
SamplerState samTriLinearSam : register(s0);

struct VertexOut
{
    float4 PosH : SV_POSITION;
    float3 PosL : POSITION;
    float3 Normal : NORMAL;
    float2 Tex : TEXCOORD;
};

// SV_Target1 몇 번째 렌더타겟을 묶을 것인지 직접 설정할 수 있다, 알베도를 타겟으로함
float4 main(VertexOut pin) : SV_Target1
{
    return gCubeMap.Sample(samTriLinearSam, pin.Normal);
}
