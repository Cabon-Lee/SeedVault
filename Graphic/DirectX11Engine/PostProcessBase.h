#pragma once

#include <memory.h>
#include "D3D11Define.h"
#include "IRenderer.h"

class VertexShader;
class PixelShader;
class ComputeShader;
class RenderTargetView;
class UAVRenderTargetView;

// IRenderOption을 쉽게 넘겨주기 위해 만든 인터페이스
class PostProcessBase
{
public:
	PostProcessBase();
	virtual ~PostProcessBase();
	virtual void ApplyFactor(const IRenderOption& option) abstract;

protected:
	void CreateBaseShader(Microsoft::WRL::ComPtr<ID3D11Device> pDevice);
	static std::unique_ptr<VertexShader> m_pFullScreenQuadVS;
	static std::unique_ptr<PixelShader> m_pFinalPassPS;
};

