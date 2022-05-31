
struct VS_OUTPUT
{
    float4 outPosition : SV_POSITION;   // �����Ͷ������� ����ϰ� �Ǵ� �ø�ƽ, �츮�� ���� ����
    float4 depthPosition : TEXTURE0;
};


float4 main(VS_OUTPUT pin) : SV_TARGET
{
	// 	Z �ȼ� ���̸� ���� W ��ǥ�� ������ �ȼ��� ���� ���� �����ɴϴ�.
    float depthValue = pin.depthPosition.z / pin.depthPosition.w;
    return float4(depthValue, depthValue, depthValue, 1.0f);
}