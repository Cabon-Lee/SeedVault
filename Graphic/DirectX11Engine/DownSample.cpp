#include "DownSample.h"

#include "Shaders.h"
#include "RenderTargetView.h"
#include "RasterizerState.h"

DownSample::DownSample()
	: m_SampleScale(1)
	, m_pDevice(nullptr)
	, m_SamplingRTV(nullptr)
	, m_VertexShader(nullptr)
	, m_PixelShader(nullptr)
{
	
}

DownSample::~DownSample()
{
	m_pDevice.Reset();
	m_SamplingRTV.reset();
	m_VertexShader.reset();
	m_PixelShader.reset();
}

void DownSample::Initialize(Microsoft::WRL::ComPtr<ID3D11Device> pDevice, UINT width, UINT height)
{

	// 이례적으로 내부에 가지고 있다
	m_pDevice = pDevice;

	m_VertexShader = std::make_shared<VertexShader>();
	m_VertexShader->Initialize(pDevice, L"../Data/Shader/ps_deferredTotal.hlsl", "FullScreenQuadVS", "vs_5_0");

	m_PixelShader = std::make_shared<PixelShader>();
	m_PixelShader->Initialize(pDevice, L"../Data/Shader/ps_deferredTotal.hlsl", "NormalRender", "ps_5_0");

	m_SamplingRTV = std::make_shared<RenderTargetView>();
	m_SamplingRTV->Initialize(pDevice.Get(),
		width, height,
		D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE,
		DXGI_FORMAT_R8G8B8A8_UNORM,
		DXGI_FORMAT_R8G8B8A8_UNORM,
		DXGI_FORMAT_R8G8B8A8_UNORM);
}

// 렌더타겟 뷰를 묶고, 클리어까지 처리함
void DownSample::BindRenderTarget(Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext)
{
	ID3D11RenderTargetView* rt[1] = { nullptr };
	pDeviceContext->RSSetViewports(1, &m_SamplingRTV->GetViewPort());
	rt[0] = m_SamplingRTV->GetRenderTargerView();

	pDeviceContext->OMSetRenderTargets(1, rt, NULL);
	float ClearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };

	pDeviceContext->ClearRenderTargetView(m_SamplingRTV->GetRenderTargerView(), ClearColor);
}

void DownSample::Sampling(
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext, 
	std::shared_ptr<RenderTargetView> pRTVToSample)
{
	ID3D11ShaderResourceView* arrViews[1] = { pRTVToSample->GetShaderResourceView() };
	pDeviceContext->PSSetShaderResources(0, 1, &arrViews[0]);

	pDeviceContext->IASetInputLayout(NULL);
	pDeviceContext->IASetVertexBuffers(0, 0, NULL, NULL, NULL);
	pDeviceContext->IASetIndexBuffer(NULL, DXGI_FORMAT_UNKNOWN, 0);
	pDeviceContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	ID3D11SamplerState* arrSamplers[1] = { RasterizerState::GetLinearSamplerState() };
	pDeviceContext->PSSetSamplers(0, 1, arrSamplers);

	// Set the shaders
	pDeviceContext->VSSetShader(m_VertexShader->GetVertexShader(), NULL, 0);
	pDeviceContext->PSSetShader(m_PixelShader->GetPixelShader(), NULL, 0);

	pDeviceContext->Draw(4, 0);

	// Cleanup
	ZeroMemory(arrViews, sizeof(arrViews));
	pDeviceContext->PSSetShaderResources(0, 1, arrViews);
	pDeviceContext->VSSetShader(NULL, NULL, 0);
	pDeviceContext->PSSetShader(NULL, NULL, 0);
}

std::shared_ptr<RenderTargetView> DownSample::GetDownedRenderTarget()
{
	return m_SamplingRTV;
}
