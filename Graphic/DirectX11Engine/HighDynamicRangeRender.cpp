#include "HighDynamicRangeRender.h"

#include "Shaders.h"
#include "RenderTargetView.h"

#include "PostProcessCB.h"
#include "RasterizerState.h"

HighDynamicRangeRender::HighDynamicRangeRender()
	: m_Width(0)
	, m_Height(0)
	, m_DownScaleGroups(0)
	, m_MiddleGrey(0.0f)
	, m_White(0.0f)
	, m_HDRFirstPassShader(nullptr)
	, m_HDRSecondPassShader(nullptr)
	, m_pDownScale1DBuffer(nullptr)
	, m_pDownScale1DUAV(nullptr)
	, m_pDownScale1DSRV(nullptr)
	, m_pAverageLuminanceBuffer(nullptr)
	, m_pAverageLuminanceDUAV(nullptr)
	, m_pAverageLuminanceDSRV(nullptr)
	, m_pDownScaleCB(nullptr)
	, m_pFinalPassCB(nullptr)
{

}

HighDynamicRangeRender::~HighDynamicRangeRender()
{

}

void HighDynamicRangeRender::Initialize(
	Microsoft::WRL::ComPtr<ID3D11Device> pDevice, UINT width, UINT height)
{
	m_Width = width;
	m_Height = height;

	CreateBuffer(pDevice);
	CreateShader(pDevice);
	CreateBaseShader(pDevice);
}

void HighDynamicRangeRender::ApplyFactor(const IRenderOption& option)
{
	m_White = option.hdrWhite;
	m_MiddleGrey = option.hdrMiddleGrey;
	m_MiddleGrey *= 0.1f;
}

void HighDynamicRangeRender::Apply(
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext,
	std::shared_ptr<RenderTargetView> pRenderTarget,
	std::shared_ptr<RenderTargetView> pSwapChainRenderTarget)
{
	// Down scale the HDR image
	ID3D11RenderTargetView* rt[1] = { NULL };
	pDeviceContext->OMSetRenderTargets(1, rt, NULL);
	DownScale(pDeviceContext, pRenderTarget->GetShaderResourceViewRawptr());

	// Do the final pass
	rt[0] = pSwapChainRenderTarget->GetRenderTargerViewRawptr();
	pDeviceContext->OMSetRenderTargets(1, rt, NULL);
	FinalPass(pDeviceContext, pRenderTarget->GetShaderResourceViewRawptr());
}

void HighDynamicRangeRender::Apply(
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext, 
	std::shared_ptr<RenderTargetView> pRenderTarget, RenderTargetView* pSwapChainRenderTarget)
{
	ID3D11RenderTargetView* rt[1] = { NULL };
	pDeviceContext->OMSetRenderTargets(1, rt, NULL);
	DownScale(pDeviceContext, pRenderTarget->GetShaderResourceViewRawptr());

	rt[0] = pSwapChainRenderTarget->GetRenderTargerViewRawptr();
	pDeviceContext->OMSetRenderTargets(1, rt, NULL);
	FinalPass(pDeviceContext, pRenderTarget->GetShaderResourceViewRawptr());
}

void HighDynamicRangeRender::OnResize(
	Microsoft::WRL::ComPtr<ID3D11Device> pDevice, UINT width, UINT height)
{
	// ���̴��� ���� �ʿ䰡 ����
	// ���� ������ ������
	{
		m_pDownScale1DBuffer.Reset();
		m_pDownScale1DUAV.Reset();
		m_pDownScale1DSRV.Reset();

		m_pAverageLuminanceBuffer.Reset();
		m_pAverageLuminanceDUAV.Reset();
		m_pAverageLuminanceDSRV.Reset();

		m_pDownScaleCB.Reset();
		m_pFinalPassCB.Reset();
	}
	
	// ũ�⸦ ������ ���¿��� ���۸� ������Ѵ�
	m_Width = width; m_Height = height;
	CreateBuffer(pDevice);
}

void HighDynamicRangeRender::CreateBuffer(Microsoft::WRL::ComPtr<ID3D11Device> pDevice)
{
	HRESULT _hr;

	m_DownScaleGroups = (UINT)ceil((float)(m_Width * m_Height / 16) / 1024.0f);

	/// ���� ���۸� �ϳ� ������ش�.
	/// �� ���۴� �ֵ� ���� ����� �� �߰� ���� �����ϴ� ������ �Ѵ�.
	/// ������ �޸� ũ��� �� ������ �ػ󵵿� ���ؼ� �����ȴ�.

	try
	{
		D3D11_BUFFER_DESC bufferDesc;
		ZeroMemory(&bufferDesc, sizeof(bufferDesc));
		bufferDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
		bufferDesc.StructureByteStride = sizeof(float);
		bufferDesc.ByteWidth = m_DownScaleGroups * bufferDesc.StructureByteStride;	
		bufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
		_hr = pDevice->CreateBuffer(&bufferDesc, NULL, m_pDownScale1DBuffer.GetAddressOf());

		COM_ERROR_IF_FAILED(_hr, "ComputeShaderBuffer CreateBuffer Fail");


		/// �� ���ۿ� �����͸� �а� �� �� �ְ� �Ѵ�.
		/// ���⿡ ���Ǵ°� UnorderedAccessView�� ShaderResourceView
		/// , ���� ���� ���⿡ ����Ǵ� ���� �ƴ϶� ���� �������
		/// 
		D3D11_UNORDERED_ACCESS_VIEW_DESC DescUAV;
		ZeroMemory(&DescUAV, sizeof(D3D11_UNORDERED_ACCESS_VIEW_DESC));
		DescUAV.Format = DXGI_FORMAT_UNKNOWN;
		DescUAV.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
		DescUAV.Buffer.NumElements = m_DownScaleGroups;
		_hr = pDevice->CreateUnorderedAccessView(m_pDownScale1DBuffer.Get(), &DescUAV, m_pDownScale1DUAV.GetAddressOf());

		COM_ERROR_IF_FAILED(_hr, "ComputeShaderBuffer CreateUnorderedAccessView Fail");

		D3D11_SHADER_RESOURCE_VIEW_DESC dsrvd;
		ZeroMemory(&dsrvd, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
		dsrvd.Format = DXGI_FORMAT_UNKNOWN;
		dsrvd.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
		dsrvd.Buffer.NumElements = m_DownScaleGroups;
		_hr = pDevice->CreateShaderResourceView(m_pDownScale1DBuffer.Get(), &dsrvd, m_pDownScale1DSRV.GetAddressOf());

		COM_ERROR_IF_FAILED(_hr, "ComputeShaderBuffer CreateShaderResourceView Fail");

		/// �̾ �� ��° ���۸� �����Ѵ�
		/// �ε��Ҽ��� ���·� ��� �ֵ����� �������ش�
		/// �� ���� ��ũ���ʹ� ó�� ���ۿ� ���� ���������� ũ�⿡�� ���̸� ���δ�.

		bufferDesc.ByteWidth = sizeof(float);	// �������� ByteWidth�� 4 * m_DownScaleGroups ����.
		_hr = pDevice->CreateBuffer(&bufferDesc, NULL, m_pAverageLuminanceBuffer.GetAddressOf());

		COM_ERROR_IF_FAILED(_hr, "ComputeShaderBuffer CreateBuffer Fail");


		DescUAV.Buffer.NumElements = 1;	// ���������� ù��° ������ UAV, SRV�� m_DownScaleGroups ����.
		_hr = pDevice->CreateUnorderedAccessView(m_pAverageLuminanceBuffer.Get(), &DescUAV, m_pAverageLuminanceDUAV.GetAddressOf());

		COM_ERROR_IF_FAILED(_hr, "ComputeShaderBuffer CreateUnorderedAccessView Fail");

		dsrvd.Buffer.NumElements = 1;
		_hr = pDevice->CreateShaderResourceView(m_pAverageLuminanceBuffer.Get(), &dsrvd, m_pAverageLuminanceDSRV.GetAddressOf());

		COM_ERROR_IF_FAILED(_hr, "ComputeShaderBuffer CreateShaderResourceView Fail");


		/// ���������� ���� 16����Ʈ ũ��� �� ���� ��� ���۸� �����Ѵ�

		ZeroMemory(&bufferDesc, sizeof(bufferDesc));
		bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		bufferDesc.ByteWidth = sizeof(CB_CS_DownScale);
		_hr = pDevice->CreateBuffer(&bufferDesc, NULL, m_pDownScaleCB.GetAddressOf());

		COM_ERROR_IF_FAILED(_hr, "ComputeShaderBuffer CreateBuffer Fail");

		bufferDesc.ByteWidth = sizeof(CB_SC_FinalPass);
		_hr = pDevice->CreateBuffer(&bufferDesc, NULL, m_pFinalPassCB.GetAddressOf());

		COM_ERROR_IF_FAILED(_hr, "ComputeShaderBuffer CreateBuffer Fail");

	}
	catch (COMException& e)
	{
		ErrorLogger::Log(e);
	}
}

void HighDynamicRangeRender::CreateShader(Microsoft::WRL::ComPtr<ID3D11Device> pDevice)
{
	m_HDRFirstPassShader = std::make_shared<ComputeShader>();
	m_HDRFirstPassShader->Initialize(pDevice, L"../Data/Shader/cs_HDR.hlsl", "DownScaleFirstPass", "cs_5_0");

	m_HDRSecondPassShader = std::make_shared<ComputeShader>();
	m_HDRSecondPassShader->Initialize(pDevice, L"../Data/Shader/cs_HDR.hlsl", "DownScaleSecondPass", "cs_5_0");
}

void HighDynamicRangeRender::DownScale(
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext,
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pShaderResource)
{
	// Output
	ID3D11UnorderedAccessView* arrUAVs[1] = { m_pDownScale1DUAV.Get() };
	pDeviceContext->CSSetUnorderedAccessViews(0, 1, arrUAVs, (UINT*)(&arrUAVs));

	// Input
	ID3D11ShaderResourceView* arrViews[1] = { pShaderResource.Get() };
	pDeviceContext->CSSetShaderResources(0, 1, arrViews);

	// Constants
	D3D11_MAPPED_SUBRESOURCE MappedResource;
	pDeviceContext->Map(m_pDownScaleCB.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource);

	CB_CS_DownScale* pDownScale = (CB_CS_DownScale*)MappedResource.pData;
	{
		// HDR ���� ������� ���� ����
		pDownScale->nWidth = m_Width / 4;
		pDownScale->nHeight = m_Height / 4;
		pDownScale->nTotalPixels = pDownScale->nWidth * pDownScale->nHeight;
		pDownScale->nGroupSize = m_DownScaleGroups;
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
	ZeroMemory(arrUAVs, sizeof(arrUAVs));
	arrUAVs[0] = m_pAverageLuminanceDUAV.Get();
	pDeviceContext->CSSetUnorderedAccessViews(0, 1, arrUAVs, (UINT*)(&arrUAVs));

	// Input
	arrViews[0] = m_pDownScale1DSRV.Get();
	pDeviceContext->CSSetShaderResources(0, 1, arrViews);

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
	pDeviceContext->CSSetShaderResources(0, 1, arrViews);
	ZeroMemory(arrUAVs, sizeof(arrUAVs));
	pDeviceContext->CSSetUnorderedAccessViews(0, 1, arrUAVs, (UINT*)(&arrUAVs));

	ID3D11ShaderResourceView* nullSRV[16] = { 0 };
	pDeviceContext->PSSetShaderResources(0, 16, nullSRV);

}

void HighDynamicRangeRender::FinalPass(
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext,
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pShaderResource)
{
	ID3D11ShaderResourceView* arrViews[2] = { pShaderResource.Get(), m_pAverageLuminanceDSRV.Get() };
	pDeviceContext->PSSetShaderResources(0, 1, &arrViews[0]);
	pDeviceContext->PSSetShaderResources(1, 1, &arrViews[1]);

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
