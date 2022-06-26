Texture2D gInputTexture : register(t0);
Texture2D gInputTexture2 : register(t1);
Texture2D<uint4> gInputIDTextrue : register(t2);

Texture2D gTransparentTexture : register(t3);
Texture2D gTransparentTexture2 : register(t4);
Texture2D<uint4> gTransparentIDTextrue : register(t5);

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
    
    // ���� ���� ����
    // MousePosX�� �ϳ��� ������ �ȿ� ������ �Ѵ�
    
    // ������ �׷츸ŭ ���� �� �ȿ� ����
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
    
    // ���� ���� ����
    // MousePosX�� �ϳ��� ������ �ȿ� ������ �Ѵ�
    
    // ������ �׷츸ŭ ���� �� �ȿ� ����
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
    float4 Position; // ���� + ����
};

RWStructuredBuffer<DeferredData> gDeferredOutPut;

[numthreads(MAX_THREAD, MAX_THREAD, 1)]
void DeferredPicking(
uint3 groupID : SV_GroupID,
uint3 groupThreadID : SV_GroupThreadID,
uint3 dispatchThreadID : SV_DispatchThreadID)
{
    // ���� ���� ����
    // MousePosX�� �ϳ��� ������ �ȿ� ������ �Ѵ�
    
    // ������ �׷츸ŭ ���� �� �ȿ� ����
    uint _nowGroup = MousePosX / MAX_THREAD;
    if (groupID.x < _nowGroup || groupID.x > _nowGroup)
        return;
    
    _nowGroup = MousePosY / MAX_THREAD;
    if (groupID.y < _nowGroup || groupID.y > _nowGroup) 
        return;
    
    // �ᱹ Ư�� ���콺 ��ġ�� �ش��ϴ� �����常 �۵��Ѵ�
    if (MousePosX % MAX_THREAD == groupThreadID.x && MousePosY % MAX_THREAD == groupThreadID.y)
    {
        uint2 _currPixel;
        _currPixel.x = (groupID.x * MAX_THREAD) + groupThreadID.x;
        _currPixel.y = (groupID.y * MAX_THREAD) + groupThreadID.y;
                
        // �븻�� �������� ����� ��������Ƿ� �ٽ� ���� ������ �����ִ� �۾�
        float3 _normal = normalize((gInputTexture[_currPixel].xyz - 0.5f) * 2.0f);
        float4 _position = gInputTexture2[_currPixel];
        
        gDeferredOutPut[0].Normal.xyz = _normal.xyz;
        gDeferredOutPut[0].Position = _position;
        
        uint _objID = gInputIDTextrue[_currPixel].x;
        gDeferredOutPut[0].ObjectID = _objID;
    }
}


[numthreads(MAX_THREAD, MAX_THREAD, 1)]
void DeferredTransparentPicking(
uint3 groupID : SV_GroupID,
uint3 groupThreadID : SV_GroupThreadID,
uint3 dispatchThreadID : SV_DispatchThreadID)
{
    // ���� ���� ����
    // MousePosX�� �ϳ��� ������ �ȿ� ������ �Ѵ�
    
    // ������ �׷츸ŭ ���� �� �ȿ� ����
    uint _nowGroup = MousePosX / MAX_THREAD;
    if (groupID.x < _nowGroup || groupID.x > _nowGroup)
        return;
    
    _nowGroup = MousePosY / MAX_THREAD;
    if (groupID.y < _nowGroup || groupID.y > _nowGroup) 
        return;
    
    // �ᱹ Ư�� ���콺 ��ġ�� �ش��ϴ� �����常 �۵��Ѵ�
    if (MousePosX % MAX_THREAD == groupThreadID.x && MousePosY % MAX_THREAD == groupThreadID.y)
    {
        uint2 _currPixel;
        _currPixel.x = (groupID.x * MAX_THREAD) + groupThreadID.x;
        _currPixel.y = (groupID.y * MAX_THREAD) + groupThreadID.y;
                
        // �븻�� �������� ����� ��������Ƿ� �ٽ� ���� ������ �����ִ� �۾�
        float4 _opaquePosition = gInputTexture2[_currPixel];
        float3 _opaqueNormal = normalize((gInputTexture[_currPixel].xyz - 0.5f) * 2.0f);
        
        float4 _transparentPosition = gTransparentTexture2[_currPixel];
        
        // near�� �������� ���� ����
        if (_opaquePosition.w < _transparentPosition.w)
        {
            gDeferredOutPut[0].Normal.xyz = _opaqueNormal.xyz;
            gDeferredOutPut[0].Position = _opaquePosition;
        
            uint _objID = gInputIDTextrue[_currPixel].x;
            gDeferredOutPut[0].ObjectID = _objID;
        }
        else
        {

            float3 _transparentNormal = normalize((gTransparentTexture[_currPixel].xyz - 0.5f) * 2.0f);
            
            gDeferredOutPut[0].Normal.xyz = _transparentNormal.xyz;
            gDeferredOutPut[0].Position = _transparentPosition;
            
            uint _objID = gTransparentIDTextrue[_currPixel].x;
            gDeferredOutPut[0].ObjectID = _objID;
            
        }
    }
}