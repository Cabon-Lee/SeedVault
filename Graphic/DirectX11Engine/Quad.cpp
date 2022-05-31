#include "Quad.h"

#include "VertexHelper.h"
#include "Shaders.h"

#include "RasterizerState.h"
#include "ConstantBufferManager.h"
#include "SimpleMath.h"

Quad::Quad()
{
}

Quad::~Quad()
{
}

void Quad::BuildHelper(ID3D11Device* pDevice)
{
	Vertex::Vertex2D _vertices[4] =
	{
		{ -1.0f, -1.0f, 0.0f, 0.0f, 1.0f},
		{ -1.0f, +1.0f, 0.0f, 0.0f, 0.0f},
		{ +1.0f, +1.0f, 0.0f, 1.0f, 0.0f},
		{ +1.0f, -1.0f, 0.0f, 1.0f, 1.0f},
	};
	
	UINT _indices[6] =
	{
		0, 1, 2, 0, 2, 3
	};

	ID3D11Buffer* _vb;
	ID3D11Buffer* _ib;

	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(Vertex::Vertex2D) * 4;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = _vertices;
	pDevice->CreateBuffer(&vbd, &vinitData, &_vb);

	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(UINT) * 6;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	ibd.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA iinitData;
	iinitData.pSysMem = _indices;
	pDevice->CreateBuffer(&ibd, &iinitData, &_ib);

	m_VertexBuffer = _vb;
	m_IndexBuffer = _ib;
	m_Stride = sizeof(Vertex::Vertex2D);
	m_IndexCount = 6;
}

void Quad::HelperRender(
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContex, 
	std::shared_ptr<VertexShader> pVertexShader, 
	std::shared_ptr<PixelShader> pPixelShader, 
	const DirectX::XMMATRIX& world, 
	const DirectX::XMMATRIX& view, 
	const DirectX::XMMATRIX& proj)
{
	// 하는일 없음
}


