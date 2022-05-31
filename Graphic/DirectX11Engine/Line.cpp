#include "Line.h"

#include "VertexHelper.h"
#include "Shaders.h"

#include "RasterizerState.h"
#include "ConstantBufferManager.h"
#include "SimpleMath.h"

#include "COMException.h"
#include "ErrorLogger.h"

DynamicLine::DynamicLine()
{

}

void DynamicLine::BuildHelper(ID3D11Device* pDevice)
{
	m_Vertices =
	{
		Vertex::VertexColor(m_Origin.x, m_Origin.y, m_Origin.z, 0.0f, 1.0f, 0.0f, 1.0f),
		Vertex::VertexColor(m_Target.x, m_Target.y, m_Target.z, 0.0f, 1.0f, 0.0f, 1.0f),
	};

	UINT _indices[] =
	{
		0, 1,
	};

	ID3D11Buffer* _vb = nullptr;
	ID3D11Buffer* _ib = nullptr;

	HRESULT _hr;

	try
	{
		D3D11_BUFFER_DESC vbd;
		vbd.Usage = D3D11_USAGE_DYNAMIC;
		vbd.ByteWidth = sizeof(Vertex::VertexColor) * (UINT)m_Vertices.size();
		vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		vbd.MiscFlags = 0;
		vbd.StructureByteStride = 0;

		D3D11_SUBRESOURCE_DATA vinitData;
		vinitData.pSysMem = m_Vertices.data();
		vinitData.SysMemPitch = 0;
		vinitData.SysMemSlicePitch = 0;
		_hr = pDevice->CreateBuffer(&vbd, &vinitData, &_vb);

		COM_ERROR_IF_FAILED(_hr, "DynamicBox CreateBuffer-Vertextbuffer Fail");

		D3D11_BUFFER_DESC ibd;
		ibd.Usage = D3D11_USAGE_IMMUTABLE;
		ibd.ByteWidth = sizeof(UINT) * 2;
		ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		ibd.CPUAccessFlags = 0;
		ibd.MiscFlags = 0;
		ibd.StructureByteStride = 0;

		D3D11_SUBRESOURCE_DATA iinitData;
		iinitData.pSysMem = _indices;
		iinitData.SysMemPitch = 0;
		iinitData.SysMemSlicePitch = 0;
		_hr = pDevice->CreateBuffer(&ibd, &iinitData, &_ib);

		COM_ERROR_IF_FAILED(_hr, "DynamicBox CreateBuffer-Indexbuffer Fail");
	}
	catch (COMException& e)
	{
		ErrorLogger::Log(e);

	}

	m_VertexBuffer = _vb;
	m_IndexBuffer = _ib;
	m_Stride = sizeof(Vertex::VertexColor);
	m_IndexCount = 2;
}

void DynamicLine::HelperRender(Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContex, std::shared_ptr<VertexShader> pVertexShader, std::shared_ptr<PixelShader> pPixelShader, const DirectX::XMMATRIX& world, const DirectX::XMMATRIX& view, const DirectX::XMMATRIX& proj)
{

}

void DynamicLine::DynamicRender(
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContex,
	std::shared_ptr<VertexShader> pVertexShader,
	std::shared_ptr<PixelShader> pPixelShader,
	const DirectX::SimpleMath::Vector3& origin,
	const DirectX::SimpleMath::Vector3& target,
	const DirectX::XMMATRIX& world,
	const DirectX::XMMATRIX& view,
	const DirectX::XMMATRIX& proj)
{
	if (m_Origin != origin || m_Target != target)
	{
		m_Origin = origin;
		m_Target = target;
		
		RebuildBuffer(pDeviceContex);
	}

	pDeviceContex->OMSetDepthStencilState(RasterizerState::GetDepthDisableStencilState(), 1);
	pDeviceContex->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
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

void DynamicLine::RebuildBuffer(Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContex)
{
	m_Vertices[0].pos = m_Origin;
	m_Vertices[1].pos = m_Target;
	m_Vertices[0].rgba = { 0.0f, 0.1f, 0.0f, 1.0f };
	m_Vertices[1].rgba = { 0.0f, 0.1f, 0.0f, 1.0f };

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
