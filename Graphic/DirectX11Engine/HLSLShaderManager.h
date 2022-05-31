#pragma once

/// 기존에는 cso로부터 쉐이더를 자동으로 로드했다.
/// 이 경우 HLSL에 EntryPoint로 main 하나만 존재하기 때문에 HLSL의 숫자가 크게 늘어난다.
/// HLSL 하나를 만들고 둘 이상의 EntryPoint를 사용해 Shader를 사용하기 위해 이 매니저 클래스를 만들었다.

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

