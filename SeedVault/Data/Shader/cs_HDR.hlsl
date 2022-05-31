cbuffer DownScaleConstatnts : register(b0)
{
    // DownScale 해상도 계산 : x(width), y(height)
    uint2 Res : packoffset(c0);         // 백 버퍼의 높이와 너비를 4로 나눈값
    // DownScale 이미지의 총 픽셀 수    
    uint Domain : packoffset(c0.z);     // 백 버퍼의 높이와 너비를 고한 후 16으로 나눈값
    // 첫 패스에 적용된 그룹 수 계산
    uint GroupSize : packoffset(c0.w);  // 백 버퍼의 높이와 너비를 곱한 후 16으로 나눈다음 1024를 나눈값
    // 1024로 나눈 이유는 최대 사용가능한 numthreads의 수가 1024라서 그런 것 같다.
}

Texture2D HDRTex : register(t0);
StructuredBuffer<float> AverageValues1D : register(t0);

RWStructuredBuffer<float> AverageLuminance : register(u0); 

groupshared float SharedPositions[1024]; // 스레드간 공유 메모리
static const float4 LUM_FACTOR = float4(0.299, 0.587, 0.114, 0);

float DownScale4x4(uint2 CurPixel, uint groupThreadID)
{
    float avgLum = 0.0f;
    
    // 픽셀 결합을 생략
    if (CurPixel.y < Res.y)
    {
        // 4x4 픽셀 그룹 값 합산
        int3 fullResPos = int3(CurPixel * 4, 0);
        float4 downScaled = float4(0.0, 0.0, 0.0, 0.0);
        
        [unroll]
        for(int i = 0; i < 4; i++)
        {
            for (int j = 0; j < 4; j++)
            {
                downScaled += HDRTex.Load(fullResPos, int2(j, i));
            }
        }
        
        downScaled /= 16.0;
        // 픽셀 별 휘도값 계산
        avgLum = dot(downScaled, LUM_FACTOR);
        
        // 공유 메모리에 결과 기록
        SharedPositions[groupThreadID] = avgLum;
    }

    // 동기화 후 다음 단계로
    GroupMemoryBarrierWithGroupSync();
    
    return avgLum;
}

float DownScale1024to4(uint dispatchThreadID, uint groupThreadID, float avgLum)
{
    [unroll]
    for (uint groupSize = 4, step1 = 1, step2 = 2, step3 = 3;
        groupSize < 1024;
        groupSize *= 4, step1 *= 4, step2 *= 4, step3 *= 4)
    {
        //픽셀 결합 계산 생략
        if (groupThreadID % groupSize == 0)
        {
			// Calculate the luminance sum for this step
            float stepAvgLum = avgLum;
            stepAvgLum += dispatchThreadID + step1 < Domain ? SharedPositions[groupThreadID + step1] : avgLum;
            stepAvgLum += dispatchThreadID + step2 < Domain ? SharedPositions[groupThreadID + step2] : avgLum;
            stepAvgLum += dispatchThreadID + step3 < Domain ? SharedPositions[groupThreadID + step3] : avgLum;
		
			// Store the results
            avgLum = stepAvgLum;
            SharedPositions[groupThreadID] = stepAvgLum;
        }

		// Synchronize before next step
        GroupMemoryBarrierWithGroupSync();
    }

	return avgLum;
}


void DownScale4to1(uint dispatchThreadId, uint groupThreadId, uint groupId, float avgLum)
{
    if (groupThreadId == 0)
    {
		// Calculate the average lumenance for this thread group
        float fFinalAvgLum = avgLum;
        fFinalAvgLum += dispatchThreadId + 256 < Domain ? SharedPositions[groupThreadId + 256] : avgLum;
        fFinalAvgLum += dispatchThreadId + 512 < Domain ? SharedPositions[groupThreadId + 512] : avgLum;
        fFinalAvgLum += dispatchThreadId + 768 < Domain ? SharedPositions[groupThreadId + 768] : avgLum;
        fFinalAvgLum /= 1024.0;

		// Write the final value into the 1D UAV which will be used on the next step
        // 현재 Dispatch 시 1차원 벡터의 형태로 Dispatch한다. 
        // GroupID도 1차원 (여기서는 127개의 배열)로 접근할 것
        AverageLuminance[groupId] = fFinalAvgLum;
    }
}

// 현재 numthreads는 최대 1024의 크기를 가진다
[numthreads(1024, 1, 1)]
void DownScaleFirstPass(
uint3 dispatchThreadId : SV_DispatchThreadID, 
uint3 groupThreadId : SV_GroupThreadID, 
uint3 groupId : SV_GroupID)
{
    uint2 CurPixel = uint2(dispatchThreadId.x % Res.x, dispatchThreadId.x / Res.x);

	// Reduce a group of 16 pixels to a single pixel and store in the shared memory
    float avgLum = DownScale4x4(CurPixel, groupThreadId.x);

	// Down scale from 1024 to 4
    avgLum = DownScale1024to4(dispatchThreadId.x, groupThreadId.x, avgLum);

	// Downscale from 4 to 1
    DownScale4to1(dispatchThreadId.x, groupThreadId.x, groupId.x, avgLum);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Second pass - convert the 1D average values into a single value
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define MAX_GROUPS 64

// Group shared memory to store the intermidiate results
groupshared float SharedAvgFinal[MAX_GROUPS];

[numthreads(MAX_GROUPS, 1, 1)]
void DownScaleSecondPass(
    uint3 groupId : SV_GroupID, 
    uint3 groupThreadId : SV_GroupThreadID,
    uint3 dispatchThreadId : SV_DispatchThreadID, 
    uint groupIndex : SV_GroupIndex)
{
	// Fill the shared memory with the 1D values
    
    // dispatchThreadID 구하는 방법 https://docs.microsoft.com/ko-kr/windows/win32/direct3dhlsl/sv-dispatchthreadid
    // 내가 계산했을 때는 0~128 사이가 dispatchThreadID의 범위다
    // 그러므로 현재 Thread가 총 128개가 돈다는 의미로 생각할 수 있을 것 같다
    // GroupSize는 백 버퍼의 높이와 너비를 곱한 후 16으로 나눈다음 1024를 나눈값, 127이다
    // 현재 dispatchThreadID가 GroupSize보다 크면 그 범위 바깥에서 값을 가져온다
    float avgLum = 0.0;
    if (dispatchThreadId.x < GroupSize)
    {
        avgLum = AverageValues1D[dispatchThreadId.x];
    }
    
    // 내 계산에 따르면 dispatchThreadId가 MAX_GROUPS인 64를 넘어가기 때문에 성립되지 않는 공식이다
    // 그렇다면 아래 연산은 하지 않는것인가?
    SharedAvgFinal[dispatchThreadId.x] = avgLum;

    GroupMemoryBarrierWithGroupSync(); // Sync before next step

	// Downscale from 64 to 16
    if (dispatchThreadId.x % 4 == 0)
    {
		// Calculate the luminance sum for this step
        float stepAvgLum = avgLum;
        stepAvgLum += dispatchThreadId.x + 1 < GroupSize ? SharedAvgFinal[dispatchThreadId.x + 1] : avgLum;
        stepAvgLum += dispatchThreadId.x + 2 < GroupSize ? SharedAvgFinal[dispatchThreadId.x + 2] : avgLum;
        stepAvgLum += dispatchThreadId.x + 3 < GroupSize ? SharedAvgFinal[dispatchThreadId.x + 3] : avgLum;
		
		// Store the results
        avgLum = stepAvgLum;
        SharedAvgFinal[dispatchThreadId.x] = stepAvgLum;
    }

    GroupMemoryBarrierWithGroupSync(); // Sync before next step

	// Downscale from 16 to 4
    if (dispatchThreadId.x % 16 == 0)
    {
		// Calculate the luminance sum for this step
        float stepAvgLum = avgLum;
        stepAvgLum += dispatchThreadId.x + 4 < GroupSize ? SharedAvgFinal[dispatchThreadId.x + 4] : avgLum;
        stepAvgLum += dispatchThreadId.x + 8 < GroupSize ? SharedAvgFinal[dispatchThreadId.x + 8] : avgLum;
        stepAvgLum += dispatchThreadId.x + 12 < GroupSize ? SharedAvgFinal[dispatchThreadId.x + 12] : avgLum;

		// Store the results
        avgLum = stepAvgLum;
        SharedAvgFinal[dispatchThreadId.x] = stepAvgLum;
    }

    GroupMemoryBarrierWithGroupSync(); // Sync before next step

	// Downscale from 4 to 1
    if (dispatchThreadId.x == 0)
    {
		// Calculate the average luminace
        float fFinalLumValue = avgLum;
        fFinalLumValue += dispatchThreadId.x + 16 < GroupSize ? SharedAvgFinal[dispatchThreadId.x + 16] : avgLum;
        fFinalLumValue += dispatchThreadId.x + 32 < GroupSize ? SharedAvgFinal[dispatchThreadId.x + 32] : avgLum;
        fFinalLumValue += dispatchThreadId.x + 48 < GroupSize ? SharedAvgFinal[dispatchThreadId.x + 48] : avgLum;
        fFinalLumValue /= 64.0;
        
		// Store the final value
        AverageLuminance[0] = max(fFinalLumValue, 0.0001);
    }
}
