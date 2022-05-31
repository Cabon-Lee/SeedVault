#include "Object.h"
#include "OIT2.h"
#include "OITShaderHelper.h"
#include "RasterizerState.h"
#include "IResourceManager.h"
#include "Shaders.h"

#define	LAYER_STD 8
/*

//fundamental elements
Object::Object(
	Microsoft::WRL::ComPtr<ID3D11Device> pDevice,
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext,
	std::string name,
	std::shared_ptr<MeshOIT> mesh,
	std::string sVS,
	const D3D11_INPUT_ELEMENT_DESC* iLayouts,
	UINT layoutCount,
	std::string sHS,
	std::string sDS,
	std::string sGS,
	std::string sPS,
	bool directRender)
	: m_pDevice(pDevice), m_pDeviceContext(pDeviceContext), name(name), mesh(mesh), layer(LAYER_STD)
{
	//transform = new Transform();

	//m_vs = new VShader(device, sVS, iLayouts, layoutCount);
	//m_ps = new PShader(device, sPS);
	m_vs = m_pResourceManager->GetVertexShader(sVS);
	m_ps = m_pResourceManager->GetPixelShader(sPS);

	// 블렌드스테이트		
	m_dxBlendState = RasterizerState::GetOITInitBS();
	// 레스터라이저스테이트
	m_dxRState = RasterizerState::GetOITInitRS();
	// 뎁스스텐실
	m_dxDSState = RasterizerState::GetOITInitDS();

	m_material = new SHADER_MATERIAL(DirectX::XMFLOAT3(0.6, 0.6, 0.6), 0.4, DirectX::XMFLOAT3(0.6, 0.6, 0.6), DirectX::XMFLOAT3(1.0, 1.0, 1.0));
}

//standard elements
Object::Object(
	Microsoft::WRL::ComPtr<ID3D11Device> pDevice,
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext,
	std::string name,
	std::shared_ptr <MeshOIT> mesh,
	ID3D11ShaderResourceView* diffSRV,
	ID3D11ShaderResourceView* normalSRV,
	bool directRender)
	: m_pDevice(pDevice), m_pDeviceContext(pDeviceContext), name(name), mesh(mesh), layer(LAYER_STD), m_mainTex(diffSRV), m_normal(normalSRV)
{
	//transform = new Transform();

	//vs = new VShader(pDevice, "StdVS.cso", Std_ILayouts, ARRAYSIZE(Std_ILayouts));
	//ps = new PShader(pDevice, "StdPS.cso");
	m_vs = m_pResourceManager->GetVertexShader("vs_OITVS.cso");
	m_ps = m_pResourceManager->GetPixelShader("ps_DepthPeeling.cso");

	m_vs->AddCB(pDevice, 0, 1, sizeof(SHADER_STD_TRANSF));
	m_ps->AddCB(pDevice, SHADER_REG_CB_MATERIAL, 1, sizeof(SHADER_MATERIAL));
	m_ps->WriteCB(pDeviceContext, SHADER_REG_CB_MATERIAL, m_material);
	
	m_ps->AddSRV(SHADER_REG_SRV_DIFFUSE, 1);
	m_ps->AddSRV(SHADER_REG_SRV_NORMAL, 1);
	m_ps->WriteSRV(SHADER_REG_SRV_DIFFUSE, m_mainTex);
	m_ps->WriteSRV(SHADER_REG_SRV_NORMAL, m_normal);

	// 블렌드스테이트		
	m_dxBlendState = RasterizerState::GetOITInitBS();
	// 레스터라이저스테이트
	m_dxRState = RasterizerState::GetOITInitRS();
	// 뎁스스텐실
	m_dxDSState = RasterizerState::GetOITInitDS();

	m_material = new SHADER_MATERIAL(DirectX::XMFLOAT3(0.6, 0.6, 0.6), 0.4, DirectX::XMFLOAT3(0.6, 0.6, 0.6), DirectX::XMFLOAT3(1.0, 1.0, 1.0));
}

Object::~Object()
{
	delete transform;

	delete m_material;
}

void Object::Update()
{
	if (!enabled)
		return;
}

int Object::Layer() const
{
	return layer;
}

void Object::SetLayer(int l)
{
	layer = l;
}

void Object::GetMaterial(SHADER_MATERIAL* material)
{
	*material = *m_material;
}

void Object::GetMainTex(ID3D11ShaderResourceView** ppSRV)
{
	*ppSRV = m_mainTex;
}

void Object::GetNormal(ID3D11ShaderResourceView** ppNormal)
{
	*ppNormal = m_normal;
}


Object::Object()
{
}

void Object::Render(Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext)const
{
	m_vs->Apply(pDeviceContext);
	m_ps->Apply(pDeviceContext);

	// 블렌드 스테이트, 래스터라이저 스테이트, 뎁스스텐실 스테이트 Apply
	float blendFactors[] = { 0.0f, 0.0f, 0.0f, 0.0f };
	pDeviceContext->OMSetBlendState(m_dxBlendState, blendFactors, 0xffffffff);
	pDeviceContext->RSSetState(m_dxRState);
	pDeviceContext->OMSetDepthStencilState(m_dxDSState, 0);

	//mesh->Apply(pDeviceContext);
}

void Object::Render(Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext, const DirectX::XMMATRIX& v, const DirectX::XMMATRIX& p, UINT sceneDepth) const
{
	if (!enabled || !show)
		return;

	//const SHADER_STD_TRANSF STransformation(transform->WorldMatrix(), v, p, 1, 1000, XM_PIDIV2, 1);

	//m_vs->WriteCB(pDeviceContext, 0, &STransformation);

	Render(pDeviceContext);
}

void Object::RenderGeom(Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext) const
{
	if (!enabled || !show)
		return;

	//mesh->Apply(pDeviceContext);
}


*/