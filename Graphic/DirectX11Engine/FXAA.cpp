#include "FXAA.h"

#include "Shaders.h"
#include "ConstantBufferManager.h"

FXAA::FXAA()
{

}

FXAA::~FXAA()
{

}

void FXAA::ApplyFactor(const IRenderOption& option)
{

}

void FXAA::Initialize(Microsoft::WRL::ComPtr<ID3D11Device> pDevice)
{
	CreateBaseShader(pDevice);

	m_FXAAPixelShader = std::make_unique<class PixelShader>();
	m_FXAAPixelShader->Initialize(pDevice, L"../Data/Shader/FXAA.fx", "FxaaPS", "ps_5_0");
}

void FXAA::Apply(
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext, 
	Microsoft::WRL::ComPtr <ID3D11ShaderResourceView> pSRV,
	UINT width, UINT height)
{

	pDeviceContext->IASetInputLayout(NULL);
	pDeviceContext->IASetVertexBuffers(0, 0, NULL, NULL, NULL);
	pDeviceContext->IASetIndexBuffer(NULL, DXGI_FORMAT_UNKNOWN, 0);
	pDeviceContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	float _cbufferData[20] =
	{
		0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f,
		1.0f / width, 1.0f / height, 0.0f, 0.0f,
	};


	ConstantBufferManager::SetGeneralCBuffer(pDeviceContext, CBufferType::PS, 
		CBufferSize::SIZE_20, 0, _cbufferData);

	ID3D11ShaderResourceView* _srv[1] =
	{
		pSRV.Get(),
	};

	pDeviceContext->PSSetShaderResources(0, 1, _srv);

	// Set the shaders
	pDeviceContext->VSSetShader(m_pFullScreenQuadVS->GetVertexShader(), NULL, 0);
	pDeviceContext->PSSetShader(m_FXAAPixelShader->GetPixelShader(), NULL, 0);

	pDeviceContext->Draw(4, 0);

	// Cleanup
	_srv[0] = nullptr;
	pDeviceContext->PSSetShaderResources(0, 1, _srv);
	pDeviceContext->VSSetShader(NULL, NULL, 0);
	pDeviceContext->PSSetShader(NULL, NULL, 0);
}
