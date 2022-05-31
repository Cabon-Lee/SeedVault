#include "LightManager.h"

#include "LightClass.h"
#include "DepthStencilView.h"
#include "RasterizerState.h"
#include "ConstantBufferManager.h"
#include "RenderTargetView.h"

#include "Shaders.h"
#include "NodeData.h"

LightManager::LightManager()
	: m_LightCount(-1)
	, m_ShadowLightCount(0)
	, m_TextureLightCount(0)
	, m_NowMappingMode(eMAP_MODE::SHADOW)
{
	m_ShadowLight_V.resize(MAX_SHADOW);
	m_TextureLight_V.resize(MAX_TEXTURE_LIGHT);
}

LightManager::~LightManager()
{

}

void LightManager::Initialize(ID3D11Device* pDevice)
{
	for (int i = 0; i < MAX_LIGHT; i++)
	{
		std::shared_ptr<LightClass> _newLight = std::make_shared<LightClass>();
		_newLight->BuildShadowMap(pDevice);
		m_Light_V.push_back(_newLight);
	}
}

void LightManager::ClearShadowDepthStencil(ID3D11DeviceContext* pDeviceContext)
{
	pDeviceContext->ClearDepthStencilView(
		m_Light_V[m_LightCount]->GetDepthBuffer()->GetDepthSetncilView(), 
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

void LightManager::AddLightCount()
{
	m_LightCount++;
}

void LightManager::LightUpdate(
	unsigned int type, 
	const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT3& dir, const DirectX::XMFLOAT3& color, 
	float intensity, float falloffStart /*= 0.0f*/, float falloffEnd /*= 0.0f*/, float spotPower /*= 0.0f*/, 
	bool castShadow /*= false*/, bool isTextured /*= false*/, unsigned int textureIndex /*= 0*/)
{
	m_Light_V[m_LightCount]->LightUpdate(
		m_LightCount, type, pos, dir, color, intensity, falloffStart, falloffEnd, spotPower, castShadow, isTextured, textureIndex);

	if (castShadow == true)
	{
		// 그림자가 생성되는 라이트 가져와서 자료구조에 넣는다

		m_ShadowLight_V[m_ShadowLightCount] = m_Light_V[m_LightCount];
		unsigned int _nowType = m_ShadowLight_V[m_ShadowLightCount]->m_LightType;

		switch (m_ShadowLightCount)
		{
		case 0: ConstantBufferManager::GetLightMatrixBuffer()->data.lightType.x = _nowType; break;
		case 1: ConstantBufferManager::GetLightMatrixBuffer()->data.lightType.y = _nowType; break;
		case 2: ConstantBufferManager::GetLightMatrixBuffer()->data.lightType.z = _nowType; break;
		case 3: ConstantBufferManager::GetLightMatrixBuffer()->data.lightType.w = _nowType; break;
		default:
			break;
		}

		// 라이트의 그림자 정보에 몇 번째 라이트맵에 묶였는지 알려준다.
		// shadow.xyzw를 의미
		ConstantBufferManager::GetPSDirLightBuffer()->data.lights[m_LightCount].shadowInfo.z = m_ShadowLightCount;


		// 라이트의 숫자를 늘리고 쉐이더에 넘겨준다
		m_ShadowLightCount++;

		ConstantBufferManager::GetLightMatrixBuffer()->data.lightCount = m_ShadowLightCount;
	}

	// 텍스쳐가 있으면 shadowInfo의 마지막에 값을 1로 해서 표시를 해준다
	if (isTextured == true) 
	{ 
		m_TextureLightCount++;
		ConstantBufferManager::GetPSDirLightBuffer()->data.lights[m_LightCount].shadowInfo.w = 1; 
		m_TextureLight_V[MAX_TEXTURE_LIGHT - 1] = m_Light_V[m_LightCount];
	}
	else 
	{ ConstantBufferManager::GetPSDirLightBuffer()->data.lights[m_LightCount].shadowInfo.w = 0; }

}

void LightManager::BindShadowMap(ID3D11DeviceContext* pDeviceContext)
{
	m_Light_V[m_LightCount]->BindShadowMap(pDeviceContext);
}

void LightManager::BindTexture(ID3D11DeviceContext* pDeviceContext)
{
	m_Light_V[m_LightCount]->BindTexture(pDeviceContext);
}

std::shared_ptr<LightClass> LightManager::GetLightPtr()
{
	return m_Light_V[m_LightCount];
}

std::shared_ptr<LightClass> LightManager::GetLightPtr(unsigned int idx)
{
	return m_Light_V[idx];
}

std::shared_ptr<LightClass> LightManager::GetShadowLightPtr(unsigned int idx)
{
	return m_ShadowLight_V[idx];
}

std::shared_ptr<LightClass> LightManager::GetTextureLightPtr(unsigned int idx)
{
	return m_TextureLight_V[idx];
}

void LightManager::BuildShadowMap(
	std::shared_ptr<LightClass> pLight, 
	std::shared_ptr<ModelMesh> pModel, const DirectX::XMMATRIX& modelWorldTM, 
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext, 
	bool skinned)
{
	UINT offset = 0;

	// 이거 안하면 구멍 뚫려서 나옴
	pDeviceContext->RSSetState(RasterizerState::GetDepthRS());
	pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	pDeviceContext->PSSetShader(nullptr, NULL, 0);

	{
		if (skinned == true)
		{
			// EndAnimation에서 미리 계산한 OffsetTM을 저장해준다
			pModel->SetBoneOffsetToBuffer(ConstantBufferManager::GetVSBoneBuffer()->data.boneOffset);
			ConstantBufferManager::GetVSBoneBuffer()->ApplyChanges();
			pDeviceContext->VSSetConstantBuffers(1, 1, ConstantBufferManager::GetVSBoneBuffer()->GetAddressOf());

			// 모델의 월드행렬을 곱해줘야함, 안곱해주면 그림자가 가만히 있음
			ConstantBufferManager::GetWVPMatrix()->data.wvpMatrix = modelWorldTM * pLight->GetViewProjTM();
			ConstantBufferManager::GetWVPMatrix()->ApplyChanges();
			pDeviceContext->VSSetConstantBuffers(0, 1, ConstantBufferManager::GetWVPMatrix()->GetAddressOf());
		}

		for (auto& _nowNode : *pModel->m_pNodeData_V)
		{

			if (skinned != true)
			{

				_nowNode->CalculateTransform(modelWorldTM);

				// 모델의 월드행렬을 곱해줘야함, 안곱해주면 그림자가 가만히 있음
				ConstantBufferManager::GetWVPMatrix()->data.wvpMatrix = _nowNode->animationTM * pLight->GetViewProjTM();
				ConstantBufferManager::GetWVPMatrix()->ApplyChanges();
				pDeviceContext->VSSetConstantBuffers(0, 1, ConstantBufferManager::GetWVPMatrix()->GetAddressOf());
			}

			pDeviceContext->IASetVertexBuffers(0, 1, _nowNode->GetVertexAddressOf(), _nowNode->StridePtr(), &offset);
			pDeviceContext->IASetIndexBuffer(_nowNode->m_IndexBuffer.Get(),	DXGI_FORMAT::DXGI_FORMAT_R32_UINT, 0);
			pDeviceContext->DrawIndexed(_nowNode->m_IndexCount, 0, 0);
		}
	}
}

void LightManager::BuildTextureMap(
	std::shared_ptr<LightClass> pLight, 
	std::shared_ptr<ModelMesh> pModel, 
	const DirectX::XMMATRIX& modelWorldTM, 
	const DirectX::XMMATRIX& viewTM,
	const DirectX::XMMATRIX& projTM,
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext, 
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pShaderResourceView,
	std::shared_ptr<VertexShader> pVertexShader, std::shared_ptr<PixelShader> pPixelShader, bool skinned)
{
	UINT offset = 0;

	// 이거 안하면 구멍 뚫려서 나옴
	pDeviceContext->RSSetState(RasterizerState::GetDepthRS());
	pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// 모델의 월드행렬을 곱해줘야함, 안곱해주면 그림자가 가만히 있음
	ConstantBufferManager::GetWVPMatrix()->data.wvpMatrix = modelWorldTM * pLight->GetViewProjTM();
	ConstantBufferManager::GetWVPMatrix()->ApplyChanges();

	pDeviceContext->VSSetConstantBuffers(0, 1, ConstantBufferManager::GetWVPMatrix()->GetAddressOf());

	std::shared_ptr<VertexShader> _vertexShader;

	// 라이트 쉐도우가 아니라 텍스쳐면
	if (pLight->GetIsTextured() == true)
	{
		if (skinned == true)
		{
			// EndAnimation에서 미리 계산한 OffsetTM을 저장해준다
			pModel->SetBoneOffsetToBuffer(ConstantBufferManager::GetVSBoneBuffer()->data.boneOffset);
			ConstantBufferManager::GetVSBoneBuffer()->ApplyChanges();
			pDeviceContext->VSSetConstantBuffers(2, 1, ConstantBufferManager::GetVSBoneBuffer()->GetAddressOf());
		}

		// 모델 관련 행렬
		ConstantBufferManager::GetVertexBuffer()->data.worldMatrix = modelWorldTM;
		ConstantBufferManager::GetVertexBuffer()->data.wvpMatrix = modelWorldTM * viewTM * projTM;
		ConstantBufferManager::GetVertexBuffer()->ApplyChanges();
		pDeviceContext->VSSetConstantBuffers(1, 1, ConstantBufferManager::GetVertexBuffer()->GetAddressOf());

		

		pDeviceContext->PSSetShaderResources(0, 1, pShaderResourceView.GetAddressOf());
		pDeviceContext->PSSetSamplers(0, 1, RasterizerState::GetLinearSamplerStateAddressOf());

		pDeviceContext->IASetInputLayout(pVertexShader->inputLayout.Get());
		pDeviceContext->VSSetShader(pVertexShader->GetVertexShader(), NULL, 0);
		pDeviceContext->PSSetShader(pPixelShader->GetPixelShader(), NULL, 0);

		for (auto& _nowNode : *pModel->m_pNodeData_V)
		{
			pDeviceContext->IASetVertexBuffers(0, 1, _nowNode->GetVertexAddressOf(), _nowNode->StridePtr(), &offset);

			pDeviceContext->IASetIndexBuffer(
				_nowNode->m_IndexBuffer.Get(),
				DXGI_FORMAT::DXGI_FORMAT_R32_UINT, 0);

			pDeviceContext->DrawIndexed(_nowNode->m_IndexCount, 0, 0);
		}
	}
}

ID3D11ShaderResourceView* LightManager::GetShadowLightShaderResourceView(unsigned int idx)
{
	// 최대 3이 들어온다
	if (idx <= m_ShadowLightCount - 1 && m_ShadowLightCount != 0)
	{
		return m_ShadowLight_V[idx]->GetDepthBuffer()->GetShaderResourceViewRawptr();
	}
	else 
	{
		return nullptr;	// 없으면 null값을 리턴하여 쉐이더에 아무것도 안넘긴다
	}
}

ID3D11ShaderResourceView* LightManager::GetTextureLightShaderResourceView(unsigned int idx)
{
	if (m_TextureLightCount > 0)
	{
		return m_TextureLight_V[idx]->GetShadowMapRenderTarget()->GetShaderResourceViewRawptr();
	}
	else
	{
		return nullptr;
	}
}

void LightManager::ShadowLightCountReset()
{
	// m_LightCount 사용하는 라이트의 수, 접근할 인덱스와 관련 있다
	// 라이트가 업데이트 될 때 ++ 되며, 이를 통해 GetLightPtr()을 가장 최근에 들어간 라이트의 인덱스에 접근하게 된다
	m_LightCount = -1;			
	m_ShadowLightCount = 0;	// 그림자를 사용하는 라이트의 수
	m_TextureLightCount = 0;
}

int LightManager::GetLightCount()
{
	return m_LightCount;
}

unsigned int LightManager::GetShadowLightCount()
{
	return m_ShadowLightCount;
}

