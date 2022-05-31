#pragma once

/// ���� �н��� �Ѱ��� �پ��� ������۵��� �������ִ� Ŭ����
/// ���� ������ ������ ����
/// ����(�׸��� ��) �߰� ����, �׸��� �׸� �� �ش� Ŭ������ ���ο��� ó���� �ϰ� �;���.
/// �̶�, ���� �ڽ��� �׸��� ���� ��� ���۸� �Ѱ��� �ʿ䰡 �����.
/// ���� �������� �ش� �Լ��� ȣ��� �� ���ڷ� ������۸� �Ѱ�����ϰ� �Ǿ��µ�,
/// �׷��� �� ��� ��ġ �ʰ� ������ ����� ��������� ����� �����ϴ� ��찡 �߻��ߴ�.
/// �� ������ ��� ���� ������ �ϴٰ� ���� ���������� ���� ������ Ŭ������ ó�����ڴ� ���̵��� ����
/// RasterizerState�� �̿� ���� �������� ����߿� �ִ�.

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

	// test�� ���� �������
	static ConstantBuffer<CB_PS_DirectionalLight>*	m_ps_DirLight;
	static ConstantBuffer<CB_PS_Material>* m_ps_Material;
	static ConstantBuffer<CB_VS_PER_OBJECT>* m_vs_PerObject;
	static ConstantBuffer<CB_PS_PER_OBJECT>* m_ps_PerObject;

	// ��Ų
	static ConstantBuffer<CB_VS_BoneOffset>* m_vs_BoneOffset;
};

