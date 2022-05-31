#include "PixelPicking.h"

#include "CATrace.h"

PixelPicking::PixelPicking()
{

}

PixelPicking::~PixelPicking()
{

}

void PixelPicking::Initialize(Microsoft::WRL::ComPtr<ID3D11Device> pDevice)
{

	m_PixelPickingBuffer = std::make_unique<StagingBuffer<PixelData>>();
	m_PixelPickingBuffer->Initialize<PixelData>(pDevice.Get(), 1);

	m_PixelPickingShader = std::make_unique<class ComputeShader>();
	m_PixelPickingShader->Initialize(pDevice, L"../Data/Shader/cs_PxielPicking.hlsl", "PixelPicking", "cs_5_0");

	m_IDPixelPickingShader = std::make_unique<class ComputeShader>();
	m_IDPixelPickingShader->Initialize(pDevice, L"../Data/Shader/cs_PxielPicking.hlsl", "IDPixelPicking", "cs_5_0");
}

const PixelData& PixelPicking::PixelPickingFromSRV(
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext, 
	POINT mousePos, UINT width, UINT height, 
	ID3D11ShaderResourceView* pSRV[], UINT srvCount)
{
	float _cbufferData[4] =
	{
		mousePos.x, mousePos.y, 0.0f, 0.0f,
	};

	ConstantBufferManager::SetGeneralCBuffer(pDeviceContext, CBufferType::CS, CBufferSize::SIZE_4, 0, _cbufferData);

	pDeviceContext->CSSetShaderResources(0, srvCount, pSRV);

	ID3D11UnorderedAccessView* _arrUAVs[1] = { m_PixelPickingBuffer->m_OutputUAV.Get() };
	pDeviceContext->CSSetUnorderedAccessViews(0, 1, _arrUAVs, (UINT*)(&_arrUAVs));
	pDeviceContext->CSSetShader(m_PixelPickingShader->GetComputeShader(), NULL, 0);

	UINT _numGroupsX = static_cast<UINT>(ceilf(width / 32));
	UINT _numGroupsY = static_cast<UINT>(ceilf(height / 32));

	pDeviceContext->Dispatch(_numGroupsX, _numGroupsY, 1);

	m_PixelPickingBuffer->CopyResource<PixelData>(pDeviceContext.Get());

	// Unbind the input texture from the CS for good housekeeping.
	ID3D11ShaderResourceView* _nullSRV[16] = { nullptr };
	ID3D11UnorderedAccessView* _nullUAV[1] = { 0 };
	pDeviceContext->CSSetShaderResources(0, srvCount, _nullSRV);
	pDeviceContext->CSSetUnorderedAccessViews(0, 1, _nullUAV, 0);
	pDeviceContext->CSSetShader(0, 0, 0);


	float _test[3] = { 0.0f, };
	_test[0] = m_PixelPickingBuffer->m_Data.normal.x;
	_test[1] = m_PixelPickingBuffer->m_Data.normal.y;
	_test[2] = m_PixelPickingBuffer->m_Data.normal.z;

	CA_TRACE("Mouse Pos %d, %d", mousePos.x, mousePos.y);
	CA_TRACE("Color %f, %f, %f", _test[0], _test[1], _test[2]);

	return m_PixelPickingBuffer->m_Data;
}
