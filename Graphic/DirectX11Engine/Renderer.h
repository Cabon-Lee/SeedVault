#pragma once
#include <queue>
#include <vector>
#include "ConstantBuffer.h"
#include "SimpleMath.h"
#include "IRenderer.h"
#include "EffectUIEnum.h"

#include "RenderQueue.h"
#include "PixelPicking.h"

__interface IResourceManager;
const unsigned int MAX_SHADOWMAP = 64;

enum class eSHADER
{
	VS_STATICMESH, VS_SKINNEDMESH, PS_IBLDEFERRED, PS_NONE_IBLDEFERRED,			// ���۵�
	VS_ENVIRONMENT, PS_ENVIRONMENT,
	PS_IRRADIANCEMAP, PS_PREFILTER, PS_REFLECTIONPROBE, PS_IRRADIANCEMAPVECTOR3,// IBL�� ����
	GS_STREAMOUT, GS_DRAWPASS, VS_STREAMOUT, VS_DRAWPASS, PS_PARTICLEDRAW,		// ��ƼŬ
	VS_COMBINE, PS_OPAQUEPASS, PS_TRANSPARENTPASS, PS_COMPOSITE, PS_COLORTOACCUMEREVEAL,		// OIT
	VS_SHADOWMAP, VS_SKINNEDSHADOW, PS_SHADOWMAP,												// �׸���
	PS_LIGHTTEXTURE, VS_STATICLIGHTTEXTURE,
	VS_NORMALSKINNINGR, PS_NORMALSKINNINGR, VS_NORMALR, PS_NORMALR,							// �ܰ���
	VS_OUTLINE, PS_OUTLINE,
	VS_2D, PS_2D,
	VS_BILLBOARD, PS_BILLBOARD,
	VS_IDPICKINGSKINNING, VS_IDPICKING, PS_IDPICKING,
	VS_SKYBOX, PS_SKYBOX,
	VS_COLOR, PS_COLOR, VS_COLORN, PS_COLORN,
	PS_DEFERREDDEBUG, PS_DEFERREDDEBUGINFO,
	VS_STANDARD, PS_CUBEMAPSPHERE,
	MAX_COUNT,
};

class Renderer : public IRenderer
{
public:
	Renderer();
	~Renderer();

	// �ʱ�ȭ-������
	virtual bool Initialize(int hwnd, int width, int height) override;
	virtual void Destroy() override;

	virtual void ResetRenderer() override;

	// �¸�������
	virtual void OnResize(int width, int height) override;

	// ������ �н�
	virtual void BeginDraw() override;

	virtual void RenderOptionUpdate(IRenderOption option) override;
	virtual IRenderOption GetRenderOption() override;
	virtual IRendererDebugInfo GetRenderDebugInfo() override;
	virtual void* GetDeferredInfo() override;

	virtual void CameraUpdate(
		const DirectX::XMMATRIX& worldTM,
		const DirectX::XMMATRIX& viewTM,
		const DirectX::XMMATRIX& projTM) override;

	virtual void CameraSkyBoxRender(const unsigned int textureIdx) override;

	virtual void MeshDraw(
		const unsigned int modelIndex,
		const unsigned int probeIndex,
		const unsigned int renderFlag,
		const unsigned int ObjectId,
		const DirectX::XMMATRIX& worldTM,
		DirectX::XMMATRIX* pBoneOffsetTM) override;

	virtual void RenderQueueProcess() override;


	virtual void DebugDraw(
		unsigned int rasterMode,
		const unsigned int objectID,
		const unsigned int modelIndex,
		const DirectX::XMMATRIX& worldTM,
		DirectX::XMMATRIX* pBoneOffsetTM) override;

	virtual void DeferredPickingPass() override;
	virtual void DebugQueueProcess() override;

	virtual void UIPassBind() override;


	//��ŷ
	virtual void Clicked(bool isClicked, POINT mousePos) override;

	virtual unsigned int GetClickedObjectId() override;

	virtual unsigned int  SpawnParticle(struct ParticleProperty* particle) override;
#pragma region RenderFuncByJinmi


	virtual void DrawSprite(
		eResourceType resourceType,
		unsigned int spriteIndex,
		const DirectX::XMMATRIX& worldTM) override;

	virtual void DrawSpriteEdge(const DirectX::XMMATRIX& worldTM) override;

	virtual void DrawBillboard(const unsigned int spriteIndex, const DirectX::XMMATRIX& worldTM) override;
	void DrawBillboardEdge(const DirectX::XMMATRIX& worldTM);

	virtual unsigned int FontInfoInitialize(std::shared_ptr<struct Text_Queue_Info> pFontDesc) override;
	virtual void OneResizeTextPos(unsigned int fontinfoIndex) override;
	virtual void DrawD2DText(std::shared_ptr<struct TextBlock> pTextBlock) override;

#pragma endregion

	virtual void DebugDraw(bool clear) override;
	virtual void HelperDraw() override;
	virtual void EndDraw() override;

	virtual void DrawDebugging(
		const DirectX::SimpleMath::Vector3& origin,
		const DirectX::SimpleMath::Vector3& dir,
		float distance,
		const DirectX::SimpleMath::Color& color) override;

	virtual void* GetDeviceVoid() override;
	virtual void* GetDeviceContextVoid() override;


	virtual bool LightUpdate(
		const unsigned int type,
		const DirectX::XMFLOAT3& pos,
		const DirectX::XMFLOAT3& dir,
		const DirectX::XMFLOAT3& color,
		float intensity,
		float falloffStart,
		float falloffEnd,
		float spotPower,
		bool castShadow,
		bool isTextured,
		unsigned int textureIndex) override;

	// �Ѱ��ٶ� view, proj�� ���� �� �ִ� ������ �Բ� �Ѱ������
	virtual void BeginShadow() override;
	virtual void LightTextureMap(unsigned int textureIndex) override;

	virtual void PreRenderPath(
		const unsigned int modelIndex,
		const DirectX::SimpleMath::Matrix& worldTM) override;

	virtual unsigned int AddReflectionProbe() override;
	virtual void ReflectionProbeSetWorldTM(
		const unsigned int probeIndex,
		const DirectX::SimpleMath::Matrix& probeWorldTM) override;
	virtual void ReflectionProbeRenderTargetViewBind(
		const unsigned int probeIndex,	// probe ����
		const unsigned int faceIndex) override;

	virtual void RenderToReflectionProbe(
		const unsigned int modelIndex,
		const DirectX::SimpleMath::Matrix& worldTM,
		const DirectX::SimpleMath::Matrix& viewTM,
		const DirectX::SimpleMath::Matrix& projTM) override;

	virtual void ReflectionProbeBaking(
		const unsigned int sceneIndex, const unsigned int probeIndex) override;
	virtual void SetReflectionBakedDDS(
		unsigned int probeIndex,
		unsigned int envMap, unsigned int irrMap, unsigned int filterMap)override;


	virtual void BindPose(
		const unsigned int modelIndex,
		const DirectX::SimpleMath::Matrix& worldTM) override;

	virtual bool AnimationProcess(
		const unsigned int modelIndex,
		const unsigned int animationIndex,
		unsigned int& keyFrame,
		const DirectX::SimpleMath::Matrix& worldTM,
		bool isStorePreAni,
		float offsetAngle = 0.0f) override;

	// bool ���� CrossFading�� ���������� ���´�
	virtual bool AnimationCrossFading(
		const unsigned int modelIndex,
		const unsigned int easeInAnim,
		const unsigned int easeOutAnim,
		unsigned int& easeInKeyFrame,
		unsigned int easeOutKeyFrame,
		unsigned int fadingPeriod,
		bool& fadingOver,
		const DirectX::SimpleMath::Matrix& worldTM,
		float offsetAngle = 0.0f) override;

	virtual bool MaskingAnimation(
		const unsigned int modelIndex,
		const unsigned int overrideAnim,		// ��ü �ִϸ��̼�
		unsigned int& overrideAnimKeyFrame,
		const DirectX::SimpleMath::Matrix& worldTM
	) override;

	virtual void MaskingAnimationBiLocomotoion(
		const unsigned int modelIndex,
		const unsigned int overrideAnimBase,	// ��ü�� �⺻ �ִϸ��̼�
		const unsigned int overrideAnimSub,		// ��ü�� ���� �ִϸ��̼�(����ġ�� ����)
		const float alpha,
		unsigned int& overrideAnimKeyFrame,
		const DirectX::SimpleMath::Matrix& worldTM
	) override;

	virtual void MaskingAnimationBiLocomotoion(
		const unsigned int modelIndex,
		const unsigned int overrideAnimBase,	// �ִϸ��̼� �ϳ��� �ѷ� �ɰ���
		const float alpha,
		const DirectX::SimpleMath::Matrix& worldTM
	) override;


	// ���߿� ���� �߰��� �Ǵ����� �������
	// �̸��׸� ���� keyframe�� �����Ѵٴ���?
	virtual void EndAnimation(
		const unsigned int modelIndex, DirectX::XMMATRIX* pBoneOffset) override;

	virtual void RenderSphere(float radius, const DirectX::SimpleMath::Matrix& worldTM) override;
	virtual void RenderBox(const DirectX::SimpleMath::Vector3& size, const DirectX::SimpleMath::Matrix& worldTM) override;
	virtual void RenderFace(
		bool isWire,
		const DirectX::SimpleMath::Vector3& v1,
		const DirectX::SimpleMath::Vector3& v2,
		const DirectX::SimpleMath::Vector3& v3,
		const DirectX::SimpleMath::Vector3& normal,
		const DirectX::SimpleMath::Matrix& worldTM,
		const unsigned int color, bool isOverLay = false) override;

public:
	//������Ʈ �Ŵ��� �ʱ�ȭ�� ���� ������ �ʿ伺�� �ִ�
	Microsoft::WRL::ComPtr<ID3D11Device>		GetDevice() { return m_pDevice; }
	Microsoft::WRL::ComPtr<ID3D11DeviceContext>	GetDeviceContext() { return m_pDeviceContext; }

	virtual HRESULT GetDeviceAndDeviceContext(void** device, void** deviceContext) override;
	virtual void SetResourceManager(std::shared_ptr<IResourceManager> pResource) override;

public:
	virtual void GetScreenSize(unsigned int& clientWidth, unsigned int& clientHeight) override;

	virtual unsigned int GetScreenWidth() override;
	virtual unsigned int GetScreenHeight() override;



private:
	// �ʱ�ȭ�� �ʿ��� �Լ�
	void CreateAdapter();
	void CreateSwapChain(UINT msaaQuality);
	void CreateRengerTarget();
	void CreateDepthAndStencil();
	void LoadResource();

private:
	void SetRasterizerTopology(int raster, int topology);

private:
	// ���� ����
	void HelperRender();
	void SkeletonGizmoRender(
		const unsigned int modelIndex,
		const DirectX::XMMATRIX& worldTM,
		const DirectX::XMMATRIX& viewTM,
		const DirectX::XMMATRIX& projTM);

public:
	// ��� ���� ����
	// ����ť���� ������ ���� ���μ���
	void SetReflectionProbeBuffer(int probeIndex);
	void SetMaterial(std::shared_ptr<struct NodeData>& pNodeData);
	void ReflectionProbeSetMaterial(std::shared_ptr<struct NodeData> pNodeData);

	void SetVertexShader(class VertexShader* pVS);
	void SetPixelShader(class PixelShader* pPS);
	void CommonConstantBuffer();

	void RenderProcess(
		int modelMeshIndex,
		std::shared_ptr<struct NodeData>& pData,	// ���� ���� ����
		const DirectX::XMMATRIX& worldTM,
		DirectX::XMMATRIX* pBoneOffsetTM);

	void RenderProcessInstance(
		int modelMeshIndex,
		Microsoft::WRL::ComPtr<ID3D11Buffer>& pInstancedBuffer,
		std::shared_ptr<struct NodeData>& pData,
		unsigned int instanceCount);

	void RenderOITProcess(
		int modelMeshIndex,
		std::shared_ptr<struct NodeData>& pData,	// ���� ���� ����
		const DirectX::XMMATRIX& worldTM,
		DirectX::XMMATRIX* pBoneOffsetTM);

	void RenderParticleProcess(
		unsigned int spriteIndex,
		const DirectX::XMMATRIX& worldTM);

	void RenderIDColor(
		unsigned int ObjectID,
		int modelIndex,
		std::shared_ptr<struct NodeData>& pData,
		const DirectX::XMMATRIX& worldTM,
		DirectX::XMMATRIX* pBoneOffset);

	void RenderOutLineProcess(
		int modelMeshIndex,
		std::shared_ptr<struct NodeData>& pData,
		const DirectX::XMMATRIX& worldTM,
		DirectX::XMMATRIX* pBoneOffsetTM);

	void RenderOutLine();

	void CombineRenderTarget(
		std::shared_ptr<class RenderTargetView> pDebugInfo,
		ID3D11BlendState* blendState,
		ID3D11DepthStencilState* DepthStencilState);

private:
	class AdapterManager* m_pAdapterManager;
	std::shared_ptr<IResourceManager> m_pResourceManager;
	std::unique_ptr<class VisualDebugger> m_pVisualDebugger;
	class DXTKFont* m_pFont;

	HWND m_hWnd;
	unsigned int m_ClientWidth = 0;
	unsigned int m_ClientHeight = 0;

	bool m_Enable4xMsaa;
	UINT m_4xMasaaQuality;

	IRenderOption m_RenderOption;
	IRendererDebugInfo m_RenderDebugInfo;
	std::unique_ptr<struct DeferredInfo> m_pDeferredInfo;

public:
	eRENDERRING m_Rendering;

private:
	// �ʱ�ȭ�� ���Ǵ� �������
	D3D_DRIVER_TYPE m_D3DDriverType;
	D3D_FEATURE_LEVEL m_eFeatureLevel;


	///����Ʈ ������ : ����� ���� �޸𸮸� �ڵ����� ����
	// GPU�� GPU�� �ڿ��� virtual representation�� ����.
	//�ַ� ���ҽ�( ���۳� �ؽ��� �� )�� ������ ���� �������̽�
	Microsoft::WRL::ComPtr<ID3D11Device> m_pDevice;
	//���������ο� ���� �׷���ó���� ��Ÿ����
	//���ҽ��� �����ϰ� �����ϱ� ���� �������̽�
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_pDeviceContext;
	//Front Buffer( ���� ȭ�鿡 �������� ���� )�� Back Buffer( ���� ������ �ؼ� ����ϴ� ���� ) �� �غ��ؼ� �̰��� Flip ��Ű�鼭 ������ ���鼭 �����ִ� ��
	Microsoft::WRL::ComPtr<IDXGISwapChain1> m_p1Swapchain;
	//������ ������������ ����� ���� �ڿ��� �����ϴµ� ���δ�.

	std::shared_ptr<class Forward> m_Forward;
	std::shared_ptr<class Deferred> m_Deferred;
	std::shared_ptr<class PostProcess> m_PostProcess;
	std::shared_ptr<class DebugRender> m_DebugRender;
	std::unique_ptr<class PixelPicking> m_PixelPicker;

	std::shared_ptr<class RenderTargetView> m_pRenderTarget;
	std::shared_ptr<class RenderTargetView> m_pDebugRenderTarget;	// ����׿� �� ����Ÿ��
	std::shared_ptr<class RenderTargetView> m_pOutLineRenderTarget;
	std::shared_ptr<class RenderTargetView> m_pIDColorRenderTarget;
	std::shared_ptr<class RenderTargetView> m_pUIRenderTarget;
	std::shared_ptr<class DepthStencilView> m_pDepthStencil;

	//ID3D11Device �ν��Ͻ��� ��鿡���� �׷����� ó���ϴ� GPU �ڿ����� �����ϰ� ȹ���ϸ�,
	//ID3D11DeviceContext�� �׷��� ���������ο����� ������ ���̴� �ܰ迡�� �ڿ����� ó���Ѵ�.
	//ID3D11Device �޼ҵ�� ���� setup�ϰų� device�� �ٲ� ȣ�������
	//ID3D11DeviceContext�� �� �����Ӹ��� ȣ���Ѵ�.

private:
	std::shared_ptr<class HLSLShaderManager> m_HLSLShader;
	std::shared_ptr<class LightManager> m_LightManager;
	std::unique_ptr<class ParticleManager> m_ParticleManager;

	DirectX::XMFLOAT3 m_CameraPos;
	DirectX::XMFLOAT3 m_CameraLook;

	DirectX::SimpleMath::Matrix m_CameraWorldTM;
	DirectX::XMMATRIX m_ViewTM;
	DirectX::XMMATRIX m_ProjectionTM;
	DirectX::XMMATRIX m_OrthoTM;

	std::shared_ptr<class SpriteQuad> m_SpriteQuad;
	std::shared_ptr<class D2D> m_pD2D;
	std::shared_ptr<class Particle> m_pParticle;
	std::shared_ptr<class Oneparticle> m_pOneParticle;

	std::shared_ptr<class IBL> m_pIBL;
	std::shared_ptr<class OIT> m_pOIT;

	std::shared_ptr<class SkyBoxSphere> m_pSkyBox;
	std::shared_ptr<class SkyBoxCube> m_pSkyBoxCube;

	std::shared_ptr<class VertexShader> m_InstanceStaticMesh;
	std::shared_ptr<class VertexShader> m_InstanceSkinnedMesh;

	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_StaticInput;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_SkinnedInput;

	std::shared_ptr<class RenderQueue> m_RenderQueue;
	std::unique_ptr<class FrustumVolume> m_FrustumVolume;

	POINT m_MousePos;
	bool m_isClicked;
	bool m_IsSelectedAny;
	unsigned int m_ClickedObjectId;
	float m_NearestDist;

	std::shared_ptr<class VertexShader> m_pCombineVertexShader;
	std::shared_ptr<class PixelShader> m_pCombinePixelShader;

	static std::vector<void*> m_ShaderIndex_V;
	class VertexShader* m_PreVertexShader;
	class PixelShader* m_PrePixelShader;

	std::unique_ptr<StagingBuffer<DeferredPixel>> m_DeferredPixelData;

};
