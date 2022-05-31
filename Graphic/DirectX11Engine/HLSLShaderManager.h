#pragma once

/// �������� cso�κ��� ���̴��� �ڵ����� �ε��ߴ�.
/// �� ��� HLSL�� EntryPoint�� main �ϳ��� �����ϱ� ������ HLSL�� ���ڰ� ũ�� �þ��.
/// HLSL �ϳ��� ����� �� �̻��� EntryPoint�� ����� Shader�� ����ϱ� ���� �� �Ŵ��� Ŭ������ �������.

#include "D3D11Define.h"

class PixelShader;
class VertexShader;
class ComputeShader;

class HLSLShaderManager
{
public:
	HLSLShaderManager();
	~HLSLShaderManager();

	void Initialize(Microsoft::WRL::ComPtr<ID3D11Device> pDevice);


	std::shared_ptr<PixelShader> m_DebugTestShader;

private:
	Microsoft::WRL::ComPtr<ID3D11Device> m_pDevice;


};

