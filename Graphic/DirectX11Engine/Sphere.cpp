#include "Sphere.h"

#include "VertexHelper.h"
#include "Shaders.h"

#include "RasterizerState.h"
#include "ConstantBufferManager.h"
#include "SimpleMath.h"

#include "COMException.h"
#include "ErrorLogger.h"

DynamicSphere::DynamicSphere()
{
	m_Radius = 1;
	m_StackCount = 5;
	m_SliceCount = 5;
}

DynamicSphere::DynamicSphere(float radius, unsigned int stackCount, unsigned int sliceCount)
{
	m_Radius = radius;
	m_StackCount = stackCount;
	m_SliceCount = sliceCount;
}

void DynamicSphere::BuildHelper(ID3D11Device* pDevice)
{
	float _radius = m_Radius;

	unsigned int _stackCount = m_StackCount;
	unsigned int _sliceCount = m_SliceCount;

	std::vector<Vertex::VertexColor> _vertices;

	Vertex::VertexColor _topVertex(0.0f, +_radius, 0.0f, 0.0f, 1.0f, 0.0f);
	Vertex::VertexColor _bottomVertex(0.0f, -_radius, 0.0f, 0.0f, 1.0f, 0.0f);

	_vertices.push_back(_topVertex);

	float phiStep = XM_PI / _stackCount;
	float thetaStep = 2.0f * XM_PI / _sliceCount;

	// Compute vertices for each stack ring (do not count the poles as rings).
	for (UINT i = 1; i <= _stackCount - 1; ++i)
	{
		float phi = i * phiStep;

		// Vertices of ring.
		for (UINT j = 0; j <= _stackCount; ++j)
		{
			float theta = j * thetaStep;

			Vertex::VertexColor _v;

			// spherical to cartesian
			_v.pos.x = _radius * sinf(phi) * cosf(theta);
			_v.pos.y = _radius * cosf(phi);
			_v.pos.z = _radius * sinf(phi) * sinf(theta);

			_v.rgba = { 0.0f, 1.0f, 0.0f, 1.0f };

			_vertices.push_back(_v);
		}
	}

	_vertices.push_back(_bottomVertex);

	std::vector<UINT> _indices;


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
	for (UINT i = 0; i < _stackCount - 2; ++i)
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
	UINT southPoleIndex = (UINT)_indices.size() - 1;

	// Offset the indices to the index of the first vertex in the last ring.
	baseIndex = southPoleIndex - ringVertexCount;

	for (UINT i = 0; i < _sliceCount; ++i)
	{
		_indices.push_back(southPoleIndex);
		_indices.push_back(baseIndex + i);
		_indices.push_back(baseIndex + i + 1);
	}


	ID3D11Buffer* _vb = nullptr;
	ID3D11Buffer* _ib = nullptr;

	try
	{
		HRESULT _hr;

		D3D11_BUFFER_DESC vbd;
		vbd.Usage = D3D11_USAGE_DYNAMIC;
		vbd.ByteWidth = sizeof(Vertex::VertexColor) * (UINT)_vertices.size();
		vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		vbd.MiscFlags = 0;
		vbd.StructureByteStride = 0;
		D3D11_SUBRESOURCE_DATA vinitData;
		vinitData.pSysMem = _vertices.data();
		_hr = pDevice->CreateBuffer(&vbd, &vinitData, &_vb);

		COM_ERROR_IF_FAILED(_hr, "Dynamic Sphere Create Failed");

		D3D11_BUFFER_DESC ibd;
		ibd.Usage = D3D11_USAGE_IMMUTABLE;
		ibd.ByteWidth = sizeof(UINT) * (UINT)_indices.size();
		ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		ibd.CPUAccessFlags = 0;
		ibd.MiscFlags = 0;
		ibd.StructureByteStride = 0;
		D3D11_SUBRESOURCE_DATA iinitData;
		iinitData.pSysMem = _indices.data();
		_hr = pDevice->CreateBuffer(&ibd, &iinitData, &_ib);

		COM_ERROR_IF_FAILED(_hr, "Dynamic Sphere Create Failed");
	}
	catch (COMException& e)
	{
		ErrorLogger::Log(e);
		
	}

	m_VertexBuffer = _vb;
	m_IndexBuffer = _ib;
	m_Stride = sizeof(Vertex::VertexColor);
	m_IndexCount = (UINT)_indices.size();
}

void DynamicSphere::HelperRender(Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContex, std::shared_ptr<VertexShader> pVertexShader, std::shared_ptr<PixelShader> pPixelShader, const DirectX::XMMATRIX& world, const DirectX::XMMATRIX& view, const DirectX::XMMATRIX& proj)
{

}

void DynamicSphere::DynamicRender(
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContex,
	std::shared_ptr<VertexShader> pVertexShader, 
	std::shared_ptr<PixelShader> pPixelShader, 
	const float radius, 
	const DirectX::XMMATRIX& world, 
	const DirectX::XMMATRIX& view, 
	const DirectX::XMMATRIX& proj)
{

	if (m_Radius != radius)
	{
		m_Radius = radius;
		RebuildBuffer(pDeviceContex);
	}

	pDeviceContex->OMSetDepthStencilState(RasterizerState::GetDepthStencilState(), 1);
	pDeviceContex->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	pDeviceContex->RSSetState(RasterizerState::GetWireFrameRS());

	ConstantBufferManager::GetVertexBuffer()->data.wvpMatrix = world * view * proj;
	ConstantBufferManager::GetVertexBuffer()->data.worldMatrix = world;
	ConstantBufferManager::GetVertexBuffer()->ApplyChanges();

	pDeviceContex->VSSetConstantBuffers(0, 1, ConstantBufferManager::GetVertexBuffer()->GetAddressOf());

	pDeviceContex->IASetInputLayout(pVertexShader->inputLayout.Get());
	pDeviceContex->VSSetShader(pVertexShader->GetVertexShader(), NULL, 0);
	pDeviceContex->PSSetShader(pPixelShader->GetPixelShader(), NULL, 0);

	UINT _stride = m_Stride;
	UINT offset = 0;

	pDeviceContex->IASetVertexBuffers(0, 1,
		m_VertexBuffer.GetAddressOf(),
		&_stride, &offset);

	pDeviceContex->IASetIndexBuffer(
		m_IndexBuffer.Get(),
		DXGI_FORMAT::DXGI_FORMAT_R32_UINT, 0);

	pDeviceContex->DrawIndexed(m_IndexCount, 0, 0);
	pDeviceContex->OMSetDepthStencilState(RasterizerState::GetDepthStencilState(), 1);

}

void DynamicSphere::RebuildBuffer(Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContex)
{
	m_Vertices.clear();

	float _radius = m_Radius;

	unsigned int _stackCount = m_StackCount;
	unsigned int _sliceCount = m_SliceCount;


	Vertex::VertexColor _topVertex(0.0f, +_radius, 0.0f, 0.0f, 1.0f, 0.0f);
	Vertex::VertexColor _bottomVertex(0.0f, -_radius, 0.0f, 0.0f, 1.0f, 0.0f);

	m_Vertices.push_back(_topVertex);

	float phiStep = XM_PI / _stackCount;
	float thetaStep = 2.0f * XM_PI / _sliceCount;

	// Compute vertices for each stack ring (do not count the poles as rings).
	for (UINT i = 1; i <= _stackCount - 1; ++i)
	{
		float phi = i * phiStep;

		// Vertices of ring.
		for (UINT j = 0; j <= _stackCount; ++j)
		{
			float theta = j * thetaStep;

			Vertex::VertexColor _v;

			// spherical to cartesian
			_v.pos.x = _radius * sinf(phi) * cosf(theta);
			_v.pos.y = _radius * cosf(phi);
			_v.pos.z = _radius * sinf(phi) * sinf(theta);

			_v.rgba = { 0.0f, 1.0f, 0.0f, 1.0f };

			m_Vertices.push_back(_v);
		}
	}

	m_Vertices.push_back(_bottomVertex);

	try
	{
		HRESULT _hr;

		D3D11_MAPPED_SUBRESOURCE _mappedResource;
		Vertex::VertexColor* _verticesPtr;

		_hr = pDeviceContex->Map(m_VertexBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &_mappedResource);

		COM_ERROR_IF_FAILED(_hr, "DynamicSphere RebuildBuffer Map Failed");

		_verticesPtr = static_cast<Vertex::VertexColor*>(_mappedResource.pData);

		memcpy(_verticesPtr, static_cast<void*>(m_Vertices.data()), (sizeof(Vertex::VertexColor)) * m_Vertices.size());

		pDeviceContex->Unmap(m_VertexBuffer.Get(), 0);
		
	}
	catch (COMException& e)
	{
		ErrorLogger::Log(e);
		
	}

}
