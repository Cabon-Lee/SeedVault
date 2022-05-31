#include "Model.h"
#include "Shaders.h"
#include <DirectXColors.h>
#include <iterator>
#include "CATrace.h"

#include "NodeData.h"
#include "JMFBXParser.h"

Model::Model()
{
}

Model::~Model()
{
}

bool Model::Initialize(std::queue<JMParserData::Mesh*> mesh_Q, ID3D11Device* device, ID3D11DeviceContext* deviceContext, ConstantBuffer<CB_VS_vertexshader>& cb_vs_vertexshader)
{
	this->device = device;
	this->deviceContext = deviceContext;
	this->cb_vs_vertexshader = &cb_vs_vertexshader;

	m_pMesh_Q = mesh_Q;
	m_meshQsize = mesh_Q.size();
	m_vertexbuffer = new VertexBuffer<Vertex::VertexColor>[m_pMesh_Q.size()];
	m_indexbuffer = new IndexBuffer[m_pMesh_Q.size()];

	for (int i = 0; i < m_meshQsize; i++)
	{
		m_pMesh_Q.front();

		/// 버텍스버퍼
		vector<Vertex::VertexColor> _vertices_V;

		for (int i = 0; i < m_pMesh_Q.front()->optVertex_V.size(); i++)
		{
			Vertex::VertexColor _vertex;
			_vertex.pos = m_pMesh_Q.front()->optVertex_V[i]->pos;
			_vertex.rgba = XMFLOAT4((const float*)DirectX::Colors::OliveDrab);

			CA_TRACE("버텍스 좌표[%d] : x = %f, y = %f z = %f ", i, _vertex.pos.x, _vertex.pos.y, _vertex.pos.z);

			_vertices_V.push_back(_vertex);
		}

		Vertex::VertexColor* _pVertices = new Vertex::VertexColor[_vertices_V.size()];
		UINT _vertexCount = _vertices_V.size();
		std::copy(_vertices_V.begin(), _vertices_V.end(), stdext::checked_array_iterator<Vertex::VertexColor*>(_pVertices, _vertexCount));
		HRESULT hr = m_vertexbuffer[i].Initialize(device, _vertices_V.data(), _vertices_V.size());


		/// 인덱스버퍼
		// 새로운 벡터에 인덱스정보를 넣는다
		std::vector<DWORD> _indices_V(m_pMesh_Q.front()->optIndexNum * 3);

		// ParsingData에서 인덱스 값을 가져온다
		for (int i = 0; i < m_pMesh_Q.front()->optIndexNum; i++)
		{
			_indices_V[i * 3 + 0] = static_cast<DWORD>(m_pMesh_Q.front()->pOptIndex[i].index_A[0]);
			_indices_V[i * 3 + 1] = static_cast<DWORD>(m_pMesh_Q.front()->pOptIndex[i].index_A[1]);
			_indices_V[i * 3 + 2] = static_cast<DWORD>(m_pMesh_Q.front()->pOptIndex[i].index_A[2]);
		}

		int a = 0;

		DWORD* _pIndices = new DWORD[_indices_V.size()];
		UINT _indexCount = _indices_V.size();
		std::copy(_indices_V.begin(), _indices_V.end(), stdext::checked_array_iterator<DWORD*>(_pIndices, _indexCount));

		hr = m_indexbuffer[i].Initialize(device, _indices_V.data(), _indices_V.size());

		//hr = m_indexbuffer[i].Initialize(device, _indices_V.data(), _indices_V.size());

		m_pMesh_Q.pop();
	}

	NodeData* _test = new NodeData();

	_test->m_VertexBuffer = m_vertexbuffer;
	_test->m_IndexBuffer = m_indexbuffer;
	_test->m_Stride = m_vertexbuffer->Stride();
	_test->m_IndexCount = m_indexbuffer->IndexCount();

	return true;
}

void Model::Draw(const XMMATRIX& worldMatrix, const XMMATRIX& viewProjectionMatrix)
{
	this->deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	this->deviceContext->VSSetConstantBuffers(0, 1, this->cb_vs_vertexshader->GetAddressOf());

	//Constant buffer 와 WVP Matrix 업데이트
	this->cb_vs_vertexshader->data.wvpMatrix = worldMatrix * viewProjectionMatrix; //Calculate World-View-Projection Matrix
	this->cb_vs_vertexshader->data.worldMatrix = worldMatrix; //Calculate World Matrix
	this->cb_vs_vertexshader->ApplyChanges();

	UINT offset = 0;

	this->deviceContext->IASetInputLayout(Shaders::colorShader.GetInputLayout());

	this->deviceContext->VSSetShader(Shaders::colorShader.GetVertexShader(), NULL, 0);
	this->deviceContext->PSSetShader(Shaders::colorShader.GetPixelShader(), NULL, 0);

	for (int i = 0; i < m_meshQsize; i++)
	{
		this->deviceContext->IASetVertexBuffers(0, 1, m_vertexbuffer[i].GetAddressOf(), m_vertexbuffer[i].StridePtr(), &offset);
		this->deviceContext->IASetIndexBuffer(m_indexbuffer[i].Get(), DXGI_FORMAT::DXGI_FORMAT_R32_UINT, 0);
		this->deviceContext->DrawIndexed(m_indexbuffer[i].IndexCount(), 0, 0);
	}
}

