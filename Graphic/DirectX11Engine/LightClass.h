#pragma once

/// ����Ʈ�� 2�� �̻��� �� ��� ó������ ���� ����� ������ ������ ����

/// ���Ɽ�� ó��
/// ���Ɽ�� pos���� �����Ƿ� �̸� �ܼ��� Trasform�� �����ؼ� ����ϸ� �Ŵ��� SpotLight�� �ȴ�.
/// DirectX11������ �̸� �ذ��ϱ� ���� '��豸'��� ���� �ξ��µ�, 
/// ���� �����ϸ� Center�� Radius�� ������ �Ŵ��� ������ ������ ���̴�.
/// pos�� �� �Ŵ��� ������ ǥ������ �����ǰ� dir�� ������ Center�� ���� ���� ���Ѵ�
/// �̷��� �Ǹ� �׻� ������ �Ÿ����� ���ο� ������ �� �� �ִ� ���̴�

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

	// DepthStencilBuffer�� �����ش�
	void BindShadowMap(ID3D11DeviceContext* deviceContext);
	// RenderTargetView�� �����ش�
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
	DirectX::XMFLOAT3& GetYawPitchRoll();	// Get�ؼ� ���� ������ �ٲ� �� �ִ�

	std::shared_ptr<DepthStencilView> GetDepthBuffer() { return m_ShadowMapDepthStencil; };
	std::shared_ptr<RenderTargetView> GetShadowMapRenderTarget() { return m_ShadowMapRenderTarget; }

	bool GetIsTextured();				// �ؽ��� ����Ʈ�ΰ�?
	unsigned int GetTextureIndex();		// �ؽ��� �� ��° �ؽ��ĸ� ���°�?

	unsigned int m_LightType;

private:
	void GetViewProjTM(unsigned int type, const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT3& dir);
	void DirectionalLightViewProjTM(const DirectX::XMFLOAT3& dir);
	void SpotLightViewProjTM(const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT3 dir);


private:
	// ���� ��Ӻ�
	int m_LightIndex;

	// �׸��ڸ� ���� DepthStancil
	std::shared_ptr<DepthStencilView> m_ShadowMapDepthStencil;
	// �ؽ��� ����Ʈ�� ���� RenderTarget
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

