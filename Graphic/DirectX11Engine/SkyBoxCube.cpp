#include "SkyBoxCube.h"

#include "VertexHelper.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"

#include "Shaders.h"

#include "ConstantBufferManager.h"
#include "RasterizerState.h"

#include "NodeData.h"

SkyBoxCube::SkyBoxCube()
{

}

SkyBoxCube::~SkyBoxCube()
{

}

void SkyBoxCube::Initialize(Microsoft::WRL::ComPtr<ID3D11Device> pDevice)
{
	std::vector<Vertex::Vertex2D> _vertices;
	_vertices.resize(24);

	const float length = 2.0f;
	const float width = 2.0f;
	const float height = 2.0f;
	XMFLOAT3 p0 = XMFLOAT3(-length * .5f, -width * .5f, height * .5f);
	XMFLOAT3 p1 = XMFLOAT3(length * .5f, -width * .5f, height * .5f);
	XMFLOAT3 p2 = XMFLOAT3(length * .5f, -width * .5f, -height * .5f);
	XMFLOAT3 p3 = XMFLOAT3(-length * .5f, -width * .5f, -height * .5f);

	XMFLOAT3 p4 = XMFLOAT3(-length * .5f, width * .5f, height * .5f);
	XMFLOAT3 p5 = XMFLOAT3(length * .5f, width * .5f, height * .5f);
	XMFLOAT3 p6 = XMFLOAT3(length * .5f, width * .5f, -height * .5f);
	XMFLOAT3 p7 = XMFLOAT3(-length * .5f, width * .5f, -height * .5f);

	_vertices[0].pos = p0;
	_vertices[1].pos= p1;
	_vertices[2].pos= p2;
	_vertices[3].pos= p3;

	_vertices[4].pos= p7;
	_vertices[5].pos= p4;
	_vertices[6].pos= p0;
	_vertices[7].pos= p3;

	_vertices[8]. pos= p4;
	_vertices[9]. pos= p5;
	_vertices[10].pos= p1;
	_vertices[11].pos= p0;
	_vertices[12].pos= p6;
	_vertices[13].pos= p7;
	_vertices[14].pos= p3;
	_vertices[15].pos= p2;
	_vertices[16].pos= p5;
	_vertices[17].pos= p6;
	_vertices[18].pos= p2;
	_vertices[19].pos= p1;
	_vertices[20].pos= p7;
	_vertices[21].pos= p6;
	_vertices[22].pos= p5;
	_vertices[23].pos= p4;

	XMFLOAT2 _00 = XMFLOAT2(0.0f, 0.0f);
	XMFLOAT2 _10 = XMFLOAT2(1.0f, 0.0f);
	XMFLOAT2 _01 = XMFLOAT2(0.0f, 1.0f);
	XMFLOAT2 _11 = XMFLOAT2(1.0f, 1.0f);

	for (int i = 0; i < 6; ++i)
	{
		_vertices[i * 4].texCoord = _00;
		_vertices[i * 4 + 1].texCoord = _10;
		_vertices[i * 4 + 2].texCoord = _11;
		_vertices[i * 4 + 3].texCoord = _01;
	}

	std::vector<UINT> _indices;
	_indices.resize(36);
	int index = 0;
	for (int i = 0; i < 6; ++i)
	{
		_indices[index++] = i * 4;
		_indices[index++] = i * 4 + 1;
		_indices[index++] = i * 4 + 3;
		_indices[index++] = i * 4 + 1;
		_indices[index++] = i * 4 + 2;
		_indices[index++] = i * 4 + 3;
	}


	/*
	std::vector<Vertex::Vertex2D> _vertices =
	{
		{-1.0f, -1.0f, -1.0f, 0.375f, 1.0f},
		{-1.0f, +1.0f, -1.0f, 0.625f, 1.0f},
		{-1.0f, -1.0f, +1.0f, 0.375f, 0.75f},

		{-1.0f, +1.0f, +1.0f, 0.625f, 0.75f},
		{+1.0f, -1.0f, -1.0f, 0.375f, 0.25f},
		{+1.0f, +1.0f, -1.0f, 0.625f, 0.25f},	// 5

		{+1.0f, -1.0f, +1.0f, 0.375f, 0.5f},
		{+1.0f, +1.0f, +1.0f, 0.625f, 0.6f},
		{-1.0f, -1.0f, +1.0f, 0.375f, 0.75f},	// 8

		{-1.0f, +1.0f, +1.0f, 0.625f, 0.75f},
		{+1.0f, -1.0f, +1.0f, 0.375f, 0.5f},
		{+1.0f, +1.0f, +1.0f, 0.625f, 0.5f},	// 11

		{+1.0f, -1.0f, -1.0f, 0.375f, 0.25f},
		{+1.0f, +1.0f, -1.0f, 0.625f, 0.25f},
		{-1.0f, +1.0f, -1.0f, 0.625f, 0.0f},	// 14

		{-1.0f, -1.0f, -1.0f, 0.375f, 0.0f},
		{-1.0f, -1.0f, +1.0f, 0.125f, 0.5f},
		{+1.0f, -1.0f, +1.0f, 0.375f, 0.5f},	// 17

		{+1.0f, -1.0f, -1.0f, 0.375f, 0.25f},
		{-1.0f, -1.0f, -1.0f, 0.125f, 0.25f},
		{+1.0f, +1.0f, +1.0f, 0.625f, 0.5f},	// 20


		{-1.0f, +1.0f, +1.0f, 0.875f, 0.5f},
		{-1.0f, +1.0f, -1.0f, 0.875f, 0.25f},
		{+1.0f, +1.0f, -1.0f, 0.625f, 0.25f},	// 23
	};
	*/

	/*
	std::vector<Vertex::Vertex2D> _vertices =
	{

	{-1.0f, -1.0f, -1.0f, 0.0f, 1.0f},
	{-1.0f, +1.0f, -1.0f, 0.0f, 0.0f},
	{+1.0f, +1.0f, -1.0f, 1.0f, 0.0f},
	{+1.0f, -1.0f, -1.0f, 1.0f, 1.0f},

	 {-1.0f, -1.0f, +1.0f, 1.0f, 1.0f},
	 {+1.0f, -1.0f, +1.0f, 0.0f, 1.0f},
	 {+1.0f, +1.0f, +1.0f, 0.0f, 0.0f},
	 {-1.0f, +1.0f, +1.0f, 1.0f, 0.0f},


	 {-1.0f, +1.0f, -1.0f, 0.0f, 1.0f},
	 {-1.0f, +1.0f, +1.0f, 0.0f, 0.0f},
	 {+1.0f, +1.0f, +1.0f, 1.0f, 0.0f},
	 {+1.0f, +1.0f, -1.0f, 1.0f, 1.0f},


	{-1.0f, -1.0f, -1.0f, 1.0f, 1.0f},
	{+1.0f, -1.0f, -1.0f, 0.0f, 1.0f},
	{+1.0f, -1.0f, +1.0f, 0.0f, 0.0f},
	{-1.0f, -1.0f, +1.0f, 1.0f, 0.0f},

	{-1.0f, -1.0f, +1.0f, 0.0f, 1.0f},
	{-1.0f, +1.0f, +1.0f, 0.0f, 0.0f},
	{-1.0f, +1.0f, -1.0f, 1.0f, 0.0f},
	{-1.0f, -1.0f, -1.0f, 1.0f, 1.0f},

	{+1.0f, -1.0f, -1.0f, 0.0f, 1.0f},
	{+1.0f, +1.0f, -1.0f, 0.0f, 0.0f},
	{+1.0f, +1.0f, +1.0f, 1.0f, 0.0f},
	{+1.0f, -1.0f, +1.0f, 1.0f, 1.0f},
	};
	*/

	/*
	std::vector<UINT> _indices =
	{
		 0,  1,  3,
		 3,  2,  0,
		 8,  9,  7,
		 7,  6,  8,
		10, 11,  5,
		 5,  4, 10,
		12, 13, 14,
		14, 15, 12,
		16, 17, 18,
		18, 19, 16,
		20, 21, 22,
		22, 23, 20,
	};
	*/

	ID3D11Buffer* _vb;
	ID3D11Buffer* _ib;

	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(Vertex::Vertex2D) * (UINT)_vertices.size();
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

	m_VertexBuffer.Swap(_vb);
	m_IndexBuffer.Swap(_ib);
	m_Stride = sizeof(Vertex::Vertex2D);
	m_IndexCount = (UINT)_indices.size();

}

void SkyBoxCube::DrawSkyBox(Microsoft::WRL::ComPtr <ID3D11DeviceContext> pDeviceContext, Microsoft::WRL::ComPtr <ID3D11ShaderResourceView> pSkyImage, const DirectX::XMFLOAT3& eyePos, const DirectX::XMMATRIX& view, const DirectX::XMMATRIX& proj, std::shared_ptr<VertexShader> pVertexShader, std::shared_ptr<PixelShader> pPixelShader)
{
	pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//pDeviceContext->RSSetState(RasterizerState::GetNoCullingRS());							// 이 두개는 문제의 소지가 있음, HLSL에서 설정해야 할 수도
	//pDeviceContext->OMSetDepthStencilState(RasterizerState::GetComparisonLessEqual(), 1);	// 이 두개는 문제의 소지가 있음, HLSL에서 설정해야 할 수도

	ConstantBufferManager::GetWVPMatrix()->data.wvpMatrix =
		DirectX::SimpleMath::Matrix::Identity * view * proj;
	ConstantBufferManager::GetWVPMatrix()->ApplyChanges();

	pDeviceContext->VSSetConstantBuffers(0, 1, ConstantBufferManager::GetWVPMatrix()->GetAddressOf());
	pDeviceContext->PSSetShaderResources(0, 1, pSkyImage.GetAddressOf());

	pDeviceContext->PSSetSamplers(0, 1, RasterizerState::GetLinearSamplerStateAddressOf());

	pDeviceContext->IASetInputLayout(pVertexShader->GetInputLayout());
	pDeviceContext->VSSetShader(pVertexShader->GetVertexShader(), NULL, 0);
	pDeviceContext->PSSetShader(pPixelShader->GetPixelShader(), NULL, 0);

	UINT offset = 0;

	pDeviceContext->IASetVertexBuffers(0, 1, m_VertexBuffer.GetAddressOf(), &m_Stride, &offset);
	pDeviceContext->IASetIndexBuffer(m_IndexBuffer.Get(), DXGI_FORMAT::DXGI_FORMAT_R32_UINT, 0);

	pDeviceContext->DrawIndexed(m_IndexCount, 0, 0);
}

Microsoft::WRL::ComPtr<ID3D11Buffer>& SkyBoxCube::GetVertexBuffer()
{
	return m_VertexBuffer;
}

Microsoft::WRL::ComPtr<ID3D11Buffer>& SkyBoxCube::GetIndexBuffer()
{
	return m_IndexBuffer;
}

