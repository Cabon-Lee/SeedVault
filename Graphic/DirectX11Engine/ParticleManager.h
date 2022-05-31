#pragma once

#include <queue>
#include <utility>
#include "D3D11Define.h"

// Geometry�� �� Point�� ������� ����
struct ParticleCore
{
	DirectX::XMFLOAT3 initialPos;	// �ʱ� ��ġ
	DirectX::XMFLOAT3 initialVel;	// �ʱ� �ӵ�
	DirectX::XMFLOAT2 size;
	DirectX::XMFLOAT2 age;			// ���� �ð�
	unsigned int Type;
};

// �� ���۸� ���ΰ� �ִ� �ϳ��� ��ƼŬ ����
struct ParticleElement
{
	void Initialize(Microsoft::WRL::ComPtr<ID3D11Device> pDevice);
	void Reset();

	unsigned int index;
	bool isRun;
	bool isFirstRun;
	
	ParticleCore particleCore;
	struct ParticleProperty* particleProperty;
	
	ID3D11ShaderResourceView* pTexture;

	ID3D11Buffer* pInitVB;
	ID3D11Buffer* pDrawVB;
	ID3D11Buffer* pSOBuffer;
};

const unsigned int PARTICLE_EMITTER = 256;
const unsigned int MAX_PARTICLE = 1024;

class ParticleManager
{
public:
	ParticleManager();
	~ParticleManager();

	void Initialize(
		Microsoft::WRL::ComPtr<ID3D11Device> pDevice,
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext,
		class GeometryShader* pShader,
		class VertexShader* pVShader);

	void BuildRandomTexture(Microsoft::WRL::ComPtr<ID3D11Device> pDevice);

	unsigned int AddParticle(
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext,
		ID3D11ShaderResourceView* pTexture,
		struct ParticleProperty* particle);

	void UpdateParticle(float dTime);

	void DrawParticle(
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext,
		class VertexShader* pStreamOutVS,
		class GeometryShader* pStreamOutGS,
		class VertexShader* pDrawVS,
		class GeometryShader* pDrawGS,
		class PixelShader* pDrawPS,
		const DirectX::XMFLOAT3& eyePos,
		const DirectX::XMMATRIX& viewTM,
		const DirectX::XMMATRIX& projTM);

private:
	std::shared_ptr<class GeometryShader> m_StreamOutGeometryShader;
	std::shared_ptr<class GeometryShader> m_StreamOutGeometryShaderTest;
	Microsoft::WRL::ComPtr <ID3D11InputLayout> m_pParticleInputLayout;
	Microsoft::WRL::ComPtr <ID3D11ShaderResourceView> m_pRandomTextureSRV;

	std::queue<std::shared_ptr<ParticleElement>> m_WaitingParticle_Q;
	std::queue<std::shared_ptr<ParticleElement>> m_ActiveParticle_Q;
	std::queue<std::shared_ptr<ParticleElement>> m_RenderParticle_Q;


	std::shared_ptr<ParticleElement> m_aParticle;


	float m_DeltaTime;
	float m_Total;
	unsigned int m_ParticleCount;
};

