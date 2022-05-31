#include "RenderableGameObject.h"
#include "Model.h"
#include "JMFBXParser.h"

RenderableGameObject::RenderableGameObject()
{
}

RenderableGameObject::~RenderableGameObject()
{
}

bool RenderableGameObject::Initialize(FBXModel* mesh_Q, ID3D11Device* device, ID3D11DeviceContext* deviceContext, ConstantBuffer<CB_VS_vertexshader>& cb_vs_vertexshader, float scale)
{
	m_pModel = new Model();

	if (!m_pModel->Initialize(mesh_Q, device, deviceContext, cb_vs_vertexshader))
		return false;

	this->SetPosition(0.0f, 0.0f, 0.0f);
	this->SetRotation(0.0f, 0.0f, 0.0f);
	this->scale = XMFLOAT3(scale, scale, scale);
	this->UpdateMatrix();

	return true;
}

void RenderableGameObject::Update()
{
}

void RenderableGameObject::Draw(const DirectX::XMMATRIX& viewProjectionMatrix)
{
    m_pModel->Draw(this->worldMatrix, viewProjectionMatrix);
}

void RenderableGameObject::UpdateMatrix()
{
    this->worldMatrix = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z) * DirectX::XMMatrixRotationRollPitchYaw(this->rot.x, this->rot.y, this->rot.z) * DirectX::XMMatrixTranslation(this->pos.x, this->pos.y, this->pos.z);
    this->UpdateDirectionVectors();
}
