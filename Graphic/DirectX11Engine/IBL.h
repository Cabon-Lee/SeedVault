#pragma once

#include "D3D11Define.h"
#include "SimpleMath.h"
#include <thread>

class CubeMapRenderTarget;
class DepthStencilView;
class RenderTargetView;

class VertexShader;
class PixelShader;

class SkyBoxCube;
class Quad;


const unsigned int PRE_FILTER_SIZE = 512;
const unsigned int BRDF_LOOKUP = 512;

const std::string ENV_PATH = "../Data/IBLData/";
const std::string IRR_PATH = "../Data/IBLData/";
const std::string FILTER_PATH = "../Data/IBLData/";

const std::string ENV_MAP = "_EnvironmentMap_";
const std::string IRR_MAP = "_IrradianceMap_";
const std::string FILTER_MAP = "_PreFilterMap_";

class ProbeHelper
{
public:
	ProbeHelper() 
		: m_VertexBuffer(nullptr), m_IndexBuffer(nullptr)
		, m_Stride(0), m_IndexCount(0){};
	~ProbeHelper() {};

	Microsoft::WRL::ComPtr<ID3D11Buffer> m_VertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_IndexBuffer;
	unsigned int m_Stride;
	unsigned int m_IndexCount;
};

class ReflectionProbe
{
public:
	ReflectionProbe();
	~ReflectionProbe();

	void Initialize(Microsoft::WRL::ComPtr<ID3D11Device> pDevice);
	void SetTexture(Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pEnviormentTexture);

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& GetTextrue();

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> GetEnvironmentMap();
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> GetIrradianceMap();
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> GetPreFilterMap();

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& GetEnvironmentTexture();
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& GetIrradianceTexture();
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& GetPreFilterTexture();

	void SetEnvironmentMap(Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pTexture);
	void SetIrradianceMap(Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pTexture);
	void SetPreFilterMap(Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pTexture);

	std::shared_ptr<CubeMapRenderTarget>& GetEnviormentCubeRenderTarget();
	std::shared_ptr<CubeMapRenderTarget>& GetIrrdaianceCubeRenderTarget();
	std::shared_ptr<CubeMapRenderTarget>& GetPreFilterCubeRenderTarget();

	std::shared_ptr<ProbeHelper> GetCube();
	std::shared_ptr<ProbeHelper> GetSphere();

	void SetBakedMapIndex(unsigned int envMap, unsigned int irrMap, unsigned int filterMap);

	const DirectX::SimpleMath::Matrix& GetViewTM(unsigned int idx);
	const DirectX::SimpleMath::Matrix& GetProjTM();
	const DirectX::SimpleMath::Matrix& GetProbeWorldTM();
	void SetViewTM(unsigned int idx, const DirectX::SimpleMath::Matrix& viewTM);
	void SetProjTM(const DirectX::SimpleMath::Matrix& projTM);
	void SetProbeWorldTM(const DirectX::SimpleMath::Matrix& worldTM);

	unsigned int m_BakingTimer;

	unsigned int GetIrradianceTextureIndex();
	unsigned int GetPreFilterTextureIndex();

private:
	// 1x1 ������ ť�긦 ����� ��ī�� �ڽ��� ������
	// 1x1 �������� ���� ����� �������� �����ϱ� ���ؼ�
	void BuildHelper(Microsoft::WRL::ComPtr<ID3D11Device> pDevice);
	void BuildCubeMap(Microsoft::WRL::ComPtr<ID3D11Device> pDevice,
		UINT width, UINT height, UINT renderTargetCount, UINT mipLevel,
		std::shared_ptr<CubeMapRenderTarget>& pCubeMap);


private:
	std::shared_ptr<CubeMapRenderTarget> m_pEnvironmentMap;
	std::shared_ptr<CubeMapRenderTarget> m_pIrrdianceMap;
	std::shared_ptr<CubeMapRenderTarget> m_pPreFilterMap;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_pEnviormentTexture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_pIrradianceTexture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_pPreFilterTexture;


	DirectX::SimpleMath::Matrix m_ViewTM_Arr[6];
	DirectX::SimpleMath::Matrix m_ProjTM;
	DirectX::SimpleMath::Matrix m_ProbeWorldTM;

	std::shared_ptr<ProbeHelper> m_Cube;
	std::shared_ptr<ProbeHelper> m_Sphere;

	unsigned int m_BakedMapIndex[3];
};


/// IBL�� �ϳ��� ���Ǵ°� �ƴ� ���÷��� ���κ긦 ���� �� �ֵ���
/// CubeMap���� �ϳ��� �������� ������ ���� �ʿ䰡 �ְڴ�

class IBL
{
public:
	IBL();
	~IBL();

	void Initialize(Microsoft::WRL::ComPtr <ID3D11Device> pDevice);

	// ���÷��� ���κ� �߰�
	unsigned int AddReflectionProbe(Microsoft::WRL::ComPtr <ID3D11Device> pDevice);	
	// ������� ���÷����� �ε��� ���·� ����
	bool IsProbeExist();
	ReflectionProbe* GetReflectionProbe(unsigned int idx);
	ReflectionProbe* GetBasicIBL();

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> GetIrradianceMapSRV(unsigned int idx);
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> GetPrefilterMapSRV(unsigned int idx);

	// �ε��� ���·� Ư�� ���κ긦 �ʱ�ȭ�Ͽ� �ٽ� ����ŷ�ϵ�����
	// �̶� �ؽ��ĸ� ���������ν� �ش� �ؽ�ó�� �ٽ� ����ŷ�ϰ� �Ѵ�
	void ResetReflectionProbe(unsigned int idx, 
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pEnviormentTexture);

	void ReflectionProbeRenderTargetViewBind(
		Microsoft::WRL::ComPtr <ID3D11DeviceContext> pDeviceContext,
		const unsigned int probeIndex, const unsigned int faceIndex, ID3D11DepthStencilView* pDepth);

	void BakeReflectionProbe(
		Microsoft::WRL::ComPtr <ID3D11Device> pDevice,
		Microsoft::WRL::ComPtr <ID3D11DeviceContext> pDeviceContext, 
		std::shared_ptr<VertexShader> vertexShader,
		std::shared_ptr<PixelShader> pIrradianceShader,
		std::shared_ptr<PixelShader> pBakePixelShader,
		unsigned int sceneIndex,
		const std::string& sceneName,
		unsigned int reflectionProbeIndex);

	bool BasicIrradiancePreFilterMap(
		Microsoft::WRL::ComPtr <ID3D11Device> pDevice,
		Microsoft::WRL::ComPtr <ID3D11DeviceContext> pDeviceContext,
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pEnviormentTexture,
		class VertexShader* vertexShader,
		class PixelShader* pEnvironmentShader,
		class PixelShader* pIrradianceShader,
		class PixelShader* pBakePixelShader,
		unsigned int reflectionProbeIndex);

	void RenderProbe(
		Microsoft::WRL::ComPtr <ID3D11DeviceContext> pDeviceContext,
		class VertexShader* pVertexShader,
		class PixelShader* pPixelShader,
		const DirectX::SimpleMath::Matrix& viewTM,
		const DirectX::SimpleMath::Matrix& projTM);

	void ClearReflectionProbe();

private:
	std::vector<std::unique_ptr<ReflectionProbe>> m_ReflectionProbe_V;
	std::unique_ptr<ReflectionProbe> m_BaseReflectionProbe;

};



