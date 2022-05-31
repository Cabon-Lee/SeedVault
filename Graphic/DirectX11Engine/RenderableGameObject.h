#pragma once
#include "GameObject3D.h"
#include <vector>
#include <queue>
#include <string>
#include <d3d11.h>
#include "ConstantBuffer.h"
#include "JMParsingDataClass.h"

class Model;
class FBXModel;
struct NodeData;
class MaterialData;

class RenderableGameObject : public GameObject3D
{
public:
	RenderableGameObject();
	~RenderableGameObject();

	bool Initialize(FBXModel* model, ID3D11Device* device, ID3D11DeviceContext* deviceContext, ConstantBuffer<CB_VS_vertexshader>& cb_vs_vertexshader, float scale);
	void Update();
	void Draw(const DirectX::XMMATRIX& viewProjectionMatrix);


	NodeData* m_pNodeData;
	MaterialData* m_pMaterialData;
	Model* m_pModel;

private:
	

	void UpdateMatrix() override;

	DirectX::XMMATRIX worldMatrix = DirectX::XMMatrixIdentity();

};

