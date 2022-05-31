#include "Particle.h"
#include "DirectXMath.h"
#include "UIData.h"

Particle::Particle()
{
}

Particle::~Particle()
{
}

bool Particle::Initialize(Microsoft::WRL::ComPtr<ID3D11Device> pDevice, Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext)
{
	m_pDevice = pDevice;
	m_pDeviceContext = pDeviceContext;

	// ��ƼŬ�� �������ϴ� �� ����� ���۸� ����ϴ�.
	if (!InitializeBuffers())
	{
		return false;
	}

	return true;
}

void Particle::Render(ParticleInfo* m_particleList)
{
	UpdateBuffers(m_particleList);

	// ���� ���� ���� �� �������� �����մϴ�.
	unsigned int stride = sizeof(Vertex::VertexColorTexture);
	unsigned int offset = 0;

	// ������ �� �� �ֵ��� �Է� ��������� ���� ���۸� Ȱ������ �����մϴ�.
	m_pDeviceContext->IASetVertexBuffers(0, 1, m_vertexBuffer.GetAddressOf(), &stride, &offset);

	// ������ �� �� �ֵ��� �Է� ��������� �ε��� ���۸� Ȱ������ �����մϴ�.
	m_pDeviceContext->IASetIndexBuffer(m_indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

	// �� ���� ���ۿ��� ������ �Ǿ�� �ϴ� ������Ƽ�� ������ �����մϴ�.
	m_pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}


bool Particle::InitializeBuffers()
{
	// ���� �迭�� �ִ� ���� ���� �����մϴ�.
	m_vertexCount = m_maxParticles * 6;

	// �ε��� �迭�� �ִ� �ε��� ���� �����մϴ�.
	m_indexCount = m_vertexCount;

	// ������ �� ���ڿ� ���� ���� �迭�� ����ϴ�.
	m_pVertices = new Vertex::VertexColorTexture[m_vertexCount];
	if (!m_pVertices)
	{
		return false;
	}

	// �ε��� �迭�� ����ϴ�.
	unsigned long* indices = new unsigned long[m_indexCount];
	if (!indices)
	{
		return false;
	}

	// ó������ ���� �迭�� 0���� �ʱ�ȭ�մϴ�.
	memset(m_pVertices, 0, (sizeof(Vertex::VertexColorTexture) * m_vertexCount));

	// �ε��� �迭�� �ʱ�ȭ�մϴ�.
	for (int i = 0; i < m_indexCount; i++)
	{
		indices[i] = i;
	}

	// ���� ���� ������ ������ �����Ѵ�.
	D3D11_BUFFER_DESC vertexBufferDesc;
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof(Vertex::VertexColorTexture) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	//D3D11_BUFFER_DESC vertexBufferDesc;
	//vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	//vertexBufferDesc.ByteWidth = sizeof(Vertex::VertexColorTexture) * m_vertexCount;
	//vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	//vertexBufferDesc.CPUAccessFlags = 0;
	//vertexBufferDesc.MiscFlags = 0;
	//vertexBufferDesc.StructureByteStride = 0;

	// subresource ������ ���� �����Ϳ� ���� �����͸� �����մϴ�.
	D3D11_SUBRESOURCE_DATA vertexData;
	vertexData.pSysMem = m_pVertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// ���� ��ħ�� ���� ���۸� ����ϴ�.
	if (FAILED(m_pDevice->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer)))
	{
		return false;
	}

	// ���� �ε��� ������ ������ �����մϴ�.
	D3D11_BUFFER_DESC indexBufferDesc;
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// ���� ���ҽ� ������ �ε��� �����Ϳ� ���� �����͸� �����մϴ�.
	D3D11_SUBRESOURCE_DATA indexData;
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// �ε��� ���۸� ����ϴ�.
	if (FAILED(m_pDevice->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer)))
	{
		return false;
	}

	// �� �̻� �ʿ����� �����Ƿ� �ε��� �迭�� �����մϴ�.
	delete[] indices;
	indices = 0;

	return true;
}


bool Particle::UpdateBuffers(ParticleInfo* m_particleList)
{
	// ó������ ���� �迭�� 0���� �ʱ�ȭ�մϴ�.
	Vertex::VertexColorTexture* verticesPtr;
	memset(m_pVertices, 0, (sizeof(Vertex::VertexColorTexture) * m_vertexCount));

	// ���� ��ƼŬ ��� �迭���� ���� �迭�� ����ϴ�. �� ��ƼŬ�� �� ���� �ﰢ������ ������� �����Դϴ�.
	int index = 0;
	float _particleSize = m_particleList->particleSize;

	for (int i = 0; i < m_particleList->currentParticleCount; i++)
	{
		// ���� �Ʒ�.
		m_pVertices[index].pos = XMFLOAT3(m_particleList[i].positionX - _particleSize, m_particleList[i].positionY - _particleSize, m_particleList[i].positionZ);
		m_pVertices[index].texture = XMFLOAT2(0.0f, 1.0f);
		m_pVertices[index].rgba = XMFLOAT4(m_particleList[i].red, m_particleList[i].green, m_particleList[i].blue, 1.0f);
		index++;

		// ���� ��.
		m_pVertices[index].pos = XMFLOAT3(m_particleList[i].positionX - _particleSize, m_particleList[i].positionY + _particleSize, m_particleList[i].positionZ);
		m_pVertices[index].texture = XMFLOAT2(0.0f, 0.0f);
		m_pVertices[index].rgba = XMFLOAT4(m_particleList[i].red, m_particleList[i].green, m_particleList[i].blue, 1.0f);
		index++;

		// ������ �Ʒ�.
		m_pVertices[index].pos = XMFLOAT3(m_particleList[i].positionX + _particleSize, m_particleList[i].positionY - _particleSize, m_particleList[i].positionZ);
		m_pVertices[index].texture = XMFLOAT2(1.0f, 1.0f);
		m_pVertices[index].rgba = XMFLOAT4(m_particleList[i].red, m_particleList[i].green, m_particleList[i].blue, 1.0f);
		index++;

		// ������ �Ʒ�.
		m_pVertices[index].pos = XMFLOAT3(m_particleList[i].positionX + _particleSize, m_particleList[i].positionY - _particleSize, m_particleList[i].positionZ);
		m_pVertices[index].texture = XMFLOAT2(1.0f, 1.0f);
		m_pVertices[index].rgba = XMFLOAT4(m_particleList[i].red, m_particleList[i].green, m_particleList[i].blue, 1.0f);
		index++;

		// ���� ��.
		m_pVertices[index].pos = XMFLOAT3(m_particleList[i].positionX - _particleSize, m_particleList[i].positionY + _particleSize, m_particleList[i].positionZ);
		m_pVertices[index].texture = XMFLOAT2(0.0f, 0.0f);
		m_pVertices[index].rgba = XMFLOAT4(m_particleList[i].red, m_particleList[i].green, m_particleList[i].blue, 1.0f);
		index++;

		// ������ ��.
		m_pVertices[index].pos = XMFLOAT3(m_particleList[i].positionX + _particleSize, m_particleList[i].positionY + _particleSize, m_particleList[i].positionZ);
		m_pVertices[index].texture = XMFLOAT2(1.0f, 0.0f);
		m_pVertices[index].rgba = XMFLOAT4(m_particleList[i].red, m_particleList[i].green, m_particleList[i].blue, 1.0f);
		index++;
	}

	// ���� ���۸� ��޴ϴ�.
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	if (FAILED(m_pDeviceContext->Map(m_vertexBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
	{
		return false;
	}

	// ���� ������ �����͸� ����Ű�� �����͸� ��´�.
	verticesPtr = (Vertex::VertexColorTexture*)mappedResource.pData;

	// �����͸� ���� ���ۿ� �����մϴ�.
	memcpy(verticesPtr, (void*)m_pVertices, (sizeof(Vertex::VertexColorTexture) * m_vertexCount));

	// ���� ������ ����� �����մϴ�.
	m_pDeviceContext->Unmap(m_vertexBuffer.Get(), 0);

	return true;
}

int Particle::GetIndexCount()
{
	return m_indexCount;
}

