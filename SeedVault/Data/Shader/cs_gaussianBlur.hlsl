//=============================================================================
// Blur.fx by Frank Luna (C) 2011 All Rights Reserved.
//
// Performs a separable blur with a blur radius of 5.  
//=============================================================================

static float gWeights[11] =
{
    0.05f, 0.05f, 0.1f, 0.1f, 0.1f, 0.2f, 0.1f, 0.1f, 0.1f, 0.05f, 0.05f,
};

static const uint gBlurRadius = 5;


Texture2D gInput : register(t0);
RWTexture2D<float4> gOutput : register(u0);

#define N 256
#define CacheSize (N + 2 * 5)
groupshared float4 gCache[CacheSize];

[numthreads(N, 1, 1)]
void HorzBlurCS(int3 groupThreadID : SV_GroupThreadID,
				int3 dispatchThreadID : SV_DispatchThreadID)
{
	//
	// Fill local thread storage to reduce bandwidth.  To blur 
	// N pixels, we will need to load N + 2*BlurRadius pixels
	// due to the blur radius.
	//
	
    int2 currPixel = 0;
    currPixel.y = dispatchThreadID.y;
    
    uint currCacheIndex = 0;
    
    float4 _nowPixel = 0;
    if (groupThreadID.x < gBlurRadius)
    {
		// Clamp out of bound samples that occur at image borders.
        int X = dispatchThreadID.x - gBlurRadius;
        currPixel.x = max(X, 0);
        
        // ���� �տ� �ִ� �� ��´�
        // groupThreadID.x = 0~255
        currCacheIndex = groupThreadID.x;
        
        // �׷� ���� �޸𸮿� �����ؼ� ���� �� ���� �׻� �ʿ��� ��쿡�� �־��ش�.
        // �� �� �ٱ����� �Ʒ� ���� �����ϰ� �Ǹ� ���� ������ ����� �ȴ�
        _nowPixel = gInput[currPixel];
        gCache[currCacheIndex] = _nowPixel;

    }
    if (groupThreadID.x >= N - gBlurRadius)
    {
		// Clamp out of bound samples that occur at image borders.
        
        int X = (dispatchThreadID.x + gBlurRadius);
        currPixel.x = min(X, gInput.Length.x - 1);
        currCacheIndex = groupThreadID.x + 2 * gBlurRadius;
        
        _nowPixel = gInput[currPixel];
        gCache[currCacheIndex] = _nowPixel;
    }
    
    // 256�� �����ϴ� ���߿� ��� �ȼ��� Ŀ���ϱ� ���ؼ��� 
    // ���� if�� �ΰ��� �����ϰ� ���� ���ִ°� �־�� �Ѵ�
    {
        currPixel.x = dispatchThreadID.x;
        currCacheIndex = groupThreadID.x + gBlurRadius;
    }
    
    _nowPixel = gInput[currPixel];
    gCache[currCacheIndex] = _nowPixel;
	
    GroupMemoryBarrierWithGroupSync();
    
    float4 blurColor = 0;
    
    [unroll]
    for (int i = -5; i <= 5; ++i)
    {
        int k = groupThreadID.x + 5 + i;
		
        blurColor += gWeights[i + 5] * gCache[k];
    }
    
    
    gOutput[dispatchThreadID.xy] = blurColor;
    
}

[numthreads(1, N, 1)]
void VertBlurCS(int3 groupThreadID : SV_GroupThreadID,
				int3 dispatchThreadID : SV_DispatchThreadID)
{
	
    int2 currPixel = 0;
    currPixel.x = dispatchThreadID.x;
    
    uint currCacheIndex = 0;
    
    float4 _nowPixel = 0;
    if (groupThreadID.y < gBlurRadius)
    {
        int Y = dispatchThreadID.y - gBlurRadius;
        currPixel.y = max(Y, 0);
        currCacheIndex = groupThreadID.y;
        
        _nowPixel = gInput[currPixel];
        gCache[currCacheIndex] = _nowPixel;
    }
    if (groupThreadID.y >= N - gBlurRadius)
    {
        int Y = dispatchThreadID.y + gBlurRadius;
        currPixel.y = min(Y, gInput.Length.y - 1);
        currCacheIndex = groupThreadID.y + 2 * gBlurRadius;
        
        _nowPixel = gInput[currPixel];
        gCache[currCacheIndex] = _nowPixel;
    }
    
    {
        currPixel.y = dispatchThreadID.y;
        currCacheIndex = groupThreadID.y + gBlurRadius;
    }
	
    _nowPixel = gInput[currPixel];
    gCache[currCacheIndex] = _nowPixel;
    
    GroupMemoryBarrierWithGroupSync();
    
    float4 blurColor = 0;
    [unroll]
    for (int i = -5; i <= 5; ++i)
    {
        int k = groupThreadID.y + 5 + i;
		
        blurColor += gWeights[i + 5] * gCache[k];
    }
    
    gOutput[dispatchThreadID.xy] = blurColor;
	

}