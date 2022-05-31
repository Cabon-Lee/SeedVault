#define MEDIUMP_FLT_MAX    65504.0
#define saturateMediump(x) min(x, MEDIUMP_FLT_MAX)

#define PI 3.14159265359

#define DielectricSpec float4(0.04, 0.04, 0.04, 1 - 0.04)
#define GammaValue float3(0.45454545, 0.45454545, 0.45454545)

#include "PBRLibrary.hlsli"

float4 PBR_GGX(
    float4 baseColor, float3 normal,
    float3 lightColor, float3 lightDir, float3 viewDir,
    float3 f0, half smoothness, half metallic)
{
    
    float3 halfDir = normalize(lightDir + viewDir);
    
    half nv = abs(dot(normal, viewDir)); // This abs allow to limit artifact
    float nl = saturate(dot(normal, lightDir));
    float nh = saturate(dot(normal, halfDir));
    float vh = saturate(dot(viewDir, halfDir));
    half lv = saturate(dot(lightDir, viewDir));
    half lh = saturate(dot(lightDir, halfDir));

    
    // Diffuse term
    float perceptualRoughness = SmoothnessToPerceptualRoughness(smoothness);
    half diffuseTerm = DisneyDiffuse(nv, nl, lh, perceptualRoughness) * nl;
    float roughness = PerceptualRoughnessToRoughness(perceptualRoughness);
    
    roughness = max(roughness, 0.002);
    float D = D_GGX(roughness, nh, normal, halfDir);
    float V = Vis_SmithJointApprox(nl, nv, roughness);
    //float3 F = F_Schlick(f0, vh);
    
    
    float specularTerm = V * D * PI; // Torrance-Sparrow model, Fresnel is applied later
    specularTerm = max(0, specularTerm * nl);
    specularTerm = sqrt(max(1e-4h, specularTerm));

    
    half surfaceReduction;
    surfaceReduction = 1.0 - 0.28 * roughness * perceptualRoughness;
    // 1-0.28*x^3 as approximation for (1/(x^4+1))^(1/2.2) on the domain [0;1]

    specularTerm *= any(f0) ? 1.0 : 0.0;
    
    
    half3 color = baseColor.xyz * lerp(diffuseTerm, specularTerm, metallic);
    
   
    /*
//    float3 Fd = baseColor.xyz * diffuseTerm;
//    float3 Fr = (D * V) * F;
//    float3 firstLayer = (Fd + Fr) * lightColor;
    */
    //float3 color = baseColor.xyz * (lightColor * diffuseTerm) + (lightColor * V * D);
    
    return float4(color, 1.0);
   
}

float4 PBR_GGX2(
    float4 baseColor, float3 normal,
    float3 lightColor, float3 lightDir, float3 viewDir,
    float3 f0, half smoothness, half metallic)
{
    float3 fvLightDirection = normalize(lightDir);
    float3 fvNormal = normal;
    float fNDotL = dot(fvNormal, fvLightDirection);
   
    float3 fvReflection = normalize(((2.0f * fvNormal) * (fNDotL)) - fvLightDirection);
    float3 fvViewDirection = normalize(viewDir);
    float fRDotV = max(0.0f, dot(fvReflection, fvViewDirection));
   
   // basic data setup
    float3 L = fvLightDirection.xyz;
    float3 N = fvNormal;
    float3 V = fvViewDirection;
    float3 H = normalize(L + V);

    float NV = dot(N, V);
    float NL = dot(N, L);
    float VH = dot(V, H);
    float NH = dot(N, H);
   
    float alpha2 = smoothness * smoothness * smoothness * smoothness;
    float D = alpha2 / (3.141592 * pow(NH * NH * (alpha2 - 1.0) + 1.0, 2.0));
    
    float fFresnel = f0.x;
    float fresnel = fFresnel + (1.0 - fFresnel) * pow(clamp(VH, 0.0, 1.0), 5.0);
   
   
    float G = 1.0 / (NV + sqrt(alpha2 + (1.0 - alpha2) * NV * NV)) /
                    (NL + sqrt(alpha2 + (1.0 - alpha2) * NL * NL));
    
    float specular = D * fresnel * G;
   
    float diffuse = (1.0 - fresnel) * clamp(dot(N, L), 0.0, 1.0) / 3.141592;
     
    float4 fvBaseColor = ToLinear(baseColor);
   
    float4 c0 = 1;
    c0.xyz = fvBaseColor.xyz * lerp(diffuse, specular, metallic);
    c0 = ToGamma(c0);
   
    return c0;
}


float4 Model1(float4 baseColor, float3 normal,
    float3 lightColor, float3 lightDir, float3 viewDir,
    float3 f0, half smoothness, half metallic)
{
    float3 H = normalize(viewDir + lightDir);
    
    float nv = abs(dot(normal, viewDir)); // This abs allow to limit artifact
    float nl = max(dot(normal, lightDir), 0.00001);
    float nh = saturate(dot(normal, H));
    float vh = max(dot(viewDir, H), 0.0);
    float lv = saturate(dot(lightDir, viewDir));
    float lh = saturate(dot(lightDir, H));
    
    float3 F0 = float(0.04);
    F0 = lerp(F0, baseColor.rgb, metallic);
    
    float3 Lo = 0.0;
    
    float perceptualRoughness = SmoothnessToPerceptualRoughness(smoothness);
    float roughness = PerceptualRoughnessToRoughness(perceptualRoughness);
    roughness = max(roughness, 0.002);
    half diffuseTerm = DisneyDiffuse(nv, nl, lh, perceptualRoughness) * nl;
    
    float D = D_GGX(roughness, nh, normal, H);
    float G = Vis_SmithJointApprox(nl, nv, roughness);
    float3 F = F_Schlick(F0, vh);
    
    float3 kS = F;
    float3 kD = float3(1.0, 1.0, 1.0) - kS;
    kD *= 1.0 - metallic;
    
    float3 numerator = D * G * F;
    float denominator = 4.0 * max(dot(normal, viewDir), 0.0) * max(dot(normal, lightDir), 0.0) + 0.0001; // Cook-Torrance Specular
    float3 specualr = numerator / denominator; // Cook-Torrance Specular value
    
    float NdotL = max(dot(normal, lightDir), 0.0);
    Lo = (kD * baseColor.xyz / PI + specualr) * lightColor.xyz * NdotL;
    
    float3 ambientValue = 0.3;
    float3 ambient = ambientValue * baseColor.xyz;
    float3 color = ambient + Lo;
    
    //float3 gamma = 0.4545;
    //color = color / color + 1.0;
    //color = pow(color, gamma);
    
    return float4(color, 1.0);
}

float4 PBR_CookTorrance2(float4 baseColor, float3 normal,
    float3 lightColor, float3 lightDir, float3 viewDir,
    half smoothness, half metallic, float3 irradiance)
{
    smoothness = max(smoothness, 0.1);
    
    float3 H = normalize(viewDir + lightDir);
    float NoH = max(dot(normal, H), 0.0);
    float NoV = max(dot(normal, viewDir), 0.0);
    //float NoL = max(dot(normal, lightDir), 0.00001);
    float NoL = (dot(normal, lightDir) * 0.5) + 0.5;
    float HoV = max(dot(H, viewDir), 0.0);
    float LoH = max(dot(lightDir, H), 0.0);
    
    float R = reflect(-viewDir, normal);
    
    float perceptualRoughness = SmoothnessToPerceptualRoughness(smoothness);
    float roughness = PerceptualRoughnessToRoughness(perceptualRoughness);
    
    float3 F0 = lerp(float3(0.04, 0.04, 0.04), baseColor.xyz, metallic);
    float3 ks = F0;
    float3 kd = (1 - ks) * (1 - metallic);
    
    // NDF 
    float D = D_GGX(roughness, NoH, normal, H);
    
    // Calculate fresnel
    float3 F = FfresnelSchlick(saturate(dot(H, viewDir)), F0);
    
    // Geometry term
    float G = GeometrySmith(normal, viewDir, lightDir, smoothness);
    
    // Calculate the Cook-Torrance denominator
    float3 numerator = D * G * F;
    float denominator = 4.0 * max(NoV, 0.0) * max(NoL, 0.0) + 0.0001;
    float3 specualr = numerator / denominator;
    
    //reflectionColor *= metallic;
    //kd *= irradiance;
    
    float3 Lo = (kd * baseColor.rgb / PI + specualr) * lightColor * NoL;
    
    // 조명을 합산하지 않고 조명을 개별로 처리하는 경우
    // 오픈지엘을 보면 모든 조명을 연산한 뒤 나중에 한번에 처리
    // 후자로 가야함
    // 따라서 아래부터는 Combine에 있어야함
    
    float3 Fresnel = FresnelSchlickRoughness(max(dot(normal, viewDir), 0.0), F0, smoothness);
    
    float3 kS = Fresnel;
    float3 kD = 1.0 - kS;
    kD *= 1.0 - metallic;
   
    float3 diffuse = irradiance * baseColor.rgb;
    float3 ambient = (kd * diffuse);
    float3 color = ambient + Lo;
    
    color = pow(color, GammaValue);
    
    return float4(color, 1.0);
    
}


float4 Model3(float4 baseColor, float3 normal,
    float3 lightColor, float3 lightDir, float3 viewDir,
    float3 f0, half smoothness, half metallic, float3 reflactionColor)
{
    float3 H = normalize(viewDir + lightDir);
    float NoH = max(dot(normal, H), 0.000001);
    float NoV = max(dot(normal, viewDir), 0.00001);
    float NoL = max(dot(normal, lightDir), 0.000001);
    float HoV = max(dot(H, viewDir), 0.0);
    
    float perceptualRoughness = SmoothnessToPerceptualRoughness(smoothness);
    float roughness = PerceptualRoughnessToRoughness(perceptualRoughness);
    
    
    float3 F0 = float(0.04);
    
    // Cook-Torrance BRDF
    F0 = lerp(F0, float3(pow(baseColor.xyz, float3(2.2, 2.2, 2.2))), metallic);
    float NDF = distributionGGX(normal, H, roughness);
    float G = geometrySmith(normal, viewDir, lightDir, smoothness);
    float3 F = FfresnelSchlick(max(dot(H, viewDir), 0.0), F0);
    float3 kD = float3(1.0, 1.0, 1.0) - F;
    kD *= 1.0 - metallic;
    
    
    float3 numerator = NDF * G * F;
    float denominator = 4.0 * max(NoV, 0.0) * max(NoL, 0.0) + 0.00001;
    float3 specualr = numerator / max(denominator, 0.001);
    
    float3 color = lightColor * (kD * pow(baseColor.rgb, float3(2.2, 2.2, 2.2)) / PI + specualr) * (NoL / dot(lightDir, lightDir)) * reflactionColor;
    
    float3 totColor = 0.0;
    totColor += color;
    
    
    return float4(pow(totColor / (totColor + 1.0), float3(1.0 / 2.2, 1.0 / 2.2, 1.0 / 2.2)), 1.0);
}

float4 UnityModel(float4 baseColor, float3 normal,
    float3 lightColor, float3 lightDir, float3 viewDir,
    float3 f0, half smoothness, half metallic)
{
    
    half oneMinusReflectivity;
    half3 specColor;
    baseColor.rgb = DiffuseAndSpecularFromMetallic(baseColor.rgb, metallic, specColor, oneMinusReflectivity);
    
    float perceptualRoughness = SmoothnessToPerceptualRoughness(smoothness);
    float3 halfDir = Unity_SafeNormalize(float3(lightDir) + viewDir);
    
    half nv = abs(dot(normal, viewDir));
    
    float nl = saturate(dot(normal, lightDir));
    float nh = saturate(dot(normal, halfDir));

    half lv = saturate(dot(lightDir, viewDir));
    half lh = saturate(dot(lightDir, halfDir));

    // Diffuse term
    half diffuseTerm = DisneyDiffuse(nv, nl, lh, perceptualRoughness) * nl;

    // Specular term
    float roughness = PerceptualRoughnessToRoughness(perceptualRoughness);
    float V = SmithJointGGXVisibilityTerm(nl, nv, roughness);
    float D = GGXTerm(nh, roughness);

    float specularTerm = V * D * PI; // Torrance-Sparrow model, Fresnel is applied later
    specularTerm = max(0, specularTerm * nl);
    
    half surfaceReduction;
    surfaceReduction = 1.0 / (roughness * roughness + 1.0); // fade \in [0.5;1]

    specularTerm *= any(specColor) ? 1.0 : 0.0;

    half grazingTerm = saturate(smoothness + (1 - oneMinusReflectivity));
    half3 color = baseColor.rgb * (lightColor * diffuseTerm)
                    + specularTerm * lightColor * FresnelTerm(specColor, lh)
                    + surfaceReduction * FresnelLerp(specColor, grazingTerm, nv);

    return half4(color, 1);
}
