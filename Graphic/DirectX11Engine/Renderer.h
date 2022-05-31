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
	VS_STATICMESH, VS_SKINNEDMESH, PS_IBLDEFERRED, PS_NONE_IBLDEFERRED,			// 디퍼드
	VS_ENVIRONMENT, PS_ENVIRONMENT,
	PS_IRRADIANCEMAP, PS_PREFILTER, PS_REFLECTIONPROBE, PS_IRRADIANCEMAPVECTOR3,// IBL맵 생성
	GS_STREAMOUT, GS_DRAWPASS, VS_STREAMOUT, VS_DRAWPASS, PS_PARTICLEDRAW,		// 파티클
	VS_COMBINE, PS_OPAQUEPASS, PS_TRANSPARENTPASS, PS_COMPOSITE, PS_COLORTOACCUMEREVEAL,		// OIT
	VS_SHADOWMAP, VS_SKINNEDSHADOW, PS_SHADOWMAP,												// 그림자
	PS_LIGHTTEXTURE, VS_STATICLIGHTTEXTURE,
	VS_NORMALSKINNINGR, PS_NORMALSKINNINGR, VS_NORMALR, PS_NORMALR,							// 외곽선
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

	// 초기화-마무리
	virtual bool Initialize(int hwnd, int width, int height) override;
	virtual void Destroy() override;

	virtual void ResetRenderer() override;

	// 온리사이즈
	virtual void OnResize(int width, int height) override;

	// 렌더링 패스
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


	//피킹
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

	// 넘겨줄때 view, proj를 만들 수 있는 정보를 함께 넘겨줘야함
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
		const unsigned int probeIndex,	// probe 접근
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

	// bool 값은 CrossFading이 끝났는지를 묻는다
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
		const unsigned int overrideAnim,		// 상체 애니메이션
		unsigned int& overrideAnimKeyFrame,
		const DirectX::SimpleMath::Matrix& worldTM
	) override;

	virtual void MaskingAnimationBiLocomotoion(
		const unsigned int modelIndex,
		const unsigned int overrideAnimBase,	// 상체의 기본 애니메이션
		const unsigned int overrideAnimSub,		// 상체의 서브 애니메이션(가중치로 섞일)
		const float alpha,
		unsigned int& overrideAnimKeyFrame,
		const DirectX::SimpleMath::Matrix& worldTM
	) override;

	virtual void MaskingAnimationBiLocomotoion(
		const unsigned int modelIndex,
		const unsigned int overrideAnimBase,	// 애니메이션 하나를 둘로 쪼갠다
		const float alpha,
		const DirectX::SimpleMath::Matrix& worldTM
	) override;


	// 나중에 뭔가 추가할 건덕지가 있을까보자
	// 이를테면 현재 keyframe을 보관한다던가?
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
	//오브젝트 매니저 초기화를 위해 내보낼 필요성이 있다
	Microsoft::WRL::ComPtr<ID3D11Device>		GetDevice() { return m_pDevice; }
	Microsoft::WRL::ComPtr<ID3D11DeviceContext>	GetDeviceContext() { return m_pDeviceContext; }

	virtual HRESULT GetDeviceAndDeviceContext(void** device, void** deviceContext) override;
	virtual void SetResourceManager(std::shared_ptr<IResourceManager> pResource) override;

public:
	virtual void GetScreenSize(unsigned int& clientWidth, unsigned int& clientHeight) override;

	virtual unsigned int GetScreenWidth() override;
	virtual unsigned int GetScreenHeight() override;



private:
	// 초기화에 필요한 함수
	void CreateAdapter();
	void CreateSwapChain(UINT msaaQuality);
	void CreateRengerTarget();
	void CreateDepthAndStencil();
	void LoadResource();

private:
	void SetRasterizerTopology(int raster, int topology);

private:
	// 헬퍼 렌더
	void HelperRender();
	void SkeletonGizmoRender(
		const unsigned int modelIndex,
		const DirectX::XMMATRIX& worldTM,
		const DirectX::XMMATRIX& viewTM,
		const DirectX::XMMATRIX& projTM);

public:
	// 상수 버퍼 관련
	// 렌더큐에게 빌려줄 렌더 프로세스
	void SetReflectionProbeBuffer(int probeIndex);
	void SetMaterial(std::shared_ptr<struct NodeData>& pNodeData);
	void ReflectionProbeSetMaterial(std::shared_ptr<struct NodeData> pNodeData);

	void SetVertexShader(class VertexShader* pVS);
	void SetPixelShader(class PixelShader* pPS);
	void CommonConstantBuffer();

	void RenderProcess(
		int modelMeshIndex,
		std::shared_ptr<struct NodeData>& pData,	// 굳이 복사 안함
		const DirectX::XMMATRIX& worldTM,
		DirectX::XMMATRIX* pBoneOffsetTM);

	void RenderProcessInstance(
		int modelMeshIndex,
		Microsoft::WRL::ComPtr<ID3D11Buffer>& pInstancedBuffer,
		std::shared_ptr<struct NodeData>& pData,
		unsigned int instanceCount);

	void RenderOITProcess(
		int modelMeshIndex,
		std::shared_ptr<struct NodeData>& pData,	// 굳이 복사 안함
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
	// 초기화에 사용되는 멤버변수
	D3D_DRIVER_TYPE m_D3DDriverType;
	D3D_FEATURE_LEVEL m_eFeatureLevel;


	///스마트 포인터 : 사용이 끝난 메모리를 자동으로 해제
	// GPU와 GPU의 자원의 virtual representation을 제공.
	//주로 리소스( 버퍼나 텍스쳐 등 )의 생성에 대한 인터페이스
	Microsoft::WRL::ComPtr<ID3D11Device> m_pDevice;
	//파이프라인에 대한 그래픽처리를 나타낸다
	//리소스를 제어하고 관리하기 위한 인터페이스
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_pDeviceContext;
	//Front Buffer( 현재 화면에 보여지는 버퍼 )와 Back Buffer( 현재 연산을 해서 기록하는 버퍼 ) 를 준비해서 이것을 Flip 시키면서 번갈아 가면서 보여주는 것
	Microsoft::WRL::ComPtr<IDXGISwapChain1> m_p1Swapchain;
	//렌더링 파이프라인의 출력을 받을 자원을 연결하는데 쓰인다.

	std::shared_ptr<class Forward> m_Forward;
	std::shared_ptr<class Deferred> m_Deferred;
	std::shared_ptr<class PostProcess> m_PostProcess;
	std::shared_ptr<class DebugRender> m_DebugRender;
	std::unique_ptr<class PixelPicking> m_PixelPicker;

	std::shared_ptr<class RenderTargetView> m_pRenderTarget;
	std::shared_ptr<class RenderTargetView> m_pDebugRenderTarget;	// 디버그에 쓸 렌더타겟
	std::shared_ptr<class RenderTargetView> m_pOutLineRenderTarget;
	std::shared_ptr<class RenderTargetView> m_pIDColorRenderTarget;
	std::shared_ptr<class RenderTargetView> m_pUIRenderTarget;
	std::shared_ptr<class DepthStencilView> m_pDepthStencil;

	//ID3D11Device 인스턴스로 장면에서의 그래픽을 처리하는 GPU 자원으로 구성하고 획득하며,
	//ID3D11DeviceContext로 그래픽 파이프라인에서의 적절한 쉐이더 단계에서 자원들을 처리한다.
	//ID3D11Device 메소드는 씬을 setup하거나 device가 바뀔때 호출되지만
	//ID3D11DeviceContext는 매 프레임마다 호출한다.

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
