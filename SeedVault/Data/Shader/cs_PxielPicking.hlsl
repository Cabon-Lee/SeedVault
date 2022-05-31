Texture2D gInputTexture : register(t0);
Texture2D gInputTexture2 : register(t1);
Texture2D<uint4> gInputIDTextrue : register(t2);


struct PixelData
{
    float3 Normal;
    float3 Position;
};

RWStructuredBuffer<PixelData> gOutput;

cbuffer MousePos : register(b0)
{
    float MousePosX;
    float MousePosY;
    uint pad;
    uint pad2;
};

#define MAX_THREAD 32

[numthreads(MAX_THREAD, MAX_THREAD, 1)]
void PixelPicking(
uint3 groupID : SV_GroupID,
uint3 groupThreadID : SV_GroupThreadID)
{
    
    // 가장 먼저 할일
    // MousePosX를 하나의 쓰레드 안에 들어오게 한다
    
    // 쓰레드 그룹만큼 나눈 것 안에 포함
    uint _nowGroup = MousePosX / MAX_THREAD;
    if (groupID.x < _nowGroup || groupID.x > _nowGroup)
        return;
    
    _nowGroup = MousePosY / MAX_THREAD;
    if (groupID.y < _nowGroup || groupID.y > _nowGroup) 
        return;
    
    
    if (MousePosX % MAX_THREAD == groupThreadID.x && 
        MousePosY % MAX_THREAD == groupThreadID.y)
    {
        uint2 _currPixel;
        _currPixel.x = (groupID.x * MAX_THREAD) + groupThreadID.x;
        _currPixel.y = (groupID.y * MAX_THREAD) + groupThreadID.y;
        
        
        float3 _normal = normalize((gInputTexture[_currPixel].xyz - 0.5f) * 2.0f);
        float3 _position = gInputTexture2[_currPixel].xyz;
        
        
        gOutput[0].Normal.xyz = _normal.xyz;
        gOutput[0].Position.xyz = _position.xyz;
    }
}


struct PixelID
{
    float4 Color;
};

RWStructuredBuffer<PixelID> gIDOutput;

#define MAX_THREAD 32

[numthreads(MAX_THREAD, MAX_THREAD, 1)]
void IDPixelPicking(
uint3 groupID : SV_GroupID,
uint3 groupThreadID : SV_GroupThreadID,
uint3 dispatchThreadID : SV_DispatchThreadID)
{
    
    // 가장 먼저 할일
    // MousePosX를 하나의 쓰레드 안에 들어오게 한다
    
    // 쓰레드 그룹만큼 나눈 것 안에 포함
    uint _nowGroup = MousePosX / MAX_THREAD;
    if (groupID.x < _nowGroup || groupID.x > _nowGroup)
        return;
    
    _nowGroup = MousePosY / MAX_THREAD;
    if (groupID.y < _nowGroup || groupID.y > _nowGroup) 
        return;
    
    if (MousePosX % MAX_THREAD == groupThreadID.x && MousePosY % MAX_THREAD == groupThreadID.y)
    {
        uint2 _currPixel;
        _currPixel.x = (groupID.x * MAX_THREAD) + groupThreadID.x;
        _currPixel.y = (groupID.y * MAX_THREAD) + groupThreadID.y;
        
        
        float4 _color = gInputTexture[_currPixel];
        
        gIDOutput[0].Color = _color;
    }
}

struct DeferredData
{
    float3 Normal;
    uint ObjectID;
    float4 Position; // 월드 + 깊이
};

RWStructuredBuffer<DeferredData> gDeferredOutPut;

[numthreads(MAX_THREAD, MAX_THREAD, 1)]
void DeferredPicking(
uint3 groupID : SV_GroupID,
uint3 groupThreadID : SV_GroupThreadID,
uint3 dispatchThreadID : SV_DispatchThreadID)
{
    // 가장 먼저 할일
    // MousePosX를 하나의 쓰레드 안에 들어오게 한다
    
    // 쓰레드 그룹만큼 나눈 것 안에 포함
    uint _nowGroup = MousePosX / MAX_THREAD;
    if (groupID.x < _nowGroup || groupID.x > _nowGroup)
        return;
    
    _nowGroup = MousePosY / MAX_THREAD;
    if (groupID.y < _nowGroup || groupID.y > _nowGroup) 
        return;
    
    // 결국 특정 마우스 위치에 해당하는 스레드만 작동한다
    if (MousePosX % MAX_THREAD == groupThreadID.x && MousePosY % MAX_THREAD == groupThreadID.y)
    {
        uint2 _currPixel;
        _currPixel.x = (groupID.x * MAX_THREAD) + groupThreadID.x;
        _currPixel.y = (groupID.y * MAX_THREAD) + groupThreadID.y;
                
        // 노말의 음수값을 양수로 사상했으므로 다시 원래 값으로 돌려주는 작업
        float3 _normal = normalize((gInputTexture[_currPixel].xyz - 0.5f) * 2.0f);
        float4 _position = gInputTexture2[_currPixel];
        
        gDeferredOutPut[0].Normal.xyz = _normal.xyz;
        gDeferredOutPut[0].Position = _position;
        
        uint _objID = gInputIDTextrue[_currPixel].x;
        gDeferredOutPut[0].ObjectID = _objID;
    }
}
