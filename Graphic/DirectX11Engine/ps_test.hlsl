// �ٱ����� ��Ʈ������ �����´�

#include "light.hlsl"

cbuffer Light : register(b0)
{
    Light lightInfo[24];
}

cbuffer LightCount : register(b1)
{
    uint4 lightType[6];
    uint lightCount;
}

cbuffer cbMaterial : register(b2)
{
    float4 DiffuseAlbedo;
    float3 FresnelR0;
    float Roughness;
    float3 AmbientDown;
    float metallic;
    float3 AmbientRange;
};

cbuffer objectPer : register(b3)
{
    float4 PerspectiveValues;
    float4x4 ViewInv;
    float3 EyePos;
}


Texture2D objTexture : TEXTURE : register(t0);
SamplerState objSamplerState : SAMPLER : register(s0);

struct VS_OUTPUT
{
    float4 outPosition : SV_POSITION;
    float3 outWorldPos : POSITION;
    float3 outNormal : NORMAL;
    float2 outTexCoord : TEXCOORD;
    float3 outTangent : TANGENT;
};


float3 CalcLight(uint type, Light L, Material mat, float3 position, float3 normal, float3 eyePos)
{
    switch (type)
    {
        case 0: // ���Ɽ
            return ComputeDirectionalLight(L, mat, normal.xyz, eyePos);
        case 1: // ����Ʈ ����Ʈ
            return ComputePointLight(L, mat, position.xyz, normal.xyz, eyePos);
        case 2: // ���� ����Ʈ
            return ComputeSpotLight(L, mat, position.xyz, normal.xyz, eyePos);
    }
    
    return float3(0.0f, 0.0f, 0.0f);

}

float4 main(VS_OUTPUT pin) : SV_Target
{
    
    // Interpolating normal can unnormalize it, so renormalize it.
    float4 diffuseAlbedo = objTexture.Sample(objSamplerState, pin.outTexCoord) * DiffuseAlbedo;
    float4 ambient = float4(CalcAmbient(pin.outNormal.xyz, AmbientDown, AmbientRange), 0.0f);
    float3 calcAmbient = float3(ambient.xyz * diffuseAlbedo.xyz);
    
    // Vector from point being lit to eye. 
    float3 toEyeW = normalize(EyePos - pin.outWorldPos);
    
    const float shininess = 1.0f - Roughness;
    Material _mat = { diffuseAlbedo, FresnelR0, shininess };
    
    float3 result = { 0.0f, 0.0f, 0.0f };
    uint _round = 0;
    
    result = ComputeDirectionalLight(lightInfo[0], _mat, pin.outNormal, toEyeW);
    
    
    
    [unroll]
    for (uint i = 0; i < lightCount; ++i)
    {
        uint nowType = lightInfo[_round].shadowInfo.x;
        
        result += CalcLight(nowType, lightInfo[_round], _mat, pin.outPosition.xyz, pin.outNormal.xyz, toEyeW.xyz);
        _round++;
        if (_round == lightCount)
            break;
    }
    
    return float4(calcAmbient.xyz, 1.0f) + float4(result, 1.0f);
    
    //return float4(result, 1.0f);
    //return float4(calcAmbient.xyz, 1.0f); // + float4(result.xxx, 1.0f);

    /*
    // Interpolating normal can unnormalize it, so normalize it.
    pin.outNormal = normalize(pin.outNormal);
    
	// The toEye vector is used in lighting.
    float3 toEyeW = normalize(EyePos - pin.outWorldPos);

	// Cache the distance to the eye from this surface point.
    float distToEye = length(toEyeW);

	// Normalize.
    toEyeW /= distToEye;
	
    // Default to multiplicative identity.
    float4 texColor = float4(1, 1, 1, 1);

	// Sample texture.
    texColor = objTexture.Sample(objSamplerState, pin.outTexCoord);
   
	 
    float4 litColor = texColor;

		// Start with a sum of zero. 
    float4 ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 spec = float4(0.0f, 0.0f, 0.0f, 0.0f);

		// Sum the light contribution from each light source.  
    float4 A, D, S;

    // The light vector aims opposite the direction the light rays travel.
    float3 lightVec = -lightInfo[0].Direction;

    // light�� ���Ͱ��� normalize�� ���ؼ� diffuseFactor�� 1�� �Ѵ´�
    lightVec = normalize(lightVec);
    
	// Add ambient term.
	// Add diffuse and specular term, provided the surface is in 
	// the line of site of the light.
	
    float diffuseFactor = dot(lightVec, pin.outNormal);

    float specFactor = 0.0;
	// Flatten to avoid dynamic branching.
	[flatten]
    if (diffuseFactor > 0.0f)
    {
		// ����Ʈ�� �븻�� �ݻ纤��, ����ŧ���� max(theta)[����]�� ���ϱ� ���� ���Ѵ�.
		// �ݻ纤�ʹ� �ٽ� ���� ���Ϳ� dot�� scalar���� ���Ѵ�. �̸� ���� max(theta)�� �˰� �ȴ�.
		// Specular.w�� �ݿ������� ���õ��� ���������� �������ִ� ��ġ��
		// ��¦�̴� ǥ���ϼ��� �ش� ��ġ�� ����.(������ ũ�Ⱑ �۾�����)
        float3 v = reflect(-lightVec, pin.outNormal);
        specFactor = pow(max(dot(v, toEyeW), 0.0f), 1);
					
    }
    
    litColor.xyz = texColor.xyz * (lightInfo[0].Strength * diffuseFactor);
    litColor.xyz += (lightInfo[0].Strength * specFactor);
    litColor.w = 1.0f;

	// Common to take alpha from diffuse material and texture.
    //litColor.a = gMaterial.Diffuse.a * texColor.a;

    return litColor;
    */
}