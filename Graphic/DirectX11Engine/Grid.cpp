#include "Grid.h"

#include <vector>
#include "VertexHelper.h"

#include "RasterizerState.h"
#include "ResourceManager.h"
#include "Shaders.h"
#include "ConstantBufferManager.h"

#include "SimpleMath.h"

Grid::Grid()
{

}

Grid::~Grid()
{

}

void Grid::BuildHelper(ID3D11Device* pDevice)
{

	std::vector<Vertex::VertexColor> _vertices;
	_vertices.resize(4000);

	std::vector<UINT> _indices;
	_indices.resize(4000); // 3 indices per face

	int _index = 0;
	for (int x = 0; x < 2000; x += 2)
	{
		_vertices[x].pos = XMFLOAT3((float)_index - 500.0f, 0.0f, 500.0f);
		_vertices[x].rgba = (XMFLOAT4)DirectX::Colors::Gray;
		_indices[x] = x;

		_vertices[x + 1].pos = XMFLOAT3((float)_index - 500.0f, 0.0f, -500.0f);
		_vertices[x + 1].rgba = (XMFLOAT4)DirectX::Colors::Gray;
		_indices[x + 1] = x + 1;

		_index++;
	}

	_index = 0;
	for (int y = 2000; y < 4000; y += 2)
	{

		_vertices[y].pos = XMFLOAT3(500, 0.0f, (float)_index - 500.0f);
		_vertices[y].rgba = (XMFLOAT4)DirectX::Colors::Gray;
		_indices[y] = y;

		_vertices[y + 1].pos = XMFLOAT3(-500.0f, 0.0f, (float)_index - 500.0f);
		_vertices[y + 1].rgba = (XMFLOAT4)DirectX::Colors::Gray;
		_indices[y + 1] = y + 1;

		_index++;
	}


	ID3D11Buffer* _vb;
	ID3D11Buffer* _ib;

	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(Vertex::VertexColor) * 4000;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = _vertices.data();
	pDevice->CreateBuffer(&vbd, &vinitData, &_vb);

	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(UINT) * (UINT)_indices.size();
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	ibd.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA iinitData;
	iinitData.pSysMem = _indices.data();
	pDevice->CreateBuffer(&ibd, &iinitData, &_ib);


	m_VertexBuffer = _vb;
	m_IndexBuffer = _ib;
	m_Stride = sizeof(Vertex::VertexColor);
	m_IndexCount = (UINT)_indices.size();

}

void Grid::HelperRender(
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext, 
	std::shared_ptr<VertexShader> pVertexShader, 
	std::shared_ptr<PixelShader> pPixelShader, 
	const DirectX::XMMATRIX& world, 
	const DirectX::XMMATRIX& view, 
	const DirectX::XMMATRIX& proj)
{
	DirectX::XMMATRIX _world = DirectX::XMMatrixIdentity();

	ConstantBufferManager::GetVertexBuffer()->data.wvpMatrix = _world * view * proj;
	ConstantBufferManager::GetVertexBuffer()->data.worldMatrix = _world;
	ConstantBufferManager::GetVertexBuffer()->ApplyChanges();
	pDeviceContext->VSSetConstantBuffers(0, 1, ConstantBufferManager::GetVertexBuffer()->GetAddressOf());

	pDeviceContext->IASetInputLayout(pVertexShader->inputLayout.Get());
	pDeviceContext->VSSetShader(pVertexShader->GetVertexShader(), NULL, 0);
	pDeviceContext->PSSetShader(pPixelShader->GetPixelShader(), NULL, 0);

	const UINT _stride = sizeof(Vertex::VertexColor);
	const UINT _offset = 0;


	pDeviceContext->IASetVertexBuffers(0, 1, m_VertexBuffer.GetAddressOf(), &_stride, &_offset);
	pDeviceContext->IASetIndexBuffer(m_IndexBuffer.Get(), DXGI_FORMAT::DXGI_FORMAT_R32_UINT, 0);

	pDeviceContext->DrawIndexed(m_IndexCount, 0, 0);
}

void Grid::HelperRender(
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContex, 
	std::shared_ptr<VertexShader> pVertexShader, 
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pPixelShader, 
	const DirectX::XMMATRIX& world, 
	const DirectX::XMMATRIX& view, 
	const DirectX::XMMATRIX& proj)
{

	DirectX::XMMATRIX _world = DirectX::XMMatrixIdentity();

	ConstantBufferManager::GetVertexBuffer()->data.wvpMatrix = _world * view * proj;
	ConstantBufferManager::GetVertexBuffer()->data.worldMatrix = _world;
	ConstantBufferManager::GetVertexBuffer()->ApplyChanges();
	pDeviceContex->VSSetConstantBuffers(0, 1, ConstantBufferManager::GetVertexBuffer()->GetAddressOf());

	pDeviceContex->IASetInputLayout(pVertexShader->inputLayout.Get());
	pDeviceContex->VSSetShader(pVertexShader->GetVertexShader(), NULL, 0);
	pDeviceContex->PSSetShader(pPixelShader.Get(), NULL, 0);

	const UINT _stride = sizeof(Vertex::VertexColor);
	const UINT _offset = 0;


	pDeviceContex->IASetVertexBuffers(0, 1, m_VertexBuffer.GetAddressOf(), &_stride, &_offset);
	pDeviceContex->IASetIndexBuffer(m_IndexBuffer.Get(), DXGI_FORMAT::DXGI_FORMAT_R32_UINT, 0);

	pDeviceContex->DrawIndexed(m_IndexCount, 0, 0);
}

