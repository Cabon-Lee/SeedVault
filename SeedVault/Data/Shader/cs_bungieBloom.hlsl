
Texture2D gInput : register(t0);
Texture2D gInput2 : register(t1);

RWTexture2D<float4> gOutput : register(u0);


#define BLOOMCURVE_METHOD_1
#define BLOOMCURVE_METHOD_2
//#define BLOOMCURVE_METHOD_3

cbuffer gBloomCurve : register(b0)
{
    float gThreshold : packoffset(c0);
    float gIntensity : packoffset(c0.y);
    uint gHeight : packoffset(c0.z);
    float gAdjustFactor : packoffset(c0.w);
};


float GetBloomCurve(float x, float threshold)
{
    float result = 0;
    result = x;
    x *= 2.0;
    
#ifdef BLOOMCURVE_METHOD_1
    result = x * 0.05 + max(0, x - threshold) * 0.5;
#endif
    
#ifdef BLOOMCURVE_METHOD_3
    result = max(0, x - threshold);
    result *= result;
#endif
    
    return result * 0.5;
    
}

#define MAX_THREAD 32
// 최대 스레드 1024에 맞는
[numthreads(MAX_THREAD, MAX_THREAD, 1)]
void BloomCurve(
uint3 groupID : SV_GroupID,
uint3 groupThreadID : SV_GroupThreadID,
uint3 dispatchThreadID : SV_DispatchThreadID)
{
    uint2 currPixel;
    currPixel.x = (groupID.x * 32) + groupThreadID.x;
    currPixel.y = (groupID.y * 32) + groupThreadID.y;
    
    float3 _color = gInput[currPixel];
    float intensity = dot(_color, float3(0.3, 0.3, 0.3));
    
    intensity *= gIntensity;
    
    float bloom_intensity = GetBloomCurve(intensity, gThreshold);
    
    float3 _finColor = _color * bloom_intensity / intensity;
    gOutput[currPixel] = float4(_finColor, 1.0);
}

[numthreads(MAX_THREAD, MAX_THREAD, 1)]
// 단순 카피만 해주는 기능
void Copy(uint3 dispatchThreadID : SV_DispatchThreadID)
{
    uint2 currPixel = dispatchThreadID.xy;
    gOutput[currPixel] = gInput[currPixel];
}

[numthreads(MAX_THREAD, MAX_THREAD, 1)]
void Merge(uint3 dispatchThreadID : SV_DispatchThreadID)
{
    uint2 _currPixel = dispatchThreadID.xy;
    
    float4 _bloomColor = gInput[_currPixel];
    float4 _pixelColor = gInput2[_currPixel];
   
    float3 _nowColor = _bloomColor.rgb + _pixelColor.rgb;
    
    GroupMemoryBarrierWithGroupSync();
    
    _nowColor = max(_nowColor, float3(0, 0, 0));
    gOutput[_currPixel] = float4(_nowColor.rgb, 1.0);
}

groupshared float4 gCache4x4[16];

[numthreads(4, 4, 1)]
void DownSample4x4(
uint3 groupID : SV_GroupID,
uint3 groupThreadID : SV_GroupThreadID,
uint3 dispatchThreadID : SV_DispatchThreadID)
{
    // Group ID 전체가 하나의 픽셀이 된다
    float3 _nowColor = float3(0, 0, 0);
    uint2 _currPixel = 0;
    
    uint _CacheIndex = 0;
    
    _CacheIndex = (groupThreadID.y * 4) + groupThreadID.x;
    gCache4x4[_CacheIndex] = gInput[dispatchThreadID.xy];
    
    GroupMemoryBarrierWithGroupSync();
    
    [unroll]
    for (uint i = 0; i < 16; i++)
    {
        _nowColor += gCache4x4[i];
    }
    
    if (groupThreadID.x == 0 && groupThreadID.y == 0)
    {
        // 위 if문을 사용하지 않은 상태라면 컴파일 오류가 난다
        // groupID로 접근하는 경우 멀티 프로세스 상에서 다른 스레드가 접근해서 매번 값이 달라지기 때문이다
        _nowColor /= 16;
        _nowColor = max(_nowColor, float3(0, 0, 0));
        gOutput[groupID.xy] = float4(_nowColor.rgb, 1.0);
    }
}

groupshared float4 gCache6x6[36];

[numthreads(6, 6, 1)]
void DownSample6x6(
uint3 groupID : SV_GroupID,
uint3 groupThreadID : SV_GroupThreadID,
uint3 dispatchThreadID : SV_DispatchThreadID)
{
    // Group ID 전체가 하나의 픽셀이 된다
    float3 _nowColor = float3(0, 0, 0);
    uint2 _currPixel = 0;
    
    uint _CacheIndex = 0;
    
    _CacheIndex = (groupThreadID.y * 6) + groupThreadID.x;
    gCache6x6[_CacheIndex] = gInput[dispatchThreadID.xy];
    
    GroupMemoryBarrierWithGroupSync();
    
    [unroll]
    for (uint i = 0; i < 36; i++)
    {
        _nowColor += gCache6x6[i];
    }
    
    if (groupThreadID.x == 0 && groupThreadID.y == 0)
    {
        _nowColor /= 36;
        _nowColor = max(_nowColor, float3(0, 0, 0));
        gOutput[groupID.xy] = float4(_nowColor.rgb, 1.0);
    }
}

[numthreads(4, 4, 1)]
void UpSample4x4(
uint3 groupID : SV_GroupID,
uint3 groupThreadID : SV_GroupThreadID,
uint3 dispatchThreadID : SV_DispatchThreadID)
{
    float3 _nowColor = 0;
    uint2 _nowPos = 0;
    _nowColor = gInput[groupID.xy].rgb;
    
    _nowPos.x = (groupID.x * 4) + groupThreadID.x;
    _nowPos.y = (groupID.y * 4) + groupThreadID.y;
    
    GroupMemoryBarrierWithGroupSync();
    
    _nowColor = max(_nowColor, float3(0, 0, 0));
    gOutput[_nowPos] = float4(_nowColor, 1.0);
}

[numthreads(6, 6, 1)]
void UpSample6x6(
uint3 groupID : SV_GroupID,
uint3 groupThreadID : SV_GroupThreadID,
uint3 dispatchThreadID : SV_DispatchThreadID)
{
    float3 _nowColor = 0;
    uint2 _nowPos = 0;
    _nowColor = gInput[groupID.xy].rgb;
    
    _nowPos.x = (groupID.x * 6) + groupThreadID.x;
    _nowPos.y = (groupID.y * 6) + groupThreadID.y;
    
    GroupMemoryBarrierWithGroupSync();
   
    _nowColor = max(_nowColor, float3(0, 0, 0));
    gOutput[_nowPos] = float4(_nowColor, 1.0);
}


[numthreads(1, 1, 1)]
void UpSampleBilinearLerp(uint3 groupID : SV_GroupID)
{
    float4 _oriHoir[3];
    float4 _virtHori[3];
    float4 _oriVert[3];
    float4 _virtVert[3];
    
    // 16개의 픽셀 중 가장 첫번째 위치를 정해준다
    // 작은 사이즈(원본)을 하나씩 읽어오는 방식, 4배로 키울 것이기 때문에 
    float3 _originColor = 0;
    
    uint2 _readPos = groupID.xy;
    uint2 _writePos;
    
    // 0,0 위치를 채운다. 해당 위치가 OriginColor가 된다.
    {
        _writePos.x = (groupID.x * 4);
        _writePos.y = (groupID.y * 4);
    
        _originColor = gInput[_readPos.xy].rgb;
        gOutput[_writePos.xy] = float4(_originColor, 1.0);
    }
    

    float3 _horizontalColor = 0;    // 1,0
    float3 _verticalColor = 0;      // 0,1
    float3 _virtualColor = 0;       // 1,1
    
    // 1,0 | 0,1 | 1,1 에 값을 채워준다
    uint2 _nextPos;
    {
        _nextPos = uint2(_readPos.x + 1, _readPos.y);
        _horizontalColor = gInput[_nextPos.xy].rgb;
    
        _nextPos.x = _readPos.x;
        _nextPos.y += 1;
        _verticalColor = gInput[_nextPos.xy].rgb;
    
        _nextPos.x += 1;
        _virtualColor = gInput[_nextPos.xy].rgb;
    }
    
    // 행 먼저 채운다
    // 0,0(groupID.xy) 에서 1만큼 떨어진 곳
    {
        [unroll]
        for (uint w = 0; w < 3; w++)
        {
            float _lerpFactor = 0.25 + (w * 0.25);
            float3 _lerpColor = lerp(_originColor.rgb, _horizontalColor.rgb, _lerpFactor);
        
            uint2 _nowPos = uint2(_writePos.x + (w + 1), _writePos.y); // 그려질 행의 위치
            gOutput[_nowPos.xy] = float4(_lerpColor, 1.0); // 실제 그리는 동작
            _oriHoir[w] = float4(_lerpColor, 1.0); // 아래 로직에서 참조를 위해 배열에 저장
            
            // 0,1 | 1,1 픽셀을 보간해서 배열에 담아둔다
            _lerpColor = lerp(_verticalColor.rgb, _virtualColor.rgb, _lerpFactor);
            _virtHori[w] = float4(_lerpColor, 1.0);
        }
    }
    
    // 열을 채운다
    {
        [unroll]
        for (uint h = 0; h < 3; h++)
        {
            float _lerpFactor = 0.25 + (h * 0.25);
            float3 _lerpColor = lerp(_originColor.rgb, _verticalColor.rgb, _lerpFactor);
        
            uint2 _nowPos = uint2(_writePos.x, _writePos.y + (h + 1));
            gOutput[_nowPos.xy] = float4(_lerpColor, 1.0);
            _oriVert[h] = float4(_lerpColor, 1.0);
        
            // 1,0 | 1,1 픽셀을 보간해서 배열에 담아둔다
            _lerpColor = lerp(_horizontalColor.rgb, _virtualColor.rgb, _lerpFactor);
            _virtVert[h] = float4(_lerpColor, 1.0);
        }
    }
 
    
    float3 _oriHoriColor = 0;
    float3 _virHoriColor = 0;
    float3 _oriVertColor = 0;
    float3 _virVertColor = 0;
    
    _writePos.x = (groupID.x * 4);
    _writePos.y = (groupID.y * 4);
    
    // 열로 내려가면서 전진한다
    _writePos.y += 1;
    [unroll]
    for (uint w1 = 0; w1 < 3; w1++)
    {
        _oriHoriColor = _oriHoir[w1].rgb;
        _virHoriColor = _virtHori[w1].rgb;
        
        float3 _horiLerpColor = lerp(_oriHoriColor.rgb, _virHoriColor.rgb, 0.25);
        
        float _lerpFactor = (0.25 + (w1 * 0.25));
        
        _oriVertColor = _oriVert[0].rgb;
        _virVertColor = _virtVert[0].rgb;
        float3 _vertLerpColor = lerp(_oriVertColor.rgb, _virVertColor.rgb, _lerpFactor);
       
        // 그릴 위치
        uint2 _nowPos = uint2(_writePos.x + (1 + w1), _writePos.y);
        
        gOutput[_nowPos.xy] = float4((_vertLerpColor + _vertLerpColor) / 2, 1.0);
    }
    
    _writePos.y += 1;
    [unroll]
    for (uint w2 = 0; w2 < 3; w2++)
    {
        _oriHoriColor = _oriHoir[w2].rgb;
        _virHoriColor = _virtHori[w2].rgb;
        
        float3 _horiLerpColor = lerp(_oriHoriColor.rgb, _virHoriColor.rgb, 0.5);
        
        float _lerpFactor = (0.25 + (w2 * 0.25));
       
        _oriVertColor = _oriVert[1].rgb;
        _virVertColor = _virtVert[1].rgb;
        float3 _vertLerpColor = lerp(_oriVertColor.rgb, _virVertColor.rgb, _lerpFactor);

        // 그릴 위치
        uint2 _nowPos = uint2(_writePos.x + (1 + w2), _writePos.y);
        gOutput[_nowPos.xy] = float4((_vertLerpColor + _vertLerpColor) / 2, 1.0);

    }
    
    _writePos.y += 1;
    [unroll]
    for (uint w3 = 0; w3 < 3; w3++)
    {
        _oriHoriColor = _oriHoir[w3].rgb;
        _virHoriColor = _virtHori[w3].rgb;
        
        float3 _horiLerpColor = lerp(_oriHoriColor.rgb, _virHoriColor.rgb, 0.75);
        
        float _lerpFactor = (0.25 + (w3 * 0.25));
        
        _oriVertColor = _oriVert[2].rgb;
        _virVertColor = _virtVert[2].rgb;
        float3 _vertLerpColor = lerp(_oriVertColor.rgb, _virVertColor.rgb, _lerpFactor);

        // 그릴 위치
        uint2 _nowPos = uint2(_writePos.x + (1 + w3), _writePos.y);
        gOutput[_nowPos.xy] = float4((_vertLerpColor + _vertLerpColor) / 2, 1.0);
    } 
    
}

#define STADARD_COLOR 4
#define TOP     3
#define BOTTOM  3
#define LEFT    3
#define RIGHT   3

#define PIXEL00 0
#define PIXEL10 1
#define PIXEL01 2
#define PIXEL11 3

#define POS10 4
#define POS20 5
#define POS30 6

#define POS14 7
#define POS24 8
#define POS34 9

#define POS01 10
#define POS02 11
#define POS03 12

#define POS41 13
#define POS42 14
#define POS43 15


groupshared float4 gColorArray[STADARD_COLOR + TOP + BOTTOM + LEFT + RIGHT];

[numthreads(4, 4, 1)]
void UpSampleBilinearLerp4x4(uint3 groupID : SV_GroupID, uint3 groupThreadID : SV_GroupThreadID)
{
    // 16개의 픽셀 중 가장 첫번째 위치를 정해준다
    // 작은 사이즈(원본)을 하나씩 읽어오는 방식, 4배로 키울 것이기 때문에 
    float3 _originColor = 0;
    
    uint2 _readPos = groupID.xy;
    uint2 _writePos;
    
    float3 _horizontalColor = 0; // 1,0
    float3 _verticalColor = 0; // 0,1
    float3 _virtualColor = 0; // 1,1
    
    // 0,0 위치를 채운다. 해당 위치가 OriginColor가 된다.
    if (groupThreadID.x == 0 && groupThreadID.y == 0)
    {
        _writePos.x = (groupID.x * 4);
        _writePos.y = (groupID.y * 4);
    
        _originColor = gInput[_readPos.xy].rgb;
        gOutput[_writePos.xy] = float4(_originColor, 1.0);
        gColorArray[PIXEL00] = float4(_originColor, 1.0);
        
        // 1,0 | 0,1 | 1,1 에 값을 채워준다
        uint2 _nextPos;
        {
            _nextPos = uint2(_readPos.x + 1, _readPos.y);
            gColorArray[PIXEL10] = float4(gInput[_nextPos.xy].rgb, 1.0);
    
            _nextPos = uint2(_readPos.x, _readPos.y + 1);
            gColorArray[PIXEL01] = float4(gInput[_nextPos.xy].rgb, 1.0);
    
            _nextPos = uint2(_readPos.x + 1, _readPos.y + 1);
            gColorArray[PIXEL11] = float4(gInput[_nextPos.xy].rgb, 1.0);
        }
    }
    
    GroupMemoryBarrierWithGroupSync();
    
    // 행 먼저 채운다
    if (groupThreadID.y == 0 && groupThreadID.x != 0)
    {
        float _lerpFactor = groupThreadID.x * 0.25;
        float3 _lerpColor = lerp(gColorArray[PIXEL00].rgb, gColorArray[PIXEL10].rgb, _lerpFactor);
        
        gColorArray[3 + groupThreadID.x] = float4(_lerpColor, 1.0); // 아래 로직에서 참조를 위해 배열에 저장
            
        // 0,1 | 1,1 픽셀을 보간해서 배열에 담아둔다
        _lerpColor = lerp(gColorArray[PIXEL01].rgb, gColorArray[PIXEL11].rgb, _lerpFactor);
        gColorArray[6 + groupThreadID.x] = float4(_lerpColor, 1.0);
        
    }
    
    GroupMemoryBarrierWithGroupSync();
    
    // 열을 채운다
    if (groupThreadID.x == 0 && groupThreadID.y != 0)
    {
        float _lerpFactor = groupThreadID.y * 0.25;
        float3 _lerpColor = lerp(gColorArray[PIXEL00].rgb, gColorArray[PIXEL01].rgb, _lerpFactor);
        
        gColorArray[9 + groupThreadID.y] = float4(_lerpColor, 1.0); // 아래 로직에서 참조를 위해 배열에 저장
        
        // 0,1 | 1,1 픽셀을 보간해서 배열에 담아둔다
        _lerpColor = lerp(gColorArray[PIXEL10].rgb, gColorArray[PIXEL11].rgb, _lerpFactor);
        gColorArray[12 + groupThreadID.y] = float4(_lerpColor, 1.0);
        
    }
 
    GroupMemoryBarrierWithGroupSync();
    
    float3 _oriHoriColor = 0;
    float3 _virHoriColor = 0;
    float3 _oriVertColor = 0;
    float3 _virVertColor = 0;
    
    _writePos.x = (groupID.x * 4);
    _writePos.y = (groupID.y * 4);
    
    // 열로 내려가면서 전진한다
    if (groupThreadID.y == 0)
    {
        float _lerpFactor = groupThreadID.x * 0.25;
        
        _oriVertColor = gColorArray[PIXEL00].rgb;
        _virVertColor = gColorArray[PIXEL10].rgb;
        float3 _vertLerpColor = lerp(_oriVertColor.rgb, _virVertColor.rgb, _lerpFactor);
       
        // 그릴 위치
        uint2 _nowPos = uint2(_writePos.x + groupThreadID.x, _writePos.y);
        gOutput[_nowPos.xy] = float4(_vertLerpColor, 1.0);
    }
    
    _writePos.y += 1;
    if (groupThreadID.y == 1 && groupThreadID.x != 0)
    {
        uint _index1 = (POS10 - 1) + groupThreadID.x;
        uint _index2 = (POS14 - 1) + groupThreadID.x;
        
        _oriHoriColor = gColorArray[_index1].rgb;
        _virHoriColor = gColorArray[_index2].rgb;
        
        float3 _horiLerpColor = lerp(_oriHoriColor.rgb, _virHoriColor.rgb, 0.25);
        
        float _lerpFactor = groupThreadID.x * 0.25;
        
        _oriVertColor =  gColorArray[POS01].rgb;
        _virVertColor =  gColorArray[POS41].rgb;
        float3 _vertLerpColor = lerp(_oriVertColor.rgb, _virVertColor.rgb, _lerpFactor);
       
        // 그릴 위치
        uint2 _nowPos = uint2(_writePos.x + groupThreadID.x, _writePos.y);
        gOutput[_nowPos.xy] = float4((_horiLerpColor + _vertLerpColor) / 2, 1.0);
    }

    // 열로 내려가면서 전진한다
    _writePos.y += 1;
    if (groupThreadID.y == 2 && groupThreadID.x != 0)
    {
        uint _index1 = (POS10 - 1) + groupThreadID.x;
        uint _index2 = (POS14 - 1) + groupThreadID.x;
        
        _oriHoriColor = gColorArray[_index1].rgb;
        _virHoriColor = gColorArray[_index2].rgb;
        
        float3 _horiLerpColor = lerp(_oriHoriColor.rgb, _virHoriColor.rgb, 0.5);
        
        float _lerpFactor = groupThreadID.x * 0.25;
        
        _oriVertColor = gColorArray[POS02].rgb;
        _virVertColor = gColorArray[POS42].rgb;
        float3 _vertLerpColor = lerp(_oriVertColor.rgb, _virVertColor.rgb, _lerpFactor);
       
        // 그릴 위치
        uint2 _nowPos = uint2(_writePos.x + groupThreadID.x, _writePos.y);
        gOutput[_nowPos.xy] = float4((_horiLerpColor + _vertLerpColor) / 2, 1.0);
    }
    
    _writePos.y += 1;
    if (groupThreadID.y == 3 && groupThreadID.x != 0)
    {
        uint _index1 = (POS10 - 1) + groupThreadID.x;
        uint _index2 = (POS14 - 1) + groupThreadID.x;
        
        _oriHoriColor = gColorArray[_index1].rgb;
        _virHoriColor = gColorArray[_index2].rgb;
        
        float3 _horiLerpColor = lerp(_oriHoriColor.rgb, _virHoriColor.rgb, 0.75);
        
        float _lerpFactor = groupThreadID.x * 0.25;
        
        _oriVertColor = gColorArray[POS03].rgb;
        _virVertColor = gColorArray[POS43].rgb;
        float3 _vertLerpColor = lerp(_oriVertColor.rgb, _virVertColor.rgb, _lerpFactor);
       
        // 그릴 위치
        uint2 _nowPos = uint2(_writePos.x + groupThreadID.x, _writePos.y);
        gOutput[_nowPos.xy] = float4((_horiLerpColor + _vertLerpColor) / 2, 1.0);
    }
    
    _writePos.x = (groupID.x * 4);
    _writePos.y = (groupID.y * 4);
    
    if (groupThreadID.x == 0 && groupThreadID.y != 0)
    {
        uint2 _nowPos = uint2(_writePos.x, _writePos.y + groupThreadID.y);
        
        float _lerpFactor = groupThreadID.y * 0.25;
        float3 _lerpColor = lerp(gColorArray[PIXEL00].rgb, gColorArray[PIXEL01].rgb, _lerpFactor);
        
        gOutput[_nowPos.xy] = float4(_lerpColor, 1.0);
    }
}

// 1024 사이즈만큼 가로로 접근하고 세로로도 쓸거임
[numthreads(1024, 1, 1)]
void ColorAdjust(uint3 groupID : SV_GroupID, uint3 groupThreadID : SV_GroupThreadID)
{
    // 세로 픽셀의 수 만큼 반복문을 조져야함
    uint2 currPixel = 0;
    float3 nowColor = 0;
    float mostVal = 0.0;
    
    for (uint i = 0; i < gHeight; i++)
    {
        currPixel.x = (groupID.x * 1024) + groupThreadID.x;
        currPixel.y = i;
        
        nowColor = gInput[currPixel].rgb;
        
        mostVal = nowColor.x;
        if (mostVal < nowColor.y)
            mostVal = nowColor.y;
        if (mostVal < nowColor.z)
            mostVal = nowColor.z;
        
        if (mostVal > gAdjustFactor)
        {
            nowColor /= mostVal;
            gOutput[currPixel] = float4(nowColor, 1.0); // 부분적으로     
        }
        else
        {
            gOutput[currPixel] = float4(nowColor, 1.0); 
        }
    }
}