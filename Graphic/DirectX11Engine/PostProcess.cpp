#include "PostProcess.h"

#include "Shaders.h"
#include "OffScreenRenderTarget.h"
#include "HighDynamicRangeRender.h"
#include "Adaptation.h"
#include "DownSampler.h"
#include "BlurFilter.h"
#include "BungieBloom.h"
#include "HLSLBloom.h"
#include "BloomFilter.h"
#include "Sampler.h"
#include "FXAA.h"

#include "RenderTargetView.h"
#include "UAVRenderTargetView.h"
#include "RasterizerState.h"

#define BUNGIE_DOWNSAMPLE

PostProcess::PostProcess()
	: m_OffScreen(nullptr)
	, m_HDRRender(nullptr)
	, m_Adaptation(nullptr)
	, m_BlurFilter(nullptr)
	, m_BungieBloom(nullptr)
	, m_DownSampler(nullptr)
	, m_SampleScale(0)
{

}

PostProcess::~PostProcess()
{
	m_OffScreen.reset();
	m_HDRRender.reset();
	m_Adaptation.reset();
	m_BlurFilter.reset();
	m_BungieBloom.reset();
	m_DownSampler.reset();
}

void PostProcess::Initialize(
	Microsoft::WRL::ComPtr<ID3D11Device> pDevice, UINT width, UINT height)
{
	m_pDevice = pDevice;


	m_FirstBlurBuffer = std::make_shared<UAVRenderTargetView>();
	m_FirstBlurBuffer->Initialize(pDevice, width, height);

	m_SecondBlurBuffer = std::make_shared<UAVRenderTargetView>();
	m_SecondBlurBuffer->Initialize(pDevice, width, height);

	m_ThirdBlurBuffer = std::make_shared<UAVRenderTargetView>();
	m_ThirdBlurBuffer->Initialize(pDevice, width, height);

	m_FirstBuffer = std::make_shared<UAVRenderTargetView>();
	m_FirstBuffer->Initialize(pDevice, width, height);

	m_SecondBuffer = std::make_shared<UAVRenderTargetView>();
	m_SecondBuffer->Initialize(pDevice, width, height);


	m_HDRRender = std::make_shared<HighDynamicRangeRender>();
	m_HDRRender->Initialize(pDevice, width, height);

	m_Adaptation = std::make_shared<Adaptation>();
	m_Adaptation->Initialize(pDevice, width, height);

	m_BlurFilter = std::make_shared<BlurFilter>();
	m_BlurFilter->Initialize(pDevice, width, height);

	m_BungieBloom = std::make_shared<BungieBloom>();
	m_BungieBloom->Initialize(pDevice, width, height);

	m_HLSLBloom = std::make_shared<HLSLBloom>();
	m_HLSLBloom->Initialize(pDevice, width, height);

	m_BloomFilter = std::make_shared<BloomFilter>();
	m_BloomFilter->Initialize(pDevice, width, height);

	m_Sampler = std::make_shared<Sampler>();
	m_Sampler->Initialize(pDevice, width, height);

	m_NonePostProcessVertexShader = std::make_shared<VertexShader>();
	m_NonePostProcessVertexShader->Initialize(pDevice, L"../Data/Shader/ps_deferredTotal.hlsl", "FullScreenQuadVS", "vs_5_0");

	m_NonePostProcessPixelShader = std::make_shared<PixelShader>();
	m_NonePostProcessPixelShader->Initialize(pDevice, L"../Data/Shader/ps_deferredTotal.hlsl", "NormalRender", "ps_5_0");

	// ���̴��� �����Ұ� �ƴ϶� Copy �Լ��� ���� ���ͳ� ���ٸ� �Ѱ��ִ°� ���
	m_OffScreen = std::make_shared<OffScreenRenderTarget>();
	m_OffScreen->Initialize(pDevice, width, height, m_NonePostProcessVertexShader, m_NonePostProcessPixelShader);

	m_DownSampler = std::make_shared<DownSampler>();
	m_DownSampler->Initialize(pDevice, m_NonePostProcessVertexShader, m_NonePostProcessPixelShader);

	m_UpSampler = std::make_shared<DownSampler>();
	m_UpSampler->Initialize(pDevice, m_NonePostProcessVertexShader, m_NonePostProcessPixelShader);

	m_FXAA = std::make_unique<class FXAA>();
	m_FXAA->Initialize(pDevice);
}

void PostProcess::ApplyPostProcess(
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext,
	std::shared_ptr<RenderTargetView> pRenderTarget,
	std::shared_ptr<RenderTargetView> pSwapChainRenderTarget,
	ePOST_PROCESS process)
{

	switch (process)
	{
	case ePOST_PROCESS::NONE:
	{
		ID3D11RenderTargetView* rt[1] = { nullptr };
		rt[0] = pSwapChainRenderTarget->GetRenderTargerViewRawptr();
		pDeviceContext->OMSetRenderTargets(1, rt, NULL);
		NonePostProcess(pDeviceContext, pRenderTarget);
		break;
	}
	case ePOST_PROCESS::HDR:
	{
		m_HDRRender->Apply(pDeviceContext, pRenderTarget, pSwapChainRenderTarget);
		break;
	}
	case ePOST_PROCESS::ADAPTATION:
	{
		m_Adaptation->Apply(pDeviceContext, pRenderTarget, pSwapChainRenderTarget);
		break;
	}
	case ePOST_PROCESS::DOWNSAMPLE:
	{
		m_DownSampler->DownSample(pDeviceContext, pRenderTarget.get());
		m_DownSampler->UpSample(pDeviceContext, pSwapChainRenderTarget.get());

		break;
	}
	case ePOST_PROCESS::BLUR:
	{
		// ���� ��ũ���� �׸��� �Ű� �׸���
		// �̶� ���� ��ũ���� ũ��� �ٿ� ���ø��� �ػ�
		m_DownSampler->DownSample(pDeviceContext, pRenderTarget.get());														// 1. �ٿ� ���ø� �Ѵ�
		m_OffScreen->Copy(pDeviceContext, m_DownSampler->GetTempSourceRawptr());												// 2. ���� ��ũ���� ���� ȭ���� �����Ѵ�(������ �ٲٴ°� �ƴ�)
		m_BlurFilter->Apply(pDeviceContext, 
			m_OffScreen->GetUAVRenderTarget()->GetShaderResourceView(), 
			m_OffScreen->GetUAVRenderTarget()->GetUnorderedAccessView());			// 3. ���� ��ũ���� ȭ�� �ؽ���(SRV)�� ���� ���δ�
		m_OffScreen->CopyTo(pDeviceContext, m_DownSampler->GetTempSourceRawptr());											// 4. ���� ��ũ���� ȭ�� �ؽ��ĸ� �ٿ� ���÷��� ���� �ҽ�(RenderTargetView)�� �����Ѵ�
		m_DownSampler->UpSample(pDeviceContext, pSwapChainRenderTarget.get());												// 5. �� ���ø� �� �� SwapChainRenderTarget�� ���� Ÿ�ٿ� �����Ѵ�

		break;
	}
	case ePOST_PROCESS::BLOOM:
	{
		
		m_OffScreen->Copy(pDeviceContext, pRenderTarget);
		m_BungieBloom->ApplyBoomCurve(pDeviceContext, m_OffScreen->GetUAVRenderTarget().get());

#ifdef BUNGIE_DOWNSAMPLE
		m_Sampler->DownSample(pDeviceContext, m_OffScreen->GetUAVRenderTarget()->GetShaderResourceViewRawptr());

		{
			m_BlurFilter->Apply(
				pDeviceContext,
				m_Sampler->m_pSecond6x6SRV,
				m_Sampler->m_pSecond6x6UAV,
				m_FirstBlurBuffer.get(),
				(4 * 6 * 6), m_NowOption.bloomScale);


			m_Sampler->UpSample(pDeviceContext,
				m_Sampler->m_pSecond6x6SRV.Get(),
				m_Sampler->m_pUpSecond6x6UAV.Get(),
				(4 * 6 * 6));


			m_Sampler->Merge(pDeviceContext,
				m_Sampler->m_pUpSecond6x6SRV.Get(),
				m_Sampler->m_pFirst6x6SRV.Get(),
				m_FirstBuffer->GetUnorderedAccessViewRawptr(),
				(4 * 6));
	}

		{
			m_BlurFilter->Apply(
				pDeviceContext,
				m_FirstBuffer->GetShaderResourceView(),
				m_FirstBuffer->GetUnorderedAccessView(),
				m_SecondBlurBuffer.get(),
				(4 * 6), m_NowOption.bloomScale);

			m_Sampler->UpSample(pDeviceContext,
				m_SecondBlurBuffer->GetShaderResourceViewRawptr(),
				m_Sampler->m_pUpFirst6x6UAV.Get(),
				(4 * 6));

			m_Sampler->Merge(
				pDeviceContext,
				m_Sampler->m_pUpFirst6x6SRV.Get(),
				m_Sampler->m_pFirst4x4SRV.Get(),
				m_SecondBuffer->GetUnorderedAccessViewRawptr(),
				4);
		}

		{
			m_BlurFilter->Apply(
				pDeviceContext,
				m_SecondBuffer->GetShaderResourceView(),
				m_SecondBuffer->GetUnorderedAccessView(),
				m_ThirdBlurBuffer.get(),
				4, m_NowOption.bloomScale);

			m_Sampler->BilinearSample(pDeviceContext,
				m_ThirdBlurBuffer->GetShaderResourceViewRawptr(),
				m_Sampler->m_pUpFirst4x4UAV.Get());

			m_Sampler->Merge(pDeviceContext,
				m_Sampler->m_pUpFirst4x4SRV.Get(),
				pRenderTarget->GetShaderResourceViewRawptr(),
				m_OffScreen->GetUAVRenderTarget()->GetUnorderedAccessViewRawptr());
		}

		m_HDRRender->Apply(pDeviceContext, m_OffScreen->GetUAVRenderTarget(), pSwapChainRenderTarget);

#else

		m_Sampler->DownSampleEX(pDeviceContext, m_OffScreen->GetUAVRenderTarget()->GetShaderResourceViewRawptr());

		{
			m_BlurFilter->Apply(
				pDeviceContext,
				m_Sampler->m_pFirst6x6SRV,
				m_Sampler->m_pFirst6x6UAV,
				m_FirstBlurBuffer.get(),
				(4 * 6 ), m_NowOption.bloomScale);


			m_Sampler->UpSample(pDeviceContext,
				m_Sampler->m_pFirst6x6SRV.Get(),
				m_Sampler->m_pUpFirst6x6UAV.Get(),
				(4 * 6 ));


			m_Sampler->Merge(pDeviceContext,
				m_Sampler->m_pUpFirst6x6SRV.Get(),
				m_Sampler->m_pFirst4x4SRV.Get(),
				m_FirstBuffer->GetUnorderedAccessViewRawptr(),
				(4));
		}

		{
			m_BlurFilter->Apply(
				pDeviceContext,
				m_FirstBuffer->GetShaderResourceView(),
				m_FirstBuffer->GetUnorderedAccessView(),
				m_SecondBlurBuffer.get(),
				(4), m_NowOption.bloomScale);

			m_Sampler->BilinearSample(pDeviceContext,
				m_FirstBuffer->GetShaderResourceViewRawptr(),
				m_Sampler->m_pUpFirst4x4UAV.Get());

			m_Sampler->Merge(pDeviceContext,
				m_Sampler->m_pUpFirst4x4SRV.Get(),
				pRenderTarget->GetShaderResourceViewRawptr(),
				m_OffScreen->GetUAVRenderTarget()->GetUnorderedAccessViewRawptr());
		}
#endif
		if (m_NowOption.isColorAdjust)
		{
			m_Adaptation->Apply(pDeviceContext, m_OffScreen->GetUAVRenderTarget(), pRenderTarget);
			m_OffScreen->Copy(pDeviceContext, pRenderTarget->GetShaderResourceViewRawptr());
			m_BungieBloom->ColorCalb(pDeviceContext, m_OffScreen->GetUAVRenderTarget().get(), m_ThirdBlurBuffer.get());
			m_OffScreen->CopyFromTo(pDeviceContext, m_ThirdBlurBuffer->GetShaderResourceViewRawptr(), pSwapChainRenderTarget.get());
		}
		else
		{
			m_Adaptation->Apply(pDeviceContext, m_OffScreen->GetUAVRenderTarget(), pSwapChainRenderTarget);
		}
		break;
	}

	default:
		break;
	}


}

void PostProcess::ApplyFXAA(
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext, 
	std::shared_ptr<RenderTargetView> pSwapChainRenderTarget, UINT width, UINT height)
{
	// ��� ��ó���� ����� ����Ÿ���� �����´�
	m_OffScreen->Copy(pDeviceContext, pSwapChainRenderTarget);
	
	pDeviceContext->OMSetRenderTargets(1, pSwapChainRenderTarget->GetRenderTargetViewAddressOf(), nullptr);

	m_FXAA->Apply(pDeviceContext, 
		m_OffScreen->GetUAVRenderTarget()->GetShaderResourceView(),
		width, height);


}

void PostProcess::OnResize(Microsoft::WRL::ComPtr<ID3D11Device> pDevice, UINT width, UINT height)
{
	// HDR�̳� �������� ��� �¸������ ���� �ʿ� ���� �� ���⵵ �ѵ�...
	{
		// HDR ����
		m_HDRRender->OnResize(pDevice, width, height);
	}
	{
		// ������
		m_Adaptation->OnResize(pDevice, width, height);
	}
	{
		m_OffScreen->OnResize(pDevice, width, height);
		m_BungieBloom->OnResize(pDevice, width, height);
		m_HLSLBloom->OnResize(pDevice, width, height);
		m_BloomFilter->OnResize(pDevice, width, height);
		m_Sampler->OnResize(pDevice, width, height);
		m_BlurFilter->OnResize(pDevice, width, height);
	}

	m_FirstBlurBuffer->Reset();
	m_FirstBlurBuffer->Initialize(pDevice, width, height);

	m_SecondBlurBuffer->Reset();
	m_SecondBlurBuffer->Initialize(pDevice, width, height);

	m_ThirdBlurBuffer->Reset();
	m_ThirdBlurBuffer->Initialize(pDevice, width, height);

	m_FirstBuffer->Reset();
	m_FirstBuffer->Initialize(pDevice, width, height);

	m_SecondBuffer->Reset();
	m_SecondBuffer->Initialize(pDevice, width, height);

}

// ����Ʈ ���μ����� �Ϻ� ���͸� �Ѱ��ִ� 
void PostProcess::RenderOption(IRenderOption& option)
{
	m_NowOption = option;

	m_HDRRender->ApplyFactor(option);
	m_Adaptation->ApplyFactor(option);
	m_BlurFilter->ApplyFactor(option);
	m_BungieBloom->ApplyFactor(option);
	m_HLSLBloom->ApplyFactor(option);
	m_BlurFilter->ApplyFactor(option);
	m_BloomFilter->ApplyFactor(option);

	if (option.sampleScale <= 0)
	{
		option.sampleScale = 1;
	}

	if (m_SampleScale != option.sampleScale)
	{
		m_SampleScale = option.sampleScale;
		m_DownSampler->m_SampleScale = m_SampleScale;

		// ���� �������� ������ 1024�� �����ش�
		// ���� ���÷��� �����ٰ� 
		//UINT _scale = (UINT)(FULL_SCREEN / m_SampleScale);
		//m_BlurFilter->OnResize(m_pDevice, _scale, _scale);
		//m_OffScreen->OnResize(m_pDevice, _scale, _scale);
		//m_BloomFilter->OnResize(m_pDevice, _scale, _scale);
	}
}

void PostProcess::NonePostProcess(
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext,
	std::shared_ptr<RenderTargetView> pRenderTarget)
{
	pDeviceContext->RSSetState(RasterizerState::GetSolidNormal());

	ID3D11ShaderResourceView* arrViews[1] = { pRenderTarget->GetShaderResourceViewRawptr() };
	pDeviceContext->PSSetShaderResources(0, 1, &arrViews[0]);

	pDeviceContext->IASetInputLayout(NULL);
	pDeviceContext->IASetVertexBuffers(0, 0, NULL, NULL, NULL);
	pDeviceContext->IASetIndexBuffer(NULL, DXGI_FORMAT_UNKNOWN, 0);
	pDeviceContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	ID3D11SamplerState* arrSamplers[1] = { RasterizerState::GetLinearSamplerState() };
	pDeviceContext->PSSetSamplers(0, 1, arrSamplers);

	// Set the shaders
	pDeviceContext->VSSetShader(m_NonePostProcessVertexShader->GetVertexShader(), NULL, 0);
	pDeviceContext->PSSetShader(m_NonePostProcessPixelShader->GetPixelShader(), NULL, 0);

	pDeviceContext->Draw(4, 0);

	// Cleanup
	ZeroMemory(arrViews, sizeof(arrViews));
	pDeviceContext->PSSetShaderResources(0, 1, arrViews);
	pDeviceContext->VSSetShader(NULL, NULL, 0);
	pDeviceContext->PSSetShader(NULL, NULL, 0);
}

