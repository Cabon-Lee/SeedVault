#pragma once
//#pragma comment(lib, "D3DCompiler.lib")
#include<d3d11.h>
#include<wrl/client.h>
#include<d3dcompiler.h>
#include <string>

#include "ErrorLogger.h"
#include "VertexHelper.h"
#include "d3dx11effect.h"

class VertexShader
{
public:
	//bool Initialize(Microsoft::WRL::ComPtr<ID3D11Device>& device, std::wstring shaderpath, const D3D11_INPUT_ELEMENT_DESC* layoutDesc, UINT numElements);
	bool Initialize(ID3D11Device* device, std::wstring shaderpath);
	ID3D11VertexShader* GetVertexShader();
	ID3D10Blob* GetVertexShaderBuffer();
	ID3D11InputLayout* GetInputLayout();

	Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;
protected:
	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;
	Microsoft::WRL::ComPtr<ID3D10Blob> vertexshader_buffer;
};

class PixelShader
{
public:
	bool Initialize(ID3D11Device* device, std::wstring shaderpath);
	ID3D11PixelShader* GetPixelShader();
	ID3D10Blob* GetPixelShaderBuffer();

protected:
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelshader;
	Microsoft::WRL::ComPtr<ID3D10Blob> pixelshader_buffer;
};

class GeometryShader
{
public:
	bool Initialize(ID3D11Device* pDevice, std::wstring shaderpath);
	bool Initialize(
		Microsoft::WRL::ComPtr<ID3D11Device> device,
		_In_ LPCWSTR srcFile, _In_ LPCSTR entryPoint, _In_ LPCSTR profile);

	bool StreamOutputInitialize(
		Microsoft::WRL::ComPtr<ID3D11Device> device,
		const D3D11_SO_DECLARATION_ENTRY* pDeclEntry,
		const UINT declSize);

	ID3D11GeometryShader* GetGeometryShader();
	ID3D11GeometryShader* GetSteamOutputGeometryShader();
	ID3D10Blob* GetGeometryShaderBuffer();

protected:
	Microsoft::WRL::ComPtr<ID3D11GeometryShader> geometryShader;
	Microsoft::WRL::ComPtr<ID3D11GeometryShader> streamoutputGeometryShader;
	Microsoft::WRL::ComPtr<ID3D10Blob> geometryShader_buffer;


};