#pragma once

#include <memory>
#include <string>
#include <windows.h>
#include <vector>

#define GIZMO_ON true
#define GIZMO_OFF false

struct SpriteData;
struct Text_Queue_Info;
struct TextBlock;
struct ParticleInfo;

enum class eSpriteType;
enum class eResourceType;
enum class eFrameRate;

enum class eRENDERRING
{
	FORWARD = 0,
	DEFERRED,
};

namespace DirectX
{
	struct XMMATRIX;
	struct XMFLOAT4;
	struct XMFLOAT3;
	struct XMFLOAT2;
	struct XMVECTORF32;

	class SpriteBatch;
	class SpriteFont;

	namespace SimpleMath
	{
		struct Vector3;
		struct Matrix;
		struct Color;
	}
}

__interface IResourceManager;
class MaterialData;
struct GraphicMaterial;

// 여러가지 변수를 주고 받을 때 함수로 주고 받는게 싫어서 만들었다
// 렌더러 옵션과 관련된 변수를 넘길때 여기 추가하는 식으로 사용해보자
struct IRenderOption
{
	IRenderOption()
		: dTime(0.0f)
		, isDeferredDebug(false)
		, isHDRRender(false), isAdaptation(false)
		, isDownSampling(false), isBlur(false)
		, isBloom(true)
		, hdrWhite(5.0f), hdrMiddleGrey(12.0f), adaptation(0.0f)
		, isBoomFilter(false), isColorAdjust(false)
		, bloomScale(10), bloomThreshold(0.1f), intensity(0.2f), adjustFactor(1.0f)
		, sampleScale(0), blurCount(0)
		, isQT(false)
	{};

	float dTime;

	bool isDeferredDebug;
	bool isHDRRender;
	bool isAdaptation;
	bool isDownSampling;
	bool isBlur;
	bool isBloom;

	float hdrWhite;
	float hdrMiddleGrey;
	float adaptation;

	bool isBoomFilter;
	bool isColorAdjust;
	int bloomScale;
	float bloomThreshold;
	float intensity;
	float adjustFactor;

	int sampleScale;

	int blurCount;

	bool isQT;
};


struct IRendererDebugInfo
{
	unsigned int renderedObject;
	unsigned int opaqueObejct;
	unsigned int transparentObejct;
	unsigned int particleObject;
	unsigned int culledObject;
};

// 변수로 전달 받았는데 너무 많아졌다
struct ParticleProperty
{
	unsigned int textureIndex;		// 어떤 텍스쳐인지
	DirectX::XMFLOAT3* emitterPos;	// 방출기 생성 위치
	DirectX::XMFLOAT3* emitDir;		// 방출 방향
	DirectX::XMFLOAT2* Particlesize;// 파티클 최초 크기
	float dirRandomWeight;			// 방출 방향 랜덤값 가중치
	float constForce;				// 중력 방향
	float constZrot;				// z축 회전값
	float deltaAlpha;				// 시간에 따른 알파값
	float deltaVelo;				// 시간에 따른 속도
	float deltaSize;				// 시간에 따른 크기 
	float deltaSizeWeight;			// 시간에 따른 크기 랜덤 가중치
	float deltaRot;					// 시간에 따른 회전값
	float deltaRotWeight;			// 시간에 따른 크기 랜덤 가중치
	float birthTime;				// 생성 주기(짧을 수록 많이 생성)
	float deadTime;					// 사라지는 주기
	float stopTime;					// 방출을 멈추는 시간
	UINT particeCount;				// 동시에 방출할 파티클 수
};

struct DeferredInfo
{
	DirectX::XMFLOAT4* postion;	// 커서가 위치한 곳의 월드 위치
	DirectX::XMFLOAT3* normal;	// 커서가 위치한 곳의 노말
	unsigned int* objectID;		// 오브젝트 ID
};


__interface IRenderer
{
public:
	virtual bool Initialize(int hwnd, int width, int height) abstract;
	virtual void Destroy() abstract;
	virtual void BeginDraw() abstract;
	virtual void DebugDraw(bool clear) abstract;
	virtual void HelperDraw() abstract;

	virtual void EndDraw() abstract;

	virtual void ResetRenderer() abstract;

	virtual void OnResize(int width, int height) abstract;

	// 렌더 옵션을 게임 엔진에서 갱신해준다
	// 현재는 Imgui에서 해줄 예정이다
	virtual void RenderOptionUpdate(IRenderOption option) abstract;

	// 반대로 렌더러는 IRenderOption을 멤버변수로 갖는다
	// 그래서 const Reference로 했다
	virtual IRenderOption GetRenderOption()abstract;
	virtual IRendererDebugInfo GetRenderDebugInfo() abstract;
	virtual void* GetDeferredInfo() abstract;

	virtual void CameraUpdate(
		const DirectX::XMMATRIX& worldTM,
		const DirectX::XMMATRIX& viewTM,
		const DirectX::XMMATRIX& projTM) abstract;

	virtual void CameraSkyBoxRender(const unsigned int textureIdx) abstract;


	// 라이트의 위치값과 현재 카메라의 위치값을 통해서 이 라이트 연산을 할지 말지 Boolean으로 반환한다
	virtual bool LightUpdate(
		const unsigned int type,
		const DirectX::XMFLOAT3& pos,
		const DirectX::XMFLOAT3& dir,
		const DirectX::XMFLOAT3& color,
		float intensity,
		float falloffStart = 0.0f,
		float falloffEnd = 0.0f,
		float spotPower = 0.0f,
		bool castShadow = false,
		bool isTextured = false,
		unsigned int textureIndex = 0) abstract;

	virtual void MeshDraw(
		const unsigned int modelIndex,
		const unsigned int probeIndex,
		const unsigned int renderFlag,
		const unsigned int ObjectId,
		const DirectX::XMMATRIX& worldTM,
		DirectX::XMMATRIX* pBoneOffsetTM) abstract;
	virtual void RenderQueueProcess() abstract;

	virtual void DebugDraw(
		unsigned int rasterMode,
		const unsigned int objectID,
		const unsigned int modelIndex,
		const DirectX::XMMATRIX& worldTM,
		DirectX::XMMATRIX* pBoneOffsetTM) abstract;
	virtual void DeferredPickingPass() abstract;
	virtual void DebugQueueProcess() abstract;
	virtual void UIPassBind() abstract;

	virtual unsigned int  SpawnParticle(ParticleProperty* particle) abstract;

	// 피킹
	virtual void Clicked(bool IsClicked, POINT mousePos) abstract;

	virtual unsigned int GetClickedObjectId() abstract;

	virtual void DrawSprite(
		eResourceType resourceType,
		unsigned int spriteIndex,
		const DirectX::XMMATRIX& worldTM) abstract;

	virtual void DrawSpriteEdge(const DirectX::XMMATRIX& worldTM) abstract;

	virtual void DrawBillboard(const unsigned int modelIndex, const DirectX::XMMATRIX& worldTM) abstract;

	virtual unsigned int FontInfoInitialize(std::shared_ptr<Text_Queue_Info> pFontDesc) abstract;
	virtual void OneResizeTextPos(unsigned int fontinfoIndex) abstract;
	virtual void DrawD2DText(std::shared_ptr<TextBlock> pTextBlock) abstract;

	virtual void DrawDebugging(
		const DirectX::SimpleMath::Vector3& origin,
		const DirectX::SimpleMath::Vector3& dir,
		float distance,
		const DirectX::SimpleMath::Color& color) abstract;

	virtual void BeginShadow() abstract;
	virtual void LightTextureMap(unsigned int textureIndex) abstract;


	virtual void PreRenderPath(
		const unsigned int modelIndex,
		const DirectX::SimpleMath::Matrix& worldTM) abstract;

	virtual unsigned int AddReflectionProbe() abstract;
	virtual void ReflectionProbeSetWorldTM(
		const unsigned int probeIndex,
		const DirectX::SimpleMath::Matrix& probeWorldTM) abstract;
	virtual void ReflectionProbeRenderTargetViewBind(
		const unsigned int probeIndex,	// probe 접근
		const unsigned int faceIndex) abstract;

	virtual void RenderToReflectionProbe(
		const unsigned int modelIndex,
		const DirectX::SimpleMath::Matrix& worldTM,
		const DirectX::SimpleMath::Matrix& viewTM,
		const DirectX::SimpleMath::Matrix& projTM) abstract;

	virtual void ReflectionProbeBaking(
		const unsigned int sceneIndex, const unsigned int probeIndex) abstract;

	virtual void SetReflectionBakedDDS(
		unsigned int probeIndex,
		unsigned int envMap, unsigned int irrMap, unsigned int filterMap)abstract;

	virtual void BindPose(const unsigned int modelIndex, const DirectX::SimpleMath::Matrix& worldTM) abstract;

	// 애니메이션이 끝났는지 확인하기 위해 bool을 리턴한다
	virtual bool AnimationProcess(
		const unsigned int modelIndex,
		const unsigned int animationIndex,
		unsigned int& keyFrame,
		const DirectX::SimpleMath::Matrix& worldTM,
		bool isStorePreAni,
		float offsetAngle = 0.0f) abstract;

	virtual bool AnimationCrossFading(
		const unsigned int modelIndex,
		const unsigned int easeInAnim,
		const unsigned int easeOutAnim,
		unsigned int& easeInKeyFrame,
		unsigned int easeOutKeyFrame,
		unsigned int fadingPeriod,
		bool& fadingOver,
		const DirectX::SimpleMath::Matrix& worldTM,
		float offsetAngle = 0.0f) abstract;

	virtual bool MaskingAnimation(
		const unsigned int modelIndex,
		const unsigned int overrideAnim,
		unsigned int& overrideAnimKeyFrame,
		const DirectX::SimpleMath::Matrix& worldTM
	) abstract;

	// 로코모션 애니메이션의 경우에는 끝난다는 개념이 없다
	virtual void MaskingAnimationBiLocomotoion(
		const unsigned int modelIndex,
		const unsigned int overrideAnimBase,
		const unsigned int overrideAnimSub,
		const float alpha,
		unsigned int& overrideAnimKeyFrame,
		const DirectX::SimpleMath::Matrix& worldTM
	) abstract;

	virtual void MaskingAnimationBiLocomotoion(
		const unsigned int modelIndex,
		const unsigned int overrideAnimBase,	// 애니메이션 하나를 둘로 쪼갠다
		const float alpha,
		const DirectX::SimpleMath::Matrix& worldTM
	) abstract;

	virtual void EndAnimation(const unsigned int modelIndex, DirectX::XMMATRIX* pBoneOffset) abstract;

	// IMGUI 초기화를 위한 Device, DeviceContext 전달
	// 다만, Engine에 DX를 까지 않기 위한 최소한의 방침으로 void*로 전달
	virtual void* GetDeviceVoid() abstract;
	virtual void* GetDeviceContextVoid() abstract;
	virtual HRESULT GetDeviceAndDeviceContext(void** device, void** deviceContext) abstract;

	virtual void SetResourceManager(std::shared_ptr<IResourceManager> pResource) abstract;

	virtual void GetScreenSize(unsigned int& m_ClientWidth, unsigned int& m_ClientHeight) abstract;

	virtual unsigned int GetScreenWidth() abstract;
	virtual unsigned int GetScreenHeight() abstract;

	virtual void RenderSphere(float radius, const DirectX::SimpleMath::Matrix& worldTM) abstract;
	virtual void RenderBox(const DirectX::SimpleMath::Vector3& size, const DirectX::SimpleMath::Matrix& worldTM) abstract;
	virtual void RenderFace(
		bool isWire,
		const DirectX::SimpleMath::Vector3& v1,
		const DirectX::SimpleMath::Vector3& v2,
		const DirectX::SimpleMath::Vector3& v3,
		const DirectX::SimpleMath::Vector3& normal,
		const DirectX::SimpleMath::Matrix& worldTM,
		const unsigned int color,
		bool isOverLay = false) abstract;


};