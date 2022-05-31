#include "ConstantBufferManager.h"

#include "COMException.h"

ConstantBuffer<CB_VS_WVPMatrix>* ConstantBufferManager::m_vs_WVPMatrix = nullptr;
ConstantBuffer<CB_VS_ShadowTM>* ConstantBufferManager::m_vs_ShadowMatrix = nullptr;

ConstantBuffer<CB_VS_vertexshader>* ConstantBufferManager::m_vs_vertexshader = nullptr;
ConstantBuffer<CB_VS_vertexshader_2d>* ConstantBufferManager::m_vs_vertexshader_2d = nullptr;
ConstantBuffer<CB_PS_light>* ConstantBufferManager::m_ps_light = nullptr;
ConstantBuffer<CB_PS_Lights>* ConstantBufferManager::m_ps_DirLight = nullptr;
ConstantBuffer<CB_PS_LightCount>* ConstantBufferManager::m_ps_lightCount = nullptr;
ConstantBuffer<CB_PS_LightMatrix>* ConstantBufferManager::m_ps_lightMatrix = nullptr;

ConstantBuffer<CB_PS_Amibent>* ConstantBufferManager::m_ps_ambient = nullptr;
ConstantBuffer<CB_VS_WVP>* ConstantBufferManager::m_vs_WVP = nullptr;

ConstantBuffer<CB_PS_Material>* ConstantBufferManager::m_ps_Material = nullptr;
ConstantBuffer<CB_PS_ObjectID>* ConstantBufferManager::m_ps_ObjectID = nullptr;
ConstantBuffer<CB_PS_ParticleEmissive>* ConstantBufferManager::m_ps_LagacyMaterial = nullptr;

ConstantBuffer<CB_VS_BoneOffset>* ConstantBufferManager::m_vs_BoneOffset = nullptr;
ConstantBuffer<CB_VS_PER_OBJECT>* ConstantBufferManager::m_vs_PerObject = nullptr;
ConstantBuffer<CB_PS_Camera>* ConstantBufferManager::m_ps_Camera = nullptr;
ConstantBuffer<CB_PS_ROUGHNESS>* ConstantBufferManager::m_ps_Roughness = nullptr;
ConstantBuffer<CB_CS_Bloom>* ConstantBufferManager::m_cs_Blur = nullptr;

ConstantBuffer<CB_GS_Paricle_PerFrame>* ConstantBufferManager::m_gs_ParticlePerFrame = nullptr;
ConstantBuffer<CB_GS_Particle_PerParticle>* ConstantBufferManager::m_gs_Particle = nullptr;
ConstantBuffer<CB_GS_PaticleProperty>* ConstantBufferManager::m_gs_ParticleProperty = nullptr;

ConstantBuffer<CB_VS_NormalR>* ConstantBufferManager::m_vs_normalR = nullptr;

ConstantBuffer<CB_PS_IDpicking>* ConstantBufferManager::m_vs_IDpicking = nullptr;

std::vector<Microsoft::WRL::ComPtr<ID3D11Buffer>>  ConstantBufferManager::m_GeneralCBuffer_V;

void ConstantBufferManager::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	m_vs_WVPMatrix = new ConstantBuffer<CB_VS_WVPMatrix>();
	m_vs_ShadowMatrix = new ConstantBuffer<CB_VS_ShadowTM>();
	m_vs_vertexshader = new ConstantBuffer<CB_VS_vertexshader>();
	m_vs_vertexshader_2d = new ConstantBuffer<CB_VS_vertexshader_2d>();
	m_vs_WVP = new ConstantBuffer<CB_VS_WVP>();
	m_ps_light = new ConstantBuffer<CB_PS_light>();
	m_ps_DirLight = new ConstantBuffer<CB_PS_Lights>();
	m_ps_lightCount = new ConstantBuffer<CB_PS_LightCount>();
	m_ps_lightMatrix = new ConstantBuffer<CB_PS_LightMatrix>();
	m_ps_ambient = new ConstantBuffer<CB_PS_Amibent>();
	m_ps_Material = new ConstantBuffer<CB_PS_Material>();
	m_ps_ObjectID = new ConstantBuffer<CB_PS_ObjectID>();
	m_ps_LagacyMaterial = new ConstantBuffer<CB_PS_ParticleEmissive>();
	m_vs_BoneOffset = new ConstantBuffer<CB_VS_BoneOffset>();
	m_vs_PerObject = new ConstantBuffer<CB_VS_PER_OBJECT>();
	m_ps_Camera = new ConstantBuffer<CB_PS_Camera>();
	m_ps_Roughness = new ConstantBuffer<CB_PS_ROUGHNESS>();
	m_cs_Blur = new ConstantBuffer<CB_CS_Bloom>();
	m_gs_ParticlePerFrame = new ConstantBuffer<CB_GS_Paricle_PerFrame>;
	m_gs_Particle = new ConstantBuffer<CB_GS_Particle_PerParticle>;
	m_vs_normalR = new ConstantBuffer<CB_VS_NormalR>;
	m_gs_ParticleProperty = new ConstantBuffer<CB_GS_PaticleProperty>;
	m_vs_IDpicking = new ConstantBuffer<CB_PS_IDpicking>;


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

		// 2D 상수버퍼
		{
			hr = m_vs_vertexshader_2d->Initialize(pDevice, pDeviceContext);
			COM_ERROR_IF_FAILED(hr, "Create VertexBuffer Fail");
		}

		// 지향광 세팅 HLSL 프로그래밍을 카피
		{
			hr = m_ps_DirLight->Initialize(pDevice, pDeviceContext);
			COM_ERROR_IF_FAILED(hr, "Create Directional Light ConstantBuffer Fail");
		}

		{
			hr = m_ps_lightCount->Initialize(pDevice, pDeviceContext);
			COM_ERROR_IF_FAILED(hr, "Create Light Count ConstantBuffer Fail");
		}

		{
			hr = m_ps_lightMatrix->Initialize(pDevice, pDeviceContext);
			COM_ERROR_IF_FAILED(hr, "Create Light Matrix ConstantBuffer Fail");
		}

		{
			hr = m_ps_ambient->Initialize(pDevice, pDeviceContext);
			COM_ERROR_IF_FAILED(hr, "Create Ambient ConstantBuffer Fail");
		}

		{
			hr = m_vs_WVP->Initialize(pDevice, pDeviceContext);
			COM_ERROR_IF_FAILED(hr, "Create skyShpere ConstantBuffer Fail");
		}
		
		// 쉐이더 상수버퍼 초기화
		{
			hr = m_vs_WVPMatrix->Initialize(pDevice, pDeviceContext);
			COM_ERROR_IF_FAILED(hr, "Create Object Per ConstantBuffer Fail");
		}

		{
			hr = m_vs_ShadowMatrix->Initialize(pDevice, pDeviceContext);
			COM_ERROR_IF_FAILED(hr, "Create Object Per ConstantBuffer Fail");
		}

		{
			hr = m_vs_PerObject->Initialize(pDevice, pDeviceContext);
			COM_ERROR_IF_FAILED(hr, "Create Object Per ConstantBuffer Fail");
		}

		{
			hr = m_ps_Camera->Initialize(pDevice, pDeviceContext);
			COM_ERROR_IF_FAILED(hr, "Create Object Per ConstantBuffer Fail");
		}

		{
			hr = m_ps_Material->Initialize(pDevice, pDeviceContext);
			COM_ERROR_IF_FAILED(hr, "Create Object Per ConstantBuffer Fail");
		}

		{
			hr = m_ps_ObjectID->Initialize(pDevice, pDeviceContext);
			COM_ERROR_IF_FAILED(hr, "Create Object ID ConstantBuffer Fail");
		}
		
		{
			hr = m_ps_LagacyMaterial->Initialize(pDevice, pDeviceContext);
			COM_ERROR_IF_FAILED(hr, "Create Lagacy Material Per ConstantBuffer Fail");
		}

		{
			hr = m_vs_BoneOffset->Initialize(pDevice, pDeviceContext);
			COM_ERROR_IF_FAILED(hr, "Create Object Per ConstantBuffer Fail");
		}

		{
			hr = m_ps_Roughness->Initialize(pDevice, pDeviceContext);
			COM_ERROR_IF_FAILED(hr, "Create Object Per ConstantBuffer Fail");
		}

		{
			hr = m_cs_Blur->Initialize(pDevice, pDeviceContext);
			COM_ERROR_IF_FAILED(hr, "Create Object Per ConstantBuffer Fail");
		}

		//OIT
		{
			hr = m_gs_ParticlePerFrame->Initialize(pDevice, pDeviceContext);
			COM_ERROR_IF_FAILED(hr, "Create Object Per ConstantBuffer Fail");
		}

		{
			hr = m_gs_Particle->Initialize(pDevice, pDeviceContext);
			COM_ERROR_IF_FAILED(hr, "Create Particle ConstantBuffer Fail");
		}

		{
			hr = m_vs_normalR->Initialize(pDevice, pDeviceContext);
			COM_ERROR_IF_FAILED(hr, "Create normalR ConstantBuffer Fail");
		}
		
		{
			hr = m_gs_ParticleProperty->Initialize(pDevice, pDeviceContext);
			COM_ERROR_IF_FAILED(hr, "Create m_gs_ParticleProperty ConstantBuffer Fail");
		}

		{
			hr = m_vs_IDpicking->Initialize(pDevice, pDeviceContext);
			COM_ERROR_IF_FAILED(hr, "Create normalR ConstantBuffer Fail");
		}

		m_GeneralCBuffer_V.resize(16);
		for (unsigned int i = 0; i < 16; i++)
		{
			UINT _nowSize = ((i + 1) * 4) * 4;

			D3D11_BUFFER_DESC desc;
			desc.Usage = D3D11_USAGE_DYNAMIC;
			desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			desc.MiscFlags = 0;
			desc.ByteWidth = _nowSize;
			desc.StructureByteStride = 0;

			hr = pDevice->CreateBuffer(&desc, 0, m_GeneralCBuffer_V[i].GetAddressOf());
			COM_ERROR_IF_FAILED(hr, "Create General ConstantBuffer Fail");
		}

	}
	catch (COMException& exception)
	{
		ErrorLogger::Log(exception);
	}
}

void ConstantBufferManager::SetGeneralCBuffer(
	Microsoft::WRL::ComPtr <ID3D11DeviceContext> pDeviceContext, 
	CBufferType eType, 
	CBufferSize eSize,
	UINT regNum, float* pData)
{
	unsigned int _nowSize = static_cast<unsigned int>(eSize);
	auto _nowBuffer = m_GeneralCBuffer_V[_nowSize];
	_nowSize = (_nowSize + 1) * 16;
	try
	{
		HRESULT _hr;

		D3D11_MAPPED_SUBRESOURCE mappedResource;
		_hr = pDeviceContext->Map(_nowBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

		COM_ERROR_IF_FAILED(_hr, "General ConstantBuffer Map Fail");
		
		CopyMemory(mappedResource.pData, pData, _nowSize);
		pDeviceContext->Unmap(_nowBuffer.Get(), 0);
	}
	catch (COMException& e)
	{
		ErrorLogger::Log(e);
	}

	switch (eType)
	{
	case CBufferType::VS:
		pDeviceContext->VSSetConstantBuffers(regNum, 1, _nowBuffer.GetAddressOf());
		break;
	case CBufferType::GS:
		pDeviceContext->GSSetConstantBuffers(regNum, 1, _nowBuffer.GetAddressOf());
		break;
	case CBufferType::PS:
		pDeviceContext->PSSetConstantBuffers(regNum, 1, _nowBuffer.GetAddressOf());
		break;
	case CBufferType::CS:
		pDeviceContext->CSSetConstantBuffers(regNum, 1, _nowBuffer.GetAddressOf());
		break;
	}
}

ConstantBuffer<CB_VS_WVPMatrix>* ConstantBufferManager::GetWVPMatrix()
{
	return m_vs_WVPMatrix;
}

ConstantBuffer<CB_VS_ShadowTM>* ConstantBufferManager::GetShadowTM()
{
	return m_vs_ShadowMatrix;
}

ConstantBuffer<CB_VS_vertexshader>* ConstantBufferManager::GetVertexBuffer()
{
	return m_vs_vertexshader;
}

ConstantBuffer<CB_VS_vertexshader_2d>* ConstantBufferManager::GetVertexBuffer2D()
{
	return m_vs_vertexshader_2d;
}

ConstantBuffer<CB_PS_light>* ConstantBufferManager::GetPSLightBuffer()
{
	return m_ps_light;
}

ConstantBuffer<CB_PS_Lights>* ConstantBufferManager::GetPSDirLightBuffer()
{
	return m_ps_DirLight;
}

ConstantBuffer<CB_PS_LightCount>* ConstantBufferManager::GetLightCountBuffer()
{
	return m_ps_lightCount;
}

ConstantBuffer<CB_PS_LightMatrix>* ConstantBufferManager::GetLightMatrixBuffer()
{
	return m_ps_lightMatrix;
}

ConstantBuffer<CB_PS_Amibent>* ConstantBufferManager::GetPSAmbientBuffer()
{
	return m_ps_ambient;
}

ConstantBuffer<CB_PS_Material>* ConstantBufferManager::GetPSMaterial()
{
	return m_ps_Material;
}

ConstantBuffer<CB_PS_ObjectID>* ConstantBufferManager::GetPSObjectID()
{
	return m_ps_ObjectID;
}

ConstantBuffer<CB_PS_ParticleEmissive>* ConstantBufferManager::GetPSParticleEmissive()
{
	return m_ps_LagacyMaterial;
}

ConstantBuffer<CB_GS_PaticleProperty>* ConstantBufferManager::GetGSParticleProperty()
{
	return m_gs_ParticleProperty;
}

ConstantBuffer<CB_VS_WVP>* ConstantBufferManager::GetVSWVPBuffer()
{
	return m_vs_WVP;
}

ConstantBuffer<CB_VS_PER_OBJECT>* ConstantBufferManager::GetVSPerObjectBuffer()
{
	return m_vs_PerObject;
}

ConstantBuffer<CB_PS_Camera>* ConstantBufferManager::GetPSCamera()
{
	return m_ps_Camera;
}

ConstantBuffer<CB_PS_ROUGHNESS>* ConstantBufferManager::GetPSRoughnessBuffer()
{
	return m_ps_Roughness;
}

ConstantBuffer<CB_VS_BoneOffset>* ConstantBufferManager::GetVSBoneBuffer()
{
	return m_vs_BoneOffset;
}

ConstantBuffer<CB_CS_Bloom>* ConstantBufferManager::GetCSBloomBuffer()
{
	return m_cs_Blur;
}

ConstantBuffer<CB_GS_Paricle_PerFrame>* ConstantBufferManager::GetGSParticlePerFrameBuffer()
{
	return m_gs_ParticlePerFrame;
}

ConstantBuffer<CB_GS_Particle_PerParticle>* ConstantBufferManager::GetGSParticleBuffer()
{
	return m_gs_Particle;
}

ConstantBuffer<CB_VS_NormalR>* ConstantBufferManager::GetVSNormalRBuffer()
{
	return m_vs_normalR;
}

ConstantBuffer<CB_PS_IDpicking>* ConstantBufferManager::GetIDpickingBuffer()
{
	return m_vs_IDpicking;
}
