#pragma once
#include <vector>
#include <queue>
#include <string>
#include <d3d11.h>
#include "JMParsingDataClass.h"
#include "ConstantBuffer.h"
#include "IndexBuffer.h"
#include "VertexBuffer.h"
#include "VertexHelper.h"

class ResourceManager;
class JMFBXParser;

struct NodeData;
struct MaterialData;
struct FBXModel;

class Model
{
public:
	Model();
	~Model();

	bool Initialize(std::queue<JMParserData::Mesh*>, ID3D11Device* device, ID3D11DeviceContext* deviceContext, ConstantBuffer<CB_VS_vertexshader>& cb_vs_vertexshader);
	void Draw(const XMMATRIX& worldMatrix, const XMMATRIX& viewProjectionMatrix);
	
	// 디버깅을 위해 위로 올렸다
	VertexBuffer<Vertex::VertexColor> m_vertexbuffer_V;
	IndexBuffer m_indexbuffer_V;

	std::vector<void*> _test_V;
	MaterialData* _material;

private:
	ID3D11Device* device = nullptr;
	ID3D11DeviceContext* deviceContext = nullptr;
	ConstantBuffer<CB_VS_vertexshader>* cb_vs_vertexshader = nullptr;
	std::string directory = "";

	std::queue<JMParserData::Mesh*> m_pMesh_Q;
	int m_meshQsize;

	VertexBuffer<Vertex::VertexColor>* m_vertexbuffer;
	IndexBuffer* m_indexbuffer;
};

