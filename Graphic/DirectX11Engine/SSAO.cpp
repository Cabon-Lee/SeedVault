#include "SSAO.h"

#include "VertexHelper.h"
#include "ConstantBufferManager.h"
#include "Shaders.h"
#include "RasterizerState.h"

#include "ErrorLogger.h"
#include "COMException.h"

float RandF()
{
	return (float)(rand()) / (float)RAND_MAX;
}

float RandF(float a, float b)
{
	return a + RandF() * (b - a);
}

SSAO::SSAO()
{

}

SSAO::~SSAO()
{

}

void SSAO::Initialize(
	Microsoft::WRL::ComPtr<ID3D11Device> pDevice,
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext,
	std::shared_ptr<class PixelShader> pSSAOPS,
	std::shared_ptr<class VertexShader> pSSAOVS,
	std::shared_ptr<class PixelShader> pSSAOBlurPS,
	std::shared_ptr<class VertexShader> pSSAOBlurVS,
	UINT width, UINT height, float fovy, float farZ)
{
	OnSize(pDevice, width, height, fovy, farZ);

	BuildFullScreenQuad(pDevice);
	BuildOffsetVectors();
	BuildRandomVectorTexture(pDevice);

	m_pComputeSSAOPS = pSSAOPS;
	m_pComputeSSAOVS = pSSAOVS;
	m_pBlurSSAOPS = pSSAOBlurPS;
	m_pBlurSSAOVS = pSSAOBlurVS;


}

void SSAO::OnSize(Microsoft::WRL::ComPtr<ID3D11Device> pDevice, UINT width, UINT height, float fovy, float farZ)
{
	m_RenderTargetWidth = width;
	m_RenderTargetHeight = height;

	// We render to ambient map at half the resolution.
	m_AmbientMapViewport.TopLeftX = 0.0f;
	m_AmbientMapViewport.TopLeftY = 0.0f;
	m_AmbientMapViewport.Width = width / 2.0f;
	m_AmbientMapViewport.Height = height / 2.0f;
	m_AmbientMapViewport.MinDepth = 0.0f;
	m_AmbientMapViewport.MaxDepth = 1.0f;

	BuildFrustumFarCorners(fovy, farZ);
	BuildTextureViews(pDevice);
}


void SSAO::ReCalcCamera(UINT width, UINT height, float fovy, float farZ)
{
	m_RenderTargetWidth = width;
	m_RenderTargetHeight = height;

	// We render to ambient map at half the resolution.
	m_AmbientMapViewport.TopLeftX = 0.0f;
	m_AmbientMapViewport.TopLeftY = 0.0f;
	m_AmbientMapViewport.Width = width / 2.0f;
	m_AmbientMapViewport.Height = height / 2.0f;
	m_AmbientMapViewport.MinDepth = 0.0f;
	m_AmbientMapViewport.MaxDepth = 1.0f;

	BuildFrustumFarCorners(fovy, farZ);
}

void SSAO::ComputeSsao(
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext, const DirectX::XMMATRIX& proj,
	ID3D11ShaderResourceView* pSRV)
{
	ID3D11RenderTargetView* renderTargets[1] = { m_AmbientRTV0.Get() };
	pDeviceContext->OMSetRenderTargets(1, renderTargets, 0);
	pDeviceContext->ClearRenderTargetView(m_AmbientRTV0.Get(), reinterpret_cast<const float*>(&Colors::Black));
	pDeviceContext->RSSetViewports(1, &m_AmbientMapViewport);

	ID3D11Buffer* _bf[2] = { nullptr, nullptr };

	pDeviceContext->VSSetConstantBuffers(0, 2, _bf);

	for (unsigned int i = 0; i < 4; i++)
	{
		ConstantBufferManager::GetPSAmbientBuffer()->data.frustumCorner[i] = m_FrustumFarCorner[i];
	}
	ConstantBufferManager::GetPSAmbientBuffer()->ApplyChanges();
	pDeviceContext->VSSetConstantBuffers(0, 1, ConstantBufferManager::GetPSAmbientBuffer()->GetAddressOf());

	ID3D11SamplerState* _sam[2] =
	{
		RasterizerState::GetSSAOSamplerState(),
		RasterizerState::GetSSAO2SamplerState(),
	};

	pDeviceContext->PSSetSamplers(0, 2, _sam);

	static const DirectX::XMMATRIX T(
		0.5f, 0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.0f, 1.0f);
	DirectX::XMMATRIX PT = XMMatrixMultiply(proj, T);

	float _ptfloat[16] =
	{
		PT._11,PT._12,PT._13,PT._14,
		PT._21,PT._22,PT._23,PT._24,
		PT._31,PT._32,PT._33,PT._34,
		PT._41,PT._42,PT._43,PT._44,
	};

	ConstantBufferManager::SetGeneralCBuffer(pDeviceContext, CBufferType::PS, CBufferSize::SIZE_16, 0, _ptfloat);
	ConstantBufferManager::SetGeneralCBuffer(pDeviceContext, CBufferType::PS, CBufferSize::SIZE_56, 1, m_OffsetsFloat);

	ID3D11ShaderResourceView* _srv[2] = { pSRV, m_RandomVectorSRV.Get() };

	pDeviceContext->PSSetShaderResources(0, 2, _srv);

	UINT stride = sizeof(Vertex::VertexPosNormalTex);
	UINT offset = 0;

	pDeviceContext->IASetInputLayout(m_pComputeSSAOVS->GetInputLayout());
	pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	pDeviceContext->IASetVertexBuffers(0, 1, m_ScreenQuadVB.GetAddressOf(), &stride, &offset);
	pDeviceContext->IASetIndexBuffer(m_ScreenQuadIB.Get(), DXGI_FORMAT_R16_UINT, 0);

	pDeviceContext->VSSetShader(m_pComputeSSAOVS->GetVertexShader(), nullptr, 0);
	pDeviceContext->PSSetShader(m_pComputeSSAOPS->GetPixelShader(), nullptr, 0);

	pDeviceContext->DrawIndexed(6, 0, 0);

	renderTargets[0] = nullptr;
	pDeviceContext->OMSetRenderTargets(1, renderTargets, 0);

	ID3D11ShaderResourceView* _clear[2] = { nullptr, nullptr };
	pDeviceContext->PSSetShaderResources(0, 2, _clear);

	m_pTempDepthMap = pSRV;
}

Microsoft::WRL::ComPtr<ID3D11Buffer>& SSAO::GetScreenQuadVB()
{
	return m_ScreenQuadVB;
}

Microsoft::WRL::ComPtr<ID3D11Buffer>& SSAO::GetScreenQuadIB()
{
	return m_ScreenQuadIB;
}

Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& SSAO::GetRandomVectorSRV()
{
	return m_RandomVectorSRV;
}

Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& SSAO::AmbientSRV()
{
	return m_AmbientSRV0;
}

void SSAO::BlurAmbientMap(
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext,
	int blurCount)
{
	for (int i = 0; i < blurCount; ++i)
	{
		// Ping-pong the two ambient map textures as we apply
		// horizontal and vertical blur passes.
		BlurAmbientMap(pDeviceContext, m_AmbientSRV0.Get(), m_AmbientRTV1.Get(), true);
		BlurAmbientMap(pDeviceContext, m_AmbientSRV1.Get(), m_AmbientRTV0.Get(), false);
	}

	m_pTempDepthMap = nullptr;
}

void SSAO::BlurAmbientMap(
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext,
	ID3D11ShaderResourceView* inputSRV, ID3D11RenderTargetView* outputRTV, bool horzBlur)
{
	ID3D11RenderTargetView* renderTargets[1] = { outputRTV };
	pDeviceContext->OMSetRenderTargets(1, renderTargets, 0);
	pDeviceContext->ClearRenderTargetView(outputRTV, reinterpret_cast<const float*>(&Colors::Black));
	pDeviceContext->RSSetViewports(1, &m_AmbientMapViewport);

	ID3D11ShaderResourceView* _srv[2] =
	{
		m_pTempDepthMap,
		inputSRV,
	};

	pDeviceContext->PSSetShaderResources(0, 2, _srv);

	float _psCbffer[4] =
	{
		1.0f / m_AmbientMapViewport.Width,
		1.0f / m_AmbientMapViewport.Height,
		(horzBlur ? 0.0f : 1.0f),				// 현재 가로 먼저 하는가?
		0.0,
	};

	ConstantBufferManager::SetGeneralCBuffer(pDeviceContext, CBufferType::PS, CBufferSize::SIZE_4, 0, _psCbffer);

	UINT stride = sizeof(Vertex::VertexPosNormalTex);
	UINT offset = 0;

	pDeviceContext->IASetInputLayout(m_pBlurSSAOVS->GetInputLayout());
	pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	pDeviceContext->IASetVertexBuffers(0, 1, m_ScreenQuadVB.GetAddressOf(), &stride, &offset);
	pDeviceContext->IASetIndexBuffer(m_ScreenQuadIB.Get(), DXGI_FORMAT_R16_UINT, 0);

	pDeviceContext->VSSetShader(m_pBlurSSAOVS->GetVertexShader(), nullptr, 0);
	pDeviceContext->PSSetShader(m_pBlurSSAOPS->GetPixelShader(), nullptr, 0);

	pDeviceContext->DrawIndexed(6, 0, 0);

	renderTargets[0] = nullptr;
	pDeviceContext->OMSetRenderTargets(1, renderTargets, 0);

	ID3D11ShaderResourceView* _clear[2] = { nullptr, nullptr };
	pDeviceContext->PSSetShaderResources(0, 2, _clear);
}


void SSAO::BuildFrustumFarCorners(float fovy, float farZ)
{
	float aspect = (float)m_RenderTargetWidth / (float)m_RenderTargetHeight;

	float halfHeight = farZ * tanf(0.5f * fovy);
	float halfWidth = aspect * halfHeight;

	m_FrustumFarCorner[0] = DirectX::XMFLOAT4(-halfWidth, -halfHeight, farZ, 0.0f);
	m_FrustumFarCorner[1] = DirectX::XMFLOAT4(-halfWidth, +halfHeight, farZ, 0.0f);
	m_FrustumFarCorner[2] = DirectX::XMFLOAT4(+halfWidth, +halfHeight, farZ, 0.0f);
	m_FrustumFarCorner[3] = DirectX::XMFLOAT4(+halfWidth, -halfHeight, farZ, 0.0f);
}

void SSAO::BuildFullScreenQuad(Microsoft::WRL::ComPtr<ID3D11Device> pDevice)
{

	Vertex::VertexPosNormalTex _vertex[4];

	_vertex[0].pos = XMFLOAT3(-1.0f, -1.0f, 0.0f);
	_vertex[1].pos = XMFLOAT3(-1.0f, +1.0f, 0.0f);
	_vertex[2].pos = XMFLOAT3(+1.0f, +1.0f, 0.0f);
	_vertex[3].pos = XMFLOAT3(+1.0f, -1.0f, 0.0f);

	// Store far plane frustum corner indices in Normal.x slot.
	_vertex[0].normal = XMFLOAT3(0.0f, 0.0f, 0.0f);
	_vertex[1].normal = XMFLOAT3(1.0f, 0.0f, 0.0f);
	_vertex[2].normal = XMFLOAT3(2.0f, 0.0f, 0.0f);
	_vertex[3].normal = XMFLOAT3(3.0f, 0.0f, 0.0f);

	_vertex[0].tex = XMFLOAT2(0.0f, 1.0f);
	_vertex[1].tex = XMFLOAT2(0.0f, 0.0f);
	_vertex[2].tex = XMFLOAT2(1.0f, 0.0f);
	_vertex[3].tex = XMFLOAT2(1.0f, 1.0f);

	D3D11_BUFFER_DESC _vbd;
	_vbd.Usage = D3D11_USAGE_IMMUTABLE;
	_vbd.ByteWidth = sizeof(Vertex::VertexPosNormalTex) * 4;
	_vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	_vbd.CPUAccessFlags = 0;
	_vbd.MiscFlags = 0;
	_vbd.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA _vinitData;
	_vinitData.pSysMem = _vertex;

	D3D11_BUFFER_DESC _ibd;
	_ibd.Usage = D3D11_USAGE_IMMUTABLE;
	_ibd.ByteWidth = sizeof(USHORT) * 6;
	_ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	_ibd.CPUAccessFlags = 0;
	_ibd.StructureByteStride = 0;
	_ibd.MiscFlags = 0;

	USHORT _indices[6] =
	{
		0, 1, 2,
		0, 2, 3
	};

	D3D11_SUBRESOURCE_DATA _iinitData;
	_iinitData.pSysMem = _indices;

	try
	{
		HRESULT _hr;

		_hr = pDevice->CreateBuffer(&_vbd, &_vinitData, m_ScreenQuadVB.GetAddressOf());
		COM_ERROR_IF_FAILED(_hr, "CreateBuffer Fail");

		_hr = pDevice->CreateBuffer(&_ibd, &_iinitData, m_ScreenQuadIB.GetAddressOf());
		COM_ERROR_IF_FAILED(_hr, "CreateBuffer Fail");

	}
	catch (COMException& e)
	{
		ErrorLogger::Log(e);
	}

}

void SSAO::BuildTextureViews(Microsoft::WRL::ComPtr<ID3D11Device> pDevice)
{
	ReleaseTextureViews();

	D3D11_TEXTURE2D_DESC texDesc;
	texDesc.Width = m_RenderTargetWidth;
	texDesc.Height = m_RenderTargetHeight;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = 0;

	try
	{
		HRESULT _hr;
		ID3D11Texture2D* normalDepthTex = 0;
		_hr = pDevice->CreateTexture2D(&texDesc, 0, &normalDepthTex);
		COM_ERROR_IF_FAILED(_hr, "CreateTexture2D Fail");

		_hr = pDevice->CreateShaderResourceView(normalDepthTex, 0, m_NormalDepthSRV.GetAddressOf());
		COM_ERROR_IF_FAILED(_hr, "CreateShaderResourceView Fail");

		_hr = pDevice->CreateRenderTargetView(normalDepthTex, 0, m_NormalDepthRTV.GetAddressOf());
		COM_ERROR_IF_FAILED(_hr, "CreateRenderTargetView Fail");

		// view saves a reference.
		ReleaseCOM(normalDepthTex);

		// Render ambient map at half resolution.
		texDesc.Width = m_RenderTargetWidth / 2;
		texDesc.Height = m_RenderTargetHeight / 2;
		texDesc.Format = DXGI_FORMAT_R16_FLOAT;
		ID3D11Texture2D* ambientTex0 = 0;
		_hr = pDevice->CreateTexture2D(&texDesc, 0, &ambientTex0);
		COM_ERROR_IF_FAILED(_hr, "CreateTexture2D Fail");

		_hr = pDevice->CreateShaderResourceView(ambientTex0, 0, m_AmbientSRV0.GetAddressOf());
		COM_ERROR_IF_FAILED(_hr, "CreateShaderResourceView Fail");

		_hr = pDevice->CreateRenderTargetView(ambientTex0, 0, m_AmbientRTV0.GetAddressOf());
		COM_ERROR_IF_FAILED(_hr, "CreateRenderTargetView Fail");

		ID3D11Texture2D* ambientTex1 = 0;
		_hr = pDevice->CreateTexture2D(&texDesc, 0, &ambientTex1);
		COM_ERROR_IF_FAILED(_hr, "CreateTexture2D Fail");

		_hr = pDevice->CreateShaderResourceView(ambientTex1, 0, m_AmbientSRV1.GetAddressOf());
		COM_ERROR_IF_FAILED(_hr, "CreateShaderResourceView Fail");

		_hr = pDevice->CreateRenderTargetView(ambientTex1, 0, m_AmbientRTV1.GetAddressOf());
		COM_ERROR_IF_FAILED(_hr, "CreateRenderTargetView Fail");

		// view saves a reference.
		ReleaseCOM(ambientTex0);
		ReleaseCOM(ambientTex1);


	}
	catch (COMException& e)
	{
		ErrorLogger::Log(e);
	}

}

void SSAO::ReleaseTextureViews()
{
	m_NormalDepthRTV.Reset();
	m_NormalDepthSRV.Reset();

	m_AmbientRTV0.Reset();
	m_AmbientSRV0.Reset();

	m_AmbientRTV1.Reset();
	m_AmbientSRV1.Reset();
}




void SSAO::BuildRandomVectorTexture(Microsoft::WRL::ComPtr<ID3D11Device> pDevice)
{
	D3D11_TEXTURE2D_DESC _texDesc;
	_texDesc.Width = 256;
	_texDesc.Height = 256;
	_texDesc.MipLevels = 1;
	_texDesc.ArraySize = 1;
	_texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	_texDesc.SampleDesc.Count = 1;
	_texDesc.SampleDesc.Quality = 0;
	_texDesc.Usage = D3D11_USAGE_IMMUTABLE;
	_texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	_texDesc.CPUAccessFlags = 0;
	_texDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA _initData = { 0 };
	_initData.SysMemPitch = 256 * sizeof(XMCOLOR);

	XMCOLOR color[256 * 256];
	for (int i = 0; i < 256; ++i)
	{
		for (int j = 0; j < 256; ++j)
		{
			XMFLOAT3 v(RandF(), RandF(), RandF());

			color[i * 256 + j] = XMCOLOR(v.x, v.y, v.z, 0.0f);
		}
	}

	_initData.pSysMem = color;

	ID3D11Texture2D* _tex = nullptr;

	try
	{
		HRESULT _hr;

		_hr = pDevice->CreateTexture2D(&_texDesc, &_initData, &_tex);
		COM_ERROR_IF_FAILED(_hr, "CreateTexture2D Fail");

		_hr = pDevice->CreateShaderResourceView(_tex, 0, m_RandomVectorSRV.GetAddressOf());
		COM_ERROR_IF_FAILED(_hr, "CreateShaderResourceView Fail");

	}
	catch (COMException& e)
	{
		ErrorLogger::Log(e);

	}

	// view saves a reference.
	ReleaseCOM(_tex);
}

void SSAO::BuildOffsetVectors()
{
	// Start with 14 uniformly distributed vectors.  We choose the 8 corners of the cube
	// and the 6 center points along each cube face.  We always alternate the points on 
	// opposites sides of the cubes.  This way we still get the vectors spread out even
	// if we choose to use less than 14 samples.

	// 8 cube corners
	DirectX::XMFLOAT4 _offsets[14] =
	{
		DirectX::XMFLOAT4(+1.0f, +1.0f, +1.0f, 0.0f),
		DirectX::XMFLOAT4(-1.0f, -1.0f, -1.0f, 0.0f),
		DirectX::XMFLOAT4(-1.0f, +1.0f, +1.0f, 0.0f),
		DirectX::XMFLOAT4(+1.0f, -1.0f, -1.0f, 0.0f),
		DirectX::XMFLOAT4(+1.0f, +1.0f, -1.0f, 0.0f),
		DirectX::XMFLOAT4(-1.0f, -1.0f, +1.0f, 0.0f),
		DirectX::XMFLOAT4(-1.0f, +1.0f, -1.0f, 0.0f),
		DirectX::XMFLOAT4(+1.0f, -1.0f, +1.0f, 0.0f),
		DirectX::XMFLOAT4(-1.0f, 0.0f, 0.0f, 0.0f),
		DirectX::XMFLOAT4(+1.0f, 0.0f, 0.0f, 0.0f),
		DirectX::XMFLOAT4(0.0f, -1.0f, 0.0f, 0.0f),
		DirectX::XMFLOAT4(0.0f, +1.0f, 0.0f, 0.0f),
		DirectX::XMFLOAT4(0.0f, 0.0f, -1.0f, 0.0f),
		DirectX::XMFLOAT4(0.0f, 0.0f, +1.0f, 0.0f),
	};

	for (int i = 0; i < 14; ++i)
	{
		float s = RandF(0.25f, 1.0f);

		XMVECTOR v = s * XMVector4Normalize(XMLoadFloat4(&_offsets[i]));

		unsigned int _idx = i * 4;
		m_OffsetsFloat[_idx + 0] = v.vector4_f32[0];
		m_OffsetsFloat[_idx + 1] = v.vector4_f32[1];
		m_OffsetsFloat[_idx + 2] = v.vector4_f32[2];
		m_OffsetsFloat[_idx + 3] = v.vector4_f32[3];
	}

}

