//-----------------------------------------------------------------------------
// File: Scene.h
//
// Desc: Holds a description for a simple scene usend in the Order Independent
//       Transparency sample.
//
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License (MIT).
//-----------------------------------------------------------------------------
#pragma once
#include "D3D11Define.h"
#include "align16.h"
#include "Shaders.h"
#include "ConstantBuffer.h"

class TempScene
{
public:
	TempScene();

	void Initialize(Microsoft::WRL::ComPtr<ID3D11Device> pDevice, Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext);
	void D3D11Render(DirectX::CXMMATRIX mWVP, Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext);
	void OnD3D11DestroyDevice();

	std::shared_ptr<VertexShader> m_pVertexShader;
protected:
	struct VS_CB : public AlignedAllocationPolicy<16>
	{
		DirectX::XMFLOAT4X4 mWorldViewProj;
	};

	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_pVertexLayout;
	ConstantBuffer<VS_CB>* m_pVS_CB;
	ID3D11Buffer* m_pVB;
	//ID3D11VertexShader* m_pVertexShader;
	//ID3D11InputLayout* m_pVertexLayout;
	//ID3D11Buffer* m_pVS_CB;
};


