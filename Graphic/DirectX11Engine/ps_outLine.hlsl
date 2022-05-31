SamplerState RT : SAMPLER : register(s0);
Texture2D Texture0 : register(t0);

struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float2 TexCoord : TEXCOORD0;
};

struct PS_OUTPUT
{
    float4 Color : COLOR0;
};

bool isApproximatelyEqual(float a, float b)
{
    return abs(a - b) <= (abs(a) < abs(b) ? abs(b) : abs(a)) * 0.00001;
}

//float4 main(float2 Tex : TEXCOORD0) //: COLOR
float4 main(PS_INPUT input) : SV_Target
{
    PS_OUTPUT output;
    
    float4 Color = 0;
    float4 Ret;
    
    float coord[3] = { -1, 0, +1 };
    float divider = 1;
    float MAP_CX = 512;
    float MAP_CY = 512;
    
    float mask[9] =
    {
        -1, -1, -1,
    -1, 8, -1,
    -1, -1, -1
    }; // Laplacian필터
    
    for (int i = 0; i < 9; i++)
    {
        Color += mask[i] * (Texture0.Sample(RT, input.TexCoord + float2(coord[i % 3] / MAP_CX, coord[i / 3] / MAP_CY)));
    }
    // 주황 rgb (255,99,71)
    float gray = Color.r * 0.3 + Color.g * 0.59 + Color.b * 0.11;
    output.Color = float4(gray, gray * 0.4, gray * 0.35 , 1) / divider;
   //if (isApproximatelyEqual(output.Color.r + output.Color.g + output.Color.b, 1.0f) == true)
   //{
   //    discard;
   //}
//    Ret = Texture0.Sample( RT, Tex ); // 원본이미지 그냥 출력
        return output.Color;
}

