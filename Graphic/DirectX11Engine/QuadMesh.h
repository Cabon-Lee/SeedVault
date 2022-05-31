#pragma once
#include <Windows.h>
#include <d3d11.h>
#include <DirectXMath.h>
#include <wrl/client.h>

struct VertexOIT {
	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT2 tex;
	DirectX::XMFLOAT3 n;
	//XMFLOAT3 tangent;

	VertexOIT() :pos(DirectX::XMFLOAT3(0, 0, 0)), n(DirectX::XMFLOAT3(0, 0, 0)), tex(DirectX::XMFLOAT2(0, 0)) {}
	VertexOIT(const VertexOIT& v)
	{
		pos = v.pos;
		n = v.n;
		tex = v.tex;
	}
	VertexOIT(DirectX::XMFLOAT3 _pos)
	{
		pos = _pos;
		n = DirectX::XMFLOAT3(0, 0, 0);
		tex = DirectX::XMFLOAT2(0, 0);
	}
	VertexOIT(DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 n, DirectX::XMFLOAT2 tex, DirectX::XMFLOAT3 tangent) :pos(pos), n(n), tex(tex) {}
};

class MeshOIT
{
public:
	MeshOIT(Microsoft::WRL::ComPtr<ID3D11Device> pDevice, VertexOIT* vertice, UINT vertByteSize, UINT vertexCount, const UINT* indice, UINT idxCount, D3D_PRIMITIVE_TOPOLOGY primitiveType);
	~MeshOIT();
	void Apply(ID3D11DeviceContext* dContext)const;

	void CalculateTangent(DirectX::XMFLOAT3 v0, DirectX::XMFLOAT3 v1, DirectX::XMFLOAT3 v2, DirectX::XMFLOAT2 t0, DirectX::XMFLOAT2 t1, DirectX::XMFLOAT2 t2, OUT DirectX::XMFLOAT3* tangent);
	void GetLBound(OUT DirectX::XMFLOAT3* minPt, OUT DirectX::XMFLOAT3* maxPt);
	void SetPrimitiveType(D3D11_PRIMITIVE_TOPOLOGY p) { primitiveType = p; }
	D3D11_PRIMITIVE_TOPOLOGY GetPrimitiveType() { return primitiveType; }
protected:
	MeshOIT() {}
	void Init(Microsoft::WRL::ComPtr<ID3D11Device> pDevice, VertexOIT* vertice, UINT vertByteSize, UINT vertexCount, const UINT* indice, UINT idxCount, D3D_PRIMITIVE_TOPOLOGY primitiveType);

	UINT idxCount;

private:

	DirectX::XMFLOAT3 lMinPt, lMaxPt;
	ID3D11Buffer* vertexBuffer = nullptr;
	ID3D11Buffer* indexBuffer = nullptr;
	UINT vertByteSize;
	D3D11_PRIMITIVE_TOPOLOGY primitiveType;
};

class QuadMesh : public MeshOIT
{
public:
	QuadMesh(Microsoft::WRL::ComPtr<ID3D11Device> pDevice);
};