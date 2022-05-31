#pragma once

#include <memory>
#include <wrl.h>

#include "D3D11Define.h"
#include "IRenderer.h"

struct ID3D11Device;
struct ID3D11DeviceContext;
struct ID3D11ShaderResourceView;

class VertexShader;
class PixelShader;
class RenderTargetView;
class UAVRenderTargetView;
class OffScreenRenderTarget;
class HighDynamicRangeRender;
class Adaptation;
class DownSampler;
class BlurFilter;
class BungieBloom;
class HLSLBloom;
class BloomFilter;
class Sampler;

//#define BUNGIE_DOWNSAMPLE

enum class ePOST_PROCESS
{
	NONE = 0,
	HDR,
	ADAPTATION,
	DOWNSAMPLE,
	BLUR,
	BLOOM,
};

const UINT FULL_SCREEN = 1024;

class PostProcess
{
public:
	PostProcess();
	~PostProcess();

	void Initialize(
		Microsoft::WRL::ComPtr<ID3D11Device> pDevice, UINT width, UINT height);

	void ApplyPostProcess(
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext,
		std::shared_ptr<RenderTargetView> pRenderTarget,
		std::shared_ptr<RenderTargetView> pSwapChainRenderTarget,
		ePOST_PROCESS process);

	void ApplyFXAA(Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext,
		std::shared_ptr<RenderTargetView> pSwapChainRenderTarget, UINT width, UINT height);

	void OnResize(Microsoft::WRL::ComPtr<ID3D11Device> pDevice, UINT width, UINT height);

	void RenderOption(IRenderOption& option);

private:
	Microsoft::WRL::ComPtr<ID3D11Device> m_pDevice;

private:
	std::shared_ptr<OffScreenRenderTarget> m_OffScreen;
	std::shared_ptr<HighDynamicRangeRender> m_HDRRender;
	std::shared_ptr<Adaptation> m_Adaptation;
	std::shared_ptr<BlurFilter> m_BlurFilter;
	std::shared_ptr<BungieBloom> m_BungieBloom;
	std::shared_ptr<HLSLBloom> m_HLSLBloom;
	std::shared_ptr<BloomFilter> m_BloomFilter;

	std::shared_ptr<UAVRenderTargetView> m_FirstBlurBuffer;
	std::shared_ptr<UAVRenderTargetView> m_SecondBlurBuffer;
	std::shared_ptr<UAVRenderTargetView> m_ThirdBlurBuffer;

	std::shared_ptr<UAVRenderTargetView> m_FirstBuffer;
	std::shared_ptr<UAVRenderTargetView> m_SecondBuffer;

	std::shared_ptr<Sampler> m_Sampler;

	std::shared_ptr<DownSampler> m_DownSampler;
	std::shared_ptr<DownSampler> m_UpSampler;
	std::unique_ptr<class FXAA> m_FXAA;

	unsigned int m_SampleScale;

	void NonePostProcess(
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext, 
		std::shared_ptr<RenderTargetView> pRenderTarget);
	std::shared_ptr<VertexShader> m_NonePostProcessVertexShader;
	std::shared_ptr<PixelShader> m_NonePostProcessPixelShader;

	IRenderOption m_NowOption;

};

