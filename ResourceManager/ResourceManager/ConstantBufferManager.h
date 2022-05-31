#pragma once

/// 렌더 패스에 넘겨줄 다양한 상수버퍼들을 관리해주는 클래스
/// 만든 이유는 다음과 같다
/// 헬퍼(그리드 등) 추가 이후, 그림을 그릴 때 해당 클래스의 내부에서 처리를 하고 싶었다.
/// 이때, 헬퍼 자신을 그리기 위한 상수 버퍼를 넘겨줄 필요가 생겼다.
/// 현재 구조에서 해당 함수가 호출될 때 인자로 상수버퍼를 넘겨줘야하게 되었는데,
/// 그렇게 될 경우 원치 않게 헬퍼의 헤더가 상수버퍼의 헤더를 참조하는 경우가 발생했다.
/// 이 구조를 어떻게 할지 생각을 하다가 차라리 전역적으로 접근 가능한 클래스로 처리하자는 아이디어로 발전
/// RasterizerState도 이와 같은 형식으로 사용중에 있다.

#include "ConstantBuffer.h"

class ConstantBufferManager
{
public: 

	static void Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);

	static ConstantBuffer<CB_VS_vertexshader>* GetVertexBuffer();
	static ConstantBuffer<CB_PS_light>* GetPSLightBuffer();
	static ConstantBuffer<CB_PS_DirectionalLight>* GetPSDirLightBuffer();
	static ConstantBuffer<CB_PS_Material>* GetPSMaterial();
	static ConstantBuffer<CB_VS_PER_OBJECT>* GetVSPerObjectBuffer();
	static ConstantBuffer<CB_PS_PER_OBJECT>* GetPSPerObjectBuffer();
	static ConstantBuffer<CB_VS_BoneOffset>* GetVSBoneBuffer();

private:
	static ConstantBuffer<CB_VS_vertexshader>*		m_vs_vertexshader;
	static ConstantBuffer<CB_PS_light>*				m_ps_light;

	// test를 위한 상수버퍼
	static ConstantBuffer<CB_PS_DirectionalLight>*	m_ps_DirLight;
	static ConstantBuffer<CB_PS_Material>* m_ps_Material;
	static ConstantBuffer<CB_VS_PER_OBJECT>* m_vs_PerObject;
	static ConstantBuffer<CB_PS_PER_OBJECT>* m_ps_PerObject;

	// 스킨
	static ConstantBuffer<CB_VS_BoneOffset>* m_vs_BoneOffset;
};

