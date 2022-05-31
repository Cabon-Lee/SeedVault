#include "OffScreenRenderTarget.h"

#include "Shaders.h"
#include "RenderTargetView.h"
#include "UAVRenderTargetView.h"
#include "RasterizerState.h"

#include "ErrorLogger.h"
#include "COMException.h"

OffScreenRenderTarget::OffScreenRenderTarget()
	: m_Width(0)
	, m_Height(0)

{

}

OffScreenRenderTarget::~OffScreenRenderTarget()
{

}


void OffScreenRenderTarget::Initialize(Microsoft::WRL::ComPtr <ID3D11Device> pDevice,
	UINT width, UINT height,
	std::shared_ptr<VertexShader> pVertexShader, std::shared_ptr<PixelShader> pPixelShader)
{
	m_VertexShader = pVertexShader;
	m_PixelShader = pPixelShader;

	m_Width = width; m_Height = height;

	m_pUAVRenderTarget.reset();
	m_pUAVRenderTarget = std::make_shared<UAVRenderTargetView>();
	m_pUAVRenderTarget->Initialize(pDevice, m_Width, m_Height);

	//CreateBuffer(pDevice, width, height);
}

void OffScreenRenderTarget::OnResize(Microsoft::WRL::ComPtr <ID3D11Device> pDevice, UINT width, UINT height)
{
	m_Width = width; m_Height = height;

	m_pUAVRenderTarget.reset();
	m_pUAVRenderTarget = std::make_shared<UAVRenderTargetView>();
	m_pUAVRenderTarget->Initialize(pDevice, m_Width, m_Height);

	//CreateBuffer(pDevice, width, height);
}

void OffScreenRenderTarget::RenderTargetBind(
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext,
	const D3D11_VIEWPORT& viewPort,
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> pRTV)
{
	// ·»´õ Å¸°Ù ¹­±â
	{
		pDeviceContext->RSSetViewports(1, &viewPort);

		ID3D11RenderTargetView* rt[1] = { nullptr };
		rt[0] = pRTV.Get();

		pDeviceContext->OMSetRenderTargets(1, rt, NULL);
		pDeviceContext->ClearRenderTargetView(pRTV.Get(), ClearColor);
	}
}

void OffScreenRenderTarget::CopyProcess(Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext)
{


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
	ID3D11ShaderResourceView* arrViews[1];
	{
		ZeroMemory(arrViews, sizeof(arrViews));
		pDeviceContext->PSSetShaderResources(0, 1, arrViews);
		pDeviceContext->VSSetShader(NULL, NULL, 0);
		pDeviceContext->PSSetShader(NULL, NULL, 0);
	}
}


void OffScreenRenderTarget::Copy(Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext,
	std::shared_ptr<RenderTargetView> pRenderTarget)
{
	// ·»´õ Å¸°Ù ¹­±â
	RenderTargetBind(pDeviceContext, 
		m_pUAVRenderTarget->GetViewPort(), m_pUAVRenderTarget->GetRenderTargetView());
	// ½¦ÀÌ´õ ¸®¼Ò½º ºä ¹­±â
	pDeviceContext->PSSetShaderResources(0, 1, pRenderTarget->GetShaderResourceViewAddressOf());
	CopyProcess(pDeviceContext);

	ID3D11RenderTargetView* _rt[1] = { nullptr };
	pDeviceContext->OMSetRenderTargets(1, _rt, nullptr);
}

void OffScreenRenderTarget::Copy(Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext, RenderTargetView* pRenderTarget)
{
	RenderTargetBind(pDeviceContext, 
		m_pUAVRenderTarget->GetViewPort(), m_pUAVRenderTarget->GetRenderTargetView());
	pDeviceContext->PSSetShaderResources(0, 1, pRenderTarget->GetShaderResourceViewAddressOf());
	CopyProcess(pDeviceContext);

	ID3D11RenderTargetView* _rt[1] = { nullptr };
	pDeviceContext->OMSetRenderTargets(1, _rt, nullptr);
}

void OffScreenRenderTarget::Copy(Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext, ID3D11ShaderResourceView* pSource)
{
	RenderTargetBind(pDeviceContext,
		m_pUAVRenderTarget->GetViewPort(), m_pUAVRenderTarget->GetRenderTargetView());
	pDeviceContext->PSSetShaderResources(0, 1, &pSource);
	CopyProcess(pDeviceContext);

	ID3D11RenderTargetView* _rt[1] = { nullptr };
	pDeviceContext->OMSetRenderTargets(1, _rt, nullptr);
}

void OffScreenRenderTarget::CopyTo(
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext,
	std::shared_ptr<RenderTargetView> pRenderTarget)
{
	// Ä«ÇÇÇÒ ·»´õÅ¸°ÙÀ» ¹­¾îÁØ´Ù
	RenderTargetBind(pDeviceContext, pRenderTarget->GetViewPort(), pRenderTarget->GetRenderTargetView());
	pDeviceContext->PSSetShaderResources(0, 1, m_pUAVRenderTarget->GetShaderResourceViewAddressOf());
	CopyProcess(pDeviceContext);
}

void OffScreenRenderTarget::CopyTo(Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext, RenderTargetView* pRenderTarget)
{
	// Ä«ÇÇÇÒ ·»´õÅ¸°ÙÀ» ¹­¾îÁØ´Ù
	RenderTargetBind(pDeviceContext, pRenderTarget->GetViewPort(), pRenderTarget->GetRenderTargetView());
	pDeviceContext->PSSetShaderResources(0, 1, m_pUAVRenderTarget->GetShaderResourceViewAddressOf());
	CopyProcess(pDeviceContext);
}

void OffScreenRenderTarget::CopyFromTo(
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext, 
	RenderTargetView* pSource, RenderTargetView* pTarget)
{
	RenderTargetBind(pDeviceContext, pTarget->GetViewPort(), pTarget->GetRenderTargetView());
	pDeviceContext->PSSetShaderResources(0, 1, pSource->GetShaderResourceViewAddressOf());
	CopyProcess(pDeviceContext);
}

void OffScreenRenderTarget::CopyFromTo(
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext, 
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pSource, RenderTargetView* pTarget)
{
	RenderTargetBind(pDeviceContext, pTarget->GetViewPort(), pTarget->GetRenderTargetView());
	pDeviceContext->PSSetShaderResources(0, 1, pSource.GetAddressOf());
	CopyProcess(pDeviceContext);
}

std::shared_ptr<UAVRenderTargetView> OffScreenRenderTarget::GetUAVRenderTarget()
{
	return m_pUAVRenderTarget;
}


