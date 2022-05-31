#include "OIT2.h"
#include "OITShaderHelper.h"
#include "Object.h"
#include "Shaders.h"
#include "QuadMesh.h"

#include "RasterizerState.h"
#include "IResourceManager.h"
#include "ConstantBufferManager.h"

OIT2::OIT2()
{
}
/*

OIT2::~OIT2()
{
	m_nullSRV->Release();

	for (auto it = m_pic_V.begin(); it != m_pic_V.end(); ++it)
	{
		it->dsv->Release();
		it->depthSrv->Release();
		it->rtv->Release();
		it->backSrv->Release();
	}
	m_pic_V.clear();

	delete m_cbEye;
}

/// <summary>
/// 레스터 라이저 등.. 렌더링의 설정을 만들어준다
/// </summary>
/// <param name="pDevice"></param>
/// <param name="pDeviceContext"></param>
/// <param name="pResourceManager"></param>
void OIT2::Initialize(Microsoft::WRL::ComPtr<ID3D11Device> pDevice, Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext, std::shared_ptr<IResourceManager> pResourceManager)
{
	m_pDevice = pDevice;
	m_pDeviceContext = pDeviceContext;
	m_pResourceManager = pResourceManager;
	
	// 블렌드스테이트		
	m_dxBlendState = RasterizerState::GetOITInitBS();
	// 레스터라이저스테이트
	m_dxRState = RasterizerState::GetOITInitRS();
	// 뎁스스텐실
	m_dxDSState = RasterizerState::GetOITInitDS();

	// 버텍스 쉐이더
	//m_dxVShader = new VShader(pDevice, "StdVS.cso", Std_ILayouts, ARRAYSIZE(Std_ILayouts));
	m_vs = m_pResourceManager->GetVertexShader("vs_OITVS.cso");
	m_vs->AddCB(pDevice, 0, 1, sizeof(SHADER_STD_TRANSF));

	// 픽셀 쉐이더
	//m_dxPShader = new PShader(pDevice, "DepthPeelingPS.cso");
	m_ps = m_pResourceManager->GetPixelShader("ps_DepthPeeling.cso");
	m_ps->AddCB(pDevice, SHADER_REG_CB_MATERIAL, 1, sizeof(SHADER_MATERIAL));
	m_ps->AddSRV(10, 1);
	m_ps->AddSRV(SHADER_REG_SRV_DIFFUSE, 1);
	m_ps->AddSRV(SHADER_REG_SRV_NORMAL, 1);

	//백버퍼를 건드려서 subResource를 만든다 왜지감자..?
	D3D11_TEXTURE2D_DESC texDesc;
	ID3D11Texture2D* tex; // -> 최종적으로 이게 필요한거같다
	
	m_backBuffer->GetDesc(&texDesc);
	texDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	texDesc.Usage = D3D11_USAGE_IMMUTABLE;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = 0;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.ArraySize = 1;
	texDesc.MipLevels = 1;
	D3D11_SUBRESOURCE_DATA data;
	int nSize = 4 * texDesc.Width * texDesc.Height;
	data.pSysMem = (void*)malloc(nSize);
	ZeroMemory((void*)data.pSysMem, nSize);
	data.SysMemPitch = 4 * texDesc.Width;
	m_pDevice->CreateTexture2D(&texDesc, &data, &tex);
	free((void*)data.pSysMem);

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Texture2D.MostDetailedMip = 0;
	m_pDevice->CreateShaderResourceView(tex, &srvDesc, &m_nullSRV); // -> 최최종적으로 m_nullSRV 이것이 필요하다
	tex->Release();

	//콘스탄트 버퍼
	m_cbEye = ConstantBufferManager::GetPSOIT_CAMPOSBuffer();

	// 오브젝트의 뎁스스텐실, 블렌드 스테이트 
	// 이건 바깥에서 해야될듯
	// 쿼드를 대체 어디에 쓰는거지??
	std::shared_ptr<QuadMesh> spMesh = std::make_shared<QuadMesh>(pDevice);
	m_renderQuad = new Object(pDevice, pDeviceContext, "renderObj", spMesh, "vs_OITVS.cso", Std_ILayouts, ARRAYSIZE(Std_ILayouts), "", "", "", "DepthPeelingRenderPS.cso");
	//m_renderQuad->transform->SetRot(-FORWARD, UP);
	//m_renderQuad->transform->SetScale(2, 2, 1);
	
	D3D11_DEPTH_STENCIL_DESC renderDepthDesc;
	ZeroMemory(&renderDepthDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
	renderDepthDesc.DepthEnable = FALSE;
	renderDepthDesc.StencilEnable = FALSE;

	D3D11_BLEND_DESC blendDesc;
	blendDesc.AlphaToCoverageEnable = FALSE;
	blendDesc.IndependentBlendEnable = FALSE;
	blendDesc.RenderTarget[0].BlendEnable = TRUE;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_MAX;
	
	// depthStancilState Modify
	m_renderQuad->m_dxDSState->Release();
	HRESULT hr = m_pDevice->CreateDepthStencilState(&renderDepthDesc, &m_renderQuad->m_dxDSState);
	// blendState Modify
	m_renderQuad->m_dxBlendState->Release();
	hr = m_pDevice->CreateBlendState(&blendDesc, &m_renderQuad->m_dxBlendState);

	m_renderQuad->m_vs->AddCB(m_pDevice, 0, 1, sizeof(SHADER_STD_TRANSF));
	m_renderQuad->m_ps->AddSRV(SHADER_REG_SRV_DIFFUSE, 1);
}

//이것도 바깥(렌더러)에서 해야될듯하다
/// <summary>
/// 
/// </summary>
/// <param name="worldTM"></param>
/// <param name="viewTM"></param>
/// <param name="projTM"></param>
/// <param name="camPos"></param>
/// <param name="objs"></param>
/// <param name="nPic"></param>
void OIT2::Run(const DirectX::XMMATRIX& worldTM,
	const DirectX::XMMATRIX& viewTM,
	const DirectX::XMMATRIX& projTM,
	const DirectX::XMFLOAT3 camPos,
	const std::vector<Object*>& objs, int nPic)
{
	ID3D11RenderTargetView* oriRTV;
	ID3D11DepthStencilView* oriDSV;
	m_pDeviceContext->OMGetRenderTargets(1, &oriRTV, &oriDSV);
	
	PreRun(nPic);

	// 콘스탄트 버퍼 카메라 pos
	m_cbEye->data.EyePos = camPos;
	m_cbEye->ApplyChanges();
	m_pDeviceContext->PSSetConstantBuffers(SHADER_REG_CB_EYE, 1, m_cbEye->GetAddressOf());

	// 블렌드 스테이트, 래스터라이저 스테이트, 뎁스스텐실 스테이트 Apply
	float blendFactors[] = { 0.0f, 0.0f, 0.0f, 0.0f };
	m_pDeviceContext->OMSetBlendState(m_dxBlendState, blendFactors, 0xffffffff);
	m_pDeviceContext->RSSetState(m_dxRState);
	m_pDeviceContext->OMSetDepthStencilState(m_dxDSState, 0);

	// 오브젝트를 업데이트 lparam만큼 루프를 돈다(why..?)
	// 뎁스를 이것으로 계산하는거같은데 잘모르겠다ㅏ

	// 렌더링할 오브젝트를 가져와서 뭔가해준다. 쉐이더관련인거 같은데
	for (int i = 0; i < nPic; ++i)
	{
		m_pDeviceContext->OMSetRenderTargets(1, &m_pic_V[i].rtv, m_pic_V[i].dsv);

		m_ps->WriteSRV(10, (i == 0) ? m_nullSRV : m_pic_V[i - 1].depthSrv);

		for (auto it = objs.begin(); it != objs.end(); ++it)
		{
			Object* obj = *it;

			obj->Update();

			//const SHADER_STD_TRANSF STransformation(obj->transform->WorldMatrix(), viewTM, projTM, NULL, NULL, NULL, NULL);
			
			ID3D11ShaderResourceView* mainTex, * normal;
			obj->GetMainTex(&mainTex);
			obj->GetNormal(&normal);

			SHADER_MATERIAL material;
			obj->GetMaterial(&material);

			//m_vs->WriteCB(m_pDeviceContext, 0, &STransformation);
			m_vs->Apply(m_pDeviceContext);
			
			m_ps->WriteCB(m_pDeviceContext, SHADER_REG_CB_MATERIAL, &material);
			m_ps->WriteSRV(SHADER_REG_SRV_DIFFUSE, mainTex);
			m_ps->WriteSRV(SHADER_REG_SRV_NORMAL, normal);
			m_ps->Apply(m_pDeviceContext);

			obj->RenderGeom(m_pDeviceContext);
		}
	}
	// 픽셀쉐이더 unordered_map에 담겨있는 리소스를 처음부터 끝까지 적용해주는데 왜하는거지??
	m_ps->UnboundSRVAll(m_pDeviceContext);
	
	m_pDeviceContext->OMSetRenderTargets(1, &oriRTV, oriDSV);

}

void OIT2::Render(int iIndex)
{
	if (iIndex < 0)
	{
		for (auto it = m_pic_V.rbegin(); it != m_pic_V.rend(); it++)
		{
			m_renderQuad->m_ps->WriteSRV(SHADER_REG_SRV_DIFFUSE, it->backSrv);
			m_renderQuad->Render(m_pDeviceContext, XMMatrixIdentity(), XMMatrixIdentity());
		}
	}
	else
	{
		m_renderQuad->m_ps->WriteSRV(SHADER_REG_SRV_DIFFUSE, m_pic_V[iIndex].backSrv);
		m_renderQuad->Render(m_pDeviceContext, XMMatrixIdentity(), XMMatrixIdentity());
	}
}

void OIT2::GetSRV(int index, ID3D11ShaderResourceView** ppSRV)
{
	*ppSRV = m_pic_V[index].depthSrv;
}

void OIT2::GetDSV(int index, ID3D11DepthStencilView** ppDSV)
{
	*ppDSV = m_pic_V[index].dsv;
}

void OIT2::GetRTV(int index, ID3D11RenderTargetView** ppRTV)
{
	*ppRTV = m_pic_V[index].rtv;
}

void OIT2::Create(PictureInfo* picInfo)
{
	D3D11_TEXTURE2D_DESC texDesc;
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	D3D11_SHADER_RESOURCE_VIEW_DESC depthSrvDesc;
	D3D11_SHADER_RESOURCE_VIEW_DESC backSrvDesc;
	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
	ID3D11Texture2D* depthTex;
	ID3D11Texture2D* renderTex;
	ID3D11DepthStencilView* dsv;
	ID3D11ShaderResourceView* depthSrv;
	ID3D11RenderTargetView* rtv;
	ID3D11ShaderResourceView* backSrv;

	m_depthStencilBuffer->GetDesc(&texDesc);
	texDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	m_pDevice->CreateTexture2D(&texDesc, nullptr, &depthTex);

	m_dsView->GetDesc(&dsvDesc);
	m_pDevice->CreateDepthStencilView(depthTex, &dsvDesc, &dsv);
	m_pDeviceContext->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH, 1.0f, NULL);

	depthSrvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	depthSrvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	depthSrvDesc.Texture2D.MipLevels = 1;
	depthSrvDesc.Texture2D.MostDetailedMip = 0;
	m_pDevice->CreateShaderResourceView(depthTex, &depthSrvDesc, &depthSrv);
	depthTex->Release();

	m_backBuffer->GetDesc(&texDesc);
	texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	m_pDevice->CreateTexture2D(&texDesc, nullptr, &renderTex);
	m_rtv->GetDesc(&rtvDesc);
	m_pDevice->CreateRenderTargetView(renderTex, &rtvDesc, &rtv);
	backSrvDesc.Format = rtvDesc.Format;
	backSrvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	backSrvDesc.Texture2D.MipLevels = 1;
	backSrvDesc.Texture2D.MostDetailedMip = 0;
	m_pDevice->CreateShaderResourceView(renderTex, &backSrvDesc, &backSrv);
	renderTex->Release();

	picInfo->dsv = dsv;
	picInfo->depthSrv = depthSrv;
	picInfo->rtv = rtv;
	picInfo->backSrv = backSrv;
}

void OIT2::PreRun(int count)
{
	ID3D11RenderTargetView* nullRTV = nullptr;
	m_pDeviceContext->OMSetRenderTargets(1, &nullRTV, nullptr);
	m_ps->UnboundSRVAll(m_pDeviceContext);

	const int nAdd = count - m_pic_V.size();
	for (int i = 0; i < nAdd; ++i)
	{
		PictureInfo pic;
		Create(&pic);
		m_pic_V.push_back(pic);
	}

	for (int i = nAdd; i < 0; ++i)
	{
		auto it = m_pic_V.rbegin();
		it->dsv->Release();
		it->depthSrv->Release();
		it->rtv->Release();
		m_pic_V.pop_back();
	}

	for (auto it = m_pic_V.begin(); it != m_pic_V.end(); ++it)
	{
		m_pDeviceContext->ClearDepthStencilView(it->dsv, D3D11_CLEAR_DEPTH, 1.0, NULL);
		const FLOAT colBlack[4] = { 0,0,0,0 };
		m_pDeviceContext->ClearRenderTargetView(it->rtv, colBlack);
	}
}
*/