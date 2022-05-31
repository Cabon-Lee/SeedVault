#include "pch.h"

#include "Shaders.h"

//bool VertexShader::Initialize(Microsoft::WRL::ComPtr<ID3D11Device>& device, std::wstring shaderpath, const D3D11_INPUT_ELEMENT_DESC* layoutDesc, UINT numElements)
bool VertexShader::Initialize(ID3D11Device* pDevice, std::wstring shaderpath)
{
	HRESULT hr = D3DReadFileToBlob(shaderpath.c_str(), this->vertexshader_buffer.GetAddressOf());
	if (FAILED(hr))
	{
		std::wstring errorMsg = L"쉐이더를 불러오는데 실패했습니다: ";
		errorMsg += shaderpath;
		ErrorLogger::Log(hr, errorMsg);
		return false;
	}

	hr = pDevice->CreateVertexShader(this->vertexshader_buffer->GetBufferPointer(), this->vertexshader_buffer->GetBufferSize(), NULL, this->vertexShader.GetAddressOf());
	if (FAILED(hr))
	{
		std::wstring errorMsg = L"버텍스 쉐이더를 생성하는데 실패했습니다: ";
		errorMsg += shaderpath;
		ErrorLogger::Log(hr, errorMsg);
		return false;
	}

	/*
	hr = device->CreateInputLayout(layoutDesc, numElements, this->vertexshader_buffer->GetBufferPointer(), this->vertexshader_buffer->GetBufferSize(), this->inputLayout.GetAddressOf());
	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, L"inputlayout생성을 실패했습니다.");
		return false;
	}
	*/
	return true;
}

ID3D11VertexShader* VertexShader::GetVertexShader()	{ return this->vertexShader.Get(); }
ID3D10Blob* VertexShader::GetVertexShaderBuffer()	{ return this->vertexshader_buffer.Get(); }
ID3D11InputLayout* VertexShader::GetInputLayout()	{ return this->inputLayout.Get(); }


bool PixelShader::Initialize(ID3D11Device* pDevice, std::wstring shaderpath)
{
	HRESULT hr = D3DReadFileToBlob(shaderpath.c_str(), this->pixelshader_buffer.GetAddressOf());
	if (FAILED(hr))
	{
		std::wstring errorMsg = L"쉐이더를 불러오는데 실패했습니다: ";
		errorMsg += shaderpath;
		ErrorLogger::Log(hr, errorMsg);
		return false;
	}

	hr = pDevice->CreatePixelShader(this->pixelshader_buffer.Get()->GetBufferPointer(), this->pixelshader_buffer.Get()->GetBufferSize(), NULL, this->pixelshader.GetAddressOf());
	if (FAILED(hr))
	{
		std::wstring errorMsg = L"픽셀 쉐이더를 생성하는데 실패했습니다: ";
		errorMsg += shaderpath;
		ErrorLogger::Log(hr, errorMsg);
		return false;
	}
	return true;
}

ID3D11PixelShader* PixelShader::GetPixelShader()	{ return this->pixelshader.Get(); }
ID3D10Blob* PixelShader::GetPixelShaderBuffer()		{ return this->pixelshader_buffer.Get(); }


bool GeometryShader::Initialize(ID3D11Device* pDevice, std::wstring shaderpath)
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

	}
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

ID3D10Blob* GeometryShader::GetGeometryShaderBuffer()
{
	return geometryShader_buffer.Get();
}
