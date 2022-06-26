#pragma once

/// 210618
/// �����Ͷ����� ������Ʈ���� ����ƽ���� �����ߴ�.
/// ������ �Ҷ� �����Ӱ� �����پ��� �����̴�.
/// ���ҽ� �Ŵ����� �ְ� �����ұ��ϱ� ������, ����ƽ�� �� ���� �� ����.

#include "D3D11Define.h"

class RasterizerState
{
public:
	RasterizerState();
	~RasterizerState();

private:
	// �޽��� ��� �׸���
	static ID3D11RasterizerState* m_WireFrameRS;
	static ID3D11RasterizerState* m_SolidRasterizeStateFCC;
	static ID3D11RasterizerState* m_SolidRasterizeStateNormal;
	static ID3D11RasterizerState* m_NoCullingRS;
	static ID3D11RasterizerState* m_DepthRasterizerState;


	// �ؽ��ĸ� ��� ȥ������
	static ID3D11BlendState* m_AlphaToCoverageBS;
	static ID3D11BlendState* m_TransparentBS;
	static ID3D11BlendState* m_pBlenderState;
	static ID3D11BlendState* m_alphaEnableBlendingState;
	static ID3D11BlendState* m_outLineBlendingState;
	static ID3D11BlendState* m_DestZeroBlendingState;
	static ID3D11BlendState* m_DeferredBlendState;
	static ID3D11BlendState* m_alphaEnableBlendingStateUI;


	// ���̴��� �Ѱ��� ���÷� ������Ʈ
	static ID3D11SamplerState* m_pLinearSample;
	static ID3D11SamplerState* m_pTriLinearSaple;
	static ID3D11SamplerState* m_pPointSample;
	static ID3D11SamplerState* m_pAnisotropicSample;	// ���漺
	static ID3D11SamplerState* m_pPCFSample;
	static ID3D11SamplerState* m_pOutLineSample;
	static ID3D11SamplerState* m_pSSAOSample;
	static ID3D11SamplerState* m_pSSAO2Sample;

	// DepthStencilState
	static ID3D11DepthStencilState* m_NormalDSS;
	static ID3D11DepthStencilState* m_DepthStencilState;
	static ID3D11DepthStencilState* m_DepthDisabledStencilState;
	static ID3D11DepthStencilState* m_ComparisonLessEqual;
	static ID3D11DepthStencilState* m_DisabledDepthStencilState;

	static ID3D11RasterizerState* m_NowRasterizer;
	static ID3D11RasterizerState* m_PrevRasterizer;

	/// OIT ����
	static ID3D11BlendState* m_OITInitBS;
	static ID3D11RasterizerState* m_OITInitRS;
	static ID3D11DepthStencilState* m_OITInitDS;

	static ID3D11BlendState* m_TransparentSet;



public:
	static void CreateRasterizerState(ID3D11Device* device);
	static void DestroyRasterizerState();

	static ID3D11RasterizerState* GetWireFrameRS() { return m_WireFrameRS; }
	static ID3D11RasterizerState* GetSolidRS() { return m_SolidRasterizeStateFCC; }
	static ID3D11RasterizerState* GetSolidNormal() { return m_SolidRasterizeStateNormal; }
	static ID3D11RasterizerState* GetNoCullingRS() { return m_NoCullingRS; }
	static ID3D11RasterizerState* GetDepthRS() { return m_DepthRasterizerState; }

	static ID3D11DepthStencilState* GetNormalDSS() { return m_NormalDSS; }
	static ID3D11DepthStencilState* GetDepthStencilState() { return m_DepthStencilState; }
	static ID3D11DepthStencilState* GetDepthDisableStencilState() { return m_DepthDisabledStencilState; }
	static ID3D11DepthStencilState* GetComparisonLessEqual() { return m_ComparisonLessEqual; }
	static ID3D11DepthStencilState* GetDisabledDSS() { return m_DisabledDepthStencilState; }

	static ID3D11BlendState* GetBlenderState() { return m_pBlenderState; }
	static ID3D11BlendState* GetAlphaBlenderState() { return m_alphaEnableBlendingState; }
	static ID3D11BlendState* GetTransparentState() { return m_TransparentBS; }
	static ID3D11BlendState* GetDeferredBlendState() { return m_DeferredBlendState; }
	static ID3D11BlendState* GetAlphaBlenderStateUI() { return m_alphaEnableBlendingStateUI; }

	static ID3D11SamplerState* GetLinearSamplerState() { return m_pLinearSample; }
	static ID3D11SamplerState* const* GetLinearSamplerStateAddressOf() { return &m_pLinearSample; }

	static ID3D11SamplerState* GetTirLinearSamplerState() { return m_pTriLinearSaple; }
	static ID3D11SamplerState* const* GetTirLinearSamplerStateAddressOf() { return &m_pTriLinearSaple; }

	static ID3D11SamplerState* GetAnisotropicSamplerState() { return m_pAnisotropicSample; }
	static ID3D11SamplerState* const* GetAnisotropicSamplerStateAddressOf() { return &m_pAnisotropicSample; }

	static ID3D11SamplerState* GetPCFSamplerState() { return m_pPCFSample; }
	static ID3D11SamplerState* const* GetPCFSamplerStateAddressOf() { return &m_pPCFSample; }

	static ID3D11SamplerState* GetPointSamplerState() { return m_pPointSample; }
	static ID3D11SamplerState* const* GetPointSamplerStateAddressOf() { return &m_pPointSample; }

	static ID3D11SamplerState* GetOutLineSamplerState() { return m_pOutLineSample; }
	static ID3D11SamplerState* const* GetOutLineSamplerStateAddressOf() { return &m_pOutLineSample; }

	static ID3D11SamplerState* GetSSAOSamplerState() { return m_pSSAOSample; }
	static ID3D11SamplerState* const* GetSSAOSamplerStateAddressOf() { return &m_pSSAOSample; }
	
	static ID3D11SamplerState* GetSSAO2SamplerState() { return m_pSSAO2Sample; }
	static ID3D11SamplerState* const* GetSSAO2SamplerStateAddressOf() { return &m_pSSAO2Sample; }

	static ID3D11RasterizerState* GetNowRasterizerState() { return m_NowRasterizer; }
	static void ChangeRasterizerState();

	/// OIT ����
	static ID3D11BlendState* GetOITInitBS() { return m_OITInitBS; }
	static ID3D11RasterizerState* GetOITInitRS() { return m_OITInitRS; }
	static ID3D11DepthStencilState* GetOITInitDS() { return m_OITInitDS; }

	static ID3D11BlendState* GetTransparentSetState() { return m_TransparentSet; }
	static ID3D11BlendState* GetOutLineBlendState() { return m_outLineBlendingState; }
	static ID3D11BlendState* GetDestZeroBlendState() { return m_DestZeroBlendingState; }

};

