struct Particle
{
    float3 InitialPosW : POSITION;
    float3 InitialVelW : VELOCITY;
    float2 SizeW : SIZE;
    float2 Age : AGE;
    uint Type : TYPE;
};

struct DrawPassVertexOut
{
    float3 PosW : POSITION0;
    float3 Dir : POSITION1;
    float2 SizeW : SIZE;
    float4 Color : COLOR;
    uint Type : TYPE0;
    float ZRot : TYPE1;
};

struct GeoOut
{
    float4 PosH : SV_Position;
    float4 Color : COLOR;
    float2 Tex : TEXCOORD;
};

#define PT_EMITTER 0
#define PT_FLARE 1