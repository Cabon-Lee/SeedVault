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
	
    // ���ϴ� ����� �������� ����ġ��
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

 // ���ؽ��� �ִ� ���� ����
[maxvertexcount(32)]
void main(point Particle gin[1], inout PointStream<Particle> ptStream)
{
    gin[0].Age.x += gTimeStep;
    gin[0].Age.y += gTimeStep;
	
    // ����� ���� �ƴ����� �����Ѵ�
    if (gin[0].Type == PT_EMITTER)
    {
        // birthTime�� ���� ��ƼŬ ���� �ñⰡ ����
        // stopTime�� ���� ������ �������� �ñⰡ ����
        if (gin[0].Age.y > birthTime && gin[0].Age.y < stopTime) 
        {
            for (uint i = 0; i < particleCount; i++)
            {
                // ��ƼŬ �ʱ� Vector ������ �ʿ��� ���� �ѱ��
                // ������ Particle ����ü�� PointStream�� Append �ȴ�
                ptStream.Append(CreateParticlePoint(i * 0.001f));
            }
            
            gin[0].Age.x = 0.0f;
        }
        
        // ����Ⱑ �������� Append �ȴ�
        // ���� ���� ���̴��� ����� �غ��� ���� �Ʒ��� ��ġ�� ���� �� �� �ִ�
        ptStream.Append(gin[0]);
    }
    else
    {
        // deadTime�� ���� ���ڰ� �������
        if (gin[0].Age.x <= deadTime)
        {
            ptStream.Append(gin[0]);
        }
    }
}

