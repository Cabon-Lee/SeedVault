#include "DirectionalLightShape.h"

#include "VertexHelper.h"
#include "Shaders.h"

#include "RasterizerState.h"
#include "ConstantBufferManager.h"
#include "SimpleMath.h"

DirectionalLightShape::DirectionalLightShape()
{

}

DirectionalLightShape::~DirectionalLightShape()
{

}

void DirectionalLightShape::BuildHelper(ID3D11Device* pDevice)
{
	Vertex::VertexColor _vertices[] =
	{
		// ³ë¶õ»ö

		{ XMFLOAT3(-0.00f, -0.070f, +0.350f),	XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f)},
		{ XMFLOAT3(-0.21f, +0.038f, -0.550f),	XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f)},
		{ XMFLOAT3(+0.21f, +0.038f, -0.550f),	XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f)},
		{ XMFLOAT3(-0.21f, -0.180f, -0.550f),	XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f)},
		{ XMFLOAT3(+0.21f, -0.180f, -0.550f),	XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f)},
	};

	UINT _indices[] =
	{
		0, 1,
		0, 2,
		0, 4,
		0, 3,
		1, 2,
		3, 4,
		2, 4,
		1, 3,
	};

	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(Vertex::VertexColor) * 5;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = _vertices;
	pDevice->CreateBuffer(&vbd, &vinitData, m_VertexBuffer.GetAddressOf());

	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(UINT) * 16;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	ibd.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA iinitData;
	iinitData.pSysMem = _indices;
	pDevice->CreateBuffer(&ibd, &iinitData, m_IndexBuffer.GetAddressOf());

	m_Stride = sizeof(Vertex::VertexColor);
	m_IndexCount = 16;
}

void DirectionalLightShape::HelperRender(
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContex, 
	std::shared_ptr<VertexShader> pVertexShader, 
	std::shared_ptr<PixelShader> pPixelShader, 
	const DirectX::XMMATRIX& world, 
	const DirectX::XMMATRIX& view, 
	const DirectX::XMMATRIX& proj)
{
	pDeviceContex->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	ConstantBufferManager::GetVertexBuffer()->data.wvpMatrix = world * view * proj;
	ConstantBufferManager::GetVertexBuffer()->data.worldMatrix = world;
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
