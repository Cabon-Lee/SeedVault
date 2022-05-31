#include "Gizmo.h"

#include "VertexHelper.h"
#include "Shaders.h"

#include "RasterizerState.h"
#include "ConstantBufferManager.h"
#include "SimpleMath.h"

Gizmo::Gizmo()
{

}

Gizmo::~Gizmo()
{

}

void Gizmo::BuildHelper(ID3D11Device* pDevice)
{
	// 스태틱으로 올려둔 버퍼들이 이미 데이터를 갖고 있다면 그냥 리턴
	if (m_VertexBuffer != nullptr && m_IndexBuffer != nullptr)
	{
		return;
	}

	Vertex::VertexColor _vertices[] =
	{
	{ XMFLOAT3(+0.20f, +0.20f, -0.20f),	XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f)},
	{ XMFLOAT3(+0.20f, +0.20f, +0.20f),	XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f)},
	{ XMFLOAT3(+6.50f, +0.20f, -0.20f),	XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f)},
	{ XMFLOAT3(+6.50f, +0.20f, +0.20f),	XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f)},
	{ XMFLOAT3(+0.20f, -0.20f, -0.20f),	XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f)},
	{ XMFLOAT3(+0.20f, -0.20f, +0.20f),	XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f)},
	{ XMFLOAT3(+6.50f, -0.20f, -0.20f),	XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f)},
	{ XMFLOAT3(+6.50f, -0.20f, +0.20f),	XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f)},
	{ XMFLOAT3(+6.50f, +0.35f, -0.35f),	XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f)},
	{ XMFLOAT3(+6.50f, +0.35f, +0.35f),	XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f)},
	{ XMFLOAT3(+6.50f, -0.35f, -0.35f),	XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f)},
	{ XMFLOAT3(+6.50f, -0.35f, +0.35f),	XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f)},
	{ XMFLOAT3(+7.50f, +0.06f, -0.06f),	XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f)},
	{ XMFLOAT3(+7.50f, +0.06f, +0.06f),	XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f)},
	{ XMFLOAT3(+7.50f, -0.06f, -0.06f),	XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f)},
	{ XMFLOAT3(+7.50f, -0.06f, +0.06f),	XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f)},
	{ XMFLOAT3(-0.20f, +0.20f, -0.20f),	XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f)},
	{ XMFLOAT3(-0.20f, +0.20f, +0.20f),	XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f)},
	{ XMFLOAT3(-0.20f, +6.50f, -0.20f),	XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f)},
	{ XMFLOAT3(-0.20f, +6.50f, +0.20f),	XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f)},
	{ XMFLOAT3(+0.20f, +0.20f, -0.20f),	XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f)},
	{ XMFLOAT3(+0.20f, +0.20f, +0.20f),	XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f)},
	{ XMFLOAT3(+0.20f, +6.50f, -0.20f),	XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f)},
	{ XMFLOAT3(+0.20f, +6.50f, +0.20f),	XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f)},
	{ XMFLOAT3(-0.35f, +6.50f, -0.35f),	XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f)},
	{ XMFLOAT3(-0.35f, +6.50f, +0.35f),	XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f)},
	{ XMFLOAT3(+0.35f, +6.50f, -0.35f),	XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f)},
	{ XMFLOAT3(+0.35f, +6.50f, +0.35f),	XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f)},
	{ XMFLOAT3(-0.06f, +7.50f, -0.06f),	XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f)},
	{ XMFLOAT3(-0.06f, +7.50f, +0.06f),	XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f)},
	{ XMFLOAT3(+0.06f, +7.50f, -0.06f),	XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f)},
	{ XMFLOAT3(+0.06f, +7.50f, +0.06f),	XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f)},
	{ XMFLOAT3(+0.20f, +0.20f, +0.20f),		XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f)},
	{ XMFLOAT3(-0.20f, +0.20f, +0.20f),	XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f)},
	{ XMFLOAT3(+0.20f, +0.20f, +6.50f),	XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f)},
	{ XMFLOAT3(-0.20f, +0.20f, +6.50f),	XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f)},
	{ XMFLOAT3(+0.20f, -0.20f, +0.20f),	XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f)},
	{ XMFLOAT3(-0.20f, -0.20f, +0.20f),	XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f)},
	{ XMFLOAT3(+0.20f, -0.20f, +6.50f),	XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f)},
	{ XMFLOAT3(-0.20f, -0.20f, +6.50f),	XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f)},
	{ XMFLOAT3(+0.35f, +0.35f, +6.50f),	XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f)},
	{ XMFLOAT3(-0.35f, +0.35f, +6.50f),	XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f)},
	{ XMFLOAT3(+0.35f, -0.35f, +6.50f),	XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f)},
	{ XMFLOAT3(-0.35f, -0.35f, +6.50f),	XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f)},
	{ XMFLOAT3(+0.06f, +0.06f, +7.50f),	XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f)},
	{ XMFLOAT3(-0.06f, +0.06f, +7.50f),	XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f)},
	{ XMFLOAT3(+0.06f, -0.06f, +7.50f),	XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f)},
	{ XMFLOAT3(-0.06f, -0.06f, +7.50f),	XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f)},

	};

	UINT _indices[] =
	{
		0, 1, 3,
		3, 2, 0,
		7, 6, 10,
		10, 11, 7,
		6, 7, 5,
		5, 4, 6,
		4, 5, 1,
		1, 0, 4,
		2, 6, 4,
		4, 0, 2,
		7, 3, 1,
		1, 5, 7,
		9, 11, 15,
		15, 13, 9,
		6, 2, 8,
		8, 10, 6,
		3, 7, 11,
		11, 9, 3,
		2, 3, 9,
		9, 8, 2,
		12, 13, 15,
		15, 14, 12,
		8, 9, 13,
		13, 12, 8,
		11, 10, 14,
		14, 15, 11,
		10, 8, 12,
		12, 14, 10,
		16, 17, 19,
		19, 18, 16,
		23, 22, 26,
		26, 27, 23,
		22, 23, 21,
		21, 20, 22,
		20, 21, 17,
		17, 16, 20,
		18, 22, 20,
		20, 16, 18,
		23, 19, 17,
		17, 21, 23,
		25, 27, 31,
		31, 29, 25,
		22, 18, 24,
		24, 26, 22,
		19, 23, 27,
		27, 25, 19,
		18, 19, 25,
		25, 24, 18,
		28, 29, 31,
		31, 30, 28,
		24, 25, 29,
		29, 28, 24,
		27, 26, 30,
		30, 31, 27,
		26, 24, 28,
		28, 30, 26,
		32, 33, 35,
		35, 34, 32,
		39, 38, 42,
		42, 43, 39,
		38, 39, 37,
		37, 36, 38,
		36, 37, 33,
		33, 32, 36,
		34, 38, 36,
		36, 32, 34,
		39, 35, 33,
		33, 37, 39,
		41, 43, 47,
		47, 45, 41,
		38, 34, 40,
		40, 42, 38,
		35, 39, 43,
		43, 41, 35,
		34, 35, 41,
		41, 40, 34,
		44, 45, 47,
		47, 46, 44,
		40, 41, 45,
		45, 44, 40,
		43, 42, 46,
		46, 47, 43,
		42, 40, 44,
		44, 46, 42,
	};

	ID3D11Buffer* _vb;
	ID3D11Buffer* _ib;

	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(Vertex::VertexColor) * 48;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = _vertices;
	pDevice->CreateBuffer(&vbd, &vinitData, &_vb);

	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(UINT) * 252;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	ibd.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA iinitData;
	iinitData.pSysMem = _indices;
	pDevice->CreateBuffer(&ibd, &iinitData, &_ib);


	m_VertexBuffer = _vb;
	m_IndexBuffer = _ib;
	m_Stride = sizeof(Vertex::VertexColor);
	m_IndexCount = 252;
}

void Gizmo::HelperRender(
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContex, 
	std::shared_ptr<VertexShader> pVertexShader, 
	std::shared_ptr<PixelShader> pPixelShader, 
	const DirectX::XMMATRIX& worldTM,
	const DirectX::XMMATRIX& viewTM,
	const DirectX::XMMATRIX& projTM)
{

}

void Gizmo::HelperRender(Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContex, std::shared_ptr<VertexShader> pVertexShader, Microsoft::WRL::ComPtr<ID3D11PixelShader> pPixelShader, 
	const DirectX::XMMATRIX& worldTM, const DirectX::XMMATRIX& viewTM, const DirectX::XMMATRIX& projTM)
{

}

void Gizmo::HelperRender(Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContex, std::shared_ptr<VertexShader> pVertexShader, std::shared_ptr<PixelShader> pPixelShader, const DirectX::XMFLOAT3& pos, const DirectX::XMMATRIX& world, const DirectX::XMMATRIX& view, const DirectX::XMMATRIX& proj)
{
	pDeviceContex->OMSetDepthStencilState(RasterizerState::GetDepthDisableStencilState(), 1);
	pDeviceContex->RSSetState(RasterizerState::GetSolidNormal());

	DirectX::SimpleMath::Matrix _newMat(world);
	DirectX::SimpleMath::Vector3 _nowPos = _newMat.Translation();
	DirectX::SimpleMath::Vector3 _nowEyePos;

	float _dis = DirectX::SimpleMath::Vector3::Distance(pos, _nowPos);
	_dis *= 0.01f;	// 크기 보정(이거 안하면 너무 큼)


	ConstantBufferManager::GetVertexBuffer()->data.wvpMatrix = DirectX::SimpleMath::Matrix::CreateScale(_dis) * world * view * proj;
	ConstantBufferManager::GetVertexBuffer()->data.worldMatrix = world;
	ConstantBufferManager::GetVertexBuffer()->ApplyChanges();

	pDeviceContex->VSSetConstantBuffers(0, 1, ConstantBufferManager::GetVertexBuffer()->GetAddressOf());

	pDeviceContex->IASetInputLayout(pVertexShader->inputLayout.Get());
	pDeviceContex->VSSetShader(pVertexShader->GetVertexShader(), NULL, 0);
	pDeviceContex->PSSetShader(pPixelShader->GetPixelShader(), NULL, 0);

	UINT _stride = Gizmo::m_Stride;
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
