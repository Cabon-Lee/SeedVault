#include "RasterizerState.h"

#include "COMException.h"
#include "ErrorLogger.h"

ID3D11RasterizerState* RasterizerState::m_WireFrameRS = nullptr;
ID3D11RasterizerState* RasterizerState::m_SolidRasterizeStateFCC = nullptr;
ID3D11RasterizerState* RasterizerState::m_SolidRasterizeStateNormal = nullptr;
ID3D11RasterizerState* RasterizerState::m_NoCullingRS = nullptr;
ID3D11RasterizerState* RasterizerState::m_DepthRasterizerState = nullptr;

ID3D11BlendState* RasterizerState::m_AlphaToCoverageBS = nullptr;
ID3D11BlendState* RasterizerState::m_TransparentBS = nullptr;

ID3D11DepthStencilState* RasterizerState::m_NormalDSS = nullptr;
ID3D11DepthStencilState* RasterizerState::m_DepthStencilState = nullptr;
ID3D11DepthStencilState* RasterizerState::m_DepthDisabledStencilState = nullptr;
ID3D11DepthStencilState* RasterizerState::m_ComparisonLessEqual = nullptr;
ID3D11DepthStencilState* RasterizerState::m_DisabledDepthStencilState = nullptr;

ID3D11RasterizerState* RasterizerState::m_NowRasterizer = nullptr;
ID3D11RasterizerState* RasterizerState::m_PrevRasterizer = nullptr;

ID3D11BlendState* RasterizerState::m_pBlenderState = nullptr;
ID3D11BlendState* RasterizerState::m_alphaEnableBlendingState = nullptr;
ID3D11BlendState* RasterizerState::m_alphaEnableBlendingStateUI = nullptr;

ID3D11SamplerState* RasterizerState::m_pLinearSample = nullptr;
ID3D11SamplerState* RasterizerState::m_pTriLinearSaple = nullptr;
ID3D11SamplerState* RasterizerState::m_pPointSample = nullptr;
ID3D11SamplerState* RasterizerState::m_pAnisotropicSample = nullptr;
ID3D11SamplerState* RasterizerState::m_pPCFSample = nullptr;
ID3D11SamplerState* RasterizerState::m_pOutLineSample = nullptr;
ID3D11SamplerState* RasterizerState::m_pSSAOSample = nullptr;
ID3D11SamplerState* RasterizerState::m_pSSAO2Sample = nullptr;

/// OIT ����
ID3D11BlendState* RasterizerState::m_OITInitBS = nullptr;
ID3D11RasterizerState* RasterizerState::m_OITInitRS = nullptr;
ID3D11DepthStencilState* RasterizerState::m_OITInitDS = nullptr;

ID3D11BlendState* RasterizerState::m_TransparentSet = nullptr;
ID3D11BlendState* RasterizerState::m_outLineBlendingState = nullptr;
ID3D11BlendState* RasterizerState::m_DestZeroBlendingState = nullptr;
ID3D11BlendState* RasterizerState::m_DeferredBlendState = nullptr;


RasterizerState::RasterizerState()
{
}

RasterizerState::~RasterizerState()
{
}

void RasterizerState::CreateRasterizerState(ID3D11Device* pDevice)
{
	//////////////////////////////////////////////////////////////
	// �޽� ����
	//////////////////////////////////////////////////////////////

	D3D11_RASTERIZER_DESC solidDesc;
	ZeroMemory(&solidDesc, sizeof(D3D11_RASTERIZER_DESC));
	solidDesc.FillMode = D3D11_FILL_SOLID;
	solidDesc.CullMode = D3D11_CULL_BACK;
	solidDesc.FrontCounterClockwise = true;
	solidDesc.DepthClipEnable = true;

	pDevice->CreateRasterizerState(&solidDesc, &m_SolidRasterizeStateFCC);

	D3D11_RASTERIZER_DESC solidNormalDesc;
	ZeroMemory(&solidNormalDesc, sizeof(D3D11_RASTERIZER_DESC));
	solidNormalDesc.FillMode = D3D11_FILL_SOLID;
	solidNormalDesc.CullMode = D3D11_CULL_BACK;
	solidNormalDesc.FrontCounterClockwise = false;
	solidNormalDesc.DepthClipEnable = true;

	pDevice->CreateRasterizerState(&solidNormalDesc, &m_SolidRasterizeStateNormal);

	D3D11_RASTERIZER_DESC wireframeDesc;
	ZeroMemory(&wireframeDesc, sizeof(D3D11_RASTERIZER_DESC));
	wireframeDesc.FillMode = D3D11_FILL_WIREFRAME;
	wireframeDesc.CullMode = D3D11_CULL_NONE;
	wireframeDesc.FrontCounterClockwise = false;
	wireframeDesc.DepthClipEnable = true;

	pDevice->CreateRasterizerState(&wireframeDesc, &m_WireFrameRS);

	D3D11_RASTERIZER_DESC noCullDesc;
	ZeroMemory(&noCullDesc, sizeof(D3D11_RASTERIZER_DESC));
	noCullDesc.FillMode = D3D11_FILL_SOLID;
	noCullDesc.CullMode = D3D11_CULL_NONE;
	noCullDesc.FrontCounterClockwise = false;
	noCullDesc.DepthClipEnable = true;

	pDevice->CreateRasterizerState(&noCullDesc, &m_NoCullingRS);

	D3D11_RASTERIZER_DESC depthDesc;
	ZeroMemory(&depthDesc, sizeof(D3D11_RASTERIZER_DESC));
	depthDesc.FillMode = D3D11_FILL_SOLID;
	depthDesc.CullMode = D3D11_CULL_NONE;
	depthDesc.DepthBias = 10000;
	depthDesc.DepthBiasClamp = 0.0f;
	depthDesc.SlopeScaledDepthBias = 1.0f;
	
	pDevice->CreateRasterizerState(&depthDesc, &m_DepthRasterizerState);


	//////////////////////////////////////////////////////////////
	// ���� ���Ľ� ����
	//////////////////////////////////////////////////////////////

	D3D11_DEPTH_STENCIL_DESC equalsDesc;
	ZeroMemory(&equalsDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
	equalsDesc.DepthEnable = true;
	// �⺻ ���� True, ��ȣ��ȭ�ϸ� ���� ������ �߻����� �ʴ´�.
	// �׷��� �׸��� ������ �߿������µ�, �ٸ� ��ü �ڿ� ������ �ȼ��� ���� ��ü�� �ȼ��� �����.
	// ���� ���� ���׹����� �ȴٴ� �ǹ̰���.

	equalsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	// D3D11_DEPTH_WRITE_MASK_ZERO, ���� ���� ���Ⱑ �����ȴ�.
	// D3D11_DEPTH_WRITE_MASK_ALL, ����/���ٽ� ������ ��� ����� �ȼ��� ���̰� ���ۿ� ��ϵȴ�.

	equalsDesc.DepthFunc = D3D11_COMPARISON_LESS;
	// ������� ���� ������ ���, �ĸ� ���ۿ� �̹� ��ϵǾ� �ִ� �ȼ� ���̺���
	// ���� �ȼ� ���̰� �� ���� ��쿡�� ������ �����ؾ� �ϴ� ����.
	// D3D11_COMPARISON_LESS�� ��������� ���ȴ�.

	pDevice->CreateDepthStencilState(&equalsDesc, &m_NormalDSS);

	equalsDesc.DepthEnable = false;
	equalsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	pDevice->CreateDepthStencilState(&equalsDesc, &m_DisabledDepthStencilState);


	//////////////////////////////////////////////////////////////
	// ���� ������Ʈ ����
	//////////////////////////////////////////////////////////////

	D3D11_BLEND_DESC alphaToCoverageDesc = { 0 };
	alphaToCoverageDesc.AlphaToCoverageEnable = true;
	alphaToCoverageDesc.IndependentBlendEnable = false;
	alphaToCoverageDesc.RenderTarget[0].BlendEnable = false;
	alphaToCoverageDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	pDevice->CreateBlendState(&alphaToCoverageDesc, &m_AlphaToCoverageBS);


	D3D11_BLEND_DESC transparentDesc = { 0 };
	transparentDesc.AlphaToCoverageEnable = false;
	transparentDesc.IndependentBlendEnable = false;

	transparentDesc.RenderTarget[0].BlendEnable = true;
	transparentDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	transparentDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	transparentDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	transparentDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	transparentDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
	transparentDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	transparentDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	pDevice->CreateBlendState(&transparentDesc, &m_TransparentBS);

	// DepthStencilState ����ü �ʱ�ȭ
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

	// DepthStencilState ����ü�� �ۼ�
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

	depthStencilDesc.StencilEnable = true;
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;

	// �ȼ� ������ ���ٽ� ����
	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// �ȼ� �޸��� ���ٽ� ����
	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	pDevice->CreateDepthStencilState(&depthStencilDesc, &m_DepthStencilState);


	D3D11_DEPTH_STENCIL_DESC depthDisabledStencilDesc;
	ZeroMemory(&depthDisabledStencilDesc, sizeof(depthDisabledStencilDesc));

	depthDisabledStencilDesc.DepthEnable = false;
	depthDisabledStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthDisabledStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
	depthDisabledStencilDesc.StencilEnable = true;
	depthDisabledStencilDesc.StencilReadMask = 0xFF;
	depthDisabledStencilDesc.StencilWriteMask = 0xFF;
	depthDisabledStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthDisabledStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	depthDisabledStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthDisabledStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	pDevice->CreateDepthStencilState(&depthDisabledStencilDesc, &m_DepthDisabledStencilState);

	D3D11_DEPTH_STENCIL_DESC ComparisonLessEqualDesc;
	ZeroMemory(&ComparisonLessEqualDesc, sizeof(ComparisonLessEqualDesc));

	ComparisonLessEqualDesc.DepthEnable = false;
	ComparisonLessEqualDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	ComparisonLessEqualDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	ComparisonLessEqualDesc.StencilEnable = true;
	ComparisonLessEqualDesc.StencilReadMask = 0xFF;
	ComparisonLessEqualDesc.StencilWriteMask = 0xFF;
	ComparisonLessEqualDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	ComparisonLessEqualDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	ComparisonLessEqualDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	ComparisonLessEqualDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	ComparisonLessEqualDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	ComparisonLessEqualDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	ComparisonLessEqualDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	ComparisonLessEqualDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	pDevice->CreateDepthStencilState(&ComparisonLessEqualDesc, &m_ComparisonLessEqual);




	// Blend State ����
	D3D11_BLEND_DESC blendDesc = { 0 };
	D3D11_RENDER_TARGET_BLEND_DESC rtbd = { 0 };

	rtbd.BlendEnable = true;
	rtbd.SrcBlend = D3D11_BLEND::D3D11_BLEND_SRC_ALPHA;
	rtbd.DestBlend = D3D11_BLEND::D3D11_BLEND_INV_SRC_ALPHA;
	rtbd.BlendOp = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;
	rtbd.SrcBlendAlpha = D3D11_BLEND::D3D11_BLEND_ONE;
	rtbd.DestBlendAlpha = D3D11_BLEND::D3D11_BLEND_ZERO;
	rtbd.BlendOpAlpha = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;
	rtbd.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE::D3D11_COLOR_WRITE_ENABLE_ALL;

	blendDesc.RenderTarget[0] = rtbd;

	pDevice->CreateBlendState(&blendDesc, &m_pBlenderState);

	rtbd.SrcBlend = D3D11_BLEND::D3D11_BLEND_SRC_COLOR;
	rtbd.DestBlend = D3D11_BLEND::D3D11_BLEND_ONE;
	rtbd.SrcBlendAlpha = D3D11_BLEND::D3D11_BLEND_ZERO;
	rtbd.DestBlendAlpha = D3D11_BLEND::D3D11_BLEND_ZERO;
	blendDesc.RenderTarget[0] = rtbd;

	pDevice->CreateBlendState(&blendDesc, &m_outLineBlendingState);


	// ���� ���� ������ ����ϴ�.
	D3D11_BLEND_DESC blendStateDescription;
	ZeroMemory(&blendStateDescription, sizeof(D3D11_BLEND_DESC));

	// ���� ���� ���� ���� ������ ����ϴ�.
	blendStateDescription.RenderTarget[0].BlendEnable = TRUE;
	blendStateDescription.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	blendStateDescription.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
	blendStateDescription.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendStateDescription.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	//blendStateDescription.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendStateDescription.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
	blendStateDescription.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendStateDescription.RenderTarget[0].RenderTargetWriteMask = 0x0f;

	// ������ ����Ͽ� ȥ�� ���¸� ����ϴ�.
	pDevice->CreateBlendState(&blendStateDescription, &m_alphaEnableBlendingState);

	//blendStateDescription.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
	blendStateDescription.RenderTarget[0].DestBlend = D3D11_BLEND_DEST_COLOR;
	//blendStateDescription.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendStateDescription.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
	pDevice->CreateBlendState(&blendStateDescription, &m_DestZeroBlendingState);

	blendStateDescription.IndependentBlendEnable = false;
	blendStateDescription.RenderTarget[0].BlendEnable = false;
	pDevice->CreateBlendState(&blendStateDescription, &m_DeferredBlendState);

	// ���� ���� ������ ����ϴ�.
	D3D11_BLEND_DESC blendStateDescription2;
	ZeroMemory(&blendStateDescription2, sizeof(D3D11_BLEND_DESC));

	// ���� ���� ���� ���� ������ ����ϴ�.
	blendStateDescription2.AlphaToCoverageEnable = TRUE;
	blendStateDescription2.RenderTarget[0].BlendEnable = TRUE;
	blendStateDescription2.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blendStateDescription2.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blendStateDescription2.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendStateDescription2.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendStateDescription2.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendStateDescription2.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendStateDescription2.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendStateDescription2.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	// ������ ����Ͽ� ȥ�� ���¸� ����ϴ�.
	pDevice->CreateBlendState(&blendStateDescription2, &m_alphaEnableBlendingStateUI);

	//// ���� ���� ������ ����ϴ�.
	//D3D11_BLEND_DESC blendStateDescription3;
	//ZeroMemory(&blendStateDescription3, sizeof(D3D11_BLEND_DESC));
	//
	//// ���� ���� ���� ���� ������ ����ϴ�.
	//blendStateDescription3.RenderTarget[0].BlendEnable = TRUE;
	//blendStateDescription3.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	//blendStateDescription3.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	//blendStateDescription3.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	//blendStateDescription3.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_INV_DEST_ALPHA;
	//blendStateDescription3.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
	//blendStateDescription3.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	//blendStateDescription3.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	//blendStateDescription3.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	//
	//// ������ ����Ͽ� ȥ�� ���¸� ����ϴ�.
	//pDevice->CreateBlendState(&blendStateDescription3, &m_alphaEnableBlendingStateUI);


	// sampler state�� ���� sampler desc ����(�ؽ��� �غ�)
	CD3D11_SAMPLER_DESC sampDesc(D3D11_DEFAULT);
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = sampDesc.AddressV = sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.MaxAnisotropy = 1;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	pDevice->CreateSamplerState(&sampDesc, &m_pLinearSample);

	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	pDevice->CreateSamplerState(&sampDesc, &m_pPointSample);

	sampDesc.Filter = D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	sampDesc.BorderColor[0] = 0.0f;
	sampDesc.BorderColor[1] = 0.0f;
	sampDesc.BorderColor[2] = 0.0f;
	sampDesc.BorderColor[3] = 1e5f;
	pDevice->CreateSamplerState(&sampDesc, &m_pSSAOSample);

	sampDesc.Filter = D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.BorderColor[0] = 0.0f;
	sampDesc.BorderColor[1] = 0.0f;
	sampDesc.BorderColor[2] = 0.0f;
	sampDesc.BorderColor[3] = 0.0f;
	pDevice->CreateSamplerState(&sampDesc, &m_pSSAO2Sample);

	CD3D11_SAMPLER_DESC _anisotropicSampleDesc(D3D11_DEFAULT);
	_anisotropicSampleDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	_anisotropicSampleDesc.MaxAnisotropy = 4;
	_anisotropicSampleDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	_anisotropicSampleDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	_anisotropicSampleDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	pDevice->CreateSamplerState(&_anisotropicSampleDesc, &m_pAnisotropicSample);

	CD3D11_SAMPLER_DESC _PCFSampleDesc(D3D11_DEFAULT);
	_PCFSampleDesc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
	_PCFSampleDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	_PCFSampleDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	_PCFSampleDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	_PCFSampleDesc.ComparisonFunc = D3D11_COMPARISON_LESS;
	_PCFSampleDesc.BorderColor[0] = 0.0f;
	_PCFSampleDesc.BorderColor[1] = 0.0f;
	_PCFSampleDesc.BorderColor[2] = 0.0f;
	_PCFSampleDesc.BorderColor[3] = 0.0f;
	pDevice->CreateSamplerState(&_PCFSampleDesc, &m_pPCFSample);

	CD3D11_SAMPLER_DESC _OutLineSampleDesc(D3D11_DEFAULT);
	_OutLineSampleDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	_OutLineSampleDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	_OutLineSampleDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	_OutLineSampleDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	_OutLineSampleDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	_OutLineSampleDesc.BorderColor[0] = 0.0f;
	_OutLineSampleDesc.BorderColor[1] = 0.0f;
	_OutLineSampleDesc.BorderColor[2] = 0.0f;
	_OutLineSampleDesc.BorderColor[3] = 0.0f;
	pDevice->CreateSamplerState(&_OutLineSampleDesc, &m_pOutLineSample);

	/// OIT ����
	// ���彺����Ʈ	
	D3D11_BLEND_DESC OITBlendDesc = { 0 };
	OITBlendDesc.AlphaToCoverageEnable = false;
	OITBlendDesc.IndependentBlendEnable = true;
	OITBlendDesc.RenderTarget[0].BlendEnable = true;
	OITBlendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	OITBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	OITBlendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
	OITBlendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	OITBlendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	OITBlendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	OITBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	OITBlendDesc.RenderTarget[1] = OITBlendDesc.RenderTarget[0];
	OITBlendDesc.RenderTarget[2] = OITBlendDesc.RenderTarget[0];
	OITBlendDesc.RenderTarget[3] = OITBlendDesc.RenderTarget[0];
	OITBlendDesc.RenderTarget[4] = OITBlendDesc.RenderTarget[0];
	OITBlendDesc.RenderTarget[5] = OITBlendDesc.RenderTarget[0];
	OITBlendDesc.RenderTarget[6] = OITBlendDesc.RenderTarget[0];

	OITBlendDesc.RenderTarget[7].BlendEnable = false;
	OITBlendDesc.RenderTarget[7].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	pDevice->CreateBlendState(&OITBlendDesc, &m_OITInitBS);

	// �����Ͷ�����������Ʈ
	D3D11_RASTERIZER_DESC curDesc;
	ZeroMemory(&curDesc, sizeof(D3D11_RASTERIZER_DESC));
	curDesc.FillMode = D3D11_FILL_SOLID;
	curDesc.CullMode = D3D11_CULL_BACK;
	curDesc.FrontCounterClockwise = false;
	
	pDevice->CreateRasterizerState(&curDesc, &m_OITInitRS);

	// �������ٽ�
	D3D11_DEPTH_STENCIL_DESC firstDesc;

	firstDesc.DepthEnable = true;
	firstDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	firstDesc.DepthFunc = D3D11_COMPARISON_LESS;
	firstDesc.StencilEnable = false;

	pDevice->CreateDepthStencilState(&firstDesc, &m_OITInitDS);

	try
	{
		HRESULT _hr;

		D3D11_BLEND_DESC _transparentBlendDesc = {0};
		_transparentBlendDesc.IndependentBlendEnable = true;
		_transparentBlendDesc.RenderTarget[0].BlendEnable = true;
		_transparentBlendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
		_transparentBlendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
		_transparentBlendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		_transparentBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		_transparentBlendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
		_transparentBlendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;

		_transparentBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

		_transparentBlendDesc.RenderTarget[1].BlendEnable = true;
		_transparentBlendDesc.RenderTarget[1].SrcBlend = D3D11_BLEND_ONE;
		_transparentBlendDesc.RenderTarget[1].DestBlend = D3D11_BLEND_ONE;
		_transparentBlendDesc.RenderTarget[1].BlendOp = D3D11_BLEND_OP_ADD;
		_transparentBlendDesc.RenderTarget[1].SrcBlendAlpha = D3D11_BLEND_ZERO;
		_transparentBlendDesc.RenderTarget[1].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
		_transparentBlendDesc.RenderTarget[1].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		_transparentBlendDesc.RenderTarget[1].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

		_hr = pDevice->CreateBlendState(&_transparentBlendDesc, &m_TransparentSet);

		COM_ERROR_IF_FAILED(_hr, "CreateBlendState Failed");
	}
	catch (COMException& e)
	{
		ErrorLogger::Log(e);
	}
}

void RasterizerState::DestroyRasterizerState()
{
	m_NowRasterizer = nullptr;
	m_PrevRasterizer = nullptr;
	ReleaseCOM(m_WireFrameRS);
	ReleaseCOM(m_SolidRasterizeStateFCC);
	ReleaseCOM(m_NoCullingRS);
	ReleaseCOM(m_DepthRasterizerState);
	ReleaseCOM(m_AlphaToCoverageBS);
	ReleaseCOM(m_TransparentBS);
	ReleaseCOM(m_NormalDSS);
	ReleaseCOM(m_ComparisonLessEqual);
	ReleaseCOM(m_pBlenderState);
	ReleaseCOM(m_alphaEnableBlendingState);
	ReleaseCOM(m_alphaEnableBlendingStateUI);
	ReleaseCOM(m_pLinearSample);
	ReleaseCOM(m_pTriLinearSaple);
	ReleaseCOM(m_pPointSample);
	ReleaseCOM(m_pAnisotropicSample);
	ReleaseCOM(m_pPCFSample);
	ReleaseCOM(m_pSSAOSample);
	ReleaseCOM(m_pSSAO2Sample);
}

void RasterizerState::ChangeRasterizerState()
{

}

