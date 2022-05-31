#pragma once

#include "D3D11Define.h"
#include "ConstantBufferManager.h"
#include "Shaders.h"

#include "StagingBuffer.h"


struct PixelData
{
	DirectX::XMFLOAT3 normal;
	DirectX::XMFLOAT3 position;
};

struct PixelID
{
	DirectX::XMFLOAT4 color;
};

struct DeferredPixel
{
	DirectX::XMFLOAT3 normal;
	UINT objectID;
	DirectX::XMFLOAT4 position;
};

class PixelPicking
{
public:
	PixelPicking();
	~PixelPicking();

public:
	void Initialize(Microsoft::WRL::ComPtr<ID3D11Device> pDevice);

	const PixelData& PixelPickingFromSRV(
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext,
		POINT mousePos, UINT width, UINT height,
		ID3D11ShaderResourceView* pSRV[],
		UINT srvCount);

	template<typename T>
	const T& PixelPickingFromSRV(
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext,
		POINT mousePos, UINT width, UINT height,
		ID3D11ShaderResourceView* pSRV[],
		UINT srvCount,
		StagingBuffer<T>* pData);

private:
	std::unique_ptr<StagingBuffer<PixelData>> m_PixelPickingBuffer;
	std::unique_ptr<class ComputeShader> m_PixelPickingShader;
	std::unique_ptr<class ComputeShader> m_IDPixelPickingShader;

};

template<typename T>
inline const T& PixelPicking::PixelPickingFromSRV(
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext, 
	POINT mousePos, UINT width, UINT height, 
	ID3D11ShaderResourceView* pSRV[], UINT srvCount, 
	StagingBuffer<T>* pData)
{
	// ���콺 ��ġ�� ��� ���۸� ���� �Ѱ��ش�
	float _cbufferData[4] =	{ mousePos.x, mousePos.y, 0.0f, 0.0f };

	ConstantBufferManager::SetGeneralCBuffer(
		pDeviceContext, CBufferType::CS, CBufferSize::SIZE_4, 0, _cbufferData);

	// �ٱ����� �迭�� ������ SRV�� Compute Shader�� �����Ѵ�
	pDeviceContext->CSSetShaderResources(0, srvCount, pSRV);

	ID3D11UnorderedAccessView* _arrUAVs[1] = { pData->m_OutputUAV.Get() };
	pDeviceContext->CSSetUnorderedAccessViews(0, 1, _arrUAVs, (UINT*)(&_arrUAVs));

	// Staging Buffer�� �ν��Ͻ��� ������ �Ҵ��� Compute Shader�� �ִ��� Ȯ���Ѵ�
	if(pData->m_ComputeShader != nullptr)
		pDeviceContext->CSSetShader(pData->m_ComputeShader->GetComputeShader(), NULL, 0);
	else
		pDeviceContext->CSSetShader(m_IDPixelPickingShader->GetComputeShader(), NULL, 0);

	UINT _numGroupsX = static_cast<UINT>(ceilf(width / 32));
	UINT _numGroupsY = static_cast<UINT>(ceilf(height / 32));
	pDeviceContext->Dispatch(_numGroupsX, _numGroupsY, 1);

	pData->CopyResource<T>(pDeviceContext.Get());

#pragma region ���̴� UnBind
	ID3D11ShaderResourceView* _nullSRV[16] = { nullptr };
	ID3D11UnorderedAccessView* _nullUAV[1] = { 0 };
	pDeviceContext->CSSetShaderResources(0, srvCount, _nullSRV);
	pDeviceContext->CSSetUnorderedAccessViews(0, 1, _nullUAV, 0);
	pDeviceContext->CSSetShader(0, 0, 0);
#pragma endregion ���̴� UnBind

	return pData->m_Data;
}

