#include "SkyBoxSphere.h"


#include "VertexHelper.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"

#include "Shaders.h"

#include "ConstantBufferManager.h"
#include "RasterizerState.h"

#include "NodeData.h"

SkyBoxSphere::SkyBoxSphere()
	: m_SkyBoxTextureIndex(0)
{

}

SkyBoxSphere::~SkyBoxSphere()
{

}

void SkyBoxSphere::Initialize(Microsoft::WRL::ComPtr<ID3D11Device> pDevice)
{
	float _radius = 50.0f;
	UINT _sliceCount = 30;
	UINT _statckCount = 30;

	std::vector<Vertex::Vertex> _vertices;
	std::vector<UINT> _indices;

	Vertex::Vertex _topVertex(0.0f, +_radius, 0.0f, 0.0f, +1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	Vertex::Vertex _bottomVertex(0.0f, -_radius, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f);

	_vertices.push_back(_topVertex);


	float phiStep = XM_PI / _statckCount;
	float thetaStep = 2.0f * XM_PI / _sliceCount;

	// Compute vertices for each stack ring (do not count the poles as rings).
	for (UINT i = 1; i <= _statckCount - 1; ++i)
	{
		float phi = i * phiStep;

		// Vertices of ring.
		for (UINT j = 0; j <= _sliceCount; ++j)
		{
			float theta = j * thetaStep;

			Vertex::Vertex v;

			// spherical to cartesian
			v.pos.x = _radius * sinf(phi) * cosf(theta);
			v.pos.y = _radius * cosf(phi);
			v.pos.z = _radius * sinf(phi) * sinf(theta);

			// Partial derivative of P with respect to theta
			//v.TangentU.x = -_radius * sinf(phi) * sinf(theta);
			//v.TangentU.y = 0.0f;
			//v.TangentU.z = +_radius * sinf(phi) * cosf(theta);

			//XMVECTOR T = XMLoadFloat3(&v.TangentU);
			//XMStoreFloat3(&v.TangentU, XMVector3Normalize(T));

			XMVECTOR p = XMLoadFloat3(&v.pos);
			XMStoreFloat3(&v.normal, XMVector3Normalize(p));
			
			v.texCoord.x = theta / XM_2PI;
			v.texCoord.y = phi / XM_PI;

			_vertices.push_back(v);
		}
	}

	_vertices.push_back(_bottomVertex);

	//
	// Compute indices for top stack.  The top stack was written first to the vertex buffer
	// and connects the top pole to the first ring.
	//

	for (UINT i = 1; i <= _sliceCount; ++i)
	{
		_indices.push_back(0);
		_indices.push_back(i + 1);
		_indices.push_back(i);
	}

	//
	// Compute indices for inner stacks (not connected to poles).
	//

	// Offset the indices to the index of the first vertex in the first ring.
	// This is just skipping the top pole vertex.
	UINT baseIndex = 1;
	UINT ringVertexCount = _sliceCount + 1;
	for (UINT i = 0; i < _statckCount - 2; ++i)
	{
		for (UINT j = 0; j < _sliceCount; ++j)
		{
			_indices.push_back(baseIndex + i * ringVertexCount + j);
			_indices.push_back(baseIndex + i * ringVertexCount + j + 1);
			_indices.push_back(baseIndex + (i + 1) * ringVertexCount + j);

			_indices.push_back(baseIndex + (i + 1) * ringVertexCount + j);
			_indices.push_back(baseIndex + i * ringVertexCount + j + 1);
			_indices.push_back(baseIndex + (i + 1) * ringVertexCount + j + 1);
		}
	}

	//
	// Compute indices for bottom stack.  The bottom stack was written last to the vertex buffer
	// and connects the bottom pole to the bottom ring.
	//

	// South pole vertex was added last.
	UINT southPoleIndex = (UINT)_vertices.size() - 1;

	// Offset the indices to the index of the first vertex in the last ring.
	baseIndex = southPoleIndex - ringVertexCount;

	for (UINT i = 0; i < _sliceCount; ++i)
	{
		_indices.push_back(southPoleIndex);
		_indices.push_back(baseIndex + i);
		_indices.push_back(baseIndex + i + 1);
	}


	ID3D11Buffer* _vb;
	ID3D11Buffer* _ib;

	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(Vertex::Vertex) * (UINT)_vertices.size();
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
	m_Stride = sizeof(Vertex::Vertex);
	m_IndexCount = (UINT)_indices.size();

}

void SkyBoxSphere::DrawSkyBox(
	Microsoft::WRL::ComPtr <ID3D11DeviceContext> pDeviceContext,
	Microsoft::WRL::ComPtr <ID3D11ShaderResourceView> pSkyImage,
	const DirectX::XMFLOAT3& eyePos, const DirectX::XMMATRIX& view, const DirectX::XMMATRIX& proj,
	std::shared_ptr<VertexShader> pVertexShader, std::shared_ptr<PixelShader> pPixelShader)
{
	pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	pDeviceContext->RSSetState(RasterizerState::GetNoCullingRS());							// 이 두개는 문제의 소지가 있음, HLSL에서 설정해야 할 수도
	pDeviceContext->OMSetDepthStencilState(RasterizerState::GetComparisonLessEqual(), 1);	// 이 두개는 문제의 소지가 있음, HLSL에서 설정해야 할 수도

	XMMATRIX T = XMMatrixTranslation(eyePos.x, eyePos.y, eyePos.z);
	ConstantBufferManager::GetWVPMatrix()->data.wvpMatrix = XMMatrixMultiply(T, view * proj);
	ConstantBufferManager::GetWVPMatrix()->ApplyChanges();

	pDeviceContext->VSSetConstantBuffers(0, 1, ConstantBufferManager::GetWVPMatrix()->GetAddressOf());
	pDeviceContext->PSSetShaderResources(0, 1, pSkyImage.GetAddressOf());

	pDeviceContext->IASetInputLayout(pVertexShader->GetInputLayout());
	pDeviceContext->VSSetShader(pVertexShader->GetVertexShader(), NULL, 0);
	pDeviceContext->PSSetShader(pPixelShader->GetPixelShader(), NULL, 0);

	UINT offset = 0;

	pDeviceContext->IASetVertexBuffers(0, 1, m_VertexBuffer.GetAddressOf(), &m_Stride, &offset);

	// DXGI_FORMAT::DXGI_FORMAT_R16_UINT 이걸로 했을 때는 결과가 완전 이상하게 나온다
	// 두개의 차이가 뭘까...
	// 인덱스 버퍼에 사용되는 것은 16, 32 비트 두개만 사용된다
	// UINT, unsigned int = 4바이트 = 32비트이기 때문에 가능한 것으로 보인다
	// 그럼 그전에는 왜 됐느냐
	pDeviceContext->IASetIndexBuffer(m_IndexBuffer.Get(), DXGI_FORMAT::DXGI_FORMAT_R32_UINT, 0);

	pDeviceContext->DrawIndexed(m_IndexCount, 0, 0);

}

void SkyBoxSphere::DrawSkyBoxTest(
	Microsoft::WRL::ComPtr <ID3D11DeviceContext> pDeviceContext, 
	std::shared_ptr<ModelMesh> pModel, Microsoft::WRL::ComPtr <ID3D11ShaderResourceView> pSkyImage, 
	const DirectX::XMFLOAT3& eyePos, 
	const DirectX::XMMATRIX& view,
	const DirectX::XMMATRIX& proj, std::shared_ptr<VertexShader> pVertexShader, std::shared_ptr<PixelShader> pPixelShader)
{
	pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	pDeviceContext->RSSetState(RasterizerState::GetNoCullingRS());							// 이 두개는 문제의 소지가 있음, HLSL에서 설정해야 할 수도
	pDeviceContext->OMSetDepthStencilState(RasterizerState::GetComparisonLessEqual(), 1);	// 이 두개는 문제의 소지가 있음, HLSL에서 설정해야 할 수도

	XMMATRIX T = XMMatrixTranslation(eyePos.x, eyePos.y, eyePos.z);
	ConstantBufferManager::GetWVPMatrix()->data.wvpMatrix = XMMatrixMultiply(T, view * proj);
	ConstantBufferManager::GetWVPMatrix()->ApplyChanges();

	pDeviceContext->VSSetConstantBuffers(0, 1, ConstantBufferManager::GetWVPMatrix()->GetAddressOf());
	pDeviceContext->PSSetShaderResources(0, 1, pSkyImage.GetAddressOf());

	pDeviceContext->IASetInputLayout(pVertexShader->GetInputLayout());
	pDeviceContext->VSSetShader(pVertexShader->GetVertexShader(), NULL, 0);
	pDeviceContext->PSSetShader(pPixelShader->GetPixelShader(), NULL, 0);

	UINT offset = 0;

	for(auto& _nowNode : *pModel->m_pNodeData_V)
	{
		pDeviceContext->IASetVertexBuffers(0, 1, _nowNode->m_VertexBuffer.GetAddressOf(), &_nowNode->m_Stride, &offset);

		// DXGI_FORMAT::DXGI_FORMAT_R16_UINT 이걸로 했을 때는 결과가 완전 이상하게 나온다
		// 두개의 차이가 뭘까...
		// 인덱스 버퍼에 사용되는 것은 16, 32 비트 두개만 사용된다
		// UINT, unsigned int = 4바이트 = 32비트이기 때문에 가능한 것으로 보인다
		// 그럼 그전에는 왜 됐느냐
		pDeviceContext->IASetIndexBuffer(_nowNode->m_IndexBuffer.Get(), DXGI_FORMAT::DXGI_FORMAT_R32_UINT, 0);

		pDeviceContext->DrawIndexed(_nowNode->m_IndexCount, 0, 0);
	
	}

}
