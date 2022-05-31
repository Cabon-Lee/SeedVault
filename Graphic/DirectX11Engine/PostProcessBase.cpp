#include "PostProcessBase.h"

#include "Shaders.h"

// ��ӹ޴� ģ������ ��� ���� ���̴��� �����ϹǷ�
// �ν��Ͻ��� �Ź� ������ �ʿ䰡 ��� Static���� �������
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
