#include "HLSLBloom.h"

#include "Shaders.h"

#include "PostProcessCB.h"
#include "RasterizerState.h"

#include "ErrorLogger.h"
#include "COMException.h"

HLSLBloom::HLSLBloom()
{

}

HLSLBloom::~HLSLBloom()
{

}

void HLSLBloom::Initialize(Microsoft::WRL::ComPtr<ID3D11Device> pDevice, UINT width, UINT height)
{
	// Find the amount of thread groups needed for the downscale operation
	m_Width = width;
	m_Height = height;
	m_DownScaleGroups = (UINT)ceil((float)(m_Width * m_Height / 16) / 1024.0f);

	CreateBuffer(pDevice);
	CreateRecycleBuffer(pDevice);
	CreateShader(pDevice);
	CreateBaseShader(pDevice);
}

void HLSLBloom::OnResize(Microsoft::WRL::ComPtr<ID3D11Device> pDevice, UINT width, UINT height)
{
	m_Width = width;
	m_Height = height;
	m_DownScaleGroups = (UINT)ceil((float)(m_Width * m_Height / 16) / 1024.0f);

	CreateBuffer(pDevice);
	CreateRecycleBuffer(pDevice);
}

void HLSLBloom::BindConstantBuffer(Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext)
{
	// Constants
	D3D11_MAPPED_SUBRESOURCE MappedResource;
	pDeviceContext->Map(m_pDownScaleCB.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource);

	CB_CS_Bloom* pDownScale = (CB_CS_Bloom*)MappedResource.pData;
	pDownScale->nWidth = m_Width / 4;
	pDownScale->nHeight = m_Height / 4;
	pDownScale->nTotalPixels = pDownScale->nWidth * pDownScale->nHeight;
	pDownScale->nGroupSize = m_DownScaleGroups;
	pDownScale->fAdaptation = m_Adaption;
	pDownScale->fBloomThreshold = m_BloomThreshold;
	pDeviceContext->Unmap(m_pDownScaleCB.Get(), 0);
	ID3D11Buffer* arrConstBuffers[1] = { m_pDownScaleCB.Get() };
	pDeviceContext->CSSetConstantBuffers(0, 1, arrConstBuffers);
}

void HLSLBloom::ClearBuffer(Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext)
{
	// Cleanup
	ID3D11Buffer* arrConstBuffers[1];
	ZeroMemory(&arrConstBuffers, sizeof(arrConstBuffers));
	pDeviceContext->CSSetConstantBuffers(0, 1, arrConstBuffers);
}

void HLSLBloom::DownScale(
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext,
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pSRV)
{
	// Output
	ID3D11UnorderedAccessView* arrUAVs[2] = { m_pDownScale1DUAV.Get(), m_pDownScaleUAV.Get() };
	pDeviceContext->CSSetUnorderedAccessViews(0, 2, arrUAVs, NULL);

	// Input
	ID3D11ShaderResourceView* arrViews[2] = { pSRV.Get(), NULL };
	pDeviceContext->CSSetShaderResources(0, 1, arrViews);

	// Shader
	pDeviceContext->CSSetShader(m_DownFirstScale->GetComputeShader(), NULL, 0);

	// Execute the downscales first pass with enough groups to cover the entire full res HDR buffer
	pDeviceContext->Dispatch(m_DownScaleGroups, 1, 1);

	//////////////////////////////////////////////////////////////////////////////////////////////////////
	// Second pass - reduce to a single pixel

	// Outoput
	ZeroMemory(arrUAVs, sizeof(arrUAVs));
	arrUAVs[0] = m_pAvgLumUAV.Get();
	pDeviceContext->CSSetUnorderedAccessViews(0, 2, arrUAVs, NULL);

	// Input
	arrViews[0] = m_pDownScale1DSRV.Get();
	arrViews[1] = m_pPrevAvgLumSRV.Get();
	pDeviceContext->CSSetShaderResources(0, 2, arrViews);

	// Shader
	pDeviceContext->CSSetShader(m_DownSecondScale->GetComputeShader(), NULL, 0);

	// Excute with a single group - this group has enough threads to process all the pixels
	pDeviceContext->Dispatch(1, 1, 1);

	// Cleanup
	pDeviceContext->CSSetShader(NULL, NULL, 0);
	ZeroMemory(arrViews, sizeof(arrViews));
	pDeviceContext->CSSetShaderResources(0, 2, arrViews);
	ZeroMemory(arrUAVs, sizeof(arrUAVs));
	pDeviceContext->CSSetUnorderedAccessViews(0, 2, arrUAVs, (UINT*)(&arrUAVs));
}

void HLSLBloom::Bloom(Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext)
{
	// Input
	ID3D11ShaderResourceView* arrViews[2] = { m_pDownScaleSRV.Get(), m_pAvgLumSRV.Get() };
	pDeviceContext->CSSetShaderResources(0, 2, arrViews);

	// Output
	ID3D11UnorderedAccessView* arrUAVs[1] = { m_pTempUAV[0].Get() };
	pDeviceContext->CSSetUnorderedAccessViews(0, 1, arrUAVs, NULL);

	// Shader
	pDeviceContext->CSSetShader(m_BloomShader->GetComputeShader(), NULL, 0);

	// Execute the downscales first pass with enough groups to cover the entire full res HDR buffer
	pDeviceContext->Dispatch(m_DownScaleGroups, 1, 1);

	// Cleanup
	pDeviceContext->CSSetShader(NULL, NULL, 0);
	ZeroMemory(arrViews, sizeof(arrViews));
	pDeviceContext->CSSetShaderResources(0, 2, arrViews);
	ZeroMemory(arrUAVs, sizeof(arrUAVs));
	pDeviceContext->CSSetUnorderedAccessViews(0, 1, arrUAVs, NULL);
}

void HLSLBloom::Blur(
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext,
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pInput,
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> pOutput)
{

	//////////////////////////////////////////////////////////////////////////////////////////////////////
	// Second pass - horizontal gaussian filter

	// Output
	ID3D11UnorderedAccessView* arrUAVs[1] = { m_pTempUAV[1].Get() };
	pDeviceContext->CSSetUnorderedAccessViews(0, 1, arrUAVs, NULL);

	// Input
	ID3D11ShaderResourceView* arrViews[1] = { pInput.Get() };
	pDeviceContext->CSSetShaderResources(0, 1, arrViews);

	// Shader
	pDeviceContext->CSSetShader(m_HorizontalBlur->GetComputeShader(), NULL, 0);

	// Execute the horizontal filter
	pDeviceContext->Dispatch((UINT)ceil((m_Width / 4.0f) / (128.0f - 12.0f)), (UINT)ceil(m_Height / 4.0f), 1);

	//////////////////////////////////////////////////////////////////////////////////////////////////////
	// First pass - vertical gaussian filter

	// Output
	arrUAVs[0] = pOutput.Get();
	pDeviceContext->CSSetUnorderedAccessViews(0, 1, arrUAVs, NULL);

	// Input
	arrViews[0] = m_pTempSRV[1].Get();
	pDeviceContext->CSSetShaderResources(0, 1, arrViews);

	// Shader
	pDeviceContext->CSSetShader(m_VerticalBlur->GetComputeShader(), NULL, 0);

	// Execute the vertical filter
	pDeviceContext->Dispatch((UINT)ceil(m_Width / 4.0f), (UINT)ceil((m_Height / 4.0f) / (128.0f - 12.0f)), 1);

	// Cleanup
	pDeviceContext->CSSetShader(NULL, NULL, 0);
	ZeroMemory(arrViews, sizeof(arrViews));
	pDeviceContext->CSSetShaderResources(0, 1, arrViews);
	ZeroMemory(arrUAVs, sizeof(arrUAVs));
	pDeviceContext->CSSetUnorderedAccessViews(0, 1, arrUAVs, NULL);
}

void HLSLBloom::Blur(
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext)
{
	//////////////////////////////////////////////////////////////////////////////////////////////////////
	// Second pass - horizontal gaussian filter

	// Output
	ID3D11UnorderedAccessView* arrUAVs[1] = { m_pTempUAV[1].Get() };
	pDeviceContext->CSSetUnorderedAccessViews(0, 1, arrUAVs, NULL);

	// Input
	ID3D11ShaderResourceView* arrViews[1] = { m_pTempSRV[0].Get() };
	pDeviceContext->CSSetShaderResources(0, 1, arrViews);

	// Shader
	pDeviceContext->CSSetShader(m_HorizontalBlur->GetComputeShader(), NULL, 0);

	// Execute the horizontal filter
	pDeviceContext->Dispatch((UINT)ceil((m_Width / 4.0f) / (128.0f - 12.0f)), (UINT)ceil(m_Height / 4.0f), 1);

	//////////////////////////////////////////////////////////////////////////////////////////////////////
	// First pass - vertical gaussian filter

	// Output
	arrUAVs[0] = m_pBloomUAV.Get();
	pDeviceContext->CSSetUnorderedAccessViews(0, 1, arrUAVs, NULL);

	// Input
	arrViews[0] = m_pTempSRV[1].Get();
	pDeviceContext->CSSetShaderResources(0, 1, arrViews);

	// Shader
	pDeviceContext->CSSetShader(m_VerticalBlur->GetComputeShader(), NULL, 0);

	// Execute the vertical filter
	pDeviceContext->Dispatch((UINT)ceil(m_Width / 4.0f), (UINT)ceil((m_Height / 4.0f) / (128.0f - 12.0f)), 1);

	// Cleanup
	pDeviceContext->CSSetShader(NULL, NULL, 0);
	ZeroMemory(arrViews, sizeof(arrViews));
	pDeviceContext->CSSetShaderResources(0, 1, arrViews);
	ZeroMemory(arrUAVs, sizeof(arrUAVs));
	pDeviceContext->CSSetUnorderedAccessViews(0, 1, arrUAVs, NULL);
}

// 이 함수를 호출하기 전에
// 최종 출력을 원하는 렌더 타겟을 바인딩해야한다
void HLSLBloom::FinalPass(
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext,
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pHDRSRV)
{
	ID3D11ShaderResourceView* arrViews[3] = { pHDRSRV.Get(), m_pAvgLumSRV.Get(), m_pBloomSRV.Get() };
	pDeviceContext->PSSetShaderResources(0, 3, arrViews);

	// Constants
	D3D11_MAPPED_SUBRESOURCE MappedResource;
	pDeviceContext->Map(m_pFinalPassCB.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource);
	CB_SC_FinalPass* pFinalPass = (CB_SC_FinalPass*)MappedResource.pData;
	pFinalPass->fMiddleGrey = m_MiddleGrey;
	pFinalPass->fLumWhiteSqr = m_White;
	pFinalPass->fLumWhiteSqr *= pFinalPass->fMiddleGrey; // Scale by the middle grey value
	pFinalPass->fLumWhiteSqr *= pFinalPass->fLumWhiteSqr; // Square
	pFinalPass->fBloomScale = m_BloomScale;
	pDeviceContext->Unmap(m_pFinalPassCB.Get(), 0);
	ID3D11Buffer* arrConstBuffers[1] = { m_pFinalPassCB.Get() };
	pDeviceContext->PSSetConstantBuffers(0, 1, arrConstBuffers);

	pDeviceContext->IASetInputLayout(NULL);
	pDeviceContext->IASetVertexBuffers(0, 0, NULL, NULL, NULL);
	pDeviceContext->IASetIndexBuffer(NULL, DXGI_FORMAT_UNKNOWN, 0);
	pDeviceContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	ID3D11SamplerState* arrSamplers[2] =
	{
		RasterizerState::GetPointSamplerState(), RasterizerState::GetLinearSamplerState()
	};
	pDeviceContext->PSSetSamplers(0, 2, arrSamplers);

	// Set the shaders
	pDeviceContext->VSSetShader(m_pFullScreenQuadVS->GetVertexShader(), NULL, 0);
	pDeviceContext->PSSetShader(m_pFinalPassPS->GetPixelShader(), NULL, 0);

	pDeviceContext->Draw(4, 0);

	// Cleanup
	ZeroMemory(arrViews, sizeof(arrViews));
	pDeviceContext->PSSetShaderResources(0, 3, arrViews);
	ZeroMemory(arrConstBuffers, sizeof(arrConstBuffers));
	pDeviceContext->PSSetConstantBuffers(0, 1, arrConstBuffers);
	pDeviceContext->VSSetShader(NULL, NULL, 0);
	pDeviceContext->PSSetShader(NULL, NULL, 0);
}

void HLSLBloom::BufferSwap()
{
	// Swap the previous frame average luminance
	Microsoft::WRL::ComPtr<ID3D11Buffer> pTempBuffer = m_pAvgLumBuffer;
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> pTempUAV = m_pAvgLumUAV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> p_TempSRV = m_pAvgLumSRV;
	m_pAvgLumBuffer = m_pPrevAvgLumBuffer;
	m_pAvgLumUAV = m_pPrevAvgLumUAV;
	m_pAvgLumSRV = m_pPrevAvgLumSRV;
	m_pPrevAvgLumBuffer = pTempBuffer;
	m_pPrevAvgLumUAV = pTempUAV;
	m_pPrevAvgLumSRV = p_TempSRV;
}

void HLSLBloom::ApplyFactor(const IRenderOption& option)
{
	m_White = option.hdrWhite;
	m_MiddleGrey = option.hdrMiddleGrey;
	//m_BloomScale = option.bloomScale;
	m_BloomThreshold = option.bloomThreshold;
	m_Adaption = option.adaptation;
}

void HLSLBloom::CreateBuffer(Microsoft::WRL::ComPtr<ID3D11Device> pDevice)
{


	m_pDownScaleRT.Reset();
	m_pDownScaleSRV.Reset();
	m_pDownScaleUAV.Reset();

	// Temporary texture
	m_pTempRT[0].Reset();
	m_pTempSRV[0].Reset();
	m_pTempUAV[0].Reset();
	m_pTempRT[1].Reset();
	m_pTempSRV[1].Reset();
	m_pTempUAV[1].Reset();

	// Bloom texture
	m_pBloomRT.Reset();
	m_pBloomSRV.Reset();
	m_pBloomUAV.Reset();

	// 1D intermediate storage for the down scale operation
	m_pDownScale1DBuffer.Reset();
	m_pDownScale1DUAV.Reset();
	m_pDownScale1DSRV.Reset();

	// Average luminance
	m_pAvgLumBuffer.Reset();
	m_pAvgLumUAV.Reset();
	m_pAvgLumSRV.Reset();

	// Previous average luminance for adaptation
	m_pPrevAvgLumBuffer.Reset();
	m_pPrevAvgLumUAV.Reset();
	m_pPrevAvgLumSRV.Reset();

	m_pDownScaleCB.Reset();
	m_pFinalPassCB.Reset();
	m_pBlurCB.Reset();


	//////////////////////////////////////////////////////////////////////////////////////////////////////
	// Allocate the downscaled target
	D3D11_TEXTURE2D_DESC dtd = {
		m_Width / 4, //UINT Width;
		m_Height / 4, //UINT Height;
		1, //UINT MipLevels;
		1, //UINT ArraySize;
		DXGI_FORMAT_R16G16B16A16_TYPELESS, //DXGI_FORMAT Format;
		1, //DXGI_SAMPLE_DESC SampleDesc;
		0,
		D3D11_USAGE_DEFAULT,//D3D11_USAGE Usage;
		D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS,//UINT BindFlags;
		0,//UINT CPUAccessFlags;
		0//UINT MiscFlags;    
	};

	try
	{
		HRESULT _hr;
		_hr = pDevice->CreateTexture2D(&dtd, NULL, m_pDownScaleRT.GetAddressOf());

		COM_ERROR_IF_FAILED(_hr, "CreateTexture2D Fail");

		// Create the resource views
		D3D11_SHADER_RESOURCE_VIEW_DESC dsrvd;
		ZeroMemory(&dsrvd, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
		dsrvd.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
		dsrvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		dsrvd.Texture2D.MipLevels = 1;
		_hr = pDevice->CreateShaderResourceView(m_pDownScaleRT.Get(), &dsrvd, m_pDownScaleSRV.GetAddressOf());

		COM_ERROR_IF_FAILED(_hr, "CreateShaderResourceView Fail");

		//////////////////////////////////////////////////////////////////////////////////////////////////////
		// Allocate temporary target
		_hr = pDevice->CreateTexture2D(&dtd, NULL, m_pTempRT[0].GetAddressOf());
		COM_ERROR_IF_FAILED(_hr, "HLSLBloom CreateTeuxte2D m_pTempRT[0] Fail");

		_hr = pDevice->CreateTexture2D(&dtd, NULL, m_pTempRT[1].GetAddressOf());
		COM_ERROR_IF_FAILED(_hr, "HLSLBloom CreateTeuxte2D m_pTempRT[1] Fail");

		//////////////////////////////////////////////////////////////////////////////////////////////////////
		// Allocate bloom target
		_hr = pDevice->CreateTexture2D(&dtd, NULL, m_pBloomRT.GetAddressOf());
		COM_ERROR_IF_FAILED(_hr, "HLSLBloom CreateUnorderedAccessView m_pBloomRT Fail");

		//////////////////////////////////////////////////////////////////////////////////////////////////////
		// Allocate constant buffers
		/// 재활용 필수
		D3D11_BUFFER_DESC bufferDesc;
		ZeroMemory(&bufferDesc, sizeof(bufferDesc));
		bufferDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
		bufferDesc.StructureByteStride = sizeof(float);
		bufferDesc.ByteWidth = m_DownScaleGroups * bufferDesc.StructureByteStride;
		bufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
		bufferDesc.Usage = D3D11_USAGE_DEFAULT;

		ZeroMemory(&bufferDesc, sizeof(bufferDesc));
		bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		bufferDesc.ByteWidth = sizeof(CB_CS_Bloom);
		// 블룸용 다운 스케일 버퍼
		_hr = pDevice->CreateBuffer(&bufferDesc, NULL, m_pDownScaleCB.GetAddressOf());

		bufferDesc.ByteWidth = sizeof(CB_SC_FinalPass);
		_hr = pDevice->CreateBuffer(&bufferDesc, NULL, m_pFinalPassCB.GetAddressOf());

		bufferDesc.ByteWidth = sizeof(CB_CS_Blur);
		_hr = pDevice->CreateBuffer(&bufferDesc, NULL, m_pBlurCB.GetAddressOf());

	}
	catch (COMException& e)
	{
		ErrorLogger::Log(e);
	}
}

void HLSLBloom::CreateShader(Microsoft::WRL::ComPtr<ID3D11Device> pDevice)
{
	m_DownFirstScale = std::make_shared<ComputeShader>();
	m_DownFirstScale->Initialize(pDevice, L"../Data/Shader/cs_HLSLbloom.hlsl", "DownScaleFirstPass", "cs_5_0");

	m_DownSecondScale = std::make_shared<ComputeShader>();
	m_DownSecondScale->Initialize(pDevice, L"../Data/Shader/cs_HLSLbloom.hlsl", "DownScaleSecondPass", "cs_5_0");

	m_BloomShader = std::make_shared<ComputeShader>();
	m_BloomShader->Initialize(pDevice, L"../Data/Shader/cs_HLSLbloom.hlsl", "BloomReveal", "cs_5_0");

	m_HorizontalBlur = std::make_shared<ComputeShader>();
	m_HorizontalBlur->Initialize(pDevice, L"../Data/Shader/cs_NVidiaBlur.hlsl", "HorizFilter", "cs_5_0");

	m_VerticalBlur = std::make_shared<ComputeShader>();
	m_VerticalBlur->Initialize(pDevice, L"../Data/Shader/cs_NVidiaBlur.hlsl", "VerticalFilter", "cs_5_0");
}

void HLSLBloom::CreateRecycleBuffer(Microsoft::WRL::ComPtr<ID3D11Device> pDevice)
{
	//////////////////////////////////////////////////////////////////////////////////////////////////////
	// Allocate down scaled luminance buffer
	try
	{
		HRESULT _hr;

		/// 재활용 필수
		D3D11_BUFFER_DESC bufferDesc;
		ZeroMemory(&bufferDesc, sizeof(bufferDesc));
		bufferDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
		bufferDesc.StructureByteStride = sizeof(float);
		bufferDesc.ByteWidth = m_DownScaleGroups * bufferDesc.StructureByteStride;
		bufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
		bufferDesc.Usage = D3D11_USAGE_DEFAULT;
		_hr = pDevice->CreateBuffer(&bufferDesc, NULL, m_pDownScale1DBuffer.GetAddressOf());
		COM_ERROR_IF_FAILED(_hr, "HLSLBloom CreateBuffer m_pDownScale1DBuffer Fail");

		// Create the resource views
		D3D11_SHADER_RESOURCE_VIEW_DESC dsrvd;
		ZeroMemory(&dsrvd, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
		dsrvd.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
		dsrvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		dsrvd.Texture2D.MipLevels = 1;
		_hr = pDevice->CreateShaderResourceView(m_pDownScaleRT.Get(), &dsrvd, m_pDownScaleSRV.GetAddressOf());

		COM_ERROR_IF_FAILED(_hr, "CreateShaderResourceView Fail");


		// Create the UAVs
		D3D11_UNORDERED_ACCESS_VIEW_DESC DescUAV;
		ZeroMemory(&DescUAV, sizeof(D3D11_UNORDERED_ACCESS_VIEW_DESC));
		DescUAV.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
		DescUAV.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
		DescUAV.Buffer.FirstElement = 0;
		DescUAV.Buffer.NumElements = m_Width * m_Height / 16;
		_hr = pDevice->CreateUnorderedAccessView(m_pDownScaleRT.Get(), &DescUAV, m_pDownScaleUAV.GetAddressOf());
		COM_ERROR_IF_FAILED(_hr, "CreateUnorderedAccessView Fail");

		_hr = pDevice->CreateShaderResourceView(m_pTempRT[0].Get(), &dsrvd, m_pTempSRV[0].GetAddressOf());
		COM_ERROR_IF_FAILED(_hr, "CreateShaderResourceView Fail");

		_hr = pDevice->CreateUnorderedAccessView(m_pTempRT[0].Get(), &DescUAV, m_pTempUAV[0].GetAddressOf());
		COM_ERROR_IF_FAILED(_hr, "CreateUnorderedAccessView Fail");

		_hr = pDevice->CreateShaderResourceView(m_pTempRT[1].Get(), &dsrvd, m_pTempSRV[1].GetAddressOf());
		COM_ERROR_IF_FAILED(_hr, "HLSLBloom CreateShaderResourceView m_pTempSRV[1] Fail");

		_hr = pDevice->CreateUnorderedAccessView(m_pTempRT[1].Get(), &DescUAV, m_pTempUAV[1].GetAddressOf());
		COM_ERROR_IF_FAILED(_hr, "HLSLBloom CreateUnorderedAccessView m_pTempUAV[1] Fail");

		_hr = pDevice->CreateShaderResourceView(m_pBloomRT.Get(), &dsrvd, m_pBloomSRV.GetAddressOf());
		COM_ERROR_IF_FAILED(_hr, "HLSLBloom CreateUnorderedAccessView m_pBloomSRV Fail");

		_hr = pDevice->CreateUnorderedAccessView(m_pBloomRT.Get(), &DescUAV, m_pBloomUAV.GetAddressOf());
		COM_ERROR_IF_FAILED(_hr, "HLSLBloom CreateUnorderedAccessView m_pBloomUAV Fail");

		ZeroMemory(&DescUAV, sizeof(D3D11_UNORDERED_ACCESS_VIEW_DESC));
		DescUAV.Format = DXGI_FORMAT_UNKNOWN;
		DescUAV.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
		DescUAV.Buffer.FirstElement = 0;
		DescUAV.Buffer.NumElements = m_DownScaleGroups;
		_hr = pDevice->CreateUnorderedAccessView(m_pDownScale1DBuffer.Get(), &DescUAV, m_pDownScale1DUAV.GetAddressOf());
		COM_ERROR_IF_FAILED(_hr, "HLSLBloom CreateUnorderedAccessView m_pDownScale1DBuffer Fail");

		dsrvd.Format = DXGI_FORMAT_UNKNOWN;
		dsrvd.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
		dsrvd.Buffer.FirstElement = 0;
		dsrvd.Buffer.NumElements = m_DownScaleGroups;
		_hr = pDevice->CreateShaderResourceView(m_pDownScale1DBuffer.Get(), &dsrvd, m_pDownScale1DSRV.GetAddressOf());
		COM_ERROR_IF_FAILED(_hr, "HLSLBloom CreateShaderResourceView m_pDownScale1DSRV Fail");

		//////////////////////////////////////////////////////////////////////////////////////////////////////
		// Allocate average luminance buffer
		bufferDesc.ByteWidth = sizeof(float);
		_hr = pDevice->CreateBuffer(&bufferDesc, NULL, m_pAvgLumBuffer.GetAddressOf());
		COM_ERROR_IF_FAILED(_hr, "HLSLBloom CreateBuffer m_pAvgLumBuffer Fail");

		DescUAV.Buffer.NumElements = 1;
		_hr = pDevice->CreateUnorderedAccessView(m_pAvgLumBuffer.Get(), &DescUAV, m_pAvgLumUAV.GetAddressOf());
		COM_ERROR_IF_FAILED(_hr, "HLSLBloom CreateUnorderedAccessView m_pAvgLumUAV Fail");

		dsrvd.Buffer.NumElements = 1;
		_hr = pDevice->CreateShaderResourceView(m_pAvgLumBuffer.Get(), &dsrvd, m_pAvgLumSRV.GetAddressOf());
		COM_ERROR_IF_FAILED(_hr, "HLSLBloom CreateShaderResourceView m_pAvgLumSRV Fail");

		//////////////////////////////////////////////////////////////////////////////////////////////////////
		// Allocate previous frame average luminance buffer
		_hr = pDevice->CreateBuffer(&bufferDesc, NULL, m_pPrevAvgLumBuffer.GetAddressOf());
		COM_ERROR_IF_FAILED(_hr, "HLSLBloom CreateBuffer m_pPrevAvgLumBuffer Fail");

		_hr = pDevice->CreateUnorderedAccessView(m_pPrevAvgLumBuffer.Get(), &DescUAV, m_pPrevAvgLumUAV.GetAddressOf());
		COM_ERROR_IF_FAILED(_hr, "HLSLBloom CreateUnorderedAccessView m_pPrevAvgLumUAV Fail");

		_hr = pDevice->CreateShaderResourceView(m_pPrevAvgLumBuffer.Get(), &dsrvd, m_pPrevAvgLumSRV.GetAddressOf());
		COM_ERROR_IF_FAILED(_hr, "HLSLBloom CreateShaderResourceView m_pPrevAvgLumSRV Fail");
	}
	catch (COMException& e)
	{
		ErrorLogger::Log(e);
	}
}
