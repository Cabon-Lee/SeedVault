#pragma once
#include <d3d11.h>
#include <DirectXMath.h>

#define SHADER_REG_CB_DIRECTIONAL_LIGHT 0
#define SHADER_REG_CB_POINT_LIGHT 1
#define SHADER_REG_CB_SPOT_LIGHT 2
#define SHADER_REG_CB_EYE 3
#define SHADER_REG_CB_MATERIAL 4
#define SHADER_REG_CB_LIGHTVP 5
#define SHADER_REG_CB_COLOR 6

#define SHADER_REG_SRV_CM 0
#define SHADER_REG_SRV_DCM 1
#define SHADER_REG_SRV_DIFFUSE 2
#define SHADER_REG_SRV_NORMAL 3
#define SHADER_REG_SRV_SSAO 4
#define SHADER_REG_SRV_SHADOW 5
#define SHADER_REG_SRV_SHADOW_TRANSP 6
#define SHADER_REG_SRV_DISPLACE 7
#define SHADER_REG_SRV_ROUGHNESS 8
#define SHADER_REG_SRV_METALLIC 9
#define SHADER_REG_SRV_DEPTH 10

#define SHADER_REG_SAMP_POINT 0
#define SHADER_REG_SAMP_LINEAR 1
#define SHADER_REG_SAMP_LINEAR_POINT 3
#define SHADER_REG_SAMP_ANISOTROPIC 4
#define SHADER_REG_SAMP_CMP_POINT 5
#define SHADER_REG_SAMP_CMP_LINEAR_POINT 6

#define FORWARD XMFLOAT3(0,0,1)
#define RIGHT XMFLOAT3(1,0,0)
#define UP XMFLOAT3(0,1,0)

struct SHADER_MATERIAL
{
private:
	DirectX::XMFLOAT4 diffuse;
	DirectX::XMFLOAT4 ambient;
	DirectX::XMFLOAT4 specular;

public:
	SHADER_MATERIAL() {}
	SHADER_MATERIAL(DirectX::XMFLOAT3 d, float transparency, DirectX::XMFLOAT3 a, DirectX::XMFLOAT3 s)
		:
		diffuse(DirectX::XMFLOAT4(d.x, d.y, d.z, transparency)),
		ambient(DirectX::XMFLOAT4(a.x, a.y, a.z, 0)),
		specular(DirectX::XMFLOAT4(s.x, s.y, s.z, 0))
	{}
	void SetTransparency(float t)
	{
		diffuse.w = t;
	}
	void SetSpecPower(float p)
	{
		specular.w = p;
	}
	void SetDiffuse(DirectX::XMFLOAT3 d)
	{
		diffuse.x = d.x;
		diffuse.y = d.y;
		diffuse.z = d.z;
	}
	void SetSpec(DirectX::XMFLOAT3 s)
	{
		specular.x = s.x;
		specular.y = s.y;
		specular.z = s.z;
	}
	void SetAmbient(DirectX::XMFLOAT3 a)
	{
		ambient.x = a.x;
		ambient.y = a.y;
		ambient.z = a.z;
	}
};

struct SHADER_STD_TRANSF
{
	DirectX::XMMATRIX w;
	DirectX::XMMATRIX v;
	DirectX::XMMATRIX p;
	DirectX::XMMATRIX n;
	float fNear;
	float fFar;
	float fAspect;
	float fRatio;

	SHADER_STD_TRANSF(DirectX::XMMATRIX v, DirectX::XMMATRIX p, float fNear, float fFar, float fAspect, float fRatio)
		:v(v), p(p), fNear(fNear), fFar(fFar), fAspect(fAspect), fRatio(fRatio)
	{
		const DirectX::XMMATRIX& mId = DirectX::XMMatrixIdentity();
		w = mId;
		n = mId;
	}
	SHADER_STD_TRANSF(DirectX::XMMATRIX world, DirectX::XMMATRIX v, DirectX::XMMATRIX p, float fNear, float fFar, float fAspect, float fRatio)
		:w(world), v(v), p(p), fNear(fNear), fFar(fFar), fAspect(fAspect), fRatio(fRatio)
	{
		DirectX::XMVECTOR det = DirectX::XMMatrixDeterminant(world);
		n = DirectX::XMMatrixTranspose(DirectX::XMMatrixInverse(&det, world));
	}
};

static const D3D11_INPUT_ELEMENT_DESC Std_ILayouts[] =
{
	{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, sizeof(DirectX::XMFLOAT3), D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, sizeof(DirectX::XMFLOAT3) + sizeof(DirectX::XMFLOAT2), D3D11_INPUT_PER_VERTEX_DATA, 0 },
	//{"TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT,0,sizeof(XMFLOAT3) * 2 + sizeof(XMFLOAT2), D3D11_INPUT_PER_VERTEX_DATA,0}
};
