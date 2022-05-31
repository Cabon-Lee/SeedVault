struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float3 Normal : NORMAL;
};

float4 main(PS_INPUT pin) : SV_Target
{
    float4 norColor = float4(pin.Normal * 0.5 + 0.5, 1);
    
    return norColor;
}
