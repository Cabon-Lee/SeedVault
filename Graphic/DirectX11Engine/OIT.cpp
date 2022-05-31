#include "OIT.h"

#include "VertexHelper.h"

#include "COMException.h"
#include "ErrorLogger.h"

OIT::OIT()
	: m_Width(0), m_Height(0)
	, m_StructureBuffer(nullptr), m_StructureedSRV(nullptr), m_StructureedUAV(nullptr)
	, m_ByteAddressBuffer(nullptr), m_ByteAddressSRV(nullptr), m_ByteAddressUAV(nullptr)
{

}

OIT::~OIT()
{

}

void OIT::Initialize(Microsoft::WRL::ComPtr<ID3D11Device> pDevice, UINT width, UINT height)
{
	m_Width = width;
	m_Height = height;

	try
	{
		UINT _enouhgSize = 20000;


		HRESULT _hr;
		D3D11_BUFFER_DESC _structBufferDesc;
		_structBufferDesc.ByteWidth = sizeof(FragmentAndLinkBuffer) * _enouhgSize;
		_structBufferDesc.StructureByteStride = sizeof(FragmentAndLinkBuffer);
		_structBufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
		_structBufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
		_structBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		_structBufferDesc.CPUAccessFlags = 0;
		//_structBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		//_structBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		std::vector<FragmentAndLinkBuffer> _linkBuffer;
		_linkBuffer.emplace_back(FragmentAndLinkBuffer());

		D3D11_SUBRESOURCE_DATA mappedResource;
		mappedResource.pSysMem = _linkBuffer.data();
		//mappedResource.SysMemPitch = 0;		// 2D/3D 텍스쳐 데이터에만 사용됨
		//mappedResource.SysMemSlicePitch = 0;	// 3D 텍스쳐 데이터에만 사용
		
		_hr = pDevice->CreateBuffer(&_structBufferDesc, &mappedResource, m_StructureBuffer.GetAddressOf());

		COM_ERROR_IF_FAILED(_hr, "OIT Initialize RWStructured Buffer Create Fail");

		D3D11_UNORDERED_ACCESS_VIEW_DESC _uavDesc;
		_uavDesc.Format = DXGI_FORMAT_UNKNOWN;
		_uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
		_uavDesc.Buffer.FirstElement = 0;
		_uavDesc.Buffer.NumElements = _enouhgSize;
		// RWStructuredBuffer를 위한 UAV를 사용할 때 IncreamentCounter 등의 method를 사용하기 위한 플래그
		_uavDesc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_COUNTER;

		_hr = pDevice->CreateUnorderedAccessView(m_StructureBuffer.Get(), &_uavDesc, m_StructureedUAV.GetAddressOf());

		COM_ERROR_IF_FAILED(_hr, "OIT Initialize RWStructured UAV Create Fail");

		D3D11_SHADER_RESOURCE_VIEW_DESC _srvDesc;
		_srvDesc.Format = DXGI_FORMAT_UNKNOWN;
		_srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
		_srvDesc.BufferEx.FirstElement = 0;
		_srvDesc.BufferEx.Flags = 0;
		_srvDesc.BufferEx.NumElements = _enouhgSize;

		_hr = pDevice->CreateShaderResourceView(m_StructureBuffer.Get(), &_srvDesc, m_StructureedSRV.GetAddressOf());

		COM_ERROR_IF_FAILED(_hr, "OIT Initialize RWStructured SRV Create Fail");


		UINT _byteWidth = m_Width * m_Height * sizeof(UINT32);
		UINT _numElement = m_Width * m_Height;

		D3D11_BUFFER_DESC _byteDesc;
		_byteDesc.ByteWidth = _byteWidth;
		_byteDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS;
		_byteDesc.StructureByteStride = 0;

		_byteDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
		_byteDesc.Usage = D3D11_USAGE_DEFAULT;
		_byteDesc.CPUAccessFlags = 0;

		_hr = pDevice->CreateBuffer(&_byteDesc, nullptr, m_ByteAddressBuffer.GetAddressOf());
		

		COM_ERROR_IF_FAILED(_hr, "OIT Initialize RWByteAddress Buffer Create Fail");

		D3D11_UNORDERED_ACCESS_VIEW_DESC _uavByteDesc;

		_uavByteDesc.Format = DXGI_FORMAT_R32_TYPELESS;
		_uavByteDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
		_uavByteDesc.Buffer.FirstElement = 0;
		_uavByteDesc.Buffer.NumElements = _numElement;		// _byteWidth가 들어가는게 아니라, 현재 NumElenment가 몇개인지를 넣는 것
		_uavByteDesc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_RAW;

		_hr = pDevice->CreateUnorderedAccessView(m_ByteAddressBuffer.Get(), &_uavByteDesc, m_ByteAddressUAV.GetAddressOf());

		COM_ERROR_IF_FAILED(_hr, "OIT Initialize RWByteAddress UAV Create Fail");

		D3D11_SHADER_RESOURCE_VIEW_DESC _srvByteDesc;

		_srvByteDesc.Format = DXGI_FORMAT_R32_TYPELESS;
		_srvByteDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
		_srvByteDesc.BufferEx.FirstElement = 0;
		_srvByteDesc.BufferEx.Flags = D3D11_BUFFEREX_SRV_FLAG_RAW;
		_srvByteDesc.BufferEx.NumElements = _numElement;

		_hr = pDevice->CreateShaderResourceView(m_ByteAddressBuffer.Get(), &_srvByteDesc, m_ByteAddressSRV.GetAddressOf());

		COM_ERROR_IF_FAILED(_hr, "OIT Initialize RWStructured SRV Create Fail");


	}
	catch (COMException& e)
	{
		ErrorLogger::Log(e);
		
	}

}

void OIT::OnResize(Microsoft::WRL::ComPtr<ID3D11Device> pDevice, UINT width, UINT height)
{
	m_Width = width;
	m_Height = height;

	m_ByteAddressBuffer.Reset();
	m_ByteAddressSRV.Reset();
	m_ByteAddressUAV.Reset();

	try
	{
		HRESULT _hr;

		UINT _byteWidth = m_Width * m_Height * sizeof(UINT32);
		UINT _numElement = m_Width * m_Height;

		D3D11_BUFFER_DESC _byteDesc;
		_byteDesc.ByteWidth = _byteWidth;
		_byteDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS;
		_byteDesc.StructureByteStride = 0;

		_byteDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
		_byteDesc.Usage = D3D11_USAGE_DEFAULT;
		_byteDesc.CPUAccessFlags = 0;

		_hr = pDevice->CreateBuffer(&_byteDesc, nullptr, m_ByteAddressBuffer.GetAddressOf());

		COM_ERROR_IF_FAILED(_hr, "OIT Initialize RWByteAddress Buffer Create Fail");

		D3D11_UNORDERED_ACCESS_VIEW_DESC _uavByteDesc;

		_uavByteDesc.Format = DXGI_FORMAT_R32_TYPELESS;
		_uavByteDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
		_uavByteDesc.Buffer.FirstElement = 0;
		_uavByteDesc.Buffer.NumElements = _numElement;		
		_uavByteDesc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_RAW;

		_hr = pDevice->CreateUnorderedAccessView(m_ByteAddressBuffer.Get(), &_uavByteDesc, m_ByteAddressUAV.GetAddressOf());

		COM_ERROR_IF_FAILED(_hr, "OIT Initialize RWByteAddress UAV Create Fail");

		D3D11_SHADER_RESOURCE_VIEW_DESC _srvByteDesc;

		_srvByteDesc.Format = DXGI_FORMAT_R32_TYPELESS;
		_srvByteDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
		_srvByteDesc.BufferEx.FirstElement = 0;
		_srvByteDesc.BufferEx.Flags = D3D11_BUFFEREX_SRV_FLAG_RAW;
		_srvByteDesc.BufferEx.NumElements = _numElement;

		_hr = pDevice->CreateShaderResourceView(m_ByteAddressBuffer.Get(), &_srvByteDesc, m_ByteAddressSRV.GetAddressOf());

		COM_ERROR_IF_FAILED(_hr, "OIT Initialize RWStructured SRV Create Fail");

	}
	catch (COMException& e)
	{
		ErrorLogger::Log(e);
	}

}
