
// 해당 레지스터의 슬롯 번호와
// PSSetConstantBuffers()의 첫번째 인자와 일치해야 한다
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

    // light의 벡터값도 normalize를 안해서 diffuseFactor가 1을 넘는다
    lightVec = normalize(lightVec);
    
	// Add ambient term.
	// Add diffuse and specular term, provided the surface is in 
	// the line of site of the light.
	
    float diffuseFactor = dot(lightVec, input.inNormal);

	// Flatten to avoid dynamic branching.
	[flatten]
    if (diffuseFactor > 0.0f)
    {
		// 라이트와 노말의 반사벡터, 스페큘러의 max(theta)[각도]를 정하기 위해 구한다.
		// 반사벡터는 다시 눈의 벡터와 dot로 scalar값을 구한다. 이를 통해 max(theta)를 알게 된다.
		// Specular.w는 반영지수로 광택도를 간접적으로 조명해주는 수치다
		// 반짝이는 표면일수록 해당 수치가 높다.(원뿔의 크기가 작아진다)
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








