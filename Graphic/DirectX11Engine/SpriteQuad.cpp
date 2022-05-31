#include "SpriteQuad.h"
#include "VertexHelper.h"

SpriteQuad::SpriteQuad()
	: m_Stride(0), m_IndexCount(0)
{
}

SpriteQuad::~SpriteQuad()
{
}

void SpriteQuad::SetBuffer(ID3D11Device* device)
{
	std::vector<Vertex::Vertex2D> vertexData =
	{
		Vertex::Vertex2D(-0.5f, -0.5f, 0.0f,	0.0f, 1.0f), //Bottom Left
		Vertex::Vertex2D(0.5f, -0.5f, 0.0f,		1.0f, 1.0f), //Bottom Right
		Vertex::Vertex2D(-0.5, 0.5, 0.0f,		0.0f, 0.0f), //TopLeft
		Vertex::Vertex2D(0.5, 0.5, 0.0f,		1.0f, 0.0f), //TopRight
	};
	
	std::vector<DWORD> indexData =
	{
		2, 1, 0,
		1, 2, 3
	};

	VertexBuffer<Vertex::Vertex2D> _vertices;
	IndexBuffer _indices;

	HRESULT hr = _vertices.Initialize(device, vertexData.data(), vertexData.size());
	hr = _indices.Initialize(device, indexData.data(), indexData.size());

	m_VertexBuffer = _vertices.GetPtr();
	m_Stride = _vertices.Stride();

	m_IndexBuffer = _indices.GetPtr();
	m_IndexCount = _indices.IndexCount();

	///////////////////--디버깅용 테두리--///////////////////////

	std::vector<Vertex::VertexColor> vertexDataEdge =
	{
		Vertex::VertexColor(-0.5f, -0.5f, 0.0f,		0.0f, 1.0f, 0.0f), //Bottom Left
		Vertex::VertexColor(0.5f, -0.5f, 0.0f,		0.0f, 1.0f, 0.0f), //Bottom Right
		Vertex::VertexColor(-0.5, 0.5, 0.0f,		0.0f, 1.0f, 0.0f), //TopLeft
		Vertex::VertexColor(0.5, 0.5, 0.0f,			0.0f, 1.0f, 0.0f), //TopRight
	};

	VertexBuffer<Vertex::VertexColor> _verticesEdge;
	
	hr = _verticesEdge.Initialize(device, vertexDataEdge.data(), vertexDataEdge.size());
	
	m_VertexBufferEdge = _verticesEdge.GetPtr();
	m_StrideEdge = _verticesEdge.Stride();

}
