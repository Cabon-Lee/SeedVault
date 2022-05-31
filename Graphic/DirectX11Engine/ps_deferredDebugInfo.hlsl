struct PS_GBUFFER_OUT
{
    float4 debug : SV_TARGET0;
    float4 Albedo : SV_TARGET1;
    float4 NormalW : SV_TARGET2;
    float4 NormalMap : SV_TARGET3;
    float4 Position : SV_TARGET4; // xyz Æ÷Áö¼Ç + depth
    float4 Material : SV_TARGET5; // FresnelR0 + roughness
};

struct VS_OUTPUT
{
    float4 inPosition : SV_POSITION;
    float4 inColor : COLOR;
};


PS_GBUFFER_OUT main(VS_OUTPUT pin) : SV_Target
{
    PS_GBUFFER_OUT deferredOut;
    
    deferredOut.debug = pin.inColor;
    deferredOut.debug.w = 1.0f;
   
    return deferredOut;
}
