#pragma once
#include<d3d11.h>
#include<wrl/client.h>
#include<d3dcompiler.h>
#include <string>

#include "ErrorLogger.h"
#include "VertexHelper.h"
#include "d3dx11effect.h"
#include "LightHelper.h"
#include "D3D11Define.h"
#include <unordered_map>

class Buffer
{
	int i = D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT;
public:
	Buffer(Microsoft::WRL::ComPtr<ID3D11Device> pDevice, D3D11_BUFFER_DESC* desc, void* initValue);
	Buffer(Microsoft::WRL::ComPtr<ID3D11Device> pDevice, UINT byteSize);
	~Buffer();

	void SetSRV(Microsoft::WRL::ComPtr<ID3D11Device> pDevice, D3D11_SHADER_RESOURCE_VIEW_DESC* srvDesc);
	void SetUAV(Microsoft::WRL::ComPtr<ID3D11Device> pDevice, D3D11_UNORDERED_ACCESS_VIEW_DESC* uavDesc);

	void Write(Microsoft::WRL::ComPtr <ID3D11DeviceContext> pDeviceContext, const void* data);

	ID3D11Buffer* Get() { return m_resource; }
	ID3D11Buffer** GetAddress() { return &m_resource; }
	void GetSRV(ID3D11ShaderResourceView** srv);
	void GetUAV(ID3D11UnorderedAccessView** uav);

	const D3D11_BUFFER_DESC desc;

private:
	ID3D11Buffer* m_resource = nullptr;
	ID3D11ShaderResourceView* m_srv = nullptr;
	ID3D11UnorderedAccessView* m_uav = nullptr;
};

class Shader
{
public:
	~Shader();
	void AddCB(Microsoft::WRL::ComPtr<ID3D11Device> pDevice, UINT slot, UINT arrayNum, UINT byteSize);
	void AddSRV(UINT slot, UINT arrayNum);
	void AddSamp(Microsoft::WRL::ComPtr<ID3D11Device> pDevice, UINT slot, UINT arrayNum, D3D11_SAMPLER_DESC* desc);

	void WriteCB(Microsoft::WRL::ComPtr <ID3D11DeviceContext> pDeviceContext, UINT slot, const void* data);
	void WriteSRV(UINT slot, ID3D11ShaderResourceView* srv);

	void RemoveCB(UINT slot);
	bool CheckCBSlot(UINT slot);

	virtual void Apply(Microsoft::WRL::ComPtr <ID3D11DeviceContext> pDeviceContext)const = 0;
	virtual void UnboundSRVAll(Microsoft::WRL::ComPtr <ID3D11DeviceContext> pDeviceContext)const = 0;

protected:
	struct BindingCB
	{
		Buffer* data;
		UINT arrayNum;
		BindingCB()
			:data(nullptr), arrayNum(0) {}
		BindingCB(Buffer* data, UINT arrayNum)
			:data(data), arrayNum(arrayNum) {}
	};
	struct BindingSRV
	{
		ID3D11ShaderResourceView* data;
		UINT arrayNum;
		BindingSRV()
			:data(nullptr), arrayNum(0) {}
		BindingSRV(ID3D11ShaderResourceView* data, UINT arrayNum)
			:data(data), arrayNum(arrayNum) {}
	};
	struct BindingSamp
	{
		ID3D11SamplerState* data;
		UINT arrayNum;
		BindingSamp()
			:data(nullptr), arrayNum(0) {}
		BindingSamp(ID3D11SamplerState* data, UINT arrayNum)
			:data(data), arrayNum(arrayNum) {}
	};

	std::unordered_map<UINT, BindingCB> cbs;
	std::unordered_map<UINT, BindingSRV> srvs;
	std::unordered_map<UINT, BindingSamp> samps;
};


class VertexShader
{
public:
	bool Initialize(Microsoft::WRL::ComPtr<ID3D11Device>& device, std::wstring shaderpath);
	bool Initialize(
		Microsoft::WRL::ComPtr<ID3D11Device> device,
		_In_ LPCWSTR srcFile, _In_ LPCSTR entryPoint, _In_ LPCSTR profile);

	ID3D11VertexShader* GetVertexShader();
	ID3D10Blob* GetVertexShaderBuffer();
	ID3D11InputLayout* GetInputLayout();

	//void Apply(Microsoft::WRL::ComPtr <ID3D11DeviceContext> pDeviceContext)const override;
	//void UnboundSRVAll(Microsoft::WRL::ComPtr <ID3D11DeviceContext> pDeviceContext)const override;

	Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;
protected:
	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;
	Microsoft::WRL::ComPtr<ID3D10Blob> vertexshader_buffer;
};

class PixelShader
{
public:
	bool Initialize(Microsoft::WRL::ComPtr<ID3D11Device>& device, std::wstring shaderpath);
	bool Initialize(
		Microsoft::WRL::ComPtr<ID3D11Device> device, 
		_In_ LPCWSTR srcFile, _In_ LPCSTR entryPoint, _In_ LPCSTR profile);

	ID3D11PixelShader* GetPixelShader();
	ID3D10Blob* GetPixelShaderBuffer();

	//void Apply(Microsoft::WRL::ComPtr <ID3D11DeviceContext> pDeviceContext)const override;
	//void UnboundSRVAll(Microsoft::WRL::ComPtr <ID3D11DeviceContext> pDeviceContext)const override;

protected:
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelshader;
	Microsoft::WRL::ComPtr<ID3D10Blob> pixelshader_buffer;
};

class ComputeShader
{
public:
	bool Initialize(Microsoft::WRL::ComPtr<ID3D11Device>& device, std::wstring shaderpath);
	bool Initialize(
		Microsoft::WRL::ComPtr<ID3D11Device> device,
		_In_ LPCWSTR srcFile, _In_ LPCSTR entryPoint, _In_ LPCSTR profile);

	ID3D11ComputeShader* GetComputeShader();
	ID3D10Blob* GetComputeShaderBuffer();

protected:
	Microsoft::WRL::ComPtr<ID3D11ComputeShader> computeShader;
	Microsoft::WRL::ComPtr<ID3D10Blob> computeShader_buffer;
};

class GeometryShader
{
public:
	bool Initialize(Microsoft::WRL::ComPtr<ID3D11Device>& device, std::wstring shaderpath);
	bool Initialize(
		Microsoft::WRL::ComPtr<ID3D11Device> device,
		_In_ LPCWSTR srcFile, _In_ LPCSTR entryPoint, _In_ LPCSTR profile);

	bool StreamOutputInitialize(
		Microsoft::WRL::ComPtr<ID3D11Device> device, 
		const D3D11_SO_DECLARATION_ENTRY* pDeclEntry,
		const UINT declSize);

	ID3D11GeometryShader* GetGeometryShader();
	ID3D11GeometryShader* GetSteamOutputGeometryShader();
	Microsoft::WRL::ComPtr<ID3D11GeometryShader>& GetStreamOutTest();
	ID3D10Blob* GetGeometryShaderBuffer();

protected:
	Microsoft::WRL::ComPtr<ID3D11GeometryShader> geometryShader;
	Microsoft::WRL::ComPtr<ID3D11GeometryShader> streamoutputGeometryShader;
	Microsoft::WRL::ComPtr<ID3D10Blob> geometryShader_buffer;


};