#include "light.hlsl"

#define MEDIUMP_FLT_MAX    65504.0
#define saturateMediump(x) min(x, MEDIUMP_FLT_MAX)

#define PI 3.14159265359

#define DielectricSpec float4(0.04, 0.04, 0.04, 1 - 0.04)

float distributionGGX(float3 N, float3 H, float roughness)
{
    float a2 = roughness * roughness * roughness * roughness;
    float NdotH = max(dot(N, H), 0.0);
    float denom = (NdotH * NdotH * (a2 - 1.0) + 1.0);
    return a2 / (PI * denom * denom);
}

float DistributionTerm(float roughness, float ndoth)
{
    float r2 = roughness * roughness;
    float d = (ndoth * r2 - ndoth) * ndoth + 1.0;
    return r2 / (d * d * PI);
}

float VisibilityTerm(float roughness, float ndotv, float ndotl)
{
    float r2 = roughness * roughness;
    float gv = ndotl * sqrt(ndotv * (ndotv - ndotv * r2) + r2);
    float gl = ndotv * sqrt(ndotl * (ndotl - ndotl * r2) + r2);
    return 0.5 / max(gv + gl, 0.00001);
}

float geometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;
    return NdotV / (NdotV * (1.0 - k) + k);
}

float geometrySmith(float3 N, float3 V, float3 L, float roughness)
{
    return geometrySchlickGGX(max(dot(N, L), 0.0), roughness) *
           geometrySchlickGGX(max(dot(N, V), 0.0), roughness);
}

float3 FfresnelSchlick(float cosTheta, float3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

float3 FresnelSchlickRoughness(float cosTheta, float3 F0, float roughness)
{
    return F0 + (max(float3(1.0 - roughness, 1.0 - roughness, 1.0 - roughness), F0) - F0) * pow(1.0 - cosTheta, 5.0);
}


// 스페큘러의 확산광 미세면 분포도 D
// 이게 제일 이쁨
float D_GGX(float roughness, float NoH, const float3 n, const float3 h)
{
    float3 NxH = cross(n, h);
    float a = NoH * roughness;
    float k = roughness / (dot(NxH, NxH) + a * a);
    //float k = roughness / (clamp(dot(NxH, NxH), 0.0, 1.0) + a * a);
    float d = k * k * (1.0 / PI);
    return saturateMediump(d);
}

float D_GGX(float NoH, float a)
{
    float a2 = a * a;
    float f = (NoH * a2 - NoH) * NoH + 1.0;
    return a2 / (PI * f * f);
}

// GGX/Trowbridge-Reitsz Normal Distribution Function
float D_GGX_TrowbridgeReitz(float alpha, float3 N, float3 H)
{
    float numerator = pow(alpha, 2.0);
    float NdotH = max(dot(N, H), 0.0);
    float denominator = PI * pow(pow(NdotH, 2.0) * (pow(alpha, 2.0) - 1.0) + 1.0, 2.0);
    denominator = max(denominator, 0.000001);
    
    return numerator / denominator;
}

float chiGGX(float v)
{
    return v > 0 ? 1 : 0;
}

float GGX_Distribution(float3 n, float3 h, float alpha)
{
    float NoH = dot(n, h);
    float alpha2 = alpha * alpha;
    float NoH2 = NoH * NoH;
    float den = NoH2 * alpha2 + (1 - NoH2);
    return (chiGGX(NoH) * alpha2) / (PI * den * den);
}

float D_GGXaniso(float ax, float ay, float NoH, float3 H, float3 X, float3 Y)
{
    float XoH = dot(X, H);
    float YoH = dot(Y, H);
    float d = XoH * XoH / (ax * ax) + YoH * YoH / (ay * ay) + NoH * NoH;
    return 1 / (PI * ax * ay * d * d);
}

// 스페큘러의 기하 감쇠율 G
float V_SmithGGXCorrelated(float NoV, float NoL, float roughness)
{
    float a2 = roughness * roughness;
    float GGXV = NoL * sqrt(NoV * NoV * (1.0 - a2) + a2);
    float GGXL = NoV * sqrt(NoL * NoL * (1.0 - a2) + a2);
    return 0.5 / (GGXV + GGXL);
}

float Vis_SmithJointApprox(float a2, float NoV, float NoL)
{
    float a = sqrt(a2);
    float Vis_SmithV = NoL * (NoV * (1 - a) + a);
    float Vis_SmithL = NoV * (NoL * (1 - a) + a);
    return 0.5 * rcp(Vis_SmithV + Vis_SmithL);
}

inline float SmithJointGGXVisibilityTerm(float NdotL, float NdotV, float roughness)
{
    // Approximation of the above formulation (simplify the sqrt, not mathematically correct but close enough)
    float a = roughness;
    float lambdaV = NdotL * (NdotV * (1 - a) + a);
    float lambdaL = NdotV * (NdotL * (1 - a) + a);
    return 0.5f / (lambdaV + lambdaL + 1e-5f);
}

float GGX_PartialGeometryTerm(float3 v, float3 n, float3 h, float alpha)
{
    float VoH2 = saturate(dot(v, h));
    float chi = chiGGX(VoH2 / saturate(dot(v, n)));
    VoH2 = VoH2 * VoH2;
    float tan2 = (1 - VoH2) / VoH2;
    return (chi * 2) / (1 + sqrt(1 + alpha * alpha * tan2));
}

float GeometrySchlickGGX(float NdotV, float k)
{
    float nom = NdotV;
    float denom = NdotV * (1.0 - k) + k;
	
    return nom / denom;
}
  
float GeometrySmith(float3 N, float3 V, float3 L, float k)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx1 = GeometrySchlickGGX(NdotV, k);
    float ggx2 = GeometrySchlickGGX(NdotL, k);
	
    return ggx1 * ggx2;
}


// F_Schlick 공식
// F0 + (1-F0)(1-(V dot H))^5
float3 F_Schlick(float3 F0, float3 V, float3 H)
{
    return F0 + (float3(1.0, 1.0, 1.0) - F0) * pow(1 - max(dot(V, H), 0.0), 5.0);
}

float3 fresnelSchlick(float VoH, float3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - VoH, 0.0, 1.0), 5.0);
}

float3 F_Schlick(float3 specualrColor, float VoH)
{
    float Fc = pow(1 - VoH, 5);
    return saturate(50.0 * specualrColor.g) * Fc + (1 - Fc) * specualrColor;
}

float3 F_Schlick(float u, float3 f0)    // u = VoH
{
    float f = pow(1.0 - u, 5.0);
    return f + f0 * (1.0 - f);
}


float ShlickFresnel(float value)
{
    float m = clamp(1 - value, 0, 1);
    return pow(m, 5);
}

inline half4 Pow5(half4 x)
{
    return x * x * x * x * x;
}

// Disney Diffuse
// Note: Disney diffuse must be multiply by diffuseAlbedo / PI. This is done outside of this function.
float3 DisneyDiffuse(float NdotL, float NdotV, float LdotH, float perceptualRoughness)
{
    half fd90 = 0.5 + 2 * LdotH * LdotH * perceptualRoughness;
    // Two schlick fresnel term
    half lightScatter = (1 + (fd90 - 1) * Pow5(1 - NdotL));
    half viewScatter = (1 + (fd90 - 1) * Pow5(1 - NdotV));

    return lightScatter * viewScatter;
}

inline half3 FresnelTerm(half3 F0, half cosA)
{
    half t = Pow5(1 - cosA); // ala Schlick interpoliation
    return F0 + (1 - F0) * t;
}
inline half3 FresnelLerp(half3 F0, half3 F90, half cosA)
{
    half t = Pow5(1 - cosA); // ala Schlick interpoliation
    return lerp(F0, F90, t);
}

float PerceptualRoughnessToRoughness(float perceptualRoughness)
{
    return perceptualRoughness * perceptualRoughness;
}

half RoughnessToPerceptualRoughness(half roughness)
{
    return sqrt(roughness);
}


half SmoothnessToRoughness(half smoothness)
{
    return (1 - smoothness) * (1 - smoothness);
}

float SmoothnessToPerceptualRoughness(float smoothness)
{
    return (1 - smoothness);
}

float4 ToLinear(float4 v)
{
    v.rgb = pow(v.rgb, 2.2);
    return v;
}

float4 ToGamma(float4 v)
{
    v.rgb = pow(v.rgb, 1.0 / 2.2);
    return v;
}


inline half OneMinusReflectivityFromMetallic(half metallic)
{
    half oneMinusDielectricSpec = DielectricSpec.a;
    return oneMinusDielectricSpec - metallic * oneMinusDielectricSpec;
}

inline half3 DiffuseAndSpecularFromMetallic(half3 albedo, half metallic, out half3 specColor, out half oneMinusReflectivity)
{
    specColor = lerp(DielectricSpec.rgb, albedo, metallic);
    oneMinusReflectivity = OneMinusReflectivityFromMetallic(metallic);
    return albedo * oneMinusReflectivity;
}

inline float3 Unity_SafeNormalize(float3 inVec)
{
    float dp3 = max(0.001f, dot(inVec, inVec));
    return inVec * rsqrt(dp3);
}

inline float GGXTerm(float NdotH, float roughness)
{
    float a2 = roughness * roughness;
    float d = (NdotH * a2 - NdotH) * NdotH + 1.0f; // 2 mad
    return PI * a2 / (d * d + 1e-7f); // This function is not intended to be running on Mobile,
                                            // therefore epsilon is smaller than what can be represented by half
}


half SpecularStrength(half3 specular)
{
#if (SHADER_TARGET < 30)
        // SM2.0: instruction count limitation
        // SM2.0: simplified SpecularStrength
    return specular.r; // Red channel - because most metals are either monocrhome or with redish/yellowish tint
#else
        return max (max (specular.r, specular.g), specular.b);
#endif
}

// Diffuse/Spec Energy conservation
inline half3 EnergyConservationBetweenDiffuseAndSpecular(half3 albedo, half3 specColor, out half oneMinusReflectivity)
{
    oneMinusReflectivity = 1 - SpecularStrength(specColor);
#if !UNITY_CONSERVE_ENERGY
    return albedo;
#elif UNITY_CONSERVE_ENERGY_MONOCHROME
        return albedo * oneMinusReflectivity;
#else
        return albedo * (half3(1,1,1) - specColor);
#endif
}

float DistributionGGX(float3 N, float3 H, float roughness)
{
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = clamp(dot(N, H), 0.0, 0.99);
    float NdotH2 = NdotH * NdotH;
	
    float num = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
	
    return num / denom;
}

float GeometrySmith2(float3 N, float3 V, float3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.00001f);
    float NdotL = max(dot(N, L), 0.00001f);
    float ggx2 = geometrySchlickGGX(NdotV, roughness);
    float ggx1 = geometrySchlickGGX(NdotL, roughness);
   
    return ggx1 * ggx2;
}

float4 IBL_PBR_CookTorrance(float4 baseColor, float3 normal,
    float3 lightColor, float3 lightDir, float3 viewDir,
    half smoothness, half metallic, float3 ambient)
{
    
    lightDir = normalize(lightDir);
    
    smoothness = max(smoothness, 0.1);
    
    float3 H = normalize(viewDir + lightDir);
    float NoH = max(dot(normal, H), 0.0);
    float NoV = max(dot(normal, viewDir), 0.0);
    float NoL = max(dot(normal, lightDir), 0.1);
    float LoH = max(dot(lightDir, H), 0.0);
    
    float3 F0 = lerp(float3(0.04, 0.04, 0.04), baseColor.xyz, metallic);
    float perceptualRoughness = SmoothnessToPerceptualRoughness(smoothness);
    float roughness = PerceptualRoughnessToRoughness(perceptualRoughness);
    
    float3 Lo = float3(0.0, 0.0, 0.0);
    {
        float3 radiance = lightColor;

        // cook-torrance brdf
        float NDF = D_GGX(roughness, NoH, normal, H);
        float G = GeometrySmith2(normal, viewDir, lightDir, perceptualRoughness);
        float3 F = fresnelSchlick(max(dot(viewDir, H), 0.001f), F0);

        float3 kS = F;
        float3 kD = float3(1.0, 1.0, 1.0) - kS;
        kD *= 1.0 - metallic;

        float3 numerator = NDF * G * F;
        float denominator = 4.0 * max(dot(normal, viewDir), 0.0) * max(dot(normal, lightDir), 0.0) + 0.0001;
        float3 specular = numerator / max(denominator, 0.001);
        float3 diffuse = kD * baseColor.xyz * 0.31831; // 1/PI
        
        // add to outgoing radiance Lo
        //Lo += (diffuse + specular) * radiance * NdotL;
        Lo += (diffuse + specular) * radiance * NoL;
    }
    
    // 조명을 합산하지 않고 조명을 개별로 처리하는 경우
    // 오픈지엘을 보면 모든 조명을 연산한 뒤 나중에 한번에 처리
    // 후자로 가야함
    // 따라서 아래부터는 Combine에 있어야함
    
    return float4(Lo, 1.0f);
}


float4 CalcLight(
uint type,
Light L,
float4 baseColor,
float3 position,
float3 normal,
float3 lookVec,
half smoothness,
half metallic,
float3 ambient)
{
    float3 lightDir = L.Position - position;
    float4 result = 0;
    
    switch (type)
    {
        case 0: // 지향광
        {
                result = float4(IBL_PBR_CookTorrance(baseColor, normal, L.Strength, -L.Direction, lookVec, smoothness, metallic, ambient));
                break;
            }
        case 1: // 포인트 라이트
        {
                float3 lightStrenth = ComputePointLight(L, position, normal);
                result = float4(IBL_PBR_CookTorrance(baseColor, normal, lightStrenth, lightDir, lookVec, smoothness, metallic, ambient));
                break;
            }
        case 2: // 스팟 라이트
        {
                float3 lightStrenth = ComputeSpotLight(L, position, normal);
                result = float4(IBL_PBR_CookTorrance(baseColor, normal, lightStrenth, lightDir, lookVec, smoothness, metallic, ambient));
                break;
            }
    }
    
    return result;
}
