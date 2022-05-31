#pragma once

#include <vector>
#include <memory>
#include "D3D11Define.h"

class LightClass;

const unsigned int MAX_LIGHT = 24;
const unsigned int MAX_TEXTURE_LIGHT = 1;

enum class eMAP_MODE
{
	TEXTURE = 0,
	SHADOW,
};

class VertexShader;
class PixelShader;
class ModelMesh;

class LightManager
{
public:
	LightManager();
	~LightManager();

	void Initialize(ID3D11Device* pDevice);
	void ClearShadowDepthStencil(ID3D11DeviceContext* pDeviceContext);

	void AddLightCount();
	void LightUpdate(
		unsigned int type,
		const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT3& dir, const DirectX::XMFLOAT3& color,
		float intensity, float falloffStart /*= 0.0f*/, 
		float falloffEnd /*= 0.0f*/, float spotPower /*= 0.0f*/,
		bool castShadow /*= false*/, bool isTextured /*= false*/, 
		unsigned int textureIndex /*= 0*/);

	void BindShadowMap(ID3D11DeviceContext* pDeviceContext);
	void BindTexture(ID3D11DeviceContext* pDeviceContext);

	std::shared_ptr<LightClass> GetLightPtr();
	std::shared_ptr<LightClass> GetLightPtr(unsigned int idx);
	std::shared_ptr<LightClass> GetShadowLightPtr(unsigned int idx);
	std::shared_ptr<LightClass> GetTextureLightPtr(unsigned int idx);

	void BuildShadowMap(std::shared_ptr<LightClass> pLight,
		std::shared_ptr<ModelMesh> pModel,
		const DirectX::XMMATRIX& modelWorldTM,
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext,
		bool skinned);

	void BuildTextureMap(std::shared_ptr<LightClass> pLight,
		std::shared_ptr<ModelMesh> pModel,
		const DirectX::XMMATRIX& modelWorldTM,
		const DirectX::XMMATRIX& viewTM,
		const DirectX::XMMATRIX& projTM,
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext,
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pShaderResourceView,
		std::shared_ptr<VertexShader> pVertexShader,
		std::shared_ptr<PixelShader> pPixelShader,
		bool skinned);


	ID3D11ShaderResourceView* GetShadowLightShaderResourceView(unsigned int idx);
	ID3D11ShaderResourceView* GetTextureLightShaderResourceView(unsigned int idx);

	void ShadowLightCountReset();			// �����츦 ����ϴ� ����Ʈ�� �� �ʱ�ȭ
	int GetLightCount();					
	unsigned int GetShadowLightCount();

	// ���� ���
	// BuildShadow�� ����, BuildTextureLight�� ����
	eMAP_MODE m_NowMappingMode;

private:
	std::vector<std::shared_ptr<LightClass>> m_Light_V;			// ��� ����Ʈ �迭
	std::vector<std::shared_ptr<LightClass>> m_ShadowLight_V;	// �׸��ڸ� ����ϴ� ����Ʈ �迭
	std::vector<std::shared_ptr<LightClass>> m_TextureLight_V;	// ����Ʈ �ؽ��ĸ� ����ϴ� ����Ʈ �迭
	int m_LightCount;											// ���� ���� ����
	unsigned int m_ShadowLightCount;							// �׸��ڸ� ������ ����Ʈ�� ��
	unsigned int m_TextureLightCount;							// �ؽ��� ����Ʈ�� ��
};

