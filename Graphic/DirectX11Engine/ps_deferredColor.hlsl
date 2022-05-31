
struct VS_OUTPUT
{
    float4 inPosition : SV_POSITION;
    float4 inColor : COLOR;
};

float4 main(VS_OUTPUT pin) : SV_Target
{
    float4 color;
    
    color.xyz = pin.inColor;
    color.w = 1.0f;
   
    return color;
}
