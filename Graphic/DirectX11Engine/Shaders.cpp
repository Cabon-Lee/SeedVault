#include "Shaders.h"

#include "ErrorLogger.h"
#include "COMException.h"

HRESULT CompileShader
(_In_ LPCWSTR srcFile, _In_ LPCSTR entryPoint, _In_ LPCSTR profile, _Outptr_ ID3DBlob** blob)
{
	if (!srcFile || !entryPoint || !profile || !blob)
		return E_INVALIDARG;

	*blob = nullptr;

	UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
	flags |= D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	const D3D_SHADER_MACRO defines[] =
	{
		"EXAMPLE_DEFINE", "1",
		NULL, NULL
	};

	ID3DBlob* shaderBlob = nullptr;
	ID3DBlob* errorBlob = nullptr;
	HRESULT hr = D3DCompileFromFile(srcFile, defines, D3D_COMPILE_STANDARD_FILE_INCLUDE,
		entryPoint, profile,
		flags, 0, &shaderBlob, &errorBlob);
	if (FAILED(hr))
	{
		if (errorBlob)
		{
			OutputDebugStringA((char*)errorBlob->GetBufferPointer());
			errorBlob->Release();
		}

		if (shaderBlob)
			shaderBlob->Release();

		return hr;
	}

	*blob = shaderBlob;

	return hr;
}

bool VertexShader::Initialize(Microsoft::WRL::ComPtr<ID3D11Device>& device, std::wstring shaderpath)
{
	HRESULT hr = D3DReadFileToBlob(shaderpath.c_str(), this->vertexshader_buffer.GetAddressOf());
	if (FAILED(hr))
	{
		std::wstring errorMsg = L"쉐이더를 불러오는데 실패했습니다: ";
		errorMsg += shaderpath;
		ErrorLogger::Log(hr, errorMsg);
		return false;
	}

	hr = device->CreateVertexShader(this->vertexshader_buffer->GetBufferPointer(), this->vertexshader_buffer->GetBufferSize(), NULL, this->vertexShader.GetAddressOf());
	if (FAILED(hr))
	{
		std::wstring errorMsg = L"버텍스 쉐이더를 생성하는데 실패했습니다: ";
		errorMsg += shaderpath;
		ErrorLogger::Log(hr, errorMsg);
		return false;
	}

	return true;
}

bool VertexShader::Initialize(Microsoft::WRL::ComPtr<ID3D11Device> pDevice, _In_ LPCWSTR srcFile, _In_ LPCSTR entryPoint, _In_ LPCSTR profile)
{
	HRESULT _hr;
	try
	{
		_hr = CompileShader(srcFile, entryPoint, profile, vertexshader_buffer.GetAddressOf());

		double _err = GetLastError();

		COM_ERROR_IF_FAILED(_hr, "CompileShader Fail");

		_hr = pDevice->CreateVertexShader(
			vertexshader_buffer->GetBufferPointer(),
			vertexshader_buffer->GetBufferSize(), NULL, vertexShader.GetAddressOf());
		COM_ERROR_IF_FAILED(_hr, "CreatePixelShader Fail");

	}
	catch (COMException& e)
	{
		ErrorLogger::Log(e);
		return false;
	}

	return true;
}

ID3D11VertexShader* VertexShader::GetVertexShader()	{ return this->vertexShader.Get(); }
ID3D10Blob* VertexShader::GetVertexShaderBuffer()	{ return this->vertexshader_buffer.Get(); }
ID3D11InputLayout* VertexShader::GetInputLayout()	{ return this->inputLayout.Get(); }


bool PixelShader::Initialize(Microsoft::WRL::ComPtr<ID3D11Device>& device, std::wstring shaderpath)
{
	HRESULT hr = D3DReadFileToBlob(shaderpath.c_str(), this->pixelshader_buffer.GetAddressOf());
	if (FAILED(hr))
	{
		std::wstring errorMsg = L"쉐이더를 불러오는데 실패했습니다: ";
		errorMsg += shaderpath;
		ErrorLogger::Log(hr, errorMsg);
		return false;
	}

	hr = device->CreatePixelShader(this->pixelshader_buffer.Get()->GetBufferPointer(), this->pixelshader_buffer.Get()->GetBufferSize(), NULL, this->pixelshader.GetAddressOf());
	if (FAILED(hr))
	{
		std::wstring errorMsg = L"픽셀 쉐이더를 생성하는데 실패했습니다: ";
		errorMsg += shaderpath;
		ErrorLogger::Log(hr, errorMsg);
		return false;
	}
	return true;
}


bool PixelShader::Initialize(
	Microsoft::WRL::ComPtr<ID3D11Device> pDevice, _In_ LPCWSTR srcFile, _In_ LPCSTR entryPoint, _In_ LPCSTR profile)
{
	HRESULT _hr;
	try
	{
		_hr = CompileShader(srcFile, entryPoint, profile, pixelshader_buffer.GetAddressOf());

		double _err = GetLastError();

		COM_ERROR_IF_FAILED(_hr, "CompileShader Fail");

		_hr = pDevice->CreatePixelShader(
			pixelshader_buffer->GetBufferPointer(), 
			pixelshader_buffer->GetBufferSize(), NULL, pixelshader.GetAddressOf());
		COM_ERROR_IF_FAILED(_hr, "CreatePixelShader Fail");


	}
	catch (COMException& e)
	{
		ErrorLogger::Log(e);
		return false;
	}
	return true;
}

ID3D11PixelShader* PixelShader::GetPixelShader()	{ return this->pixelshader.Get(); }
ID3D10Blob* PixelShader::GetPixelShaderBuffer()		{ return this->pixelshader_buffer.Get(); }


bool ComputeShader::Initialize(Microsoft::WRL::ComPtr<ID3D11Device>& device, std::wstring shaderpath)
{
	HRESULT hr = D3DReadFileToBlob(shaderpath.c_str(), computeShader_buffer.GetAddressOf());
	if (FAILED(hr))
	{
		std::wstring errorMsg = L"쉐이더를 불러오는데 실패했습니다: ";
		errorMsg += shaderpath;
		ErrorLogger::Log(hr, errorMsg);
		return false;
	}

	hr = device->CreateComputeShader(
		computeShader_buffer.Get()->GetBufferPointer(),
		computeShader_buffer.Get()->GetBufferSize(), NULL, computeShader.GetAddressOf());
	if (FAILED(hr))
	{
		std::wstring errorMsg = L"픽셀 쉐이더를 생성하는데 실패했습니다: ";
		errorMsg += shaderpath;
		ErrorLogger::Log(hr, errorMsg);
		return false;
	}
	return true;
}

bool ComputeShader::Initialize(
	Microsoft::WRL::ComPtr<ID3D11Device> device, 
	_In_ LPCWSTR srcFile, _In_ LPCSTR entryPoint, _In_ LPCSTR profile)
{
	HRESULT _hr;
	try
	{
		_hr = CompileShader(srcFile, entryPoint, profile, computeShader_buffer.GetAddressOf());

		double _err = GetLastError();

		COM_ERROR_IF_FAILED(_hr, "CompileShader Fail");

		_hr = device->CreateComputeShader(
			computeShader_buffer->GetBufferPointer(),
			computeShader_buffer->GetBufferSize(), NULL, computeShader.GetAddressOf());
		COM_ERROR_IF_FAILED(_hr, "CreatePixelShader Fail");

	}
	catch (COMException& e)
	{
		ErrorLogger::Log(e);
		return false;
	}
	return true;
}

ID3D11ComputeShader* ComputeShader::GetComputeShader()	{return computeShader.Get();}
ID3D10Blob* ComputeShader::GetComputeShaderBuffer()		{return computeShader_buffer.Get();}

// OIT 를 돌리기위해 임시로 만드는 Buffer class
// 제대로 돌아가는게 확인되고 구조파악이되면 없어질것임
/// -------------------------------------
/// Buffer
/// -------------------------------------
Buffer::Buffer(Microsoft::WRL::ComPtr<ID3D11Device> pDevice, D3D11_BUFFER_DESC* desc, void* initValue)
	:desc(*desc)
{
	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = initValue;

	if (initValue == nullptr)
	{
		HRESULT hr = pDevice->CreateBuffer(desc,	nullptr,&m_resource);
	}
	else
	{
		HRESULT hr = pDevice->CreateBuffer(desc,	&data,&m_resource);
	}
}

UINT SizeCB(UINT byteSize)
{
	return (15 + byteSize - (byteSize - 1) % 16);
}

Buffer::Buffer(Microsoft::WRL::ComPtr<ID3D11Device> pDevice, UINT byteSize)
	:desc(CD3D11_BUFFER_DESC(SizeCB(byteSize), D3D11_BIND_CONSTANT_BUFFER, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE, 0, 0))
{
	HRESULT hr = pDevice->CreateBuffer(&desc, nullptr, &m_resource);
}

Buffer::~Buffer()
{
	m_resource->Release();
	if (m_srv)
		m_srv->Release();
	if (m_uav)
		m_uav->Release();
}


void Buffer::SetSRV(Microsoft::WRL::ComPtr<ID3D11Device> pDevice, D3D11_SHADER_RESOURCE_VIEW_DESC* srvDesc)
{
	if (m_srv)
		m_srv->Release();

	HRESULT hr = pDevice->CreateShaderResourceView(m_resource, srvDesc, &m_srv);
}
void Buffer::SetUAV(Microsoft::WRL::ComPtr<ID3D11Device> pDevice, D3D11_UNORDERED_ACCESS_VIEW_DESC* uavDesc)
{
	if (m_uav)
		m_uav->Release();

	HRESULT hr = pDevice->CreateUnorderedAccessView(m_resource, uavDesc,	&m_uav);
}

void Buffer::Write(Microsoft::WRL::ComPtr <ID3D11DeviceContext> pDeviceContext, const void* data)
{
	D3D11_MAPPED_SUBRESOURCE mappedData;

	pDeviceContext->Map(m_resource, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData);
	CopyMemory(mappedData.pData, data, desc.ByteWidth);
	pDeviceContext->Unmap(m_resource, 0);
}

void Buffer::GetSRV(ID3D11ShaderResourceView** srv)
{
	*srv = m_srv;
}

void Buffer::GetUAV(ID3D11UnorderedAccessView** uav)
{
	*uav = m_uav;
}

// OIT 를 돌리기위해 임시로 만드는 Shader class
// 제대로 돌아가는게 확인되고 구조파악이되면 없어질것임
/// -------------------------------------
/// Shader
/// -------------------------------------
Shader::~Shader()
{
	for (auto cb : cbs)
	{
		delete cb.second.data;
	}
	for (auto samp : samps)
	{
		if (samp.second.data)
			samp.second.data->Release();
	}
}

void Shader::AddCB(Microsoft::WRL::ComPtr<ID3D11Device> pDevice, UINT slot, UINT arrayNum, UINT byteSize)
{
	if (cbs.find(slot) == cbs.end())
	{
		cbs.insert(std::pair<UINT, BindingCB>(slot, BindingCB(new Buffer(pDevice, byteSize), arrayNum)));
	}
	else
	{
		// slot overlap
		assert(false);
	}
}

void Shader::AddSRV(UINT slot, UINT arrayNum)
{
	assert(srvs.find(slot) == srvs.end());

	srvs.insert(std::pair<UINT, BindingSRV>(slot, BindingSRV(nullptr, arrayNum)));
}

void Shader::AddSamp(Microsoft::WRL::ComPtr<ID3D11Device> pDevice, UINT slot, UINT arrayNum, D3D11_SAMPLER_DESC* desc)
{
	if (samps.find(slot) == samps.end())
	{
		ID3D11SamplerState* newSampler = nullptr;
		pDevice->CreateSamplerState(desc, &newSampler);

		samps.insert(std::pair<UINT, BindingSamp>(slot, BindingSamp(newSampler, arrayNum)));
	}
	else
	{
		// slot overlap
		assert(false);
	}
}

void Shader::WriteCB(Microsoft::WRL::ComPtr <ID3D11DeviceContext> pDeviceContext, UINT slot, const void* data)
{
	if (cbs.find(slot) != cbs.end() && data)
	{
		cbs[slot].data->Write(pDeviceContext, data);
	}
	else
	{
		//no reg
		assert(false);
	}
}

void Shader::WriteSRV(UINT slot, ID3D11ShaderResourceView* srv)
{
	assert(srvs.find(slot) != srvs.end());

	srvs[slot].data = srv;
}

void Shader::RemoveCB(UINT slot)
{
	assert(cbs.find(slot) != cbs.end());

	cbs.erase(slot);
}

bool Shader::CheckCBSlot(UINT slot)
{
	return cbs.count(slot);
}

bool GeometryShader::Initialize(Microsoft::WRL::ComPtr<ID3D11Device>& pDevice, std::wstring shaderpath)
{
	HRESULT hr = D3DReadFileToBlob(shaderpath.c_str(), this->geometryShader_buffer.GetAddressOf());
	if (FAILED(hr))
	{
		std::wstring errorMsg = L"쉐이더를 불러오는데 실패했습니다: ";
		errorMsg += shaderpath;
		ErrorLogger::Log(hr, errorMsg);
		return false;
	}

	hr = pDevice->CreateGeometryShader(
		this->geometryShader_buffer.Get()->GetBufferPointer(), 
		this->geometryShader_buffer.Get()->GetBufferSize(),
		NULL, 
		this->geometryShader.GetAddressOf());

	if (FAILED(hr))
	{
		std::wstring errorMsg = L"픽셀 쉐이더를 생성하는데 실패했습니다: ";
		errorMsg += shaderpath;
		ErrorLogger::Log(hr, errorMsg);
		return false;
	}
	return true;
}

bool GeometryShader::Initialize(Microsoft::WRL::ComPtr<ID3D11Device> pDevice, _In_ LPCWSTR srcFile, _In_ LPCSTR entryPoint, _In_ LPCSTR profile)
{
	HRESULT _hr;
	try
	{
		_hr = CompileShader(srcFile, entryPoint, profile, geometryShader_buffer.GetAddressOf());

		double _err = GetLastError();

		COM_ERROR_IF_FAILED(_hr, "CompileShader Fail");

		_hr = pDevice->CreateGeometryShader(
			geometryShader_buffer->GetBufferPointer(),
			geometryShader_buffer->GetBufferSize(), NULL, geometryShader.GetAddressOf());

		COM_ERROR_IF_FAILED(_hr, "CreatePixelShader Fail");

	}
	catch (COMException& e)
	{
		ErrorLogger::Log(e);
		return false;
	}
	return true;
}

bool GeometryShader::StreamOutputInitialize(
	Microsoft::WRL::ComPtr<ID3D11Device> pDevice, 
	const D3D11_SO_DECLARATION_ENTRY* pDeclEntry,
	const UINT declSize)
{
	if (geometryShader_buffer == nullptr || geometryShader == nullptr) return false;

	try
	{
		HRESULT _hr;

		_hr = pDevice->CreateGeometryShaderWithStreamOutput(
			geometryShader_buffer->GetBufferPointer(),
			geometryShader_buffer->GetBufferSize(),
			pDeclEntry,
			declSize,
			nullptr, 0, D3D11_SO_NO_RASTERIZED_STREAM,
			nullptr,
			streamoutputGeometryShader.GetAddressOf());

		COM_ERROR_IF_FAILED(_hr, "CreateGeometryShaderWithStreamOutput Failed");
	}
	catch (COMException& e)
	{
		ErrorLogger::Log(e);
		return false;
	}
	return true;
}

ID3D11GeometryShader* GeometryShader::GetGeometryShader()
{
	return geometryShader.Get();
}

ID3D11GeometryShader* GeometryShader::GetSteamOutputGeometryShader()
{
	if (streamoutputGeometryShader == nullptr)
	{
		return nullptr;
	}
	else
	{
		return streamoutputGeometryShader.Get();
	}
}

Microsoft::WRL::ComPtr<ID3D11GeometryShader>& GeometryShader::GetStreamOutTest()
{
	return streamoutputGeometryShader;
}

ID3D10Blob* GeometryShader::GetGeometryShaderBuffer()
{
	return geometryShader_buffer.Get();
}
