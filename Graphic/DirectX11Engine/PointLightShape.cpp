#include "PointLightShape.h"

#include "VertexHelper.h"
#include "Shaders.h"

#include "RasterizerState.h"
#include "ConstantBufferManager.h"
#include "SimpleMath.h"

PointLightShape::PointLightShape()
{

}

PointLightShape::~PointLightShape()
{

}

void PointLightShape::BuildHelper(ID3D11Device* pDevice)
{
	Vertex::VertexColor _vertices[] =
	{
		// ¸¶Á¨Å¸
		{ XMFLOAT3(+0.00f, +0.500f, +0.850f),	XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f)},
		{ XMFLOAT3(+0.00f, -0.500f, +0.850f),	XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f)},
		{ XMFLOAT3(+0.75f, +0.500f, -0.430f),	XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f)},
		{ XMFLOAT3(+0.75f, -0.500f, -0.430f),	XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f)},
		{ XMFLOAT3(+0.00f, -1.000f, -0.000f),	XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f)}, //4
		{ XMFLOAT3(+0.00f, +1.000f, -0.000f),	XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f)},
		{ XMFLOAT3(-0.75f, +0.500f, -0.430f),	XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f)},
		{ XMFLOAT3(-0.75f, -0.500f, -0.430f),	XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f)},
		{ XMFLOAT3(-0.00f, -0.500f, +0.850f),	XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f)},
		{ XMFLOAT3(+0.75f, -0.500f, -0.430f),	XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f)},	//9
		{ XMFLOAT3(+0.00f, +0.500f, +0.850f),	XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f)},
		{ XMFLOAT3(+0.75f, +0.500f, -0.430f),	XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f)},
		{ XMFLOAT3(+0.00f, -1.000f, +0.000f),	XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f)},
		{ XMFLOAT3(+0.75f, -0.500f, -0.430f),	XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f)},
		{ XMFLOAT3(+0.75f, -0.500f, -0.430f),	XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f)},	// 14
		{ XMFLOAT3(+0.75f, +0.500f, -0.430f),	XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f)},
		{ XMFLOAT3(-0.75f, -0.500f, -0.430f),	XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f)},
		{ XMFLOAT3(+0.75f, +0.500f, -0.430f),	XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f)},
		{ XMFLOAT3(+0.00f, +1.000f, +0.000f),	XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f)},
		{ XMFLOAT3(-0.75f, +0.500f, -0.430f),	XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f)},	// 19
		{ XMFLOAT3(+0.00f, -1.000f, +0.000f),	XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f)},
		{ XMFLOAT3(-0.75f, -0.500f, -0.430f),	XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f)},
		{ XMFLOAT3(+0.00f, -0.500f, +0.850f),	XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f)},
		{ XMFLOAT3(-0.75f, -0.500f, -0.430f),	XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f)},
		{ XMFLOAT3(-0.75f, +0.500f, -0.430f),	XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f)},	// 24
		{ XMFLOAT3(+0.00f, +0.500f, +0.850f),	XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f)},
		{ XMFLOAT3(+0.00f, -0.500f, +0.850f),	XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f)},
		{ XMFLOAT3(-0.75f, +0.500f, -0.430f),	XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f)},
		{ XMFLOAT3(+0.00f, +1.000f, +0.000f),	XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f)},
		{ XMFLOAT3(+0.00f, +0.500f, +0.850f),	XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f)},	// 29
	};


	UINT _indices[] =
	{
		4,	 1, 3 ,
		8,	 0, 2 ,
		2,	 9, 8 ,
		10,  5, 11,
		12, 13, 7 ,
		14, 15, 6 ,
		6,  16, 14,
		17, 18, 19,
		20, 21, 22,
		23, 24, 25,
		25, 26, 23,
		27, 28, 29,
	};

	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(Vertex::VertexColor) * 30;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = _vertices;
	pDevice->CreateBuffer(&vbd, &vinitData, m_VertexBuffer.GetAddressOf());

	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(UINT) * 36;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	ibd.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA iinitData;
	iinitData.pSysMem = _indices;
	pDevice->CreateBuffer(&ibd, &iinitData, m_IndexBuffer.GetAddressOf());

	m_Stride = sizeof(Vertex::VertexColor);
	m_IndexCount = 36;
}

void PointLightShape::HelperRender(
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContex,
	std::shared_ptr<VertexShader> pVertexShader, std::shared_ptr<PixelShader> pPixelShader,
	const DirectX::XMMATRIX& world, const DirectX::XMMATRIX& view, const DirectX::XMMATRIX& proj)
{
	pDeviceContex->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	pDeviceContex->RSSetState(RasterizerState::GetWireFrameRS());

	
	ConstantBufferManager::GetVertexBuffer()->data.wvpMatrix = DirectX::SimpleMath::Matrix::CreateScale(0.1f) * world * view * proj;
	ConstantBufferManager::GetVertexBuffer()->data.worldMatrix = DirectX::SimpleMath::Matrix::CreateScale(0.1f) * world;
	ConstantBufferManager::GetVertexBuffer()->ApplyChanges();
	pDeviceContex->VSSetConstantBuffers(0, 1, ConstantBufferManager::GetVertexBuffer()->GetAddressOf());

	pDeviceContex->IASetInputLayout(pVertexShader->inputLayout.Get());
	pDeviceContex->VSSetShader(pVertexShader->GetVertexShader(), NULL, 0);
	pDeviceContex->PSSetShader(pPixelShader->GetPixelShader(), NULL, 0);

	const UINT _stride = sizeof(Vertex::VertexColor);
	const UINT _offset = 0;

	pDeviceContex->IASetVertexBuffers(0, 1, m_VertexBuffer.GetAddressOf(), &_stride, &_offset);
	pDeviceContex->IASetIndexBuffer(m_IndexBuffer.Get(), DXGI_FORMAT::DXGI_FORMAT_R32_UINT, 0);

	pDeviceContex->DrawIndexed(m_IndexCount, 0, 0);
}
