#include "Adaptation.h"

#include "Shaders.h"
#include "RasterizerState.h"

#include "PostProcessCB.h"

#include "ErrorLogger.h"
#include "COMException.h"

Adaptation::Adaptation()
	: m_Adaptation(0.0f)
	, m_pPrevAverageLuminanceBuffer(nullptr)
	, m_pPrevAverageLuminanceUAV(nullptr)
	, m_pPrevAverageLuminanceSRV(nullptr)
{

}

Adaptation::~Adaptation()
{

}

void Adaptation::Initialize(Microsoft::WRL::ComPtr<ID3D11Device> pDevice, UINT width, UINT height)
{
	m_Width = width; m_Height = height;

	CreateBuffer(pDevice);		// 부모가 가진 버퍼들 생성
	CreateExtraBuffer(pDevice);	// 해당 개체가 가진 버퍼들 생성
	CreateShader(pDevice);
}

void Adaptation::CreateExtraBuffer(Microsoft::WRL::ComPtr<ID3D11Device> pDevice)
{
	try
	{
		// Allocate down scaled luminance buffer
		HRESULT _hr;
		D3D11_BUFFER_DESC bufferDesc;
		ZeroMemory(&bufferDesc, sizeof(bufferDesc));
		bufferDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
		bufferDesc.StructureByteStride = sizeof(float);
		bufferDesc.ByteWidth = m_DownScaleGroups * bufferDesc.StructureByteStride;
		bufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
		_hr = pDevice->CreateBuffer(&bufferDesc, NULL, m_pPrevAverageLuminanceBuffer.GetAddressOf());

		COM_ERROR_IF_FAILED(_hr, "ComputeShaderBuffer CreateBuffer Fail");

		D3D11_UNORDERED_ACCESS_VIEW_DESC DescUAV;
		ZeroMemory(&DescUAV, sizeof(D3D11_UNORDERED_ACCESS_VIEW_DESC));
		DescUAV.Format = DXGI_FORMAT_UNKNOWN;
		DescUAV.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
		DescUAV.Buffer.NumElements = 1;
		_hr = pDevice->CreateUnorderedAccessView(m_pPrevAverageLuminanceBuffer.Get(), &DescUAV, m_pPrevAverageLuminanceUAV.GetAddressOf());

		COM_ERROR_IF_FAILED(_hr, "ComputeShaderBuffer CreateUnorderedAccessView Fail");

		D3D11_SHADER_RESOURCE_VIEW_DESC dsrvd;
		ZeroMemory(&dsrvd, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
		dsrvd.Format = DXGI_FORMAT_UNKNOWN;
		dsrvd.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
		dsrvd.Buffer.NumElements = 1;
		_hr = pDevice->CreateShaderResourceView(m_pPrevAverageLuminanceBuffer.Get(), &dsrvd, m_pPrevAverageLuminanceSRV.GetAddressOf());

		COM_ERROR_IF_FAILED(_hr, "ComputeShaderBuffer CreateShaderResourceView Fail");

		// 부모 클래스에서 생성되므로 날리고 다시 만들어준다
		m_pDownScaleCB.Reset();

		ZeroMemory(&bufferDesc, sizeof(bufferDesc));
		bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		bufferDesc.ByteWidth = sizeof(CB_CS_Adaptation);
		_hr = pDevice->CreateBuffer(&bufferDesc, NULL, m_pDownScaleCB.GetAddressOf());

		COM_ERROR_IF_FAILED(_hr, "ComputeShaderBuffer CreateBuffer Fail");
	}
	catch (COMException& e)
	{
		ErrorLogger::Log(e);
	}
}

void Adaptation::ApplyFactor(const IRenderOption& option)
{
	m_White = option.hdrWhite;
	m_MiddleGrey = option.hdrMiddleGrey;
	m_MiddleGrey *= 0.1f;

	float fAdaptationNorm;
	static bool s_bFirstTime = true;
	if (s_bFirstTime)
	{
		// On the first frame we want to fully adapt the new value so use 0
		fAdaptationNorm = 0.0f;
		s_bFirstTime = false;
	}
	else
	{
		// Normalize the adaptation time with the frame time (all in seconds)
		// Never use a value higher or equal to 1 since that means no adaptation at all (keeps the old value)
		fAdaptationNorm = min(option.adaptation < 0.0001f ? 1.0f : option.dTime / option.adaptation, 0.9999f);
	}

	m_Adaptation = fAdaptationNorm;
}

void Adaptation::Apply(
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext, 
	std::shared_ptr<RenderTargetView> pRenderTarget, 
	std::shared_ptr<RenderTargetView> pSwapChainRenderTarget)
{
	// 기본적인 포스트 프로세싱 적용을 마친다
	HighDynamicRangeRender::Apply(pDeviceContext, pRenderTarget, pSwapChainRenderTarget);

	// 이전 프레임과 계산하기 위해서 임시로 옮겨주는 작업을 겹친다
	Microsoft::WRL::ComPtr<ID3D11Buffer> pTempBuffer = m_pAverageLuminanceBuffer;
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> pTempUAV = m_pAverageLuminanceDUAV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pTempSRV = m_pAverageLuminanceDSRV;
	m_pAverageLuminanceBuffer = m_pPrevAverageLuminanceBuffer;
	m_pAverageLuminanceDUAV = m_pPrevAverageLuminanceUAV;
	m_pAverageLuminanceDSRV = m_pPrevAverageLuminanceSRV;
	m_pPrevAverageLuminanceBuffer = pTempBuffer;
	m_pPrevAverageLuminanceUAV = pTempUAV;
	m_pPrevAverageLuminanceSRV = pTempSRV;
}

void Adaptation::Apply(
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext, 
	std::shared_ptr<RenderTargetView> pRenderTarget, RenderTargetView* pSwapChainRenderTarget)
{
	HighDynamicRangeRender::Apply(pDeviceContext, pRenderTarget, pSwapChainRenderTarget);

	// 이전 프레임과 계산하기 위해서 임시로 옮겨주는 작업을 겹친다
	Microsoft::WRL::ComPtr<ID3D11Buffer> pTempBuffer = m_pAverageLuminanceBuffer;
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> pTempUAV = m_pAverageLuminanceDUAV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pTempSRV = m_pAverageLuminanceDSRV;
	m_pAverageLuminanceBuffer = m_pPrevAverageLuminanceBuffer;
	m_pAverageLuminanceDUAV = m_pPrevAverageLuminanceUAV;
	m_pAverageLuminanceDSRV = m_pPrevAverageLuminanceSRV;
	m_pPrevAverageLuminanceBuffer = pTempBuffer;
	m_pPrevAverageLuminanceUAV = pTempUAV;
	m_pPrevAverageLuminanceSRV = pTempSRV;
}

void Adaptation::OnResize(Microsoft::WRL::ComPtr<ID3D11Device> pDevice, UINT width, UINT height)
{
	HighDynamicRangeRender::OnResize(pDevice, width, height);

	{
		// 추가 생성된 버퍼를 날리고 다시 만든다
		CreateBuffer(pDevice);
		CreateExtraBuffer(pDevice);
	}

}

void Adaptation::CreateShader(Microsoft::WRL::ComPtr<ID3D11Device> pDevice)
{
	m_HDRFirstPassShader = std::make_shared<ComputeShader>();
	m_HDRFirstPassShader->Initialize(pDevice, L"../Data/Shader/cs_adaption.hlsl", "DownScaleFirstPass", "cs_5_0");

	m_HDRSecondPassShader = std::make_shared<ComputeShader>();
	m_HDRSecondPassShader->Initialize(pDevice, L"../Data/Shader/cs_adaption.hlsl", "DownScaleSecondPass", "cs_5_0");
}

void Adaptation::DownScale(Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pShaderResource)
{
	// Output
	ID3D11UnorderedAccessView* arrUAVs[1] = { m_pDownScale1DUAV.Get() };
	pDeviceContext->CSSetUnorderedAccessViews(0, 1, arrUAVs, (UINT*)(&arrUAVs));

	// Input
	ID3D11ShaderResourceView* arrViews[2] = { pShaderResource.Get(), nullptr };
	pDeviceContext->CSSetShaderResources(0, 1, arrViews);

	// Constants
	D3D11_MAPPED_SUBRESOURCE MappedResource;
	pDeviceContext->Map(m_pDownScaleCB.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource);

	CB_CS_Adaptation* pDownScale = (CB_CS_Adaptation*)MappedResource.pData;
	{
		// HDR 렌더 톤매핑을 위한 팩터
		pDownScale->nWidth = m_Width / 4;
		pDownScale->nHeight = m_Height / 4;
		pDownScale->nTotalPixels = pDownScale->nWidth * pDownScale->nHeight;
		pDownScale->nGroupSize = m_DownScaleGroups;
		// 추가로 적응시를 위한 팩터
		pDownScale->fAdaptation = m_Adaptation;
	}


	pDeviceContext->Unmap(m_pDownScaleCB.Get(), 0);

	ID3D11Buffer* arrConstBuffers[1] = { m_pDownScaleCB.Get() };
	pDeviceContext->CSSetConstantBuffers(0, 1, arrConstBuffers);


	// Shader
	pDeviceContext->CSSetShader(m_HDRFirstPassShader->GetComputeShader(), NULL, 0);

	// Execute the downscales first pass with enough groups to cover the entire full res HDR buffer
	pDeviceContext->Dispatch(m_DownScaleGroups, 1, 1);

	//////////////////////////////////////////////////////////////////////////////////////////////////////
	// Second pass - reduce to a single pixel

	// Outoput
	arrUAVs[0] = m_pAverageLuminanceDUAV.Get();
	pDeviceContext->CSSetUnorderedAccessViews(0, 1, arrUAVs, (UINT*)(&arrUAVs));

	// Input
	arrViews[0] = m_pDownScale1DSRV.Get();
	arrViews[1] = m_pPrevAverageLuminanceSRV.Get();
	pDeviceContext->CSSetShaderResources(0, 2, arrViews);

	// Constants
	pDeviceContext->CSSetConstantBuffers(0, 1, arrConstBuffers);

	// Shader
	pDeviceContext->CSSetShader(m_HDRSecondPassShader->GetComputeShader(), NULL, 0);

	// Excute with a single group - this group has enough threads to process all the pixels
	pDeviceContext->Dispatch(1, 1, 1);

	// Cleanup
	pDeviceContext->CSSetShader(NULL, NULL, 0);
	ZeroMemory(arrConstBuffers, sizeof(arrConstBuffers));
	pDeviceContext->CSSetConstantBuffers(0, 1, arrConstBuffers);
	ZeroMemory(arrViews, sizeof(arrViews));
	pDeviceContext->CSSetShaderResources(0, 2, arrViews);
	ZeroMemory(arrUAVs, sizeof(arrUAVs));
	pDeviceContext->CSSetUnorderedAccessViews(0, 1, arrUAVs, (UINT*)(&arrUAVs));
}

void Adaptation::FinalPass(Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pShaderResource)
{
	ID3D11ShaderResourceView* arrViews[2] = { pShaderResource.Get(), m_pAverageLuminanceDSRV.Get() };
	pDeviceContext->PSSetShaderResources(0, 2, arrViews);

	// Constants
	D3D11_MAPPED_SUBRESOURCE MappedResource;
	pDeviceContext->Map(m_pFinalPassCB.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource);
	CB_SC_FinalPass* pFinalPass = (CB_SC_FinalPass*)MappedResource.pData;
	pFinalPass->fMiddleGrey = m_MiddleGrey;
	pFinalPass->fLumWhiteSqr = m_White;
	pFinalPass->fLumWhiteSqr *= pFinalPass->fMiddleGrey; // Scale by the middle grey value
	pFinalPass->fLumWhiteSqr *= pFinalPass->fLumWhiteSqr; // Squre

	pDeviceContext->Unmap(m_pFinalPassCB.Get(), 0);
	ID3D11Buffer* arrConstBuffers[1] = { m_pFinalPassCB.Get() };
	pDeviceContext->PSSetConstantBuffers(0, 1, arrConstBuffers);

	pDeviceContext->IASetInputLayout(NULL);
	pDeviceContext->IASetVertexBuffers(0, 0, NULL, NULL, NULL);
	pDeviceContext->IASetIndexBuffer(NULL, DXGI_FORMAT_UNKNOWN, 0);
	pDeviceContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	ID3D11SamplerState* arrSamplers[1] = { RasterizerState::GetPointSamplerState() };
	pDeviceContext->PSSetSamplers(0, 1, arrSamplers);

	// Set the shaders
	pDeviceContext->VSSetShader(m_pFullScreenQuadVS->GetVertexShader(), NULL, 0);
	pDeviceContext->PSSetShader(m_pFinalPassPS->GetPixelShader(), NULL, 0);

	pDeviceContext->Draw(4, 0);

	// Cleanup
	ZeroMemory(arrViews, sizeof(arrViews));
	pDeviceContext->PSSetShaderResources(0, 2, arrViews);
	ZeroMemory(arrConstBuffers, sizeof(arrConstBuffers));
	pDeviceContext->PSSetConstantBuffers(0, 1, arrConstBuffers);
	pDeviceContext->VSSetShader(NULL, NULL, 0);
	pDeviceContext->PSSetShader(NULL, NULL, 0);
}

