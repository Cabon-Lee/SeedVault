#include "VisualDebugger.h"

#include "VertexHelper.h"
//#include "DebugShader.h"
#include "Sphere.h"
#include "Box.h"
#include "Face.h"
#include "Grid.h"
#include "Gizmo.h"
#include "Line.h"
#include "DirectionalLightShape.h"
#include "PointLightShape.h"
#include "SpotLightShape.h"

#include "ConstantBufferManager.h"

#include "Shaders.h"

VisualDebugger::VisualDebugger()
{

}

VisualDebugger::~VisualDebugger()
{

}

void VisualDebugger::Initilze(
	Microsoft::WRL::ComPtr<ID3D11Device> pDevice, 
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContex)
{
	m_pDevice = pDevice;
	m_pDeviceContext = pDeviceContex;

	// 아직 못씀, 경로나 뭔가가 문제
	//m_pDebugShader = std::make_shared<DebugShader>();
	//m_pDebugShader->Initialize(pDevice);

	m_DebugBox = std::make_shared<DynamicBox>();
	m_DebugBox->BuildHelper(pDevice.Get());

	m_DebugSphere = std::make_shared<DynamicSphere>();
	m_DebugSphere->BuildHelper(pDevice.Get());

	m_DebugFace = std::make_shared<DynamicFace>();
	m_DebugFace->BuildHelper(pDevice.Get());

	m_DebugLine = std::make_shared<DynamicLine>();
	m_DebugLine->BuildHelper(pDevice.Get());

	m_pGrid = std::make_shared<Grid>();
	m_pGrid->BuildHelper(m_pDevice.Get());
	m_pGizmo = std::make_shared<Gizmo>();
	m_pGizmo->BuildHelper(m_pDevice.Get());

	m_pDirectionalLight = std::make_shared<DirectionalLightShape>();
	m_pDirectionalLight->BuildHelper(m_pDevice.Get());

	m_pPointLight = std::make_shared<PointLightShape>();
	m_pPointLight->BuildHelper(m_pDevice.Get());

	m_pSpotLight = std::make_shared<SpotLightShape>();
	m_pSpotLight->BuildHelper(pDevice.Get());

	m_OrginSize = DirectX::XMMatrixScaling(1.0f, 1.0f, 1.0f);
}

void VisualDebugger::DrawLine(
	const DirectX::SimpleMath::Vector3& origin, 
	const DirectX::SimpleMath::Vector3& dir, 
	float distance, 
	const DirectX::SimpleMath::Color& color, 
	const DirectX::XMMATRIX& viewTM, 
	const DirectX::XMMATRIX& projTM)
{
	Microsoft::WRL::ComPtr<ID3D11Buffer> _vb;
	Microsoft::WRL::ComPtr<ID3D11Buffer> _ib;

	UINT _indexCount = 2;


	std::vector<Vertex::VertexColor> _vertices;
	_vertices.resize(_indexCount);


	DirectX::SimpleMath::Vector3 _nowEnd;
	_nowEnd = dir * distance;
	
	// 시작점
	_vertices[0] = { _nowEnd, color };
	_vertices[1] = { origin, color };


	DirectX::SimpleMath::Matrix _nowWorld = DirectX::SimpleMath::Matrix::Identity * DirectX::SimpleMath::Matrix::CreateTranslation(origin);


	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(Vertex::VertexColor) * _indexCount;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = _vertices.data();

	m_pDevice->CreateBuffer(&vbd, &vinitData, &_vb);

	std::vector<UINT> _indices;
	_indices.resize(_indexCount);

	_indices = { 1, 0 };

	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(UINT) * _indexCount;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	ibd.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA iinitData;
	iinitData.pSysMem = _indices.data();
	m_pDevice->CreateBuffer(&ibd, &iinitData, &_ib);


	m_LineDebugger_V.push_back(std::make_shared<LineDebugger>(_vb, _ib, _nowWorld));
	
	_vertices.clear();
	_indices.clear();
}

void VisualDebugger::DrawLine(
	std::shared_ptr<VertexShader> pVertexShader,
	std::shared_ptr<PixelShader> pPixelShader,
	const DirectX::XMMATRIX& viewTM,
	const DirectX::XMMATRIX& projTM)
{

	for(auto& _nowLine : m_LineDebugger_V)
	{
		m_pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

		ConstantBufferManager::GetVertexBuffer()->data.wvpMatrix = _nowLine->worldTM * viewTM * projTM;
		ConstantBufferManager::GetVertexBuffer()->data.worldMatrix = _nowLine->worldTM;
		ConstantBufferManager::GetVertexBuffer()->ApplyChanges();

		m_pDeviceContext->VSSetConstantBuffers(0, 1, ConstantBufferManager::GetVertexBuffer()->GetAddressOf());

		m_pDeviceContext->IASetInputLayout(pVertexShader->GetInputLayout());
		m_pDeviceContext->VSSetShader(pVertexShader->GetVertexShader(), NULL, 0);
		m_pDeviceContext->PSSetShader(pPixelShader->GetPixelShader(), NULL, 0);

		const UINT _stride = sizeof(Vertex::VertexColor);
		const UINT _offset = 0;

		m_pDeviceContext->IASetVertexBuffers(0, 1, _nowLine->_vb.GetAddressOf(), &_stride, &_offset);
		m_pDeviceContext->IASetIndexBuffer(_nowLine->_ib.Get(), DXGI_FORMAT::DXGI_FORMAT_R32_UINT, 0);

		m_pDeviceContext->DrawIndexed(2, 0, 0);
	}	

	m_LineDebugger_V.clear();
}

void VisualDebugger::DrawSqure(const DirectX::XMMATRIX& worldTM, DirectX::SimpleMath::Vector3 m_Corners[])
{
	DrawLine(worldTM, m_Corners[0], m_Corners[1], 0.1f, 0.9f, 0.5f);
	DrawLine(worldTM, m_Corners[1], m_Corners[2], 0.1f, 0.9f, 0.5f);
	DrawLine(worldTM, m_Corners[2], m_Corners[3], 0.1f, 0.9f, 0.5f);
	DrawLine(worldTM, m_Corners[3], m_Corners[0], 0.1f, 0.9f, 0.5f);

	DrawLine(worldTM, m_Corners[0], m_Corners[4], 0.1f, 0.9f, 0.5f);
	DrawLine(worldTM, m_Corners[1], m_Corners[5], 0.1f, 0.9f, 0.5f);
	DrawLine(worldTM, m_Corners[2], m_Corners[6], 0.1f, 0.9f, 0.5f);
	DrawLine(worldTM, m_Corners[3], m_Corners[7], 0.1f, 0.9f, 0.5f);

	DrawLine(worldTM, m_Corners[4], m_Corners[5], 0.1f, 0.9f, 0.5f);
	DrawLine(worldTM, m_Corners[5], m_Corners[6], 0.1f, 0.9f, 0.5f);
	DrawLine(worldTM, m_Corners[6], m_Corners[7], 0.1f, 0.9f, 0.5f);
	DrawLine(worldTM, m_Corners[7], m_Corners[4], 0.1f, 0.9f, 0.5f);
}


void VisualDebugger::DrawLine(
	const DirectX::XMMATRIX& worldTM,
	const DirectX::XMFLOAT3& origin, 
	const DirectX::XMFLOAT3& target)
{
	Microsoft::WRL::ComPtr<ID3D11Buffer> _vb;
	Microsoft::WRL::ComPtr<ID3D11Buffer> _ib;

	UINT _indexCount = 2;

	std::vector<Vertex::VertexColor> _vertices;
	_vertices.resize(_indexCount);

	_vertices[0] = { origin, DirectX::SimpleMath::Color(0.0f, 1.0f, 0.0f, 1.0f) };
	_vertices[1] = { target, DirectX::SimpleMath::Color(0.0f, 1.0f, 0.0f, 1.0f) };


	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(Vertex::VertexColor) * _indexCount;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = _vertices.data();

	m_pDevice->CreateBuffer(&vbd, &vinitData, &_vb);

	std::vector<UINT> _indices;
	_indices.resize(_indexCount);

	_indices = { 1, 0 };

	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(UINT) * _indexCount;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	ibd.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA iinitData;
	iinitData.pSysMem = _indices.data();
	m_pDevice->CreateBuffer(&ibd, &iinitData, &_ib);

	//DirectX::SimpleMath::Matrix _nowWorld = DirectX::SimpleMath::Matrix::CreateTranslation(origin);
	//m_LineDebugger_V.push_back(std::make_shared<LineDebugger>(_vb, _ib, _nowWorld));
	m_LineDebugger_V.push_back(std::make_shared<LineDebugger>(_vb, _ib, worldTM));

	_vertices.clear();
	_indices.clear();
}

void VisualDebugger::DrawLine(
	const DirectX::XMMATRIX& worldTM,
	const DirectX::XMFLOAT3& origin,
	const DirectX::XMFLOAT3& target,
	float r, float g, float b)
{
	Microsoft::WRL::ComPtr<ID3D11Buffer> _vb;
	Microsoft::WRL::ComPtr<ID3D11Buffer> _ib;

	UINT _indexCount = 2;

	std::vector<Vertex::VertexColor> _vertices;
	_vertices.resize(_indexCount);

	_vertices[0] = { origin, DirectX::SimpleMath::Color(r, g, b, 1.0f) };
	_vertices[1] = { target, DirectX::SimpleMath::Color(r, g, b, 1.0f) };


	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(Vertex::VertexColor) * _indexCount;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = _vertices.data();

	m_pDevice->CreateBuffer(&vbd, &vinitData, &_vb);

	std::vector<UINT> _indices;
	_indices.resize(_indexCount);

	_indices = { 1, 0 };

	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(UINT) * _indexCount;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	ibd.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA iinitData;
	iinitData.pSysMem = _indices.data();
	m_pDevice->CreateBuffer(&ibd, &iinitData, &_ib);

	//DirectX::SimpleMath::Matrix _nowWorld = DirectX::SimpleMath::Matrix::CreateTranslation(origin);
	//m_LineDebugger_V.push_back(std::make_shared<LineDebugger>(_vb, _ib, _nowWorld));
	m_LineDebugger_V.push_back(std::make_shared<LineDebugger>(_vb, _ib, worldTM));

	_vertices.clear();
	_indices.clear();
}

std::shared_ptr<DynamicFace>& VisualDebugger::GetFaceRender()
{
	return m_DebugFace;
}

std::shared_ptr<DynamicLine>& VisualDebugger::GetLineRender()
{
	return m_DebugLine;
}

void VisualDebugger::DebuggerRender(
	std::shared_ptr<VertexShader> pVertexShader, 
	std::shared_ptr<PixelShader> pPixelShader,
	const DirectX::XMFLOAT3& pos,
	const DirectX::XMMATRIX& worldTM, 
	const DirectX::XMMATRIX& viewTM, 
	const DirectX::XMMATRIX& projTM,
	eHELPER_TYPE type, 
	const DirectX::SimpleMath::Vector3& size)
{

	switch (type)
	{
	case eHELPER_TYPE::GRID:
		m_pGrid->HelperRender(m_pDeviceContext, pVertexShader, pPixelShader,
			worldTM, viewTM, projTM);
		break;
	case eHELPER_TYPE::GIZMO:
	{
		DirectX::SimpleMath::Matrix _nowTM = worldTM;
		DirectX::SimpleMath::Vector3 _size;
		DirectX::SimpleMath::Quaternion _rot;
		DirectX::SimpleMath::Vector3 _pos;
		_nowTM.Decompose(_size, _rot, _pos);

		_nowTM = DirectX::SimpleMath::Matrix::CreateScale(1.0f) *
			DirectX::SimpleMath::Matrix::CreateFromQuaternion(_rot) *
			DirectX::SimpleMath::Matrix::CreateTranslation(_pos);

		m_pGizmo->HelperRender(m_pDeviceContext, pVertexShader, pPixelShader,
			pos, _nowTM, viewTM, projTM);
		break;
	}
	case eHELPER_TYPE::DIRECTIONAL_LIGHT:
	{
		m_pDirectionalLight->HelperRender(m_pDeviceContext, pVertexShader, pPixelShader, worldTM, viewTM, projTM);
		break;
	}
	case eHELPER_TYPE::POINT_LIGHT:
	{
		m_pPointLight->HelperRender(m_pDeviceContext, pVertexShader, pPixelShader, worldTM, viewTM, projTM);
		break;
	}
	case eHELPER_TYPE::SPOT_LIGHT:
	{
		m_pSpotLight->HelperRender(m_pDeviceContext, pVertexShader, pPixelShader, worldTM, viewTM, projTM);
		break;
	}
	case eHELPER_TYPE::BOX:
	{
		m_DebugBox->DynamicRender(m_pDeviceContext, pVertexShader, pPixelShader, size, worldTM, viewTM, projTM);
		break;
	}
	case eHELPER_TYPE::SPHERE:
	{
		m_DebugSphere->DynamicRender(m_pDeviceContext, pVertexShader, pPixelShader, size.x, worldTM, viewTM, projTM);
		break;
	}
	default:
		break;
	}
}

void VisualDebugger::DrawGrid(
	std::shared_ptr<VertexShader> pVertexShader, std::shared_ptr<PixelShader> pPixelShader,
	const DirectX::XMMATRIX& worldTM, const DirectX::XMMATRIX& viewTM, const DirectX::XMMATRIX& projTM)
{
	m_pGrid->HelperRender(
		m_pDeviceContext, pVertexShader, pPixelShader,
		worldTM, viewTM, projTM);
}