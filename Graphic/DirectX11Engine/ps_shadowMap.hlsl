
struct VS_OUTPUT
{
    float4 outPosition : SV_POSITION;   // 래스터라이저가 사용하게 되는 시맨틱, 우리는 접근 못함
    float4 depthPosition : TEXTURE0;
};


float4 main(VS_OUTPUT pin) : SV_TARGET
{
	// 	Z 픽셀 깊이를 균질 W 좌표로 나누어 픽셀의 깊이 값을 가져옵니다.
    float depthValue = pin.depthPosition.z / pin.depthPosition.w;
    return float4(depthValue, depthValue, depthValue, 1.0f);
}