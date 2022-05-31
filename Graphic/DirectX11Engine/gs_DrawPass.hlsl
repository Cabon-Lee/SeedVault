#include "ParticleLib.hlsli"

#pragma pack_matrix( row_major )

cbuffer cbPerFrame : register(b0)
{
    float4x4 gViewProj;
    float3 gEyePosW;
}

[maxvertexcount(4)]
void main(point DrawPassVertexOut gin[1], inout TriangleStream<GeoOut> triStream)
{
    
    float2 gQuadTexC[4];
    gQuadTexC[0] = float2(0.0f, 1.0f);
    gQuadTexC[1] = float2(1.0f, 1.0f);
    gQuadTexC[2] = float2(0.0f, 0.0f);
    gQuadTexC[3]= float2(1.0f, 0.0f);
    
	// do not draw emitter particles.
    if (gin[0].Type != PT_EMITTER)
    {
        
        // Z축에 대한 2X2 행렬을 만든다
        float _radian = radians(gin[0].ZRot);
        float3 _basis1 = { cos(_radian), sin(_radian), 0 };
        float3 _basis2 = { -sin(_radian), cos(_radian), 0 };
        float3x3 _rTM;
        
        _rTM._11 = _basis1.x; _rTM._12 = _basis1.y; _rTM._13 = _basis1.z;
        _rTM._21 = _basis2.x; _rTM._22 = _basis2.y; _rTM._23 = _basis2.z;
        _rTM._31 = 0.0;       _rTM._32 = 0.0;       _rTM._33 = 1.0;
        
		// 카메라의 위치와 버텍스의 위치를 통해 빌보드 행렬을 만든다
        float3 look = normalize(gEyePosW.xyz - gin[0].PosW);
        float3 right = normalize(cross(float3(0, 1, 0), look));
        float3 up = cross(look, right);
        float3x3 _oriTM;
        
        _oriTM._11 = right.x; _oriTM._12 = right.y; _oriTM._13 = right.z;
        _oriTM._21 = up.x;    _oriTM._22 = up.y;    _oriTM._23 = up.z;
        _oriTM._31 = look.x;  _oriTM._32 = look.y;  _oriTM._33 = look.z;
        
        // 회전 행렬과 빌보드의 행렬을 연산해 Z축으로 회전 시킨다
        _oriTM = mul(_rTM, _oriTM);
        right = _oriTM._11_12_13; up = _oriTM._21_22_23; look = _oriTM._31_32_33;
        
        // 받아온 사이즈를 통해 빌보드의 크기를 정의한다 만든다
        float halfWidth = 0.5f * gin[0].SizeW.x;
        float halfHeight = 0.5f * gin[0].SizeW.y;
        
        float4 v[4];
        v[0] = float4(gin[0].PosW + halfWidth * right - halfHeight * up, 1.0f);
        v[1] = float4(gin[0].PosW + halfWidth * right + halfHeight * up, 1.0f);
        v[2] = float4(gin[0].PosW - halfWidth * right - halfHeight * up, 1.0f);
        v[3] = float4(gin[0].PosW - halfWidth * right + halfHeight * up, 1.0f);
        
		// 쿼드 형태의 버텍스들을 내보낸다
        GeoOut gout;
		[unroll]
        for (int i = 0; i < 4; ++i)
        {
            gout.PosH = mul(v[i], gViewProj);
            gout.Tex = gQuadTexC[i];
            gout.Color = gin[0].Color;
            triStream.Append(gout);
        }
    }
}