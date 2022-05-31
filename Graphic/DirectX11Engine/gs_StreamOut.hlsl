#include "ParticleLib.hlsli"

cbuffer cbPerParticle : register(b0)
{
    float3 gEmitPosW;
    float gGameTime;
    float3 gEmitDirW;
    float gTimeStep;
    float randomWeight;
    float size;
    float birthTime;
    float deadTime;
    
    float stopTime;
    uint particleCount;
    float pad;
    float pad2;
};
  
// Random texture used to generate random numbers in shaders.
Texture1D gRandomTex : register(t0);
SamplerState samLinear : register(s0);

//***********************************************
// HELPER FUNCTIONS                             *
//***********************************************
float3 RandUnitVec3(float offset)
{
	// Use game time plus offset to sample random texture.
    float u = (gGameTime + offset);
	
	// coordinates in [-1,1]
    float3 v = gRandomTex.SampleLevel(samLinear, u, 0).xyz;
	
    // 원하는 방향과 랜덤과의 가중치로
    float3 nowDirection = lerp(normalize(gEmitDirW), v, randomWeight);
    
	// project onto unit sphere
    return normalize(nowDirection);
}

Particle CreateParticlePoint(float offset)
{
    float3 vRandom = RandUnitVec3(offset);
    vRandom.x *= 0.5f;
    vRandom.z *= 0.5f;
			
    Particle p;
    p.InitialPosW = gEmitPosW.xyz;
    p.InitialVelW = length(gEmitDirW) * vRandom;
            
    p.SizeW = float2(size, size);
            
    p.Age = 0.0f;
    p.Type = PT_FLARE;
			
    return p;
}

 // 버텍스의 최대 생성 개수
[maxvertexcount(32)]
void main(point Particle gin[1], inout PointStream<Particle> ptStream)
{
    gin[0].Age.x += gTimeStep;
    gin[0].Age.y += gTimeStep;
	
    // 방출기 인지 아닌지를 구분한다
    if (gin[0].Type == PT_EMITTER)
    {
        // birthTime에 의해 파티클 생성 시기가 결정
        // stopTime에 의해 생성이 멈춰지는 시기가 결정
        if (gin[0].Age.y > birthTime && gin[0].Age.y < stopTime) 
        {
            for (uint i = 0; i < particleCount; i++)
            {
                // 파티클 초기 Vector 설정에 필요한 값을 넘긴다
                // 생성된 Particle 구조체는 PointStream에 Append 된다
                ptStream.Append(CreateParticlePoint(i * 0.001f));
            }
            
            gin[0].Age.x = 0.0f;
        }
        
        // 방출기가 마지막에 Append 된다
        // 기하 도형 셰이더를 디버깅 해보면 가장 아래에 위치한 것을 볼 수 있다
        ptStream.Append(gin[0]);
    }
    else
    {
        // deadTime에 의해 입자가 사라진다
        if (gin[0].Age.x <= deadTime)
        {
            ptStream.Append(gin[0]);
        }
    }
}

