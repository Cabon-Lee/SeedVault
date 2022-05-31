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

	// 파티클을 렌더링하는 데 사용할 버퍼를 만듭니다.
	if (!InitializeBuffers())
	{
		return false;
	}

	return true;
}

void Particle::Render(ParticleInfo* m_particleList)
{
	UpdateBuffers(m_particleList);

	// 정점 버퍼 보폭 및 오프셋을 설정합니다.
	unsigned int stride = sizeof(Vertex::VertexColorTexture);
	unsigned int offset = 0;

	// 렌더링 할 수 있도록 입력 어셈블러에서 정점 버퍼를 활성으로 설정합니다.
	m_pDeviceContext->IASetVertexBuffers(0, 1, m_vertexBuffer.GetAddressOf(), &stride, &offset);

	// 렌더링 할 수 있도록 입력 어셈블러에서 인덱스 버퍼를 활성으로 설정합니다.
	m_pDeviceContext->IASetIndexBuffer(m_indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

	// 이 정점 버퍼에서 렌더링 되어야 하는 프리미티브 유형을 설정합니다.
	m_pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}


bool Particle::InitializeBuffers()
{
	// 정점 배열의 최대 정점 수를 설정합니다.
	m_vertexCount = m_maxParticles * 6;

	// 인덱스 배열의 최대 인덱스 수를 설정합니다.
	m_indexCount = m_vertexCount;

	// 렌더링 될 입자에 대한 정점 배열을 만듭니다.
	m_pVertices = new Vertex::VertexColorTexture[m_vertexCount];
	if (!m_pVertices)
	{
		return false;
	}

	// 인덱스 배열을 만듭니다.
	unsigned long* indices = new unsigned long[m_indexCount];
	if (!indices)
	{
		return false;
	}

	// 처음에는 정점 배열을 0으로 초기화합니다.
	memset(m_pVertices, 0, (sizeof(Vertex::VertexColorTexture) * m_vertexCount));

	// 인덱스 배열을 초기화합니다.
	for (int i = 0; i < m_indexCount; i++)
	{
		indices[i] = i;
	}

	// 동적 정점 버퍼의 설명을 설정한다.
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

	// subresource 구조에 정점 데이터에 대한 포인터를 제공합니다.
	D3D11_SUBRESOURCE_DATA vertexData;
	vertexData.pSysMem = m_pVertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// 이제 마침내 정점 버퍼를 만듭니다.
	if (FAILED(m_pDevice->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer)))
	{
		return false;
	}

	// 정적 인덱스 버퍼의 설명을 설정합니다.
	D3D11_BUFFER_DESC indexBufferDesc;
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// 하위 리소스 구조에 인덱스 데이터에 대한 포인터를 제공합니다.
	D3D11_SUBRESOURCE_DATA indexData;
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// 인덱스 버퍼를 만듭니다.
	if (FAILED(m_pDevice->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer)))
	{
		return false;
	}

	// 더 이상 필요하지 않으므로 인덱스 배열을 해제합니다.
	delete[] indices;
	indices = 0;

	return true;
}


bool Particle::UpdateBuffers(ParticleInfo* m_particleList)
{
	// 처음에는 정점 배열을 0으로 초기화합니다.
	Vertex::VertexColorTexture* verticesPtr;
	memset(m_pVertices, 0, (sizeof(Vertex::VertexColorTexture) * m_vertexCount));

	// 이제 파티클 목록 배열에서 정점 배열을 만듭니다. 각 파티클은 두 개의 삼각형으로 만들어진 쿼드입니다.
	int index = 0;
	float _particleSize = m_particleList->particleSize;

	for (int i = 0; i < m_particleList->currentParticleCount; i++)
	{
		// 왼쪽 아래.
		m_pVertices[index].pos = XMFLOAT3(m_particleList[i].positionX - _particleSize, m_particleList[i].positionY - _particleSize, m_particleList[i].positionZ);
		m_pVertices[index].texture = XMFLOAT2(0.0f, 1.0f);
		m_pVertices[index].rgba = XMFLOAT4(m_particleList[i].red, m_particleList[i].green, m_particleList[i].blue, 1.0f);
		index++;

		// 왼쪽 위.
		m_pVertices[index].pos = XMFLOAT3(m_particleList[i].positionX - _particleSize, m_particleList[i].positionY + _particleSize, m_particleList[i].positionZ);
		m_pVertices[index].texture = XMFLOAT2(0.0f, 0.0f);
		m_pVertices[index].rgba = XMFLOAT4(m_particleList[i].red, m_particleList[i].green, m_particleList[i].blue, 1.0f);
		index++;

		// 오른쪽 아래.
		m_pVertices[index].pos = XMFLOAT3(m_particleList[i].positionX + _particleSize, m_particleList[i].positionY - _particleSize, m_particleList[i].positionZ);
		m_pVertices[index].texture = XMFLOAT2(1.0f, 1.0f);
		m_pVertices[index].rgba = XMFLOAT4(m_particleList[i].red, m_particleList[i].green, m_particleList[i].blue, 1.0f);
		index++;

		// 오른쪽 아래.
		m_pVertices[index].pos = XMFLOAT3(m_particleList[i].positionX + _particleSize, m_particleList[i].positionY - _particleSize, m_particleList[i].positionZ);
		m_pVertices[index].texture = XMFLOAT2(1.0f, 1.0f);
		m_pVertices[index].rgba = XMFLOAT4(m_particleList[i].red, m_particleList[i].green, m_particleList[i].blue, 1.0f);
		index++;

		// 왼쪽 위.
		m_pVertices[index].pos = XMFLOAT3(m_particleList[i].positionX - _particleSize, m_particleList[i].positionY + _particleSize, m_particleList[i].positionZ);
		m_pVertices[index].texture = XMFLOAT2(0.0f, 0.0f);
		m_pVertices[index].rgba = XMFLOAT4(m_particleList[i].red, m_particleList[i].green, m_particleList[i].blue, 1.0f);
		index++;

		// 오른쪽 위.
		m_pVertices[index].pos = XMFLOAT3(m_particleList[i].positionX + _particleSize, m_particleList[i].positionY + _particleSize, m_particleList[i].positionZ);
		m_pVertices[index].texture = XMFLOAT2(1.0f, 0.0f);
		m_pVertices[index].rgba = XMFLOAT4(m_particleList[i].red, m_particleList[i].green, m_particleList[i].blue, 1.0f);
		index++;
	}

	// 정점 버퍼를 잠급니다.
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	if (FAILED(m_pDeviceContext->Map(m_vertexBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
	{
		return false;
	}

	// 정점 버퍼의 데이터를 가리키는 포인터를 얻는다.
	verticesPtr = (Vertex::VertexColorTexture*)mappedResource.pData;

	// 데이터를 정점 버퍼에 복사합니다.
	memcpy(verticesPtr, (void*)m_pVertices, (sizeof(Vertex::VertexColorTexture) * m_vertexCount));

	// 정점 버퍼의 잠금을 해제합니다.
	m_pDeviceContext->Unmap(m_vertexBuffer.Get(), 0);

	return true;
}

int Particle::GetIndexCount()
{
	return m_indexCount;
}

