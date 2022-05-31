#include "DebugRender.h"

#include "VertexHelper.h"
#include "RasterizerState.h"
#include "RenderTargetView.h"
#include "DepthStencilView.h"

#include "ConstantBufferManager.h"

#include "Shaders.h"

#include "Quad.h"

DebugRender::DebugRender()
{

}

DebugRender::~DebugRender()
{

}

void DebugRender::Initialize(Microsoft::WRL::ComPtr<ID3D11Device> pDevice)
{
	// 외부에서 만들어서 넣어줄 수도 있지만 내부에서 처리하는게 훨씬 덜 복잡하다고 생각했다
	m_pScreenQaud = std::make_shared<Quad>();
	m_pScreenQaud->BuildHelper(pDevice.Get());

	m_DebugVertexShader = std::make_shared<VertexShader>();
	m_DebugVertexShader->Initialize(pDevice, L"../Data/Shader/ps_deferredTotal.hlsl", "FullScreenQuadVS", "vs_5_0");

	m_DebugPixelShader = std::make_shared<PixelShader>();
	m_DebugPixelShader->Initialize(pDevice, L"../Data/Shader/ps_deferredTotal.hlsl", "DebugRender", "ps_5_0");
}

void DebugRender::DrawDebugInfo(
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext, 
	std::shared_ptr<RenderTargetView> pDebugInfo)
{

	ID3D11ShaderResourceView* arrViews[1] = { pDebugInfo->GetShaderResourceViewRawptr() };
	pDeviceContext->PSSetShaderResources(0, 1, &arrViews[0]);

	pDeviceContext->IASetInputLayout(NULL);
	pDeviceContext->IASetVertexBuffers(0, 0, NULL, NULL, NULL);
	pDeviceContext->IASetIndexBuffer(NULL, DXGI_FORMAT_UNKNOWN, 0);
	pDeviceContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	ID3D11SamplerState* arrSamplers[1] = { RasterizerState::GetPointSamplerState() };
	pDeviceContext->PSSetSamplers(0, 1, arrSamplers);

	/// 블렌드 설정을 해야지 기존 렌더 타겟에 그려져 있던 것에 추가로 덧그릴 수 있다
	/// 이게 없을 때는 새로 그려버린다
	pDeviceContext->OMSetBlendState(RasterizerState::GetBlenderState(), nullptr, 0xffffff);
	//pDeviceContext->OMSetBlendState(RasterizerState::GetOutLineBlendState(), nullptr, 0xffffff);

	// Set the shaders
	pDeviceContext->VSSetShader(m_DebugVertexShader->GetVertexShader(), NULL, 0);
	pDeviceContext->PSSetShader(m_DebugPixelShader->GetPixelShader(), NULL, 0);

	pDeviceContext->Draw(4, 0);

	// Cleanup
	ZeroMemory(arrViews, sizeof(arrViews));
	pDeviceContext->PSSetShaderResources(0, 1, arrViews);
	pDeviceContext->VSSetShader(NULL, NULL, 0);
	pDeviceContext->PSSetShader(NULL, NULL, 0);
}

void DebugRender::DrawDeferredDebug(
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext, 
	std::vector<std::shared_ptr<RenderTargetView>> pDeferred,
	std::shared_ptr<VertexShader> pVertexShader,
	std::shared_ptr<PixelShader> pPixelShader)
{
	UINT stride = sizeof(Vertex::Vertex2D);
	UINT offset = 0;

	pDeviceContext->RSSetState(RasterizerState::GetNoCullingRS());

	pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	pDeviceContext->IASetVertexBuffers(0, 1, m_pScreenQaud->GetAddressOfVertexBuffer(), &stride, &offset);
	pDeviceContext->IASetIndexBuffer(m_pScreenQaud->GetIndexBuffer(), DXGI_FORMAT_R32_UINT, 0);
	pDeviceContext->PSSetSamplers(0, 1, RasterizerState::GetLinearSamplerStateAddressOf());

	ID3D11ShaderResourceView* _arrView[MAX_COUNT] = { nullptr };

	for (int i = 0; i < pDeferred.size(); i++)
	{
		_arrView[i] = pDeferred[i]->GetShaderResourceViewRawptr();
	}

	for (int i = 0; i < 4; i++)
	{
		double _nowX = (((i * 0.5) - 1) + 0.25f);

		// Scale and shift quad to lower-right corner.
		XMMATRIX world(
			0.25f, 0.0f, 0.0f, 0.0f,
			0.0f, 0.25f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			static_cast<float>(_nowX), -0.75f, 0.0f, 1.0f);

		ConstantBufferManager::GetVSPerObjectBuffer()->data.World = world;
		ConstantBufferManager::GetVSPerObjectBuffer()->ApplyChanges();

		pDeviceContext->VSSetConstantBuffers(0, 1, ConstantBufferManager::GetVSPerObjectBuffer()->GetAddressOf());
		pDeviceContext->PSSetShaderResources(0, 1, &_arrView[i]);


		pDeviceContext->IASetInputLayout(pVertexShader->inputLayout.Get());
		pDeviceContext->VSSetShader(pVertexShader->GetVertexShader(), NULL, 0);
		pDeviceContext->PSSetShader(pPixelShader->GetPixelShader(), NULL, 0);
		pDeviceContext->DrawIndexed(m_pScreenQaud->GetIndexCount(), 0, 0);
	}

	if (pDeferred.size() <= 4) return;

	for (int i = 4; i < pDeferred.size(); i++)
	{
		double _nowY = ((((i - 3) * 0.5) - 1) + 0.25f);

		// Scale and shift quad to lower-right corner.
		XMMATRIX world(
			0.25f, 0.0f, 0.0f, 0.0f,
			0.0f, 0.25f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.75, static_cast<float>(_nowY), 0.0f, 1.0f);

		ConstantBufferManager::GetVSPerObjectBuffer()->data.World = world;
		ConstantBufferManager::GetVSPerObjectBuffer()->ApplyChanges();

		pDeviceContext->VSSetConstantBuffers(0, 1, ConstantBufferManager::GetVSPerObjectBuffer()->GetAddressOf());
		pDeviceContext->PSSetShaderResources(0, 1, &_arrView[i]);

		pDeviceContext->IASetInputLayout(pVertexShader->inputLayout.Get());
		pDeviceContext->VSSetShader(pVertexShader->GetVertexShader(), NULL, 0);
		pDeviceContext->PSSetShader(pPixelShader->GetPixelShader(), NULL, 0);

		pDeviceContext->DrawIndexed(m_pScreenQaud->GetIndexCount(), 0, 0);
	}
}

void DebugRender::DrawDeferredDebug(
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext, 
	std::vector<std::shared_ptr<RenderTargetView>> pDeferred, 
	std::shared_ptr<DepthStencilView> pDepth,
	std::shared_ptr<VertexShader> pVertexShader, 
	std::shared_ptr<PixelShader> pPixelShader)
{
	UINT stride = sizeof(Vertex::Vertex2D);
	UINT offset = 0;

	pDeviceContext->RSSetState(RasterizerState::GetNoCullingRS());

	pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	pDeviceContext->IASetVertexBuffers(0, 1, m_pScreenQaud->GetAddressOfVertexBuffer(), &stride, &offset);
	pDeviceContext->IASetIndexBuffer(m_pScreenQaud->GetIndexBuffer(), DXGI_FORMAT_R32_UINT, 0);
	pDeviceContext->PSSetSamplers(0, 1, RasterizerState::GetLinearSamplerStateAddressOf());

	ID3D11ShaderResourceView* _arrView[MAX_COUNT] = { nullptr };

	for (int i = 0; i < pDeferred.size(); i++)
	{
		_arrView[i] = pDeferred[i]->GetShaderResourceViewRawptr();
	}
	_arrView[pDeferred.size()] = pDepth->GetShaderResourceViewRawptr();


	for (int i = 0; i < 4; i++)
	{
		double _nowX = (((i * 0.5) - 1) + 0.25f);

		// Scale and shift quad to lower-right corner.
		XMMATRIX world(
			0.25f, 0.0f, 0.0f, 0.0f,
			0.0f, 0.25f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			static_cast<float>(_nowX), -0.75f, 0.0f, 1.0f);

		ConstantBufferManager::GetVSPerObjectBuffer()->data.World = world;
		ConstantBufferManager::GetVSPerObjectBuffer()->ApplyChanges();

		pDeviceContext->VSSetConstantBuffers(0, 1, ConstantBufferManager::GetVSPerObjectBuffer()->GetAddressOf());
		pDeviceContext->PSSetShaderResources(0, 1, &_arrView[i]);


		pDeviceContext->IASetInputLayout(pVertexShader->inputLayout.Get());
		pDeviceContext->VSSetShader(pVertexShader->GetVertexShader(), NULL, 0);
		pDeviceContext->PSSetShader(pPixelShader->GetPixelShader(), NULL, 0);
		pDeviceContext->DrawIndexed(m_pScreenQaud->GetIndexCount(), 0, 0);
	}

	if (pDeferred.size() <= 4) return;

	for (int i = 4; i < pDeferred.size() + 1; i++)
	{
		double _nowY = ((((i - 3) * 0.5) - 1) + 0.25f);

		// Scale and shift quad to lower-right corner.
		XMMATRIX world(
			0.25f, 0.0f, 0.0f, 0.0f,
			0.0f, 0.25f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.75, static_cast<float>(_nowY), 0.0f, 1.0f);

		ConstantBufferManager::GetVSPerObjectBuffer()->data.World = world;
		ConstantBufferManager::GetVSPerObjectBuffer()->ApplyChanges();

		pDeviceContext->VSSetConstantBuffers(0, 1, ConstantBufferManager::GetVSPerObjectBuffer()->GetAddressOf());
		pDeviceContext->PSSetShaderResources(0, 1, &_arrView[i]);

		pDeviceContext->IASetInputLayout(pVertexShader->inputLayout.Get());
		pDeviceContext->VSSetShader(pVertexShader->GetVertexShader(), NULL, 0);
		pDeviceContext->PSSetShader(pPixelShader->GetPixelShader(), NULL, 0);

		pDeviceContext->DrawIndexed(m_pScreenQaud->GetIndexCount(), 0, 0);
	}
}

void DebugRender::DrawDeferredDebug(
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext, 
	const std::vector<std::unique_ptr<RenderTargetView>>& pDeferred,
	ID3D11ShaderResourceView* pRenderTarget,
	class VertexShader* pVertexShader,
	class PixelShader* pPixelShader)
{
	UINT stride = sizeof(Vertex::Vertex2D);
	UINT offset = 0;

	pDeviceContext->RSSetState(RasterizerState::GetNoCullingRS());

	pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	pDeviceContext->IASetVertexBuffers(0, 1, m_pScreenQaud->GetAddressOfVertexBuffer(), &stride, &offset);
	pDeviceContext->IASetIndexBuffer(m_pScreenQaud->GetIndexBuffer(), DXGI_FORMAT_R32_UINT, 0);
	pDeviceContext->PSSetSamplers(0, 1, RasterizerState::GetLinearSamplerStateAddressOf());

	ID3D11ShaderResourceView* _arrView[MAX_COUNT] = { nullptr };

	for (int i = 0; i < pDeferred.size() - 1; i++)
	{
		_arrView[i] = pDeferred[i]->GetShaderResourceViewRawptr();
	}
	_arrView[pDeferred.size() - 1] = pRenderTarget;


	for (int i = 0; i < 4; i++)
	{
		double _nowX = (((i * 0.5) - 1) + 0.25f);

		// Scale and shift quad to lower-right corner.
		XMMATRIX world(
			0.25f, 0.0f, 0.0f, 0.0f,
			0.0f, 0.25f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			static_cast<float>(_nowX), -0.75f, 0.0f, 1.0f);

		ConstantBufferManager::GetVSPerObjectBuffer()->data.World = world;
		ConstantBufferManager::GetVSPerObjectBuffer()->ApplyChanges();

		pDeviceContext->VSSetConstantBuffers(0, 1, ConstantBufferManager::GetVSPerObjectBuffer()->GetAddressOf());
		pDeviceContext->PSSetShaderResources(0, 1, &_arrView[i]);


		pDeviceContext->IASetInputLayout(pVertexShader->inputLayout.Get());
		pDeviceContext->VSSetShader(pVertexShader->GetVertexShader(), NULL, 0);
		pDeviceContext->PSSetShader(pPixelShader->GetPixelShader(), NULL, 0);
		pDeviceContext->DrawIndexed(m_pScreenQaud->GetIndexCount(), 0, 0);
	}

	if (pDeferred.size() <= 4) return;

	for (int i = 4; i < pDeferred.size() + 1; i++)
	{
		double _nowY = ((((i - 3) * 0.5) - 1) + 0.25f);

		// Scale and shift quad to lower-right corner.
		XMMATRIX world(
			0.25f, 0.0f, 0.0f, 0.0f,
			0.0f, 0.25f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.75, static_cast<float>(_nowY), 0.0f, 1.0f);

		ConstantBufferManager::GetVSPerObjectBuffer()->data.World = world;
		ConstantBufferManager::GetVSPerObjectBuffer()->ApplyChanges();

		pDeviceContext->VSSetConstantBuffers(0, 1, ConstantBufferManager::GetVSPerObjectBuffer()->GetAddressOf());
		pDeviceContext->PSSetShaderResources(0, 1, &_arrView[i]);

		pDeviceContext->IASetInputLayout(pVertexShader->inputLayout.Get());
		pDeviceContext->VSSetShader(pVertexShader->GetVertexShader(), NULL, 0);
		pDeviceContext->PSSetShader(pPixelShader->GetPixelShader(), NULL, 0);

		pDeviceContext->DrawIndexed(m_pScreenQaud->GetIndexCount(), 0, 0);
	}
}
