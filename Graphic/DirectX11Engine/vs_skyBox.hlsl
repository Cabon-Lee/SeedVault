#pragma pack_matrix( row_major )

cbuffer cbPerObject : register(b0)
{
    float4x4 gWorldViewProj;
};

struct VertexIn
{
    float3 inPosL : POSITION;
    float3 inNormal : NORMAL;
    float2 inTex : TEXCOORD;
};

struct VertexOut
{
    float4 PosH : SV_POSITION;
    float3 PosL : POSITION;
    float3 Normal : NORMAL;
    float2 Tex : TEXCOORD;
};

VertexOut main(VertexIn vin)
{
    VertexOut vout;
	
	// Set z = w so that z/w = 1 (i.e., skydome always on far plane).
    vout.PosH = mul(float4(vin.inPosL, 1.0f), gWorldViewProj).xyww;
	
	// Use local vertex position as cubemap lookup vector.
    vout.PosL = vin.inPosL;
    vout.Normal = vin.inNormal;
	
    vout.Tex = vin.inTex;
    
    
    return vout;
}
