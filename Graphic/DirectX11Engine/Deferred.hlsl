struct SURFACE_DATA
{
    float LinearDepth;
    float3 Color;
    float3 Normal;
    float SpecPow;
    float SpecIntensity;
};

static const float2 g_SpecPowerRange = { 10.0, 250.0 };

float3 DecodeNormal(float2 encodedNormal)
{
    float4 decodedNormal = encodedNormal.xyyy * float4(2, 2, 0, 0) + float4(-1, -1, 1, -1);
    decodedNormal.z = dot(decodedNormal.xyz, -decodedNormal.xyw);
    decodedNormal.xy *= sqrt(decodedNormal.z);
    return decodedNormal.xyz * 2.0 + float3(0.0, 0.0, -1.0);
}

float ConvertZToLinearDepth(float depth, float4 PerspectiveValues)
{
    float linearDepth = PerspectiveValues.z / (depth + PerspectiveValues.w);
    return linearDepth;
}

float3 CalcWorldPos(float2 csPos, float depth, float4 PerspectiveValues, float4x4 ViewInv)
{
    float4 position;

    position.xy = csPos.xy * PerspectiveValues.xy * depth;
    position.z = depth;
    position.w = 1.0;
	
    return mul(position, ViewInv).xyz;
}
