#pragma once

/// 2022.03.25
/// ���� �������� �� ģ������ ��Ƶδ� Ŭ����
/// ����
/// 1. ���� ���ҽ��� ����ϴ� �����͸� �����Ͽ� ���ҽ� ��� ������带 ���δ�
/// 2. Frustum Culling�� ���� �׸� �ʿ䰡 ���� ���ҽ��� �����Ѵ�
/// 3. Opaque�� Transparent�� ���ҽ��� �����Ѵ�


#include "D3D11Define.h"
#include <queue>

class Renderer;
struct NodeData;
struct SpriteData;

const unsigned int MAX_RENDER_NODEDATA = 10000;

// ���� ���� �׷��� NodeData�� ������ŭ �ʿ��ϴ�
struct RenderQueueElement
{
	unsigned int renderQueueIndex;

	class VertexShader* pVertexShader;
	class PixelShader* pPixelShader;
	unsigned int reflectionProbe;
	unsigned int objectID;			// ��ŷ ������

	int modelMeshIndex;
	bool isTransparent;					// ���� �޽��� Opaque�� �ٸ��� ���� �־������

	std::shared_ptr<NodeData> pNodeData;
	DirectX::XMMATRIX WorldTM;
	DirectX::XMMATRIX* pBoneOffsetTM;

	RenderQueueElement* pNext;
	RenderQueueElement* pPrev;		// ȯ�� ��ũ�� ����Ʈ�� �����

	bool isUsing;
};

// ����Ʈ�� ��쿡�� ������ ����θ� ���� ���̴�
// + �����常 ���ȴ�
// + ���÷��� ���κ긦 ������� �ʰ�, ��� ����Ʈ�� Transparent�ϴ�
// ���� �̹����� ����ϴ� ����Ʈ�� ���ÿ� Ȱ���ϱ� ���� ��ũ�� ����Ʈ�� �����
struct RenderQueueParticleElement
{
	unsigned int renderQueueIndex;

	class VertexShader* pVertexShader;
	class PixelShader* pPixelShader;
	unsigned int spriteIndex;

	DirectX::XMMATRIX WorldTM;

	RenderQueueParticleElement* pNext;
	RenderQueueParticleElement* pPrev;
};

struct InstancedData
{
	DirectX::XMMATRIX world;
	//DirectX::XMFLOAT4 f1;
	//DirectX::XMFLOAT4 f2;
	//DirectX::XMFLOAT4 f3;
	//DirectX::XMFLOAT4 f4;
};

class RenderQueue
{
public:
	RenderQueue();
	~RenderQueue();

	void Initialize(Microsoft::WRL::ComPtr<ID3D11Device> pDevice);

	void AddRenderQueue(
		class VertexShader* pVertexShader,
		class PixelShader* pPixelShader,
		unsigned int reflectionProbe,
		unsigned int objectID,
		int modelIndex,
		bool isOpaque,
		std::shared_ptr<NodeData> pData,
		const DirectX::XMMATRIX& pWorldTM,
		DirectX::XMMATRIX* pBoneOFfsetTM);

	void AddRenderQueueParticle(
		class VertexShader* pVertexShader,
		class PixelShader* pPixelShader,
		unsigned int spriteIndex,
		std::shared_ptr<SpriteData> pSpriteData,
		const DirectX::XMMATRIX& pWorldTM);


	void CopyOutLineMesh(
		class VertexShader* pVertexShader,
		class PixelShader* pPixelShader,
		int modelIndex,
		std::shared_ptr<NodeData> pData, 
		const DirectX::XMMATRIX& worldTM,
		DirectX::XMMATRIX* pBoneOffsetTM);

	void ProcessOpaqueQueue(Renderer* pRenderer);
	void ProcessQueueByInstance(Renderer* pRenderer);
	void ProcessTransparentQueue(Renderer* pRenderer);
	void ProcessParticleQueue(Renderer* pRenderer);
	void ProcessDebugQueue(Renderer* pRenderer);
	void ProcessOutLineQueue(Renderer* pRenderer, bool isQT);

	void GetObjectCount(unsigned int& __out opaque, unsigned int& __out transparent, unsigned int& __out particle);
	void Clear();
	
	void GetSelectedObjectTM(const DirectX::XMMATRIX& worldTM);
	const DirectX::XMMATRIX& GetSelectedObjectTM();

private:
	unsigned int m_OpaqueCount;
	unsigned int m_TransparentCount;

	// ����ť�� ������Ʈ Ǯ
	std::vector<RenderQueueElement*> m_RenderQueueObejctPool_V;
	std::vector<RenderQueueParticleElement*> m_RenderQueueParticleObejctPool_V;

	std::queue<RenderQueueElement*> m_OpaqueRenderQueue_Q;
	std::queue<RenderQueueElement*> m_TransparentRenderQueue_Q;
	std::queue<RenderQueueElement*> m_OutLineQueue_Q;

	std::queue<RenderQueueParticleElement*> m_ParticleRenderQueue_Q;

	Microsoft::WRL::ComPtr<ID3D11Buffer> m_InstancedBuffer;

	std::vector<InstancedData> m_InstancedData;


	// ���� �����ؾ��ϴ� �迭 ����
	// �̰� ������ �׻� ������� �ʰ� �ִ� ���� �˻��ؾ��Ѵ�
	unsigned int m_NowIndex;
	unsigned int m_SelectedObjectIndex;
	unsigned int m_NowParticleIndex;

	DirectX::XMMATRIX m_SelectedWorldTM;

};

