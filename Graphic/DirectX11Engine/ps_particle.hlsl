////////////////////////////////////////////////////////////////////////////////
// Filename: particle_ps.hlsl
////////////////////////////////////////////////////////////////////////////////

/////////////
// GLOBALS //
/////////////
Texture2D shaderTexture;
SamplerState SampleType;

//////////////
// TYPEDEFS //
//////////////
struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float4 color : COLOR;
};

////////////////////////////////////////////////////////////////////////////////
// Pixel Shader
////////////////////////////////////////////////////////////////////////////////
float4 main(PixelInputType input) : SV_TARGET
{
    float4 textureColor;
    float4 finalColor;


    // �� �ؽ�ó ��ǥ ��ġ���� ���÷��� ����Ͽ� �ؽ�ó���� �ȼ� ������ ���ø��մϴ�.
    textureColor = shaderTexture.Sample(SampleType, input.tex);

    // �ؽ�ó ����� ���� ������ �����Ͽ� ���� ���� ����� ����ϴ�.
    finalColor = textureColor * input.color;

    return finalColor;
}