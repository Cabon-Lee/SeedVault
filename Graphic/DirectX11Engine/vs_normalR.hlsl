#pragma pack_matrix( row_major )

cbuffer perObjectBuffer : register(b0)
{
    //float4x4 wvpMatrix;
    float4x4 worldMatrix;
    float4x4 matView;
    float4x4 matViewProjection;
};

struct VS_INPUT
{
    float4 Pos : POSITION;
    float3 Normal : NORMAL;
};

struct VS_OUTPUT
{
    float4 Pos : SV_POSITION;
    float3 Normal : NORMAL;
};

//float4x4 view_proj_matrix : register(c0);
//float4x4 view_matrix;

VS_OUTPUT main(VS_INPUT In)
{
    VS_OUTPUT Out = (VS_OUTPUT) 0;
    
    In.Pos.w = 1.0f;
    In.Pos = mul(In.Pos, worldMatrix);
    
    Out.Pos = mul(In.Pos, matViewProjection);
    Out.Normal = mul(float4(0.0f, 0.0f, 0.0f, 0.f), matView);
    //Out.Normal = mul(float4(In.Normal, 0.f), matView);
        
    //Out.C0 = float4(normal * 0.5 + 0.5, 1);
    
    return Out;
}