#include "pch.h"

#include "ConstantBufferManager.h"
#include "COMException.h"

ConstantBuffer<CB_VS_vertexshader>* ConstantBufferManager::m_vs_vertexshader = nullptr;
ConstantBuffer<CB_PS_light>* ConstantBufferManager::m_ps_light = nullptr;
ConstantBuffer<CB_PS_DirectionalLight>* ConstantBufferManager::m_ps_DirLight = nullptr;
ConstantBuffer<CB_VS_PER_OBJECT>* ConstantBufferManager::m_vs_PerObject = nullptr;
ConstantBuffer<CB_PS_PER_OBJECT>* ConstantBufferManager::m_ps_PerObject = nullptr;
ConstantBuffer<CB_PS_Material>* ConstantBufferManager::m_ps_Material = nullptr;
ConstantBuffer<CB_VS_BoneOffset>* ConstantBufferManager::m_vs_BoneOffset = nullptr;


void ConstantBufferManager::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	m_vs_vertexshader = new ConstantBuffer<CB_VS_vertexshader>();
	m_ps_light = new ConstantBuffer<CB_PS_light>();
	m_ps_DirLight = new ConstantBuffer<CB_PS_DirectionalLight>();
	m_vs_PerObject = new ConstantBuffer<CB_VS_PER_OBJECT>();
	m_ps_PerObject = new ConstantBuffer<CB_PS_PER_OBJECT>();
	m_ps_Material = new ConstantBuffer<CB_PS_Material>();
	m_vs_BoneOffset = new ConstantBuffer<CB_VS_BoneOffset>();

	try
	{
		HRESULT hr;

		//상수 버퍼
		hr = m_vs_vertexshader->Initialize(pDevice, pDeviceContext);
		COM_ERROR_IF_FAILED(hr, "Create VertexBuffer Fail");

		hr = m_ps_light->Initialize(pDevice, pDeviceContext);
		COM_ERROR_IF_FAILED(hr, "Create Light VertexBuffer Fail");

		m_ps_light->data.ambientLightColor = DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f);
		m_ps_light->data.ambientLightStrength = 1.0f;

		// 지향광 세팅 HLSL 프로그래밍을 카피
		{
			hr = m_ps_DirLight->Initialize(pDevice, pDeviceContext);
			COM_ERROR_IF_FAILED(hr, "Create Directional Light ConstantBuffer Fail");
		}

		// 쉐이더 상수버퍼 초기화
		{
			m_vs_PerObject->Initialize(pDevice, pDeviceContext);
			COM_ERROR_IF_FAILED(hr, "Create Object Per ConstantBuffer Fail");
		}

		{
			m_ps_PerObject->Initialize(pDevice, pDeviceContext);
			COM_ERROR_IF_FAILED(hr, "Create Object Per ConstantBuffer Fail");
		}

		{
			m_ps_Material->Initialize(pDevice, pDeviceContext);
			COM_ERROR_IF_FAILED(hr, "Create Object Per ConstantBuffer Fail");

		}

		{
			m_vs_BoneOffset->Initialize(pDevice, pDeviceContext);
			COM_ERROR_IF_FAILED(hr, "Create Object Per ConstantBuffer Fail");

		}


	}
	catch (COMException& exception)
	{
		ErrorLogger::Log(exception);
	}
}

ConstantBuffer<CB_VS_vertexshader>* ConstantBufferManager::GetVertexBuffer()
{
	return m_vs_vertexshader;
}

ConstantBuffer<CB_PS_light>* ConstantBufferManager::GetPSLightBuffer()
{
	return m_ps_light;
}

ConstantBuffer<CB_PS_DirectionalLight>* ConstantBufferManager::GetPSDirLightBuffer()
{
	return m_ps_DirLight;
}

ConstantBuffer<CB_PS_Material>* ConstantBufferManager::GetPSMaterial()
{
	return m_ps_Material;
}

ConstantBuffer<CB_VS_PER_OBJECT>* ConstantBufferManager::GetVSPerObjectBuffer()
{
	return m_vs_PerObject;
}

ConstantBuffer<CB_PS_PER_OBJECT>* ConstantBufferManager::GetPSPerObjectBuffer()
{
	return m_ps_PerObject;
}

ConstantBuffer<CB_VS_BoneOffset>* ConstantBufferManager::GetVSBoneBuffer()
{
	return m_vs_BoneOffset;
}
