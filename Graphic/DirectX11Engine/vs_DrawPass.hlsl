#include "ParticleLib.hlsli"

cbuffer differenceByTime : register(b0)
{
    float gForce;       // 중력을 수정한다. 음수의 경우에는 위로 올라간다(연기나 불)
    float gZRot;
    
    float gAlpha;       // 0 < gAlpha 값
    float gVelocity;    // 점점 빨리지거나 점점 느려진다
    
    float gSize;        // 점점 커지거나 줄어든다. 1이 기본, 0~1 사이값이면 줄어듬
    float gDeltaRot;
    
    float gRandomSize;         // 사이즈 난수 값 가중치
    float gRandomRot;          // 로테이션의 난수 값 가중치
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
   
    // 1 이상이면 시간이 지날수록 더 빨리 사라진다
    float _deltaAlpha = 1 - gAlpha;  
    float _deltaVelocity = (1.0 + _t) * gVelocity;
    
    // 기본
    float _factor = 1.0 + gSize;
    
    // 시간이 지날수록 가중을 주고 싶다
    float _step = (1.0 + (_t * 5));
    float _pow = pow(_step, gSize);
    
    _factor += _pow;
    _factor = RandFloat(_t, 0.0, _factor, gRandomSize, 10);
    
    // zRot가 360 기준이므로 마찬가지로 해준다
    float _deltaZRotation = ((1.0 + _t) * 10) * (gDeltaRot * 360); 
    _deltaZRotation = RandFloat(_t, 0.0, _deltaZRotation, gRandomRot, 100);
    
	// 중력의 방향 및 세기를 결정하고 초기 속도와 위치를 통해 현재 위치를 구한다
    vout.PosW = 0.5f * _t * _t * (float3(0.0f, -9.8f, 0.0f) * gForce) +
                _t * (vin.InitialVelW * _deltaVelocity) + vin.InitialPosW;
    
    vout.Dir = normalize(vin.InitialVelW);
    
	// 컬러 값은 시간이 지날수록 점점 낮아진다
    float _opacity = 1.0f - smoothstep(0.0f, 1.0f, _t / 1.0f);
    vout.Color = float4(1.0f, 1.0f, 1.0f, _opacity * _deltaAlpha);
	
    vout.SizeW = vin.SizeW * _factor;
    vout.Type = vin.Type;
    
    // _deltaZRotation가 0일수도 있으므로 +로 간다
    vout.ZRot = gZRot + _deltaZRotation;
    
    return vout;
}

