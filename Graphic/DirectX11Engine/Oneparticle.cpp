#include "Oneparticle.h"

Oneparticle::Oneparticle()
{
}

Oneparticle::~Oneparticle()
{
}

bool Oneparticle::Initialize(Microsoft::WRL::ComPtr<ID3D11Device> pDevice, Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext)
{
	m_pDevice = pDevice;
	m_pDeviceContext = pDeviceContext;

	InitializeBuffers();

	return true;
}



void Oneparticle::Render()
{
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



bool Oneparticle::InitializeBuffers()
{
	return false;

	//m_vertexCount = 6;
	//m_indexCount = 6;
	//m_pVertices = new Vertex::VertexColorTexture[m_vertexCount];
	//unsigned long* indices = new unsigned long[m_indexCount];
	//
	//// 처음에는 정점 배열을 0으로 초기화합니다.
	//memset(m_pVertices, 0, (sizeof(Vertex::VertexColorTexture) * m_vertexCount));
	//
	//// 인덱스 배열을 초기화합니다.
	//for (int i = 0; i < m_indexCount; i++)
	//{
	//	indices[i] = i;
	//}
	//
	//VertexBuffer<Vertex::VertexColorTexture> _vertices;
	//IndexBuffer _indices;
	//
	//HRESULT hr = _vertices.InitializeDynamic(m_pDevice.Get(), m_pVertices, m_vertexCount);
	//hr = _indices.Initialize(m_pDevice.Get(), indices, m_indexCount);
	//
	//m_vertexBuffer = _vertices.GetPtr();
	//m_Stride = _vertices.Stride();
	//
	//m_indexBuffer = _indices.GetPtr();
	//m_IndexCount = _indices.IndexCount();
	//
	//// 더 이상 필요하지 않으므로 인덱스 배열을 해제합니다.
	//delete[] indices;
	//indices = 0;
	//
	//
	//float positionX = (((float)rand() - (float)rand()) / RAND_MAX);
	//float positionY = (((float)rand() - (float)rand()) / RAND_MAX);
	//float positionZ = (((float)rand() - (float)rand()) / RAND_MAX);
	//
	//float velocity = (((float)rand() - (float)rand()) / RAND_MAX);
	//
	//float red = (((float)rand() - (float)rand()) / RAND_MAX) + 0.5f;
	//float green = (((float)rand() - (float)rand()) / RAND_MAX) + 0.5f;
	//float blue = (((float)rand() - (float)rand()) / RAND_MAX) + 0.5f;
	//
	//m_particleList = new ParticleType[1];
	//
	//m_particleList[0].positionX = positionX;
	//m_particleList[0].positionY = positionY;
	//m_particleList[0].positionZ = positionZ;
	//m_particleList[0].red = red;
	//m_particleList[0].green = green;
	//m_particleList[0].blue = blue;
	//m_particleList[0].velocity = velocity;
	//m_particleList[0].active = true;
	//
	//
	//// 처음에는 정점 배열을 0으로 초기화합니다.
	//Vertex::VertexColorTexture* verticesPtr;
	//memset(m_pVertices, 0, (sizeof(Vertex::VertexColorTexture) * m_vertexCount));
	//
	//// 이제 파티클 목록 배열에서 정점 배열을 만듭니다. 각 파티클은 두 개의 삼각형으로 만들어진 쿼드입니다.
	//int index = 0;
	//float m_particleSize = 0.2f;
	//
	//for (int i = 0; i < 1; i++)
	//{
	//	// 왼쪽 아래.
	//	m_pVertices[index].pos = XMFLOAT3(-m_particleSize, -m_particleSize, 0);
	//	m_pVertices[index].texture = XMFLOAT2(0.0f, 1.0f);
	//	m_pVertices[index].rgba = XMFLOAT4(m_particleList[i].red, m_particleList[i].green, m_particleList[i].blue, 1.0f);
	//	index++;
	//
	//	// 왼쪽 위.
	//	m_pVertices[index].pos = XMFLOAT3(- m_particleSize, m_particleSize, 0);
	//	m_pVertices[index].texture = XMFLOAT2(0.0f, 0.0f);
	//	m_pVertices[index].rgba = XMFLOAT4(m_particleList[i].red, m_particleList[i].green, m_particleList[i].blue, 1.0f);
	//	index++;
	//
	//	// 오른쪽 아래.
	//	m_pVertices[index].pos = XMFLOAT3(m_particleSize, -m_particleSize, 0);
	//	m_pVertices[index].texture = XMFLOAT2(1.0f, 1.0f);
	//	m_pVertices[index].rgba = XMFLOAT4(m_particleList[i].red, m_particleList[i].green, m_particleList[i].blue, 1.0f);
	//	index++;
	//
	//	// 오른쪽 아래.
	//	m_pVertices[index].pos = XMFLOAT3(m_particleSize, -m_particleSize, 0);
	//	m_pVertices[index].texture = XMFLOAT2(1.0f, 1.0f);
	//	m_pVertices[index].rgba = XMFLOAT4(m_particleList[i].red, m_particleList[i].green, m_particleList[i].blue, 1.0f);
	//	index++;
	//
	//	// 왼쪽 위.
	//	m_pVertices[index].pos = XMFLOAT3(-m_particleSize,m_particleSize, 0);
	//	m_pVertices[index].texture = XMFLOAT2(0.0f, 0.0f);
	//	m_pVertices[index].rgba = XMFLOAT4(m_particleList[i].red, m_particleList[i].green, m_particleList[i].blue, 1.0f);
	//	index++;
	//
	//	// 오른쪽 위.
	//	m_pVertices[index].pos = XMFLOAT3(m_particleSize, m_particleSize, 0);
	//	m_pVertices[index].texture = XMFLOAT2(1.0f, 0.0f);
	//	m_pVertices[index].rgba = XMFLOAT4(m_particleList[i].red, m_particleList[i].green, m_particleList[i].blue, 1.0f);
	//	index++;
	//}
	//
	//// 정점 버퍼를 잠급니다.
	//D3D11_MAPPED_SUBRESOURCE mappedResource;
	//if (FAILED(m_pDeviceContext->Map(m_vertexBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
	//{
	//	return false;
	//}
	//
	//// 정점 버퍼의 데이터를 가리키는 포인터를 얻는다.
	//verticesPtr = (Vertex::VertexColorTexture*)mappedResource.pData;
	//
	//// 데이터를 정점 버퍼에 복사합니다.
	//memcpy(verticesPtr, (void*)m_pVertices, (sizeof(Vertex::VertexColorTexture) * m_vertexCount));
	//
	//// 정점 버퍼의 잠금을 해제합니다.
	//m_pDeviceContext->Unmap(m_vertexBuffer.Get(), 0);
	//
	//return true;
}



