#pragma once
#include <d3d11.h>
#include <wrl.h>
#include <memory>
#include <vector>
#include "VertexHelper.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"

class ParticleInfo;

class Particle
{
public:
	Particle();
	~Particle();

public:
	bool Initialize(Microsoft::WRL::ComPtr<ID3D11Device> pDevice, Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext);
	void Render(ParticleInfo* m_particleList);

	int GetIndexCount();

private:
	bool InitializeBuffers();
	bool UpdateBuffers(ParticleInfo* m_particleList);

private:
	Microsoft::WRL::ComPtr<ID3D11Device> m_pDevice;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_pDeviceContext;
	std::shared_ptr<ID3D11ShaderResourceView> m_texture;

	int m_vertexCount = 0;
	int m_indexCount = 0;

	int m_maxParticles;

	Vertex::VertexColorTexture* m_pVertices;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_vertexBuffer = nullptr;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_indexBuffer = nullptr;

};

