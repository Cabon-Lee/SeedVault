#include "Renderer.h"

#include "IResourceManager.h"

#include "AdapterManager.h"
#include "Shaders.h"
#include "D2D.h"
#include "IBL.h"
#include "OIT.h"
#include "BoundingVolume.h"
#include "RenderQueue.h"
#include "PixelPicking.h"

#include "Quad.h"
#include "SpriteQuad.h"
#include "ParticleManager.h"
#include "Face.h"
#include "Line.h"
#include "BoundingVolume.h"

#include "SkyBoxSphere.h"
#include "SkyBoxCube.h"

#include "LoadingScene.h"

#include "StringHelper.h"
#include "SimpleMath.h"

#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "VertexHelper.h"
#include "StagingBuffer.h"

#include "NodeData.h"
#include "MaterialData.h"

#include "VisualDebugger.h"
#include "HLSLShaderManager.h"
#include "SSAO.h"

#include "LightClass.h"
#include "LightManager.h"
#include "Forward.h"
#include "Deferred.h"
#include "PostProcess.h"
#include "DebugRender.h"

#include "RasterizerState.h"
#include "ConstantBufferManager.h"
#include "RenderTargetView.h"
#include "DepthStencilView.h"
#include "CubeMapRenderTarget.h"

#include "dxgidebug.h"
#include "CATrace.h"

#include <WICTextureLoader.h>
#include <DDSTextureLoader.h>
#include <wrl/event.h>


std::vector<void*> Renderer::m_ShaderIndex_V;

#define ENUM(x) static_cast<unsigned int>(x)
#define VS_SHADER(x) reinterpret_cast<class VertexShader*>(Renderer::m_ShaderIndex_V[static_cast<unsigned int>(x)])
#define PS_SHADER(x) reinterpret_cast<class PixelShader*>(Renderer::m_ShaderIndex_V[static_cast<unsigned int>(x)])
#define GS_SHADER(x) reinterpret_cast<class GeometryShader*>(Renderer::m_ShaderIndex_V[static_cast<unsigned int>(x)])

DirectX::SimpleMath::Vector3 LinearToGamma(const DirectX::SimpleMath::Vector3 color)
{
	return color * color;
}

Renderer::Renderer()
	: m_pAdapterManager(nullptr)
	, m_pResourceManager(nullptr)
	, m_pFont(nullptr)
	, m_Enable4xMsaa(false)
	, m_4xMasaaQuality(0)
	, m_ClientWidth(0)
	, m_ClientHeight(0)
	, m_hWnd(0)
	, m_pRenderTarget(nullptr)
	, m_pDepthStencil(nullptr)

	// 초기화 변수
	, m_D3DDriverType(D3D_DRIVER_TYPE_HARDWARE)
	, m_eFeatureLevel()
	, m_pDevice(nullptr)
	, m_pDeviceContext(nullptr)
	, m_p1Swapchain(nullptr)

	, m_ViewTM()
	, m_ProjectionTM()
	//, m_Rendering(eRENDERRING::FORWARD)
	, m_Rendering(eRENDERRING::DEFERRED)
	//, m_LightCount(-1)
{

}

Renderer::~Renderer()
{
}

bool Renderer::Initialize(int hwnd, int width, int height)
{
	m_hWnd = reinterpret_cast<HWND>(hwnd);

	m_ClientWidth = width;
	m_ClientHeight = height;
	CA_TRACE("현재 창 크기 Width : %d | Height : %d", m_ClientWidth, m_ClientHeight);

	UINT createDeviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;

#ifdef _DEBUG
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;

#endif // DEBUG || _DEBUG

	try
	{
		// Com오브젝트를 초기화(dds 외에 다른 텍스쳐를 사용하려면 써야함)
		HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);

		COM_ERROR_IF_FAILED(hr, "COInitialize Fail");

		D3D_FEATURE_LEVEL featureLevels[] =
		{
			D3D_FEATURE_LEVEL_11_0,
			D3D_FEATURE_LEVEL_10_1,
			D3D_FEATURE_LEVEL_10_0,
			D3D_FEATURE_LEVEL_9_3,
			D3D_FEATURE_LEVEL_9_2,
			D3D_FEATURE_LEVEL_9_1
		};

		hr = D3D11CreateDevice(
			nullptr,                 // default adapter
			m_D3DDriverType,
			nullptr,                 // no software device
			createDeviceFlags,
			featureLevels,
			ARRAYSIZE(featureLevels),              // default feature level array
			D3D11_SDK_VERSION,
			&m_pDevice,
			&m_eFeatureLevel,
			&m_pDeviceContext);

		COM_ERROR_IF_FAILED(hr, "CreateDevice Fail");

		hr = m_pDevice->CreateDeferredContext(0, m_pDeferredDeviceContext.GetAddressOf());

		COM_ERROR_IF_FAILED(hr, "CreateDeferredContext Fail");

		if (m_eFeatureLevel != D3D_FEATURE_LEVEL_11_0)
		{
			MessageBox(0, L"Direct3D Feature Level 11 unsupported.", 0, 0);
			return false;
		}

		//---------------------------------------------------------------------------------------
		// MSAA(MultiSampling Anti-Aliasing) 초기화만들기
		//---------------------------------------------------------------------------------------
		UINT _4xMsaaQuality;

		hr = m_pDevice->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM, 4, &_4xMsaaQuality);


		//---------------------------------------------------------------------------------------
		// 어댑터 팩토리 생성
		// 어댑터들의 정보들은 SwapChain에서도 사용되고 디버그 정보로도 출력된다
		// 필요한 곳이 많다는 생각이 들어 클래스로 따로 만들었다.
		//---------------------------------------------------------------------------------------
		CreateAdapter();

		//---------------------------------------------------------------------------------------
		// 스왑 체인 생성
		//---------------------------------------------------------------------------------------
		m_Forward = std::make_shared<Forward>();
		m_Forward->Initialize(m_pDevice.Get(), width, height);
		CreateSwapChain(_4xMsaaQuality);



	}
	catch (COMException& exception)
	{
		ErrorLogger::Log(exception);
		return false;
	}



	// 레스터라이저 스테이트 초기화
	// 내부가 스태틱으로 되어있어 어디서든 Rasterizer를 끌어다 쓸 수 있다
	RasterizerState::CreateRasterizerState(m_pDevice.Get());

	//---------------------------------------------------------------------------------------
	// 카메라가 생성 된 후 리사이즈
	// 카메라 매니저의 SetLens를 사용하기 위해
	//---------------------------------------------------------------------------------------

	m_Deferred = std::make_shared<Deferred>();
	m_Deferred->Initialize(m_pDevice.Get(), width, height);

	m_DebugRender = std::make_shared<DebugRender>();
	m_DebugRender->Initialize(m_pDevice);

	m_LightManager = std::make_shared<LightManager>();
	m_LightManager->Initialize(m_pDevice.Get());

	m_HLSLShader = std::make_shared<HLSLShaderManager>();
	m_HLSLShader->Initialize(m_pDevice);

	m_PostProcess = std::make_shared<PostProcess>();
	m_PostProcess->Initialize(m_pDevice, width, height);

	m_pVisualDebugger = std::make_unique<VisualDebugger>();
	m_pVisualDebugger->Initilze(m_pDevice, m_pDeviceContext);


	m_pRenderTarget = std::make_shared<RenderTargetView>();
	m_pRenderTarget->Initialize(m_pDevice.Get(), width, height,
		D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE,
		DXGI_FORMAT_R16G16B16A16_FLOAT,
		DXGI_FORMAT_R16G16B16A16_FLOAT,
		DXGI_FORMAT_R16G16B16A16_FLOAT);

	m_pDebugRenderTarget = std::make_shared<RenderTargetView>();
	m_pDebugRenderTarget->Initialize(m_pDevice.Get(), width, height,
		D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE,
		DXGI_FORMAT_R8G8B8A8_UNORM,
		DXGI_FORMAT_R8G8B8A8_UNORM,
		DXGI_FORMAT_R8G8B8A8_UNORM);

	m_pOutLineRenderTarget = std::make_shared<RenderTargetView>();
	m_pOutLineRenderTarget->Initialize(m_pDevice.Get(), width, height,
		D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE,
		DXGI_FORMAT_R8G8B8A8_UNORM,
		DXGI_FORMAT_R8G8B8A8_UNORM,
		DXGI_FORMAT_R8G8B8A8_UNORM);

	m_pIDColorRenderTarget = std::make_shared<RenderTargetView>();
	m_pIDColorRenderTarget->Initialize(m_pDevice.Get(), width, height,
		D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE,
		DXGI_FORMAT_R32G32B32A32_FLOAT,
		DXGI_FORMAT_R32G32B32A32_FLOAT,
		DXGI_FORMAT_R32G32B32A32_FLOAT);

	m_pUIRenderTarget = std::make_shared<RenderTargetView>();
	m_pUIRenderTarget->Initialize(m_pDevice.Get(), width, height,
		D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE,
		DXGI_FORMAT_R8G8B8A8_UNORM,
		DXGI_FORMAT_R8G8B8A8_UNORM,
		DXGI_FORMAT_R8G8B8A8_UNORM);

	m_pDepthStencil = std::make_shared<DepthStencilView>();
	m_pDepthStencil->Initialize(m_pDevice.Get(), width, height,
		D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE,	// 그냥 BIND_DEPTH_STENCIL만 하면 오류가..
		DXGI_FORMAT_R24G8_TYPELESS,
		DXGI_FORMAT_R24_UNORM_X8_TYPELESS,
		DXGI_FORMAT_D24_UNORM_S8_UINT);


	m_OrthoTM = DirectX::XMMatrixOrthographicOffCenterLH(0.0f, (float)m_ClientWidth, (float)m_ClientHeight, 0.0f, 0.0f, 1.0f);

	// sprite용
	m_SpriteQuad = std::make_shared<SpriteQuad>();
	m_SpriteQuad->SetBuffer(m_pDevice.Get());

	//D2D 폰트
	m_pD2D = std::make_shared<D2D>();
	m_pD2D->Initialize(m_hWnd, m_p1Swapchain.Get(), width, height);


	m_PixelPicker = std::make_unique<class PixelPicking>();
	m_PixelPicker->Initialize(m_pDevice);

	ConstantBufferManager::Initialize(m_pDevice.Get(), m_pDeviceContext.Get());


	/// 테스트
	//m_pOIT = std::make_shared<OIT>();
	//m_pOIT->Initialize(m_pDevice, width, height);

	m_FrustumVolume = std::make_unique<FrustumVolume>();

	m_RenderQueue = std::make_shared<RenderQueue>();
	m_RenderQueue->Initialize(m_pDevice);

	m_InstanceStaticMesh = std::make_shared<VertexShader>();
	m_InstanceStaticMesh->Initialize(m_pDevice, L"../Data/Shader/vs_Instance.hlsl", "StaticMesh", "vs_5_0");

	const D3D11_INPUT_ELEMENT_DESC _nowStaticInput[8] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{ "TANGENT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{ "WORLD", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "WORLD", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 16, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "WORLD", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 32, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "WORLD", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 48, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
	};

	m_pDevice->CreateInputLayout(_nowStaticInput, 8,
		m_InstanceStaticMesh->GetVertexShaderBuffer()->GetBufferPointer(),
		m_InstanceStaticMesh->GetVertexShaderBuffer()->GetBufferSize(),
		m_StaticInput.GetAddressOf());

	m_InstanceSkinnedMesh = std::make_shared<VertexShader>();
	m_InstanceSkinnedMesh->Initialize(m_pDevice, L"../Data/Shader/vs_Instance.hlsl", "SkinnedMesh", "vs_5_0");

	const D3D11_INPUT_ELEMENT_DESC _nowSkinnedInput[10] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{ "WEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 44, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{ "BONEINDICES", 0, DXGI_FORMAT_R32G32B32A32_UINT , 0, 60, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{ "WORLD", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "WORLD", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 16, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "WORLD", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 32, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "WORLD", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 48, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
	};

	m_pDevice->CreateInputLayout(_nowSkinnedInput, 10,
		m_InstanceSkinnedMesh->GetVertexShaderBuffer()->GetBufferPointer(),
		m_InstanceSkinnedMesh->GetVertexShaderBuffer()->GetBufferSize(),
		m_SkinnedInput.GetAddressOf());

	m_pCombineVertexShader = std::make_shared<VertexShader>();
	m_pCombineVertexShader->Initialize(m_pDevice, L"../Data/Shader/ps_deferredTotal.hlsl", "FullScreenQuadVS", "vs_5_0");

	m_pCombinePixelShader = std::make_shared<PixelShader>();
	m_pCombinePixelShader->Initialize(m_pDevice, L"../Data/Shader/ps_deferredTotal.hlsl", "DebugRender", "ps_5_0");

	m_LoadingScene = std::make_shared<LoadingScene>();
	//m_LoadingScene->Initialize(m_pDevice, m_pDeviceContext);

	return true;
}

void Renderer::Destroy()
{

	m_RenderQueue.reset();
	m_FrustumVolume.reset();
	m_PostProcess.reset();

	CoUninitialize();
}

void Renderer::ResetRenderer()
{
	m_pIBL->ClearReflectionProbe();
	m_ParticleManager->ResetParticle();
}

void Renderer::OnResize(int width, int height)
{
	m_ClientWidth = width;
	m_ClientHeight = height;
	CA_TRACE("현재 창 크기 Width : %d | Height : %d", m_ClientWidth, m_ClientHeight);

	if (m_p1Swapchain != nullptr)
	{
		//CreateRengerTarget();
		//CreateDepthAndStencil();
	}

	/// 이부분이 문제다
	/// 스왑체인이랑 렌더 타겟을 분리하는 것을 해봐야할 것 같은데
	/// 기존에는 어떻게 했더라?

	/// 기존에는 RenderTarget을 Forward 등으로 따로 빼지 않았음
	/// 무조건 화면에 그림을 그릴때는 Renderer의 멤버변수로 존재하는 RenderTarget으로 그림을 그림

	// 현재 만들어져있는 스왑체인의 Desc정보들을 가져올 준비를 한다.
	// 스왑체인의 ResizeBuffer 만으로는 변경된 해상도로 2D를 위한 Surface를 만들 수 없기 때문이다.
	// 그래서 기존 정보 + 새로운 해상도의 크기로 SwapChain을 재생성한다.
	DXGI_SWAP_CHAIN_DESC1* _nowSwapDesc = new DXGI_SWAP_CHAIN_DESC1;
	DXGI_SWAP_CHAIN_FULLSCREEN_DESC* _nowSwapFullDesc = new DXGI_SWAP_CHAIN_FULLSCREEN_DESC;
	m_p1Swapchain->GetDesc1(_nowSwapDesc);
	m_p1Swapchain->GetFullscreenDesc(_nowSwapFullDesc);

	// 기존 Desc는 유지한채 해상도 정보만 수정한다
	_nowSwapDesc->Width = m_ClientWidth;
	_nowSwapDesc->Height = m_ClientHeight;

	// 기존 스왑 체인을 해제한다
	m_p1Swapchain.Reset();

	try
	{
		HRESULT _hr;

		// 기존 SwapChainDesc를 토대로 해상도만 변경해 SwapChain은 생성한다
		_hr = m_pAdapterManager->GetIDXGIFactory()->CreateSwapChainForHwnd(
			m_pDevice.Get(),
			m_hWnd,
			_nowSwapDesc,
			_nowSwapFullDesc,
			nullptr,
			&m_p1Swapchain);

		COM_ERROR_IF_FAILED(_hr, "CreateSwapChainForHwnd Fail");

		CComPtr<ID3D11Texture2D> _backBuffer = nullptr;
		_hr = m_p1Swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&_backBuffer));

		COM_ERROR_IF_FAILED(_hr, "SwapChain1 GetBuffer Fail");

		// Forward의 렌더 타겟을 변경된 SwapChain의 ID3D11Texture2D 형식에 맞게 다시 만들어준다
		m_Forward->OnResize(m_pDevice.Get(), m_ClientWidth, m_ClientHeight);
		m_Forward->GetRenderTarget()->CreateRTVFromID3D11Texture(m_pDevice.Get(), _backBuffer);
		m_Forward->SetViewports(m_pDeviceContext);

	}
	catch (COMException& e)
	{
		ErrorLogger::Log(e);
	}

	m_Deferred->OnResize(m_pDevice.Get(), m_ClientWidth, m_ClientHeight);

	m_PostProcess->OnResize(m_pDevice, m_ClientWidth, m_ClientHeight);

	m_pD2D->OnResize(m_p1Swapchain.Get(), m_ClientWidth, m_ClientHeight);

	m_pRenderTarget->Reset();
	m_pRenderTarget->Initialize(m_pDevice.Get(), width, height,
		D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE,
		DXGI_FORMAT_R16G16B16A16_FLOAT,
		DXGI_FORMAT_R16G16B16A16_FLOAT,
		DXGI_FORMAT_R16G16B16A16_FLOAT);

	m_pDebugRenderTarget->Reset();
	m_pDebugRenderTarget->Initialize(m_pDevice.Get(), width, height,
		D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE,
		DXGI_FORMAT_R8G8B8A8_UNORM,
		DXGI_FORMAT_R8G8B8A8_UNORM,
		DXGI_FORMAT_R8G8B8A8_UNORM);

	m_pOutLineRenderTarget->Reset();
	m_pOutLineRenderTarget->Initialize(m_pDevice.Get(), width, height,
		D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE,
		DXGI_FORMAT_R8G8B8A8_UNORM,
		DXGI_FORMAT_R8G8B8A8_UNORM,
		DXGI_FORMAT_R8G8B8A8_UNORM);

	m_pIDColorRenderTarget->Reset();
	m_pIDColorRenderTarget->Initialize(m_pDevice.Get(), width, height,
		D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE,
		DXGI_FORMAT_R32G32B32A32_FLOAT,
		DXGI_FORMAT_R32G32B32A32_FLOAT,
		DXGI_FORMAT_R32G32B32A32_FLOAT);

	m_pUIRenderTarget->Reset();
	m_pUIRenderTarget->Initialize(m_pDevice.Get(), width, height,
		D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE,
		DXGI_FORMAT_R8G8B8A8_UNORM,
		DXGI_FORMAT_R8G8B8A8_UNORM,
		DXGI_FORMAT_R8G8B8A8_UNORM);

	m_pDepthStencil->Reset();
	m_pDepthStencil->Initialize(m_pDevice.Get(), width, height,
		D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE,	// 그냥 BIND_DEPTH_STENCIL만 하면 오류가..
		DXGI_FORMAT_R24G8_TYPELESS,
		DXGI_FORMAT_R24_UNORM_X8_TYPELESS,
		DXGI_FORMAT_D24_UNORM_S8_UINT);

	m_SSAOManager->OnSize(m_pDevice, width, height, 0.25f * DirectX::XM_PI, 1000.0f);

	delete(_nowSwapDesc);
	_nowSwapDesc = nullptr;
	delete(_nowSwapFullDesc);
	_nowSwapDesc = nullptr;

}

void Renderer::CreateAdapter()
{
	m_pAdapterManager = new AdapterManager();

	// 내부에서 들고 있을게 아니라서 원시 포인터로 넘겼다
	m_pAdapterManager->Initialize(m_hWnd, m_pDevice.Get());

	unsigned int _deviceIndex = 0;
	DXGI_OUTPUT_DESC _nowOutput;
	while (m_pAdapterManager->GetMonitorDesc(_deviceIndex, _nowOutput))
	{
		std::string _nowOuputName = StringHelper::WideToString(_nowOutput.DeviceName);
		CA_TRACE("%d 번째 모니터 | 이름 %s | 핸들 %d", _deviceIndex, _nowOuputName.c_str(), _nowOutput.Monitor);
		_deviceIndex++;
	}

	_deviceIndex = 0;
	DXGI_ADAPTER_DESC _nowAdapter;
	while (m_pAdapterManager->GetVideoCardDesc(_deviceIndex, _nowAdapter))
	{
		std::string _nowAdapterName = StringHelper::WideToString(_nowAdapter.Description);
		CA_TRACE("%d 번째 비디오 카드 | 이름 %s | 메모리 %d",
			_deviceIndex,
			_nowAdapterName.c_str(),
			(int)(_nowAdapter.DedicatedVideoMemory / 1024));
		_deviceIndex++;
	}
}

void Renderer::CreateSwapChain(UINT msaaQuality)
{
	assert(m_pDevice != nullptr);

	//---------------------------------------------------------------------------------------
	// 스왑체인 생성
	//---------------------------------------------------------------------------------------
	/// SwapChain 초기화 
	/// 현재 11.1 부터는 CreateSwapChain은 사용을 권장하고 있지 않다.
	/// CreateSwapChainForHwnd를 통해 스왑 체인을 만들 예정이다.
	/// 단, 이때 DXGI_SWAP_CHAIN_DESC 대신 DXGI_SWAP_CHAIN_DESC1을 사용한다.

	/// **** 스왑체인을 초기화할 때에는 반드시 모든 멤버 변수를 {0}으로 초기화하는 과정을 거쳐야한다.
	/// 하지 않을 경우, 스왑체인 포인터로 값을 반환해 올 수 없는 이슈가 발생해 초기화에 실패한다.
	/// 반드시 기억할 것!!

	DXGI_SWAP_CHAIN_DESC1 _SwapChain = { 0 };
	_SwapChain.Width = m_ClientWidth;
	_SwapChain.Height = m_ClientHeight;

	//_SwapChain.Format = m_pAdapterManager->GetDXGIModeDesc(0)->Format;
	_SwapChain.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	_SwapChain.Stereo = false;

	/// 멀티샘플링 안티앨리어싱 적용 여부
	if (m_Enable4xMsaa)
	{
		_SwapChain.SampleDesc.Count = 4;
		_SwapChain.SampleDesc.Quality = msaaQuality - 1;
	}
	else
	{
		_SwapChain.SampleDesc.Count = 1;
		_SwapChain.SampleDesc.Quality = 0;
	}

	_SwapChain.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	_SwapChain.BufferCount = 2;
	_SwapChain.Flags = 0;
	_SwapChain.Scaling = DXGI_SCALING_STRETCH;
	_SwapChain.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	//_SwapChain.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	// DXGI_SWAP_EFFECT_DISCARD은 legacy한 bitblit형 모델이라 FLIP형 모델로 바꿨다.
	/// 바꾸려고 했다가 아직 못 바꿨다...
	/// dxgi1_4를 사용하는 것 같고, factory4를 이용한다
	/// 그렇게 수정도 해봤지만 아직 작동되지 않는다.

	DXGI_SWAP_CHAIN_FULLSCREEN_DESC _swapChainDescFull;
	_swapChainDescFull.RefreshRate = m_pAdapterManager->GetDXGIModeDesc(0)->RefreshRate;
	_swapChainDescFull.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	_swapChainDescFull.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;

	// 스왑체인 생성 시 Window모드가 아니라면 전체화면으로 창을 생성하게 된다.
	// 이때, WM_SIZE가 호출되어 콜백 함수로 점프한다.
	// 그러면 OnResize 등의 함수들이 자연스럽게 이어지게 된다.
	_swapChainDescFull.Windowed = true;

	try
	{
		HRESULT _hr;

		_hr = m_pAdapterManager->GetIDXGIFactory()->CreateSwapChainForHwnd(
			m_pDevice.Get(),
			m_hWnd,
			&_SwapChain,
			&_swapChainDescFull,
			nullptr,
			&m_p1Swapchain);

		CA_TRACE("스왑 체인 생성");

		COM_ERROR_IF_FAILED(_hr, "CreateSwapChainForHwnd Fail");

		CComPtr<ID3D11Texture2D> _backBuffer = nullptr;
		_hr = m_p1Swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&_backBuffer));

		COM_ERROR_IF_FAILED(_hr, "SwapChain1 GetBuffer Fail");

		// Forward의 렌더 타겟을 변경된 SwapChain의 ID3D11Texture2D 형식에 맞게 다시 만들어준다
		m_Forward->OnResize(m_pDevice.Get(), m_ClientWidth, m_ClientHeight);
		m_Forward->GetRenderTarget()->CreateRTVFromID3D11Texture(m_pDevice.Get(), _backBuffer);
		m_Forward->SetViewports(m_pDeviceContext);
	}
	catch (COMException& e)
	{
		ErrorLogger::Log(e);
	}

	assert(m_p1Swapchain != nullptr);
}

void Renderer::CreateRengerTarget()
{
	// Resize가 불렸을때, 처음으로 렌더러가 생성되었을 때 수행하는 루틴

	if (m_pRenderTarget == nullptr)
	{
		// 스왑체인을 통한 렌더타겟뷰의 초기화는 별도 이니셜라이즈 과정을 거치지 않았다
		m_pRenderTarget = std::make_shared<RenderTargetView>();
	}
	else
	{
		m_pRenderTarget->Reset();
	}

	try
	{
		HRESULT hr;

		hr = m_p1Swapchain->ResizeBuffers(1, m_ClientWidth, m_ClientHeight, DXGI_FORMAT_R8G8B8A8_UNORM, 0);

		COM_ERROR_IF_FAILED(hr, "SwapChain ResizeBuffers 실패");

		CComPtr<ID3D11Texture2D> _backBuffer = nullptr;
		hr = m_p1Swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&_backBuffer));

		COM_ERROR_IF_FAILED(hr, "SwapChain GetBuffer 실패");

		// 스왑체인을 통해 렌더타겟뷰를 생성하고, 이를 바탕으로 생성된 뷰포트를 붙인다
		hr = m_pRenderTarget->CreateRTVFromID3D11Texture(m_pDevice.Get(), _backBuffer);
		//m_pDeviceContext->RSSetViewports(1, &m_pRenderTarget->GetViewPort());

		COM_ERROR_IF_FAILED(hr, "ID3D11Device CreateRenderTargetView");

	}
	catch (COMException& e)
	{
		ErrorLogger::Log(e);
	}
}

void Renderer::CreateDepthAndStencil()
{
	//---------------------------------------------------------------------------------------
	// 뎁스 스텐실 텍스쳐 만드는 루틴
	// 뎁스 버퍼는 카메라에서 가장 가까운 물체의 깊이 정보를 저장하는 2D Texture다
	// 그래서 작성도 2D Texture로 해준다.
	// DX12에서는 D3D12_RESOURCE_DESC라는 구조체를 쓰는 것 같다.
	//---------------------------------------------------------------------------------------

	if (m_pDepthStencil == nullptr)
	{
		m_pDepthStencil = std::make_shared<DepthStencilView>();
	}
	else
	{
		m_pDepthStencil->Reset();
	}

	try
	{
		HRESULT hr;

		hr = m_pDepthStencil->Initialize(
			m_pDevice.Get(),
			m_ClientWidth,
			m_ClientHeight,
			D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE,	// 그냥 BIND_DEPTH_STENCIL만 하면 오류가..
			DXGI_FORMAT_R24G8_TYPELESS,
			DXGI_FORMAT_R24_UNORM_X8_TYPELESS,
			DXGI_FORMAT_D24_UNORM_S8_UINT);

		COM_ERROR_IF_FAILED(hr, "DepthStencil 초기화 실패");

	}
	catch (COMException& e)
	{
		ErrorLogger::Log(e);
	}
}

void Renderer::LoadResource()
{
	CA_TRACE("렌더러 리소스 초기화 시작");

	m_pIBL = std::make_unique<IBL>();
	m_pIBL->Initialize(m_pDevice);

	m_pSkyBox = std::make_shared<SkyBoxSphere>();
	m_pSkyBox->Initialize(m_pDevice);

	m_pSkyBoxCube = std::make_shared<SkyBoxCube>();
	m_pSkyBoxCube->Initialize(m_pDevice);

	m_ShaderIndex_V.resize(ENUM(eSHADER::MAX_COUNT));

	auto _storeShader =
		[this](std::vector<void*>& vec, unsigned int num, const std::string& name)
	{
		char _head = name.at(0);
		if (_head == 'v')
			vec[num] = static_cast<void*>(this->m_pResourceManager->GetVertexShader(name).get());
		else if (_head == 'p')
			vec[num] = static_cast<void*>(this->m_pResourceManager->GetPixelShader(name).get());
		else if (_head == 'g')
			vec[num] = static_cast<void*>(this->m_pResourceManager->GetGeometryShader(name).get());
	};

	_storeShader(m_ShaderIndex_V, ENUM(eSHADER::VS_STATICMESH), "vs_StaticMesh.cso");
	_storeShader(m_ShaderIndex_V, ENUM(eSHADER::VS_SKINNEDMESH), "vs_SkinnedMesh.cso");
	_storeShader(m_ShaderIndex_V, ENUM(eSHADER::PS_IBLDEFERRED), "ps_IBL_Deferred.cso");
	_storeShader(m_ShaderIndex_V, ENUM(eSHADER::PS_NONE_IBLDEFERRED), "ps_None_IBL_Deferred.cso");

	_storeShader(m_ShaderIndex_V, ENUM(eSHADER::VS_ENVIRONMENT), "vs_Environment.cso");
	_storeShader(m_ShaderIndex_V, ENUM(eSHADER::PS_ENVIRONMENT), "ps_Environment.cso");
	_storeShader(m_ShaderIndex_V, ENUM(eSHADER::PS_IRRADIANCEMAP), "ps_irradianceMap.cso");
	_storeShader(m_ShaderIndex_V, ENUM(eSHADER::PS_PREFILTER), "ps_prefilter.cso");
	_storeShader(m_ShaderIndex_V, ENUM(eSHADER::PS_REFLECTIONPROBE), "ps_reflectionProbe.cso");
	_storeShader(m_ShaderIndex_V, ENUM(eSHADER::PS_IRRADIANCEMAPVECTOR3), "ps_irradianceMapVector3.cso");
	_storeShader(m_ShaderIndex_V, ENUM(eSHADER::GS_STREAMOUT), "gs_StreamOut.cso");
	_storeShader(m_ShaderIndex_V, ENUM(eSHADER::GS_DRAWPASS), "gs_DrawPass.cso");
	_storeShader(m_ShaderIndex_V, ENUM(eSHADER::VS_STREAMOUT), "vs_StreamOut.cso");
	_storeShader(m_ShaderIndex_V, ENUM(eSHADER::VS_DRAWPASS), "vs_DrawPass.cso");
	_storeShader(m_ShaderIndex_V, ENUM(eSHADER::PS_PARTICLEDRAW), "ps_ParticleDraw.cso");

	_storeShader(m_ShaderIndex_V, ENUM(eSHADER::VS_COMBINE), "vs_combine.cso");
	_storeShader(m_ShaderIndex_V, ENUM(eSHADER::PS_OPAQUEPASS), "ps_OpaquePass.cso");
	_storeShader(m_ShaderIndex_V, ENUM(eSHADER::PS_TRANSPARENTPASS), "ps_TransparentPass.cso");
	_storeShader(m_ShaderIndex_V, ENUM(eSHADER::PS_COMPOSITE), "ps_Composite.cso");
	_storeShader(m_ShaderIndex_V, ENUM(eSHADER::PS_COLORTOACCUMEREVEAL), "ps_ColorToAccumeReveal.cso");

	_storeShader(m_ShaderIndex_V, ENUM(eSHADER::VS_SHADOWMAP), "vs_shadowMap.cso");
	_storeShader(m_ShaderIndex_V, ENUM(eSHADER::VS_SKINNEDSHADOW), "vs_skinnedShadow.cso");
	_storeShader(m_ShaderIndex_V, ENUM(eSHADER::PS_SHADOWMAP), "ps_shadowMap.cso");

	_storeShader(m_ShaderIndex_V, ENUM(eSHADER::VS_SSAO), "vs_SSAO.cso");
	_storeShader(m_ShaderIndex_V, ENUM(eSHADER::PS_SSAO), "ps_SSAO.cso");

	_storeShader(m_ShaderIndex_V, ENUM(eSHADER::PS_LIGHTTEXTURE), "ps_lightTextureMap.cso");
	_storeShader(m_ShaderIndex_V, ENUM(eSHADER::VS_STATICLIGHTTEXTURE), "vs_staticLightTexture.cso");

	_storeShader(m_ShaderIndex_V, ENUM(eSHADER::VS_NORMALR), "vs_normalR.cso");
	_storeShader(m_ShaderIndex_V, ENUM(eSHADER::VS_NORMALSKINNINGR), "vs_normalSkinningR.cso");
	_storeShader(m_ShaderIndex_V, ENUM(eSHADER::PS_NORMALR), "ps_normalR.cso");
	_storeShader(m_ShaderIndex_V, ENUM(eSHADER::PS_NORMALSKINNINGR), "ps_normalSkinningR.cso");
	_storeShader(m_ShaderIndex_V, ENUM(eSHADER::VS_OUTLINE), "vs_outLine.cso");
	_storeShader(m_ShaderIndex_V, ENUM(eSHADER::PS_OUTLINE), "ps_outLine.cso");

	_storeShader(m_ShaderIndex_V, ENUM(eSHADER::VS_2D), "vs_2d.cso");
	_storeShader(m_ShaderIndex_V, ENUM(eSHADER::PS_2D), "ps_2d.cso");
	_storeShader(m_ShaderIndex_V, ENUM(eSHADER::VS_BILLBOARD), "vs_Billboard.cso");
	_storeShader(m_ShaderIndex_V, ENUM(eSHADER::PS_BILLBOARD), "ps_Billboard.cso");

	_storeShader(m_ShaderIndex_V, ENUM(eSHADER::VS_IDPICKING), "vs_IDpicking.cso");
	_storeShader(m_ShaderIndex_V, ENUM(eSHADER::VS_IDPICKINGSKINNING), "vs_IDpickingSkinning.cso");
	_storeShader(m_ShaderIndex_V, ENUM(eSHADER::PS_IDPICKING), "ps_IDpicking.cso");

	_storeShader(m_ShaderIndex_V, ENUM(eSHADER::VS_SKYBOX), "vs_skyBox.cso");
	_storeShader(m_ShaderIndex_V, ENUM(eSHADER::PS_SKYBOX), "ps_skyBox.cso");
	_storeShader(m_ShaderIndex_V, ENUM(eSHADER::VS_COLOR), "vs_color.cso");
	_storeShader(m_ShaderIndex_V, ENUM(eSHADER::VS_COLORN), "vs_colorN.cso");
	_storeShader(m_ShaderIndex_V, ENUM(eSHADER::PS_COLOR), "ps_color.cso");
	_storeShader(m_ShaderIndex_V, ENUM(eSHADER::PS_COLORN), "ps_colorN.cso");

	_storeShader(m_ShaderIndex_V, ENUM(eSHADER::PS_DEFERREDDEBUG), "ps_deferredDebug.cso");
	_storeShader(m_ShaderIndex_V, ENUM(eSHADER::PS_DEFERREDDEBUGINFO), "ps_deferredDebugInfo.cso");
	_storeShader(m_ShaderIndex_V, ENUM(eSHADER::VS_STANDARD), "vs_Standard.cso");
	_storeShader(m_ShaderIndex_V, ENUM(eSHADER::PS_CUBEMAPSPHERE), "ps_cubeMapSphere.cso");

	m_ParticleManager = std::make_unique<class ParticleManager>();
	m_ParticleManager->Initialize(m_pDevice, m_pDeviceContext,
		reinterpret_cast<GeometryShader*>(m_ShaderIndex_V[ENUM(eSHADER::GS_STREAMOUT)]),
		reinterpret_cast<VertexShader*>(m_ShaderIndex_V[ENUM(eSHADER::VS_STREAMOUT)]));

	m_DeferredPixelData = std::make_unique<StagingBuffer<DeferredPixel>>();
	m_DeferredPixelData->Initialize<DeferredPixel>(m_pDevice.Get(), 1);

	m_pDeferredInfo = std::make_unique<struct DeferredInfo>();
	m_pDeferredInfo->postion = &m_DeferredPixelData->m_Data.position;
	m_pDeferredInfo->normal = &m_DeferredPixelData->m_Data.normal;
	m_pDeferredInfo->objectID = &m_DeferredPixelData->m_Data.objectID;

	m_DeferredPixelData->m_ComputeShader = std::make_unique<class ComputeShader>();
	m_DeferredPixelData->m_ComputeShader->Initialize(
		m_pDevice, L"../Data/Shader/cs_PxielPicking.hlsl", "DeferredTransparentPicking", "cs_5_0");

	m_SSAOManager = std::make_unique<class SSAO>();
	m_SSAOManager->Initialize(m_pDevice, m_pDeviceContext,
		m_pResourceManager->GetPixelShader("ps_SSAO.cso"),
		m_pResourceManager->GetVertexShader("vs_SSAO.cso"),
		m_pResourceManager->GetPixelShader("ps_SSAOBlur.cso"),
		m_pResourceManager->GetVertexShader("vs_SSAOBlur.cso"),
		m_ClientWidth, m_ClientHeight, 0.25 * DirectX::XM_PI, 1000.0f);


	m_pIBL->GetBasicIBL()->SetIrradianceMap(m_pResourceManager->GetIBLImage("BasicIrradianceMap"));
	m_pIBL->GetBasicIBL()->SetPreFilterMap(m_pResourceManager->GetIBLImage("BasicPreFilterMap"));

	CA_TRACE("기본 IBL 생성 완료");
}

void Renderer::CameraUpdate(
	const DirectX::XMMATRIX& worldTM,
	const DirectX::XMMATRIX& viewTM,
	const DirectX::XMMATRIX& projTM, float fovy, float farZ)
{
	m_CameraWorldTM = worldTM;

	m_CameraPos = m_CameraWorldTM.Translation();
	m_CameraLook = m_CameraWorldTM.Forward();

	m_ViewTM = viewTM;
	m_ProjectionTM = projTM;

	m_FrustumVolume->Update(m_CameraWorldTM, m_ProjectionTM, true);
	ConstantBufferManager::GetPSCamera()->data.CameraPos.x = m_CameraPos.x;
	ConstantBufferManager::GetPSCamera()->data.CameraPos.y = m_CameraPos.y;
	ConstantBufferManager::GetPSCamera()->data.CameraPos.z = m_CameraPos.z;

	static const DirectX::XMMATRIX T(
		0.5f, 0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.0f, 1.0f);

	ConstantBufferManager::GetPSCamera()->data.viewProjTex = m_ViewTM * m_ProjectionTM * T;
	ConstantBufferManager::GetPSCamera()->ApplyChanges();

	if (m_CameraFovY != fovy)
	{
		m_CameraFovY = fovy;
		m_SSAOManager->ReCalcCamera(m_ClientWidth, m_ClientHeight, m_CameraFovY, farZ);
	}
}

void Renderer::CameraSkyBoxRender(const unsigned int textureIdx)
{

}

void Renderer::BeginDraw()
{
	if (m_RenderOption.bDebugRenderMode = true)
	{
		float nullColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
		m_pDeviceContext->ClearRenderTargetView(m_pDebugRenderTarget->GetRenderTargerViewRawptr(), nullColor);
	}

	// Update에서 이전 정보를 가져가기 때문에 여기서 호출
	m_RenderDebugInfo.renderedObject = 0;
	m_RenderDebugInfo.culledObject = 0;

	// 디버그에 사용할 렌더러를 먼저 그려준다
	m_pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_pDeviceContext->RSSetState(RasterizerState::GetSolidNormal());
	m_pDeviceContext->OMSetDepthStencilState(RasterizerState::GetDepthStencilState(), 0);
	m_pDeviceContext->OMSetBlendState(RasterizerState::GetBlenderState(), NULL, 0xFFFFFFFF);
	m_pDeviceContext->PSSetSamplers(0, 1, RasterizerState::GetLinearSamplerStateAddressOf());
	m_pDeviceContext->RSSetViewports(1, &m_pRenderTarget->GetViewPort());

	switch (m_Rendering)
	{
	case eRENDERRING::DEFERRED:
	{
		m_Deferred->OpaqueBeginRender(m_pDeviceContext);
		break;
	}
	case eRENDERRING::FORWARD:
	default:
	{
		m_Forward->BeginRender(m_pDeviceContext);
		break;
	}
	}

	m_PostProcess->RenderOption(m_RenderOption);


	m_IsSelectedAny = false;
	m_NearestDist = 0.f;
}

void Renderer::RenderOptionUpdate(IRenderOption option)
{
	// 포인터로 받아와서 값만 복사한다
	m_RenderOption = option;
	m_RenderOption.isQT = false;
}

IRenderOption Renderer::GetRenderOption()
{
	return m_RenderOption;
}

IRendererDebugInfo Renderer::GetRenderDebugInfo()
{
	return m_RenderDebugInfo;
}

void* Renderer::GetDeferredInfo()
{
	return reinterpret_cast<void*>(m_pDeferredInfo.get());
}

void Renderer::ExecuteCommandLine()
{

	while (ContextInUse(TRUE) == TRUE)
		Sleep(0);

	if (m_pD3DCommandList != nullptr && m_FillCommandLine == true)
	{
		m_pDeviceContext->ExecuteCommandList(m_pD3DCommandList, FALSE);

		SAFE_RELEASE(m_pD3DCommandList);
		this->m_p1Swapchain->Present(0, NULL);
		m_FillCommandLine = false;
	}

	ContextInUse(FALSE);
}

BOOL Renderer::ContextInUse(BOOL isUse)
{
	return InterlockedExchange8(&m_ContextInUse, isUse);
}

bool Renderer::LightUpdate(
	const unsigned int type,
	const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT3& dir,
	const DirectX::XMFLOAT3& color, float intensity,
	float falloffStart, float falloffEnd, float spotPower,
	bool castShadow,
	bool isTextured, unsigned int textureIndex)
{
	// Type이 Directional Light인 경우에는 거리 상관없이 처리해야한다. 
	// 그것이 DirectionalLight니까.(끄덕)

	float _nowDistance = DirectX::SimpleMath::Vector3::Distance(m_CameraPos, pos);

	// 거리 값에 대한 기준을 잡아야할 듯
	// 일단은 한 10?
	if (_nowDistance <= 20.0f || type == DIRECTONAL_LIGHT)
	{
		m_LightManager->AddLightCount();									// 사용할 라이트의 총 개수 추가
		m_LightManager->ClearShadowDepthStencil(m_pDeviceContext.Get());	// 사용할 라이트의 개수에 대해 깊이 버퍼 초기화
		m_LightManager->LightUpdate(type, pos, dir, color, intensity, falloffStart, falloffEnd, spotPower, castShadow, isTextured, textureIndex);

		return true;
	}

	return false;
}

void Renderer::BeginShadow()
{
	// 빛에 대한 깊이 버퍼를 만들기 위한 사전 작업
	m_LightManager->m_NowMappingMode = eMAP_MODE::SHADOW;
	m_LightManager->BindShadowMap(m_pDeviceContext.Get());
}

void Renderer::LightTextureMap(unsigned int textureIndex)
{
	// 라이트 텍스쳐를 위한 렌더 타겟 묶기 사전 작업
	m_LightManager->m_NowMappingMode = eMAP_MODE::TEXTURE;
	m_LightManager->BindTexture(m_pDeviceContext.Get());
}

void Renderer::PreRenderPath(const unsigned int modelIndex, const DirectX::SimpleMath::Matrix& worldTM)
{
	std::shared_ptr<ModelMesh> _nowModel = m_pResourceManager->GetModelMesh(modelIndex);
	std::shared_ptr<LightClass> _nowLight = m_LightManager->GetLightPtr();

	// 원래 SetBoneOffSet을 여기서 해주면 안될 것 같은데 미리 해줌
	// 나중에 FixedUpdate 등이 추가되었을 때 그쪽으로 바꿔주자

	switch (m_LightManager->m_NowMappingMode)
	{
	case eMAP_MODE::TEXTURE:
	{
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> _nowTexture =
			m_pResourceManager->GetTexture(_nowLight->GetTextureIndex());

		std::shared_ptr<PixelShader> _nowPixel =
			m_pResourceManager->GetPixelShader("ps_lightTextureMap.cso");

		std::shared_ptr<VertexShader> _nowVertex;
		if (_nowModel->m_pSkeleton != nullptr)
		{
			// 아직 스킨드에 대한 쉐이더 없음
			_nowVertex = m_pResourceManager->GetVertexShader("vs_skinnedLightTexture.cso");
			m_LightManager->BuildTextureMap(_nowLight,
				_nowModel, worldTM, m_ViewTM, m_ProjectionTM,
				m_pDeviceContext, _nowTexture, _nowVertex, _nowPixel, true);
		}
		else
		{
			_nowVertex = m_pResourceManager->GetVertexShader("vs_staticLightTexture.cso");
			m_LightManager->BuildTextureMap(_nowLight,
				_nowModel, worldTM, m_ViewTM, m_ProjectionTM,
				m_pDeviceContext, _nowTexture, _nowVertex, _nowPixel, false);
		}

		break;
	}
	case eMAP_MODE::SHADOW:
	default:
	{
		SetPixelShader(PS_SHADER(eSHADER::PS_SHADOWMAP));

		if (_nowModel->m_pSkeleton != nullptr)
		{
			SetVertexShader(VS_SHADER(eSHADER::VS_SKINNEDSHADOW));
			m_LightManager->BuildShadowMap(_nowLight,
				_nowModel, worldTM, m_pDeviceContext, true);
		}
		else
		{

			for (auto& _nowNode : *m_pResourceManager->GetModelMesh(modelIndex)->m_pNodeData_V)
			{
				_nowNode->CalculateTransform(worldTM);
				_nowNode->m_BoundingVolume->Update(_nowNode->animationTM);

				/// 프러스텀과 바운딩 박스가 intersect되는 원리 알아오기
				if (m_FrustumVolume->GetFrustum().Intersects(_nowNode->m_BoundingVolume->GetBoundingSphere()) == true)
				{
					SetVertexShader(VS_SHADER(eSHADER::VS_SHADOWMAP));
					m_LightManager->BuildShadowMap(_nowLight,
						_nowModel, worldTM, m_pDeviceContext, false);
				}
			}
		}

		break;
	}
	}
}

unsigned int Renderer::AddReflectionProbe()
{
	return m_pIBL->AddReflectionProbe(m_pDevice);
}

void Renderer::ReflectionProbeSetWorldTM(const unsigned int probeIndex,
	const DirectX::SimpleMath::Matrix& probeWorldTM)
{
	// 프로브의 월드 위치를 갱신해준다.
	// 프로브 자체를 디버깅할 때 필요한 요소
	m_pIBL->GetReflectionProbe(probeIndex)->SetProbeWorldTM(probeWorldTM);
}

void Renderer::ReflectionProbeRenderTargetViewBind(const unsigned int probeIndex, const unsigned int faceIndex)
{
	// 바깥에서 6회 포문 돌면서 지정된 프로브의 6면을 차례로 바인딩 - 그리기 한다
	m_pIBL->ReflectionProbeRenderTargetViewBind(m_pDeviceContext, probeIndex, faceIndex, m_pDepthStencil->GetDepthSetncilView());
}

void Renderer::RenderToReflectionProbe(
	const unsigned int modelIndex,
	const DirectX::SimpleMath::Matrix& worldTM,
	const DirectX::SimpleMath::Matrix& viewTM,
	const DirectX::SimpleMath::Matrix& projTM)
{
	// ReflectionProbeRenderTargetViewBind 에서 렌더 타겟이 묶였으므로 포워드 방식으로 그림을 그린다
	// 어차피 실시간 렌더링이 아니므로 조명을 포워드로 처리해도 문제는 없다
	// 애니메이션이 없는 스태틱 오브젝트들만 베이킹 될 예정이므로 애니메이션이 고려된 쉐이더는 필요없다
	UINT offset = 0;

	std::shared_ptr<ModelMesh> _nowModelMesh = m_pResourceManager->GetModelMesh(modelIndex);

	//std::shared_ptr<VertexShader> _vertex = m_pResourceManager->GetVertexShader("vs_test.cso");
	//std::shared_ptr<PixelShader> _pixel = m_pResourceManager->GetPixelShader("ps_reflectionProbe.cso");

	m_pDeviceContext->IASetInputLayout(VS_SHADER(eSHADER::VS_STATICMESH)->inputLayout.Get());
	m_pDeviceContext->VSSetShader(VS_SHADER(eSHADER::VS_STATICMESH)->GetVertexShader(), NULL, 0);
	m_pDeviceContext->PSSetShader(PS_SHADER(eSHADER::PS_REFLECTIONPROBE)->GetPixelShader(), NULL, 0);

	for (auto& _nowNode : *_nowModelMesh->m_pNodeData_V)
	{
		m_pDeviceContext->IASetVertexBuffers(0, 1, _nowNode->GetVertexBuffer().GetAddressOf(), _nowNode->StridePtr(), &offset);
		m_pDeviceContext->IASetIndexBuffer(_nowNode->GetIndexBuffer().Get(), DXGI_FORMAT::DXGI_FORMAT_R32_UINT, 0);

		_nowNode->CalculateTransform(worldTM);
		ConstantBufferManager::GetVSPerObjectBuffer()->data.WorldViewProjection = _nowNode->animationTM * viewTM * projTM;
		ConstantBufferManager::GetVSPerObjectBuffer()->data.World = _nowNode->animationTM;
		ConstantBufferManager::GetVSPerObjectBuffer()->ApplyChanges();

		m_pDeviceContext->VSSetConstantBuffers(0, 1, ConstantBufferManager::GetVSPerObjectBuffer()->GetAddressOf());

		ReflectionProbeSetMaterial(_nowNode);

		// 그림자를 넣을 때 모델의 world값도 같이 곱해서 넣어줘야한다
		// 그림자 캐스팅을 하는 라이트가 없으면 안돌아야함
		for (unsigned int i = 0; i < m_LightManager->GetShadowLightCount(); i++)
		{
			ConstantBufferManager::GetLightMatrixBuffer()->data.lightMatrix[i] =
				_nowNode->animationTM * m_LightManager->GetShadowLightPtr(i)->GetShadowTM();
		}
		ConstantBufferManager::GetLightMatrixBuffer()->ApplyChanges();
		m_pDeviceContext->VSSetConstantBuffers(1, 1, ConstantBufferManager::GetLightMatrixBuffer()->GetAddressOf());

		// 아직 조명에 대한 고려가 안되어있다
		// LightCount Constatnt 버퍼를 밀어넣을 수 있어야한다

		m_pDeviceContext->DrawIndexed(_nowNode->m_IndexCount, 0, 0);
	}

	ID3D11Buffer* _bf[2] = { NULL };
	m_pDeviceContext->VSSetConstantBuffers(0, 2, _bf);
}

void Renderer::ReflectionProbeBaking(const unsigned int sceneIndex, const std::string& sceneName, const unsigned int probeIndex)
{
	m_pIBL->BakeReflectionProbe(
		m_pDevice, m_pDeviceContext,
		m_pResourceManager->GetVertexShader("vs_Environment.cso"),
		m_pResourceManager->GetPixelShader("ps_irradianceMapVector3.cso"),
		m_pResourceManager->GetPixelShader("ps_prefilter.cso"),
		sceneIndex, sceneName, probeIndex);
}

void Renderer::SetReflectionBakedDDS(
	unsigned int probeIndex, unsigned int envMap, unsigned int irrMap, unsigned int filterMap)
{
	if (m_pIBL->IsProbeExist() == true)
	{
		m_pIBL->GetReflectionProbe(probeIndex)->SetBakedMapIndex(envMap, irrMap, filterMap);
	}
	else return;
}

void Renderer::BindPose(const unsigned int modelIndex,
	const DirectX::SimpleMath::Matrix& worldTM)
{
	std::shared_ptr<ModelMesh> _nowModel = m_pResourceManager->GetModelMesh(modelIndex);

	if (_nowModel->m_pSkeleton != nullptr)
	{
		_nowModel->m_pSkeleton->BindPose(worldTM);
		_nowModel->CalculateBoneOffsetTM();
	}
}


void Renderer::MeshDraw(
	const unsigned int modelIndex,
	const unsigned int probeIndex,
	const unsigned int renderFlag,
	const unsigned int ObjectId,
	const DirectX::XMMATRIX& worldTM,
	DirectX::XMMATRIX* pBoneOffsetTM)
{

	if (renderFlag & 1)
	{
		m_IsSelectedAny = true;
	}

	std::shared_ptr<ModelMesh> _nowModelMesh = m_pResourceManager->GetModelMesh(modelIndex);

	// 리플렉션 프로브를 담는다, 반사하지 않으면 매직넘버를
	PixelShader* _nowPixelShader = nullptr;
	unsigned int _nowReflectionProbe = 9999;
	if (renderFlag & 2)
	{
		_nowReflectionProbe = probeIndex;
		_nowPixelShader = PS_SHADER(eSHADER::PS_IBLDEFERRED);
	}
	else
	{
		_nowPixelShader = PS_SHADER(eSHADER::PS_NONE_IBLDEFERRED);
	}

	// 본 오프셋
	if (_nowModelMesh->m_pSkeleton != nullptr)
	{
		auto _parent = _nowModelMesh->m_pNodeData_V->at(0);
		_parent->m_BoundingVolume->Update(worldTM);

		if (m_FrustumVolume->GetFrustum().Intersects(_parent->m_BoundingVolume->GetBoundingSphere()) == true)
		{
			for (auto& _nowNode : *_nowModelMesh->m_pNodeData_V)
			{
				{
					m_RenderDebugInfo.renderedObject++;

					m_RenderQueue->AddRenderQueue(
						VS_SHADER(eSHADER::VS_SKINNEDMESH),
						_nowPixelShader,
						_nowReflectionProbe, ObjectId, modelIndex,
						_nowNode->pMaterial->isTransparency,
						_nowNode, worldTM, pBoneOffsetTM);

					// 하나라도 선택된게있으면 m_isSelectedAny가 true
					if (renderFlag & 1)
					{
						/// 외곽선 그리기
						// 노멀을 렌더타겟에 그리기위한 재료를 저장 
						m_RenderQueue->CopyOutLineMesh(
							VS_SHADER(eSHADER::VS_NORMALSKINNINGR),
							PS_SHADER(eSHADER::PS_NORMALSKINNINGR),
							modelIndex, _nowNode, worldTM, pBoneOffsetTM);
					}

				}
			}
		}
		else
			m_RenderDebugInfo.culledObject++;

	}
	else // 매쉬만 있는경우
	{

		for (auto& _nowNode : *_nowModelMesh->m_pNodeData_V)
		{

			_nowNode->CalculateTransform(worldTM);
			_nowNode->m_BoundingVolume->Update(_nowNode->animationTM);

			if (m_FrustumVolume->GetFrustum().Intersects(_nowNode->m_BoundingVolume->GetBoundingSphere()) == true)
			{
				m_RenderDebugInfo.renderedObject++;

				m_RenderQueue->AddRenderQueue(
					VS_SHADER(eSHADER::VS_STATICMESH),
					_nowPixelShader,
					_nowReflectionProbe, ObjectId, -1, _nowNode->pMaterial->isTransparency,
					_nowNode, _nowNode->animationTM, pBoneOffsetTM);

				// 하나라도 선택된게있으면 m_isSelectedAny가 true
				if (renderFlag & 1)
				{
					/// 외곽선 그리기
					// 노멀을 렌더타겟에 그리기위한 재료를 저장 
					m_RenderQueue->CopyOutLineMesh(
						VS_SHADER(eSHADER::VS_NORMALR),
						PS_SHADER(eSHADER::PS_NORMALR),
						modelIndex, _nowNode, _nowNode->animationTM, pBoneOffsetTM);
				}
			}
			else
			{
				m_RenderDebugInfo.culledObject++;
			}
		}
	}
}

void Renderer::RenderQueueProcess()
{
	unsigned int _opaqueSize, _transparentSize, _particleSize;

	m_RenderQueue->GetObjectCount(_opaqueSize, _transparentSize, _particleSize);
	m_Deferred->RenderTargetClear(m_pDeviceContext);

	m_pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_pDeviceContext->RSSetState(RasterizerState::GetSolidNormal());

	m_pDeviceContext->OMSetDepthStencilState(RasterizerState::GetDepthStencilState(), 1);
	m_pDeviceContext->OMSetBlendState(RasterizerState::GetDeferredBlendState(), nullptr, 0xffffffff);

	CommonConstantBuffer();

	// Opaque 처리
	if (_opaqueSize > 0)
	{
		// Deferred 처리를 먼저 진행한다

		if (m_RenderOption.isAdaptation == true)
		{
			m_RenderQueue->ProcessQueueByInstance(this);
		}
		else
		{
			m_RenderQueue->ProcessOpaqueQueue(this);
		}
	}

	m_SSAOManager->ComputeSsao(m_pDeviceContext, m_ProjectionTM,
		m_Deferred->GetDrawLayers()[NORMALDEPTH_MAP]->GetShaderResourceViewRawptr());
	m_SSAOManager->BlurAmbientMap(m_pDeviceContext, 4);

	// Opaque가 있긴 없건 무조건 돌아간다
	// Deferred 버퍼들을 하나로 뭉쳐서 IBL 연산이 끝난 FrameBuffer로 만든다
	{
		float colorNull[4] = { 0.3f,0.3f, 0.3f, 0.0f };
		m_pDeviceContext->ClearRenderTargetView(m_pRenderTarget->GetRenderTargerViewRawptr(), colorNull);
		m_pDeviceContext->OMSetRenderTargets(1, m_pRenderTarget->GetRenderTargetViewAddressOf(), nullptr);	// 여기서는 뎁스 버퍼가 없는게 맞다
																											// 실제 메쉬가 있을 때 체크를 하는 것이니
		m_pDeviceContext->RSSetViewports(1, &m_pRenderTarget->GetViewPort());


		m_Deferred->CombineRenderTargetOpaque(
			true,
			m_pDeviceContext,
			VS_SHADER(eSHADER::VS_COMBINE),
			PS_SHADER(eSHADER::PS_OPAQUEPASS),
			m_LightManager->GetTextureLightShaderResourceView(0),
			m_SSAOManager->AmbientSRV().Get());

		m_Deferred->UnbindRenderTargets(m_pDeviceContext);

	}

	// Transparent 처리
	if (_transparentSize > 0)
	{
		m_Deferred->TransparentBeginRender(m_pDeviceContext);

		m_RenderQueue->ProcessTransparentQueue(this);

		m_Deferred->CombineRenderTargetTransparent(
			true,
			m_pDeviceContext,
			VS_SHADER(eSHADER::VS_COMBINE),
			PS_SHADER(eSHADER::PS_TRANSPARENTPASS),
			m_LightManager->GetTextureLightShaderResourceView(0));

		m_Deferred->UnbindRenderTargets(m_pDeviceContext);
	}

	{
		m_Deferred->BindAccumReveal(m_pDeviceContext);
		m_ParticleManager->UpdateParticle(m_RenderOption.dTime);
		m_ParticleManager->DrawParticle(
			m_pDeviceContext,
			VS_SHADER(eSHADER::VS_STREAMOUT),
			GS_SHADER(eSHADER::GS_STREAMOUT),
			VS_SHADER(eSHADER::VS_DRAWPASS),
			GS_SHADER(eSHADER::GS_DRAWPASS),
			PS_SHADER(eSHADER::PS_PARTICLEDRAW),
			m_CameraPos, m_ViewTM, m_ProjectionTM);
	}

	{
		// Transparent object가 뭐라도 하나가 그려지고 있다면 
		m_Deferred->CompositeFrameBuffers(
			m_pDeviceContext,
			m_pRenderTarget,
			VS_SHADER(eSHADER::VS_COMBINE),
			PS_SHADER(eSHADER::PS_COMPOSITE));
	}

	m_RenderDebugInfo.opaqueObejct = _opaqueSize;
	m_RenderDebugInfo.transparentObejct = _transparentSize;
	m_RenderDebugInfo.particleObject = _particleSize;
	m_RenderQueue->Clear();
}

void Renderer::Clicked(bool isClicked, POINT mousePos)
{
	m_isClicked = isClicked;
	m_MousePos = mousePos;
}

unsigned int Renderer::GetClickedObjectId()
{
	return m_ClickedObjectId;
}

unsigned int Renderer::SpawnParticle(struct ParticleProperty* particle)
{
	return m_ParticleManager->AddParticle(m_pDeviceContext,
		m_pResourceManager->GetTexture(particle->textureIndex).Get(),
		particle);
}

#pragma region RenderFuncByJinmi

/// <summary>
/// UI용 스프라이트(+ 버튼, 애니메이션) 렌더링
/// </summary>
/// <param name="resourceType">이미지 타입(단일이미지, 한장의 애니메이션 이미지, 여러장의 애니메이션 이미지)</param>
/// <param name="spriteIndex">리소스 매니저에 요청할 이미지의 인덱스</param>
/// <param name="worldTM">스프라이트의 위치, 크기, 회전값 정보를 가지고있는 월드 변환 행렬</param>
void Renderer::DrawSprite(eResourceType resourceType, unsigned int spriteIndex, const DirectX::XMMATRIX& worldTM)
{
	// 이미지 테두리
	//DrawSpriteEdge(worldTM);

	float blendFactor[] = { 0, 0, 0, 0 };

	m_pDeviceContext->RSSetState(RasterizerState::GetSolidNormal());
	//m_pDeviceContext->OMSetDepthStencilState(RasterizerState::GetDepthStencilState(), 1);
	m_pDeviceContext->OMSetBlendState(RasterizerState::GetAlphaBlenderStateUI(), blendFactor, 0xffffffff);

	m_pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	std::shared_ptr<SpriteData> _nowSprite = m_pResourceManager->GetSpriteData(spriteIndex);

	m_pDeviceContext->IASetInputLayout(VS_SHADER(eSHADER::VS_2D)->inputLayout.Get());
	m_pDeviceContext->VSSetShader(VS_SHADER(eSHADER::VS_2D)->GetVertexShader(), NULL, 0);
	m_pDeviceContext->PSSetShader(PS_SHADER(eSHADER::PS_2D)->GetPixelShader(), NULL, 0);


	m_OrthoTM = DirectX::XMMatrixOrthographicOffCenterLH(0.0f, (float)m_ClientWidth, (float)m_ClientHeight, 0.0f, 0.0f, 1.0f);
	XMMATRIX wvpMatrix = worldTM * m_OrthoTM;

	m_pDeviceContext->VSSetConstantBuffers(0, 1, ConstantBufferManager::GetVertexBuffer2D()->GetAddressOf());
	ConstantBufferManager::GetVertexBuffer2D()->data.wvpMatrix = wvpMatrix;
	ConstantBufferManager::GetVertexBuffer2D()->ApplyChanges();

	switch (resourceType)
	{
	case eResourceType::eSingleImage:
	case eResourceType::eSeveralAni:
		m_pDeviceContext->PSSetShaderResources(0, 1, _nowSprite->shaderResource.GetAddressOf());
		break;
	case eResourceType::eSingleAni:
		//uv값을 바꿔서 이미지 출력하게 할거다
		break;
	default:
		break;
	}

	const UINT offsets = 0;

	m_pDeviceContext->IASetVertexBuffers(0, 1, m_SpriteQuad->GetVertexAddressOf(), m_SpriteQuad->StridePtr(), &offsets);
	m_pDeviceContext->IASetIndexBuffer(m_SpriteQuad->GetIndexBuffer(), DXGI_FORMAT::DXGI_FORMAT_R32_UINT, 0);
	m_pDeviceContext->DrawIndexed(m_SpriteQuad->GetIndexCount(), 0, 0);

	m_pDeviceContext->OMSetBlendState(RasterizerState::GetAlphaBlenderState(), nullptr, 0xffffffff);

}

/// <summary>
/// 로딩씬 띄울때 쓰는 함수
/// </summary>
/// <param name="_tempBitmap"></param>
/// <param name="worldTM"></param>
/// <param name="vertexShader"></param>
/// <param name="pixelShader"></param>
/// <param name="inputLayout"></param>
void Renderer::DrawSprite(
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> bitmap, 
	const DirectX::XMMATRIX& worldTM, 
	VertexShader* vertexShader,
	PixelShader* pixelShader)
{
	float blendFactor[] = { 0, 0, 0, 0 };
	m_pDeferredDeviceContext->RSSetState(RasterizerState::GetNoCullingRS());
	m_pDeferredDeviceContext->OMSetBlendState(RasterizerState::GetAlphaBlenderStateUI(), blendFactor, 0xffffffff);
	
	m_pDeferredDeviceContext->PSSetSamplers(0, 1, RasterizerState::GetLinearSamplerStateAddressOf());

	m_pDeferredDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	m_pDeferredDeviceContext->IASetInputLayout(vertexShader->inputLayout.Get());
	m_pDeferredDeviceContext->VSSetShader(vertexShader->GetVertexShader(), NULL, 0);
	m_pDeferredDeviceContext->PSSetShader(pixelShader->GetPixelShader(), NULL, 0);

	m_OrthoTM = DirectX::XMMatrixOrthographicOffCenterLH(0.0f, (float)m_ClientWidth, (float)m_ClientHeight, 0.0f, 0.0f, 1.0f);
	XMMATRIX wvpMatrix = worldTM * m_OrthoTM;

	ConstantBufferManager::GetVertexBuffer2D()->data.wvpMatrix = wvpMatrix;
	ConstantBufferManager::GetVertexBuffer2D()->ApplyChanges(m_pDeferredDeviceContext.Get());

	m_pDeferredDeviceContext->VSSetConstantBuffers(0, 1, ConstantBufferManager::GetVertexBuffer2D()->GetAddressOf());
	m_pDeferredDeviceContext->PSSetShaderResources(0, 1, bitmap.GetAddressOf());

	const UINT offsets = 0;

	m_pDeferredDeviceContext->IASetVertexBuffers(0, 1, m_SpriteQuad->GetVertexAddressOf(), m_SpriteQuad->StridePtr(), &offsets);
	m_pDeferredDeviceContext->IASetIndexBuffer(m_SpriteQuad->GetIndexBuffer(), DXGI_FORMAT::DXGI_FORMAT_R32_UINT, 0);
	m_pDeferredDeviceContext->DrawIndexed(m_SpriteQuad->GetIndexCount(), 0, 0);

}

void Renderer::DrawSpriteEdge(const DirectX::XMMATRIX& worldTM)
{
	m_pDeviceContext->RSSetState(RasterizerState::GetWireFrameRS());

	m_pDeviceContext->IASetInputLayout(m_pResourceManager->GetVertexShader("vs_ColorDebug.cso")->inputLayout.Get());
	m_pDeviceContext->VSSetShader(m_pResourceManager->GetVertexShader("vs_ColorDebug.cso")->GetVertexShader(), NULL, 0);
	m_pDeviceContext->PSSetShader(m_pResourceManager->GetPixelShader("ps_color.cso")->GetPixelShader(), NULL, 0);

	m_OrthoTM = DirectX::XMMatrixOrthographicOffCenterLH(0.0f, (float)m_ClientWidth, (float)m_ClientHeight, 0.0f, 0.0f, 1.0f);
	XMMATRIX wvpMatrix = worldTM * m_OrthoTM;

	m_pDeviceContext->VSSetConstantBuffers(0, 1, ConstantBufferManager::GetVertexBuffer()->GetAddressOf());
	ConstantBufferManager::GetVertexBuffer()->data.wvpMatrix = wvpMatrix;
	ConstantBufferManager::GetVertexBuffer()->data.worldMatrix = worldTM;
	ConstantBufferManager::GetVertexBuffer()->ApplyChanges();

	const UINT offsets = 0;

	m_pDeviceContext->IASetVertexBuffers(0, 1, m_SpriteQuad->GetVertexEdgeAddressOf(), m_SpriteQuad->StrideEdgePtr(), &offsets);
	m_pDeviceContext->IASetIndexBuffer(m_SpriteQuad->GetIndexBuffer(), DXGI_FORMAT::DXGI_FORMAT_R32_UINT, 0);
	m_pDeviceContext->DrawIndexed(m_SpriteQuad->GetIndexCount(), 0, 0);
}

/// <summary>
/// 빌보드 렌더링
/// </summary>
/// <param name="gizmoOn">기즈모 on/off</param>
/// <param name="spriteIndex">리소스 매니저에 요청할 이미지의 인덱스</param>
/// <param name="worldTM">스프라이트의 위치, 크기, 회전값 정보를 가지고있는 월드 변환 행렬</param>
/// <param name="viewTM">카메라 뷰 행렬</param>
/// <param name="projTM">카메라 투영 행렬</param>
void Renderer::DrawBillboard(const unsigned int spriteIndex, const DirectX::XMMATRIX& worldTM)
{
	std::shared_ptr<SpriteData> _nowSprite = m_pResourceManager->GetSpriteData(spriteIndex);
	m_RenderQueue->AddRenderQueueBillboard(
		VS_SHADER(eSHADER::VS_BILLBOARD),
		PS_SHADER(eSHADER::PS_BILLBOARD),
		spriteIndex,
		_nowSprite,
		worldTM);
}

void Renderer::DrawBillboardUI()
{
	unsigned int _opaqueSize, _transparentSize, _particleSize;

	m_RenderQueue->GetObjectCount(_opaqueSize, _transparentSize, _particleSize);

	if (_particleSize > 0)
	{
		m_Deferred->BindAccumReveal(m_pDeviceContext);
		m_RenderQueue->ProcessBillboardQueue(this);
	}

	m_RenderQueue->m_NowParticleIndex = 0;
}

void Renderer::DrawBillboardEdge(const DirectX::XMMATRIX& worldTM)
{
	m_pDeviceContext->RSSetState(RasterizerState::GetWireFrameRS());

	m_pDeviceContext->IASetInputLayout(m_pResourceManager->GetVertexShader("vs_color.cso")->inputLayout.Get());
	m_pDeviceContext->VSSetShader(m_pResourceManager->GetVertexShader("vs_color.cso")->GetVertexShader(), NULL, 0);
	m_pDeviceContext->PSSetShader(m_pResourceManager->GetPixelShader("ps_color.cso")->GetPixelShader(), NULL, 0);

	XMMATRIX wvpMatrix = worldTM * m_ViewTM * m_ProjectionTM;

	m_pDeviceContext->VSSetConstantBuffers(0, 1, ConstantBufferManager::GetVertexBuffer()->GetAddressOf());
	ConstantBufferManager::GetVertexBuffer()->data.wvpMatrix = wvpMatrix;
	ConstantBufferManager::GetVertexBuffer()->data.worldMatrix = worldTM;
	ConstantBufferManager::GetVertexBuffer()->ApplyChanges();

	const UINT offsets = 0;

	m_pDeviceContext->IASetVertexBuffers(0, 1, m_SpriteQuad->GetVertexEdgeAddressOf(), m_SpriteQuad->StrideEdgePtr(), &offsets);
	m_pDeviceContext->IASetIndexBuffer(m_SpriteQuad->GetIndexBuffer(), DXGI_FORMAT::DXGI_FORMAT_R32_UINT, 0);
	m_pDeviceContext->DrawIndexed(m_SpriteQuad->GetIndexCount(), 0, 0);
}

unsigned int Renderer::FontInfoInitialize(std::shared_ptr<Text_Queue_Info> pFontDesc)
{
	return m_pD2D->FontInitialize(pFontDesc);
}

void Renderer::OneResizeTextPos(unsigned int fontinfoIndex)
{
	//m_pD2D->
}

void Renderer::DrawD2DText(std::shared_ptr<TextBlock> pTextBlock)
{
	m_pD2D->Push_DrawText(pTextBlock);
}

void Renderer::SetLoadingSceneImage(
	const std::string& imagePath, 
	const std::string& vertexShaderPath, 
	const std::string& pixelShaderPath, 
	float frame)
{
	m_LoadingScene->SetImageAndFps(m_pDevice, imagePath, vertexShaderPath, pixelShaderPath, frame);
}

void Renderer::DrawLoadingScene()
{
	while (ContextInUse(TRUE) == TRUE)
		Sleep(0);

	float _color[4] = { 0.0f, 0.0f,  0.0f, 0.0f };
	ID3D11RenderTargetView* _rt[1] = { m_Forward->GetRenderTargetView()->GetRenderTargerViewRawptr() };
	m_pDeferredDeviceContext->OMSetRenderTargets(1, _rt, nullptr);
	m_pDeferredDeviceContext->ClearRenderTargetView(m_Forward->GetRenderTargetView()->GetRenderTargerViewRawptr(), _color);
	m_pDeferredDeviceContext->RSSetViewports(1, &m_Forward->GetRenderTargetView()->GetViewPort());

	m_LoadingScene->Draw(this);

	m_pDeferredDeviceContext->FinishCommandList(FALSE, &m_pD3DCommandList);
	m_FillCommandLine = true;

	ContextInUse(FALSE);
}

#pragma endregion

void Renderer::SetMaterial(std::shared_ptr<NodeData>& pNodeData)
{
	for (unsigned int _textureIndex = 0; _textureIndex < 4; _textureIndex++)
	{
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> _nowView = nullptr;

		int _texidx = pNodeData->pMaterial->textureArray[_textureIndex];

		_nowView = m_pResourceManager->GetTexture(_texidx);

		if (_textureIndex == DIFFUSE_TEXUTRE && _nowView != nullptr)
		{
			ConstantBufferManager::GetPSMaterial()->data.DiffuseAlbedo = { 1.0f, 1.0f , 1.0f, 1.0f };
		}
		else if (_textureIndex == DIFFUSE_TEXUTRE && _nowView == nullptr)
		{
			ConstantBufferManager::GetPSMaterial()->data.DiffuseAlbedo = pNodeData->pMaterial->albedoColor;
		}

		m_pDeviceContext->PSSetShaderResources(_textureIndex, 1, _nowView.GetAddressOf());
	}

	DirectX::SimpleMath::Vector3 _lower = { 0.3f, 0.6f, 0.3f };
	DirectX::SimpleMath::Vector3 _upper = { 0.3f, 0.3f, 0.6f };

	// 앰비언트를 매 오브젝트마다 넣어줄 필요가 없다, 씬 별로 가지고 있는 것이므로 단순
	//ConstantBufferManager::GetPSMaterial()->data.LowerAmbinet = LinearToGamma(_lower);
	//ConstantBufferManager::GetPSMaterial()->data.RangeAmbinet = LinearToGamma(_upper) - LinearToGamma(_lower);

	ConstantBufferManager::GetPSMaterial()->data.FresnelR0 = pNodeData->pMaterial->fresnelR0;
	ConstantBufferManager::GetPSMaterial()->data.Shininess = pNodeData->pMaterial->smoothness;
	ConstantBufferManager::GetPSMaterial()->data.metallic = pNodeData->pMaterial->metallic;
	ConstantBufferManager::GetPSMaterial()->data.normalFactor = pNodeData->pMaterial->normalFactor;
	ConstantBufferManager::GetPSMaterial()->data.emissiveFactor = pNodeData->pMaterial->emissiveFactor;

	ConstantBufferManager::GetPSMaterial()->ApplyChanges();
	m_pDeviceContext->PSSetConstantBuffers(0, 1, ConstantBufferManager::GetPSMaterial()->GetAddressOf());

	// 이부분을 매 오브젝트의 머트리얼을 갱신할 때마다 해줄 필요가 전혀 없다
	ConstantBufferManager::GetLightMatrixBuffer()->ApplyChanges();
	m_pDeviceContext->PSSetConstantBuffers(1, 1, ConstantBufferManager::GetLightMatrixBuffer()->GetAddressOf());

	m_pDeviceContext->PSSetSamplers(0, 1, RasterizerState::GetLinearSamplerStateAddressOf());
	m_pDeviceContext->PSSetSamplers(1, 1, RasterizerState::GetPCFSamplerStateAddressOf());

	ID3D11ShaderResourceView* _arrShadow[MAX_SHADOW] =
	{
		m_LightManager->GetShadowLightShaderResourceView(0),
		m_LightManager->GetShadowLightShaderResourceView(1),
		m_LightManager->GetShadowLightShaderResourceView(2),
		m_LightManager->GetShadowLightShaderResourceView(3)
	};

	m_pDeviceContext->PSSetShaderResources(4, m_LightManager->GetShadowLightCount(), _arrShadow);

}

void Renderer::SetBasicIrradiance()
{
	float _iblFactor[1] = { m_RenderOption.adjustFactor };

	ConstantBufferManager::SetGeneralCBuffer(m_pDeviceContext, CBufferType::PS, CBufferSize::SIZE_4, 4, _iblFactor);

	ID3D11ShaderResourceView* _SRV[3] =
	{
		m_pResourceManager->GetTexture("ibl_brdf_lut.png").Get(),
		m_pIBL->GetBasicIBL()->GetIrradianceTexture().Get(),
		m_pIBL->GetBasicIBL()->GetPreFilterTexture().Get(),
		//m_pIBL->GetBasicIBL()->GetIrrdaianceCubeRenderTarget()->GetShaderResourceView().Get(),
		//m_pIBL->GetBasicIBL()->GetPreFilterCubeRenderTarget()->GetShaderResourceView().Get(),
	};

	m_pDeviceContext->PSSetShaderResources(8, 3, _SRV);
}

void Renderer::SetReflectionProbeBuffer(int probeIndex)
{
	ID3D11ShaderResourceView* _SRV[3] =
	{
		m_pResourceManager->GetTexture("ibl_brdf_lut.png").Get(),
		m_pResourceManager->GetIBLImage(m_pIBL->GetReflectionProbe(probeIndex)->GetIrradianceTextureIndex()).Get(),
		m_pResourceManager->GetIBLImage(m_pIBL->GetReflectionProbe(probeIndex)->GetPreFilterTextureIndex()).Get(),
	};

	// 거리체크가 끝난 리플렉션 프로브
	m_pDeviceContext->PSSetShaderResources(8, 3, _SRV);
}

void Renderer::ReflectionProbeSetMaterial(std::shared_ptr<NodeData> pNodeData)
{
	for (unsigned int _textureIndex = 0; _textureIndex < 4; _textureIndex++)
	{
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> _nowView = nullptr;

		int _texidx = pNodeData->pMaterial->textureArray[_textureIndex];

		_nowView = m_pResourceManager->GetTexture(_texidx);

		if (_textureIndex == DIFFUSE_TEXUTRE && _nowView != nullptr)
		{
			ConstantBufferManager::GetPSMaterial()->data.DiffuseAlbedo = { 1.0f, 1.0f , 1.0f, 1.0f };
		}
		else if (_textureIndex == DIFFUSE_TEXUTRE && _nowView == nullptr)
		{
			ConstantBufferManager::GetPSMaterial()->data.DiffuseAlbedo = pNodeData->pMaterial->albedoColor;
		}

		m_pDeviceContext->PSSetShaderResources(_textureIndex, 1, _nowView.GetAddressOf());
	}

	ConstantBufferManager::GetPSMaterial()->data.FresnelR0 = pNodeData->pMaterial->fresnelR0;
	ConstantBufferManager::GetPSMaterial()->data.Shininess = pNodeData->pMaterial->smoothness;
	ConstantBufferManager::GetPSMaterial()->data.metallic = pNodeData->pMaterial->metallic;
	ConstantBufferManager::GetPSMaterial()->data.normalFactor = pNodeData->pMaterial->normalFactor;
	ConstantBufferManager::GetPSMaterial()->data.emissiveFactor = pNodeData->pMaterial->emissiveFactor;

	// 빛에 대한 정보도 함께 넘겨준다
	ConstantBufferManager::GetPSDirLightBuffer()->ApplyChanges();
	m_pDeviceContext->PSSetConstantBuffers(0, 1, ConstantBufferManager::GetPSDirLightBuffer()->GetAddressOf());

	// 오브젝트 별로 필요한 것만 여기서 밀어넣는다
	ConstantBufferManager::GetPSMaterial()->ApplyChanges();
	m_pDeviceContext->PSSetConstantBuffers(1, 1, ConstantBufferManager::GetPSMaterial()->GetAddressOf());

	m_pDeviceContext->PSSetSamplers(0, 1, RasterizerState::GetLinearSamplerStateAddressOf());
}



void Renderer::CommonConstantBuffer()
{
	m_pDeviceContext->PSSetConstantBuffers(2, 1, ConstantBufferManager::GetPSCamera()->GetAddressOf());
}

void Renderer::RenderProcess(
	int modelMeshIndex,
	std::shared_ptr<NodeData>& pData, /* 굳이 복사 안함 */
	const DirectX::XMMATRIX& worldTM,
	DirectX::XMMATRIX* pBoneOffsetTM)
{
	// 현재 이버전은 하드웨어 인스턴싱과 거리가 멀다
	// 셋 머트리얼을 이전에 한번 만 해준다
	// 어차피 같은 노드데이터를 쓰니까
	if (modelMeshIndex != -1)
	{
		if (pBoneOffsetTM == nullptr)
		{
			auto _model = m_pResourceManager->GetModelMesh(modelMeshIndex);
			_model->SetBoneOffsetToBuffer(ConstantBufferManager::GetVSBoneBuffer()->data.boneOffset);
		}
		else
		{
			memcpy(ConstantBufferManager::GetVSBoneBuffer()->data.boneOffset, pBoneOffsetTM, sizeof(DirectX::XMMATRIX) * 96);
		}

		ConstantBufferManager::GetVSBoneBuffer()->ApplyChanges();
		m_pDeviceContext->VSSetConstantBuffers(2, 1, ConstantBufferManager::GetVSBoneBuffer()->GetAddressOf());

		ConstantBufferManager::GetVSPerObjectBuffer()->data.WorldViewProjection = DirectX::SimpleMath::Matrix::Identity * m_ViewTM * m_ProjectionTM;
		ConstantBufferManager::GetVSPerObjectBuffer()->data.World = DirectX::SimpleMath::Matrix::Identity;
		ConstantBufferManager::GetVSPerObjectBuffer()->data.WorldInverse = m_ViewTM;

		// 그림자를 넣을 때 모델의 world값도 같이 곱해서 넣어줘야한다
		// 그림자 캐스팅을 하는 라이트가 없으면 안돌아야함
		for (unsigned int i = 0; i < m_LightManager->GetShadowLightCount(); i++)
		{
			ConstantBufferManager::GetLightMatrixBuffer()->data.lightMatrix[i] =
				DirectX::SimpleMath::Matrix::Identity * m_LightManager->GetShadowLightPtr(i)->GetShadowTM();
		}
	}
	else
	{
		ConstantBufferManager::GetVSPerObjectBuffer()->data.WorldViewProjection = worldTM * m_ViewTM * m_ProjectionTM;
		ConstantBufferManager::GetVSPerObjectBuffer()->data.World = worldTM;
		ConstantBufferManager::GetVSPerObjectBuffer()->data.WorldInverse = m_ViewTM;



		for (unsigned int i = 0; i < m_LightManager->GetShadowLightCount(); i++)
		{
			ConstantBufferManager::GetLightMatrixBuffer()->data.lightMatrix[i] =
				worldTM * m_LightManager->GetShadowLightPtr(i)->GetShadowTM();
		}
	}

	UINT offset = 0;
	m_pDeviceContext->IASetVertexBuffers(0, 1, pData->GetVertexBuffer().GetAddressOf(), pData->StridePtr(), &offset);
	m_pDeviceContext->IASetIndexBuffer(pData->GetIndexBuffer().Get(), DXGI_FORMAT::DXGI_FORMAT_R32_UINT, 0);

	ID3D11Buffer* _bf[2] = { nullptr, };
	m_pDeviceContext->VSSetConstantBuffers(0, 1, _bf);

	// 이 부분이 SetVertexBuffers 보다 뒤에서 호출되어야 한다
	ConstantBufferManager::GetVSPerObjectBuffer()->ApplyChanges();
	m_pDeviceContext->VSSetConstantBuffers(0, 1, ConstantBufferManager::GetVSPerObjectBuffer()->GetAddressOf());

	// 버텍스 마다 달라서 여기서 처리해줘야 한다
	ConstantBufferManager::GetLightMatrixBuffer()->ApplyChanges();
	m_pDeviceContext->VSSetConstantBuffers(1, 1, ConstantBufferManager::GetLightMatrixBuffer()->GetAddressOf());

	m_pDeviceContext->DrawIndexed(pData->m_IndexCount, 0, 0);
}

void Renderer::RenderProcessInstance(
	int modelMeshIndex,
	Microsoft::WRL::ComPtr<ID3D11Buffer>& pInstancedBuffer,
	std::shared_ptr<NodeData>& pData,
	unsigned int instanceCount)
{
	ConstantBufferManager::GetVSPerObjectBuffer()->data.WorldViewProjection = m_ViewTM * m_ProjectionTM;
	UINT stride[2] = { sizeof(Vertex::Vertex), sizeof(InstancedData) };
	UINT offset[2] = { 0,0 };

	ID3D11Buffer* vbs[2] = { pData->GetVertexBuffer().Get(), pInstancedBuffer.Get() };
	m_pDeviceContext->IASetVertexBuffers(0, 2, vbs, stride, offset);
	m_pDeviceContext->IASetIndexBuffer(pData->GetIndexBuffer().Get(), DXGI_FORMAT::DXGI_FORMAT_R32_UINT, 0);


	ID3D11Buffer* _bf[2] = { nullptr, };
	m_pDeviceContext->VSSetConstantBuffers(0, 1, _bf);

	// 이 부분이 SetVertexBuffers 보다 뒤에서 호출되어야 한다
	ConstantBufferManager::GetVSPerObjectBuffer()->ApplyChanges();
	m_pDeviceContext->VSSetConstantBuffers(0, 1, ConstantBufferManager::GetVSPerObjectBuffer()->GetAddressOf());

	// 버텍스 마다 달라서 여기서 처리해줘야 한다
	ConstantBufferManager::GetLightMatrixBuffer()->ApplyChanges();
	m_pDeviceContext->VSSetConstantBuffers(1, 1, ConstantBufferManager::GetLightMatrixBuffer()->GetAddressOf());

	if (modelMeshIndex != -1)
	{
		m_pDeviceContext->IASetInputLayout(m_SkinnedInput.Get());
		m_pDeviceContext->VSSetShader(m_InstanceSkinnedMesh->GetVertexShader(), NULL, 0);
	}
	else
	{
		m_pDeviceContext->IASetInputLayout(m_StaticInput.Get());
		m_pDeviceContext->VSSetShader(m_InstanceStaticMesh->GetVertexShader(), NULL, 0);
	}

	// 이것도 이전에 한번만 해주자	
	m_pDeviceContext->DrawIndexedInstanced(pData->m_IndexCount, instanceCount, 0, 0, 0);
}

void Renderer::RenderOITProcess(
	int modelMeshIndex,
	std::shared_ptr<NodeData>& pData, /* 굳이 복사 안함 */
	const DirectX::XMMATRIX& worldTM,
	DirectX::XMMATRIX* pBoneOffsetTM)
{
	if (modelMeshIndex != -1)
	{
		if (pBoneOffsetTM == nullptr)
		{
			auto _model = m_pResourceManager->GetModelMesh(modelMeshIndex);
			_model->SetBoneOffsetToBuffer(ConstantBufferManager::GetVSBoneBuffer()->data.boneOffset);
		}
		else
		{
			memcpy(ConstantBufferManager::GetVSBoneBuffer()->data.boneOffset, pBoneOffsetTM, sizeof(DirectX::XMMATRIX) * 96);
		}

		ConstantBufferManager::GetVSBoneBuffer()->ApplyChanges();
		m_pDeviceContext->VSSetConstantBuffers(2, 1, ConstantBufferManager::GetVSBoneBuffer()->GetAddressOf());

		ConstantBufferManager::GetVSPerObjectBuffer()->data.WorldViewProjection = DirectX::SimpleMath::Matrix::Identity * m_ViewTM * m_ProjectionTM;
		ConstantBufferManager::GetVSPerObjectBuffer()->data.World = DirectX::SimpleMath::Matrix::Identity;
		ConstantBufferManager::GetVSPerObjectBuffer()->data.WorldInverse = DirectX::SimpleMath::Matrix::Identity.Invert();

		// 그림자를 넣을 때 모델의 world값도 같이 곱해서 넣어줘야한다
		// 그림자 캐스팅을 하는 라이트가 없으면 안돌아야함
		for (unsigned int i = 0; i < m_LightManager->GetShadowLightCount(); i++)
		{
			ConstantBufferManager::GetLightMatrixBuffer()->data.lightMatrix[i] =
				DirectX::SimpleMath::Matrix::Identity * m_LightManager->GetShadowLightPtr(i)->GetShadowTM();
		}
	}
	else
	{
		ConstantBufferManager::GetVSPerObjectBuffer()->data.WorldViewProjection = worldTM * m_ViewTM * m_ProjectionTM;
		ConstantBufferManager::GetVSPerObjectBuffer()->data.World = worldTM;
	}

	UINT offset = 0;
	m_pDeviceContext->IASetVertexBuffers(0, 1, pData->GetVertexBuffer().GetAddressOf(), pData->StridePtr(), &offset);
	m_pDeviceContext->IASetIndexBuffer(pData->GetIndexBuffer().Get(), DXGI_FORMAT::DXGI_FORMAT_R32_UINT, 0);

	m_pDeviceContext->OMSetDepthStencilState(RasterizerState::GetOITInitDS(), 1);
	m_pDeviceContext->OMSetBlendState(RasterizerState::GetOITInitBS(), nullptr, 0xffffffff);

	ID3D11Buffer* _bf[2] = { nullptr, };
	m_pDeviceContext->VSSetConstantBuffers(0, 1, _bf);

	// 이 부분이 SetVertexBuffers 보다 뒤에서 호출되어야 한다
	ConstantBufferManager::GetVSPerObjectBuffer()->ApplyChanges();
	m_pDeviceContext->VSSetConstantBuffers(0, 1, ConstantBufferManager::GetVSPerObjectBuffer()->GetAddressOf());

	// 버텍스 마다 달라서 여기서 처리해줘야 한다
	ConstantBufferManager::GetLightMatrixBuffer()->ApplyChanges();
	m_pDeviceContext->VSSetConstantBuffers(1, 1, ConstantBufferManager::GetLightMatrixBuffer()->GetAddressOf());

	m_pDeviceContext->DrawIndexed(pData->m_IndexCount, 0, 0);
}

void Renderer::RenderBillboardProcess(
	unsigned int spriteIndex,
	const DirectX::XMMATRIX& worldTM)
{
	std::shared_ptr<SpriteData> _nowSprite = m_pResourceManager->GetSpriteData(spriteIndex);

	// RenderQeueueEffectElement를 초기화해서 Queue에 존재하지 않는다는 신호를 준다
	_nowSprite->m_pTempRenderQueueEffectElement = nullptr;

	m_pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_pDeviceContext->RSSetState(RasterizerState::GetSolidNormal());

	UINT offset = 0;

	m_pDeviceContext->IASetVertexBuffers(0, 1, m_SpriteQuad->GetVertexBuffer().GetAddressOf(), m_SpriteQuad->StridePtr(), &offset);
	m_pDeviceContext->IASetIndexBuffer(m_SpriteQuad->GetIndexBuffer(), DXGI_FORMAT::DXGI_FORMAT_R32_UINT, 0);

	m_pDeviceContext->OMSetBlendState(RasterizerState::GetAlphaBlenderStateUI(), nullptr, 0xffffffff);

	ConstantBufferManager::GetVSWVPBuffer()->data.worldMatrix = worldTM;
	ConstantBufferManager::GetVSWVPBuffer()->data.viewMatrix = m_ViewTM;
	ConstantBufferManager::GetVSWVPBuffer()->data.projectionMatrix = m_ProjectionTM;
	ConstantBufferManager::GetVSWVPBuffer()->ApplyChanges();

	m_pDeviceContext->VSSetConstantBuffers(0, 1, ConstantBufferManager::GetVSWVPBuffer()->GetAddressOf());
	m_pDeviceContext->PSSetShaderResources(0, 1, _nowSprite->shaderResource.GetAddressOf());

	m_pDeviceContext->PSSetSamplers(0, 1, RasterizerState::GetPointSamplerStateAddressOf());

	m_pDeviceContext->DrawIndexed(m_SpriteQuad->GetIndexCount(), 0, 0);
}

void Renderer::RenderIDColor(
	unsigned int ObjectID,
	int modelIndex,
	std::shared_ptr<NodeData>& pData,
	const DirectX::XMMATRIX& worldTM,
	DirectX::XMMATRIX* pBoneOffsetTM)
{
	m_pDeviceContext->RSSetState(RasterizerState::GetSolidNormal());
	m_pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	m_pDeviceContext->OMSetDepthStencilState(RasterizerState::GetDepthStencilState(), 1);
	//m_pDeviceContext->OMSetBlendState(RasterizerState::GetDestZeroBlendState(), nullptr, 0xffffffff);
	m_pDeviceContext->OMSetBlendState(RasterizerState::GetAlphaBlenderState(), nullptr, 0xffffffff);

	UINT offset = 0;
	m_pDeviceContext->IASetVertexBuffers(0, 1, pData->GetVertexBuffer().GetAddressOf(), pData->StridePtr(), &offset);
	m_pDeviceContext->IASetIndexBuffer(pData->GetIndexBuffer().Get(), DXGI_FORMAT::DXGI_FORMAT_R32_UINT, 0);

	if (modelIndex != -1)
	{
		auto _nowModelMesh = m_pResourceManager->GetModelMesh(modelIndex);

		if (pBoneOffsetTM == nullptr)
		{
			auto _model = m_pResourceManager->GetModelMesh(modelIndex);
			_model->SetBoneOffsetToBuffer(ConstantBufferManager::GetVSBoneBuffer()->data.boneOffset);
		}
		else
		{
			memcpy(ConstantBufferManager::GetVSBoneBuffer()->data.boneOffset, pBoneOffsetTM, sizeof(DirectX::XMMATRIX) * 96);
		}

		ConstantBufferManager::GetVSBoneBuffer()->ApplyChanges();
		m_pDeviceContext->VSSetConstantBuffers(1, 1, ConstantBufferManager::GetVSBoneBuffer()->GetAddressOf());

		ConstantBufferManager::GetWVPMatrix()->data.wvpMatrix = pData->animationTM * m_ViewTM * m_ProjectionTM;
		ConstantBufferManager::GetWVPMatrix()->ApplyChanges();
	}
	else
	{
		ConstantBufferManager::GetWVPMatrix()->data.wvpMatrix = worldTM * m_ViewTM * m_ProjectionTM;
		ConstantBufferManager::GetWVPMatrix()->ApplyChanges();
	}

	m_pDeviceContext->VSSetConstantBuffers(0, 1, ConstantBufferManager::GetWVPMatrix()->GetAddressOf());

	//ID를 컬러값으로 바꾼다. 0x는 16진법
	unsigned int _r = (ObjectID & 0x000000FF) >> 0;
	unsigned int _g = (ObjectID & 0x0000FF00) >> 8;
	unsigned int _b = (ObjectID & 0x00FF0000) >> 16;
	unsigned int _a = (ObjectID & 0xFF000000) >> 24;

	ConstantBufferManager::GetIDpickingBuffer()->data.r = _r / 255.0f;
	ConstantBufferManager::GetIDpickingBuffer()->data.g = _g / 255.0f;
	ConstantBufferManager::GetIDpickingBuffer()->data.b = _b / 255.0f;
	ConstantBufferManager::GetIDpickingBuffer()->data.a = _a / 255.0f;
	ConstantBufferManager::GetIDpickingBuffer()->ApplyChanges();

	m_pDeviceContext->PSSetConstantBuffers(0, 1, ConstantBufferManager::GetIDpickingBuffer()->GetAddressOf());
	m_pDeviceContext->DrawIndexed(pData->m_IndexCount, 0, 0);

}

void Renderer::DebugDraw(bool clear)
{
	// 바깥에 있는 디버그용 렌더 타겟을 가져와서 묶었다
	m_pDeviceContext->OMSetRenderTargets(1,
		m_pDebugRenderTarget->GetRenderTargetViewAddressOf(),
		m_Deferred->GetDepthBuffer()->GetDepthSetncilView());

	if (clear == true)
	{
		float nullColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
		m_pDeviceContext->ClearRenderTargetView(m_pDebugRenderTarget->GetRenderTargerViewRawptr(), nullColor);
	}
}

void Renderer::DebugDraw(
	unsigned int rasterMode,
	const unsigned int objectID,
	const unsigned int modelIndex,
	const DirectX::XMMATRIX& worldTM,
	DirectX::XMMATRIX* pBoneOffsetTM)
{

	auto _nowModelMesh = m_pResourceManager->GetModelMesh(modelIndex);

	// 본 오프셋
	if (_nowModelMesh->m_pSkeleton != nullptr)
	{

		for (auto& _nowNode : *_nowModelMesh->m_pNodeData_V)
		{
			_nowNode->m_BoundingVolume->Update(worldTM);

			if (m_FrustumVolume->GetFrustum().Intersects(_nowNode->m_BoundingVolume->GetBoundingSphere()) == true)
			{
				m_RenderDebugInfo.renderedObject++;

				m_RenderQueue->AddRenderQueue(
					VS_SHADER(eSHADER::VS_IDPICKINGSKINNING),
					PS_SHADER(eSHADER::PS_IDPICKING),
					0, objectID, modelIndex, false,
					_nowNode, worldTM, pBoneOffsetTM);
			}
			else
			{
				m_RenderDebugInfo.culledObject++;
			}
		}

	}
	else // 매쉬만 있는경우
	{
		for (auto& _nowNode : *_nowModelMesh->m_pNodeData_V)
		{
			_nowNode->CalculateTransform(worldTM);
			_nowNode->m_BoundingVolume->Update(_nowNode->animationTM);


			/// 프러스텀과 바운딩 박스가 intersect되는 원리 알아오기
			if (m_FrustumVolume->GetFrustum().Intersects(_nowNode->m_BoundingVolume->GetBoundingSphere()) == true)
			{
				m_RenderDebugInfo.renderedObject++;

				m_RenderQueue->AddRenderQueue(
					VS_SHADER(eSHADER::VS_IDPICKING),
					PS_SHADER(eSHADER::PS_IDPICKING),
					0, objectID, -1, false,
					_nowNode, _nowNode->animationTM, pBoneOffsetTM);
			}
		}
	}
}

void Renderer::DeferredPickingPass()
{
	// 디퍼드 정보 중 SRV를 가져와 배열에 담는다
	ID3D11ShaderResourceView* _srv[6] =
	{
		m_Deferred->GetDrawLayers()[NORMAL_MAP]->GetShaderResourceViewRawptr(),
		m_Deferred->GetDrawLayers()[POSITION_MAP]->GetShaderResourceViewRawptr(),
		m_Deferred->GetDrawLayers()[ID_MAP]->GetShaderResourceViewRawptr(),

		m_Deferred->GetTransparentDrawLayers()[NORMAL_MAP]->GetShaderResourceViewRawptr(),
		m_Deferred->GetTransparentDrawLayers()[POSITION_MAP]->GetShaderResourceViewRawptr(),
		m_Deferred->GetTransparentDrawLayers()[ID_MAP]->GetShaderResourceViewRawptr(),
	};

	// 템플릿으로 만들어진 함수에 해당 픽셀에서 원하는 클래스를 넣는다
	// 마우스의 위치, 윈도우 크기, SRV 배열과 크기, Staging Buffer를 전달한다
	m_PixelPicker->PixelPickingFromSRV<DeferredPixel>(
		m_pDeviceContext, m_MousePos,
		m_ClientWidth, m_ClientHeight, _srv, 6, m_DeferredPixelData.get());

	m_ClickedObjectId = m_DeferredPixelData->m_Data.objectID;
}

void Renderer::DebugQueueProcess()
{
	m_RenderQueue->ProcessDebugQueue(this);
	m_RenderQueue->Clear();

	ID3D11RenderTargetView* _rt[1] = { nullptr };
	m_pDeviceContext->OMSetRenderTargets(1, _rt, nullptr);

	ID3D11ShaderResourceView* _srv[1] =
	{
		m_pIDColorRenderTarget->GetShaderResourceViewRawptr(),
	};


	StagingBuffer<PixelID>* _nowStaging = new StagingBuffer<PixelID>();
	_nowStaging->Initialize<PixelID>(m_pDevice.Get(), 1);
	m_PixelPicker->PixelPickingFromSRV<PixelID>(
		m_pDeviceContext, m_MousePos, m_ClientWidth, m_ClientHeight, _srv, 1, _nowStaging);

	unsigned int _id = 0x00000000;
	unsigned int _r = static_cast<int>(_nowStaging->m_Data.color.x * 255.0f);
	unsigned int _g = static_cast<int>(_nowStaging->m_Data.color.y * 255.0f);
	unsigned int _b = static_cast<int>(_nowStaging->m_Data.color.z * 255.0f);
	unsigned int _a = static_cast<int>(_nowStaging->m_Data.color.w * 255.0f);

	_id = _r | _g << 8 | _b << 16 | _a << 24;
	m_ClickedObjectId = _id;

	delete _nowStaging;
}

void Renderer::UIPassBind()
{
	float _clear[4] = { 0.0f, 0.0f, 0.0f, 0.0f, };
	m_pDeviceContext->ClearRenderTargetView(m_pUIRenderTarget->GetRenderTargerViewRawptr(), _clear);
	m_pDeviceContext->OMSetRenderTargets(1, m_pUIRenderTarget->GetRenderTargetViewAddressOf(), m_pDepthStencil->GetDepthSetncilView());
	m_pDeviceContext->ClearDepthStencilView(m_pDepthStencil->GetDepthSetncilView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

void Renderer::HelperDraw()
{
	// 디버그용 렌더 타겟이 바인딩 된 이후다
	HelperRender();
}

void Renderer::RenderOutLineProcess(
	int modelMeshIndex,
	std::shared_ptr<NodeData>& pData,
	const DirectX::XMMATRIX& worldTM,
	DirectX::XMMATRIX* pBoneOffsetTM)
{
	if (modelMeshIndex > -1)
	{
		auto _nowModelMesh = m_pResourceManager->GetModelMesh(modelMeshIndex);

		m_pDeviceContext->RSSetState(RasterizerState::GetSolidNormal());
		m_pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		UINT offset = 0;
		m_pDeviceContext->IASetVertexBuffers(0, 1, pData->GetVertexBuffer().GetAddressOf(), pData->StridePtr(), &offset);
		m_pDeviceContext->IASetIndexBuffer(pData->GetIndexBuffer().Get(), DXGI_FORMAT::DXGI_FORMAT_R32_UINT, 0);

		ID3D11Buffer* _bf[2] = { nullptr, };
		m_pDeviceContext->VSSetConstantBuffers(0, 1, _bf);

		if (_nowModelMesh->m_pSkeleton != nullptr)
		{
			ConstantBufferManager::GetVertexBuffer()->data.wvpMatrix = m_ViewTM * m_ProjectionTM;
			ConstantBufferManager::GetVertexBuffer()->data.worldMatrix = DirectX::SimpleMath::Matrix::Identity;
			ConstantBufferManager::GetVertexBuffer()->ApplyChanges();

			m_pDeviceContext->VSSetConstantBuffers(0, 1, ConstantBufferManager::GetVertexBuffer()->GetAddressOf());

			if (pBoneOffsetTM == nullptr)
			{
				_nowModelMesh->SetBoneOffsetToBuffer(ConstantBufferManager::GetVSBoneBuffer()->data.boneOffset);
			}
			else
			{
				memcpy(ConstantBufferManager::GetVSBoneBuffer()->data.boneOffset, pBoneOffsetTM, sizeof(DirectX::XMMATRIX) * 96);
			}

			ConstantBufferManager::GetVSBoneBuffer()->ApplyChanges();
			m_pDeviceContext->VSSetConstantBuffers(2, 1, ConstantBufferManager::GetVSBoneBuffer()->GetAddressOf());

		}
		else
		{
			ConstantBufferManager::GetVSWVPBuffer()->data.worldMatrix = worldTM;
			ConstantBufferManager::GetVSWVPBuffer()->data.viewMatrix = m_ViewTM;
			ConstantBufferManager::GetVSWVPBuffer()->data.projectionMatrix = m_ViewTM * m_ProjectionTM;
			ConstantBufferManager::GetVSWVPBuffer()->ApplyChanges();

			m_pDeviceContext->VSSetConstantBuffers(0, 1, ConstantBufferManager::GetVSWVPBuffer()->GetAddressOf());
		}

		m_pDeviceContext->DrawIndexed(pData->m_IndexCount, 0, 0);
	}
	else
	{
		m_pDeviceContext->OMSetRenderTargets(1, m_Forward->GetRenderTargetView()->GetRenderTargetViewAddressOf(), nullptr);
	}
}

void Renderer::RenderOutLine()
{
	m_pDeviceContext->OMSetRenderTargets(1, m_Forward->GetRenderTargetView()->GetRenderTargetViewAddressOf(), nullptr);

	/// 노멀그린것 가져와서 외곽선 그리기
	m_pDeviceContext->OMSetBlendState(RasterizerState::GetOutLineBlendState(), nullptr, 0xffffffff);
	//m_pDeviceContext->OMSetBlendState(RasterizerState::GetAlphaBlenderState(), nullptr, 0xffffffff);
	ID3D11ShaderResourceView* arrViews[1] = { m_pOutLineRenderTarget->GetShaderResourceViewRawptr() };
	m_pDeviceContext->PSSetShaderResources(0, 1, &arrViews[0]);

	m_pDeviceContext->PSSetSamplers(0, 1, RasterizerState::GetOutLineSamplerStateAddressOf());

	SetVertexShader(VS_SHADER(eSHADER::VS_OUTLINE));
	SetPixelShader(PS_SHADER(eSHADER::PS_OUTLINE));

	m_pDeviceContext->IASetInputLayout(NULL);
	m_pDeviceContext->IASetVertexBuffers(0, 0, NULL, NULL, NULL);
	m_pDeviceContext->IASetIndexBuffer(NULL, DXGI_FORMAT_UNKNOWN, 0);
	m_pDeviceContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	m_pDeviceContext->Draw(4, 0);
}

void Renderer::CombineRenderTarget(
	std::shared_ptr<RenderTargetView> pRenderTarget,
	ID3D11BlendState* blendState,
	ID3D11DepthStencilState* DepthStencilState)
{
	ID3D11ShaderResourceView* arrViews[1] = { pRenderTarget->GetShaderResourceViewRawptr() };
	m_pDeviceContext->PSSetShaderResources(0, 1, &arrViews[0]);

	m_pDeviceContext->IASetInputLayout(NULL);
	m_pDeviceContext->IASetVertexBuffers(0, 0, NULL, NULL, NULL);
	m_pDeviceContext->IASetIndexBuffer(NULL, DXGI_FORMAT_UNKNOWN, 0);
	m_pDeviceContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	ID3D11SamplerState* arrSamplers[1] = { RasterizerState::GetPointSamplerState() };
	m_pDeviceContext->PSSetSamplers(0, 1, arrSamplers);

	/// 블렌드 설정을 해야지 기존 렌더 타겟에 그려져 있던 것에 추가로 덧그릴 수 있다
	/// 이게 없을 때는 새로 그려버린다
	if (blendState != nullptr)
	{
		m_pDeviceContext->OMSetBlendState(blendState, nullptr, 0xffffff);
	}

	if (DepthStencilState != nullptr)
	{
		m_pDeviceContext->OMSetDepthStencilState(DepthStencilState, 1); //스텐실 레퍼런스는 모지??
	}
	// Set the shaders
	m_pDeviceContext->VSSetShader(m_pCombineVertexShader->GetVertexShader(), NULL, 0);
	m_pDeviceContext->PSSetShader(PS_SHADER(eSHADER::PS_2D)->GetPixelShader(), NULL, 0);

	m_pDeviceContext->Draw(4, 0);

	// Cleanup
	//ZeroMemory(arrViews, sizeof(arrViews));
	//m_pDeviceContext->PSSetShaderResources(0, 1, arrViews);
	//m_pDeviceContext->VSSetShader(NULL, NULL, 0);
	//m_pDeviceContext->PSSetShader(NULL, NULL, 0);
}

void Renderer::EndDraw()
{
	m_pDeviceContext->RSSetState(RasterizerState::GetNoCullingRS());
	m_pDeviceContext->OMSetBlendState(RasterizerState::GetBlenderState(), nullptr, 0xffffff);

	if (m_RenderOption.isBloom == true)
		m_PostProcess->ApplyPostProcess(m_pDeviceContext, m_pRenderTarget, m_Forward->GetRenderTargetView(), ePOST_PROCESS::BLOOM);
	else
	{
		m_PostProcess->ApplyPostProcess(m_pDeviceContext, m_pRenderTarget,
			m_Forward->GetRenderTargetView(), ePOST_PROCESS::NONE);
	}

	if (m_RenderOption.isHDRRender == true)
	{
		m_PostProcess->ApplyFXAA(m_pDeviceContext, m_pRenderTarget, m_ClientWidth, m_ClientHeight);
	}

	// 디버그 정보 추가


	///
	// vs/psnoraml그림을 그린다.
	// rendertarget은 outlinerendertarget
	// renderQueue에 넣어준 Temp를 가져와서 그린다(m_clicked == true일때만)
	///
	// ibl렌더링 중 하나라도 피킹됭게 있으면 true

	float null[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	m_pDeviceContext->ClearRenderTargetView(m_pOutLineRenderTarget->GetRenderTargerViewRawptr(), null);
	m_pDeviceContext->OMSetRenderTargets(1, m_pOutLineRenderTarget->GetRenderTargetViewAddressOf(), nullptr);
	/// 노멀 그리기
	// 렌더큐에 담아놓은 아웃라인 정보 가져오기
	if (m_IsSelectedAny == true)
	{
		m_RenderQueue->CopyOutLineMesh(VS_SHADER(eSHADER::VS_OUTLINE), PS_SHADER(eSHADER::PS_OUTLINE), -1, nullptr, DirectX::XMMATRIX(), nullptr);
	}

	m_RenderQueue->ProcessOutLineQueue(this, m_RenderOption.isQT);

	if (m_RenderOption.isQT == false)
	{
	}

	m_pDeviceContext->OMSetBlendState(RasterizerState::GetAlphaBlenderState(), nullptr, 0xffffffff);
	m_pDeviceContext->OMSetRenderTargets(1, m_Forward->GetRenderTargetView()->GetRenderTargetViewAddressOf(), nullptr);

	// UI를 묶는다
	CombineRenderTarget(m_pUIRenderTarget, RasterizerState::GetDeferredBlendState(), nullptr);


	// 텍스트
	m_pD2D->Draw_AllText();


	if (m_RenderOption.bDebugRenderMode == true)
	{
		// 디버그정보들(IMGUI, grid등...)을 묶는다
		CombineRenderTarget(m_pDebugRenderTarget, RasterizerState::GetAlphaBlenderState(), nullptr);
	}

	// 디버그정보들(IMGUI, grid등...)을 묶는다
	CombineRenderTarget(m_pDebugRenderTarget, RasterizerState::GetAlphaBlenderState(), nullptr);

	if (m_RenderOption.isDeferredDebug == true)
	{
		m_DebugRender->DrawDeferredDebug(
			m_pDeviceContext,
			m_Deferred->GetDrawLayers(),
			m_LightManager->GetTextureLightShaderResourceView(0),
			VS_SHADER(eSHADER::VS_COMBINE),
			PS_SHADER(eSHADER::PS_DEFERREDDEBUG));
	}

	// 그림자 패스를 다 지운다
	//for (int i = 0; i < m_LightCount; i++)
	for (unsigned int i = 0; i < m_LightManager->GetShadowLightCount(); i++)
	{
		m_LightManager->GetShadowLightPtr(i)->ClearShadowRenderTarget(m_pDeviceContext.Get());
	}
	// 그림자 개수도 초기화
	//m_Light_V[0]->m_ShadowLightCount = 0;
	//m_LightCount = -1;
	m_LightManager->ShadowLightCountReset();


	//---------------------------------------------------------------------------------------
	// Set per frame constants.	
	// restore default states, as the SkyFX changes them in the effect file.
	//---------------------------------------------------------------------------------------
	m_pDeviceContext->RSSetState(0);
	m_pDeviceContext->OMSetDepthStencilState(0, 0);

	//---------------------------------------------------------------------------------------
	// Unbind shadow map as a shader input because we are going to render to it next frame.
	// The shadow might might be at any slot, so clear all slots.
	//---------------------------------------------------------------------------------------
	ID3D11ShaderResourceView* nullSRV[16] = { 0 };
	m_pDeviceContext->PSSetShaderResources(0, 16, nullSRV);

	this->m_pDeviceContext->RSSetState(0);
	this->m_p1Swapchain->Present(0, NULL);

	m_isClicked = false;
}

// 기본이 되는 애니메이션 처리 기능
bool Renderer::AnimationProcess(
	const unsigned int modelIndex,
	const unsigned int animationIndex,
	unsigned int& keyFrame,
	const DirectX::SimpleMath::Matrix& worldTM,
	DirectX::SimpleMath::Matrix* pPrevWorld,
	float offsetAngle)
{
	bool _isAnimationOver = false;

	std::shared_ptr<ModelMesh>  _nowModel = m_pResourceManager->GetModelMesh(modelIndex);
	std::shared_ptr<AnimationClipData> _nowAnimData = m_pResourceManager->GetAnimationClipData(animationIndex);

	if (keyFrame >= _nowAnimData->m_LastKeyFrame)
	{
		_isAnimationOver = true;
		// 키가 레퍼런스로 넘어왔기 때문에 처음으로 초기화된다
		keyFrame = _nowAnimData->m_FirstKeyFrame;
	}

	switch (_nowAnimData->animType)
	{
	case eANIM_TYPE::SKINNED:
	{
		if (_nowModel->m_pSkeleton == nullptr) return true;
		_nowModel->m_pSkeleton->BindPose();

		// 넘어온 키프레임을 써야 모든 애니메이션을 가진 객체들이 독립적으로 행동한다
		_nowModel->m_pSkeleton->AnimateByAnimationClip(
			_nowAnimData, worldTM, keyFrame, pPrevWorld, offsetAngle);
		break;
	}
	case eANIM_TYPE::MESH:

	default:
		break;
	}

	return _isAnimationOver;
}

bool Renderer::AnimationCrossFading(
	const unsigned int modelIndex,
	const unsigned int easeInAnim,
	const unsigned int easeOutAnim,
	unsigned int& easeInKeyFrame,
	unsigned int easeOutKeyFrame,
	unsigned int fadingPeriod,
	bool& fadingOver,
	const DirectX::SimpleMath::Matrix& worldTM,
	DirectX::SimpleMath::Matrix* pPrevWorld,
	DirectX::SimpleMath::Matrix* pInterpolateTM,
	float offsetAngle /*= 0.0f*/)
{
	bool _isAnimationOver = false;

	std::shared_ptr<ModelMesh>  _nowModel = m_pResourceManager->GetModelMesh(modelIndex);

	// 스키닝만 해당
	if (_nowModel->m_pSkeleton == nullptr) return true;
	_nowModel->m_pSkeleton->BindPose();

	// easeIn data가 최종적으로 남아야하는 데이터
	std::shared_ptr<AnimationClipData> _easeInAnimData = m_pResourceManager->GetAnimationClipData(easeInAnim);
	std::shared_ptr<AnimationClipData> _easeOutAnimData = m_pResourceManager->GetAnimationClipData(easeOutAnim);

	float _periodPercent = fadingPeriod * 0.01f;
	// fading에서 마지막으로 도달하는 키프레임, 소수로 나와도 정수로 표현되어야 한다
	float _fadingLimitKeyFrame = _easeInAnimData->m_TotalKeyFrames * _periodPercent;
	float _blendUnit = (float)(1 / _fadingLimitKeyFrame);


	if (easeOutKeyFrame >= _easeOutAnimData->m_LastKeyFrame)
	{
		// easeOut되는 애니메이션을 그대로 유지한다
		easeOutKeyFrame = _easeOutAnimData->m_LastKeyFrame - 1;
	}

	if (easeInKeyFrame >= _easeInAnimData->m_LastKeyFrame)
	{
		// easeIn되는 애니메이션은 그대로 남아있어야 하는데, 해당 애니메이션 끝에 도달하면 애니메이션은 종료된 것
		_isAnimationOver = true;
		easeInKeyFrame = _easeInAnimData->m_FirstKeyFrame;
	}

	// easeIn 애니메이션은 무조건 0부터 시작하므로
	// fading될 Period만 알고 있으면 언제 종료할지도 알 수 있다
	_nowModel->m_pSkeleton->CrossFadingByPrevAnimatoinTM(
		_easeInAnimData, worldTM,
		pPrevWorld, pInterpolateTM,
		easeInKeyFrame, _blendUnit * easeInKeyFrame);


	if (easeInKeyFrame >= _fadingLimitKeyFrame)
	{
		// 이제 fading 끝남
		fadingOver = true;
	}
	else
	{
		fadingOver = false;
	}

	return _isAnimationOver;
}

bool Renderer::MaskingAnimation(
	const unsigned int modelIndex,
	const unsigned int overrideAnim,
	unsigned int& overrideAnimKeyFrame,
	const DirectX::SimpleMath::Matrix& worldTM)
{
	bool _isAnimationOver = false;

	std::shared_ptr<ModelMesh>  _nowModel = m_pResourceManager->GetModelMesh(modelIndex);

	if (_nowModel->m_pSkeleton == nullptr) return true;
	_nowModel->m_pSkeleton->BindPose();

	std::shared_ptr<AnimationClipData> _targetAnimData = m_pResourceManager->GetAnimationClipData(overrideAnim);

	if (overrideAnimKeyFrame >= _targetAnimData->m_LastKeyFrame)
	{
		_isAnimationOver = true;
		overrideAnimKeyFrame = _targetAnimData->m_FirstKeyFrame;
	}

	// 넘어온 키프레임을 써야 모든 애니메이션을 가진 객체들이 독립적으로 행동한다
	_nowModel->m_pSkeleton->MaskingAnimation(_targetAnimData, worldTM, overrideAnimKeyFrame);

	return _isAnimationOver;
}

void Renderer::MaskingAnimationBiLocomotoion(
	const unsigned int modelIndex,
	const unsigned int overrideAnimBase, const unsigned int overrideAnimSub,
	const float alpha, unsigned int& overrideAnimKeyFrame,
	const DirectX::SimpleMath::Matrix& worldTM)
{
	std::shared_ptr<ModelMesh>  _nowModel = m_pResourceManager->GetModelMesh(modelIndex);
	std::shared_ptr<AnimationClipData> _baseAnim = m_pResourceManager->GetAnimationClipData(overrideAnimBase);
	std::shared_ptr<AnimationClipData> _subAnim = m_pResourceManager->GetAnimationClipData(overrideAnimSub);

	if (overrideAnimKeyFrame >= _baseAnim->m_LastKeyFrame)
	{
		overrideAnimKeyFrame = _baseAnim->m_FirstKeyFrame;
	}

	if (_nowModel->m_pSkeleton == nullptr) return;
	_nowModel->m_pSkeleton->BindPose();

	// 넘어온 키프레임을 써야 모든 애니메이션을 가진 객체들이 독립적으로 행동한다
	_nowModel->m_pSkeleton->MaskingAnimation(_baseAnim, _subAnim, alpha, overrideAnimKeyFrame, worldTM);
}

void Renderer::MaskingAnimationBiLocomotoion(
	const unsigned int modelIndex, const unsigned int overrideAnimBase,
	const float alpha, const DirectX::SimpleMath::Matrix& worldTM)
{
	std::shared_ptr<ModelMesh>  _nowModel = m_pResourceManager->GetModelMesh(modelIndex);
	std::shared_ptr<AnimationClipData> _baseAnim = m_pResourceManager->GetAnimationClipData(overrideAnimBase);

	if (_nowModel->m_pSkeleton == nullptr) return;
	_nowModel->m_pSkeleton->BindPose();

	// 넘어온 키프레임을 써야 모든 애니메이션을 가진 객체들이 독립적으로 행동한다
	_nowModel->m_pSkeleton->MaskingAnimation(_baseAnim, alpha);
}

// 애니메이션 종료 후 BoneOffset 처리
// 실제 본 애니메이션과는 무관하다.
void Renderer::EndAnimation(const unsigned int modelIndex, DirectX::XMMATRIX* pBoneOffset)
{
	std::shared_ptr<ModelMesh> pModel = m_pResourceManager->GetModelMesh(modelIndex);

	// NodeData 내부로 들어가서 본 하나하나의 Offset값을 계산하고 저장, Buffer에도 저장해준다
	if (pModel->m_pSkeleton != nullptr)
	{
		pModel->CalculateBoneOffsetTM(pBoneOffset);
	}
}

void Renderer::SkeletonGizmoRender(
	const unsigned int modelIndex,
	const DirectX::XMMATRIX& worldTM,
	const DirectX::XMMATRIX& viewTM,
	const DirectX::XMMATRIX& projTM)
{
	// 스키닝 애니메이션이 없는데 여기 들어와서 오류가 생긴거
	if (m_pResourceManager->GetModelMesh(modelIndex)->m_pSkeleton == nullptr) return;

	for (auto& _nowBone : m_pResourceManager->GetModelMesh(modelIndex)->m_pSkeleton->pBones_V)
	{
		m_pVisualDebugger->DebuggerRender(
			m_pResourceManager->GetVertexShader("vs_color.cso"),
			m_pResourceManager->GetPixelShader("ps_deferredDebugInfo.cso"),
			m_CameraPos, _nowBone->worldTM,
			viewTM, projTM,
			eHELPER_TYPE::GIZMO);
	}
}

void* Renderer::GetDeviceVoid()
{
	return m_pDevice.Get();
}

void* Renderer::GetDeviceContextVoid()
{
	return m_pDeviceContext.Get();
}

// 시작 지점과 방향을 설정하면 길이 만큼 선을 그려준다
// 내부에서 Buffer를 실시간으로 만들기 때문에 비싼 함수다
void Renderer::DrawDebugging(
	const DirectX::SimpleMath::Vector3& origin,
	const DirectX::SimpleMath::Vector3& dir,
	float distance,
	const DirectX::SimpleMath::Color& color)
{
	m_pVisualDebugger->DrawLine(origin, dir, distance, color, m_ViewTM, m_ProjectionTM);
}

void Renderer::HelperRender()
{
	// 드로우 전용 렌더 타겟을 묶는다
	// 가장 처음으로 불리는 것이므로 내부를 지워준다
	if (m_RenderOption.bDebugRenderMode = true)
	{
		std::shared_ptr<VertexShader> _nowVertexShader = m_pResourceManager->GetVertexShader("vs_color.cso");
		std::shared_ptr<PixelShader> _nowPixelShader = m_pResourceManager->GetPixelShader("ps_deferredColor.cso");

		m_pDeviceContext->RSSetState(RasterizerState::GetSolidNormal());
		m_pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

		m_pDeviceContext->OMSetDepthStencilState(RasterizerState::GetDisabledDSS(), 1);

		for (int i = 0; i < m_LightManager->GetLightCount() + 1; i++)
		{
			switch (m_LightManager->GetLightPtr(i)->m_LightType)
			{
			case DIRECTONAL_LIGHT:
				m_pVisualDebugger->DebuggerRender(_nowVertexShader, _nowPixelShader,
					m_CameraPos,
					m_LightManager->GetLightPtr(i)->GetLightWorldTM(),
					m_ViewTM, m_ProjectionTM, eHELPER_TYPE::DIRECTIONAL_LIGHT);
				break;
			case POINT_LIGHT:
				m_pVisualDebugger->DebuggerRender(_nowVertexShader, _nowPixelShader,
					m_CameraPos,
					m_LightManager->GetLightPtr(i)->GetLightWorldTM(),
					m_ViewTM, m_ProjectionTM, eHELPER_TYPE::POINT_LIGHT);
				break;
			case SPOT_LIGHT:
				m_pVisualDebugger->DebuggerRender(_nowVertexShader, _nowPixelShader,
					m_CameraPos,
					m_LightManager->GetLightPtr(i)->GetLightWorldTM(),
					m_ViewTM, m_ProjectionTM, eHELPER_TYPE::SPOT_LIGHT);
				break;
			}
		}

		m_pVisualDebugger->DrawLine(
			_nowVertexShader,
			m_HLSLShader->m_DebugTestShader,
			m_ViewTM, m_ProjectionTM);


		/// 기즈모
		if (m_IsSelectedAny == true)
		{
			m_pVisualDebugger->DebuggerRender(
				_nowVertexShader,
				_nowPixelShader,
				m_CameraPos,
				m_RenderQueue->GetSelectedObjectTM(),
				m_ViewTM, m_ProjectionTM, eHELPER_TYPE::GIZMO);
		}

		m_pDeviceContext->OMSetDepthStencilState(RasterizerState::GetDepthStencilState(), 1);
		m_pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_LINELIST);


		/// 기즈모
		/*
		if (m_IsSelectedAny == true)
		{
			m_pVisualDebugger->DebuggerRender(
				_nowVertexShader,
				_nowPixelShader,
				m_CameraPos,
				m_RenderQueue->GetSelectedObjectTM(),
				m_ViewTM, m_ProjectionTM, eHELPER_TYPE::GIZMO);
		}
		*/

		m_pDeviceContext->OMSetDepthStencilState(RasterizerState::GetDepthStencilState(), 1);
		m_pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_LINELIST);


		// 헬퍼에서 IBL의 프로브를 모두 그려준다
		// 그러지 않을 필요는 없다.
		// 들어가는 버텍스는 Pos, Normal, Tex
		m_pIBL->RenderProbe(m_pDeviceContext,
			VS_SHADER(eSHADER::VS_STANDARD), PS_SHADER(eSHADER::PS_CUBEMAPSPHERE),
			m_ViewTM, m_ProjectionTM);
	}

}

void Renderer::RenderSphere(float radius, const DirectX::SimpleMath::Matrix& worldTM)
{
	m_pVisualDebugger->DebuggerRender(
		m_pResourceManager->GetVertexShader("vs_color.cso"),
		m_pResourceManager->GetPixelShader("ps_color.cso"),
		m_CameraPos, worldTM, m_ViewTM, m_ProjectionTM,
		eHELPER_TYPE::SPHERE, DirectX::SimpleMath::Vector3(radius, 0.0f, 0.0f));
}

void Renderer::RenderBox(const DirectX::SimpleMath::Vector3& size, const DirectX::SimpleMath::Matrix& worldTM)
{
	m_pVisualDebugger->DebuggerRender(
		m_pResourceManager->GetVertexShader("vs_color.cso"),
		m_pResourceManager->GetPixelShader("ps_color.cso"),
		m_CameraPos, worldTM, m_ViewTM, m_ProjectionTM,
		eHELPER_TYPE::BOX, size);
}

void Renderer::RenderFace(
	bool isWire,
	const DirectX::SimpleMath::Vector3& v1,
	const DirectX::SimpleMath::Vector3& v2,
	const DirectX::SimpleMath::Vector3& v3,
	const DirectX::SimpleMath::Vector3& normal,
	const DirectX::SimpleMath::Matrix& worldTM,
	const unsigned int color, bool isOverLay)
{
	if (isOverLay == true)
		m_pDeviceContext->OMSetDepthStencilState(RasterizerState::GetDepthDisableStencilState(), 1);
	else
		m_pDeviceContext->OMSetDepthStencilState(RasterizerState::GetDepthStencilState(), 1);


	m_pVisualDebugger->GetFaceRender()->DynamicRender(
		m_pDeviceContext,
		m_pResourceManager->GetVertexShader("vs_colorN.cso"),
		m_pResourceManager->GetPixelShader("ps_colorN.cso"),
		isWire,
		v1, v2, v3, normal, worldTM, m_ViewTM, m_ProjectionTM, color);
}



HRESULT Renderer::GetDeviceAndDeviceContext(void** device, void** deviceContext)
{
	HRESULT hr;

	*device = m_pDevice.Get();
	*deviceContext = m_pDeviceContext.Get();

	if (device != nullptr || deviceContext != nullptr)
	{
		hr = E_OUTOFMEMORY;
		goto lb_return;
	}

	hr = S_OK;
lb_return:
	return hr;
}

void Renderer::SetResourceManager(std::shared_ptr<IResourceManager> pResource)
{
	m_pResourceManager = pResource;

	LoadResource();
}

void Renderer::GetScreenSize(unsigned int& clientWidth, unsigned int& clientHeight)
{
	clientWidth = m_ClientWidth;
	clientHeight = m_ClientHeight;
}

unsigned int Renderer::GetScreenWidth()
{
	return m_ClientWidth;
}

unsigned int Renderer::GetScreenHeight()
{
	return m_ClientHeight;
}

void Renderer::SetVertexShader(class VertexShader* pVS)
{

	//if (reinterpret_cast<std::uintptr_t>(pVS) == reinterpret_cast<std::uintptr_t>(m_PreVertexShader)) return;
	m_pDeviceContext->IASetInputLayout(pVS->inputLayout.Get());
	m_pDeviceContext->VSSetShader(pVS->GetVertexShader(), NULL, 0);

	//m_PreVertexShader = pVS;
}

void Renderer::SetPixelShader(class PixelShader* pPS)
{
	//if (reinterpret_cast<std::uintptr_t>(pPS) == reinterpret_cast<std::uintptr_t>(m_PrePixelShader)) return;
	m_pDeviceContext->PSSetShader(pPS->GetPixelShader(), NULL, 0);

	//m_PrePixelShader = pPS;
}

void Renderer::SetRasterizerTopology(int raster, int topology)
{
	switch (raster)
	{
	case 1: m_pDeviceContext->RSSetState(RasterizerState::GetWireFrameRS()); break;
	case 0:
	default: m_pDeviceContext->RSSetState(RasterizerState::GetSolidNormal()); break;
	}

	switch (topology)
	{
	case 1: m_pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
	case 0:
	default: m_pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);	break;
	}

}
