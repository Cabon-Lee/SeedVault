#include "ParticleLib.hlsli"

// Array of textures for texturing the particles.
//Texture2DArray gTexArray : register(t0);
Texture2D gTexArray : register(t0);

SamplerState samLinear
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = WRAP;
    AddressV = WRAP;
};

struct OUT_TARGET
{
    float4 accum : SV_Target0;
    float4 reveal : SV_Target1;
};

cbuffer ColorAdjust : register(b0)
{
    float4 colorFactor;
};

OUT_TARGET main(GeoOut pin) : SV_TARGET
{
    OUT_TARGET pout;
    
    float4 color = gTexArray.Sample(samLinear, pin.Tex);
    float depth = pin.PosH.z / pin.PosH.w;
    
    // 원래 컬러에 점점 흐려지는 컬러가 혼합되어야함
    float _nowAlpha = color.a * (pin.Color.a);
    
    float weight = clamp(pow(min(1.0, _nowAlpha * 10.0) + 0.01, 3.0) * 1e8 *
                       pow(1.0 - depth * 0.9, 3.0), 1e-2, 3e3);
    
    pout.accum = float4(color.rgb * _nowAlpha, _nowAlpha) * weight;
    pout.reveal.w = _nowAlpha;
    
    pout.reveal.rgb = 
    float3(color.r * colorFactor.r, color.g * colorFactor.g, color.b * colorFactor.b) * colorFactor.w;
    
    return pout;

}