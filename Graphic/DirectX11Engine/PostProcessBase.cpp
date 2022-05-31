#include "PostProcessBase.h"

#include "Shaders.h"

// 상속받는 친구들은 모두 같은 쉐이더를 공유하므로
// 인스턴스를 매번 새로할 필요가 없어서 Static으로 만들었다
std::unique_ptr<VertexShader> PostProcessBase::m_pFullScreenQuadVS = nullptr;
std::unique_ptr<PixelShader> PostProcessBase::m_pFinalPassPS = nullptr;

PostProcessBase::PostProcessBase()
{

}

PostProcessBase::~PostProcessBase()
{

}

void PostProcessBase::CreateBaseShader(Microsoft::WRL::ComPtr<ID3D11Device> pDevice)
{
	//if (m_pFullScreenQuadVS == nullptr)
	{
		m_pFullScreenQuadVS = std::make_unique<VertexShader>();
		m_pFullScreenQuadVS->Initialize(pDevice, L"../Data/Shader/PostFX.hlsl", "FullScreenQuadVS", "vs_5_0");
	}

	//if (m_pFullScreenQuadVS == nullptr)
	{
		m_pFinalPassPS = std::make_unique<PixelShader>();
		m_pFinalPassPS->Initialize(pDevice, L"../Data/Shader/PostFX.hlsl", "FinalPassPS", "ps_5_0");
	}
}
