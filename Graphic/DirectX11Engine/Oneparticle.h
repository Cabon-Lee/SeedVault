#pragma once
#include "Particle.h"

class Oneparticle
{
public:
	Oneparticle();
	~Oneparticle();

public:
	bool Initialize(Microsoft::WRL::ComPtr<ID3D11Device> pDevice, Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext);
	bool Update(float dTime) {};
	void Render();

	ID3D11Buffer* const* GetVertexAddressOf()
	{
		return m_vertexBuffer.GetAddressOf();
	}

	Microsoft::WRL::ComPtr<ID3D11Buffer> GetVertexBuffer()
	{
		return m_vertexBuffer;
	}

	ID3D11Buffer* GetIndexBuffer()
	{
		return m_indexBuffer.Get();
	}

	const UINT GetIndexCount()
	{
		return m_IndexCount;
	}

	const UINT* StridePtr() const
	{
		return &this->m_Stride;
	}
	
	//void SetBuffer(ID3D11Device* device);

private:
	bool InitializeBuffers();

private:
	Microsoft::WRL::ComPtr<ID3D11Device> m_pDevice;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_pDeviceContext;
	std::shared_ptr<ID3D11ShaderResourceView> m_texture;

	//ParticleType* m_particleList = nullptr;

	int m_vertexCount = 1;
	int m_indexCount = 0;

	Vertex::VertexColorTexture* m_pVertices;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_vertexBuffer = nullptr;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_indexBuffer = nullptr;

	UINT m_Stride;
	UINT m_IndexCount;
};

