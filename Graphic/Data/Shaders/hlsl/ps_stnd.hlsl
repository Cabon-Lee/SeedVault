
// �ش� ���������� ���� ��ȣ��
// PSSetConstantBuffers()�� ù��° ���ڿ� ��ġ�ؾ� �Ѵ�
cbuffer alphaBuffer : register(b0)
{
    float3 lightDir;
    float3 ambientLightColor;
    float ambientLightStrength;
}

cbuffer objectPer : register(b1)
{
    float3 eyePos;
}


struct PS_INPUT
{
    float4 inPosition : SV_POSITION;
    float3 inWorldPos : POSITION;
    float3 inNormal : NORMAL;
    float2 inTexCoord : TEXCOORD;
};
 
Texture2D objTexture : TEXTURE : register(t0);
SamplerState objSamplerState : SAMPLER : register(s0);

float4 main(PS_INPUT input) : SV_TARGET
{
    
    // Interpolating normal can unnormalize it, so normalize it.
    input.inNormal = normalize(input.inNormal);
    
	// The toEye vector is used in lighting.
    float3 toEye = eyePos - input.inWorldPos;

	// Cache the distance to the eye from this surface point.
    float distToEye = length(toEye);

	// Normalize.
    toEye /= distToEye;
	
    // Default to multiplicative identity.
    float4 texColor = float4(1, 1, 1, 1);

		// Sample texture.
    texColor = objTexture.Sample(objSamplerState, input.inTexCoord);
   
	 
    float4 litColor = texColor;

		// Start with a sum of zero. 
    float4 ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 spec = float4(0.0f, 0.0f, 0.0f, 0.0f);

		// Sum the light contribution from each light source.  
    float4 A, D, S;

    // The light vector aims opposite the direction the light rays travel.
    float3 lightVec = -lightDir;

    // light�� ���Ͱ��� normalize�� ���ؼ� diffuseFactor�� 1�� �Ѵ´�
    lightVec = normalize(lightVec);
    
	// Add ambient term.
	// Add diffuse and specular term, provided the surface is in 
	// the line of site of the light.
	
    float diffuseFactor = dot(lightVec, input.inNormal);

	// Flatten to avoid dynamic branching.
	[flatten]
    if (diffuseFactor > 0.0f)
    {
		// ����Ʈ�� �븻�� �ݻ纤��, ����ŧ���� max(theta)[����]�� ���ϱ� ���� ���Ѵ�.
		// �ݻ纤�ʹ� �ٽ� ���� ���Ϳ� dot�� scalar���� ���Ѵ�. �̸� ���� max(theta)�� �˰� �ȴ�.
		// Specular.w�� �ݿ������� ���õ��� ���������� �������ִ� ��ġ��
		// ��¦�̴� ǥ���ϼ��� �ش� ��ġ�� ����.(������ ũ�Ⱑ �۾�����)
        float3 v = reflect(-lightVec, input.inNormal);
        float specFactor = pow(max(dot(v, toEye), 0.0f), 1);
					
    }
    
    
    //ambient += A;
    //diffuse += D;
    //spec += S;
    

		// Modulate with late add.
    litColor = texColor + diffuseFactor;
    litColor.w = 1.0f;

	// Common to take alpha from diffuse material and texture.
    //litColor.a = gMaterial.Diffuse.a * texColor.a;

    return litColor;
    
    
    
    
    
    
    
    
    //float3 sampleColor = objTexture.Sample(objSamplerState, input.inTexCoord);
    ////float3 sampleColor = input.inNormal;
    //float3 ambientLight = ambientLightColor * ambientLightStrength;
    //float3 finalColor = sampleColor * ambientLight;
    //
    //return float4(finalColor, 1.0f);
}








