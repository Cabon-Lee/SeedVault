#include "ParticleLib.hlsli"

cbuffer differenceByTime : register(b0)
{
    float gForce;       // �߷��� �����Ѵ�. ������ ��쿡�� ���� �ö󰣴�(���⳪ ��)
    float gZRot;
    
    float gAlpha;       // 0 < gAlpha ��
    float gVelocity;    // ���� �������ų� ���� ��������
    
    float gSize;        // ���� Ŀ���ų� �پ���. 1�� �⺻, 0~1 ���̰��̸� �پ��
    float gDeltaRot;
    
    float gRandomSize;         // ������ ���� �� ����ġ
    float gRandomRot;          // �����̼��� ���� �� ����ġ
};

// Random texture used to generate random numbers in shaders.
Texture1D gRandomTex : register(t0);
SamplerState samLinear : register(s0);

float RandFloat(float age, float offset, float targetVal, float weight, float intense)
{
    float u = (age + offset);
	
    float3 v = gRandomTex.SampleLevel(samLinear, u, 0).xyz;
	
    float factor = ((v.x + v.y + v.z) / 3) * intense;
    
    float nowVal = lerp(targetVal, factor, weight);
    
    return nowVal;
}

DrawPassVertexOut main(Particle vin)
{
    DrawPassVertexOut vout;
	
    float _t = vin.Age;
   
    // 1 �̻��̸� �ð��� �������� �� ���� �������
    float _deltaAlpha = 1 - gAlpha;  
    float _deltaVelocity = (1.0 + _t) * gVelocity;
    
    // �⺻
    float _factor = 1.0 + gSize;
    
    // �ð��� �������� ������ �ְ� �ʹ�
    float _step = (1.0 + (_t * 5));
    float _pow = pow(_step, gSize);
    
    _factor += _pow;
    _factor = RandFloat(_t, 0.0, _factor, gRandomSize, 10);
    
    // zRot�� 360 �����̹Ƿ� ���������� ���ش�
    float _deltaZRotation = ((1.0 + _t) * 10) * (gDeltaRot * 360); 
    _deltaZRotation = RandFloat(_t, 0.0, _deltaZRotation, gRandomRot, 100);
    
	// �߷��� ���� �� ���⸦ �����ϰ� �ʱ� �ӵ��� ��ġ�� ���� ���� ��ġ�� ���Ѵ�
    vout.PosW = 0.5f * _t * _t * (float3(0.0f, -9.8f, 0.0f) * gForce) +
                _t * (vin.InitialVelW * _deltaVelocity) + vin.InitialPosW;
    
    vout.Dir = normalize(vin.InitialVelW);
    
	// �÷� ���� �ð��� �������� ���� ��������
    float _opacity = 1.0f - smoothstep(0.0f, 1.0f, _t / 1.0f);
    vout.Color = float4(1.0f, 1.0f, 1.0f, _opacity * _deltaAlpha);
	
    vout.SizeW = vin.SizeW * _factor;
    vout.Type = vin.Type;
    
    // _deltaZRotation�� 0�ϼ��� �����Ƿ� +�� ����
    vout.ZRot = gZRot + _deltaZRotation;
    
    return vout;
}

