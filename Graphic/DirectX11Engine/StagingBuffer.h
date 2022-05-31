#pragma once

#include "D3D11Define.h"
#include "COMException.h"



template<class T>
class StagingBuffer
{
public:
	StagingBuffer() {};
	~StagingBuffer() {};


public:
	T m_Data;
	UINT m_Size;

	Microsoft::WRL::ComPtr<ID3D11Buffer> m_OutputBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_OutputDebugBuffer;
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> m_OutputUAV;
	std::unique_ptr<class ComputeShader> m_ComputeShader;

public:
	template<typename T>
	void Initialize(ID3D11Device* pDevice, UINT size)
	{
		m_Size = size;

		try
		{
			HRESULT _hr;

			D3D11_BUFFER_DESC _outputDesc;
			_outputDesc.Usage = D3D11_USAGE_DEFAULT;
			_outputDesc.ByteWidth = sizeof(T) * size;
			_outputDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
			_outputDesc.CPUAccessFlags = 0;
			_outputDesc.StructureByteStride = sizeof(T);
			_outputDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;

			_hr = pDevice->CreateBuffer(&_outputDesc, 0, m_OutputBuffer.GetAddressOf());

			COM_ERROR_IF_FAILED(_hr, "Staging Buffer m_OutputBuffer Create Fail");

			_outputDesc.Usage = D3D11_USAGE_STAGING;
			_outputDesc.BindFlags = 0;
			_outputDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;

			_hr = pDevice->CreateBuffer(&_outputDesc, 0, m_OutputDebugBuffer.GetAddressOf());

			COM_ERROR_IF_FAILED(_hr, "Staging Buffer m_OutputDebugBuffer Create Fail");

			D3D11_UNORDERED_ACCESS_VIEW_DESC _uavDesc;
			_uavDesc.Format = DXGI_FORMAT_UNKNOWN;
			_uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
			_uavDesc.Buffer.FirstElement = 0;
			_uavDesc.Buffer.Flags = 0;
			_uavDesc.Buffer.NumElements = size;

			_hr = pDevice->CreateUnorderedAccessView(m_OutputBuffer.Get(), &_uavDesc, m_OutputUAV.GetAddressOf());

			COM_ERROR_IF_FAILED(_hr, "Staging Buffer m_OutputUAV Create Fail");

		}
		catch (COMException& e)
		{
			ErrorLogger::Log(e);
		}

	}

	template<class T>
	void CopyResource(ID3D11DeviceContext* pDeviceContext)
	{
		pDeviceContext->CopyResource(m_OutputDebugBuffer.Get(), m_OutputBuffer.Get());

		// Map the data for reading.
		D3D11_MAPPED_SUBRESOURCE _mappedData;
		pDeviceContext->Map(m_OutputDebugBuffer.Get(), 0, D3D11_MAP_READ, 0, &_mappedData);

		m_Data = *reinterpret_cast<T*>(_mappedData.pData);

		pDeviceContext->Unmap(m_OutputDebugBuffer.Get(), 0);
	}
};
