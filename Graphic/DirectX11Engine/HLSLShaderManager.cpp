#include "HLSLShaderManager.h"

#include "Shaders.h"

HLSLShaderManager::HLSLShaderManager()
{

}

HLSLShaderManager::~HLSLShaderManager()
{

}

void HLSLShaderManager::Initialize(Microsoft::WRL::ComPtr<ID3D11Device> pDevice)
{
	m_pDevice = pDevice;


	m_DebugTestShader = std::make_shared<PixelShader>();
	m_DebugTestShader->Initialize(m_pDevice, L"../Data/Shader/ps_deferredTotal.hlsl", "AlbedoMain", "ps_5_0");
}
