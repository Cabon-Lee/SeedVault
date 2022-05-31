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

// ����Ÿ�� �並 ����, Ŭ������� ó����
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
	// �ӽ� �ҽ��� ����
	m_pTempSource = pTarget;
	m_TempSampleCount = -1;
	// �ٿ� ���ø�
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
	// �� ���ø�
	// SampleCount�� 0�� �ƴ� ����
	// _nowSource�� ������ �̾�޴´�
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
	// �ܺο��� �� ������ �ҽ��� �����´�.
	// ������ �� �ܰ���� �ߴ��� ������ ��(�ű⼭ +1 �ؼ� �����ؾ���)
	// ��32�ػ󵵶�� 64�ػ󵵷�
	// �� Ŭ���� �ۿ��� ������ �� �ֵ��� �������� ����� RTV�� TempSource�� �����ؾ���

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
	// �ӽ� �ҽ��� ����
	m_pTempSource = pTarget;
	m_TempSampleCount = -1;
	// �ٿ� ���ø�
	for (unsigned int i = 0; i < m_SampleScale; i++)
	{
		BindRenderTarget(pDeviceContext, i);
		Sampling(pDeviceContext, m_pTempSource);
		m_pTempSource = m_SamplingRTV_V[i].get();
		m_TempSampleCount++;
	}


	// �� ���ø�
	// SampleCount�� 0�� �ƴ� ����
	// _nowSource�� ������ �̾�޴´�
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
