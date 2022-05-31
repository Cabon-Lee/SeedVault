#include "DownSampler.h"

#include "Shaders.h"
#include "RenderTargetView.h"
#include "UAVRenderTargetView.h"
#include "RasterizerState.h"

DownSampler::DownSampler()
	: m_SampleScale(1)
	, m_VertexShader(nullptr)
	, m_PixelShader(nullptr)
{
	
}

DownSampler::~DownSampler()
{
	m_VertexShader.reset();
	m_PixelShader.reset();
	m_SamplingRTV_V.clear();
}

void DownSampler::Initialize(Microsoft::WRL::ComPtr<ID3D11Device> pDevice,
	std::shared_ptr<VertexShader> pVertexShader,
	std::shared_ptr<PixelShader> pPixelShader)
{
	m_SamplingRTV_V.resize(SAMPLE_SCALE);

	for (unsigned int i = 0; i < SAMPLE_SCALE; i++)
	{
		UINT _scale = SCREEN_SIZE[i];

		m_SamplingRTV_V[i] = std::make_shared<UAVRenderTargetView>();
		m_SamplingRTV_V[i]->Initialize(pDevice, _scale, _scale);
	}

	m_VertexShader = pVertexShader;	m_PixelShader = pPixelShader;
}

// 렌더타겟 뷰를 묶고, 클리어까지 처리함
void DownSampler::BindRenderTarget(Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext,
	const unsigned int RTVindex)
{
	pDeviceContext->RSSetViewports(1, &m_SamplingRTV_V[RTVindex]->GetViewPort());

	ID3D11RenderTargetView* rt[1] = { nullptr };
	rt[0] = m_SamplingRTV_V[RTVindex]->GetRenderTargerViewRawptr();

	pDeviceContext->OMSetRenderTargets(1, rt, NULL);
	float ClearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };

	pDeviceContext->ClearRenderTargetView(m_SamplingRTV_V[RTVindex]->GetRenderTargerViewRawptr(), ClearColor);
}

void DownSampler::DownSample(
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext, RenderTargetView* pTarget)
{
	// 임시 소스에 저장
	m_pTempSource = pTarget;
	m_TempSampleCount = -1;
	// 다운 샘플링
	for (unsigned int i = 0; i < m_SampleScale; i++)
	{
		BindRenderTarget(pDeviceContext, i);
		Sampling(pDeviceContext, m_pTempSource);
		m_pTempSource = m_SamplingRTV_V[i].get();
		m_TempSampleCount++;
	}
}

void DownSampler::UpSample(Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext, 
	RenderTargetView* pSwapChain)
{
	// 업 샘플링
	// SampleCount가 0이 아닌 동안
	// _nowSource의 바통을 이어받는다
	while (m_TempSampleCount != 0 && m_TempSampleCount > 0)
	{
		m_TempSampleCount--;
		BindRenderTarget(pDeviceContext, m_TempSampleCount);
		Sampling(pDeviceContext, m_pTempSource);
		m_pTempSource = m_SamplingRTV_V[m_TempSampleCount].get();
	}

	pDeviceContext->RSSetViewports(1, &pSwapChain->GetViewPort());

	ID3D11RenderTargetView* rt[1] = { nullptr };
	rt[0] = pSwapChain->GetRenderTargerViewRawptr();
	pDeviceContext->OMSetRenderTargets(1, rt, NULL);
	Sampling(pDeviceContext, m_pTempSource);
}


void DownSampler::UpScaleCopy(
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext,
	unsigned int scale,
	RenderTargetView* pSource)
{
	// 외부에서 업 스텝할 소스를 가져온다.
	// 이전에 몇 단계까지 했는지 가져올 것(거기서 +1 해서 접근해야함)
	// ㄴ32해상도라면 64해상도로
	// 이 클래스 밖에서 가져갈 수 있도록 업스텝이 적용된 RTV를 TempSource에 복사해야함

	unsigned int _nowSampleIndex = scale - 1;
	BindRenderTarget(pDeviceContext, _nowSampleIndex);
	Sampling(pDeviceContext, pSource);

	ID3D11RenderTargetView* rt[1] = { nullptr };
	pDeviceContext->OMSetRenderTargets(1, rt, NULL);

	m_pTempSource = m_SamplingRTV_V[_nowSampleIndex].get();
}

void DownSampler::DownScaleCopy(
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext, 
	unsigned int scale, RenderTargetView* pSource)
{
	BindRenderTarget(pDeviceContext, 0);
	Sampling(pDeviceContext, pSource);

	ID3D11RenderTargetView* rt[1] = { nullptr };
	pDeviceContext->OMSetRenderTargets(1, rt, NULL);

	m_pTempSource = m_SamplingRTV_V[0].get();
}

void DownSampler::DownAndUpSamepling(Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext,
	RenderTargetView* pTarget, RenderTargetView* pDrawing)
{
	// 임시 소스에 저장
	m_pTempSource = pTarget;
	m_TempSampleCount = -1;
	// 다운 샘플링
	for (unsigned int i = 0; i < m_SampleScale; i++)
	{
		BindRenderTarget(pDeviceContext, i);
		Sampling(pDeviceContext, m_pTempSource);
		m_pTempSource = m_SamplingRTV_V[i].get();
		m_TempSampleCount++;
	}


	// 업 샘플링
	// SampleCount가 0이 아닌 동안
	// _nowSource의 바통을 이어받는다
	while (m_TempSampleCount != 0 && m_TempSampleCount > 0)
	{
		m_TempSampleCount--;
		BindRenderTarget(pDeviceContext, m_TempSampleCount);
		Sampling(pDeviceContext, m_pTempSource);
		m_pTempSource = m_SamplingRTV_V[m_TempSampleCount].get();
	}

	pDeviceContext->RSSetViewports(1, &pDrawing->GetViewPort());

	ID3D11RenderTargetView* rt[1] = { nullptr };
	rt[0] = pDrawing->GetRenderTargerViewRawptr();
	pDeviceContext->OMSetRenderTargets(1, rt, NULL);
	Sampling(pDeviceContext, m_pTempSource);
}

void DownSampler::Sampling(
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext, 
	RenderTargetView* pRTVToSample)
{
	ID3D11ShaderResourceView* arrViews[1] = { pRTVToSample->GetShaderResourceViewRawptr() };
	pDeviceContext->PSSetShaderResources(0, 1, &arrViews[0]);

	pDeviceContext->IASetInputLayout(NULL);
	pDeviceContext->IASetVertexBuffers(0, 0, NULL, NULL, NULL);
	pDeviceContext->IASetIndexBuffer(NULL, DXGI_FORMAT_UNKNOWN, 0);
	pDeviceContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	//ID3D11SamplerState* arrSamplers[1] = { RasterizerState::GetLinearSamplerState() };
	ID3D11SamplerState* arrSamplers[1] = { RasterizerState::GetPointSamplerState() };
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

RenderTargetView* DownSampler::GetTempSourceRawptr()
{
	return m_pTempSource;
}

RenderTargetView* DownSampler::GetSampleRawptr(const unsigned int RTVindex)
{
	return static_cast<RenderTargetView*>(m_SamplingRTV_V[RTVindex].get());
}

std::shared_ptr<UAVRenderTargetView> DownSampler::GetSample(const unsigned int RTVindex)
{
	return m_SamplingRTV_V[RTVindex];
}
