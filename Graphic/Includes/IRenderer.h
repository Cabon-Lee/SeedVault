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

// �������� ������ �ְ� ���� �� �Լ��� �ְ� �޴°� �Ⱦ �������
// ������ �ɼǰ� ���õ� ������ �ѱ涧 ���� �߰��ϴ� ������ ����غ���
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

// ������ ���� �޾Ҵµ� �ʹ� ��������
struct ParticleProperty
{
	unsigned int textureIndex;		// � �ؽ�������
	DirectX::XMFLOAT3* emitterPos;	// ����� ���� ��ġ
	DirectX::XMFLOAT3* emitDir;		// ���� ����
	DirectX::XMFLOAT2* Particlesize;// ��ƼŬ ���� ũ��
	float dirRandomWeight;			// ���� ���� ������ ����ġ
	float constForce;				// �߷� ����
	float constZrot;				// z�� ȸ����
	float deltaAlpha;				// �ð��� ���� ���İ�
	float deltaVelo;				// �ð��� ���� �ӵ�
	float deltaSize;				// �ð��� ���� ũ�� 
	float deltaSizeWeight;			// �ð��� ���� ũ�� ���� ����ġ
	float deltaRot;					// �ð��� ���� ȸ����
	float deltaRotWeight;			// �ð��� ���� ũ�� ���� ����ġ
	float birthTime;				// ���� �ֱ�(ª�� ���� ���� ����)
	float deadTime;					// ������� �ֱ�
	float stopTime;					// ������ ���ߴ� �ð�
	UINT particeCount;				// ���ÿ� ������ ��ƼŬ ��
};

struct DeferredInfo
{
	DirectX::XMFLOAT4* postion;	// Ŀ���� ��ġ�� ���� ���� ��ġ
	DirectX::XMFLOAT3* normal;	// Ŀ���� ��ġ�� ���� �븻
	unsigned int* objectID;		// ������Ʈ ID
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

	// ���� �ɼ��� ���� �������� �������ش�
	// ����� Imgui���� ���� �����̴�
	virtual void RenderOptionUpdate(IRenderOption option) abstract;

	// �ݴ�� �������� IRenderOption�� ��������� ���´�
	// �׷��� const Reference�� �ߴ�
	virtual IRenderOption GetRenderOption()abstract;
	virtual IRendererDebugInfo GetRenderDebugInfo() abstract;
	virtual void* GetDeferredInfo() abstract;

	virtual void CameraUpdate(
		const DirectX::XMMATRIX& worldTM,
		const DirectX::XMMATRIX& viewTM,
		const DirectX::XMMATRIX& projTM) abstract;

	virtual void CameraSkyBoxRender(const unsigned int textureIdx) abstract;


	// ����Ʈ�� ��ġ���� ���� ī�޶��� ��ġ���� ���ؼ� �� ����Ʈ ������ ���� ���� Boolean���� ��ȯ�Ѵ�
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

	// ��ŷ
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
		const unsigned int probeIndex,	// probe ����
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

	// �ִϸ��̼��� �������� Ȯ���ϱ� ���� bool�� �����Ѵ�
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

	// ���ڸ�� �ִϸ��̼��� ��쿡�� �����ٴ� ������ ����
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
		const unsigned int overrideAnimBase,	// �ִϸ��̼� �ϳ��� �ѷ� �ɰ���
		const float alpha,
		const DirectX::SimpleMath::Matrix& worldTM
	) abstract;

	virtual void EndAnimation(const unsigned int modelIndex, DirectX::XMMATRIX* pBoneOffset) abstract;

	// IMGUI �ʱ�ȭ�� ���� Device, DeviceContext ����
	// �ٸ�, Engine�� DX�� ���� �ʱ� ���� �ּ����� ��ħ���� void*�� ����
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