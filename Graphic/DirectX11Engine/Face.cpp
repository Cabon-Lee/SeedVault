#include "Face.h"

#include "Shaders.h"
#include "RasterizerState.h"
#include "ConstantBufferManager.h"
#include "SimpleMath.h"

#include "COMException.h"
#include "ErrorLogger.h"

DynamicFace::DynamicFace()
{
	m_Vertices_Arr[0] = DirectX::SimpleMath::Vector3(1.0f, 0.0f, 1.0f);
	m_Vertices_Arr[1] = DirectX::SimpleMath::Vector3(-1.0f, 0.0f, 1.0f);
	m_Vertices_Arr[2] = DirectX::SimpleMath::Vector3(1.0f, 0.0f, -1.0f);
	m_Normal = DirectX::SimpleMath::Vector3(0.0f, 1.0f, 0.0f);
}

DynamicFace::DynamicFace(const DirectX::SimpleMath::Vector3& v1, const DirectX::SimpleMath::Vector3& v2, const DirectX::SimpleMath::Vector3& v3, const DirectX::SimpleMath::Vector3& normal)
{
	m_Vertices_Arr[0] = v1;
	m_Vertices_Arr[1] = v2;
	m_Vertices_Arr[2] = v3;
	m_Normal = DirectX::SimpleMath::Vector3(0.0f, 1.0f, 0.0f);
}


DynamicFace::~DynamicFace()
{

}

void DynamicFace::BuildHelper(ID3D11Device* pDevice)
{
	// 삼각형을 컬러로 표현하기 위해서 r값에만 변화를 주었다

	m_Vertices =
	{
		Vertex::VertexColorNormal(m_Vertices_Arr[0].x, m_Vertices_Arr[0].y, m_Vertices_Arr[0].z, m_Normal.x, m_Normal.y, m_Normal.z,
		0.3f, 1.0f, 0.3f, 0.8f),
		Vertex::VertexColorNormal(m_Vertices_Arr[1].x, m_Vertices_Arr[1].y, m_Vertices_Arr[1].z, m_Normal.x, m_Normal.y, m_Normal.z,
		0.3f, 1.0f, 0.3f, 0.8f),
		Vertex::VertexColorNormal(m_Vertices_Arr[2].x, m_Vertices_Arr[2].y, m_Vertices_Arr[2].z, m_Normal.x, m_Normal.y, m_Normal.z,
		0.3f, 1.0f, 0.3f, 0.8f),
	};

	UINT _indices[] =
	{
		0, 1, 2
	};

	ID3D11Buffer* _vb = nullptr;
	ID3D11Buffer* _ib = nullptr;

	HRESULT _hr;

	try
	{
		D3D11_BUFFER_DESC vbd;
		vbd.Usage = D3D11_USAGE_DYNAMIC;
		vbd.ByteWidth = sizeof(Vertex::VertexColorNormal) * (UINT)m_Vertices.size();
		vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		vbd.MiscFlags = 0;
		vbd.StructureByteStride = 0;

		D3D11_SUBRESOURCE_DATA vinitData;
		vinitData.pSysMem = m_Vertices.data();
		vinitData.SysMemPitch = 0;
		vinitData.SysMemSlicePitch = 0;
		_hr = pDevice->CreateBuffer(&vbd, &vinitData, &_vb);

		COM_ERROR_IF_FAILED(_hr, "DynamicFace CreateBuffer-Vertextbuffer Fail");

		D3D11_BUFFER_DESC ibd;
		ibd.Usage = D3D11_USAGE_IMMUTABLE;
		ibd.ByteWidth = sizeof(UINT) * 3;
		ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		ibd.CPUAccessFlags = 0;
		ibd.MiscFlags = 0;
		ibd.StructureByteStride = 0;

		D3D11_SUBRESOURCE_DATA iinitData;
		iinitData.pSysMem = _indices;
		iinitData.SysMemPitch = 0;
		iinitData.SysMemSlicePitch = 0;
		_hr = pDevice->CreateBuffer(&ibd, &iinitData, &_ib);

		COM_ERROR_IF_FAILED(_hr, "DynamicFace CreateBuffer-Indexbuffer Fail");
	}
	catch (COMException& e)
	{
		ErrorLogger::Log(e);

	}

	m_VertexBuffer = _vb;
	m_IndexBuffer = _ib;
	m_Stride = sizeof(Vertex::VertexColorNormal);
	m_IndexCount = 3;
}

void DynamicFace::HelperRender(Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContex, std::shared_ptr<VertexShader> pVertexShader, std::shared_ptr<PixelShader> pPixelShader, const DirectX::XMMATRIX& world, const DirectX::XMMATRIX& view, const DirectX::XMMATRIX& proj)
{

}

void DynamicFace::DynamicRender(
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContex,
	std::shared_ptr<VertexShader> pVertexShader,
	std::shared_ptr<PixelShader> pPixelShader,
	bool isWire,
	const DirectX::SimpleMath::Vector3& v1,
	const DirectX::SimpleMath::Vector3& v2,
	const DirectX::SimpleMath::Vector3& v3,
	const DirectX::SimpleMath::Vector3& normal,
	const DirectX::XMMATRIX& world,
	const DirectX::XMMATRIX& view,
	const DirectX::XMMATRIX& proj,
	const unsigned int color)
{
	m_Vertices_Arr[0] = v1;
	m_Vertices_Arr[1] = v2;
	m_Vertices_Arr[2] = v3;
	m_Normal = normal;

	switch (color)
	{
	case WHITE: m_Color = DirectX::Colors::White;  break;
	case RED: m_Color = DirectX::Colors::Red;  break;
	case GREEN: m_Color = DirectX::Colors::Green;  break;
	case BLUE: m_Color = DirectX::Colors::Blue;  break;
	case DARKGRAY: m_Color = DirectX::Colors::DarkGray;  break;
	default: m_Color = DirectX::Colors::Green;  break;
	}

	RebuildBuffer(pDeviceContex);

	pDeviceContex->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	if (isWire == true)
	{
		pDeviceContex->RSSetState(RasterizerState::GetWireFrameRS());
	}
	else
	{
		pDeviceContex->RSSetState(RasterizerState::GetSolidNormal());
	}

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

void FaceXMFLOAT3(XMFLOAT3& target, float x, float y, float z)
{
	target.x = x; target.y = y; target.z = z;
}

void FaceXMFLOAT4(XMFLOAT4& target, float x, float y, float z, float w)
{
	target.x = x; target.y = y; target.z = z; target.w = w;
}

void DynamicFace::RebuildBuffer(Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContex)
{

	FaceXMFLOAT3(m_Vertices[0].pos, m_Vertices_Arr[0].x, m_Vertices_Arr[0].y, m_Vertices_Arr[0].z);
	FaceXMFLOAT3(m_Vertices[1].pos, m_Vertices_Arr[1].x, m_Vertices_Arr[1].y, m_Vertices_Arr[1].z);
	FaceXMFLOAT3(m_Vertices[2].pos, m_Vertices_Arr[2].x, m_Vertices_Arr[2].y, m_Vertices_Arr[2].z);
	FaceXMFLOAT3(m_Vertices[0].normal, m_Normal.x, m_Normal.y, m_Normal.z);
	FaceXMFLOAT3(m_Vertices[1].normal, m_Normal.x, m_Normal.y, m_Normal.z);
	FaceXMFLOAT3(m_Vertices[2].normal, m_Normal.x, m_Normal.y, m_Normal.z);
	FaceXMFLOAT4(m_Vertices[0].rgba, m_Color.x, m_Color.y, m_Color.z, m_Color.w);
	FaceXMFLOAT4(m_Vertices[1].rgba, m_Color.x, m_Color.y, m_Color.z, m_Color.w);
	FaceXMFLOAT4(m_Vertices[2].rgba, m_Color.x, m_Color.y, m_Color.z, m_Color.w);

	try
	{
		HRESULT _hr;

		D3D11_MAPPED_SUBRESOURCE _mappedResource;
		Vertex::VertexColorNormal* _verticesPtr;

		_hr = pDeviceContex->Map(m_VertexBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &_mappedResource);

		COM_ERROR_IF_FAILED(_hr, "DynamicBox RebuildBuffer Map Failed");

		_verticesPtr = static_cast<Vertex::VertexColorNormal*>(_mappedResource.pData);

		memcpy(_verticesPtr, static_cast<void*>(m_Vertices.data()), (sizeof(Vertex::VertexColorNormal)) * m_Vertices.size());

		pDeviceContex->Unmap(m_VertexBuffer.Get(), 0);

	}
	catch (COMException& e)
	{
		ErrorLogger::Log(e);
	}
}
