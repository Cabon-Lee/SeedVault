Texture2D projectionTexture : register(t0);

SamplerState LinearSampler : register(s0);

struct VS_OUTPUT
{
    float4 outPosition : SV_POSITION; // SV로 묶여있어 직접 접근이 안된다.
    float4 depthPosition : TEXTURE0; // 위의 이유 때문에 추가로 같은 복사해서 타입을 넘겨준다
    float3 outNormal : NORMAL0;
    float2 outTex : TEXCOORD1;
    float4 viewPosition : TEXCOORD2;
    float3 lightPos : TEXCOORD3;
};

float4 main(VS_OUTPUT pin) : SV_Target
{   
    float2 projectTexCoord;
    
    projectTexCoord.x = +pin.viewPosition.x / pin.viewPosition.w / 2.0f + 0.5f;
    projectTexCoord.y = -pin.viewPosition.y / pin.viewPosition.w / 2.0f + 0.5f;
    
    float4 color = { 0, 0, 0, 0};
    if ((saturate(projectTexCoord.x) == projectTexCoord.x) && 
        (saturate(projectTexCoord.y) == projectTexCoord.y))
    {
        float4 projectionColor = projectionTexture.Sample(LinearSampler, projectTexCoord);
        color = projectionColor;
    }
    
    return color;
}