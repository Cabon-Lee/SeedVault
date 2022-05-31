#pragma once

/// 라이트가 2개 이상일 때 어떻게 처리할지 아직 고민이 완전히 끝나지 않음

/// 지향광의 처리
/// 지향광은 pos값이 없으므로 이를 단순히 Trasform에 부착해서 사용하면 거대한 SpotLight가 된다.
/// DirectX11에서는 이를 해결하기 위해 '경계구'라는 것을 두었는데, 
/// 쉽게 설명하면 Center와 Radius를 가지고 거대한 구면을 생각한 것이다.
/// pos는 이 거대한 구면의 표면으로 고정되고 dir은 내부의 Center를 향해 빛을 발한다
/// 이렇게 되면 항상 일정한 거리에서 내부에 조명을 쏠 수 있는 것이다

#include "D3D11Define.h"
#include "SimpleMath.h"

class RenderTargetView;
class DepthStencilView;

static const int SHDOWMAP_SIZE = 2048;

const unsigned int  DIRECTONAL_LIGHT = 0;
const unsigned int  POINT_LIGHT = 1;
const unsigned int  SPOT_LIGHT = 2;

const unsigned int MAX_SHADOW = 4;

class LightClass
{
public:
	LightClass();
	~LightClass();

public:	
	void BuildShadowMap(ID3D11Device* pDevice);

	void ClearShadowRenderTarget(ID3D11DeviceContext* deviceContext);

	// DepthStencilBuffer를 묶어준다
	void BindShadowMap(ID3D11DeviceContext* deviceContext);
	// RenderTargetView를 묶어준다
	void BindTexture(ID3D11DeviceContext* deviceContext);

	void LightUpdate(
		unsigned int lightIndex,
		unsigned int type,
		const DirectX::XMFLOAT3& pos,
		const DirectX::XMFLOAT3& dir,
		const DirectX::XMFLOAT3& color, float intensity,
		float falloffStart = 0.0f, float falloffEnd = 0.0f,	float spotPower = 0.0f,
		bool castShadow = false,
		bool isTextured = false, unsigned int textureIndex = 0);

	
	const DirectX::XMMATRIX& GetViewTM();
	const DirectX::XMMATRIX& GetProjTM();

	DirectX::XMMATRIX& GetViewProjTM();
	DirectX::XMMATRIX& GetShadowTM();

	const DirectX::SimpleMath::Matrix& GetLightWorldTM();
	const DirectX::XMFLOAT3& GetLightPos();
	DirectX::XMFLOAT3& GetYawPitchRoll();	// Get해서 내부 내용을 바꿀 수 있다

	std::shared_ptr<DepthStencilView> GetDepthBuffer() { return m_ShadowMapDepthStencil; };
	std::shared_ptr<RenderTargetView> GetShadowMapRenderTarget() { return m_ShadowMapRenderTarget; }

	bool GetIsTextured();				// 텍스쳐 라이트인가?
	unsigned int GetTextureIndex();		// 텍스쳐 몇 번째 텍스쳐를 쓰는가?

	unsigned int m_LightType;

private:
	void GetViewProjTM(unsigned int type, const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT3& dir);
	void DirectionalLightViewProjTM(const DirectX::XMFLOAT3& dir);
	void SpotLightViewProjTM(const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT3 dir);


private:
	// 값을 계속비교
	int m_LightIndex;

	// 그림자를 위한 DepthStancil
	std::shared_ptr<DepthStencilView> m_ShadowMapDepthStencil;
	// 텍스쳐 라이트를 위한 RenderTarget
	std::shared_ptr<RenderTargetView> m_ShadowMapRenderTarget;

	DirectX::SimpleMath::Matrix m_LightWorldTM;

	DirectX::XMMATRIX m_ZeroTM;
	DirectX::XMMATRIX m_ViewTM;
	DirectX::XMMATRIX m_ProjTM;
	DirectX::XMMATRIX m_ViewProjTM;
	DirectX::XMMATRIX m_ShadowTM;

	float m_DirectionalRadius;
	DirectX::XMFLOAT3 m_Center;
	DirectX::XMFLOAT3 m_Direction;
	DirectX::XMFLOAT3 m_LightPos;
	DirectX::XMFLOAT3 m_YawPitchRoll;

	bool m_IsTextured;
	unsigned int m_TextureIndex;
};

