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
#include <vector>

enum class CBufferType
{
	VS = 0,
	GS,
	PS,
	CS
};

enum class CBufferSize
{
	SIZE_4,		SIZE_8,		SIZE_12,	SIZE_16,	SIZE_20,
	SIZE_24,	SIZE_28,	SIZE_32,	SIZE_36,	SIZE_40,
	SIZE_44,	SIZE_48,	SIZE_52,	SIZE_54,	SIZE_60,	SIZE_64,
};


class ConstantBufferManager
{
public: 

	static void Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	static void SetGeneralCBuffer(
		Microsoft::WRL::ComPtr <ID3D11DeviceContext> pDeviceContext, 
		CBufferType eType,
		CBufferSize eSize,
		UINT regNum,
		float* pData);

	static ConstantBuffer<CB_VS_WVPMatrix>* GetWVPMatrix();
	static ConstantBuffer<CB_VS_ShadowTM>* GetShadowTM();

	static ConstantBuffer<CB_VS_vertexshader>* GetVertexBuffer();
	static ConstantBuffer<CB_VS_vertexshader_2d>* GetVertexBuffer2D();
	static ConstantBuffer<CB_PS_light>* GetPSLightBuffer();
	static ConstantBuffer<CB_PS_Lights>* GetPSDirLightBuffer();
	static ConstantBuffer<CB_PS_LightCount>* GetLightCountBuffer();
	static ConstantBuffer<CB_PS_LightMatrix>* GetLightMatrixBuffer();

	static ConstantBuffer<CB_PS_Amibent>* GetPSAmbientBuffer();
	static ConstantBuffer<CB_PS_Material>* GetPSMaterial();
	static ConstantBuffer<CB_PS_ObjectID>* GetPSObjectID();

	static ConstantBuffer<CB_PS_ParticleEmissive>* GetPSParticleEmissive();
	static ConstantBuffer<CB_GS_PaticleProperty>* GetGSParticleProperty();


	static ConstantBuffer<CB_VS_WVP>* GetVSWVPBuffer();

	static ConstantBuffer<CB_VS_BoneOffset>* GetVSBoneBuffer();
	static ConstantBuffer<CB_VS_PER_OBJECT>* GetVSPerObjectBuffer();
	static ConstantBuffer<CB_PS_Camera>* GetPSCamera();
	static ConstantBuffer<CB_PS_ROUGHNESS>* GetPSRoughnessBuffer();
	static ConstantBuffer<CB_CS_Bloom>* GetCSBloomBuffer();

	static ConstantBuffer<CB_GS_Paricle_PerFrame>* GetGSParticlePerFrameBuffer();
	static ConstantBuffer<CB_GS_Particle_PerParticle>* GetGSParticleBuffer();

	static ConstantBuffer<CB_VS_NormalR>* GetVSNormalRBuffer();

	static ConstantBuffer<CB_PS_IDpicking>* GetIDpickingBuffer();



private:
	static ConstantBuffer<CB_VS_WVPMatrix>* m_vs_WVPMatrix;
	static ConstantBuffer<CB_VS_ShadowTM>* m_vs_ShadowMatrix;

	static ConstantBuffer<CB_VS_vertexshader>* m_vs_vertexshader;
	static ConstantBuffer<CB_VS_vertexshader_2d>* m_vs_vertexshader_2d;

	static ConstantBuffer<CB_PS_light>*				m_ps_light;
	static ConstantBuffer< CB_PS_LightCount>* m_ps_lightCount;
	static ConstantBuffer< CB_PS_LightMatrix>* m_ps_lightMatrix;


	static ConstantBuffer< CB_PS_Amibent>* m_ps_ambient;
	static ConstantBuffer<CB_VS_WVP>* m_vs_WVP;


	// test�� ���� �������
	static ConstantBuffer<CB_PS_Lights>*	m_ps_DirLight;
	static ConstantBuffer<CB_PS_Material>* m_ps_Material;
	static ConstantBuffer<CB_PS_ObjectID>* m_ps_ObjectID;
	
	static ConstantBuffer<CB_PS_ParticleEmissive>* m_ps_LagacyMaterial;
	static ConstantBuffer<CB_GS_PaticleProperty>* m_gs_ParticleProperty;


	static ConstantBuffer<CB_VS_PER_OBJECT>* m_vs_PerObject;
	static ConstantBuffer<CB_PS_Camera>* m_ps_Camera;
	static ConstantBuffer<CB_PS_ROUGHNESS>* m_ps_Roughness;
	static ConstantBuffer<CB_CS_Bloom>* m_cs_Blur;

	// ��Ų
	static ConstantBuffer<CB_VS_BoneOffset>* m_vs_BoneOffset;

	// OIT
	static ConstantBuffer<CB_GS_Paricle_PerFrame>* m_gs_ParticlePerFrame;

	// ��ƼŬ
	static ConstantBuffer<CB_GS_Particle_PerParticle>* m_gs_Particle;

	// 
	static ConstantBuffer<CB_VS_NormalR>* m_vs_normalR;

	static std::vector<Microsoft::WRL::ComPtr<ID3D11Buffer>> m_GeneralCBuffer_V;

	static ConstantBuffer<CB_PS_IDpicking>* m_vs_IDpicking;


};

