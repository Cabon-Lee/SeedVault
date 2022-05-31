struct PS_GBUFFER_OUT
{
    float4 debug : SV_TARGET0;
    float4 Albedo : SV_TARGET1;
    float4 NormalMap : SV_TARGET2;
    float4 Position : SV_TARGET3; // xyz Æ÷Áö¼Ç + depth
    float4 Material : SV_TARGET4; // FresnelR0 + roughness
};

struct VS_OUTPUT
{
    float4 inPosition : SV_POSITION;
    float4 inColor : COLOR;
};

PS_GBUFFER_OUT DebugInfoMain(VS_OUTPUT Input) : SV_TARGET
{
    PS_GBUFFER_OUT deferredOut;
    
    deferredOut.debug = Input.inColor;
    deferredOut.debug.w = 1.0f;
   
    return deferredOut;
}

PS_GBUFFER_OUT AlbedoMain(VS_OUTPUT pin) : SV_Target
{
    PS_GBUFFER_OUT deferredOut;
    
    deferredOut.Albedo = pin.inColor;
    deferredOut.Albedo.w = 1.0f;
   
    return deferredOut;
}