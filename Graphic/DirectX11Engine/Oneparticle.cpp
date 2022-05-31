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



bool Oneparticle::InitializeBuffers()
{
	return false;

	//m_vertexCount = 6;
	//m_indexCount = 6;
	//m_pVertices = new Vertex::VertexColorTexture[m_vertexCount];
	//unsigned long* indices = new unsigned long[m_indexCount];
	//
	//// ó������ ���� �迭�� 0���� �ʱ�ȭ�մϴ�.
	//memset(m_pVertices, 0, (sizeof(Vertex::VertexColorTexture) * m_vertexCount));
	//
	//// �ε��� �迭�� �ʱ�ȭ�մϴ�.
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
	//// �� �̻� �ʿ����� �����Ƿ� �ε��� �迭�� �����մϴ�.
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
	//// ó������ ���� �迭�� 0���� �ʱ�ȭ�մϴ�.
	//Vertex::VertexColorTexture* verticesPtr;
	//memset(m_pVertices, 0, (sizeof(Vertex::VertexColorTexture) * m_vertexCount));
	//
	//// ���� ��ƼŬ ��� �迭���� ���� �迭�� ����ϴ�. �� ��ƼŬ�� �� ���� �ﰢ������ ������� �����Դϴ�.
	//int index = 0;
	//float m_particleSize = 0.2f;
	//
	//for (int i = 0; i < 1; i++)
	//{
	//	// ���� �Ʒ�.
	//	m_pVertices[index].pos = XMFLOAT3(-m_particleSize, -m_particleSize, 0);
	//	m_pVertices[index].texture = XMFLOAT2(0.0f, 1.0f);
	//	m_pVertices[index].rgba = XMFLOAT4(m_particleList[i].red, m_particleList[i].green, m_particleList[i].blue, 1.0f);
	//	index++;
	//
	//	// ���� ��.
	//	m_pVertices[index].pos = XMFLOAT3(- m_particleSize, m_particleSize, 0);
	//	m_pVertices[index].texture = XMFLOAT2(0.0f, 0.0f);
	//	m_pVertices[index].rgba = XMFLOAT4(m_particleList[i].red, m_particleList[i].green, m_particleList[i].blue, 1.0f);
	//	index++;
	//
	//	// ������ �Ʒ�.
	//	m_pVertices[index].pos = XMFLOAT3(m_particleSize, -m_particleSize, 0);
	//	m_pVertices[index].texture = XMFLOAT2(1.0f, 1.0f);
	//	m_pVertices[index].rgba = XMFLOAT4(m_particleList[i].red, m_particleList[i].green, m_particleList[i].blue, 1.0f);
	//	index++;
	//
	//	// ������ �Ʒ�.
	//	m_pVertices[index].pos = XMFLOAT3(m_particleSize, -m_particleSize, 0);
	//	m_pVertices[index].texture = XMFLOAT2(1.0f, 1.0f);
	//	m_pVertices[index].rgba = XMFLOAT4(m_particleList[i].red, m_particleList[i].green, m_particleList[i].blue, 1.0f);
	//	index++;
	//
	//	// ���� ��.
	//	m_pVertices[index].pos = XMFLOAT3(-m_particleSize,m_particleSize, 0);
	//	m_pVertices[index].texture = XMFLOAT2(0.0f, 0.0f);
	//	m_pVertices[index].rgba = XMFLOAT4(m_particleList[i].red, m_particleList[i].green, m_particleList[i].blue, 1.0f);
	//	index++;
	//
	//	// ������ ��.
	//	m_pVertices[index].pos = XMFLOAT3(m_particleSize, m_particleSize, 0);
	//	m_pVertices[index].texture = XMFLOAT2(1.0f, 0.0f);
	//	m_pVertices[index].rgba = XMFLOAT4(m_particleList[i].red, m_particleList[i].green, m_particleList[i].blue, 1.0f);
	//	index++;
	//}
	//
	//// ���� ���۸� ��޴ϴ�.
	//D3D11_MAPPED_SUBRESOURCE mappedResource;
	//if (FAILED(m_pDeviceContext->Map(m_vertexBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
	//{
	//	return false;
	//}
	//
	//// ���� ������ �����͸� ����Ű�� �����͸� ��´�.
	//verticesPtr = (Vertex::VertexColorTexture*)mappedResource.pData;
	//
	//// �����͸� ���� ���ۿ� �����մϴ�.
	//memcpy(verticesPtr, (void*)m_pVertices, (sizeof(Vertex::VertexColorTexture) * m_vertexCount));
	//
	//// ���� ������ ����� �����մϴ�.
	//m_pDeviceContext->Unmap(m_vertexBuffer.Get(), 0);
	//
	//return true;
}



