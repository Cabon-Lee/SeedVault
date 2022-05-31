#pragma pack_matrix( row_major )

cbuffer cbPerObject : register(b0)
{
    float4x4 lightViewProjTM;
};

struct SHADOW_VS_INPUT
{
    // 범용적으로 쓸 수 있나 각을 재봐야함
    float3 PosL : POSITION;
};

struct VS_OUTPUT
{
    float4 outPosition : SV_POSITION;   // SV로 묶여있어 직접 접근이 안된다.
    float4 depthPosition : TEXTURE0;    // 위의 이유 때문에 추가로 같은 복사해서 타입을 넘겨준다
};


VS_OUTPUT main(SHADOW_VS_INPUT vin)
{
    VS_OUTPUT vout;
    
    vout.outPosition = mul(float4(vin.PosL, 1.0f), lightViewProjTM);
    vout.depthPosition = vout.outPosition;
    
    return vout;
}